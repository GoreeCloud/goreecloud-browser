#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>

#include "goreecloud/browser/file_session_recovery_store.hpp"

int main() {
  using namespace goreecloud::browser;
  const auto root = std::filesystem::temp_directory_path() / "goreecloud-browser-recovery-store-smoke";
  std::error_code error;
  std::filesystem::remove_all(root, error);

  FileSessionRecoveryStore store(root, {.max_checkpoints = 2});
  SessionRecoveryCoordinator coordinator(store);

  SessionCheckpoint first;
  first.checkpoint_id = "checkpoint-001";
  first.created_unix_ms = 100;
  first.windows = {RecoverableWindow{
      .window_id = "normal-window",
      .privacy_mode = SessionPrivacyMode::normal,
      .tabs = {RecoverableTab{
          .tab_id = "tab-1",
          .url = "https://example.com/a",
          .title = "Example A",
          .workspace_id = "workspace-main",
          .group_id = std::nullopt,
          .split_id = std::nullopt,
          .pinned = true,
          .active = true,
          .last_active_unix_ms = 99,
      }},
      .active_tab_id = "tab-1",
  }};
  assert(coordinator.mark_unclean_shutdown(first));

  auto recent = store.read_recent(5);
  assert(recent.size() == 1);
  assert(recent.front().checkpoint_id == "checkpoint-001");
  assert(recent.front().windows.size() == 1);
  assert(recent.front().windows.front().tabs.front().url == "https://example.com/a");

  SessionCheckpoint second = first;
  second.checkpoint_id = "checkpoint-002";
  second.created_unix_ms = 200;
  assert(coordinator.checkpoint(second));

  SessionCheckpoint third = first;
  third.checkpoint_id = "checkpoint-003";
  third.created_unix_ms = 300;
  assert(coordinator.checkpoint(third));
  assert(store.read_recent(5).size() == 2);

  const auto corrupt = root / "checkpoint-003.gcrs";
  if (std::filesystem::exists(corrupt)) {
    std::ofstream out(corrupt, std::ios::binary | std::ios::app);
    assert(out);
    out << "corruption";
    out.close();
    recent = store.read_recent(5);
    for (const auto& checkpoint : recent) {
      assert(checkpoint.checkpoint_id != "checkpoint-003");
    }
  }

  assert(store.erase("checkpoint-002") || !std::filesystem::exists(root / "checkpoint-002.gcrs"));
  std::filesystem::remove_all(root, error);
  return 0;
}
