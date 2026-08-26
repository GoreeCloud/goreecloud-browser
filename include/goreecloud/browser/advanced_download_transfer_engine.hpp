#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
#include <vector>

#include "goreecloud/browser/advanced_download_manager_service.hpp"

namespace goreecloud::browser {

struct DownloadResourceMetadata {
  std::uint64_t total_bytes{0};
  bool accepts_byte_ranges{false};
  bool resumable{false};
  std::optional<std::string> etag;
  std::optional<std::string> last_modified;
  std::optional<std::string> mime_type;
};

struct DownloadByteRange { std::uint64_t begin{0}; std::uint64_t end_inclusive{0}; };
struct DownloadSegment { std::size_t index{0}; DownloadByteRange range; std::uint64_t completed_bytes{0}; unsigned retry_count{0}; bool finished{false}; };
struct DownloadTransferPlan { std::string download_id; std::string source_url; std::string referrer_url; DownloadResourceMetadata metadata; std::vector<DownloadSegment> segments; std::uint64_t completed_bytes{0}; };
struct DownloadTransportRequest { std::string download_id; std::string source_url; std::string referrer_url; DownloadByteRange range; std::uint64_t resume_offset{0}; std::optional<std::string> etag; std::optional<std::string> last_modified; };
struct DownloadTransportResult { bool completed{false}; bool retryable{false}; std::uint64_t transferred_bytes{0}; std::string message; };

class DownloadTransport {
 public:
  virtual ~DownloadTransport() = default;
  virtual std::optional<DownloadResourceMetadata> inspect(std::string_view source_url, std::string_view referrer_url) = 0;
  virtual DownloadTransportResult transfer(const DownloadTransportRequest& request) = 0;
};

class DownloadTransferPlanner {
 public:
  static DownloadTransferPlan make_plan(const DownloadRecord& record,
                                        const DownloadResourceMetadata& metadata,
                                        std::size_t requested_segments = InProcessAdvancedDownloadManagerService::kMaximumSegmentsPerDownload) {
    DownloadTransferPlan plan;
    plan.download_id = record.download_id;
    plan.source_url = record.request.source_url;
    plan.referrer_url = record.request.referrer_url;
    plan.metadata = metadata;
    if (metadata.total_bytes == 0) return plan;
    const auto segment_cap = std::max<std::size_t>(1, std::min({requested_segments, record.segment_limit, InProcessAdvancedDownloadManagerService::kMaximumSegmentsPerDownload}));
    const auto segment_count = metadata.accepts_byte_ranges ? std::min<std::uint64_t>(segment_cap, metadata.total_bytes) : std::uint64_t{1};
    const auto base_size = metadata.total_bytes / segment_count;
    const auto remainder = metadata.total_bytes % segment_count;
    std::uint64_t cursor = 0;
    plan.segments.reserve(static_cast<std::size_t>(segment_count));
    for (std::uint64_t i = 0; i < segment_count; ++i) {
      const auto size = base_size + (i < remainder ? 1 : 0);
      plan.segments.push_back({static_cast<std::size_t>(i), {cursor, cursor + size - 1}, 0, 0, false});
      cursor += size;
    }
    return plan;
  }
};

class DownloadTransferScheduler {
 public:
  using ProgressCallback = std::function<void(const DownloadTransferPlan&)>;
  using CompletionCallback = std::function<void(const DownloadTransferPlan&)>;
  using FailureCallback = std::function<void(std::string_view)>;
  using RestorePlanCallback = std::function<bool(DownloadTransferPlan&)>;

  static constexpr std::size_t kMaximumActiveDownloads = InProcessAdvancedDownloadManagerService::kMaximumSimultaneousDownloads;
  static constexpr unsigned kMaximumRetriesPerSegment = 3;

  explicit DownloadTransferScheduler(DownloadTransport& transport) : transport_(transport) {}
  void set_progress_callback(ProgressCallback callback) { progress_callback_ = std::move(callback); }
  void set_completion_callback(CompletionCallback callback) { completion_callback_ = std::move(callback); }
  void set_failure_callback(FailureCallback callback) { failure_callback_ = std::move(callback); }
  void set_restore_plan_callback(RestorePlanCallback callback) { restore_plan_callback_ = std::move(callback); }

  bool queue(DownloadRecord record) {
    if (record.download_id.empty() || record.request.source_url.empty()) return false;
    if (contains(record.download_id)) return false;
    pending_.push_back(std::move(record));
    return true;
  }

  bool pause(std::string_view id) {
    if (!contains(id)) return false;
    paused_.insert(std::string{id});
    return true;
  }

  bool resume(std::string_view id) {
    return paused_.erase(std::string{id}) > 0;
  }

  bool cancel(std::string_view id) {
    const std::string key{id};
    bool removed = false;
    pending_.erase(std::remove_if(pending_.begin(), pending_.end(), [&](const DownloadRecord& record) {
      if (record.download_id == key) { removed = true; return true; }
      return false;
    }), pending_.end());
    active_.erase(std::remove_if(active_.begin(), active_.end(), [&](const ActiveTransfer& active) {
      if (active.record.download_id == key) { removed = true; return true; }
      return false;
    }), active_.end());
    paused_.erase(key);
    cancelled_.insert(key);
    return removed;
  }

