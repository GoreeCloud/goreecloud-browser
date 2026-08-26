#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <utility>

#include "goreecloud/browser/session_recovery.hpp"

namespace goreecloud::browser {

class SessionSnapshotProvider {
 public:
  virtual ~SessionSnapshotProvider() = default;
  [[nodiscard]] virtual SessionCheckpoint capture(std::string checkpoint_id,
                                                  std::uint64_t unix_ms) const = 0;
};

struct SessionRecoveryLifecyclePolicy {
  std::uint64_t checkpoint_interval_ms{30'000};
};

class SessionRecoveryLifecycleController {
 public:
  SessionRecoveryLifecycleController(SessionRecoveryCoordinator& recovery,
                                     const SessionSnapshotProvider& snapshots,
                                     SessionRecoveryLifecyclePolicy policy = {})
      : recovery_(recovery), snapshots_(snapshots), policy_(std::move(policy)) {}

  bool begin(std::string checkpoint_id, std::uint64_t unix_ms) {
    if (checkpoint_id.empty()) return false;
    checkpoint_id_ = std::move(checkpoint_id);
    last_checkpoint_unix_ms_ = unix_ms;
    active_ = recovery_.checkpoint(snapshots_.capture(checkpoint_id_, unix_ms));
    return active_;
  }

  bool checkpoint_if_due(std::uint64_t unix_ms) {
    if (!active_) return false;
    if (unix_ms < last_checkpoint_unix_ms_) return false;
    if (unix_ms - last_checkpoint_unix_ms_ < policy_.checkpoint_interval_ms) return true;
    if (!recovery_.checkpoint(snapshots_.capture(checkpoint_id_, unix_ms))) return false;
    last_checkpoint_unix_ms_ = unix_ms;
    return true;
  }

  bool checkpoint_now(std::uint64_t unix_ms) {
    if (!active_) return false;
    if (!recovery_.checkpoint(snapshots_.capture(checkpoint_id_, unix_ms))) return false;
    last_checkpoint_unix_ms_ = unix_ms;
    return true;
  }

  bool clean_shutdown(std::uint64_t unix_ms) {
    if (!active_) return false;
    const auto saved = recovery_.mark_clean_shutdown(
        snapshots_.capture(checkpoint_id_, unix_ms));
    if (saved) active_ = false;
    return saved;
  }

  [[nodiscard]] bool active() const noexcept { return active_; }
  [[nodiscard]] const std::string& checkpoint_id() const noexcept { return checkpoint_id_; }

 private:
  SessionRecoveryCoordinator& recovery_;
  const SessionSnapshotProvider& snapshots_;
  SessionRecoveryLifecyclePolicy policy_;
  std::string checkpoint_id_;
  std::uint64_t last_checkpoint_unix_ms_{0};
  bool active_{false};
};

}  // namespace goreecloud::browser
