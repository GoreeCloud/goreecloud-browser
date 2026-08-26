#pragma once

#include <array>
#include <string_view>

namespace goreecloud::browser {

enum class ToolbarItem {
  back,
  forward,
  refresh,
  home,
  unified_search,
  advanced_download_manager,
  privacy_shield,
  wardveil_security,
  settings,
};

inline constexpr std::array<ToolbarItem, 9> kDefaultToolbar{
    ToolbarItem::back,
    ToolbarItem::forward,
    ToolbarItem::refresh,
    ToolbarItem::home,
    ToolbarItem::unified_search,
    ToolbarItem::advanced_download_manager,
    ToolbarItem::privacy_shield,
    ToolbarItem::wardveil_security,
    ToolbarItem::settings,
};

inline constexpr bool kAllowAdditionalDefaultToolbarItems = false;

[[nodiscard]] constexpr std::string_view toolbar_item_name(ToolbarItem item) {
  switch (item) {
    case ToolbarItem::back: return "Back";
    case ToolbarItem::forward: return "Forward";
    case ToolbarItem::refresh: return "Refresh";
    case ToolbarItem::home: return "Home";
    case ToolbarItem::unified_search: return "Unified Search";
    case ToolbarItem::advanced_download_manager: return "Advanced Download Manager";
    case ToolbarItem::privacy_shield: return "Privacy Shield";
    case ToolbarItem::wardveil_security: return "Wardveil Security";
    case ToolbarItem::settings: return "Settings";
  }
  return "Unknown";
}

static_assert(kDefaultToolbar.size() == 9);
static_assert(!kAllowAdditionalDefaultToolbarItems);

}  // namespace goreecloud::browser
