#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "goreecloud/browser/sync_submission.hpp"

namespace goreecloud::browser {

struct SyncRetrievalBatch {
  std::string dataset;
  std::vector<SyncEnvelope> records;
};

// The platform transport implementation owns authenticated Sync session state.
// Browser receives only dataset envelopes; credentials never enter record data.
class AuthenticatedSyncRetrievalTransport {
 public:
  virtual ~AuthenticatedSyncRetrievalTransport() = default;
  virtual std::optional<SyncRetrievalBatch> Fetch(std::string_view dataset) const = 0;
};

inline bool ValidateSyncRetrievalBatch(const SyncRetrievalBatch& batch,
                                       std::string_view expected_dataset) {
  if (expected_dataset.empty() || batch.dataset != expected_dataset) {
    return false;
  }
  for (const auto& record : batch.records) {
    if (record.dataset != expected_dataset || record.schema_version < 1 ||
        record.record_id.empty() || record.revision == 0 || record.updated_at.empty() ||
        record.origin_device.empty()) {
      return false;
    }
    if (!record.deleted && record.payload_json.empty()) {
      return false;
    }
  }
  return true;
}

struct BrowserSyncSnapshot {
  SyncRetrievalBatch tabs;
  SyncRetrievalBatch history;
};

inline std::optional<BrowserSyncSnapshot> FetchBrowserSyncSnapshot(
    const AuthenticatedSyncRetrievalTransport& transport) {
  auto tabs = transport.Fetch("browser.tabs");
  if (!tabs || !ValidateSyncRetrievalBatch(*tabs, "browser.tabs")) {
    return std::nullopt;
  }
  auto history = transport.Fetch("browser.history");
  if (!history || !ValidateSyncRetrievalBatch(*history, "browser.history")) {
    return std::nullopt;
  }
  return BrowserSyncSnapshot{.tabs = std::move(*tabs), .history = std::move(*history)};
}

}  // namespace goreecloud::browser
