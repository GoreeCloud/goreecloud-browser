#include <cassert>
#include <optional>
#include <string>
#include <string_view>

#include "goreecloud/browser/sync_retrieval.hpp"

namespace {

class FixtureTransport final : public goreecloud::browser::AuthenticatedSyncRetrievalTransport {
 public:
  std::optional<goreecloud::browser::SyncRetrievalBatch> Fetch(
      std::string_view dataset, std::string_view after, std::size_t limit) const override {
    using goreecloud::browser::SyncEnvelope;
    using goreecloud::browser::SyncRetrievalBatch;

    assert(limit == goreecloud::browser::kSyncRetrievalPageSize);
    if (dataset == "browser.tabs" && after.empty()) {
      return SyncRetrievalBatch{
          .dataset = "browser.tabs",
          .records = {SyncEnvelope{
              .dataset = "browser.tabs",
              .schema_version = 1,
              .record_id = "tab-1",
              .revision = 2,
              .updated_at = "2026-08-26T23:40:00Z",
              .origin_device = "device-1",
              .deleted = false,
              .payload_json = R"({"url":"https://one.invalid"})",
          }},
          .next_after = "tab-1",
      };
    }
    if (dataset == "browser.tabs" && after == "tab-1") {
      return SyncRetrievalBatch{
          .dataset = "browser.tabs",
          .records = {SyncEnvelope{
              .dataset = "browser.tabs",
              .schema_version = 1,
              .record_id = "tab-2",
              .revision = 1,
              .updated_at = "2026-08-26T23:41:00Z",
              .origin_device = "device-1",
              .deleted = false,
              .payload_json = R"({"url":"https://two.invalid"})",
          }},
      };
    }
    if (dataset == "browser.history" && after.empty()) {
      return SyncRetrievalBatch{.dataset = "browser.history", .records = {}};
    }
    return std::nullopt;
  }
};

class CrossDatasetTransport final : public goreecloud::browser::AuthenticatedSyncRetrievalTransport {
 public:
  std::optional<goreecloud::browser::SyncRetrievalBatch> Fetch(
      std::string_view dataset, std::string_view, std::size_t) const override {
    if (dataset == "browser.tabs") {
      return goreecloud::browser::SyncRetrievalBatch{
          .dataset = "browser.tabs",
          .records = {goreecloud::browser::SyncEnvelope{
              .dataset = "search.history",
              .schema_version = 1,
              .record_id = "query-1",
              .revision = 1,
              .updated_at = "2026-08-26T23:40:00Z",
              .origin_device = "device-1",
              .deleted = false,
              .payload_json = "{}",
          }},
      };
    }
    return goreecloud::browser::SyncRetrievalBatch{.dataset = std::string(dataset), .records = {}};
  }
};

class RepeatingContinuationTransport final
    : public goreecloud::browser::AuthenticatedSyncRetrievalTransport {
 public:
  std::optional<goreecloud::browser::SyncRetrievalBatch> Fetch(
      std::string_view dataset, std::string_view, std::size_t) const override {
    return goreecloud::browser::SyncRetrievalBatch{
        .dataset = std::string(dataset),
        .records = {goreecloud::browser::SyncEnvelope{
            .dataset = std::string(dataset),
            .schema_version = 1,
            .record_id = "record-1",
            .revision = 1,
            .updated_at = "2026-08-28T19:00:00Z",
            .origin_device = "device-1",
            .deleted = false,
            .payload_json = "{}",
        }},
        .next_after = "record-1",
    };
  }
};

}  // namespace

int main() {
  using goreecloud::browser::SyncEnvelope;
  using goreecloud::browser::SyncRetrievalBatch;

  const auto capabilities = goreecloud::browser::sync_capabilities();
  const auto* tab_capability =
      goreecloud::browser::find_sync_capability(capabilities, "browser.tabs");
  assert(tab_capability != nullptr);

  FixtureTransport fixture;
  const auto snapshot = goreecloud::browser::FetchBrowserSyncSnapshot(fixture);
  assert(snapshot.has_value());
  assert(snapshot->tabs.records.size() == 2);
  assert(snapshot->tabs.records.front().record_id == "tab-1");
  assert(snapshot->tabs.records.back().record_id == "tab-2");
  assert(snapshot->history.records.empty());

  CrossDatasetTransport cross_dataset;
  assert(!goreecloud::browser::FetchBrowserSyncSnapshot(cross_dataset).has_value());

  RepeatingContinuationTransport repeating;
  assert(!goreecloud::browser::FetchSyncDataset(repeating, "browser.tabs").has_value());

  SyncRetrievalBatch oversized{.dataset = "browser.tabs", .records = {}};
  oversized.records.resize(goreecloud::browser::kSyncRetrievalPageSize + 1);
  assert(!goreecloud::browser::ValidateSyncRetrievalBatch(oversized, "browser.tabs"));

  SyncRetrievalBatch oversized_record_id{
      .dataset = "browser.tabs",
      .records = {SyncEnvelope{
          .dataset = "browser.tabs",
          .schema_version = 1,
          .record_id = std::string(goreecloud::browser::kMaxSyncRecordIDBytes + 1, 'r'),
          .revision = 1,
          .updated_at = "2026-08-28T19:00:00Z",
          .origin_device = "device-1",
          .deleted = false,
          .payload_json = "{}",
      }},
  };
  assert(!goreecloud::browser::ValidateSyncRetrievalBatch(oversized_record_id,
                                                          "browser.tabs"));

  SyncRetrievalBatch oversized_cursor{
      .dataset = "browser.tabs",
      .records = {SyncEnvelope{
          .dataset = "browser.tabs",
          .schema_version = 1,
          .record_id = "tab-3",
          .revision = 1,
          .updated_at = "2026-08-28T19:00:00Z",
          .origin_device = "device-1",
          .deleted = false,
          .payload_json = "{}",
      }},
      .next_after = std::string(goreecloud::browser::kMaxSyncRecordIDBytes + 1, 'c'),
  };
  assert(!goreecloud::browser::ValidateSyncRetrievalBatch(oversized_cursor, "browser.tabs"));

  SyncRetrievalBatch unnegotiated_schema{
      .dataset = "browser.tabs",
      .records = {SyncEnvelope{
          .dataset = "browser.tabs",
          .schema_version = tab_capability->schema_version + 1,
          .record_id = "tab-future",
          .revision = 1,
          .updated_at = "2026-08-28T19:00:00Z",
          .origin_device = "device-1",
          .deleted = false,
          .payload_json = "{}",
      }},
  };
  assert(!goreecloud::browser::ValidateSyncRetrievalBatch(unnegotiated_schema,
                                                          "browser.tabs"));

  SyncRetrievalBatch tombstone_with_payload{
      .dataset = "browser.tabs",
      .records = {SyncEnvelope{
          .dataset = "browser.tabs",
          .schema_version = 1,
          .record_id = "tab-deleted",
          .revision = 3,
          .updated_at = "2026-08-28T19:00:00Z",
          .origin_device = "device-1",
          .deleted = true,
          .payload_json = R"({"url":"https://should-not-survive.invalid"})",
      }},
  };
  assert(!goreecloud::browser::ValidateSyncRetrievalBatch(tombstone_with_payload,
                                                          "browser.tabs"));

  tombstone_with_payload.records.front().payload_json.clear();
  assert(goreecloud::browser::ValidateSyncRetrievalBatch(tombstone_with_payload,
                                                         "browser.tabs"));
}
