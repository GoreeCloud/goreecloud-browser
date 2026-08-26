#include <cassert>
#include <string_view>

#include "goreecloud/browser/sync_capabilities.hpp"

int main() {
  const auto capabilities = goreecloud::browser::sync_capabilities();
  assert(capabilities.size() == 3);

  const auto* tabs = goreecloud::browser::find_sync_capability(
      capabilities, "browser.tabs");
  assert(tabs != nullptr);
  assert(tabs->application == std::string_view{"browser"});
  assert(tabs->schema_version == 1);
  assert(tabs->read && tabs->write && tabs->erase);

  const auto* preferences = goreecloud::browser::find_sync_capability(
      capabilities, "browser.preferences");
  assert(preferences != nullptr);
  assert(preferences->read && preferences->write);
  assert(!preferences->erase);

  assert(goreecloud::browser::find_sync_capability(
             capabilities, "search.history") == nullptr);
  return 0;
}
