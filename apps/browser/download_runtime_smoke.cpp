#include <cassert>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include "goreecloud/browser/advanced_download_panel.hpp"
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

std::int64_t unix_now_seconds() {
  return std::chrono::duration_cast<std::chrono::seconds>(
      std::chrono::system_clock::now().time_since_epoch()).count();
}

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

  const auto completed_panel = AdvancedDownloadPanelBuilder::build(
      resumed_runtime, DownloadPanelFilter::completed);
  assert(completed_panel.completed_count == 1);
  assert(completed_panel.rows.size() == 1);
  assert(completed_panel.rows.front().can_open);
  assert(!completed_panel.rows.front().can_pause);

  const auto controls_root = root / "controls";
  std::string paused_id;
  {
    FakeHttpClient control_client(true);
    AdvancedDownloadRuntimeService control_runtime(control_client, controls_root);
    const auto queued = control_runtime.enqueue({
        .source_url = "https://example.test/paused.bin",
        .referrer_url = "https://example.test/",
        .suggested_filename = std::string{"paused.bin"},
        .private_session = false,
    });
    assert(queued.accepted);
    paused_id = queued.download_id;
    control_runtime.pump();
    const auto requests_before_pause = control_client.requests.size();
    assert(requests_before_pause == 1);
    assert(control_runtime.pause(paused_id));
    for (int i = 0; i < 4; ++i) control_runtime.pump();
    assert(control_client.requests.size() == requests_before_pause);
    const auto paused_progress = control_runtime.progress(paused_id);
    assert(paused_progress && paused_progress->state == DownloadState::paused);
    const auto paused_panel = AdvancedDownloadPanelBuilder::build(
        control_runtime, DownloadPanelFilter::paused);
    assert(paused_panel.paused_count == 1);
    assert(paused_panel.rows.size() == 1);
    assert(paused_panel.rows.front().can_resume);
  }

  FakeHttpClient paused_restore_client(false);
  AdvancedDownloadRuntimeService paused_restore(paused_restore_client, controls_root);
  const auto restored_paused = paused_restore.progress(paused_id);
  assert(restored_paused && restored_paused->state == DownloadState::paused);
  paused_restore.pump();
  assert(paused_restore_client.requests.empty());
  assert(paused_restore.resume(paused_id));
  paused_restore.pump();
  assert(!paused_restore_client.requests.empty());

  const auto cancel_root = root / "cancel";
  FakeHttpClient cancel_client(true);
  AdvancedDownloadRuntimeService cancel_runtime(cancel_client, cancel_root);
  const auto cancel_queued = cancel_runtime.enqueue({
      .source_url = "https://example.test/cancel.bin",
      .referrer_url = "https://example.test/",
      .suggested_filename = std::string{"cancel.bin"},
      .private_session = false,
  });
  assert(cancel_queued.accepted);
  cancel_runtime.pump();
  const auto requests_before_cancel = cancel_client.requests.size();
  assert(cancel_runtime.cancel(cancel_queued.download_id, true));
  for (int i = 0; i < 4; ++i) cancel_runtime.pump();
  assert(cancel_client.requests.size() == requests_before_cancel);
  const auto cancelled = cancel_runtime.progress(cancel_queued.download_id);
  assert(cancelled && cancelled->state == DownloadState::cancelled);

  // Future schedules do not inspect or transfer until released; elapsed schedules start normally.
  const auto schedule_root = root / "schedule";
  FakeHttpClient schedule_client(false);
  AdvancedDownloadRuntimeService schedule_runtime(schedule_client, schedule_root);
  const auto future = schedule_runtime.enqueue({
      .source_url = "https://example.test/future.bin",
      .referrer_url = "https://example.test/",
      .suggested_filename = std::string{"future.bin"},
      .private_session = false,
      .scheduled_start_unix_seconds = unix_now_seconds() + 3600,
  });
  assert(future.accepted);
  for (int i = 0; i < 4; ++i) schedule_runtime.pump();
  assert(schedule_client.requests.empty());
  const auto future_progress = schedule_runtime.progress(future.download_id);
  assert(future_progress && future_progress->message == "Scheduled");

  const auto elapsed = schedule_runtime.enqueue({
      .source_url = "https://example.test/elapsed.bin",
      .referrer_url = "https://example.test/",
      .suggested_filename = std::string{"elapsed.bin"},
      .private_session = false,
      .scheduled_start_unix_seconds = unix_now_seconds() - 1,
  });
  assert(elapsed.accepted);
  schedule_runtime.pump();
  assert(!schedule_client.requests.empty());

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
