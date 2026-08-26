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

struct ServiceHealth {
  ServiceStatus status{ServiceStatus::unavailable};
  std::string detail;
};

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
