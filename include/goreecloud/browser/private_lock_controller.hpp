#pragma once

#include <chrono>
#include <string>
#include <unordered_map>

namespace goreecloud::browser {

struct PrivateLockPolicy {
  bool enabled{false};
  bool lock_after_inactivity{false};
  std::chrono::minutes inactivity_timeout{15};
  bool lock_on_device_lock{false};
  bool lock_when_backgrounded{false};
  bool lock_during_user_switch{false};
  bool close_when_browser_locks{false};
};

struct PrivateLockState {
  bool locked{false};
  std::chrono::steady_clock::time_point last_activity{std::chrono::steady_clock::now()};
};

class PrivateLockController {
 public:
  void configure(const std::string& session_id, PrivateLockPolicy policy) {
    policies_[session_id] = policy;
    states_.try_emplace(session_id);
  }

  void note_activity(const std::string& session_id) {
    states_[session_id].last_activity = std::chrono::steady_clock::now();
  }

  [[nodiscard]] bool should_lock_for_inactivity(const std::string& session_id,
                                                 std::chrono::steady_clock::time_point now) const {
    const auto p = policies_.find(session_id);
    const auto s = states_.find(session_id);
    if (p == policies_.end() || s == states_.end()) return false;
    return p->second.enabled && p->second.lock_after_inactivity &&
           now - s->second.last_activity >= p->second.inactivity_timeout;
  }

  [[nodiscard]] bool should_lock_on_device_lock(const std::string& session_id) const {
    return flag(session_id, &PrivateLockPolicy::lock_on_device_lock);
  }

  [[nodiscard]] bool should_lock_when_backgrounded(const std::string& session_id) const {
    return flag(session_id, &PrivateLockPolicy::lock_when_backgrounded);
  }

  [[nodiscard]] bool should_lock_during_user_switch(const std::string& session_id) const {
    return flag(session_id, &PrivateLockPolicy::lock_during_user_switch);
  }

  [[nodiscard]] bool should_close_when_browser_locks(const std::string& session_id) const {
    return flag(session_id, &PrivateLockPolicy::close_when_browser_locks);
  }

  void set_locked(const std::string& session_id, bool locked) {
    states_[session_id].locked = locked;
  }

  [[nodiscard]] bool is_locked(const std::string& session_id) const {
    const auto it = states_.find(session_id);
    return it != states_.end() && it->second.locked;
  }

  void forget_session(const std::string& session_id) {
    policies_.erase(session_id);
    states_.erase(session_id);
  }

 private:
  [[nodiscard]] bool flag(const std::string& session_id,
                          bool PrivateLockPolicy::*member) const {
    const auto it = policies_.find(session_id);
    return it != policies_.end() && it->second.enabled && it->second.*member;
  }

  std::unordered_map<std::string, PrivateLockPolicy> policies_;
  std::unordered_map<std::string, PrivateLockState> states_;
};

}  // namespace goreecloud::browser
