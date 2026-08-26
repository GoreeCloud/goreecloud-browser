#pragma once

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
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
  std::optional<std::int64_t> scheduled_start_unix_seconds;
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

class InProcessAdvancedDownloadManagerService final
    : public AdvancedDownloadManagerService {
 public:
  static constexpr std::size_t kMaximumSimultaneousDownloads = 5;
  static constexpr std::size_t kMaximumSegmentsPerDownload = 16;

  DownloadEnqueueResult enqueue(DownloadEnqueueRequest request) override {
    if (request.source_url.empty()) return {false, {}, "Download source URL is empty."};
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

  bool restore(DownloadRecord record) {
    if (record.download_id.empty() || record.request.source_url.empty()) return false;
    record.segment_limit = std::max<std::size_t>(1, std::min(record.segment_limit,
        kMaximumSegmentsPerDownload));
    if (record.state == DownloadState::running) record.state = DownloadState::queued;
    std::scoped_lock lock(mutex_);
    for (const auto& existing : queue_) if (existing.download_id == record.download_id) return false;
    queue_.push_back(std::move(record));
    return true;
  }

  [[nodiscard]] std::vector<DownloadRecord> snapshot() const {
    std::scoped_lock lock(mutex_);
    return queue_;
  }

  [[nodiscard]] std::optional<DownloadRecord> find(std::string_view download_id) const {
    std::scoped_lock lock(mutex_);
    for (const auto& record : queue_) if (record.download_id == download_id) return record;
    return std::nullopt;
  }

  bool set_state(std::string_view download_id, DownloadState state) {
    std::scoped_lock lock(mutex_);
    for (auto& record : queue_) {
      if (record.download_id == download_id) { record.state = state; return true; }
    }
    return false;
  }

  bool start(std::string_view download_id) {
    return transition(download_id, {DownloadState::queued, DownloadState::paused, DownloadState::failed}, DownloadState::running);
  }
  bool pause(std::string_view download_id) { return transition(download_id, {DownloadState::running}, DownloadState::paused); }
  bool resume(std::string_view download_id) { return transition(download_id, {DownloadState::paused, DownloadState::failed}, DownloadState::running); }
  bool cancel(std::string_view download_id) {
    return transition(download_id, {DownloadState::queued, DownloadState::running, DownloadState::paused, DownloadState::failed}, DownloadState::cancelled);
  }
  bool restart(std::string_view download_id) {
    return transition(download_id, {DownloadState::completed, DownloadState::failed, DownloadState::cancelled}, DownloadState::queued);
  }

 private:
  bool transition(std::string_view download_id,
                  std::initializer_list<DownloadState> allowed,
                  DownloadState next) {
    std::scoped_lock lock(mutex_);
    for (auto& record : queue_) {
      if (record.download_id != download_id) continue;
      for (const auto state : allowed) {
        if (record.state == state) { record.state = next; return true; }
      }
      return false;
    }
    return false;
  }

  [[nodiscard]] std::string next_id() {
    const auto value = next_id_.fetch_add(1, std::memory_order_relaxed);
    return "download-" + std::to_string(value);
  }

  mutable std::mutex mutex_;
  std::vector<DownloadRecord> queue_;
  std::atomic<std::uint64_t> next_id_{1};
};

class UnavailableAdvancedDownloadManagerService final : public AdvancedDownloadManagerService {
 public:
  DownloadEnqueueResult enqueue(DownloadEnqueueRequest) override {
    return {false, {}, "Advanced Download Manager runtime adapter is not available."};
  }
};

static_assert(InProcessAdvancedDownloadManagerService::kMaximumSimultaneousDownloads == 5);
static_assert(InProcessAdvancedDownloadManagerService::kMaximumSegmentsPerDownload == 16);

}  // namespace goreecloud::browser
