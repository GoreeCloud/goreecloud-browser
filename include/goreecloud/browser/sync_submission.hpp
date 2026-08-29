#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "goreecloud/browser/sync_records.hpp"

namespace goreecloud::browser {

inline constexpr std::size_t kMaxSyncRecordIDBytes = 512;
inline constexpr int kBrowserSyncSchemaVersion = 1;

struct SyncEnvelope {
  std::string dataset;
  int schema_version{1};
  std::string record_id;
  std::uint64_t revision{0};
  std::string updated_at;
  std::string origin_device;
  bool deleted{false};
  std::string payload_json;
};

inline bool IsBrowserSyncDataset(std::string_view dataset) {
  return dataset == "browser.tabs" || dataset == "browser.history" ||
         dataset == "browser.preferences";
}

inline bool IsBrowserSyncDeleteCapableDataset(std::string_view dataset) {
  return dataset == "browser.tabs" || dataset == "browser.history";
}

// Browser accepts only its currently negotiated schema and preserves Privacy
// Shield data minimization by requiring payload-free tombstones.
inline bool ValidateSyncEnvelopeShape(const SyncEnvelope& envelope) {
  if (!IsBrowserSyncDataset(envelope.dataset) ||
      envelope.schema_version != kBrowserSyncSchemaVersion || envelope.record_id.empty() ||
      envelope.record_id.size() > kMaxSyncRecordIDBytes || envelope.revision == 0 ||
      envelope.updated_at.empty() || envelope.origin_device.empty()) {
    return false;
  }
  if (envelope.deleted) {
    return IsBrowserSyncDeleteCapableDataset(envelope.dataset) && envelope.payload_json.empty();
  }
  return !envelope.payload_json.empty();
}

struct SyncProof {
  std::string device_id;
  std::string public_key;
  std::string signature;
};

class SyncSigner {
 public:
  virtual ~SyncSigner() = default;
  virtual std::optional<SyncProof> Sign(const SyncEnvelope& envelope) const = 0;
};

inline std::optional<SyncEnvelope> MakeSyncEnvelope(const SyncRecord& record,
                                                    std::uint64_t revision,
                                                    std::string updated_at,
                                                    std::string origin_device) {
  SyncEnvelope envelope{
      .dataset = record.dataset,
      .schema_version = record.schema_version,
      .record_id = record.record_id,
      .revision = revision,
      .updated_at = std::move(updated_at),
      .origin_device = std::move(origin_device),
      .deleted = false,
      .payload_json = record.payload_json,
  };
  if (!ValidateSyncEnvelopeShape(envelope)) {
    return std::nullopt;
  }
  return envelope;
}

inline std::optional<SyncEnvelope> MakeSyncTombstone(std::string dataset,
                                                     std::string record_id,
                                                     std::uint64_t revision,
                                                     std::string updated_at,
                                                     std::string origin_device) {
  SyncEnvelope envelope{
      .dataset = std::move(dataset),
      .schema_version = kBrowserSyncSchemaVersion,
      .record_id = std::move(record_id),
      .revision = revision,
      .updated_at = std::move(updated_at),
      .origin_device = std::move(origin_device),
      .deleted = true,
      .payload_json = {},
  };
  if (!ValidateSyncEnvelopeShape(envelope)) {
    return std::nullopt;
  }
  return envelope;
}

// Cryptographic key custody is delegated to the platform identity/secret layer.
// Browser never exposes a private key through this contract.
inline std::optional<SyncProof> SignSyncEnvelope(const SyncEnvelope& envelope,
                                                 const SyncSigner& signer) {
  if (!ValidateSyncEnvelopeShape(envelope)) {
    return std::nullopt;
  }
  return signer.Sign(envelope);
}

}  // namespace goreecloud::browser
