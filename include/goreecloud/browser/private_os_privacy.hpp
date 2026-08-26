#pragma once

#include <string>

namespace goreecloud::browser {

struct PrivateOsPrivacyCapabilities {
  bool screenshot_blocking{false};
  bool task_switcher_redaction{false};
  bool window_preview_redaction{false};
  bool recent_apps_redaction{false};
  bool device_authentication{false};
};

struct PrivateOsPrivacyRequest {
  bool screenshot_blocking{false};
  bool task_switcher_redaction{false};
  bool window_preview_redaction{false};
  bool recent_apps_redaction{false};
};

struct PrivateOsPrivacyState {
  bool screenshot_blocking_enforced{false};
  bool task_switcher_redaction_enforced{false};
  bool window_preview_redaction_enforced{false};
  bool recent_apps_redaction_enforced{false};
};

class PrivateOsPrivacyHost {
 public:
  virtual ~PrivateOsPrivacyHost() = default;
  [[nodiscard]] virtual PrivateOsPrivacyCapabilities capabilities() const = 0;
  virtual PrivateOsPrivacyState apply(const std::string& window_id,
                                      const PrivateOsPrivacyRequest& request) = 0;
  virtual void clear(const std::string& window_id) = 0;
};

// UI must report the returned enforced state, never merely the requested state.
inline constexpr bool kPrivateOsProtectionClaimsRequireEnforcementEvidence = true;
static_assert(kPrivateOsProtectionClaimsRequireEnforcementEvidence);

}  // namespace goreecloud::browser
