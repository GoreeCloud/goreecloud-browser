#pragma once

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "goreecloud/browser/download_scheduler.hpp"

namespace goreecloud::browser {

class BasicDownloadScheduler final : public DownloadScheduler {
 public:
  void set_retry_policy(RetryPolicy policy) override { retry_policy_ = policy; }

  [[nodiscard]] SchedulerDecision evaluate(
      const DownloadRecord& record,
      const DownloadPolicy& policy,
      const DeviceTransferState& device_state) const override {
    if (record.state != DownloadState::queued &&
        record.state != DownloadState::failed &&
        record.state != DownloadState::paused) {
      return {false, "record state is not schedulable"};
    }
    if (device_state.network == NetworkClass::offline) {
      return {false, "network is offline"};
    }
    if (policy.wifi_only && device_state.network != NetworkClass::wifi) {
      return {false, "Wi-Fi-only policy is active"};
    }
    if (!policy.allow_metered_network &&
        (device_state.network == NetworkClass::metered ||
         device_state.network == NetworkClass::cellular)) {
      return {false, "metered network is disallowed"};
    }
    if (device_state.battery_saver && !device_state.charging) {
      return {false, "battery saver is active"};
    }
    if (device_state.battery_percent && *device_state.battery_percent <= 10 &&
        !device_state.charging) {
      return {false, "battery is too low for background transfer"};
    }
    return {true, "eligible"};
  }

  [[nodiscard]] std::vector<std::string> select_next(
      const std::vector<DownloadRecord>& records,
      std::size_t available_slots) const override {
    std::vector<const DownloadRecord*> candidates;
    for (const auto& record : records) {
      if (record.state == DownloadState::queued || record.state == DownloadState::failed) {
        candidates.push_back(&record);
      }
    }

    std::stable_sort(candidates.begin(), candidates.end(), [](const auto* a, const auto* b) {
      if (a->priority != b->priority) {
        return static_cast<int>(a->priority) > static_cast<int>(b->priority);
      }
      return a->created_sequence < b->created_sequence;
    });

    std::vector<std::string> selected;
    for (std::size_t i = 0; i < candidates.size() && selected.size() < available_slots; ++i) {
      selected.push_back(candidates[i]->id);
    }
    return selected;
  }

  [[nodiscard]] std::chrono::seconds retry_delay(
      std::uint8_t attempt) const override {
    if (attempt == 0) {
      return std::chrono::seconds{0};
    }
    const auto exponent = static_cast<double>(attempt - 1);
    const auto raw = static_cast<double>(retry_policy_.initial_delay.count()) *
                     std::pow(retry_policy_.multiplier, exponent);
    const auto capped = std::min<double>(
        raw, static_cast<double>(retry_policy_.maximum_delay.count()));
    return std::chrono::seconds{static_cast<long long>(capped)};
  }

 private:
  RetryPolicy retry_policy_{};
};

}  // namespace goreecloud::browser
