#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace goreecloud::browser {

enum class ServiceStatus {
  unavailable,
  degraded,
  available,
};

// CapabilityEvidence is Browser's transport-neutral consumer view of a
// first-party GoreeCloud service capability. The producing service remains the
// authority for its capability and contract version; Browser only decides
// whether the supplied evidence is strong enough to use the integration.
struct CapabilityEvidence {
  std::string id;
  std::string contract_version;
  bool authoritative{false};
  bool current{false};
  bool production_accepted{false};
};

struct ServiceHealth {
  ServiceStatus status{ServiceStatus::unavailable};
  std::string detail;
  std::vector<CapabilityEvidence> capabilities;
};

// service_capability_available intentionally fails closed. A healthy transport
// alone is not sufficient for Browser to invoke a first-party capability: the
// producer must identify the exact capability as current, authoritative, and
// explicitly production-accepted. When an expected contract version is
// supplied, the version must match exactly.
[[nodiscard]] inline bool service_capability_available(
    const ServiceHealth& health,
    std::string_view capability_id,
    std::string_view expected_contract_version = {}) {
  if (health.status != ServiceStatus::available || capability_id.empty()) {
    return false;
  }
  for (const auto& capability : health.capabilities) {
    if (capability.id != capability_id || !capability.authoritative || !capability.current ||
        !capability.production_accepted) {
      continue;
    }
    if (!expected_contract_version.empty() &&
        capability.contract_version != expected_contract_version) {
      continue;
    }
    return true;
  }
  return false;
}

struct SearchRequest {
  std::string query;
  bool private_context{false};
};

struct SearchResponse {
  bool accepted{false};
  std::string destination;
  std::string error;
};

class SearchService {
 public:
  virtual ~SearchService() = default;
  [[nodiscard]] virtual ServiceHealth health() const = 0;
  [[nodiscard]] virtual SearchResponse search(const SearchRequest& request) = 0;
};

struct BookmarkRecord {
  std::string id;
  std::string title;
  std::string url;
  bool synchronized{false};
};

class BookmarkService {
 public:
  virtual ~BookmarkService() = default;
  [[nodiscard]] virtual ServiceHealth health() const = 0;
  virtual bool save(BookmarkRecord bookmark) = 0;
};

struct SyncRecord {
  std::string collection;
  std::string object_id;
  std::string payload;
};

class SyncService {
 public:
  virtual ~SyncService() = default;
  [[nodiscard]] virtual ServiceHealth health() const = 0;
  virtual bool enqueue(SyncRecord record) = 0;
};

struct VaultSecretRequest {
  std::string relying_party;
  std::string purpose;
};

struct VaultSecretHandle {
  std::string opaque_handle;
};

class VaultService {
 public:
  virtual ~VaultService() = default;
  [[nodiscard]] virtual ServiceHealth health() const = 0;
  [[nodiscard]] virtual std::optional<VaultSecretHandle> request_secret(
      const VaultSecretRequest& request) = 0;
};

struct DnsState {
  ServiceHealth health;
  bool goreecloud_policy_active{false};
};

class DnsService {
 public:
  virtual ~DnsService() = default;
  [[nodiscard]] virtual DnsState state() const = 0;
  virtual bool clear_browser_cache() = 0;
};

struct NetworkState {
  ServiceHealth health;
  bool private_connectivity_active{false};
  bool metered{false};
};

class NetworkService {
 public:
  virtual ~NetworkService() = default;
  [[nodiscard]] virtual NetworkState state() const = 0;
};

struct ProtectionState {
  ServiceHealth health;
  bool active{false};
  std::vector<std::string> evidence;
};

class PrivacyShieldService {
 public:
  virtual ~PrivacyShieldService() = default;
  [[nodiscard]] virtual ProtectionState state_for(std::string_view url) const = 0;
};

class WardveilService {
 public:
  virtual ~WardveilService() = default;
  [[nodiscard]] virtual ProtectionState state_for(std::string_view url) const = 0;
};

}  // namespace goreecloud::browser
