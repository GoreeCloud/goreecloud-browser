#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "goreecloud/browser/advanced_download_manager_service.hpp"
#include "goreecloud/browser/advanced_download_transfer_engine.hpp"
#include "goreecloud/browser/download_file_store.hpp"
#include "goreecloud/browser/download_queue_store.hpp"
#include "goreecloud/browser/file_download_checkpoint_store.hpp"
#include "goreecloud/browser/http_download_transport.hpp"

namespace goreecloud::browser {

struct LiveDownloadProgress {
  std::uint64_t completed_bytes{0};
  std::uint64_t total_bytes{0};
  double bytes_per_second{0.0};
  DownloadState state{DownloadState::queued};
  std::string message;
};

class AdvancedDownloadRuntimeService final : public AdvancedDownloadManagerService {
 public:
  AdvancedDownloadRuntimeService(HttpDownloadClient& client,
                                 std::filesystem::path download_directory)
      : download_directory_(std::move(download_directory)),
        file_store_(download_directory_),
        checkpoints_(download_directory_ / ".goreecloud-checkpoints"),
        queue_store_(download_directory_ / ".goreecloud-download-state"),
        transport_(client, [this](std::string_view download_id,
                                  std::uint64_t offset,
                                  std::span<const std::byte> bytes) {
          const auto found = files_.find(std::string{download_id});
          if (found == files_.end()) return false;
          return file_store_.write_at(found->second, offset, bytes);
        }),
        scheduler_(transport_) {
    scheduler_.set_restore_plan_callback([this](DownloadTransferPlan& plan) {
      const auto checkpoint = checkpoints_.load(plan.download_id);
      if (!checkpoint) return false;
      return apply_download_checkpoint(plan, *checkpoint);
    });
    scheduler_.set_progress_callback([this](const DownloadTransferPlan& plan) {
      checkpoints_.save(make_download_checkpoint(plan));
      queue_.set_state(plan.download_id, DownloadState::running);
      persist_queue();
      auto& progress = progress_[plan.download_id];
      const auto now = Clock::now();
      if (progress_samples_.contains(plan.download_id)) {
        const auto& sample = progress_samples_.at(plan.download_id);
        const auto elapsed = std::chrono::duration<double>(now - sample.time).count();
        if (elapsed > 0.0 && plan.completed_bytes >= sample.bytes) {
          progress.bytes_per_second =
              static_cast<double>(plan.completed_bytes - sample.bytes) / elapsed;
        }
      }
      progress_samples_[plan.download_id] = {plan.completed_bytes, now};
      progress.completed_bytes = plan.completed_bytes;
      progress.total_bytes = plan.metadata.total_bytes;
      progress.state = DownloadState::running;
      progress.message = "Downloading";
    });
    scheduler_.set_completion_callback([this](const DownloadTransferPlan& plan) {
      auto found = files_.find(plan.download_id);
      auto& progress = progress_[plan.download_id];
      if (found == files_.end()) {
        mark_failed(plan.download_id, "Download file state is unavailable.");
        return;
      }
      const auto evidence = file_store_.commit(found->second, plan.metadata.total_bytes);
      if (!evidence.committed) {
        mark_failed(plan.download_id, evidence.message);
        return;
      }
      progress.completed_bytes = evidence.final_size;
      progress.total_bytes = evidence.final_size;
      progress.bytes_per_second = 0.0;
      progress.state = DownloadState::completed;
      progress.message = evidence.message;
      queue_.set_state(plan.download_id, DownloadState::completed);
      checkpoints_.erase(plan.download_id);
      scheduled_waiting_.erase(plan.download_id);
      persist_queue();
    });
    scheduler_.set_failure_callback([this](std::string_view download_id) {
      mark_failed(download_id, "Download transfer failed.");
    });
    restore_persistent_queue();
  }

