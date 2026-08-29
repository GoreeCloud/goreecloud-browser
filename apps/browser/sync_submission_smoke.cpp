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

int main() {
  auto record = MakeTabSyncRecord(TabSyncInput{
      .id = "tab-1", .url = "https://goreecloud.com", .title = "GoreeCloud", .private_mode = false});
  assert(record.has_value());

  auto envelope = MakeSyncEnvelope(*record, 7, "2026-08-26T23:00:00Z", "device-browser");
  assert(envelope.has_value());
  assert(envelope->dataset == "browser.tabs");
  assert(envelope->revision == 7);
  assert(!envelope->deleted);

  FakeSigner signer;
  auto proof = SignSyncEnvelope(*envelope, signer);
  assert(proof.has_value());
  assert(proof->device_id == "device-browser");

  auto oversized_record = *record;
  oversized_record.record_id = std::string(kMaxSyncRecordIDBytes + 1, 'r');
  assert(!MakeSyncEnvelope(oversized_record, 8, "2026-08-26T23:01:00Z", "device-browser")
              .has_value());

  auto oversized_envelope = *envelope;
  oversized_envelope.record_id = std::string(kMaxSyncRecordIDBytes + 1, 'r');
  assert(!SignSyncEnvelope(oversized_envelope, signer).has_value());

  auto private_record = MakeTabSyncRecord(TabSyncInput{
      .id = "private", .url = "https://private.example", .title = "Private", .private_mode = true});
  assert(!private_record.has_value());
}
