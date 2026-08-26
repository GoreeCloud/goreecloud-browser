#pragma once

#include <algorithm>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "goreecloud/browser/private_browsing.hpp"

namespace goreecloud::browser {

class InMemoryPrivateBrowsingManager final : public PrivateBrowsingManager {
 public:
  std::string create_session(PrivateSessionMode mode) override {
    const auto id = std::string{"private-session-"} + std::to_string(next_session_id_++);
    sessions_.emplace(id, SessionState{PrivateSessionDescriptor{id, mode, {}, {}, false}});
    return id;
  }

  bool attach_window(const std::string& session_id, const std::string& window_id) override {
    auto* state = find(session_id);
    if (!state) return false;
    state->window_ids.insert(window_id);
    return true;
  }

  bool forget_tab(const std::string& session_id, const std::string& tab_id) override {
    auto* state = find(session_id);
    if (!state) return false;
    state->tab_ids.erase(tab_id);
    if (state->descriptor.counters.open_tabs > 0) --state->descriptor.counters.open_tabs;
    return true;
  }

  bool forget_site(const std::string& session_id, const std::string& origin) override {
    auto* state = find(session_id);
    if (!state) return false;
    state->site_origins.erase(origin);
    state->descriptor.counters.sites_holding_temporary_data = state->site_origins.size();
    return true;
  }

  bool clear_cookies(const std::string& session_id) override { return clear_flag(session_id, &SessionState::cookies_present); }
  bool clear_cache(const std::string& session_id) override { return clear_flag(session_id, &SessionState::cache_present); }
  bool clear_site_storage(const std::string& session_id) override {
    auto* state = find(session_id);
    if (!state) return false;
    state->site_origins.clear();
    state->storage_present = false;
    state->descriptor.counters.sites_holding_temporary_data = 0;
    return true;
  }
  bool clear_authentication(const std::string& session_id) override { return clear_flag(session_id, &SessionState::authentication_present); }
  bool clear_permissions(const std::string& session_id) override {
    auto* state = find(session_id);
    if (!state) return false;
    state->permissions_present = false;
    state->descriptor.counters.temporary_permissions = 0;
    return true;
  }
  bool clear_history(const std::string& session_id) override { return clear_flag(session_id, &SessionState::history_present); }
  bool clear_search_state(const std::string& session_id) override { return clear_flag(session_id, &SessionState::search_state_present); }

  bool clear_everything(const std::string& session_id) override {
    auto* state = find(session_id);
    if (!state) return false;
    state->cookies_present = false;
    state->cache_present = false;
    state->storage_present = false;
    state->authentication_present = false;
    state->permissions_present = false;
    state->history_present = false;
    state->search_state_present = false;
    state->site_origins.clear();
    state->descriptor.counters.temporary_permissions = 0;
    state->descriptor.counters.sites_holding_temporary_data = 0;
    return true;
  }

  bool close_and_forget_session(const std::string& session_id) override {
    return sessions_.erase(session_id) > 0;
  }

  bool lock_session(const std::string& session_id) override {
    auto* state = find(session_id);
    if (!state) return false;
    state->descriptor.locked = true;
    return true;
  }

  bool unlock_session(const std::string& session_id) override {
    auto* state = find(session_id);
    if (!state) return false;
    state->descriptor.locked = false;
    return true;
  }

  [[nodiscard]] std::optional<PrivateSessionDescriptor> inspect(const std::string& session_id) const override {
    const auto it = sessions_.find(session_id);
    if (it == sessions_.end()) return std::nullopt;
    return it->second.descriptor;
  }

 private:
  struct SessionState {
    PrivateSessionDescriptor descriptor;
    std::unordered_set<std::string> window_ids;
    std::unordered_set<std::string> tab_ids;
    std::unordered_set<std::string> site_origins;
    bool cookies_present{true};
    bool cache_present{true};
    bool storage_present{true};
    bool authentication_present{true};
    bool permissions_present{true};
    bool history_present{true};
    bool search_state_present{true};
  };

  using FlagMember = bool SessionState::*;

  SessionState* find(const std::string& session_id) {
    const auto it = sessions_.find(session_id);
    return it == sessions_.end() ? nullptr : &it->second;
  }

  bool clear_flag(const std::string& session_id, FlagMember member) {
    auto* state = find(session_id);
    if (!state) return false;
    state->*member = false;
    return true;
  }

  std::unordered_map<std::string, SessionState> sessions_;
  std::uint64_t next_session_id_{1};
};

}  // namespace goreecloud::browser
