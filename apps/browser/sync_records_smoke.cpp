#include <cassert>

#include "goreecloud/browser/sync_records.hpp"

using goreecloud::browser::HistorySyncInput;
using goreecloud::browser::MakeHistorySyncRecord;
using goreecloud::browser::MakeTabSyncRecord;
using goreecloud::browser::TabSyncInput;

int main() {
  auto tab = MakeTabSyncRecord(TabSyncInput{
      .id = "tab-1", .url = "https://goreecloud.com", .title = "GoreeCloud", .private_mode = false});
  assert(tab.has_value());
  assert(tab->dataset == "browser.tabs");

  auto private_tab = MakeTabSyncRecord(TabSyncInput{
      .id = "tab-private", .url = "https://private.example", .title = "Private", .private_mode = true});
  assert(!private_tab.has_value());

  auto history = MakeHistorySyncRecord(HistorySyncInput{
      .id = "history-1",
      .url = "https://goreecloud.com/search",
      .title = "Search",
      .visited_at = "2026-08-26T22:00:00Z",
      .private_mode = false});
  assert(history.has_value());
  assert(history->dataset == "browser.history");

  auto private_history = MakeHistorySyncRecord(HistorySyncInput{
      .id = "history-private",
      .url = "https://private.example",
      .title = "Private",
      .visited_at = "2026-08-26T22:00:00Z",
      .private_mode = true});
  assert(!private_history.has_value());
}
