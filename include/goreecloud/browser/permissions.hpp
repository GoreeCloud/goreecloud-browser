#pragma once

#include <string>
#include <string_view>

namespace goreecloud::browser {

enum class PermissionKind {
  camera,
  microphone,
  location,
  notifications,
  clipboard_read,
  clipboard_write,
  midi,
  usb,
  serial,
  bluetooth,
  filesystem,
  popups,
  autoplay,
};

enum class PermissionDecision { ask, allow, block };

enum class PermissionScope { once, session, persistent };

struct PermissionRequest {
  std::string origin;
  PermissionKind kind{PermissionKind::notifications};
  bool private_context{false};
};

struct PermissionGrant {
  PermissionDecision decision{PermissionDecision::ask};
  PermissionScope scope{PermissionScope::once};
};

class PermissionPolicy {
 public:
  virtual ~PermissionPolicy() = default;
  [[nodiscard]] virtual PermissionGrant evaluate(
      const PermissionRequest& request) const = 0;
  virtual bool revoke(std::string_view origin, PermissionKind kind) = 0;
  virtual void clear_session_grants() = 0;
};

}  // namespace goreecloud::browser
