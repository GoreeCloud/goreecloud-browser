#pragma once

#include <optional>
#include <string>
#include <string_view>

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

inline std::string EscapeSyncJson(std::string_view value) {
  std::string out;
  out.reserve(value.size());
  for (const char ch : value) {
    switch (ch) {
      case '\\': out += "\\\\"; break;
      case '"': out += "\\\""; break;
      case '\n': out += "\\n"; break;
      case '\r': out += "\\r"; break;
      case '\t': out += "\\t"; break;
      default: out += ch; break;
    }
  }
  return out;
}

// Private/incognito state is structurally excluded from Browser synchronization.
inline std::optional<SyncRecord> MakeTabSyncRecord(const TabSyncInput& tab) {
  if (tab.private_mode || tab.id.empty() || tab.url.empty()) {
    return std::nullopt;
  }
  return SyncRecord{
      .dataset = "browser.tabs",
      .schema_version = 1,
      .record_id = tab.id,
      .payload_json = "{\"url\":\"" + EscapeSyncJson(tab.url) + "\",\"title\":\"" +
                      EscapeSyncJson(tab.title) + "\"}",
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
      .payload_json = "{\"url\":\"" + EscapeSyncJson(history.url) + "\",\"title\":\"" +
                      EscapeSyncJson(history.title) + "\",\"visitedAt\":\"" +
                      EscapeSyncJson(history.visited_at) + "\"}",
  };
}

}  // namespace goreecloud::browser
