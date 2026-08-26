#pragma once

#include <string>
#include <vector>

namespace goreecloud::browser {

enum class Platform {
  linux,
  android,
  ios,
  ipados,
  windows,
  macos,
};

enum class PlatformTier { first_class, family_client, future_candidate };

struct PlatformTarget {
  Platform platform{Platform::linux};
  PlatformTier tier{PlatformTier::future_candidate};
  bool native_browser_role_required{false};
};

struct DefaultBrowserState {
  bool eligible{false};
  bool registered{false};
  bool selected_by_user{false};
};

class PlatformIntegration {
 public:
  virtual ~PlatformIntegration() = default;
  [[nodiscard]] virtual Platform platform() const = 0;
  [[nodiscard]] virtual DefaultBrowserState default_browser_state() const = 0;
  virtual bool request_default_browser_selection() = 0;
};

// GoreeCloud Browser may request the OS-controlled default-browser flow but
// must never silently seize or force the default-browser role.

}  // namespace goreecloud::browser
