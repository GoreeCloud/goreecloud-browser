#pragma once

#include <string_view>

#include "goreecloud/browser/toolbar.hpp"

namespace goreecloud::browser {

enum class ChromePanel {
  none,
  advanced_download_manager,
  privacy_shield,
  wardveil_security,
  clipboard,
  dns_cache,
  advanced_proxy_manager,
  settings,
};

[[nodiscard]] constexpr ChromePanel panel_for_toolbar_item(ToolbarItem item) {
  switch (item) {
    case ToolbarItem::advanced_download_manager:
      return ChromePanel::advanced_download_manager;
    case ToolbarItem::privacy_shield:
      return ChromePanel::privacy_shield;
    case ToolbarItem::wardveil_security:
      return ChromePanel::wardveil_security;
    case ToolbarItem::clipboard:
      return ChromePanel::clipboard;
    case ToolbarItem::dns_cache:
      return ChromePanel::dns_cache;
    case ToolbarItem::advanced_proxy_manager:
      return ChromePanel::advanced_proxy_manager;
    case ToolbarItem::settings:
      return ChromePanel::settings;
    case ToolbarItem::back:
    case ToolbarItem::forward:
    case ToolbarItem::refresh:
    case ToolbarItem::home:
    case ToolbarItem::unified_search:
      return ChromePanel::none;
  }
  return ChromePanel::none;
}

[[nodiscard]] constexpr std::string_view chrome_panel_url(ChromePanel panel) {
  switch (panel) {
    case ChromePanel::advanced_download_manager:
      return "goreecloud://downloads";
    case ChromePanel::privacy_shield:
      return "goreecloud://privacy-shield";
    case ChromePanel::wardveil_security:
      return "goreecloud://wardveil";
    case ChromePanel::clipboard:
      return "goreecloud://clipboard";
    case ChromePanel::dns_cache:
      return "goreecloud://dns-cache";
    case ChromePanel::advanced_proxy_manager:
      return "goreecloud://proxy";
    case ChromePanel::settings:
      return "goreecloud://settings";
    case ChromePanel::none:
      return "";
  }
  return "";
}

inline constexpr bool kChromePanelsAreGoreeCloudOwned = true;
inline constexpr bool kChromePanelsUseLatestApprovedGlaze = true;

static_assert(kChromePanelsAreGoreeCloudOwned);
static_assert(kChromePanelsUseLatestApprovedGlaze);

}  // namespace goreecloud::browser
