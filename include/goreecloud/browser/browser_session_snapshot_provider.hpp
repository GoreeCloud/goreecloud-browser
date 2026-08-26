#pragma once

#include <algorithm>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "goreecloud/browser/advanced_tab_manager.hpp"
#include "goreecloud/browser/session_recovery_lifecycle.hpp"

namespace goreecloud::browser {

struct BrowserWindowSessionState {
  std::string window_id;
  SessionPrivacyMode privacy_mode{SessionPrivacyMode::normal};
  std::optional<std::string> active_tab_id;
};

struct BrowserTabNavigationState {
  std::string tab_id;
  std::string url;
  std::string title;
  bool active{false};
  std::uint64_t last_active_unix_ms{0};
};

class BrowserSessionStateSource {
 public:
  virtual ~BrowserSessionStateSource() = default;

  [[nodiscard]] virtual std::vector<BrowserWindowSessionState> windows() const = 0;
  [[nodiscard]] virtual std::vector<ManagedTabState> managed_tabs() const = 0;
  [[nodiscard]] virtual std::vector<BrowserTabNavigationState> navigation_states() const = 0;
};

class BrowserSessionSnapshotProvider final : public SessionSnapshotProvider {
 public:
  explicit BrowserSessionSnapshotProvider(const BrowserSessionStateSource& source)
      : source_(source) {}

  [[nodiscard]] SessionCheckpoint capture(std::string checkpoint_id,
                                          std::uint64_t unix_ms) const override {
    SessionCheckpoint checkpoint;
    checkpoint.checkpoint_id = std::move(checkpoint_id);
    checkpoint.created_unix_ms = unix_ms;

    const auto managed_tabs = source_.managed_tabs();
    const auto navigation = source_.navigation_states();

    std::unordered_map<std::string, ManagedTabState> managed_by_id;
    managed_by_id.reserve(managed_tabs.size());
    for (const auto& tab : managed_tabs) {
      if (!tab.tab_id.empty()) managed_by_id.emplace(tab.tab_id, tab);
    }

    std::unordered_map<std::string, BrowserTabNavigationState> navigation_by_id;
    navigation_by_id.reserve(navigation.size());
    for (const auto& state : navigation) {
      if (!state.tab_id.empty()) navigation_by_id.emplace(state.tab_id, state);
    }

    for (const auto& window_state : source_.windows()) {
      if (window_state.window_id.empty()) continue;

      RecoverableWindow window;
      window.window_id = window_state.window_id;
      window.privacy_mode = window_state.privacy_mode;
      window.active_tab_id = window_state.active_tab_id;

      for (const auto& [tab_id, managed] : managed_by_id) {
        if (managed.window_id != window_state.window_id) continue;
        const auto nav_it = navigation_by_id.find(tab_id);
        if (nav_it == navigation_by_id.end()) continue;

        const auto& nav = nav_it->second;
        RecoverableTab tab;
        tab.tab_id = tab_id;
        tab.url = nav.url;
        tab.title = nav.title;
        tab.workspace_id = managed.workspace_id;
        tab.group_id = managed.group_id;
        tab.split_id = managed.split_id;
        tab.pinned = managed.pinned;
        tab.active = nav.active ||
                     (window_state.active_tab_id && *window_state.active_tab_id == tab_id);
        tab.last_active_unix_ms = tab.active
                                      ? std::max(nav.last_active_unix_ms, unix_ms)
                                      : nav.last_active_unix_ms;
        window.tabs.push_back(std::move(tab));
      }

      checkpoint.windows.push_back(std::move(window));
    }

    return checkpoint;
  }

 private:
  const BrowserSessionStateSource& source_;
};

}  // namespace goreecloud::browser
