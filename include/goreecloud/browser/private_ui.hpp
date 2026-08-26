#pragma once

#include <string>
#include <vector>

#include "goreecloud/browser/private_browsing.hpp"

namespace goreecloud::browser {

struct PrivateStartPageModel {
  std::string title{"Private Browsing"};
  std::string summary{"Temporary local browsing state with Privacy Shield and Wardveil Security active."};
  bool show_private_indicator{true};
  bool show_no_anonymity_notice{true};
  bool show_sync_disabled{true};
  bool show_privacy_shield_status{true};
  bool show_wardveil_status{true};
  bool show_dns_status{true};
  bool show_network_status{true};
};

struct PrivateDashboardModel {
  PrivateSessionDescriptor session;
  std::vector<std::string> available_actions{
      "View Privacy Details",
      "View Stored Data",
      "Clear Private Cookies",
      "Clear Private Cache",
      "Clear Private Site Storage",
      "Clear Private Authentication State",
      "Clear Private Permissions",
      "Clear Private History Entries",
      "Clear Private Search State",
      "Clear Everything",
      "Close & Forget Private Session",
  };
};

class PrivateUiModelBuilder {
 public:
  [[nodiscard]] PrivateStartPageModel start_page() const { return {}; }

  [[nodiscard]] std::optional<PrivateDashboardModel> dashboard(
      const PrivateBrowsingManager& manager,
      const std::string& session_id) const {
    auto session = manager.inspect(session_id);
    if (!session.has_value()) return std::nullopt;
    return PrivateDashboardModel{*session};
  }
};

inline constexpr bool kPrivateUiUsesLatestGlaze = true;
inline constexpr bool kPrivateUiHasPersistentIndicator = true;
inline constexpr bool kPrivateUiExplainsLimits = true;

}  // namespace goreecloud::browser
