#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace goreecloud::browser {

enum class ReleaseChannel { development, preview, stable };

enum class AcceptanceResult { not_run, passed, failed };

struct ArtifactEvidence {
  std::string filename;
  std::string platform;
  std::string artifact_type;
  std::string sha256;
  std::uint64_t byte_size{};
  std::string signature_scheme;
  std::string signer_identity;
  std::string signature_sha256;
  std::string acceptance_record_sha256;
  std::string git_revision;
};

struct AcceptanceCheck {
  std::string id;
  std::string description;
  AcceptanceResult result{AcceptanceResult::not_run};
  std::string evidence_reference;
};

struct ReleaseAcceptance {
  ReleaseChannel channel{ReleaseChannel::development};
  std::vector<ArtifactEvidence> artifacts;
  std::vector<AcceptanceCheck> checks;
};

// Stable acceptance requires evidence tied to the exact 40-character source
// revision. CI success alone is not production acceptance.
class ReleaseGate {
 public:
  virtual ~ReleaseGate() = default;
  [[nodiscard]] virtual bool eligible_for_stable(
      const ReleaseAcceptance& acceptance) const = 0;
};

}  // namespace goreecloud::browser
