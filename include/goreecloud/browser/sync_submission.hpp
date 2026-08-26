#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <utility>

#include "goreecloud/browser/sync_records.hpp"

namespace goreecloud::browser {

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
  if (record.dataset.empty() || record.record_id.empty() || record.payload_json.empty() ||
      revision == 0 || updated_at.empty() || origin_device.empty()) {
    return std::nullopt;
  }
  return SyncEnvelope{
      .dataset = record.dataset,
      .schema_version = record.schema_version,
      .record_id = record.record_id,
      .revision = revision,
      .updated_at = std::move(updated_at),
      .origin_device = std::move(origin_device),
      .deleted = false,
      .payload_json = record.payload_json,
  };
}

// Cryptographic key custody is delegated to the platform identity/secret layer.
// Browser never exposes a private key through this contract.
inline std::optional<SyncProof> SignSyncEnvelope(const SyncEnvelope& envelope,
                                                 const SyncSigner& signer) {
  if (envelope.dataset.empty() || envelope.record_id.empty() || envelope.revision == 0 ||
      envelope.updated_at.empty() || envelope.origin_device.empty()) {
    return std::nullopt;
  }
  return signer.Sign(envelope);
}

}  // namespace goreecloud::browser
