#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "goreecloud/browser/internal_pages.hpp"
#include "goreecloud/browser/settings_surface.hpp"

namespace goreecloud::browser {

enum class InternalSurfaceKind {
  new_tab,
  home,
  settings,
};

struct SurfaceAction {
  std::string id;
  std::string label;
  std::string target;
};

struct InternalSurfaceModel {
  InternalSurfaceKind kind{InternalSurfaceKind::new_tab};
  std::string title;
  std::string subtitle;
  std::vector<SurfaceAction> actions;
  bool uses_glaze{true};
  bool advertising_allowed{false};
  bool sponsored_content_allowed{false};
};

class InternalSurfaceRenderer {
 public:
  [[nodiscard]] InternalSurfaceModel model_for(std::string_view url) const {
    if (url == kNewTabUrl) {
      return InternalSurfaceModel{
          .kind = InternalSurfaceKind::new_tab,
          .title = "GoreeCloud Browser",
          .subtitle = "Search, browse, and continue where you left off.",
          .actions = {
              {"search", "Search with GoreeCloud Search", "goreecloud://search"},
              {"bookmarks", "Open Unified Bookmarks", "goreecloud://bookmarks"},
              {"downloads", "Open Advanced Download Manager", "goreecloud://downloads"},
          },
      };
    }

    if (url == kHomeUrl) {
      return InternalSurfaceModel{
          .kind = InternalSurfaceKind::home,
          .title = "GoreeCloud Home",
          .subtitle = "Your Browser home for search, saved content, activity, and GoreeCloud services.",
          .actions = {
              {"search", "Search the web", "goreecloud://search"},
              {"bookmarks", "Unified Bookmarks", "goreecloud://bookmarks"},
              {"vault", "Unified Vault", "goreecloud://vault"},
              {"everkeep", "Everkeep", "goreecloud://settings/everkeep"},
          },
      };
    }

    return InternalSurfaceModel{
        .kind = InternalSurfaceKind::settings,
        .title = "GoreeCloud Browser Settings",
        .subtitle = "Control Browser behavior, privacy, security, data, network, recovery, and accessibility.",
        .actions = {
            {"general", "General", "goreecloud://settings/general"},
            {"privacy", "Privacy & Blocking", "goreecloud://settings/privacy"},
            {"security", "Wardveil Security", "goreecloud://settings/security"},
            {"vault", "Unified Vault", "goreecloud://settings/vault"},
            {"downloads", "Advanced Download Manager", "goreecloud://settings/downloads"},
            {"everkeep", "Everkeep", "goreecloud://settings/everkeep"},
        },
    };
  }
};

inline constexpr bool kInternalPagesUseLatestApprovedGlaze = true;
inline constexpr bool kInternalPagesAllowAds = false;
inline constexpr bool kInternalPagesAllowSponsoredContent = false;

static_assert(kInternalPagesUseLatestApprovedGlaze);
static_assert(!kInternalPagesAllowAds);
static_assert(!kInternalPagesAllowSponsoredContent);

}  // namespace goreecloud::browser