  DownloadEnqueueResult enqueue(DownloadEnqueueRequest request) override {
    auto queued = queue_.enqueue(std::move(request));
    if (!queued.accepted) return queued;
    const auto record = queue_.find(queued.download_id);
    if (!record) return {false, {}, "Download queue record could not be created."};

    const auto filename = record->request.suggested_filename.value_or(default_filename(record->request.source_url));
    const auto paths = file_store_.prepare(record->download_id, filename, 0);
    if (!paths) {
      queue_.cancel(record->download_id);
      persist_queue();
      return {false, {}, "Download destination could not be prepared."};
    }
    files_[record->download_id] = *paths;
    progress_[record->download_id] = {.state = DownloadState::queued, .message = "Queued"};
    if (!scheduler_.queue(*record)) {
      file_store_.discard(*paths);
      queue_.cancel(record->download_id);
      persist_queue();
      return {false, {}, "Download scheduler rejected the request."};
    }
    if (scheduled_for_future(*record)) {
      scheduler_.pause(record->download_id);
      scheduled_waiting_.insert(record->download_id);
      progress_[record->download_id].message = "Scheduled";
    }
    persist_queue();
    return queued;
  }

  bool pause(std::string_view download_id) {
    const auto record = queue_.find(download_id);
    if (!record || record->state == DownloadState::completed ||
        record->state == DownloadState::cancelled) return false;
    if (!scheduler_.pause(download_id) || !queue_.set_state(download_id, DownloadState::paused)) return false;
    scheduled_waiting_.erase(std::string{download_id});
    auto& state = progress_[std::string{download_id}];
    state.state = DownloadState::paused;
    state.bytes_per_second = 0.0;
    state.message = "Paused";
    persist_queue();
    return true;
  }

  bool resume(std::string_view download_id) {
    const auto record = queue_.find(download_id);
    if (!record || record->state != DownloadState::paused) return false;
    scheduled_waiting_.erase(std::string{download_id});
    if (!scheduler_.resume(download_id) || !queue_.set_state(download_id, DownloadState::queued)) return false;
    auto& state = progress_[std::string{download_id}];
    state.state = DownloadState::queued;
    state.message = "Resuming";
    persist_queue();
    return true;
  }

  bool cancel(std::string_view download_id, bool discard_partial_file = false) {
    const auto record = queue_.find(download_id);
    if (!record || record->state == DownloadState::completed ||
        record->state == DownloadState::cancelled) return false;
    scheduler_.cancel(download_id);
    scheduled_waiting_.erase(std::string{download_id});
    if (!queue_.set_state(download_id, DownloadState::cancelled)) return false;
    checkpoints_.erase(download_id);
    auto found = files_.find(std::string{download_id});
    if (discard_partial_file && found != files_.end()) file_store_.discard(found->second);
    auto& state = progress_[std::string{download_id}];
    state.state = DownloadState::cancelled;
    state.bytes_per_second = 0.0;
    state.message = discard_partial_file ? "Cancelled and partial data removed" : "Cancelled";
    persist_queue();
    return true;
  }

  void pump() {
    release_due_schedules();
    scheduler_.pump();
  }

  [[nodiscard]] std::optional<LiveDownloadProgress> progress(std::string_view id) const {
    const auto found = progress_.find(std::string{id});
    if (found == progress_.end()) return std::nullopt;
    return found->second;
  }

  [[nodiscard]] std::vector<DownloadRecord> snapshot() const { return queue_.snapshot(); }
  [[nodiscard]] std::size_t active_count() const noexcept { return scheduler_.active_count(); }
  [[nodiscard]] std::size_t pending_count() const noexcept { return scheduler_.pending_count(); }

 private:
  using Clock = std::chrono::steady_clock;
  struct ProgressSample { std::uint64_t bytes{0}; Clock::time_point time{}; };

  static std::int64_t unix_now_seconds() {
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
  }

  static bool scheduled_for_future(const DownloadRecord& record) {
    return record.request.scheduled_start_unix_seconds &&
           *record.request.scheduled_start_unix_seconds > unix_now_seconds();
  }

