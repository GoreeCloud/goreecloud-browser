#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <span>
#include <string>
#include <string_view>

#include "goreecloud/browser/advanced_download_transfer_engine.hpp"

namespace goreecloud::browser {

struct HttpDownloadRequest {
  std::string url;
  std::string referrer;
  std::optional<DownloadByteRange> range;
  std::optional<std::string> if_range_etag;
  std::optional<std::string> if_range_last_modified;
};

struct HttpDownloadResponseMetadata {
  int status_code{0};
  std::optional<std::uint64_t> content_length;
  bool accepts_byte_ranges{false};
  std::optional<std::string> etag;
  std::optional<std::string> last_modified;
  std::optional<std::string> mime_type;
};

struct HttpDownloadStreamResult {
  bool completed{false};
  bool retryable{false};
  std::uint64_t body_bytes{0};
  std::string message;
};

class HttpDownloadClient {
 public:
  using ChunkCallback = std::function<bool(std::span<const std::byte>)>;
  virtual ~HttpDownloadClient() = default;
  virtual std::optional<HttpDownloadResponseMetadata> inspect(
      const HttpDownloadRequest& request) = 0;
  virtual HttpDownloadStreamResult stream(const HttpDownloadRequest& request,
                                          ChunkCallback on_chunk) = 0;
};

class HttpDownloadTransport final : public DownloadTransport {
 public:
  using Writer = std::function<bool(std::string_view download_id,
                                    std::uint64_t absolute_offset,
                                    std::span<const std::byte> data)>;

  HttpDownloadTransport(HttpDownloadClient& client, Writer writer)
      : client_(client), writer_(std::move(writer)) {}

  std::optional<DownloadResourceMetadata> inspect(
      std::string_view source_url,
      std::string_view referrer_url) override {
    HttpDownloadRequest request;
    request.url = std::string{source_url};
    request.referrer = std::string{referrer_url};
    const auto response = client_.inspect(request);
    if (!response || !response->content_length || *response->content_length == 0) {
      return std::nullopt;
    }
    return DownloadResourceMetadata{
        .total_bytes = *response->content_length,
        .accepts_byte_ranges = response->accepts_byte_ranges,
        .resumable = response->accepts_byte_ranges,
        .etag = response->etag,
        .last_modified = response->last_modified,
        .mime_type = response->mime_type,
    };
  }

  DownloadTransportResult transfer(const DownloadTransportRequest& request) override {
    if (!writer_) return {false, false, 0, "Download file writer is unavailable."};

    HttpDownloadRequest http;
    http.url = request.source_url;
    http.referrer = request.referrer_url;
    const auto absolute_begin = request.range.begin + request.resume_offset;
    http.range = DownloadByteRange{absolute_begin, request.range.end_inclusive};
    http.if_range_etag = request.etag;
    http.if_range_last_modified = request.last_modified;

    std::uint64_t written = 0;
    bool write_failed = false;
    const auto streamed = client_.stream(
        http,
        [&](std::span<const std::byte> chunk) {
          if (!writer_(request.download_id, absolute_begin + written, chunk)) {
            write_failed = true;
            return false;
          }
          written += static_cast<std::uint64_t>(chunk.size());
          return true;
        });

    if (write_failed) {
      return {false, false, written, "Failed to persist a downloaded byte range."};
    }
    return {streamed.completed,
            streamed.retryable,
            written,
            streamed.message};
  }

 private:
  HttpDownloadClient& client_;
  Writer writer_;
};

}  // namespace goreecloud::browser
