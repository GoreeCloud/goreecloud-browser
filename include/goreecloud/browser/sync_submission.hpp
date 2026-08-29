#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>

#include "goreecloud/browser/sync_capabilities.hpp"
#include "goreecloud/browser/sync_records.hpp"

namespace goreecloud::browser {

inline constexpr std::size_t kMaxSyncRecordIDBytes = 512;

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

inline const SyncCapability* SyncCapabilityFor(std::string_view dataset,
                                               const std::vector<SyncCapability>& capabilities) {
  return find_sync_capability(capabilities, dataset);
}

// Shape validation is direction-neutral. Operation permissions are enforced by
// the producer/signer/retrieval boundary that uses the envelope.
inline bool ValidateSyncEnvelopeShape(const SyncEnvelope& envelope) {
  const auto capabilities = sync_capabilities();
  const auto* capability = SyncCapabilityFor(envelope.dataset, capabilities);
  if (capability == nullptr || envelope.schema_version != capability->schema_version ||
      envelope.record_id.empty() || envelope.record_id.size() > kMaxSyncRecordIDBytes ||
      envelope.revision == 0 || envelope.updated_at.empty() || envelope.origin_device.empty()) {
    return false;
  }
  // Privacy Shield data minimization: tombstones contain no application data;
  // live records carry an application payload.
  return envelope.deleted ? envelope.payload_json.empty() : !envelope.payload_json.empty();
}

struct SyncProof {
  std::string device_id;
  std::string public_key;
  std::string signature;
};

inline bool ValidateSyncProofForEnvelope(const SyncProof& proof, const SyncEnvelope& envelope) {
  return proof.device_id == envelope.origin_device && !proof.public_key.empty() && !proof.signature.empty();
}

class SyncSigner {
 public:
  virtual ~SyncSigner() = default;
  virtual std::optional<SyncProof> Sign(const SyncEnvelope& envelope) const = 0;
};

inline std::optional<SyncEnvelope> MakeSyncEnvelope(const SyncRecord& record,
                                                    std::uint64_t revision,
                                                    std::string updated_at,
                                                    std::string origin_device) {
  const auto capabilities = sync_capabilities();
  const auto* capability = SyncCapabilityFor(record.dataset, capabilities);
  if (capability == nullptr || !capability->write || record.schema_version != capability->schema_version) {
    return std::nullopt;
  }
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
  const auto capabilities = sync_capabilities();
  const auto* capability = SyncCapabilityFor(dataset, capabilities);
  if (capability == nullptr || !capability->erase) {
    return std::nullopt;
  }
  SyncEnvelope envelope{
      .dataset = std::move(dataset),
      .schema_version = capability->schema_version,
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

// Cryptographic key custody and signature verification are delegated to the
// platform identity/secret layer. Browser still rejects structurally invalid
// signer output before that proof can leave the application boundary.
inline std::optional<SyncProof> SignSyncEnvelope(const SyncEnvelope& envelope,
                                                 const SyncSigner& signer) {
  if (!ValidateSyncEnvelopeShape(envelope)) {
    return std::nullopt;
  }
  const auto capabilities = sync_capabilities();
  const auto* capability = SyncCapabilityFor(envelope.dataset, capabilities);
  if (capability == nullptr || (envelope.deleted ? !capability->erase : !capability->write)) {
    return std::nullopt;
  }
  auto proof = signer.Sign(envelope);
  if (!proof.has_value() || !ValidateSyncProofForEnvelope(*proof, envelope)) {
    return std::nullopt;
  }
  return proof;
}

}  // namespace goreecloud::browser
