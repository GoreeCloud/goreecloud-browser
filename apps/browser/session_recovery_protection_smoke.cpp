#include <cassert>
#include <optional>
#include <string>
#include <string_view>

#include "goreecloud/browser/session_recovery_protection.hpp"

namespace {

class FakeAuthenticatedProtector final
    : public goreecloud::browser::SessionRecoveryPayloadProtector {
 public:
  [[nodiscard]] bool authenticated_encryption() const noexcept override { return true; }

  [[nodiscard]] std::optional<goreecloud::browser::ProtectedRecoveryPayload> protect(
      std::string_view purpose,
      std::string_view plaintext) override {
    if (purpose != "browser.session-recovery.v1" || plaintext.empty()) return std::nullopt;
    return goreecloud::browser::ProtectedRecoveryPayload{
        .algorithm = "TEST-AEAD",
        .key_id = "test-key-1",
        .nonce = "nonce-1",
        .ciphertext = std::string{plaintext.rbegin(), plaintext.rend()},
        .authentication_tag = "test-tag",
    };
  }

  [[nodiscard]] std::optional<std::string> unprotect(
      std::string_view purpose,
      const goreecloud::browser::ProtectedRecoveryPayload& payload) override {
    if (purpose != "browser.session-recovery.v1" || payload.authentication_tag != "test-tag") {
      return std::nullopt;
    }
    return std::string{payload.ciphertext.rbegin(), payload.ciphertext.rend()};
  }
};

}  // namespace

int main() {
  using namespace goreecloud::browser;

  UnavailableSessionRecoveryPayloadProtector unavailable;
  SessionRecoveryProtectionBoundary blocked(&unavailable);
  assert(!blocked.protected_at_rest_available());
  assert(!blocked.persistence_allowed());
  assert(!blocked.protect("browser.session-recovery.v1", "payload"));

  SessionRecoveryProtectionBoundary dev(
      nullptr,
      SessionRecoveryProtectionPolicy{
          .require_authenticated_encryption = false,
          .allow_unprotected_development_storage = true,
      });
  assert(dev.persistence_allowed());
  assert(!dev.protected_at_rest_available());

  FakeAuthenticatedProtector authenticated;
  SessionRecoveryProtectionBoundary boundary(&authenticated);
  assert(boundary.protected_at_rest_available());
  assert(boundary.persistence_allowed());

  const auto protected_payload =
      boundary.protect("browser.session-recovery.v1", "session-state");
  assert(protected_payload);
  assert(protected_payload->algorithm == "TEST-AEAD");
  assert(protected_payload->key_id == "test-key-1");

  const auto plaintext =
      boundary.unprotect("browser.session-recovery.v1", *protected_payload);
  assert(plaintext && *plaintext == "session-state");

  auto tampered = *protected_payload;
  tampered.authentication_tag = "tampered";
  assert(!boundary.unprotect("browser.session-recovery.v1", tampered));

  return 0;
}
