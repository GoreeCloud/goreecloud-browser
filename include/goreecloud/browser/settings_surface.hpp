#pragma once

#include <array>
#include <string_view>

namespace goreecloud::browser {

enum class SettingsSection {
  general,
  appearance,
  search,
  privacy_and_blocking,
  wardveil_security,
  passwords_and_vault,
  bookmarks_and_data,
  downloads,
  network_and_dns,
  proxy_manager,
  permissions,
  containers,
  sync_and_identity,
  everkeep,
  memory_management,
  private_browsing,
  accessibility,
  advanced,
  about,
};

inline constexpr std::array kSettingsSections{
    SettingsSection::general,
    SettingsSection::appearance,
    SettingsSection::search,
    SettingsSection::privacy_and_blocking,
    SettingsSection::wardveil_security,
    SettingsSection::passwords_and_vault,
    SettingsSection::bookmarks_and_data,
    SettingsSection::downloads,
    SettingsSection::network_and_dns,
    SettingsSection::proxy_manager,
    SettingsSection::permissions,
    SettingsSection::containers,
    SettingsSection::sync_and_identity,
    SettingsSection::everkeep,
    SettingsSection::memory_management,
    SettingsSection::private_browsing,
    SettingsSection::accessibility,
    SettingsSection::advanced,
    SettingsSection::about,
};

inline constexpr bool kSettingsSearchEnabled = true;
inline constexpr bool kSettingsUseProgressiveDisclosure = true;
inline constexpr bool kInheritedSettingsTaxonomyAllowed = false;
inline constexpr bool kSettingsUseLatestApprovedGlaze = true;
inline constexpr bool kEverkeepFirstClassSettingsArea = true;

static_assert(kSettingsSearchEnabled);
static_assert(kSettingsUseProgressiveDisclosure);
static_assert(!kInheritedSettingsTaxonomyAllowed);
static_assert(kSettingsUseLatestApprovedGlaze);
static_assert(kEverkeepFirstClassSettingsArea);

}  // namespace goreecloud::browser
