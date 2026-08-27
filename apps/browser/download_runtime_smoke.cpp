#include <cassert>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <fstream>
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

enum class FakeScanMode { clean, suspicious, malicious, unavailable };

class FakeWardveilScanner final : public goreecloud::browser::WardveilDownloadScanner {
 public:
  explicit FakeWardveilScanner(FakeScanMode mode = FakeScanMode::clean) : mode_(mode) {}

  goreecloud::browser::WardveilDownloadScanResponse scan(
      const goreecloud::browser::WardveilDownloadScanRequest& request,
      const std::filesystem::path& staged_file) override {
    ++scan_count;
    assert(std::filesystem::exists(staged_file));
    if (mode_ == FakeScanMode::unavailable) return {false, std::nullopt, "offline"};
    const auto now = goreecloud::browser::wardveil_unix_now_seconds();
    auto result = goreecloud::browser::WardveilScanResult::clean;
    if (mode_ == FakeScanMode::suspicious) result = goreecloud::browser::WardveilScanResult::suspicious;
    if (mode_ == FakeScanMode::malicious) result = goreecloud::browser::WardveilScanResult::malicious;
    return {true,
            goreecloud::browser::WardveilDownloadScanRecord{
                .contract_version = std::string{goreecloud::browser::kWardveilDownloadContractVersion},
                .record_type = "scan_finding",
                .authoritative_producer = true,
                .producer_id = "wardveil-scan",
                .resource_type = std::string{goreecloud::browser::kWardveilDownloadResourceType},
                .resource_id = request.resource_id,
                .resource_digest_sha256 = request.resource_digest_sha256,
                .result = result,
                .observed_at_unix_seconds = now - 1,
                .valid_until_unix_seconds = now + 300,
                .evidence_refs = {"evidence:runtime"},
            },
            "ok"};
  }

  int scan_count{0};

