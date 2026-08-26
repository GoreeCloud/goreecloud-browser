#pragma once

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "goreecloud/browser/session_recovery.hpp"

namespace goreecloud::browser {

enum class RecoveryStartupAction {
  none,
  offer_restore,
  restore_automatically,
};

struct RecoveryStartupPolicy {
  bool offer_after_unclean_shutdown{true};
  bool auto_restore_after_unclean_shutdown{false};
  bool allow_clean_session_restore{false};
};

struct RecoveryStartupModel {
  RecoveryStartupAction action{RecoveryStartupAction::none};
  std::string checkpoint_id;
  std::size_t window_count{0};
  std::size_t tab_count{0};
  std::vector<std::string> window_ids;
};

class SessionRecoveryStartupController {
 public:
  SessionRecoveryStartupController(SessionRecoveryCoordinator& coordinator,
                                   RecoveryStartupPolicy policy = {})
      : coordinator_(coordinator), policy_(std::move(policy)) {}

  [[nodiscard]] RecoveryStartupModel evaluate() const {
    RecoveryStartupModel model;
    const auto candidate = coordinator_.latest_candidate();
    if (!candidate.has_value()) {
      return model;
    }

    const bool eligible_unclean =
        candidate->was_unclean_shutdown && policy_.offer_after_unclean_shutdown;
    const bool eligible_clean =
        !candidate->was_unclean_shutdown && policy_.allow_clean_session_restore;
    if (!eligible_unclean && !eligible_clean) {
      return model;
    }

    model.action = candidate->was_unclean_shutdown && policy_.auto_restore_after_unclean_shutdown
                       ? RecoveryStartupAction::restore_automatically
                       : RecoveryStartupAction::offer_restore;
    model.checkpoint_id = candidate->checkpoint.checkpoint_id;
    model.window_count = candidate->recoverable_window_count;
    model.tab_count = candidate->recoverable_tab_count;
    model.window_ids.reserve(candidate->checkpoint.windows.size());
    for (const auto& window : candidate->checkpoint.windows) {
      model.window_ids.push_back(window.window_id);
    }
    return model;
  }

  bool discard(const RecoveryStartupModel& model) {
    return !model.checkpoint_id.empty() && coordinator_.discard(model.checkpoint_id);
  }

 private:
  SessionRecoveryCoordinator& coordinator_;
  RecoveryStartupPolicy policy_;
};

}  // namespace goreecloud::browser
