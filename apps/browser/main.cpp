#include <iostream>
#include <memory>

#include "goreecloud/browser/application.hpp"
#include "goreecloud/browser/development_engine.hpp"

int main() {
  using namespace goreecloud::browser;

  BrowserApplicationOptions options;
  options.initial_url = "https://search.goreecloud.local/";

  BrowserApplication browser(std::make_unique<DevelopmentEngine>(), options);
  browser.initialize();

  std::cout << "GoreeCloud Browser 0.1 development runtime\n";
  std::cout << "Engine: " << browser.engine().name() << " "
            << browser.engine().version() << "\n";
  std::cout << "Windows: " << browser.window_count() << "\n";
  std::cout << "Glaze UI: latest approved Stable baseline enforced by build\n";
  std::cout << "Chromium adapter: pending integration\n";

  browser.shutdown();
  return 0;
}
