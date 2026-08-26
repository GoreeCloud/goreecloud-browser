#include <cassert>
#include <memory>
#include <string>

#include "goreecloud/browser/development_engine.hpp"
#include "goreecloud/browser/in_memory_private_browsing.hpp"
#include "goreecloud/browser/in_memory_tab_manager.hpp"
#include "goreecloud/browser/internal_pages.hpp"
#include "goreecloud/browser/private_cleanup_controller.hpp"
#include "goreecloud/browser/private_session_runtime.hpp"
#include "goreecloud/browser/toolbar.hpp"
#include "goreecloud/browser/unified_search_bar.hpp"
#include "goreecloud/browser/window_controller.hpp"

int main() {
  using namespace goreecloud::browser;

  static_assert(kToolbarItems.size() == 12);
  static_assert(kUnifiedSearchBarControls.size() == 3);
  static_assert(!kAlternateSearchProvidersAllowed);
  static_assert(!kSilentSearchFallbackAllowed);

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
  return 0;
}
