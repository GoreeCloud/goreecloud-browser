#pragma once

#include <atomic>
#include <cstddef>
#include <mutex>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace goreecloud::browser {

enum class DownloadState {
  queued,
  running,
  paused,
  completed,
  failed,
  cancelled,
};

struct DownloadEnqueueRequest {
  std::string source_url;
  std::string referrer_url;
  std::optional<std::string> suggested_filename;
  bool private_session{false};
};

struct DownloadRecord {
  std::string download_id;
  DownloadEnqueueRequest request;
  DownloadState state{DownloadState::queued};
  std::size_t segment_limit{16};
  bool resumable{true};
};

struct DownloadEnqueueResult {
  bool accepted{false};
  std::string download_id;
  std::string message;
};

class AdvancedDownloadManagerService {
 public:
  virtual ~AdvancedDownloadManagerService() = default;
  virtual DownloadEnqueueResult enqueue(DownloadEnqueueRequest request) = 0;
};

// Browser-owned queue core for the first-party Advanced Download Manager.
// This layer owns download identity and queue state. Network transfer,
// segmentation, resume, scheduling and persistence workers attach below this
// interface and must not be falsely implied by queue acceptance alone.
class InProcessAdvancedDownloadManagerService final
    : public AdvancedDownloadManagerService {
 public:
  static constexpr std::size_t kMaximumSimultaneousDownloads = 5;
  static constexpr std::size_t kMaximumSegmentsPerDownload = 16;

  DownloadEnqueueResult enqueue(DownloadEnqueueRequest request) override {
    if (request.source_url.empty()) {
      return {false, {}, "Download source URL is empty."};
    }

    DownloadRecord record;
    record.download_id = next_id();
    record.request = std::move(request);
    record.state = DownloadState::queued;
    record.segment_limit = kMaximumSegmentsPerDownload;
    record.resumable = true;

    {
      std::scoped_lock lock(mutex_);
      queue_.push_back(record);
    }

    return {true, record.download_id, "Added to Advanced Download Manager queue."};
  }

  [[nodiscard]] std::vector<DownloadRecord> snapshot() const {
    std::scoped_lock lock(mutex_);
    return queue_;
  }

  [[nodiscard]] std::optional<DownloadRecord> find(std::string_view download_id) const {
    std::scoped_lock lock(mutex_);
    for (const auto& record : queue_) {
      if (record.download_id == download_id) return record;
    }
    return std::nullopt;
  }

 private:
  [[nodiscard]] std::string next_id() {
    const auto value = next_id_.fetch_add(1, std::memory_order_relaxed);
    return "download-" + std::to_string(value);
  }

  mutable std::mutex mutex_;
  std::vector<DownloadRecord> queue_;
  std::atomic<std::uint64_t> next_id_{1};
};

class UnavailableAdvancedDownloadManagerService final
    : public AdvancedDownloadManagerService {
 public:
  DownloadEnqueueResult enqueue(DownloadEnqueueRequest) override {
    return {false, {}, "Advanced Download Manager runtime adapter is not available."};
  }
};

static_assert(InProcessAdvancedDownloadManagerService::kMaximumSimultaneousDownloads == 5);
static_assert(InProcessAdvancedDownloadManagerService::kMaximumSegmentsPerDownload == 16);

}  // namespace goreecloud::browser