  [[nodiscard]] bool paused(std::string_view id) const { return paused_.contains(std::string{id}); }
  [[nodiscard]] bool cancelled(std::string_view id) const { return cancelled_.contains(std::string{id}); }

  void pump() {
    std::size_t scans = pending_.size();
    while (active_.size() < kMaximumActiveDownloads && !pending_.empty() && scans-- > 0) {
      auto record = std::move(pending_.front()); pending_.pop_front();
      if (cancelled(record.download_id)) continue;
      if (paused(record.download_id)) { pending_.push_back(std::move(record)); continue; }
      const auto metadata = transport_.inspect(record.request.source_url, record.request.referrer_url);
      if (!metadata) { fail_record(record.download_id); continue; }
      ActiveTransfer active; active.record = std::move(record); active.plan = DownloadTransferPlanner::make_plan(active.record, *metadata);
      if (active.plan.segments.empty()) { fail_record(active.record.download_id); continue; }
      if (restore_plan_callback_) restore_plan_callback_(active.plan);
      if (progress_callback_) progress_callback_(active.plan);
      active_.push_back(std::move(active));
    }
    for (auto& active : active_) {
      if (active.finished || active.failed || paused(active.record.download_id) || cancelled(active.record.download_id)) continue;
      run_one_segment(active);
      if (progress_callback_) progress_callback_(active.plan);
    }
    for (const auto& active : active_) {
      if (active.finished) { completed_.push_back(active.record.download_id); if (completion_callback_) completion_callback_(active.plan); }
      if (active.failed) fail_record(active.record.download_id);
    }
    active_.erase(std::remove_if(active_.begin(), active_.end(), [&](const ActiveTransfer& active) {
      return active.finished || active.failed || cancelled(active.record.download_id);
    }), active_.end());
  }

  [[nodiscard]] std::size_t pending_count() const noexcept { return pending_.size(); }
  [[nodiscard]] std::size_t active_count() const noexcept { return active_.size(); }
  [[nodiscard]] const std::vector<std::string>& completed_downloads() const noexcept { return completed_; }
  [[nodiscard]] const std::vector<std::string>& failed_downloads() const noexcept { return failed_; }

 private:
  struct ActiveTransfer { DownloadRecord record; DownloadTransferPlan plan; bool finished{false}; bool failed{false}; };

  [[nodiscard]] bool contains(std::string_view id) const {
    const std::string key{id};
    return std::any_of(pending_.begin(), pending_.end(), [&](const DownloadRecord& record) { return record.download_id == key; }) ||
           std::any_of(active_.begin(), active_.end(), [&](const ActiveTransfer& active) { return active.record.download_id == key; });
  }

  void fail_record(std::string_view id) { failed_.push_back(std::string{id}); if (failure_callback_) failure_callback_(id); }

  void run_one_segment(ActiveTransfer& active) {
    auto segment_it = std::find_if(active.plan.segments.begin(), active.plan.segments.end(), [](const DownloadSegment& segment) { return !segment.finished; });
    if (segment_it == active.plan.segments.end()) { active.finished = true; return; }
    auto& segment = *segment_it;
    DownloadTransportRequest request{active.record.download_id, active.record.request.source_url, active.record.request.referrer_url,
                                     segment.range, segment.completed_bytes, active.plan.metadata.etag, active.plan.metadata.last_modified};
    const auto result = transport_.transfer(request);
    const auto segment_size = segment.range.end_inclusive - segment.range.begin + 1;
    segment.completed_bytes = std::min(segment_size, segment.completed_bytes + result.transferred_bytes);
    active.plan.completed_bytes = 0;
    for (const auto& planned : active.plan.segments) active.plan.completed_bytes += planned.completed_bytes;
    if (result.completed || segment.completed_bytes >= segment_size) {
      segment.completed_bytes = segment_size; segment.finished = true; active.plan.completed_bytes = 0;
      for (const auto& planned : active.plan.segments) active.plan.completed_bytes += planned.completed_bytes;
      active.finished = std::all_of(active.plan.segments.begin(), active.plan.segments.end(), [](const DownloadSegment& planned) { return planned.finished; });
      return;
    }
    if (result.retryable && segment.retry_count < kMaximumRetriesPerSegment) { ++segment.retry_count; return; }
    active.failed = true;
  }

  DownloadTransport& transport_;
  ProgressCallback progress_callback_;
  CompletionCallback completion_callback_;
  FailureCallback failure_callback_;
  RestorePlanCallback restore_plan_callback_;
  std::deque<DownloadRecord> pending_;
  std::vector<ActiveTransfer> active_;
  std::unordered_set<std::string> paused_;
  std::unordered_set<std::string> cancelled_;
  std::vector<std::string> completed_;
  std::vector<std::string> failed_;
};

static_assert(DownloadTransferScheduler::kMaximumActiveDownloads == 5);
static_assert(InProcessAdvancedDownloadManagerService::kMaximumSegmentsPerDownload == 16);

}  // namespace goreecloud::browser
