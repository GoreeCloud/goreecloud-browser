#include "goreecloud/browser/curl_http_download_client.hpp"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

#include <curl/curl.h>

namespace goreecloud::browser {
namespace {

struct CurlHandleDeleter {
  void operator()(CURL* handle) const noexcept {
    if (handle) curl_easy_cleanup(handle);
  }
};
using CurlHandle = std::unique_ptr<CURL, CurlHandleDeleter>;

struct HeaderCapture {
  std::optional<std::string> etag;
  std::optional<std::string> last_modified;
  bool accepts_ranges{false};
};

std::string trim(std::string_view value) {
  std::size_t begin = 0;
  while (begin < value.size() && std::isspace(static_cast<unsigned char>(value[begin]))) ++begin;
  std::size_t end = value.size();
  while (end > begin && std::isspace(static_cast<unsigned char>(value[end - 1]))) --end;
  return std::string{value.substr(begin, end - begin)};
}

bool starts_with_case_insensitive(std::string_view value, std::string_view prefix) {
  if (value.size() < prefix.size()) return false;
  for (std::size_t i = 0; i < prefix.size(); ++i) {
    if (std::tolower(static_cast<unsigned char>(value[i])) !=
        std::tolower(static_cast<unsigned char>(prefix[i]))) return false;
  }
  return true;
}

size_t header_callback(char* buffer, size_t size, size_t count, void* userdata) {
  const auto total = size * count;
  auto* capture = static_cast<HeaderCapture*>(userdata);
  if (!capture || total == 0) return total;
  const std::string_view line(buffer, total);
  if (starts_with_case_insensitive(line, "ETag:")) {
    capture->etag = trim(line.substr(5));
  } else if (starts_with_case_insensitive(line, "Last-Modified:")) {
    capture->last_modified = trim(line.substr(14));
  } else if (starts_with_case_insensitive(line, "Accept-Ranges:")) {
    const auto value = trim(line.substr(14));
    capture->accepts_ranges = value == "bytes" || value == "Bytes";
  }
  return total;
}

struct StreamContext {
  HttpDownloadClient::ChunkCallback* callback{nullptr};
  std::uint64_t body_bytes{0};
  bool callback_rejected{false};
};

size_t write_callback(char* buffer, size_t size, size_t count, void* userdata) {
  const auto total = size * count;
  auto* context = static_cast<StreamContext*>(userdata);
  if (!context || !context->callback) return 0;
  const auto* bytes = reinterpret_cast<const std::byte*>(buffer);
  if (!(*context->callback)(std::span<const std::byte>(bytes, total))) {
    context->callback_rejected = true;
    return 0;
  }
  context->body_bytes += static_cast<std::uint64_t>(total);
  return total;
}

void apply_common_options(CURL* handle, const HttpDownloadRequest& request) {
  curl_easy_setopt(handle, CURLOPT_URL, request.url.c_str());
  curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(handle, CURLOPT_FAILONERROR, 0L);
  curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1L);
  curl_easy_setopt(handle, CURLOPT_USERAGENT, "GoreeCloudBrowser/0.1-beta");
  if (!request.referrer.empty()) {
    curl_easy_setopt(handle, CURLOPT_REFERER, request.referrer.c_str());
  }
}

std::string range_string(const DownloadByteRange& range) {
  return std::to_string(range.begin) + "-" + std::to_string(range.end_inclusive);
}

curl_slist* build_if_range_headers(const HttpDownloadRequest& request) {
  curl_slist* headers = nullptr;
  if (request.if_range_etag) {
    const auto header = "If-Range: " + *request.if_range_etag;
    headers = curl_slist_append(headers, header.c_str());
  } else if (request.if_range_last_modified) {
    const auto header = "If-Range: " + *request.if_range_last_modified;
    headers = curl_slist_append(headers, header.c_str());
  }
  return headers;
}

bool retryable_curl_code(CURLcode code) {
  switch (code) {
    case CURLE_COULDNT_RESOLVE_HOST:
    case CURLE_COULDNT_CONNECT:
    case CURLE_OPERATION_TIMEDOUT:
    case CURLE_RECV_ERROR:
    case CURLE_SEND_ERROR:
    case CURLE_PARTIAL_FILE:
      return true;
    default:
      return false;
  }
}

}  // namespace

