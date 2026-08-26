#pragma once

#include <string_view>
#include <vector>

namespace goreecloud::browser {

struct SyncCapability {
  std::string_view dataset;
  std::string_view application;
  int schema_version;
  bool read;
  bool write;
  bool erase;
};

inline std::vector<SyncCapability> sync_capabilities() {
  return {
      {"browser.tabs", "browser", 1, true, true, true},
      {"browser.history", "browser", 1, true, true, true},
      {"browser.preferences", "browser", 1, true, true, false},
  };
}

inline const SyncCapability* find_sync_capability(
    const std::vector<SyncCapability>& capabilities,
    std::string_view dataset) {
  for (const auto& capability : capabilities) {
    if (capability.dataset == dataset) {
      return &capability;
    }
  }
  return nullptr;
}

}  // namespace goreecloud::browser
