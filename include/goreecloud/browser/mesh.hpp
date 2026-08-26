#pragma once

#include <string>
#include <vector>

namespace goreecloud::browser {

enum class MeshActionKind {
  navigate,
  capture,
  query_status,
  notify,
  open_service_object,
};

struct MeshAction {
  MeshActionKind kind{MeshActionKind::navigate};
  std::string service;
  std::string object_id;
  std::string payload;
  bool private_context{false};
};

struct MeshReceipt {
  bool accepted{false};
  std::string correlation_id;
  std::string error;
};

class MeshCoordinator {
 public:
  virtual ~MeshCoordinator() = default;
  [[nodiscard]] virtual MeshReceipt dispatch(const MeshAction& action) = 0;
  [[nodiscard]] virtual std::vector<std::string> available_services() const = 0;
};

// GoreeCloud Mesh coordinates first-party services but does not absorb their
// data ownership or authority. Each destination service remains authoritative
// for its own stored data and security policy.

}  // namespace goreecloud::browser
