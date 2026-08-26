#pragma once

#include <array>
#include <string_view>

namespace goreecloud::browser {

enum class ToolbarAction {
  back,
  forward,
  home,
  reload_or_stop,
  address_and_search,
  clipboard,
  settings,
  downloads,
  clear_dns_cache,
  proxy_quick_switch,
};

inline constexpr std::array<ToolbarAction, 8> kDefaultDesktopToolbar{
    ToolbarAction::back,
    ToolbarAction::forward,
    ToolbarAction::home,
    ToolbarAction::reload_or_stop,
    ToolbarAction::address_and_search,
    ToolbarAction::clipboard,
    ToolbarAction::settings,
    ToolbarAction::downloads,
};

enum class ReloadStopMode {
  hard_reload,
  stop,
};

[[nodiscard]] constexpr ReloadStopMode reload_stop_mode(bool page_loading) {
  return page_loading ? ReloadStopMode::stop : ReloadStopMode::hard_reload;
}

}  // namespace goreecloud::browser
