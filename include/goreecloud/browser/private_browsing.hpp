#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace goreecloud::browser {

enum class PrivateSessionMode { shared, isolated_window };
enum class PrivateProtectionLevel { balanced, strict, custom_level };
enum class PrivateDownloadPersistence { keep_file, delete_on_session_close, ask_each_time };
enum class PrivateVaultAccess { disabled, local_only, goreecloud_only, both };

struct PrivateProtectionState {
  bool privacy_shield_active{true};
  bool wardveil_active{true};
  bool goreecloud_dns_active{true};
  bool encrypted_dns_active{false};
  bool goreecloud_network_active{false};
  bool sync_disabled{true};
};

struct PrivateSessionCounters {
  std::size_t open_tabs{0};
  std::uint64_t trackers_blocked{0};
  std::size_t temporary_permissions{0};
  std::size_t private_downloads{0};
  std::size_t sites_holding_temporary_data{0};
};

struct PrivateBrowsingPolicy {
  PrivateProtectionLevel protection_level{PrivateProtectionLevel::balanced};
  PrivateDownloadPersistence downloads{PrivateDownloadPersistence::ask_each_time};
  PrivateVaultAccess vault_access{PrivateVaultAccess::both};
  bool require_private_vault_unlock{false};
  bool private_search_history{false};
  bool private_search_personalization{false};
  bool synchronize_session_state{false};
  bool durable_operational_logging{false};
  bool retain_clipboard_history{false};
  bool expose_normal_clipboard_history{false};
  bool use_for_behavioral_analytics{false};
  bool preserve_dns_security{true};
  bool preserve_network_security{true};
  bool preserve_certificate_security{true};
  bool persist_private_session_recovery{false};
  bool screenshot_protection{false};
  bool task_switcher_protection{false};
  bool lock_on_device_lock{false};
  bool lock_when_backgrounded{false};
  bool close_on_browser_lock{false};
};

struct PrivateSessionDescriptor {
  std::string id;
  PrivateSessionMode mode{PrivateSessionMode::shared};
  PrivateProtectionState protection;
  PrivateSessionCounters counters;
  bool locked{false};
};

class PrivateBrowsingManager {
 public:
  virtual ~PrivateBrowsingManager() = default;
  virtual std::string create_session(PrivateSessionMode mode) = 0;
  virtual bool attach_window(const std::string& session_id, const std::string& window_id) = 0;
  virtual bool forget_tab(const std::string& session_id, const std::string& tab_id) = 0;
  virtual bool forget_site(const std::string& session_id, const std::string& origin) = 0;
  virtual bool clear_cookies(const std::string& session_id) = 0;
  virtual bool clear_cache(const std::string& session_id) = 0;
  virtual bool clear_site_storage(const std::string& session_id) = 0;
  virtual bool clear_authentication(const std::string& session_id) = 0;
  virtual bool clear_permissions(const std::string& session_id) = 0;
  virtual bool clear_history(const std::string& session_id) = 0;
  virtual bool clear_search_state(const std::string& session_id) = 0;
  virtual bool clear_everything(const std::string& session_id) = 0;
  virtual bool close_and_forget_session(const std::string& session_id) = 0;
  virtual bool lock_session(const std::string& session_id) = 0;
  virtual bool unlock_session(const std::string& session_id) = 0;
  [[nodiscard]] virtual std::optional<PrivateSessionDescriptor> inspect(const std::string& session_id) const = 0;
};

inline constexpr bool kPrivateActivitySynchronizesByDefault = false;
inline constexpr bool kPrivateStateInNormalSessionRestore = false;
inline constexpr bool kPrivateOperationalLogsDurable = false;
inline constexpr bool kPrivateBrowsingClaimsAnonymity = false;
inline constexpr bool kSecurityProtectionsRemainActive = true;

static_assert(!kPrivateActivitySynchronizesByDefault);
static_assert(!kPrivateStateInNormalSessionRestore);
static_assert(!kPrivateOperationalLogsDurable);
static_assert(!kPrivateBrowsingClaimsAnonymity);
static_assert(kSecurityProtectionsRemainActive);

}  // namespace goreecloud::browser