  void release_due_schedules() {
    std::vector<std::string> due;
    for (const auto& id : scheduled_waiting_) {
      const auto record = queue_.find(id);
      if (!record || !scheduled_for_future(*record)) due.push_back(id);
    }
    for (const auto& id : due) {
      scheduled_waiting_.erase(id);
      if (scheduler_.resume(id)) {
        auto& state = progress_[id];
        state.state = DownloadState::queued;
        state.message = "Scheduled start released";
      }
    }
  }

  void restore_persistent_queue() {
    for (auto record : queue_store_.load()) {
      if (record.request.private_session || record.state == DownloadState::completed ||
          record.state == DownloadState::cancelled) continue;
      if (!queue_.restore(record)) continue;
      const auto restored = queue_.find(record.download_id);
      if (!restored) continue;
      const auto filename = restored->request.suggested_filename.value_or(default_filename(restored->request.source_url));
      const auto paths = file_store_.prepare(restored->download_id, filename, 0);
      if (!paths) {
        mark_failed(restored->download_id, "Interrupted download file could not be reopened.");
        continue;
      }
      files_[restored->download_id] = *paths;
      const auto checkpoint = checkpoints_.load(restored->download_id);
      const bool was_paused = record.state == DownloadState::paused;
      const bool is_scheduled = !was_paused && scheduled_for_future(*restored);
      progress_[restored->download_id] = {
          .completed_bytes = checkpoint ? checkpoint->completed_bytes : 0,
          .total_bytes = checkpoint ? checkpoint->total_bytes : 0,
          .bytes_per_second = 0.0,
          .state = was_paused ? DownloadState::paused : DownloadState::queued,
          .message = was_paused ? "Paused" : (is_scheduled ? "Scheduled" : (checkpoint ? "Ready to resume" : "Restored")),
      };
      if (!scheduler_.queue(*restored)) {
        mark_failed(restored->download_id, "Restored download could not be scheduled.");
      } else if (was_paused || is_scheduled) {
        scheduler_.pause(restored->download_id);
        if (is_scheduled) scheduled_waiting_.insert(restored->download_id);
      }
    }
    persist_queue();
  }

  void persist_queue() {
    std::vector<DownloadRecord> persistent;
    for (const auto& record : queue_.snapshot()) {
      if (!record.request.private_session) persistent.push_back(record);
    }
    queue_store_.save(persistent);
  }

  void mark_failed(std::string_view download_id, std::string message) {
    const auto id = std::string{download_id};
    auto& progress = progress_[id];
    progress.state = DownloadState::failed;
    progress.bytes_per_second = 0.0;
    progress.message = std::move(message);
    scheduled_waiting_.erase(id);
    queue_.set_state(id, DownloadState::failed);
    persist_queue();
  }

  static std::string default_filename(std::string_view url) {
    auto end = url.find_first_of("?#");
    if (end == std::string_view::npos) end = url.size();
    const auto slash = url.rfind('/', end == 0 ? 0 : end - 1);
    auto name = std::string{url.substr(slash == std::string_view::npos ? 0 : slash + 1,
                                       end - (slash == std::string_view::npos ? 0 : slash + 1))};
    if (name.empty()) name = "download.bin";
    return name;
  }

  std::filesystem::path download_directory_;
  InProcessAdvancedDownloadManagerService queue_;
  LocalDownloadFileStore file_store_;
  FileDownloadCheckpointStore checkpoints_;
  FileDownloadQueueStore queue_store_;
  HttpDownloadTransport transport_;
  DownloadTransferScheduler scheduler_;
  std::unordered_map<std::string, DownloadFilePaths> files_;
  std::unordered_map<std::string, LiveDownloadProgress> progress_;
  std::unordered_map<std::string, ProgressSample> progress_samples_;
  std::unordered_set<std::string> scheduled_waiting_;
};

}  // namespace goreecloud::browser
