#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace goreecloud::browser {

inline constexpr std::size_t kMaxRecoveryIdentifierBytes = 512;
inline constexpr std::size_t kMaxRecoveryWindows = 64;
inline constexpr std::size_t kMaxRecoveryTabsPerWindow = 512;
inline constexpr std::size_t kMaxRecoveryTotalTabs = 4096;

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
    return persist_sanitized(std::move(checkpoint));
  }

  bool mark_clean_shutdown(SessionCheckpoint checkpoint) {
    checkpoint.exit_state = SessionExitState::clean_shutdown;
    return persist_sanitized(std::move(checkpoint));
  }

  bool mark_unclean_shutdown(SessionCheckpoint checkpoint) {
    checkpoint.exit_state = SessionExitState::unclean_shutdown;
    return persist_sanitized(std::move(checkpoint));
  }

  [[nodiscard]] std::optional<SessionRecoveryCandidate> latest_candidate() const {
    if (policy_.max_checkpoints == 0) {
      return std::nullopt;
    }

    const auto recent = store_.read_recent(1);
    if (recent.empty()) {
      return std::nullopt;
    }

    auto checkpoint = sanitize_for_persistence(recent.front());
    if (!valid_checkpoint(checkpoint) || checkpoint.exit_state == SessionExitState::clean_shutdown ||
        checkpoint.windows.empty()) {
      return std::nullopt;
    }

    SessionRecoveryCandidate candidate;
    candidate.checkpoint = std::move(checkpoint);
    candidate.was_unclean_shutdown = true;
    candidate.recoverable_window_count = candidate.checkpoint.windows.size();
    for (const auto& window : candidate.checkpoint.windows) {
      candidate.recoverable_tab_count += window.tabs.size();
    }
    return candidate;
  }

  bool discard(const std::string& checkpoint_id) {
    if (!valid_identifier(checkpoint_id)) {
      return false;
    }
    return store_.erase(checkpoint_id);
  }

 private:
  [[nodiscard]] static bool valid_identifier(const std::string& value) {
    return !value.empty() && value.size() <= kMaxRecoveryIdentifierBytes;
  }

  [[nodiscard]] bool persist_sanitized(SessionCheckpoint checkpoint) {
    checkpoint = sanitize_for_persistence(std::move(checkpoint));
    if (!valid_checkpoint(checkpoint)) {
      return false;
    }
    return store_.write(checkpoint);
  }

  [[nodiscard]] bool valid_checkpoint(const SessionCheckpoint& checkpoint) const {
    if (!valid_identifier(checkpoint.checkpoint_id) || checkpoint.created_unix_ms == 0 ||
        checkpoint.windows.size() > kMaxRecoveryWindows) {
      return false;
    }

    std::unordered_set<std::string> window_ids;
    std::unordered_set<std::string> tab_ids;
    std::size_t total_tabs = 0;

    for (const auto& window : checkpoint.windows) {
      if (!valid_identifier(window.window_id) || !window_ids.insert(window.window_id).second ||
          window.tabs.size() > kMaxRecoveryTabsPerWindow) {
        return false;
      }

      total_tabs += window.tabs.size();
      if (total_tabs > kMaxRecoveryTotalTabs) {
        return false;
      }

      std::size_t active_count = 0;
      std::optional<std::string> active_flagged_id;
      for (const auto& tab : window.tabs) {
        if (!valid_identifier(tab.tab_id) || !tab_ids.insert(tab.tab_id).second) {
          return false;
        }
        if (tab.active) {
          ++active_count;
          active_flagged_id = tab.tab_id;
          if (active_count > 1) {
            return false;
          }
        }
      }

      if (window.active_tab_id.has_value()) {
        if (!valid_identifier(*window.active_tab_id)) {
          return false;
        }
        bool found = false;
        for (const auto& tab : window.tabs) {
          if (tab.tab_id == *window.active_tab_id) {
            found = true;
            break;
          }
        }
        if (!found || (active_flagged_id.has_value() && *active_flagged_id != *window.active_tab_id)) {
          return false;
        }
      }
    }

    return true;
  }

  [[nodiscard]] bool should_persist(SessionPrivacyMode mode) const {
    // Private and isolated-private state is deliberately non-configurable here.
    // A future product change must introduce a separately reviewed privacy contract
    // rather than flipping a persistence flag in this recovery layer.
    return mode == SessionPrivacyMode::normal && policy_.persist_normal_windows;
  }

  SessionRecoveryStore& store_;
  SessionRecoveryPolicy policy_;
};

}  // namespace goreecloud::browser
