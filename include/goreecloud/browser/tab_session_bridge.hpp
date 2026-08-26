#pragma once

#include <string>
#include <vector>

#include "goreecloud/browser/advanced_tab_manager.hpp"
#include "goreecloud/browser/session.hpp"
#include "goreecloud/browser/window_controller.hpp"

namespace goreecloud::browser {

class TabSessionBridge {
 public:
  [[nodiscard]] SessionSnapshot snapshot(
      const std::vector<BrowserWindowState>& windows,
      const std::vector<WorkspaceState>& workspaces,
      const std::vector<TabGroupState>& groups,
      const std::vector<SplitViewState>& splits) const {
    SessionSnapshot result;
    result.windows = windows;
    result.workspaces = workspaces;
    result.groups = groups;
    result.splits = splits;
    result.schema_version = 2;
    return result;
  }

  [[nodiscard]] bool can_restore(const SessionSnapshot& snapshot) const noexcept {
    return snapshot.schema_version >= 2;
  }
};

}  // namespace goreecloud::browser
