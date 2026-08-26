#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "goreecloud/browser/application.hpp"
#include "goreecloud/browser/browser_session_snapshot_provider.hpp"

namespace goreecloud::browser {

struct RuntimeTabRecoveryMetadata {
  std::string workspace_id{"default"};
  std::optional<std::string> group_id;
  std::optional<std::string> split_id;
  bool pinned{false};
  std::uint64_t last_active_unix_ms{0};
};

class RuntimeBrowserSessionStateSource final : public BrowserSessionStateSource {
 public:
  explicit RuntimeBrowserSessionStateSource(const BrowserApplication& application)
      : application_(application) {}

  void update_tab_topology(std::string tab_id,
                           std::string workspace_id,
                           std::optional<std::string> group_id,
                           std::optional<std::string> split_id,
                           bool pinned) {
    if (tab_id.empty()) return;
    auto& metadata = tab_metadata_[std::move(tab_id)];
    metadata.workspace_id = workspace_id.empty() ? "default" : std::move(workspace_id);
    metadata.group_id = std::move(group_id);
    metadata.split_id = std::move(split_id);
    metadata.pinned = pinned;
  }

  void mark_tab_active(const std::string& tab_id, std::uint64_t unix_ms) {
    if (tab_id.empty()) return;
    tab_metadata_[tab_id].last_active_unix_ms = unix_ms;
  }

  void forget_tab(const std::string& tab_id) {
    tab_metadata_.erase(tab_id);
  }

  [[nodiscard]] std::vector<BrowserWindowSessionState> windows() const override {
    std::vector<BrowserWindowSessionState> result;
    for (const auto* window : application_.windows()) {
      if (!window) continue;
      BrowserWindowSessionState state;
      state.window_id = window->window_id();
      state.privacy_mode = window->private_window()
                               ? SessionPrivacyMode::private_window
                               : SessionPrivacyMode::normal;
      if (const auto* active = window->active_tab()) state.active_tab_id = active->id();
      result.push_back(std::move(state));
    }
    return result;
  }

  [[nodiscard]] std::vector<ManagedTabState> managed_tabs() const override {
    std::vector<ManagedTabState> result;
    for (const auto* window : application_.windows()) {
      if (!window) continue;
      for (const auto* tab : window->tabs()) {
        if (!tab) continue;
        ManagedTabState state;
        state.tab_id = tab->id();
        state.window_id = window->window_id();
        const auto found = tab_metadata_.find(tab->id());
        if (found != tab_metadata_.end()) {
          state.workspace_id = found->second.workspace_id;
          state.group_id = found->second.group_id;
          state.split_id = found->second.split_id;
          state.pinned = found->second.pinned;
        } else {
          state.workspace_id = "default";
        }
        result.push_back(std::move(state));
      }
    }
    return result;
  }

  [[nodiscard]] std::vector<BrowserTabNavigationState> navigation_states() const override {
    std::vector<BrowserTabNavigationState> result;
    for (const auto* window : application_.windows()) {
      if (!window) continue;
      const auto* active = window->active_tab();
      for (const auto* tab : window->tabs()) {
        if (!tab) continue;
        const auto navigation = tab->engine_view().navigation_state();
        BrowserTabNavigationState state;
        state.tab_id = tab->id();
        state.url = navigation.url;
        state.title = navigation.title;
        state.active = active && active->id() == tab->id();
        const auto found = tab_metadata_.find(tab->id());
        if (found != tab_metadata_.end()) {
          state.last_active_unix_ms = found->second.last_active_unix_ms;
        }
        result.push_back(std::move(state));
      }
    }
    return result;
  }

 private:
  const BrowserApplication& application_;
  std::unordered_map<std::string, RuntimeTabRecoveryMetadata> tab_metadata_;
};

}  // namespace goreecloud::browser
