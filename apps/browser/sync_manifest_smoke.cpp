#include <cassert>
#include <string_view>

#include "goreecloud/browser/sync_manifest.hpp"

int main() {
  const auto manifest = goreecloud::browser::sync_manifest();
  assert(manifest.schema_version == 1);
  assert(manifest.application == std::string_view{"browser"});
  assert(manifest.capabilities.size() == 3);
  assert(!manifest.records_exposed);
  assert(!manifest.private_state_exposed);
  assert(!manifest.credentials_exposed);
  assert(!manifest.production_approved);

  const auto* history = goreecloud::browser::find_sync_capability(
      manifest.capabilities, "browser.history");
  assert(history != nullptr);
  assert(history->read && history->write && history->erase);
  return 0;
}
