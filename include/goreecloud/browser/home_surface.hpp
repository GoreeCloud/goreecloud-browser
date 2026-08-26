#pragma once

#include <array>
#include <string_view>

namespace goreecloud::browser {

enum class NewTabModule {
  search,
  shortcuts,
  recent_bookmarks,
  favorites,
  read_later,
  downloads_summary,
  privacy_status,
};

inline constexpr std::array kDefaultNewTabModules{
    NewTabModule::search,
    NewTabModule::shortcuts,
    NewTabModule::recent_bookmarks,
    NewTabModule::favorites,
    NewTabModule::read_later,
    NewTabModule::downloads_summary,
    NewTabModule::privacy_status,
};

enum class HomeModule {
  search,
  continue_browsing,
  bookmarks,
  read_later,
  recent_downloads,
  goreecloud_services,
  privacy_shield,
  wardveil_security,
  sync_status,
};

inline constexpr std::array kDefaultHomeModules{
    HomeModule::search,
    HomeModule::continue_browsing,
    HomeModule::bookmarks,
    HomeModule::read_later,
    HomeModule::recent_downloads,
    HomeModule::goreecloud_services,
    HomeModule::privacy_shield,
    HomeModule::wardveil_security,
    HomeModule::sync_status,
};

inline constexpr bool kSponsoredContentAllowed = false;
inline constexpr bool kPaidPlacementAllowed = false;
inline constexpr bool kThirdPartyPromotionsAllowed = false;
inline constexpr bool kNewTabAndHomeUseLatestApprovedGlaze = true;

static_assert(!kSponsoredContentAllowed);
static_assert(!kPaidPlacementAllowed);
static_assert(!kThirdPartyPromotionsAllowed);
static_assert(kNewTabAndHomeUseLatestApprovedGlaze);

}  // namespace goreecloud::browser
