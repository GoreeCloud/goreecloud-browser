#pragma once

namespace goreecloud::browser {

enum class ResolverAuthority {
  goreecloud_dns,
  approved_system_resolver,
};

struct BrowserNetworkPolicy {
  ResolverAuthority resolver_authority{ResolverAuthority::goreecloud_dns};
  bool unauthorized_external_doh_allowed{false};
  bool browser_cache_clear_changes_global_dns{false};
  bool browser_cache_clear_changes_os_networking{false};
  bool private_mode_weakens_network_security{false};
};

inline constexpr BrowserNetworkPolicy kBrowserNetworkPolicy{};

static_assert(!kBrowserNetworkPolicy.unauthorized_external_doh_allowed);
static_assert(!kBrowserNetworkPolicy.browser_cache_clear_changes_global_dns);
static_assert(!kBrowserNetworkPolicy.browser_cache_clear_changes_os_networking);
static_assert(!kBrowserNetworkPolicy.private_mode_weakens_network_security);

}  // namespace goreecloud::browser
