#pragma once

#include <string_view>

#include "goreecloud/browser/chrome_panels.hpp"
#include "goreecloud/browser/toolbar.hpp"
#include "goreecloud/browser/window_controller.hpp"

namespace goreecloud::browser {

class ChromeCommandRouter {
 public:
  explicit ChromeCommandRouter(WindowController& window) : window_(window) {}

  bool invoke(ToolbarItem item) {
    switch (item) {
      case ToolbarItem::back:
        window_.back();
        return true;
      case ToolbarItem::forward:
        window_.forward();
        return true;
      case ToolbarItem::refresh:
        window_.reload_or_stop();
        return true;
      case ToolbarItem::home:
        window_.open_home();
        return true;
      case ToolbarItem::unified_search:
        return true;
      case ToolbarItem::advanced_download_manager:
        panel_ = "downloads";
        return true;
      case ToolbarItem::privacy_shield:
        panel_ = "privacy-shield";
        return true;
      case ToolbarItem::wardveil_security:
        panel_ = "wardveil-security";
        return true;
      case ToolbarItem::clipboard:
        panel_ = "clipboard";
        return true;
      case ToolbarItem::dns_cache:
        panel_ = "dns-cache";
        return true;
      case ToolbarItem::advanced_proxy_manager:
        panel_ = "proxy-manager";
        return true;
      case ToolbarItem::settings:
        window_.open_settings();
        return true;
    }
    return false;
  }

  [[nodiscard]] std::string_view active_panel() const noexcept { return panel_; }
  void clear_panel() noexcept { panel_ = {}; }

 private:
  WindowController& window_;
  std::string_view panel_{};
};

}  // namespace goreecloud::browser
