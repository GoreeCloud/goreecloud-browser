#include <cassert>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>

#include "goreecloud/browser/file_session_recovery_store.hpp"

namespace {

class FakeAuthenticatedProtector final
    : public goreecloud::browser::SessionRecoveryPayloadProtector {
 public:
  [[nodiscard]] bool authenticated_encryption() const noexcept override { return true; }

  [[nodiscard]] std::optional<goreecloud::browser::ProtectedRecoveryPayload> protect(
      std::string_view purpose,
      std::string_view plaintext) override {
    if (purpose != goreecloud::browser::FileSessionRecoveryStore::kProtectionPurpose ||
        plaintext.empty()) {
      return std::nullopt;
    }
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
    if (purpose != goreecloud::browser::FileSessionRecoveryStore::kProtectionPurpose ||
        payload.algorithm != "TEST-AEAD" || payload.key_id != "test-key-1" ||
        payload.nonce != "nonce-1" || payload.authentication_tag != "test-tag") {
      return std::nullopt;
    }
    return std::string{payload.ciphertext.rbegin(), payload.ciphertext.rend()};
  }
};

std::string read_all(const std::filesystem::path& path) {
  std::ifstream in(path, std::ios::binary);
  return {std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
}

}  // namespace

int main() {
  using namespace goreecloud::browser;

  const auto root = std::filesystem::temp_directory_path() /
                    "goreecloud-browser-protected-recovery-store-smoke";
  std::error_code error;
  std::filesystem::remove_all(root, error);

  UnavailableSessionRecoveryPayloadProtector unavailable;
  SessionRecoveryProtectionBoundary blocked(&unavailable);
  FileSessionRecoveryStore blocked_store(root / "blocked", blocked);
  SessionCheckpoint blocked_checkpoint;
  blocked_checkpoint.checkpoint_id = "blocked";
  assert(!blocked_store.write(blocked_checkpoint));
  assert(!std::filesystem::exists(root / "blocked" / "blocked.gcrs"));

  FakeAuthenticatedProtector protector;
  SessionRecoveryProtectionBoundary protection(&protector);
  FileSessionRecoveryStore store(root / "protected", protection);
  SessionRecoveryCoordinator coordinator(store);

  SessionCheckpoint checkpoint;
  checkpoint.checkpoint_id = "protected-001";
  checkpoint.created_unix_ms = 4242;
  checkpoint.windows = {RecoverableWindow{
      .window_id = "normal-window",
      .privacy_mode = SessionPrivacyMode::normal,
      .tabs = {RecoverableTab{
          .tab_id = "tab-1",
          .url = "https://secret.example/recovery",
          .title = "Sensitive title",
          .workspace_id = "workspace-main",
          .active = true,
          .last_active_unix_ms = 4200,
      }},
      .active_tab_id = "tab-1",
  }};

  assert(coordinator.mark_unclean_shutdown(checkpoint));
  const auto path = root / "protected" / "protected-001.gcrs";
  assert(std::filesystem::exists(path));

  const auto persisted = read_all(path);
  assert(persisted.starts_with("GCRS2\n"));
  assert(persisted.find("https://secret.example/recovery") == std::string::npos);
  assert(persisted.find("Sensitive title") == std::string::npos);
  assert(persisted.find("workspace-main") == std::string::npos);

  const auto recovered = store.read_recent(1);
  assert(recovered.size() == 1);
  assert(recovered.front().checkpoint_id == "protected-001");
  assert(recovered.front().windows.front().tabs.front().url ==
         "https://secret.example/recovery");

  auto tampered = persisted;
  const auto tag_line_start = tampered.find('\n', tampered.find('\n', tampered.find('\n', tampered.find('\n') + 1) + 1) + 1) + 1;
  assert(tag_line_start != std::string::npos && tag_line_start + 1 < tampered.size());
  tampered[tag_line_start + 1] = tampered[tag_line_start + 1] == '0' ? '1' : '0';
  {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    out.write(tampered.data(), static_cast<std::streamsize>(tampered.size()));
  }
  assert(store.read_recent(1).empty());

  std::filesystem::remove_all(root, error);
  return 0;
}
