#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

#include "goreecloud/browser/session_recovery_startup.hpp"

namespace {

class MemoryStore final : public goreecloud::browser::SessionRecoveryStore {
 public:
  bool write(const goreecloud::browser::SessionCheckpoint& checkpoint) override {
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

}  // namespace

int main() {
  using namespace goreecloud::browser;

  MemoryStore store;
  SessionRecoveryCoordinator coordinator(store);

  SessionCheckpoint checkpoint;
  checkpoint.checkpoint_id = "crash-001";
  checkpoint.windows = {
      RecoverableWindow{
          .window_id = "window-1",
          .privacy_mode = SessionPrivacyMode::normal,
          .tabs = {RecoverableTab{.tab_id = "tab-1", .url = "https://example.com/"}},
      },
      RecoverableWindow{
          .window_id = "private-window",
          .privacy_mode = SessionPrivacyMode::private_window,
          .tabs = {RecoverableTab{.tab_id = "private-tab", .url = "https://private.example/"}},
      },
  };
  assert(coordinator.mark_unclean_shutdown(checkpoint));

  SessionRecoveryStartupController startup(coordinator);
  const auto offer = startup.evaluate();
  assert(offer.action == RecoveryStartupAction::offer_restore);
  assert(offer.checkpoint_id == "crash-001");
  assert(offer.window_count == 1);
  assert(offer.tab_count == 1);
  assert(offer.window_ids.size() == 1);
  assert(offer.window_ids.front() == "window-1");

  SessionRecoveryStartupController automatic(
      coordinator,
      RecoveryStartupPolicy{
          .offer_after_unclean_shutdown = true,
          .auto_restore_after_unclean_shutdown = true,
      });
  assert(automatic.evaluate().action == RecoveryStartupAction::restore_automatically);

  assert(startup.discard(offer));
  assert(startup.evaluate().action == RecoveryStartupAction::none);
  return 0;
}
