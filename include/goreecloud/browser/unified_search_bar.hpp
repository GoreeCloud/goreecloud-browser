#pragma once

#include <array>
#include <string_view>

namespace goreecloud::browser {

enum class UnifiedSearchBarControl {
  advanced_reader_mode,
  unified_bookmarks,
  wardveil_security,
};

inline constexpr std::array kUnifiedSearchBarControls{
    UnifiedSearchBarControl::advanced_reader_mode,
    UnifiedSearchBarControl::unified_bookmarks,
    UnifiedSearchBarControl::wardveil_security,
};

inline constexpr std::string_view kSearchAuthority{"GoreeCloud Search"};
inline constexpr bool kAlternateSearchProvidersAllowed = false;
inline constexpr bool kSilentSearchFallbackAllowed = false;
inline constexpr bool kDirectUrlNavigationIndependentFromSearch = true;
inline constexpr bool kBookmarkButtonOffersLocalAndGoreeCloud = true;
inline constexpr bool kReaderModeButtonBuiltIntoSearchBar = true;
inline constexpr bool kWardveilButtonBuiltIntoSearchBar = true;

static_assert(kUnifiedSearchBarControls.size() == 3);
static_assert(!kAlternateSearchProvidersAllowed);
static_assert(!kSilentSearchFallbackAllowed);
static_assert(kDirectUrlNavigationIndependentFromSearch);
static_assert(kBookmarkButtonOffersLocalAndGoreeCloud);

}  // namespace goreecloud::browser
