#include <cassert>
#include <optional>
#include <string>

#include "goreecloud/browser/sync_submission.hpp"

using namespace goreecloud::browser;

class FakeSigner final : public SyncSigner {
 public:
  std::optional<SyncProof> Sign(const SyncEnvelope& envelope) const override {
    if (envelope.origin_device != "device-browser") return std::nullopt;
    return SyncProof{.device_id = envelope.origin_device,
                     .public_key = "platform-public-key",
                     .signature = "platform-signature"};
  }
};

class WrongDeviceSigner final : public SyncSigner {
 public:
  std::optional<SyncProof> Sign(const SyncEnvelope&) const override {
    return SyncProof{.device_id = "other-device",
                     .public_key = "platform-public-key",
                     .signature = "platform-signature"};
  }
};

int main() {
  const auto capabilities = sync_capabilities();
  const auto* tab_capability = find_sync_capability(capabilities, "browser.tabs");
  assert(tab_capability != nullptr);

  auto record = MakeTabSyncRecord(TabSyncInput{
      .id = "tab-1", .url = "https://goreecloud.com", .title = "GoreeCloud", .private_mode = false});
  assert(record.has_value());

  auto envelope = MakeSyncEnvelope(*record, 7, "2026-08-26T23:00:00Z", "device-browser");
  assert(envelope.has_value());
  assert(envelope->dataset == "browser.tabs");
  assert(envelope->schema_version == tab_capability->schema_version);
  assert(envelope->revision == 7);
  assert(!envelope->deleted);

  FakeSigner signer;
  auto proof = SignSyncEnvelope(*envelope, signer);
  assert(proof.has_value());
  assert(proof->device_id == "device-browser");
  assert(ValidateSyncProofForEnvelope(*proof, *envelope));

  WrongDeviceSigner wrong_device_signer;
  assert(!SignSyncEnvelope(*envelope, wrong_device_signer).has_value());
  assert(!ValidateSyncProofForEnvelope(
      SyncProof{.device_id = envelope->origin_device, .public_key = "", .signature = "platform-signature"},
      *envelope));
  assert(!ValidateSyncProofForEnvelope(
      SyncProof{.device_id = envelope->origin_device, .public_key = "platform-public-key", .signature = ""},
      *envelope));

  auto tombstone = MakeSyncTombstone(
      "browser.tabs", "tab-deleted", 8, "2026-08-26T23:01:00Z", "device-browser");
  assert(tombstone.has_value());
  assert(tombstone->deleted);
  assert(tombstone->payload_json.empty());
  assert(SignSyncEnvelope(*tombstone, signer).has_value());

  assert(!MakeSyncTombstone("browser.preferences", "prefs", 1,
                            "2026-08-26T23:01:00Z", "device-browser")
              .has_value());
  assert(!MakeSyncTombstone("browser.tabs", std::string(kMaxSyncRecordIDBytes + 1, 'r'), 1,
                            "2026-08-26T23:01:00Z", "device-browser")
              .has_value());

  auto oversized_record = *record;
  oversized_record.record_id = std::string(kMaxSyncRecordIDBytes + 1, 'r');
  assert(!MakeSyncEnvelope(oversized_record, 8, "2026-08-26T23:01:00Z", "device-browser")
              .has_value());

  auto unnegotiated_record = *record;
  unnegotiated_record.schema_version = tab_capability->schema_version + 1;
  assert(!MakeSyncEnvelope(unnegotiated_record, 8, "2026-08-26T23:01:00Z", "device-browser")
              .has_value());

  auto oversized_envelope = *envelope;
  oversized_envelope.record_id = std::string(kMaxSyncRecordIDBytes + 1, 'r');
  assert(!SignSyncEnvelope(oversized_envelope, signer).has_value());

  auto unnegotiated_envelope = *envelope;
  unnegotiated_envelope.schema_version = tab_capability->schema_version + 1;
  assert(!SignSyncEnvelope(unnegotiated_envelope, signer).has_value());

  auto tombstone_with_payload = *tombstone;
  tombstone_with_payload.payload_json = "{}";
  assert(!SignSyncEnvelope(tombstone_with_payload, signer).has_value());

  auto private_record = MakeTabSyncRecord(TabSyncInput{
      .id = "private", .url = "https://private.example", .title = "Private", .private_mode = true});
  assert(!private_record.has_value());
}
