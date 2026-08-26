#include <array>
#include <cassert>
#include <cstddef>
#include <filesystem>
#include <optional>
#include <span>
#include <string>

#include "goreecloud/browser/advanced_download_runtime.hpp"

namespace {

class FakeHttpClient final : public goreecloud::browser::HttpDownloadClient {
 public:
  std::optional<goreecloud::browser::HttpDownloadResponseMetadata> inspect(
      const goreecloud::browser::HttpDownloadRequest&) override {
    return goreecloud::browser::HttpDownloadResponseMetadata{
        .status_code = 200,
        .content_length = 16,
        .accepts_byte_ranges = true,
        .etag = std::string{"runtime-etag"},
        .last_modified = std::nullopt,
        .mime_type = std::string{"application/octet-stream"},
    };
  }

  goreecloud::browser::HttpDownloadStreamResult stream(
      const goreecloud::browser::HttpDownloadRequest& request,
      ChunkCallback on_chunk) override {
    if (!request.range) return {false, false, 0, "range required"};
    const auto count = request.range->end_inclusive - request.range->begin + 1;
    std::array<std::byte, 16> bytes{};
    if (!on_chunk(std::span<const std::byte>(bytes.data(), static_cast<std::size_t>(count)))) {
      return {false, false, 0, "writer rejected"};
    }
    return {true, false, count, "ok"};
  }
};

}  // namespace

int main() {
  using namespace goreecloud::browser;

  const auto root = std::filesystem::temp_directory_path() / "goreecloud-browser-runtime-smoke";
  std::error_code error;
  std::filesystem::remove_all(root, error);

  FakeHttpClient client;
  AdvancedDownloadRuntimeService runtime(client, root);
  const auto queued = runtime.enqueue({
      .source_url = "https://example.test/runtime.bin",
      .referrer_url = "https://example.test/",
      .suggested_filename = std::string{"runtime.bin"},
      .private_session = false,
  });
  assert(queued.accepted);

  for (int i = 0; i < 40; ++i) {
    runtime.pump();
    const auto progress = runtime.progress(queued.download_id);
    if (progress && progress->state == DownloadState::completed) break;
  }

  const auto progress = runtime.progress(queued.download_id);
  assert(progress);
  assert(progress->state == DownloadState::completed);
  assert(progress->completed_bytes == 16);
  assert(progress->total_bytes == 16);
  assert(std::filesystem::exists(root / "runtime.bin"));
  assert(std::filesystem::file_size(root / "runtime.bin") == 16);

  std::filesystem::remove_all(root, error);
  return 0;
}
