#pragma once

#include <string>

namespace goreecloud::browser {

enum class PersistentDestination {
  local_bookmarks,
  goreecloud_bookmarks,
  local_vault,
  goreecloud_vault,
  notes,
  memos,
  tasks,
  documents,
  ai_history,
  downloaded_file,
};

struct PrivatePersistenceRequest {
  std::string private_session_id;
  PersistentDestination destination{PersistentDestination::local_bookmarks};
  std::string description;
  bool explicit_user_action{false};
  bool boundary_warning_presented{false};
  bool boundary_warning_accepted{false};
};

struct PrivatePersistenceResult {
  bool accepted{false};
  bool destination_confirmed_write{false};
  std::string failure_reason;
};

class PrivatePersistenceBoundary {
 public:
  [[nodiscard]] static bool may_commit(const PrivatePersistenceRequest& request) {
    return request.explicit_user_action &&
           request.boundary_warning_presented &&
           request.boundary_warning_accepted;
  }

  [[nodiscard]] static bool may_report_success(
      const PrivatePersistenceRequest& request,
      const PrivatePersistenceResult& result) {
    return may_commit(request) && result.accepted && result.destination_confirmed_write;
  }
};

}  // namespace goreecloud::browser
