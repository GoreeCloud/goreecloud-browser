#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace goreecloud::browser {

enum class SessionPrivacyMode {
  normal,
  private_window,
  isolated_private,
};

enum class SessionExitState {
  running,
  clean_shutdown,
  unclean_shutdown,
};

struct RecoverableTab {
  std::string tab_id;
  std::string url;
  std::string title;
  std::string workspace_id;
  std::optional<std::string> group_id;
  std::optional<std::string> split_id;
  bool pinned{false};
  bool active{false};
  std::uint64_t last_active_unix_ms{0};
};

struct RecoverableWindow {
  std::string window_id;
  SessionPrivacyMode privacy_mode{SessionPrivacyMode::normal};
  std::vector<RecoverableTab> tabs;
  std::optional<std::string> active_tab_id;
};

struct SessionCheckpoint {
  std::string checkpoint_id;
  std::uint64_t created_unix_ms{0};
  SessionExitState exit_state{SessionExitState::running};
  std::vector<RecoverableWindow> windows;
};

struct SessionRecoveryPolicy {
  bool persist_normal_windows{true};
  bool persist_private_windows{false};
  bool persist_isolated_private_windows{false};
  std::size_t max_checkpoints{5};
};

struct SessionRecoveryCandidate {
  SessionCheckpoint checkpoint;
  std::size_t recoverable_window_count{0};
  std::size_t recoverable_tab_count{0};
  bool was_unclean_shutdown{false};
};

class SessionRecoveryStore {
 public:
  virtual ~SessionRecoveryStore() = default;
  virtual bool write(const SessionCheckpoint& checkpoint) = 0;
  [[nodiscard]] virtual std::vector<SessionCheckpoint> read_recent(std::size_t limit) const = 0;
  virtual bool erase(const std::string& checkpoint_id) = 0;
};

class SessionRecoveryCoordinator {
 public:
  SessionRecoveryCoordinator(SessionRecoveryStore& store,
                             SessionRecoveryPolicy policy = {})
      : store_(store), policy_(std::move(policy)) {}

  [[nodiscard]] SessionCheckpoint sanitize_for_persistence(SessionCheckpoint checkpoint) const {
    std::vector<RecoverableWindow> retained;
    retained.reserve(checkpoint.windows.size());

    for (auto& window : checkpoint.windows) {
      if (!should_persist(window.privacy_mode)) {
        continue;
      }
      retained.push_back(std::move(window));
    }

    checkpoint.windows = std::move(retained);
    return checkpoint;
  }

  bool checkpoint(SessionCheckpoint checkpoint) {
    checkpoint.exit_state = SessionExitState::running;
    return store_.write(sanitize_for_persistence(std::move(checkpoint)));
  }

  bool mark_clean_shutdown(SessionCheckpoint checkpoint) {
    checkpoint.exit_state = SessionExitState::clean_shutdown;
    return store_.write(sanitize_for_persistence(std::move(checkpoint)));
  }

  bool mark_unclean_shutdown(SessionCheckpoint checkpoint) {
    checkpoint.exit_state = SessionExitState::unclean_shutdown;
    return store_.write(sanitize_for_persistence(std::move(checkpoint)));
  }

  [[nodiscard]] std::optional<SessionRecoveryCandidate> latest_candidate() const {
    const auto recent = store_.read_recent(policy_.max_checkpoints);
    for (const auto& checkpoint : recent) {
      SessionRecoveryCandidate candidate;
      candidate.checkpoint = sanitize_for_persistence(checkpoint);
      candidate.was_unclean_shutdown =
          candidate.checkpoint.exit_state == SessionExitState::unclean_shutdown;
      candidate.recoverable_window_count = candidate.checkpoint.windows.size();
      for (const auto& window : candidate.checkpoint.windows) {
        candidate.recoverable_tab_count += window.tabs.size();
      }

      if (candidate.recoverable_window_count > 0) {
        return candidate;
      }
    }
    return std::nullopt;
  }

  bool discard(const std::string& checkpoint_id) {
    return store_.erase(checkpoint_id);
  }

 private:
  [[nodiscard]] bool should_persist(SessionPrivacyMode mode) const {
    switch (mode) {
      case SessionPrivacyMode::normal:
        return policy_.persist_normal_windows;
      case SessionPrivacyMode::private_window:
        return policy_.persist_private_windows;
      case SessionPrivacyMode::isolated_private:
        return policy_.persist_isolated_private_windows;
    }
    return false;
  }

  SessionRecoveryStore& store_;
  SessionRecoveryPolicy policy_;
};

}  // namespace goreecloud::browser
