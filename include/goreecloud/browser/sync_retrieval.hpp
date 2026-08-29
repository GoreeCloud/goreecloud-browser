#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "goreecloud/browser/sync_submission.hpp"

namespace goreecloud::browser {

inline constexpr std::size_t kSyncRetrievalPageSize = 256;
inline constexpr std::size_t kMaxSyncRetrievalPages = 4096;
inline constexpr std::size_t kMaxSyncRetrievalRecords = 4096;

struct SyncRetrievalBatch {
  std::string dataset;
  std::vector<SyncEnvelope> records;
  std::string next_after;
};

// The platform transport implementation owns authenticated Sync session state.
// Browser receives only dataset envelopes and opaque continuation cursors;
// credentials never enter record data.
class AuthenticatedSyncRetrievalTransport {
 public:
  virtual ~AuthenticatedSyncRetrievalTransport() = default;
  virtual std::optional<SyncRetrievalBatch> Fetch(std::string_view dataset,
                                                   std::string_view after,
                                                   std::size_t limit) const = 0;
};

inline bool ValidateSyncRetrievalBatch(const SyncRetrievalBatch& batch,
                                       std::string_view expected_dataset,
                                       std::string_view after = {}) {
  if (expected_dataset.empty() || batch.dataset != expected_dataset ||
      after.size() > kMaxSyncRecordIDBytes || batch.next_after.size() > kMaxSyncRecordIDBytes ||
      batch.records.size() > kSyncRetrievalPageSize) {
    return false;
  }

  std::string_view previous_id = after;
  for (const auto& record : batch.records) {
    if (record.dataset != expected_dataset || !ValidateSyncEnvelopeShape(record) ||
        (!previous_id.empty() && record.record_id <= previous_id)) {
      return false;
    }
    previous_id = record.record_id;
  }
  return true;
}

inline std::optional<SyncRetrievalBatch> FetchSyncDataset(
    const AuthenticatedSyncRetrievalTransport& transport,
    std::string_view dataset) {
  SyncRetrievalBatch result{.dataset = std::string(dataset), .records = {}, .next_after = {}};
  std::string after;

  for (std::size_t page = 0; page < kMaxSyncRetrievalPages; ++page) {
    auto batch = transport.Fetch(dataset, after, kSyncRetrievalPageSize);
    if (!batch || !ValidateSyncRetrievalBatch(*batch, dataset, after) ||
        result.records.size() + batch->records.size() > kMaxSyncRetrievalRecords) {
      return std::nullopt;
    }

    result.records.insert(result.records.end(), batch->records.begin(), batch->records.end());
    if (batch->next_after.empty()) {
      return result;
    }
    if (batch->records.empty() || batch->next_after != batch->records.back().record_id ||
        (!after.empty() && batch->next_after <= after)) {
      return std::nullopt;
    }
    after = batch->next_after;
  }
  return std::nullopt;
}

struct BrowserSyncSnapshot {
  SyncRetrievalBatch tabs;
  SyncRetrievalBatch history;
};

inline std::optional<BrowserSyncSnapshot> FetchBrowserSyncSnapshot(
    const AuthenticatedSyncRetrievalTransport& transport) {
  auto tabs = FetchSyncDataset(transport, "browser.tabs");
  if (!tabs) {
    return std::nullopt;
  }
  auto history = FetchSyncDataset(transport, "browser.history");
  if (!history) {
    return std::nullopt;
  }
  return BrowserSyncSnapshot{.tabs = std::move(*tabs), .history = std::move(*history)};
}

}  // namespace goreecloud::browser
