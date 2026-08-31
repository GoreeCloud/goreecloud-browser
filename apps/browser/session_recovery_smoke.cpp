#include <algorithm>
#include <cassert>
#include <cstddef>
#include <string>
#include <vector>

#include "goreecloud/browser/session_recovery.hpp"

namespace {

class MemorySessionRecoveryStore final : public goreecloud::browser::SessionRecoveryStore {
 public:
  bool write(const goreecloud::browser::SessionCheckpoint& checkpoint) override {
    checkpoints_.erase(
        std::remove_if(checkpoints_.begin(), checkpoints_.end(), [&](const auto& existing) {
          return existing.checkpoint_id == checkpoint.checkpoint_id;
        }),
        checkpoints_.end());
    checkpoints_.insert(checkpoints_.begin(), checkpoint);
    return true;
  }

  [[nodiscard]] std::vector<goreecloud::browser::SessionCheckpoint> read_recent(
      std::size_t limit) const override {
    const auto count = std::min(limit, checkpoints_.size());
    return {checkpoints_.begin(), checkpoints_.begin() + static_cast<std::ptrdiff_t>(count)};
  }

  bool erase(const std::string& checkpoint_id) override {
    const auto before = checkpoints_.size();
    checkpoints_.erase(
        std::remove_if(checkpoints_.begin(), checkpoints_.end(), [&](const auto& checkpoint) {
          return checkpoint.checkpoint_id == checkpoint_id;
        }),
        checkpoints_.end());
    return checkpoints_.size() != before;
  }

 private:
  std::vector<goreecloud::browser::SessionCheckpoint> checkpoints_;
};

goreecloud::browser::RecoverableWindow window(
    std::string id,
    goreecloud::browser::SessionPrivacyMode privacy_mode,
    std::string tab_id) {
  return goreecloud::browser::RecoverableWindow{
      .window_id = std::move(id),
      .privacy_mode = privacy_mode,
      .tabs = {goreecloud::browser::RecoverableTab{
          .tab_id = std::move(tab_id),
          .url = "https://example.com/",
          .title = "Example",
          .workspace_id = "workspace-main",
          .active = true,
      }},
  };
}

}  // namespace

int main() {
  using namespace goreecloud::browser;

  MemorySessionRecoveryStore store;
  SessionRecoveryCoordinator recovery(store);

  SessionCheckpoint crash;
  crash.checkpoint_id = "checkpoint-crash";
  crash.created_unix_ms = 1'788'128'400'000ULL;
  crash.windows = {
      window("normal-window", SessionPrivacyMode::normal, "tab-normal"),
      window("private-window", SessionPrivacyMode::private_window, "tab-private"),
      window("isolated-window", SessionPrivacyMode::isolated_private, "tab-isolated"),
  };

  assert(recovery.mark_unclean_shutdown(crash));
  auto candidate = recovery.latest_candidate();
  assert(candidate.has_value());
  assert(candidate->was_unclean_shutdown);
  assert(candidate->recoverable_window_count == 1);
  assert(candidate->recoverable_tab_count == 1);
  assert(candidate->checkpoint.windows.front().window_id == "normal-window");

  SessionCheckpoint clean = crash;
  clean.checkpoint_id = "checkpoint-clean";
  clean.created_unix_ms += 1;
  assert(recovery.mark_clean_shutdown(clean));
  assert(!recovery.latest_candidate().has_value());

  SessionCheckpoint newer_private_only;
  newer_private_only.checkpoint_id = "checkpoint-private-only";
  newer_private_only.created_unix_ms = clean.created_unix_ms + 1;
  newer_private_only.windows = {
      window("private-only", SessionPrivacyMode::private_window, "private-only-tab"),
  };
  assert(recovery.checkpoint(newer_private_only));
  assert(!recovery.latest_candidate().has_value());

  SessionCheckpoint running;
  running.checkpoint_id = "checkpoint-running";
  running.created_unix_ms = newer_private_only.created_unix_ms + 1;
  running.windows = {
      window("normal-running", SessionPrivacyMode::normal, "running-tab"),
  };
  assert(recovery.checkpoint(running));
  candidate = recovery.latest_candidate();
  assert(candidate.has_value());
  assert(candidate->was_unclean_shutdown);
  assert(candidate->checkpoint.exit_state == SessionExitState::running);

  SessionCheckpoint invalid;
  invalid.created_unix_ms = running.created_unix_ms + 1;
  assert(!recovery.checkpoint(invalid));
  invalid.checkpoint_id = "missing-time";
  invalid.created_unix_ms = 0;
  assert(!recovery.checkpoint(invalid));
  assert(!recovery.discard(""));

  assert(recovery.discard("checkpoint-running"));
  assert(!recovery.latest_candidate().has_value());

  return 0;
}
