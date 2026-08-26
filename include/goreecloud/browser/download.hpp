#pragma once

#include <cstdint>
#include <optional>
#include <string>

namespace goreecloud::browser {

enum class DownloadPriority { low, normal, high };
enum class QueueMode { sequential, parallel };

enum class DownloadState {
  queued,
  transferring,
  paused,
  verifying,
  completed,
  failed,
  blocked,
};

struct DownloadPolicy {
  std::uint8_t maximum_segments{16};
  std::uint8_t simultaneous_downloads{5};
  bool allow_single_connection_fallback{true};
  bool wifi_only{false};
  bool allow_metered_network{true};
  bool require_safe_browsing_verdict{true};
};

struct DownloadRequest {
  std::string url;
  std::string destination_class;
  DownloadPriority priority{DownloadPriority::normal};
  std::optional<std::string> expected_sha256;
  std::optional<std::string> expected_sha512;
};

struct DownloadRecord {
  std::string id;
  DownloadState state{DownloadState::queued};
  std::uint64_t received_bytes{};
  std::optional<std::uint64_t> expected_bytes;
};

// Download acceleration is subordinate to TLS, certificate validation,
// authorization, Safe Browsing, dangerous-file protections, and access
// controls. It must never be used as an access-control bypass.
class DownloadManager {
 public:
  virtual ~DownloadManager() = default;
  [[nodiscard]] virtual DownloadPolicy policy() const = 0;
  virtual void set_policy(DownloadPolicy policy) = 0;
  [[nodiscard]] virtual std::optional<DownloadRecord> enqueue(
      const DownloadRequest& request) = 0;
  virtual bool pause(std::string_view id) = 0;
  virtual bool resume(std::string_view id) = 0;
};

}  // namespace goreecloud::browser
