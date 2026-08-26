#include <iostream>
#include <memory>

#include "goreecloud/browser/application.hpp"
#include "goreecloud/browser/development_engine.hpp"
#include "goreecloud/browser/internal_pages.hpp"
#include "goreecloud/browser/toolbar.hpp"
#include "goreecloud/browser/unified_search_bar.hpp"

int main() {
  using namespace goreecloud::browser;

  BrowserApplicationOptions options;
  options.initial_url = std::string{kNewTabUrl};

  BrowserApplication browser(std::make_unique<DevelopmentEngine>(), options);
  browser.initialize();

  std::cout << "GoreeCloud Browser 0.1 development runtime\n";
  std::cout << "Engine: " << browser.engine().name() << " "
            << browser.engine().version() << "\n";
  std::cout << "Windows: " << browser.window_count() << "\n";
  std::cout << "Startup surface: " << kNewTabUrl << "\n";
  std::cout << "Toolbar controls: " << kDefaultToolbar.size() << "\n";
  std::cout << "Search authority: " << kSearchAuthority << "\n";
  std::cout << "Glaze UI: latest approved Stable baseline enforced by build\n";
  std::cout << "Chromium rendering: not yet integrated\n";

  browser.shutdown();
  return 0;
}
