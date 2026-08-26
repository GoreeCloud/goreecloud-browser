#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace goreecloud::browser {

enum class DownloadConnectionType {
  unknown,
  ethernet,
  wifi,
  cellular,
  metered,
  offline,
};

enum class DownloadTurboMode {
  off,
  balanced,
  aggressive,
};

struct DownloadConnectionState {
  DownloadConnectionType type{DownloadConnectionType::unknown};
  bool metered{false};
  bool roaming{false};
  bool online{true};
};

struct DownloadPowerState {
  std::optional<unsigned> battery_percent;
  bool charging{false};
};

struct DownloadTransferPolicy {
  bool wifi_only{false};
  bool allow_metered{true};
  bool allow_roaming{false};
  bool stop_on_low_battery{false};
  unsigned low_battery_threshold_percent{15};
  std::optional<std::uint64_t> bytes_per_second_limit;
  DownloadTurboMode turbo_mode{DownloadTurboMode::balanced};
};

struct DownloadPolicyDecision {
  bool allowed{false};
  std::string reason;
  std::optional<std::uint64_t> effective_bytes_per_second_limit;
};

class DownloadTransferPolicyEngine {
 public:
  static DownloadPolicyDecision decide(const DownloadTransferPolicy& policy,
                                       const DownloadConnectionState& network,
                                       const DownloadPowerState& power) {
    if (!network.online || network.type == DownloadConnectionType::offline) {
      return {false, "Network is offline.", std::nullopt};
    }
    if (policy.wifi_only && network.type != DownloadConnectionType::wifi) {
      return {false, "This download is restricted to Wi-Fi.", std::nullopt};
    }
    if (!policy.allow_metered &&
        (network.metered || network.type == DownloadConnectionType::metered)) {
      return {false, "Metered connections are disabled for this download profile.", std::nullopt};
    }
    if (!policy.allow_roaming && network.roaming) {
      return {false, "Roaming transfers are disabled for this download profile.", std::nullopt};
    }
    if (policy.stop_on_low_battery && power.battery_percent && !power.charging &&
        *power.battery_percent <= policy.low_battery_threshold_percent) {
      return {false, "Download paused because battery level is low.", std::nullopt};
    }

    return {true, {}, policy.bytes_per_second_limit};
  }
};

inline constexpr unsigned kDefaultDownloadLowBatteryThresholdPercent = 15;
static_assert(kDefaultDownloadLowBatteryThresholdPercent == 15);

}  // namespace goreecloud::browser
