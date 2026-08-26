#include <cassert>
#include <optional>
#include <string_view>

#include "goreecloud/browser/sync_retrieval.hpp"

namespace {

class FixtureTransport final : public goreecloud::browser::AuthenticatedSyncRetrievalTransport {
 public:
  std::optional<goreecloud::browser::SyncRetrievalBatch> Fetch(
      std::string_view dataset) const override {
    using goreecloud::browser::SyncEnvelope;
    using goreecloud::browser::SyncRetrievalBatch;

    if (dataset == "browser.tabs") {
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
              .payload_json = R"({"url":"https://example.invalid"})",
          }},
      };
    }
    if (dataset == "browser.history") {
      return SyncRetrievalBatch{.dataset = "browser.history", .records = {}};
    }
    return std::nullopt;
  }
};

class CrossDatasetTransport final : public goreecloud::browser::AuthenticatedSyncRetrievalTransport {
 public:
  std::optional<goreecloud::browser::SyncRetrievalBatch> Fetch(
      std::string_view dataset) const override {
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

}  // namespace

int main() {
  FixtureTransport fixture;
  const auto snapshot = goreecloud::browser::FetchBrowserSyncSnapshot(fixture);
  assert(snapshot.has_value());
  assert(snapshot->tabs.records.size() == 1);
  assert(snapshot->tabs.records.front().record_id == "tab-1");
  assert(snapshot->history.records.empty());

  CrossDatasetTransport cross_dataset;
  assert(!goreecloud::browser::FetchBrowserSyncSnapshot(cross_dataset).has_value());
}
