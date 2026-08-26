#include <cassert>
#include <memory>
#include <string>

#include "goreecloud/browser/application.hpp"
#include "goreecloud/browser/command_line.hpp"
#include "goreecloud/browser/configured_search_router.hpp"
#include "goreecloud/browser/development_engine.hpp"
#include "goreecloud/browser/in_memory_tab_manager.hpp"
#include "goreecloud/browser/internal_pages.hpp"
#include "goreecloud/browser/omnibox_controller.hpp"
#include "goreecloud/browser/toolbar.hpp"
#include "goreecloud/browser/unified_search_bar.hpp"
#include "goreecloud/browser/version.hpp"
#include "goreecloud/browser/window_controller.hpp"

int main() {
  using namespace goreecloud::browser;

  static_assert(kDefaultToolbar.size() == 12);
  static_assert(kUnifiedSearchBarControls.size() == 3);
  static_assert(!kAlternateSearchProvidersAllowed);
  static_assert(!kSilentSearchFallbackAllowed);
  static_assert(kBetaChannel);
  static_assert(!kProductionApproved);

  {
    char executable[] = "goreecloud-browser";
    char private_flag[] = "--private";
    char url[] = "https://example.com/";
    char* argv[]{executable, private_flag, url};
    const auto launch = parse_browser_launch_request(3, argv);
    assert(launch.private_window);
    assert(!launch.isolated_private_window);
    assert(launch.urls.size() == 1);
    assert(launch.urls.front() == "https://example.com/");
  }

  ConfiguredGoreeCloudSearchRouter search_router("https://search.goreecloud.test/search");
  OmniboxController omnibox(search_router);
  const auto url_resolution = omnibox.resolve("example.com");
  assert(url_resolution.intent == OmniboxIntent::direct_navigation);
  assert(url_resolution.value == "https://example.com");
  const auto search_resolution = omnibox.resolve("goreecloud browser beta");
  assert(search_resolution.intent == OmniboxIntent::goreecloud_search);
  assert(search_resolution.value.find("https://search.goreecloud.test/search?q=") == 0);

  DevelopmentEngine engine;
  engine.initialize();

  EngineContextOptions normal_options;
  normal_options.profile_id = "smoke-normal";
  normal_options.storage_path = "smoke-profile";
  normal_options.private_context = false;
  normal_options.persistent_storage = true;

  auto normal_context = engine.create_context(normal_options);
  assert(normal_context);

  InMemoryAdvancedTabManager tab_manager;
  WindowController window(*normal_context, false, &tab_manager, "window-smoke");

  auto& first = window.new_tab(std::string{kNewTabUrl});
  tab_manager.register_tab({first.id(), window.window_id(), "workspace-main"});
  auto& second = window.new_tab("https://example.com/");
  tab_manager.register_tab({second.id(), window.window_id(), "workspace-main"});

  assert(window.tab_count() == 2);
  assert(window.select_tab(first.id(), false));
  assert(window.select_tab(second.id(), true));
  assert(window.pin_selected_tabs(true));
  assert(window.protect_selected_tabs(true));
  assert(!window.close_tab(second.id()));
  assert(window.close_tab(second.id(), true));
  assert(window.tab_count() == 1);

  window.open_home();
  assert(window.active_tab());
  assert(window.active_tab()->engine_view().navigation_state().url == kHomeUrl);
  window.open_settings();
  assert(window.active_tab()->engine_view().navigation_state().url == kSettingsUrl);

  engine.shutdown();

  BrowserApplicationOptions private_options;
  private_options.initial_private_window = true;
  private_options.initial_private_session_id = "smoke-private";
  private_options.initial_url = std::string{kPrivateStartUrl};
  BrowserApplication private_browser(std::make_unique<DevelopmentEngine>(), private_options);
  private_browser.initialize();
  assert(private_browser.window_count() == 1);
  assert(private_browser.first_window());
  assert(private_browser.first_window()->private_window());
  assert(private_browser.first_window()->active_tab());
  assert(private_browser.first_window()->active_tab()->engine_view().navigation_state().url ==
         kPrivateStartUrl);
  assert(private_browser.has_private_session_context("smoke-private"));
  private_browser.shutdown();

  return 0;
}
