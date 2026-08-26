#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

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
  bool pinned{false};
  bool private_context{false};
};

struct BrowserWindowState {
  WindowId id{};
  std::vector<SessionTab> tabs;
  std::optional<TabId> active_tab_id;
  bool private_window{false};
};

struct SessionSnapshot {
  std::vector<BrowserWindowState> windows;
  std::uint64_t schema_version{1};
};

class SessionStore {
 public:
  virtual ~SessionStore() = default;
  [[nodiscard]] virtual bool save(const SessionSnapshot& snapshot) = 0;
  [[nodiscard]] virtual std::optional<SessionSnapshot> load() = 0;
  virtual bool clear_private_state() = 0;
};

}  // namespace goreecloud::browser
