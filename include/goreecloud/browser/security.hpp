#pragma once

#include <string>
#include <vector>

namespace goreecloud::browser {

enum class SecurityMechanism {
  tls,
  certificate_validation,
  safe_browsing,
  sandbox,
  site_isolation,
  process_isolation,
  permissions,
  redirect_protection,
  download_protection,
  update_security,
  origin_isolation,
};

enum class MechanismState { unknown, inactive, active, degraded, failed };

struct SecurityEvidence {
  SecurityMechanism mechanism{SecurityMechanism::tls};
  MechanismState state{MechanismState::unknown};
  std::string evidence_reference;
};

struct BrowserSecurityState {
  std::vector<SecurityEvidence> mechanisms;
};

class SecurityStateProvider {
 public:
  virtual ~SecurityStateProvider() = default;
  [[nodiscard]] virtual BrowserSecurityState current_state() const = 0;
};

// Wardveil presentation must be derived from evidence supplied by the actual
// mechanisms. Branding must never manufacture a protected state.

}  // namespace goreecloud::browser
