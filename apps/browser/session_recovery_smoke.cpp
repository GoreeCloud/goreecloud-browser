#include <algorithm>
#include <cassert>
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

}  // namespace

int main() {
  using namespace goreecloud::browser;

  MemorySessionRecoveryStore store;
  SessionRecoveryCoordinator recovery(store);

  SessionCheckpoint checkpoint;
  checkpoint.checkpoint_id = "checkpoint-001";
  checkpoint.created_unix_ms = 1'788'128'400'000ULL;
  checkpoint.windows = {
      RecoverableWindow{
          .window_id = "normal-window",
          .privacy_mode = SessionPrivacyMode::normal,
          .tabs = {
              RecoverableTab{
                  .tab_id = "tab-1",
                  .url = "https://example.com/",
                  .title = "Example",
                  .workspace_id = "workspace-main",
                  .pinned = true,
                  .active = true,
              },
          },
          .active_tab_id = "tab-1",
      },
      RecoverableWindow{
          .window_id = "private-window",
          .privacy_mode = SessionPrivacyMode::private_window,
          .tabs = {
              RecoverableTab{
                  .tab_id = "private-tab",
                  .url = "https://private.example/",
                  .title = "Private",
                  .workspace_id = "private-workspace",
              },
          },
      },
      RecoverableWindow{
          .window_id = "isolated-private-window",
          .privacy_mode = SessionPrivacyMode::isolated_private,
          .tabs = {
              RecoverableTab{
                  .tab_id = "isolated-tab",
                  .url = "https://isolated.example/",
                  .title = "Isolated",
                  .workspace_id = "isolated-workspace",
              },
          },
      },
  };

  assert(recovery.mark_unclean_shutdown(checkpoint));

  const auto candidate = recovery.latest_candidate();
  assert(candidate.has_value());
  assert(candidate->was_unclean_shutdown);
  assert(candidate->recoverable_window_count == 1);
  assert(candidate->recoverable_tab_count == 1);
  assert(candidate->checkpoint.windows.front().window_id == "normal-window");
  assert(candidate->checkpoint.windows.front().tabs.front().url == "https://example.com/");

  assert(recovery.discard("checkpoint-001"));
  assert(!recovery.latest_candidate().has_value());

  return 0;
}
