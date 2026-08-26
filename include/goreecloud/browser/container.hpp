#pragma once

#include <cstdint>
#include <string>

namespace goreecloud::browser {

using ContainerId = std::uint64_t;

enum class ContainerKind {
  personal,
  work,
  banking,
  shopping,
  custom,
};

struct ContainerDefinition {
  ContainerId id{};
  ContainerKind kind{ContainerKind::custom};
  std::string name;
  std::string color_token;
  std::string icon_token;
};

struct ContainerAssignment {
  std::string hostname;
  ContainerId container_id{};
};

// Container configuration may synchronize. Website state associated with a
// container (cookies, sessions, local storage, tokens) remains local.
struct ContainerSyncPolicy {
  bool synchronize_definitions{true};
  bool synchronize_assignments{true};
  bool synchronize_site_state{false};
};

}  // namespace goreecloud::browser
