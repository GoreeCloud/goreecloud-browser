#pragma once

#include <array>
#include <string_view>

namespace goreecloud::browser {

enum class SettingsSection {
  general,
  privacy_and_blocking,
  search,
  passwords_and_vault,
  bookmarks_and_data,
  network_and_dns,
  proxy_manager,
  permissions,
  downloads,
  memory_management,
  private_browsing,
  advanced,
};

inline constexpr std::array<SettingsSection, 12> kPrimarySettingsSections{
    SettingsSection::general,
    SettingsSection::privacy_and_blocking,
    SettingsSection::search,
    SettingsSection::passwords_and_vault,
    SettingsSection::bookmarks_and_data,
    SettingsSection::network_and_dns,
    SettingsSection::proxy_manager,
    SettingsSection::permissions,
    SettingsSection::downloads,
    SettingsSection::memory_management,
    SettingsSection::private_browsing,
    SettingsSection::advanced,
};

struct PrivacyDefaults {
  bool https_only_direction{true};
  bool tracking_protection{true};
  bool sponsored_content{false};
  bool sponsored_search_suggestions{false};
  bool studies_enabled{false};
  bool unauthorized_search_fallback{false};
  bool unauthorized_external_dns_bypass{false};
  bool unwanted_inherited_ai_surfaces{false};
};

}  // namespace goreecloud::browser
