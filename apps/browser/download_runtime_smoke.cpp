#include <cassert>
#include <cstddef>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include "goreecloud/browser/advanced_download_runtime.hpp"

namespace {

class FakeHttpClient final : public goreecloud::browser::HttpDownloadClient {
 public:
  explicit FakeHttpClient(bool interrupt_first = false)
      : interrupt_first_(interrupt_first) {}

  std::optional<goreecloud::browser::HttpDownloadResponseMetadata> inspect(
      const goreecloud::browser::HttpDownloadRequest&) override {
    return goreecloud::browser::HttpDownloadResponseMetadata{
        .status_code = 200,
        .content_length = 160,
        .accepts_byte_ranges = true,
        .etag = std::string{"runtime-etag"},
        .last_modified = std::nullopt,
        .mime_type = std::string{"application/octet-stream"},
    };
  }

  goreecloud::browser::HttpDownloadStreamResult stream(
      const goreecloud::browser::HttpDownloadRequest& request,
      ChunkCallback on_chunk) override {
    requests.push_back(request);
    if (!request.range) return {false, false, 0, "range required"};
    const auto count = request.range->end_inclusive - request.range->begin + 1;
    auto send = count;
    bool completed = true;
    bool retryable = false;
    if (interrupt_first_ && requests.size() == 1) {
      send = count / 2;
      completed = false;
      retryable = true;
    }
    std::vector<std::byte> bytes(static_cast<std::size_t>(send));
    if (!bytes.empty() && !on_chunk(bytes)) return {false, false, 0, "writer rejected"};
    return {completed, retryable, send, completed ? "ok" : "simulated interruption"};
  }

  std::vector<goreecloud::browser::HttpDownloadRequest> requests;

 private:
  bool interrupt_first_{false};
};

}  // namespace

int main() {
  using namespace goreecloud::browser;

  const auto root = std::filesystem::temp_directory_path() / "goreecloud-browser-runtime-smoke";
  std::error_code error;
  std::filesystem::remove_all(root, error);

  std::string interrupted_id;
  {
    FakeHttpClient interrupted_client(true);
    AdvancedDownloadRuntimeService runtime(interrupted_client, root);
    const auto queued = runtime.enqueue({
        .source_url = "https://example.test/runtime.bin",
        .referrer_url = "https://example.test/",
        .suggested_filename = std::string{"runtime.bin"},
        .private_session = false,
    });
    assert(queued.accepted);
    interrupted_id = queued.download_id;
    runtime.pump();
    assert(!interrupted_client.requests.empty());
    const auto progress = runtime.progress(interrupted_id);
    assert(progress);
    assert(progress->completed_bytes == 5);
    assert(progress->state == DownloadState::running);
  }

  FakeHttpClient resumed_client(false);
  AdvancedDownloadRuntimeService resumed_runtime(resumed_client, root);
  const auto restored_progress = resumed_runtime.progress(interrupted_id);
  assert(restored_progress);
  assert(restored_progress->completed_bytes == 5);

  resumed_runtime.pump();
  assert(!resumed_client.requests.empty());
  assert(resumed_client.requests.front().range);
  assert(resumed_client.requests.front().range->begin == 5);

  for (int i = 0; i < 40; ++i) {
    resumed_runtime.pump();
    const auto progress = resumed_runtime.progress(interrupted_id);
    if (progress && progress->state == DownloadState::completed) break;
  }

  const auto progress = resumed_runtime.progress(interrupted_id);
  assert(progress);
  assert(progress->state == DownloadState::completed);
  assert(progress->completed_bytes == 160);
  assert(progress->total_bytes == 160);
  assert(std::filesystem::exists(root / "runtime.bin"));
  assert(std::filesystem::file_size(root / "runtime.bin") == 160);

  // Private-session queue records must not be reconstructed after restart.
  std::string private_id;
  {
    FakeHttpClient private_client(true);
    AdvancedDownloadRuntimeService private_runtime(private_client, root);
    const auto queued = private_runtime.enqueue({
        .source_url = "https://example.test/private.bin",
        .referrer_url = "https://example.test/private",
        .suggested_filename = std::string{"private.bin"},
        .private_session = true,
    });
    assert(queued.accepted);
    private_id = queued.download_id;
    private_runtime.pump();
  }
  FakeHttpClient privacy_check_client(false);
  AdvancedDownloadRuntimeService privacy_check_runtime(privacy_check_client, root);
  assert(!privacy_check_runtime.progress(private_id));

  std::filesystem::remove_all(root, error);
  return 0;
}
