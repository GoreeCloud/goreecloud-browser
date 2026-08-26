#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

#include "goreecloud/browser/session_recovery_lifecycle.hpp"

namespace {

class MemoryStore final : public goreecloud::browser::SessionRecoveryStore {
 public:
  bool write(const goreecloud::browser::SessionCheckpoint& checkpoint) override {
    checkpoints_.erase(
        std::remove_if(checkpoints_.begin(), checkpoints_.end(),
                       [&](const auto& item) { return item.checkpoint_id == checkpoint.checkpoint_id; }),
        checkpoints_.end());
    checkpoints_.insert(checkpoints_.begin(), checkpoint);
    return true;
  }

  [[nodiscard]] std::vector<goreecloud::browser::SessionCheckpoint> read_recent(
      std::size_t limit) const override {
    const auto count = std::min(limit, checkpoints_.size());
    return {checkpoints_.begin(), checkpoints_.begin() + static_cast<std::ptrdiff_t>(count)};
  }

  bool erase(const std::string& id) override {
    const auto before = checkpoints_.size();
    checkpoints_.erase(
        std::remove_if(checkpoints_.begin(), checkpoints_.end(),
                       [&](const auto& item) { return item.checkpoint_id == id; }),
        checkpoints_.end());
    return checkpoints_.size() != before;
  }

 private:
  std::vector<goreecloud::browser::SessionCheckpoint> checkpoints_;
};

class SnapshotProvider final : public goreecloud::browser::SessionSnapshotProvider {
 public:
  [[nodiscard]] goreecloud::browser::SessionCheckpoint capture(
      std::string checkpoint_id, std::uint64_t unix_ms) const override {
    using namespace goreecloud::browser;
    SessionCheckpoint checkpoint;
    checkpoint.checkpoint_id = std::move(checkpoint_id);
    checkpoint.created_unix_ms = unix_ms;
    checkpoint.windows = {
        RecoverableWindow{
            .window_id = "normal",
            .privacy_mode = SessionPrivacyMode::normal,
            .tabs = {RecoverableTab{
                .tab_id = "tab-1",
                .url = "https://example.com/",
                .title = "Example",
                .workspace_id = "main",
            }},
        },
        RecoverableWindow{
            .window_id = "private",
            .privacy_mode = SessionPrivacyMode::private_window,
            .tabs = {RecoverableTab{
                .tab_id = "private-tab",
                .url = "https://private.example/",
                .title = "Private",
                .workspace_id = "private",
            }},
        },
    };
    return checkpoint;
  }
};

}  // namespace

int main() {
  using namespace goreecloud::browser;

  MemoryStore store;
  SessionRecoveryCoordinator recovery(store);
  SnapshotProvider snapshots;
  SessionRecoveryLifecycleController lifecycle(
      recovery, snapshots,
      SessionRecoveryLifecyclePolicy{.checkpoint_interval_ms = 1'000});

  assert(lifecycle.begin("session-001", 10'000));
  assert(lifecycle.active());
  auto candidate = recovery.latest_candidate();
  assert(candidate.has_value());
  assert(candidate->recoverable_window_count == 1);
  assert(candidate->recoverable_tab_count == 1);

  assert(lifecycle.checkpoint_if_due(10'500));
  assert(lifecycle.checkpoint_if_due(11'000));
  assert(lifecycle.checkpoint_now(11'250));
  assert(lifecycle.clean_shutdown(12'000));
  assert(!lifecycle.active());

  candidate = recovery.latest_candidate();
  assert(candidate.has_value());
  assert(!candidate->was_unclean_shutdown);
  assert(candidate->recoverable_window_count == 1);
  return 0;
}
