#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace goreecloud::browser {

enum class DownloadPriority { low, normal, high };
enum class QueueMode { sequential, parallel };
enum class DownloadMode { normal, accelerated, turbo };
enum class DownloadSource { browser, clipboard, manual_entry, system_share, batch_import, media_detector };

enum class DownloadState {
  queued,
  transferring,
  paused,
  verifying,
  completed,
  failed,
  blocked,
  stopped,
};

enum class NetworkClass { unknown, wifi, ethernet, cellular, metered, private_network };

struct DownloadPolicy {
  std::uint8_t maximum_segments{16};
  std::uint8_t simultaneous_downloads{5};
  bool allow_single_connection_fallback{true};
  bool resume_after_restart{true};
  bool retry_transient_failures{true};
  bool wifi_only{false};
  bool allow_metered_network{true};
  bool require_safe_browsing_verdict{true};
  std::optional<std::uint64_t> global_bandwidth_limit_bytes_per_second;
};

struct DownloadRequest {
  std::string url;
  std::string destination_class;
  DownloadPriority priority{DownloadPriority::normal};
  DownloadMode mode{DownloadMode::accelerated};
  DownloadSource source{DownloadSource::browser};
  std::optional<std::uint8_t> requested_segments;
  std::optional<std::uint64_t> bandwidth_limit_bytes_per_second;
  std::optional<std::string> expected_sha256;
  std::optional<std::string> expected_sha512;
  std::optional<std::string> preferred_filename;
  std::optional<std::string> user_agent_override;
};

struct DownloadProgress {
  std::uint64_t received_bytes{};
  std::optional<std::uint64_t> expected_bytes;
  std::uint64_t bytes_per_second{};
  std::optional<std::uint64_t> eta_seconds;
  std::uint8_t active_segments{1};
};

struct DownloadRecord {
  std::string id;
  std::string resolved_filename;
  std::string destination_path;
  DownloadState state{DownloadState::queued};
  DownloadProgress progress;
  std::uint32_t retry_count{};
  bool resumable{false};
};

struct DownloadQueue {
  std::string id;
  std::string name;
  QueueMode mode{QueueMode::parallel};
  std::vector<std::string> download_ids;
};

// Download acceleration is subordinate to TLS, certificate validation,
// authentication, authorization, Safe Browsing, dangerous-file protections,
// DRM, paywalls, hotlink/access restrictions, and other technical controls.
// Performance features must never become access-control bypass mechanisms.
class DownloadManager {
 public:
  virtual ~DownloadManager() = default;

  [[nodiscard]] virtual DownloadPolicy policy() const = 0;
  virtual void set_policy(DownloadPolicy policy) = 0;

  [[nodiscard]] virtual std::optional<DownloadRecord> enqueue(
      const DownloadRequest& request) = 0;
  [[nodiscard]] virtual std::vector<DownloadRecord> records() const = 0;
  [[nodiscard]] virtual std::optional<DownloadRecord> record(
      std::string_view id) const = 0;

  virtual bool pause(std::string_view id) = 0;
  virtual bool resume(std::string_view id) = 0;
  virtual bool restart(std::string_view id) = 0;
  virtual bool stop(std::string_view id) = 0;
  virtual bool remove(std::string_view id, bool delete_file) = 0;

  virtual bool start_all() = 0;
  virtual bool stop_all() = 0;

  [[nodiscard]] virtual std::optional<DownloadQueue> create_queue(
      std::string name, QueueMode mode) = 0;
  virtual bool move_to_queue(std::string_view download_id,
                             std::string_view queue_id) = 0;
  virtual bool reorder(std::string_view queue_id,
                       std::string_view download_id,
                       std::size_t new_index) = 0;

  virtual void on_network_changed(NetworkClass network) = 0;
  virtual void recover_interrupted_transfers() = 0;
};

}  // namespace goreecloud::browser
