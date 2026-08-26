#pragma once

#include <string_view>
#include <vector>

#include "goreecloud/browser/sync_capabilities.hpp"

namespace goreecloud::browser {

// SyncManifest is the stable Browser-owned discovery surface consumed by a
// GoreeCloud Sync adapter. It advertises protocol metadata only; browser state
// is exported through dataset-specific adapters after authorization.
struct SyncManifest {
  int schema_version;
  std::string_view application;
  std::vector<SyncCapability> capabilities;
  bool records_exposed;
  bool private_state_exposed;
  bool credentials_exposed;
  bool production_approved;
};

inline SyncManifest sync_manifest() {
  return SyncManifest{
      .schema_version = 1,
      .application = "browser",
      .capabilities = sync_capabilities(),
      .records_exposed = false,
      .private_state_exposed = false,
      .credentials_exposed = false,
      .production_approved = false,
  };
}

}  // namespace goreecloud::browser
