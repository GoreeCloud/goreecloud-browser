#include <cassert>
#include <optional>
#include <string>
#include <vector>

#include "goreecloud/browser/advanced_download_manager_service.hpp"
#include "goreecloud/browser/advanced_download_transfer_engine.hpp"
#include "goreecloud/browser/download_checkpoint_store.hpp"
#include "goreecloud/browser/download_transfer_policy.hpp"

namespace {

class FakeTransport final : public goreecloud::browser::DownloadTransport {
 public:
  std::optional<goreecloud::browser::DownloadResourceMetadata> inspect(
      std::string_view,
      std::string_view) override {
    return goreecloud::browser::DownloadResourceMetadata{
        .total_bytes = 1600,
        .accepts_byte_ranges = true,
        .resumable = true,
        .etag = std::string{"etag-v1"},
        .last_modified = std::string{"Wed, 26 Aug 2026 20:00:00 GMT"},
        .mime_type = std::string{"application/octet-stream"},
    };
  }

  goreecloud::browser::DownloadTransportResult transfer(
      const goreecloud::browser::DownloadTransportRequest& request) override {
    requests.push_back(request);
    const auto size = request.range.end_inclusive - request.range.begin + 1;
    if (requests.size() == 1) {
      return {.completed = false,
              .retryable = true,
              .transferred_bytes = size / 2,
              .message = "simulated interruption"};
    }
    return {.completed = true,
            .retryable = false,
            .transferred_bytes = size - request.resume_offset,
            .message = "ok"};
  }

  std::vector<goreecloud::browser::DownloadTransportRequest> requests;
};

}  // namespace

int main() {
  using namespace goreecloud::browser;

  InProcessAdvancedDownloadManagerService queue;
  const auto enqueue_result = queue.enqueue({
      .source_url = "https://example.test/archive.bin",
      .referrer_url = "https://example.test/",
      .suggested_filename = std::string{"archive.bin"},
      .private_session = false,
  });
  assert(enqueue_result.accepted);

  const auto record = queue.find(enqueue_result.download_id);
  assert(record);
  assert(record->segment_limit == 16);
  assert(record->resumable);

  DownloadResourceMetadata metadata;
  metadata.total_bytes = 1600;
  metadata.accepts_byte_ranges = true;
  metadata.resumable = true;
  metadata.etag = std::string{"etag-v1"};
  metadata.last_modified = std::string{"Wed, 26 Aug 2026 20:00:00 GMT"};

  auto plan = DownloadTransferPlanner::make_plan(*record, metadata);
  assert(plan.segments.size() == 16);
  assert(plan.segments.front().range.begin == 0);
  assert(plan.segments.back().range.end_inclusive == 1599);

  plan.segments[0].completed_bytes = 40;
  plan.completed_bytes = 40;
  InMemoryDownloadCheckpointStore checkpoints;
  assert(checkpoints.save(make_download_checkpoint(plan)));
  const auto saved = checkpoints.load(plan.download_id);
  assert(saved);
  assert(saved->completed_bytes == 40);

  auto restored_plan = DownloadTransferPlanner::make_plan(*record, metadata);
  assert(apply_download_checkpoint(restored_plan, *saved));
  assert(restored_plan.completed_bytes == 40);
  assert(restored_plan.segments[0].completed_bytes == 40);

  auto changed_metadata = metadata;
  changed_metadata.etag = std::string{"etag-v2"};
  auto changed_plan = DownloadTransferPlanner::make_plan(*record, changed_metadata);
  assert(!apply_download_checkpoint(changed_plan, *saved));

  FakeTransport transport;
  DownloadTransferScheduler scheduler(transport);
  scheduler.set_progress_callback([&](const DownloadTransferPlan& progress) {
    assert(checkpoints.save(make_download_checkpoint(progress)));
  });
  assert(scheduler.queue(*record));
  scheduler.pump();
  assert(scheduler.active_count() == 1);
  assert(transport.requests.size() == 1);
  assert(transport.requests[0].resume_offset == 0);
  const auto automatic_checkpoint = checkpoints.load(record->download_id);
  assert(automatic_checkpoint);
  assert(automatic_checkpoint->completed_bytes == 50);
  assert(automatic_checkpoint->segments[0].retry_count == 1);

  scheduler.pump();
  assert(transport.requests.size() >= 2);
  assert(transport.requests[1].resume_offset == 50);

  for (int i = 0; i < 40 && scheduler.completed_downloads().empty(); ++i) {
    scheduler.pump();
  }
  assert(scheduler.completed_downloads().size() == 1);
  assert(scheduler.completed_downloads().front() == record->download_id);
  const auto completed_checkpoint = checkpoints.load(record->download_id);
  assert(completed_checkpoint);
  assert(completed_checkpoint->completed_bytes == 1600);

  FakeTransport concurrency_transport;
  DownloadTransferScheduler concurrency_scheduler(concurrency_transport);
  for (int i = 0; i < 7; ++i) {
    DownloadRecord synthetic;
    synthetic.download_id = "download-concurrency-" + std::to_string(i);
    synthetic.request.source_url = "https://example.test/" + std::to_string(i);
    synthetic.request.referrer_url = "https://example.test/";
    synthetic.segment_limit = 1;
    assert(concurrency_scheduler.queue(std::move(synthetic)));
  }
  concurrency_scheduler.pump();
  assert(concurrency_scheduler.active_count() <= 5);
  assert(concurrency_scheduler.pending_count() >= 2);

  DownloadTransferPolicy wifi_policy;
  wifi_policy.wifi_only = true;
  wifi_policy.allow_metered = false;
  wifi_policy.stop_on_low_battery = true;
  wifi_policy.bytes_per_second_limit = 2'000'000;

  DownloadConnectionState wifi;
  wifi.type = DownloadConnectionType::wifi;
  wifi.online = true;
  DownloadPowerState healthy_power;
  healthy_power.battery_percent = 80;
  const auto wifi_allowed = DownloadTransferPolicyEngine::decide(
      wifi_policy, wifi, healthy_power);
  assert(wifi_allowed.allowed);
  assert(wifi_allowed.effective_bytes_per_second_limit == 2'000'000);

  DownloadConnectionState cellular;
  cellular.type = DownloadConnectionType::cellular;
  const auto cellular_denied = DownloadTransferPolicyEngine::decide(
      wifi_policy, cellular, healthy_power);
  assert(!cellular_denied.allowed);

  DownloadPowerState low_power;
  low_power.battery_percent = 10;
  low_power.charging = false;
  const auto low_battery_denied = DownloadTransferPolicyEngine::decide(
      wifi_policy, wifi, low_power);
  assert(!low_battery_denied.allowed);

  low_power.charging = true;
  const auto charging_allowed = DownloadTransferPolicyEngine::decide(
      wifi_policy, wifi, low_power);
  assert(charging_allowed.allowed);

  assert(queue.start(record->download_id));
  assert(queue.pause(record->download_id));
  assert(queue.resume(record->download_id));
  assert(queue.cancel(record->download_id));
  assert(queue.restart(record->download_id));

  return 0;
}
