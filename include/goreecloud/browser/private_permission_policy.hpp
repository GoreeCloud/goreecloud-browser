#pragma once

#include <optional>
#include <string>
#include <unordered_map>

namespace goreecloud::browser {

enum class PrivatePermissionKind {
  camera,
  microphone,
  location,
  notifications,
  bluetooth,
  usb,
  midi,
  clipboard,
};

enum class PrivatePermissionDecision { allow_once, allow_for_session, block };

struct PrivatePermissionGrant {
  PrivatePermissionKind kind{PrivatePermissionKind::camera};
  PrivatePermissionDecision decision{PrivatePermissionDecision::block};
  std::string origin;
  std::string session_id;
};

class PrivatePermissionStore {
 public:
  void set(PrivatePermissionGrant grant) {
    grants_[key(grant.session_id, grant.origin, grant.kind)] = std::move(grant);
  }

  [[nodiscard]] std::optional<PrivatePermissionGrant> get(
      const std::string& session_id,
      const std::string& origin,
      PrivatePermissionKind kind) const {
    const auto it = grants_.find(key(session_id, origin, kind));
    if (it == grants_.end()) return std::nullopt;
    return it->second;
  }

  void consume_allow_once(const std::string& session_id,
                          const std::string& origin,
                          PrivatePermissionKind kind) {
    const auto k = key(session_id, origin, kind);
    const auto it = grants_.find(k);
    if (it != grants_.end() &&
        it->second.decision == PrivatePermissionDecision::allow_once) {
      grants_.erase(it);
    }
  }

  void forget_origin(const std::string& session_id, const std::string& origin) {
    erase_matching(session_id, origin);
  }

  void forget_session(const std::string& session_id) {
    erase_matching(session_id, std::nullopt);
  }

 private:
  static std::string key(const std::string& session_id,
                         const std::string& origin,
                         PrivatePermissionKind kind) {
    return session_id + "\n" + origin + "\n" + std::to_string(static_cast<int>(kind));
  }

  void erase_matching(const std::string& session_id,
                      std::optional<std::string> origin) {
    for (auto it = grants_.begin(); it != grants_.end();) {
      const bool session_match = it->second.session_id == session_id;
      const bool origin_match = !origin || it->second.origin == *origin;
      if (session_match && origin_match) it = grants_.erase(it);
      else ++it;
    }
  }

  std::unordered_map<std::string, PrivatePermissionGrant> grants_;
};

}  // namespace goreecloud::browser