CurlHttpDownloadClient::CurlHttpDownloadClient() {
  curl_global_init(CURL_GLOBAL_DEFAULT);
}

CurlHttpDownloadClient::~CurlHttpDownloadClient() {
  curl_global_cleanup();
}

std::optional<HttpDownloadResponseMetadata> CurlHttpDownloadClient::inspect(
    const HttpDownloadRequest& request) {
  CurlHandle handle(curl_easy_init());
  if (!handle) return std::nullopt;

  HeaderCapture headers;
  apply_common_options(handle.get(), request);
  curl_easy_setopt(handle.get(), CURLOPT_NOBODY, 1L);
  curl_easy_setopt(handle.get(), CURLOPT_HEADERFUNCTION, header_callback);
  curl_easy_setopt(handle.get(), CURLOPT_HEADERDATA, &headers);

  const auto code = curl_easy_perform(handle.get());
  if (code != CURLE_OK) return std::nullopt;

  long status = 0;
  curl_off_t content_length = -1;
  char* content_type = nullptr;
  curl_easy_getinfo(handle.get(), CURLINFO_RESPONSE_CODE, &status);
  curl_easy_getinfo(handle.get(), CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &content_length);
  curl_easy_getinfo(handle.get(), CURLINFO_CONTENT_TYPE, &content_type);
  if (status < 200 || status >= 400 || content_length <= 0) return std::nullopt;

  HttpDownloadResponseMetadata result;
  result.status_code = static_cast<int>(status);
  result.content_length = static_cast<std::uint64_t>(content_length);
  result.accepts_byte_ranges = headers.accepts_ranges;
  result.etag = headers.etag;
  result.last_modified = headers.last_modified;
  if (content_type) result.mime_type = std::string{content_type};
  return result;
}

HttpDownloadStreamResult CurlHttpDownloadClient::stream(
    const HttpDownloadRequest& request,
    ChunkCallback on_chunk) {
  CurlHandle handle(curl_easy_init());
  if (!handle) return {false, false, 0, "libcurl handle creation failed."};

  apply_common_options(handle.get(), request);

  std::string range;
  if (request.range) {
    range = range_string(*request.range);
    curl_easy_setopt(handle.get(), CURLOPT_RANGE, range.c_str());
  }

  curl_slist* raw_headers = build_if_range_headers(request);
  struct HeaderListGuard {
    curl_slist* value{nullptr};
    ~HeaderListGuard() { if (value) curl_slist_free_all(value); }
  } header_guard{raw_headers};
  if (raw_headers) curl_easy_setopt(handle.get(), CURLOPT_HTTPHEADER, raw_headers);

  StreamContext context{&on_chunk, 0, false};
  curl_easy_setopt(handle.get(), CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(handle.get(), CURLOPT_WRITEDATA, &context);

  const auto code = curl_easy_perform(handle.get());
  long status = 0;
  curl_easy_getinfo(handle.get(), CURLINFO_RESPONSE_CODE, &status);

  if (context.callback_rejected) {
    return {false, false, context.body_bytes, "Download file writer rejected streamed bytes."};
  }
  if (code != CURLE_OK) {
    return {false, retryable_curl_code(code), context.body_bytes, curl_easy_strerror(code)};
  }

  const bool status_ok = status >= 200 && status < 300;
  if (request.range && status != 206) {
    return {false, false, context.body_bytes,
            "Server did not honor the requested byte range."};
  }
  if (!status_ok) {
    const bool retryable = status == 408 || status == 429 || status >= 500;
    return {false, retryable, context.body_bytes,
            "HTTP transfer returned status " + std::to_string(status) + "."};
  }
  return {true, false, context.body_bytes, "HTTP byte range completed."};
}

}  // namespace goreecloud::browser
