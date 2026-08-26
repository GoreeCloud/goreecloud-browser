#pragma once

#include <optional>
#include <string>
#include <vector>

#include "goreecloud/browser/container.hpp"

namespace goreecloud::browser {

enum class ProxyType {
  direct,
  system,
  http,
  https,
  socks4,
  socks5,
  pac,
};

struct ProxyProfile {
  std::string id;
  std::string name;
  ProxyType type{ProxyType::direct};
  std::string endpoint;
  bool remote_dns{false};
  bool fail_closed{false};
  std::optional<std::string> vault_credential_handle;
};

struct ProxySiteRule {
  std::string hostname_pattern;
  std::string profile_id;
};

struct ProxyContainerRule {
  ContainerId container_id{};
  std::string profile_id;
};

struct ProxyRoutingDecision {
  std::string profile_id;
  std::string reason;
  bool bypassed{false};
};

class ProxyRouter {
 public:
  virtual ~ProxyRouter() = default;

  // Required precedence: Bypass -> Site Rule -> Container Rule -> Global Profile.
  [[nodiscard]] virtual ProxyRoutingDecision resolve(
      const std::string& hostname,
      std::optional<ContainerId> container_id) const = 0;
};

}  // namespace goreecloud::browser
