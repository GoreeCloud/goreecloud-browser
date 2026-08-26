#include <cassert>
#include <string>
#include <vector>

#include "goreecloud/browser/browser_session_snapshot_provider.hpp"

namespace {

class FakeBrowserSessionStateSource final
    : public goreecloud::browser::BrowserSessionStateSource {
 public:
  [[nodiscard]] std::vector<goreecloud::browser::BrowserWindowSessionState> windows() const override {
    return {
        {.window_id = "normal-window",
         .privacy_mode = goreecloud::browser::SessionPrivacyMode::normal,
         .active_tab_id = "tab-2"},
        {.window_id = "private-window",
         .privacy_mode = goreecloud::browser::SessionPrivacyMode::private_window,
         .active_tab_id = "tab-private"},
    };
  }

  [[nodiscard]] std::vector<goreecloud::browser::ManagedTabState> managed_tabs() const override {
    using namespace goreecloud::browser;
    return {
        {.tab_id = "tab-1",
         .window_id = "normal-window",
         .workspace_id = "workspace-main",
         .group_id = "group-research",
         .split_id = "split-primary",
         .pinned = true},
        {.tab_id = "tab-2",
         .window_id = "normal-window",
         .workspace_id = "workspace-main",
         .group_id = std::nullopt,
         .split_id = "split-primary",
         .pinned = false},
        {.tab_id = "tab-private",
         .window_id = "private-window",
         .workspace_id = "workspace-private",
         .group_id = std::nullopt,
         .split_id = std::nullopt,
         .pinned = false},
    };
  }

  [[nodiscard]] std::vector<goreecloud::browser::BrowserTabNavigationState> navigation_states() const override {
    return {
        {.tab_id = "tab-1",
         .url = "https://example.com/research",
         .title = "Research",
         .active = false,
         .last_active_unix_ms = 100},
        {.tab_id = "tab-2",
         .url = "https://example.com/active",
         .title = "Active",
         .active = true,
         .last_active_unix_ms = 200},
        {.tab_id = "tab-private",
         .url = "https://private.example/",
         .title = "Private",
         .active = true,
         .last_active_unix_ms = 300},
    };
  }
};

}  // namespace

int main() {
  using namespace goreecloud::browser;

  FakeBrowserSessionStateSource source;
  BrowserSessionSnapshotProvider provider(source);
  auto checkpoint = provider.capture("live-001", 500);

  assert(checkpoint.checkpoint_id == "live-001");
  assert(checkpoint.created_unix_ms == 500);
  assert(checkpoint.windows.size() == 2);
  assert(checkpoint.windows[0].window_id == "normal-window");
  assert(checkpoint.windows[0].tabs.size() == 2);

  const auto& first = checkpoint.windows[0].tabs[0].tab_id == "tab-1"
                          ? checkpoint.windows[0].tabs[0]
                          : checkpoint.windows[0].tabs[1];
  assert(first.workspace_id == "workspace-main");
  assert(first.group_id && *first.group_id == "group-research");
  assert(first.split_id && *first.split_id == "split-primary");
  assert(first.pinned);

  const auto& second = checkpoint.windows[0].tabs[0].tab_id == "tab-2"
                           ? checkpoint.windows[0].tabs[0]
                           : checkpoint.windows[0].tabs[1];
  assert(second.active);
  assert(second.url == "https://example.com/active");

  class MemoryStore final : public SessionRecoveryStore {
   public:
    bool write(const SessionCheckpoint& value) override {
      saved = value;
      return true;
    }
    [[nodiscard]] std::vector<SessionCheckpoint> read_recent(std::size_t) const override {
      return saved.checkpoint_id.empty() ? std::vector<SessionCheckpoint>{}
                                         : std::vector<SessionCheckpoint>{saved};
    }
    bool erase(const std::string&) override { return true; }
    SessionCheckpoint saved;
  } store;

  SessionRecoveryCoordinator coordinator(store);
  assert(coordinator.checkpoint(checkpoint));
  assert(store.saved.windows.size() == 1);
  assert(store.saved.windows.front().window_id == "normal-window");
  assert(store.saved.windows.front().tabs.size() == 2);

  return 0;
}
