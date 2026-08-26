#pragma once

#include <string>
#include <vector>

namespace goreecloud::browser {

enum class VaultAuthority {
  local,
  goreecloud,
};

enum class VaultItemKind {
  password,
  passkey,
  credential,
  secure_note,
  proxy_credential,
  token,
  secret,
};

struct VaultItemDescriptor {
  std::string id;
  VaultAuthority authority{VaultAuthority::local};
  VaultItemKind kind{VaultItemKind::password};
  std::string display_name;
};

struct VaultSaveRequest {
  VaultAuthority destination{VaultAuthority::local};
  VaultItemKind kind{VaultItemKind::password};
  bool explicit_user_choice{false};
};

inline constexpr bool kLocalVaultSynchronizes = false;
inline constexpr bool kGoreeCloudVaultSynchronizes = true;
inline constexpr bool kSilentLocalToGoreeCloudMigrationAllowed = false;
inline constexpr bool kSecretsAllowedInBrowserPreferences = false;

class UnifiedVault {
 public:
  virtual ~UnifiedVault() = default;

  [[nodiscard]] virtual std::vector<VaultItemDescriptor> list(
      VaultAuthority authority) const = 0;
  virtual bool save(const VaultSaveRequest& request) = 0;
  virtual bool remove(const std::string& id, VaultAuthority authority) = 0;
  virtual bool move(const std::string& id,
                    VaultAuthority source,
                    VaultAuthority destination,
                    bool explicit_user_action) = 0;
  virtual bool lock(VaultAuthority authority) = 0;
  virtual bool unlock(VaultAuthority authority) = 0;
};

static_assert(!kLocalVaultSynchronizes);
static_assert(kGoreeCloudVaultSynchronizes);
static_assert(!kSilentLocalToGoreeCloudMigrationAllowed);
static_assert(!kSecretsAllowedInBrowserPreferences);

}  // namespace goreecloud::browser
