#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "goreecloud/browser/download.hpp"

namespace goreecloud::browser {

enum class NetworkClass { offline, wifi, ethernet, cellular, metered, unknown };

struct DeviceTransferState {
  NetworkClass network{NetworkClass::unknown};
  bool battery_saver{false};
  std::optional<std::uint8_t> battery_percent;
  bool charging{false};
};

struct RetryPolicy {
  std::uint8_t maximum_attempts{8};
  std::chrono::seconds initial_delay{2};
  std::chrono::seconds maximum_delay{300};
  double multiplier{2.0};
};

struct SchedulerDecision {
  bool eligible{false};
  std::string reason;
};

class DownloadScheduler {
 public:
  virtual ~DownloadScheduler() = default;
  virtual void set_retry_policy(RetryPolicy policy) = 0;
  [[nodiscard]] virtual SchedulerDecision evaluate(
      const DownloadRecord& record,
      const DownloadPolicy& policy,
      const DeviceTransferState& device_state) const = 0;
  [[nodiscard]] virtual std::vector<std::string> select_next(
      const std::vector<DownloadRecord>& records,
      std::size_t available_slots) const = 0;
  [[nodiscard]] virtual std::chrono::seconds retry_delay(
      std::uint8_t attempt) const = 0;
};

}  // namespace goreecloud::browser
