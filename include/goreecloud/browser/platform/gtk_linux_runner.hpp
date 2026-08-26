#pragma once

#include <chrono>
#include <stdexcept>
#include <string>
#include <thread>

#include "goreecloud/browser/application.hpp"
#include "goreecloud/browser/chrome_command_router.hpp"
#include "goreecloud/browser/chrome_shell.hpp"
#include "goreecloud/browser/configured_search_router.hpp"
#include "goreecloud/browser/internal_pages.hpp"
#include "goreecloud/browser/omnibox_controller.hpp"
#include "goreecloud/browser/platform/gtk_linux_glaze_host.hpp"

namespace goreecloud::browser::platform {

inline int run_gtk_linux_browser(BrowserApplication& application) {
  auto* window = application.first_window();
  if (!window || !window->active_tab()) {
    throw std::runtime_error("GoreeCloud Browser has no initial window/tab for GTK host");
  }

  GtkLinuxGlazeWindowHost host;
  host.set_private_window(window->private_window());
  BrowserChromeShell chrome(*window);
  ChromeCommandRouter commands(*window);
  auto search_router = search_router_from_environment();
  OmniboxController omnibox(search_router);

  host.set_toolbar_handler([&](ToolbarItem item) {
    commands.clear_panel();
    if (!commands.invoke(item)) return;
    if (!commands.active_panel().empty()) {
      host.show_panel(commands.active_panel());
    }
  });

  host.set_search_handler([&](std::string_view input) {
    try {
      const auto resolution = omnibox.resolve(input);
      if (resolution.intent == OmniboxIntent::empty) return;
      commands.clear_panel();
      window->navigate_active(resolution.value);
      if (is_goreecloud_internal_url(resolution.value)) {
        host.show_internal_surface(resolution.value);
      } else if (auto* tab = window->active_tab()) {
        host.attach_engine_view(tab->engine_view());
      }
    } catch (const std::exception&) {
      // Search failure is visible and fail-closed. No alternate provider is
      // ever substituted when GoreeCloud Search is unavailable/unconfigured.
      host.show_panel("GoreeCloud Search unavailable");
    }
  });

  host.set_search_control_handler([&](UnifiedSearchBarControl control) {
    switch (control) {
      case UnifiedSearchBarControl::advanced_reader_mode:
        host.show_panel("Advanced Reader Mode");
        break;
      case UnifiedSearchBarControl::unified_bookmarks:
        host.show_panel("Unified Bookmarks");
        break;
      case UnifiedSearchBarControl::wardveil_security:
        host.show_panel("Wardveil Security");
        break;
    }
  });

  if (!host.create()) {
    throw std::runtime_error(
        "Linux beta host requires GTK3 on an X11/XWayland display; native Wayland embedding is not yet enabled");
  }

  host.set_title(window->private_window() ? "GoreeCloud Browser — Private"
                                          : "GoreeCloud Browser");
  host.render_chrome(chrome.snapshot());

  auto current_url = window->active_tab()->engine_view().navigation_state().url;
  if (is_goreecloud_internal_url(current_url)) {
    host.show_internal_surface(current_url);
  } else {
    host.attach_engine_view(window->active_tab()->engine_view());
  }
  host.show();

  std::string last_url = current_url;
  while (host.pump_events()) {
    application.engine().pump_events();

    host.render_chrome(chrome.snapshot());
    if (auto* tab = window->active_tab()) {
      const auto navigation = tab->engine_view().navigation_state();
      if (navigation.url != last_url) {
        last_url = navigation.url;
        if (is_goreecloud_internal_url(navigation.url)) {
          host.show_internal_surface(navigation.url);
        } else {
          host.attach_engine_view(tab->engine_view());
        }
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  host.close();
  return 0;
}

}  // namespace goreecloud::browser::platform
