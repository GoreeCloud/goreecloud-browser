#include <cassert>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include "goreecloud/browser/advanced_download_panel.hpp"
#include "goreecloud/browser/advanced_download_panel_controller.hpp"
#include "goreecloud/browser/advanced_download_runtime.hpp"

namespace {

class FakeHttpClient final : public goreecloud::browser::HttpDownloadClient {
 public:
  explicit FakeHttpClient(bool interrupt_first = false) : interrupt_first_(interrupt_first) {}
  std::optional<goreecloud::browser::HttpDownloadResponseMetadata> inspect(
      const goreecloud::browser::HttpDownloadRequest&) override {
    return goreecloud::browser::HttpDownloadResponseMetadata{
        .status_code = 200, .content_length = 160, .accepts_byte_ranges = true,
        .etag = std::string{"runtime-etag"}, .last_modified = std::nullopt,
        .mime_type = std::string{"application/octet-stream"}};
  }
  goreecloud::browser::HttpDownloadStreamResult stream(
      const goreecloud::browser::HttpDownloadRequest& request, ChunkCallback on_chunk) override {
    requests.push_back(request);
    if (!request.range) return {false, false, 0, "range required"};
    const auto count = request.range->end_inclusive - request.range->begin + 1;
    auto send = count; bool completed = true; bool retryable = false;
    if (interrupt_first_ && requests.size() == 1) { send = count / 2; completed = false; retryable = true; }
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
  std::error_code error; std::filesystem::remove_all(root, error);

  std::string interrupted_id;
  {
    FakeHttpClient client(true); AdvancedDownloadRuntimeService runtime(client, root);
    const auto queued = runtime.enqueue({.source_url="https://example.test/runtime.bin", .referrer_url="https://example.test/", .suggested_filename=std::string{"runtime.bin"}, .private_session=false});
    assert(queued.accepted); interrupted_id = queued.download_id; runtime.pump();
    const auto p = runtime.progress(interrupted_id); assert(p && p->completed_bytes == 5 && p->state == DownloadState::running);
  }

  FakeHttpClient resumed_client(false); AdvancedDownloadRuntimeService resumed(resumed_client, root);
  assert(resumed.progress(interrupted_id) && resumed.progress(interrupted_id)->completed_bytes == 5);
  resumed.pump(); assert(!resumed_client.requests.empty() && resumed_client.requests.front().range && resumed_client.requests.front().range->begin == 5);
  for (int i=0;i<40;++i) { resumed.pump(); auto p=resumed.progress(interrupted_id); if (p && p->state==DownloadState::completed) break; }
  const auto done = resumed.progress(interrupted_id); assert(done && done->state==DownloadState::completed && done->completed_bytes==160);
  assert(std::filesystem::exists(root / "runtime.bin"));

  AdvancedDownloadPanelController panel(resumed);
  panel.set_filter(DownloadPanelFilter::completed); panel.set_sort(DownloadPanelSort::filename);
  auto model = panel.model(); assert(model.completed_count == 1 && model.rows.size() == 1 && model.rows.front().can_open);
  const auto open = panel.invoke(DownloadPanelAction::open, interrupted_id);
  assert(open.accepted && open.open_path && *open.open_path == root / "runtime.bin");
  const auto restarted = panel.invoke(DownloadPanelAction::restart, interrupted_id);
  assert(restarted.accepted); assert(resumed.progress(interrupted_id)->state == DownloadState::queued);

  const auto controls_root = root / "controls";
  FakeHttpClient control_client(true); AdvancedDownloadRuntimeService control(control_client, controls_root);
  const auto cq = control.enqueue({.source_url="https://example.test/paused.bin", .referrer_url="https://example.test/", .suggested_filename=std::string{"paused.bin"}, .private_session=false});
  assert(cq.accepted); control.pump();
  AdvancedDownloadPanelController control_panel(control);
  assert(control_panel.invoke(DownloadPanelAction::pause, cq.download_id).accepted);
  const auto before = control_client.requests.size(); for (int i=0;i<4;++i) control.pump(); assert(control_client.requests.size()==before);
  assert(control_panel.invoke(DownloadPanelAction::resume, cq.download_id).accepted); control.pump(); assert(control_client.requests.size()>before);

  const auto cancel_root = root / "cancel";
  FakeHttpClient cancel_client(true); AdvancedDownloadRuntimeService cancel_runtime(cancel_client, cancel_root);
  const auto cancel_q = cancel_runtime.enqueue({.source_url="https://example.test/cancel.bin", .referrer_url="https://example.test/", .suggested_filename=std::string{"cancel.bin"}, .private_session=false});
  assert(cancel_q.accepted); cancel_runtime.pump(); AdvancedDownloadPanelController cancel_panel(cancel_runtime);
  assert(cancel_panel.invoke(DownloadPanelAction::cancel_and_discard, cancel_q.download_id).accepted);
  assert(cancel_runtime.progress(cancel_q.download_id)->state == DownloadState::cancelled);

  const auto schedule_root = root / "schedule";
  FakeHttpClient schedule_client(false); AdvancedDownloadRuntimeService schedule(schedule_client, schedule_root);
  const auto future = schedule.enqueue({.source_url="https://example.test/future.bin", .referrer_url="https://example.test/", .suggested_filename=std::string{"future.bin"}, .private_session=false, .scheduled_start_unix_seconds=unix_now_seconds()+3600});
  assert(future.accepted); for (int i=0;i<4;++i) schedule.pump(); assert(schedule_client.requests.empty());
  const auto elapsed = schedule.enqueue({.source_url="https://example.test/elapsed.bin", .referrer_url="https://example.test/", .suggested_filename=std::string{"elapsed.bin"}, .private_session=false, .scheduled_start_unix_seconds=unix_now_seconds()-1});
  assert(elapsed.accepted); schedule.pump(); assert(!schedule_client.requests.empty());

  std::string private_id;
  {
    FakeHttpClient pc(true); AdvancedDownloadRuntimeService pr(pc, root);
    const auto q=pr.enqueue({.source_url="https://example.test/private.bin", .referrer_url="https://example.test/private", .suggested_filename=std::string{"private.bin"}, .private_session=true});
    assert(q.accepted); private_id=q.download_id; pr.pump();
  }
  FakeHttpClient privacy_client(false); AdvancedDownloadRuntimeService privacy(privacy_client, root); assert(!privacy.progress(private_id));

  std::filesystem::remove_all(root, error); return 0;
}
