#include <exception>
#include <iostream>

#include "goreecloud/browser/application.hpp"
#include "goreecloud/browser/cef_process.hpp"
#include "goreecloud/browser/internal_pages.hpp"
#include "goreecloud/browser/runtime_engine_factory.hpp"
#include "goreecloud/browser/toolbar.hpp"
#include "goreecloud/browser/unified_search_bar.hpp"

int main(int argc, char** argv) {
  using namespace goreecloud::browser;

  const int cef_exit_code = execute_cef_subprocess_if_needed(argc, argv);
  if (cef_exit_code >= 0) return cef_exit_code;

  try {
    BrowserApplicationOptions options;
    options.initial_url = std::string{kNewTabUrl};

    auto selection = create_runtime_engine_from_environment();
    BrowserApplication browser(std::move(selection.engine), options);
    browser.initialize();

    std::cout << "GoreeCloud Browser 0.1.0-beta.1\n";
    std::cout << "Runtime mode: " << selection.mode << "\n";
    std::cout << "Engine: " << browser.engine().name() << " "
              << browser.engine().version() << "\n";
    std::cout << "Windows: " << browser.window_count() << "\n";
    std::cout << "Startup surface: " << kNewTabUrl << "\n";
    std::cout << "Toolbar controls: " << kDefaultToolbar.size() << "\n";
    std::cout << "Search authority: " << kSearchAuthority << "\n";
    std::cout << "Glaze UI: approved Stable baseline enforced by build\n";
    std::cout << "Production renderer requested: "
              << (selection.production_renderer_requested ? "yes" : "no") << "\n";

    browser.shutdown();
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "GoreeCloud Browser startup failed: " << error.what() << "\n";
    return 1;
  }
}
