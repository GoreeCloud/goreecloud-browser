#pragma once

#include <optional>
#include <string>

namespace goreecloud::browser {

struct SyncRecord {
  std::string dataset;
  int schema_version{1};
  std::string record_id;
  std::string payload_json;
};

struct TabSyncInput {
  std::string id;
  std::string url;
  std::string title;
  bool private_mode{false};
};

struct HistorySyncInput {
  std::string id;
  std::string url;
  std::string title;
  std::string visited_at;
  bool private_mode{false};
};

// Private/incognito state is structurally excluded from Browser synchronization.
inline std::optional<SyncRecord> MakeTabSyncRecord(const TabSyncInput& tab) {
  if (tab.private_mode || tab.id.empty() || tab.url.empty()) {
    return std::nullopt;
  }
  return SyncRecord{
      .dataset = "browser.tabs",
      .schema_version = 1,
      .record_id = tab.id,
      .payload_json = "{\"url\":\"" + tab.url + "\",\"title\":\"" + tab.title + "\"}",
  };
}

inline std::optional<SyncRecord> MakeHistorySyncRecord(const HistorySyncInput& history) {
  if (history.private_mode || history.id.empty() || history.url.empty() || history.visited_at.empty()) {
    return std::nullopt;
  }
  return SyncRecord{
      .dataset = "browser.history",
      .schema_version = 1,
      .record_id = history.id,
      .payload_json = "{\"url\":\"" + history.url + "\",\"title\":\"" + history.title +
                      "\",\"visitedAt\":\"" + history.visited_at + "\"}",
  };
}

}  // namespace goreecloud::browser
