#pragma once

#include <string>
#include <vector>

namespace goreecloud::browser {

enum class HomeSurface { new_tab, home };

struct HomeItem {
  std::string id;
  std::string title;
  std::string destination;
};

struct HomePolicy {
  bool goreecloud_search_enabled{true};
  bool shortcuts_enabled{true};
  bool approved_recent_content_enabled{true};
  bool favorites_enabled{true};
  bool bookmarks_integration_enabled{true};
  bool sponsored_tiles_allowed{false};
  bool sponsored_stories_allowed{false};
  bool advertising_allowed{false};
  bool paid_placement_allowed{false};
  bool promoted_downloads_allowed{false};
};

class HomeSurfaceProvider {
 public:
  virtual ~HomeSurfaceProvider() = default;
  [[nodiscard]] virtual HomePolicy policy(HomeSurface surface) const = 0;
  [[nodiscard]] virtual std::vector<HomeItem> items(HomeSurface surface) const = 0;
};

}  // namespace goreecloud::browser
