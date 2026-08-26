#include <cassert>
#include <memory>
#include <string>

#include "goreecloud/browser/application.hpp"
#include "goreecloud/browser/browser_session_snapshot_provider.hpp"
#include "goreecloud/browser/development_engine.hpp"
#include "goreecloud/browser/runtime_browser_session_state_source.hpp"

int main() {
  using namespace goreecloud::browser;

  BrowserApplication browser(std::make_unique<DevelopmentEngine>());
  browser.initialize();
  assert(browser.first_window());
  auto* normal_window = browser.first_window();
  assert(!normal_window->private_window());
  assert(normal_window->active_tab());

  const auto normal_tab_id = normal_window->active_tab()->id();
  normal_window->navigate_active("https://example.com/runtime");

  auto& private_window = browser.new_private_window("recovery-smoke-private");
  auto& private_tab = private_window.new_tab("https://private.example/runtime");
  (void)private_tab;

  RuntimeBrowserSessionStateSource source(browser);
  source.update_tab_topology(normal_tab_id,
                             "workspace-runtime",
                             "group-runtime",
                             "split-runtime",
                             true);
  source.mark_tab_active(normal_tab_id, 4242);

  const auto windows = source.windows();
  assert(windows.size() == 2);
  assert(windows[0].window_id != windows[1].window_id);

  const auto managed = source.managed_tabs();
  assert(managed.size() == 2);
  bool saw_normal = false;
  for (const auto& tab : managed) {
    if (tab.tab_id != normal_tab_id) continue;
    saw_normal = true;
    assert(tab.window_id == normal_window->window_id());
    assert(tab.workspace_id == "workspace-runtime");
    assert(tab.group_id && *tab.group_id == "group-runtime");
    assert(tab.split_id && *tab.split_id == "split-runtime");
    assert(tab.pinned);
  }
  assert(saw_normal);

  const auto navigation = source.navigation_states();
  bool saw_navigation = false;
  for (const auto& state : navigation) {
    if (state.tab_id != normal_tab_id) continue;
    saw_navigation = true;
    assert(state.url == "https://example.com/runtime");
    assert(state.active);
    assert(state.last_active_unix_ms == 4242);
  }
  assert(saw_navigation);

  BrowserSessionSnapshotProvider snapshots(source);
  const auto checkpoint = snapshots.capture("runtime-checkpoint", 5000);
  assert(checkpoint.windows.size() == 2);

  class MemoryStore final : public SessionRecoveryStore {
   public:
    bool write(const SessionCheckpoint& checkpoint) override {
      last = checkpoint;
      return true;
    }
    [[nodiscard]] std::vector<SessionCheckpoint> read_recent(std::size_t) const override {
      return last.checkpoint_id.empty() ? std::vector<SessionCheckpoint>{}
                                        : std::vector<SessionCheckpoint>{last};
    }
    bool erase(const std::string&) override {
      last = {};
      return true;
    }
    SessionCheckpoint last;
  } store;

  SessionRecoveryCoordinator recovery(store);
  assert(recovery.checkpoint(checkpoint));
  assert(store.last.windows.size() == 1);
  assert(store.last.windows.front().window_id == normal_window->window_id());
  assert(store.last.windows.front().tabs.size() == 1);
  assert(store.last.windows.front().tabs.front().workspace_id == "workspace-runtime");
  assert(store.last.windows.front().tabs.front().group_id == "group-runtime");
  assert(store.last.windows.front().tabs.front().split_id == "split-runtime");
  assert(store.last.windows.front().tabs.front().pinned);

  browser.shutdown();
  return 0;
}
