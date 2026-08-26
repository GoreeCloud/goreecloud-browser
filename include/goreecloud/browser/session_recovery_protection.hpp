#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace goreecloud::browser {

struct ProtectedRecoveryPayload {
  std::string algorithm;
  std::string key_id;
  std::string nonce;
  std::string ciphertext;
  std::string authentication_tag;
};

class SessionRecoveryPayloadProtector {
 public:
  virtual ~SessionRecoveryPayloadProtector() = default;

  [[nodiscard]] virtual bool authenticated_encryption() const noexcept = 0;
  [[nodiscard]] virtual std::optional<ProtectedRecoveryPayload> protect(
      std::string_view purpose,
      std::string_view plaintext) = 0;
  [[nodiscard]] virtual std::optional<std::string> unprotect(
      std::string_view purpose,
      const ProtectedRecoveryPayload& payload) = 0;
};

struct SessionRecoveryProtectionPolicy {
  bool require_authenticated_encryption{true};
  bool allow_unprotected_development_storage{false};
};

class SessionRecoveryProtectionBoundary {
 public:
  SessionRecoveryProtectionBoundary(SessionRecoveryPayloadProtector* protector,
                                    SessionRecoveryProtectionPolicy policy = {})
      : protector_(protector), policy_(std::move(policy)) {}

  [[nodiscard]] bool protected_at_rest_available() const noexcept {
    return protector_ != nullptr && protector_->authenticated_encryption();
  }

  [[nodiscard]] bool persistence_allowed() const noexcept {
    if (protected_at_rest_available()) return true;
    return !policy_.require_authenticated_encryption &&
           policy_.allow_unprotected_development_storage;
  }

  [[nodiscard]] std::optional<ProtectedRecoveryPayload> protect(
      std::string_view purpose,
      std::string_view plaintext) const {
    if (!protector_ || !protector_->authenticated_encryption()) return std::nullopt;
    auto payload = protector_->protect(purpose, plaintext);
    if (!payload || payload->algorithm.empty() || payload->key_id.empty() ||
        payload->nonce.empty() || payload->ciphertext.empty() ||
        payload->authentication_tag.empty()) {
      return std::nullopt;
    }
    return payload;
  }

  [[nodiscard]] std::optional<std::string> unprotect(
      std::string_view purpose,
      const ProtectedRecoveryPayload& payload) const {
    if (!protector_ || !protector_->authenticated_encryption()) return std::nullopt;
    if (payload.algorithm.empty() || payload.key_id.empty() || payload.nonce.empty() ||
        payload.ciphertext.empty() || payload.authentication_tag.empty()) {
      return std::nullopt;
    }
    return protector_->unprotect(purpose, payload);
  }

 private:
  SessionRecoveryPayloadProtector* protector_{nullptr};
  SessionRecoveryProtectionPolicy policy_;
};

class UnavailableSessionRecoveryPayloadProtector final
    : public SessionRecoveryPayloadProtector {
 public:
  [[nodiscard]] bool authenticated_encryption() const noexcept override { return false; }

  [[nodiscard]] std::optional<ProtectedRecoveryPayload> protect(
      std::string_view,
      std::string_view) override {
    return std::nullopt;
  }

  [[nodiscard]] std::optional<std::string> unprotect(
      std::string_view,
      const ProtectedRecoveryPayload&) override {
    return std::nullopt;
  }
};

}  // namespace goreecloud::browser
