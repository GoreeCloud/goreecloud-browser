#pragma once

#include <string>
#include <vector>

namespace goreecloud::browser {

enum class RecoveryDomain {
  browser_profile,
  vault,
  bookmarks,
  dns_configuration,
  network_configuration,
  package,
  application_state,
};

enum class RecoveryOperation { backup, restore, migrate, rollback, validate };
enum class RecoveryResult { pending, succeeded, failed };

struct RecoveryRequest {
  RecoveryDomain domain{RecoveryDomain::browser_profile};
  RecoveryOperation operation{RecoveryOperation::validate};
  std::string source_reference;
  std::string destination_reference;
};

struct RecoveryReceipt {
  RecoveryResult result{RecoveryResult::pending};
  std::string evidence_reference;
  std::string error;
};

class RecoveryService {
 public:
  virtual ~RecoveryService() = default;
  [[nodiscard]] virtual RecoveryReceipt execute(
      const RecoveryRequest& request) = 0;
};

// Everkeep recovery must preserve security boundaries. Recovery is not an
// authorization bypass and must not weaken encryption, identity, Vault, or
// platform security controls to make data accessible.

}  // namespace goreecloud::browser
