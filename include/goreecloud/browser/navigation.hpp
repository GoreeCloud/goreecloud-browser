#pragma once

#include <string>
#include <string_view>

namespace goreecloud::browser {

enum class NavigationIntent {
  direct_url,
  goreecloud_search,
  invalid,
};

struct NavigationDecision {
  NavigationIntent intent{NavigationIntent::invalid};
  std::string value;
};

// The address surface distinguishes known destinations from search requests.
// Search failure must never be converted into an unauthorized provider
// fallback. Direct navigation remains independent from Search availability.
class NavigationRouter {
 public:
  virtual ~NavigationRouter() = default;
  [[nodiscard]] virtual NavigationDecision classify(
      std::string_view input) const = 0;
};

}  // namespace goreecloud::browser
