#pragma once

#include <optional>
#include <string>
#include <vector>

namespace goreecloud::browser {

enum class DeviceTrustState { pending, authorized, revoked };

struct BrowserProfile {
  std::string id;
  std::string display_name;
  bool local_only{true};
};

struct AuthorizedDevice {
  std::string id;
  std::string display_name;
  DeviceTrustState trust{DeviceTrustState::pending};
};

class IdentityService {
 public:
  virtual ~IdentityService() = default;
  [[nodiscard]] virtual std::optional<BrowserProfile> active_profile() const = 0;
  [[nodiscard]] virtual std::vector<AuthorizedDevice> devices() const = 0;
  virtual bool enroll_device(const std::string& device_id) = 0;
  virtual bool revoke_device(const std::string& device_id) = 0;
};

// Browser profiles and device authorization are GoreeCloud-owned concepts.
// Engine profile identifiers must not become canonical GoreeCloud identities.

}  // namespace goreecloud::browser
