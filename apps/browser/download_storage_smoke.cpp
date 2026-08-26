#include <cassert>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include "goreecloud/browser/download_file_store.hpp"
#include "goreecloud/browser/download_integrity.hpp"
#include "goreecloud/browser/http_download_transport.hpp"

namespace {

class FakeHttpClient final : public goreecloud::browser::HttpDownloadClient {
 public:
  std::optional<goreecloud::browser::HttpDownloadResponseMetadata> inspect(
      const goreecloud::browser::HttpDownloadRequest&) override {
    return goreecloud::browser::HttpDownloadResponseMetadata{
        .status_code = 200,
        .content_length = std::uint64_t{8},
        .accepts_byte_ranges = true,
        .etag = std::string{"etag-storage-test"},
        .last_modified = std::string{"Wed, 26 Aug 2026 20:55:00 GMT"},
        .mime_type = std::string{"application/octet-stream"},
    };
  }

  goreecloud::browser::HttpDownloadStreamResult stream(
      const goreecloud::browser::HttpDownloadRequest& request,
      ChunkCallback on_chunk) override {
    last_request = request;
    const std::vector<std::byte> bytes{
        std::byte{'G'}, std::byte{'O'}, std::byte{'R'}, std::byte{'E'},
        std::byte{'E'}, std::byte{'C'}, std::byte{'L'}, std::byte{'D'}};

    if (!request.range) return {false, false, 0, "range required"};
    const auto begin = static_cast<std::size_t>(request.range->begin);
    const auto end = static_cast<std::size_t>(request.range->end_inclusive);
    if (begin > end || end >= bytes.size()) return {false, false, 0, "invalid range"};
    const std::span<const std::byte> chunk(bytes.data() + begin, end - begin + 1);
    if (!on_chunk(chunk)) return {false, false, 0, "writer rejected chunk"};
    return {true, false, static_cast<std::uint64_t>(chunk.size()), "ok"};
  }

  goreecloud::browser::HttpDownloadRequest last_request;
};

}  // namespace

int main() {
  using namespace goreecloud::browser;

  const auto root = std::filesystem::temp_directory_path() / "goreecloud-browser-download-smoke";
  std::error_code error;
  std::filesystem::remove_all(root, error);

  LocalDownloadFileStore store(root);
  const auto paths = store.prepare("download-1", "media.bin", 8);
  assert(paths);
  assert(std::filesystem::exists(paths->partial_path));
  assert(!std::filesystem::exists(paths->final_path));

  FakeHttpClient client;
  HttpDownloadTransport transport(
      client,
      [&](std::string_view id, std::uint64_t offset, std::span<const std::byte> data) {
        assert(id == "download-1");
        return store.write_at(*paths, offset, data);
      });

  const auto metadata = transport.inspect("https://example.test/media.bin",
                                          "https://example.test/");
  assert(metadata);
  assert(metadata->total_bytes == 8);
  assert(metadata->accepts_byte_ranges);
  assert(metadata->etag && *metadata->etag == "etag-storage-test");

  DownloadTransportRequest request;
  request.download_id = "download-1";
  request.source_url = "https://example.test/media.bin";
  request.referrer_url = "https://example.test/";
  request.range = {0, 7};
  request.resume_offset = 0;
  request.etag = metadata->etag;
  request.last_modified = metadata->last_modified;

  const auto transferred = transport.transfer(request);
  assert(transferred.completed);
  assert(transferred.transferred_bytes == 8);
  assert(client.last_request.range);
  assert(client.last_request.range->begin == 0);
  assert(client.last_request.range->end_inclusive == 7);
  assert(client.last_request.if_range_etag == metadata->etag);

  const auto partial_size = store.partial_size(*paths);
  assert(partial_size && *partial_size == 8);

  SizeOnlyDownloadIntegrityVerifier verifier;
  const auto integrity = verifier.verify(
      paths->partial_path,
      DownloadIntegrityExpectation{.expected_size = std::uint64_t{8},
                                   .expected_sha256 = std::nullopt});
  assert(integrity.verified);

  const auto committed = store.commit(*paths, 8);
  assert(committed.committed);
  assert(committed.final_size == 8);
  assert(!std::filesystem::exists(paths->partial_path));
  assert(std::filesystem::exists(paths->final_path));

  const auto bad = store.prepare("download-2", "bad.bin", 4);
  assert(bad);
  const auto rejected = store.commit(*bad, 5);
  assert(!rejected.committed);
  assert(std::filesystem::exists(bad->partial_path));
  assert(store.discard(*bad));

  std::filesystem::remove_all(root, error);
  return 0;
}
