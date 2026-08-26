#pragma once

#include <string>
#include <vector>

namespace goreecloud::browser {

enum class ExtensionSupportState {
  disabled,
  compatibility_only,
  goreecloud_managed,
};

struct ExtensionPolicy {
  ExtensionSupportState support{ExtensionSupportState::disabled};
  bool inherited_addons_manager_exposed{false};
  bool upstream_store_exposed{false};
  bool recommendations_enabled{false};
  bool sponsored_extensions_enabled{false};
};

struct ExtensionDescriptor {
  std::string id;
  std::string name;
  std::vector<std::string> permissions;
};

class ExtensionManager {
 public:
  virtual ~ExtensionManager() = default;
  [[nodiscard]] virtual ExtensionPolicy policy() const = 0;
  [[nodiscard]] virtual std::vector<ExtensionDescriptor> installed() const = 0;
};

// If installable extensions are exposed, they require a separately approved
// GoreeCloud-owned security, permission, installation, update, review,
// compatibility, removal, recovery, and management experience.

}  // namespace goreecloud::browser
