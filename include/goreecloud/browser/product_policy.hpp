#pragma once

namespace goreecloud::browser {

struct ProductPolicy {
  bool advertising_allowed{false};
  bool sponsorship_allowed{false};
  bool paid_placement_allowed{false};
  bool sponsored_search_allowed{false};
  bool promoted_extensions_allowed{false};
  bool behavioral_analytics_allowed{false};
  bool silent_search_fallback_allowed{false};
  bool silent_external_dns_bypass_allowed{false};
  bool inherited_consumer_branding_allowed{false};
};

inline constexpr ProductPolicy kGoreeCloudProductPolicy{};

static_assert(!kGoreeCloudProductPolicy.advertising_allowed);
static_assert(!kGoreeCloudProductPolicy.sponsorship_allowed);
static_assert(!kGoreeCloudProductPolicy.paid_placement_allowed);
static_assert(!kGoreeCloudProductPolicy.silent_search_fallback_allowed);
static_assert(!kGoreeCloudProductPolicy.silent_external_dns_bypass_allowed);
static_assert(!kGoreeCloudProductPolicy.inherited_consumer_branding_allowed);

}  // namespace goreecloud::browser