 private:
  FakeScanMode mode_;
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
  std::filesystem::create_directories(root, error);
  const auto sha_sample = root / "sha256-abc.bin";
  { std::ofstream out(sha_sample, std::ios::binary); out << "abc"; }
  const auto sha_sample_digest = sha256_file(sha_sample);
  assert(sha_sample_digest && *sha_sample_digest == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
  std::filesystem::remove(sha_sample, error);
  FakeWardveilScanner clean_scanner;

  std::string interrupted_id;
  {
    FakeHttpClient client(true); AdvancedDownloadRuntimeService runtime(client, root, &clean_scanner);
    const auto queued = runtime.enqueue({.source_url="https://example.test/runtime.bin", .referrer_url="https://example.test/", .suggested_filename=std::string{"runtime.bin"}, .private_session=false});
    assert(queued.accepted); interrupted_id = queued.download_id; runtime.pump();
    const auto p = runtime.progress(interrupted_id); assert(p && p->completed_bytes == 5 && p->state == DownloadState::running);
  }

  FakeHttpClient resumed_client(false); AdvancedDownloadRuntimeService resumed(resumed_client, root, &clean_scanner);
  assert(resumed.progress(interrupted_id) && resumed.progress(interrupted_id)->completed_bytes == 5);
  resumed.pump(); assert(!resumed_client.requests.empty() && resumed_client.requests.front().range && resumed_client.requests.front().range->begin == 5);
  for (int i=0;i<40;++i) { resumed.pump(); auto p=resumed.progress(interrupted_id); if (p && p->state==DownloadState::completed) break; }
  const auto done = resumed.progress(interrupted_id); assert(done && done->state==DownloadState::completed && done->completed_bytes==160);
  assert(clean_scanner.scan_count > 0);
  assert(std::filesystem::exists(root / "runtime.bin"));
  const auto clean_decision = resumed.security_decision(interrupted_id);
  assert(clean_decision && clean_decision->can_release && clean_decision->can_open);

  AdvancedDownloadPanelController panel(resumed);
  panel.set_filter(DownloadPanelFilter::completed); panel.set_sort(DownloadPanelSort::filename);
  auto model = panel.model(); assert(model.completed_count == 1 && model.rows.size() == 1 && model.rows.front().can_open);
  const auto open = panel.invoke(DownloadPanelAction::open, interrupted_id);
  assert(open.accepted && open.open_path && *open.open_path == root / "runtime.bin");
  const auto restarted = panel.invoke(DownloadPanelAction::restart, interrupted_id);
  assert(restarted.accepted); assert(resumed.progress(interrupted_id)->state == DownloadState::queued);

  const auto controls_root = root / "controls";
  FakeHttpClient control_client(true); AdvancedDownloadRuntimeService control(control_client, controls_root, &clean_scanner);
  const auto cq = control.enqueue({.source_url="https://example.test/paused.bin", .referrer_url="https://example.test/", .suggested_filename=std::string{"paused.bin"}, .private_session=false});
  assert(cq.accepted); control.pump();
  AdvancedDownloadPanelController control_panel(control);
  assert(control_panel.invoke(DownloadPanelAction::pause, cq.download_id).accepted);
  const auto before = control_client.requests.size(); for (int i=0;i<4;++i) control.pump(); assert(control_client.requests.size()==before);
  assert(control_panel.invoke(DownloadPanelAction::resume, cq.download_id).accepted); control.pump(); assert(control_client.requests.size()>before);

  const auto cancel_root = root / "cancel";
  FakeHttpClient cancel_client(true); AdvancedDownloadRuntimeService cancel_runtime(cancel_client, cancel_root, &clean_scanner);
  const auto cancel_q = cancel_runtime.enqueue({.source_url="https://example.test/cancel.bin", .referrer_url="https://example.test/", .suggested_filename=std::string{"cancel.bin"}, .private_session=false});
  assert(cancel_q.accepted); cancel_runtime.pump(); AdvancedDownloadPanelController cancel_panel(cancel_runtime);
  assert(cancel_panel.invoke(DownloadPanelAction::cancel_and_discard, cancel_q.download_id).accepted);
  assert(cancel_runtime.progress(cancel_q.download_id)->state == DownloadState::cancelled);

  const auto schedule_root = root / "schedule";
  FakeHttpClient schedule_client(false); AdvancedDownloadRuntimeService schedule(schedule_client, schedule_root, &clean_scanner);
  const auto future = schedule.enqueue({.source_url="https://example.test/future.bin", .referrer_url="https://example.test/", .suggested_filename=std::string{"future.bin"}, .private_session=false, .scheduled_start_unix_seconds=unix_now_seconds()+3600});
  assert(future.accepted); for (int i=0;i<4;++i) schedule.pump(); assert(schedule_client.requests.empty());
  const auto elapsed = schedule.enqueue({.source_url="https://example.test/elapsed.bin", .referrer_url="https://example.test/", .suggested_filename=std::string{"elapsed.bin"}, .private_session=false, .scheduled_start_unix_seconds=unix_now_seconds()-1});
  assert(elapsed.accepted); schedule.pump(); assert(!schedule_client.requests.empty());

  std::string private_id;
  {
    FakeHttpClient pc(true); AdvancedDownloadRuntimeService pr(pc, root, &clean_scanner);
    const auto q=pr.enqueue({.source_url="https://example.test/private.bin", .referrer_url="https://example.test/private", .suggested_filename=std::string{"private.bin"}, .private_session=true});
    assert(q.accepted); private_id=q.download_id; pr.pump();
  }
  FakeHttpClient privacy_client(false); AdvancedDownloadRuntimeService privacy(privacy_client, root, &clean_scanner); assert(!privacy.progress(private_id));

  const auto malicious_root = root / "malicious";
  FakeWardveilScanner malicious_scanner(FakeScanMode::malicious);
  FakeHttpClient malicious_client(false); AdvancedDownloadRuntimeService malicious_runtime(malicious_client, malicious_root, &malicious_scanner);
  const auto mq = malicious_runtime.enqueue({.source_url="https://example.test/malware.bin", .referrer_url="https://example.test/", .suggested_filename=std::string{"malware.bin"}, .private_session=false});
  assert(mq.accepted);
  for (int i=0;i<40;++i) { malicious_runtime.pump(); auto p=malicious_runtime.progress(mq.download_id); if (p && p->state==DownloadState::blocked) break; }
  const auto blocked = malicious_runtime.progress(mq.download_id);
  assert(blocked && blocked->state == DownloadState::blocked);
  assert(!std::filesystem::exists(malicious_root / "malware.bin"));
  const auto malicious_decision = malicious_runtime.security_decision(mq.download_id);
  assert(malicious_decision && malicious_decision->quarantine_required && !malicious_decision->can_release);
  const auto quarantine = malicious_decision->quarantine_handoff();
  assert(quarantine && quarantine->requires_explicit_executor_authority && !quarantine->destructive_action);

  const auto suspicious_root = root / "suspicious";
  FakeWardveilScanner suspicious_scanner(FakeScanMode::suspicious);
  FakeHttpClient suspicious_client(false); AdvancedDownloadRuntimeService suspicious_runtime(suspicious_client, suspicious_root, &suspicious_scanner);
  const auto sq = suspicious_runtime.enqueue({.source_url="https://example.test/suspicious.bin", .referrer_url="https://example.test/", .suggested_filename=std::string{"suspicious.bin"}, .private_session=false});
  assert(sq.accepted);
  for (int i=0;i<40;++i) { suspicious_runtime.pump(); auto p=suspicious_runtime.progress(sq.download_id); if (p && p->state==DownloadState::held) break; }
  assert(suspicious_runtime.progress(sq.download_id)->state == DownloadState::held);
  assert(!std::filesystem::exists(suspicious_root / "suspicious.bin"));

  const auto unavailable_root = root / "unavailable";
  FakeWardveilScanner unavailable_scanner(FakeScanMode::unavailable);
  FakeHttpClient unavailable_client(false); AdvancedDownloadRuntimeService unavailable_runtime(unavailable_client, unavailable_root, &unavailable_scanner);
  const auto uq = unavailable_runtime.enqueue({.source_url="https://example.test/unverified.bin", .referrer_url="https://example.test/", .suggested_filename=std::string{"unverified.bin"}, .private_session=false});
  assert(uq.accepted);
  for (int i=0;i<40;++i) { unavailable_runtime.pump(); auto p=unavailable_runtime.progress(uq.download_id); if (p && p->state==DownloadState::blocked) break; }
  assert(unavailable_runtime.progress(uq.download_id)->state == DownloadState::blocked);
  assert(!std::filesystem::exists(unavailable_root / "unverified.bin"));

  std::filesystem::remove_all(root, error); return 0;
}
