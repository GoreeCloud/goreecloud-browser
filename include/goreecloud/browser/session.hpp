#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "goreecloud/browser/advanced_tab_manager.hpp"
#include "goreecloud/browser/container.hpp"

namespace goreecloud::browser {

using WindowId = std::uint64_t;
using TabId = std::uint64_t;

struct NavigationEntry {
  std::string url;
  std::string title;
};

struct SessionTab {
  TabId id{};
  std::vector<NavigationEntry> history;
  std::size_t current_index{};
  std::optional<ContainerId> container_id;
  std::string workspace_id;
  std::optional<std::string> group_id;
  std::optional<std::string> split_id;
  bool pinned{false};
  bool locked{false};
  bool protected_tab{false};
  bool sleeping{false};
  bool private_context{false};
};

struct SessionTabGroup {
  std::string id;
  std::string workspace_id;
  std::string name;
  std::string color_token;
  std::string icon_token;
  bool collapsed{false};
  std::vector<TabId> tab_ids;
};

struct SessionSplitView {
  std::string id;
  std::string workspace_id;
  SplitLayout layout{SplitLayout::none};
  std::vector<TabId> tab_ids;
  std::vector<double> pane_fractions;
  std::optional<std::size_t> expanded_pane;
};

struct SessionWorkspace {
  std::string id;
  std::string name;
  std::vector<TabId> tab_ids;
  std::vector<SessionTabGroup> groups;
  std::vector<SessionSplitView> splits;
};

struct BrowserWindowState {
  WindowId id{};
  std::vector<SessionTab> tabs;
  std::optional<TabId> active_tab_id;
  std::string active_workspace_id;
  bool private_window{false};
};

struct SessionSnapshot {
  std::vector<BrowserWindowState> windows;
  std::vector<SessionWorkspace> workspaces;
  std::uint64_t schema_version{2};
};

class SessionStore {
 public:
  virtual ~SessionStore() = default;
  [[nodiscard]] virtual bool save(const SessionSnapshot& snapshot) = 0;
  [[nodiscard]] virtual std::optional<SessionSnapshot> load() = 0;
  virtual bool clear_private_state() = 0;
};

}  // namespace goreecloud::browser
