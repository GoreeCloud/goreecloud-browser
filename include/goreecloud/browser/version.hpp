#pragma once

#include <string_view>

namespace goreecloud::browser {

inline constexpr std::string_view kBrowserVersion{"0.1.0-beta.1"};
inline constexpr std::string_view kBrowserReleaseChannel{"beta"};
inline constexpr bool kProductionApproved = false;
inline constexpr bool kBetaChannel = true;

static_assert(kBetaChannel);
static_assert(!kProductionApproved);

}  // namespace goreecloud::browser
