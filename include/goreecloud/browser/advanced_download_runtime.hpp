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
#include "goreecloud/browser/wardveil_download_security.hpp"

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
                                 std::filesystem::path download_directory,
                                 WardveilDownloadScanner* wardveil_scanner = nullptr)
      : download_directory_(std::move(download_directory)),
        file_store_(download_directory_),
        checkpoints_(download_directory_ / ".goreecloud-checkpoints"),
        queue_store_(download_directory_ / ".goreecloud-download-state"),
        wardveil_scanner_(wardveil_scanner ? wardveil_scanner : &unavailable_wardveil_scanner()),
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
      verify_and_commit(plan);
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
        record->state == DownloadState::cancelled || record->state == DownloadState::held ||
        record->state == DownloadState::blocked || record->state == DownloadState::verifying) return false;
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
        record->state == DownloadState::cancelled || record->state == DownloadState::verifying) return false;
    scheduler_.cancel(download_id);
    scheduled_waiting_.erase(std::string{download_id});
    if (!queue_.set_state(download_id, DownloadState::cancelled)) return false;
    checkpoints_.erase(download_id);
    auto found = files_.find(std::string{download_id});
    if (discard_partial_file && found != files_.end()) file_store_.discard(found->second);
    security_decisions_.erase(std::string{download_id});
    auto& state = progress_[std::string{download_id}];
    state.state = DownloadState::cancelled;
    state.bytes_per_second = 0.0;
    state.message = discard_partial_file ? "Cancelled and partial data removed" : "Cancelled";
    persist_queue();
    return true;
  }

  bool restart(std::string_view download_id) {
    const auto record = queue_.find(download_id);
    if (!record || (record->state != DownloadState::completed &&
                    record->state != DownloadState::failed &&
                    record->state != DownloadState::cancelled &&
                    record->state != DownloadState::held &&
                    record->state != DownloadState::blocked)) return false;
    const auto id = std::string{download_id};
    scheduler_.cancel(id);
    scheduled_waiting_.erase(id);
    checkpoints_.erase(id);
    security_decisions_.erase(id);
    const auto filename = record->request.suggested_filename.value_or(default_filename(record->request.source_url));
    const auto paths = file_store_.prepare(id, filename, 0);
    if (!paths) return false;
    file_store_.discard(*paths);
    const auto fresh_paths = file_store_.prepare(id, filename, 0);
    if (!fresh_paths) return false;
    files_[id] = *fresh_paths;
    auto restarted = *record;
    restarted.state = DownloadState::queued;
    if (!queue_.set_state(id, DownloadState::queued) || !scheduler_.queue(restarted)) return false;
    progress_[id] = {.state = DownloadState::queued, .message = "Restarted"};
    persist_queue();
    return true;
  }

  [[nodiscard]] std::optional<std::filesystem::path> completed_file_path(
      std::string_view download_id) const {
    const auto progress = this->progress(download_id);
    if (!progress || progress->state != DownloadState::completed) return std::nullopt;
    const auto found = files_.find(std::string{download_id});
    if (found == files_.end()) return std::nullopt;
    std::error_code error;
    if (!std::filesystem::exists(found->second.final_path, error) || error) return std::nullopt;
    return found->second.final_path;
  }

  [[nodiscard]] std::optional<DownloadSecurityDecision> security_decision(
      std::string_view download_id) const {
    const auto found = security_decisions_.find(std::string{download_id});
    if (found == security_decisions_.end()) return std::nullopt;
    return found->second;
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

  static UnavailableWardveilDownloadScanner& unavailable_wardveil_scanner() {
    static UnavailableWardveilDownloadScanner scanner;
    return scanner;
  }

  static std::int64_t unix_now_seconds() {
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
  }

  static bool scheduled_for_future(const DownloadRecord& record) {
    return record.request.scheduled_start_unix_seconds &&
           *record.request.scheduled_start_unix_seconds > unix_now_seconds();
  }

  void verify_and_commit(const DownloadTransferPlan& plan) {
    const auto id = plan.download_id;
    const auto found = files_.find(id);
    if (found == files_.end()) {
      mark_failed(id, "Download file state is unavailable.");
      return;
    }
    auto& progress = progress_[id];
    progress.completed_bytes = plan.completed_bytes;
    progress.total_bytes = plan.metadata.total_bytes;
    progress.bytes_per_second = 0.0;
    progress.state = DownloadState::verifying;
    progress.message = "Verifying with Wardveil Security";
    queue_.set_state(id, DownloadState::verifying);
    persist_queue();

    const auto size = file_store_.partial_size(found->second);
    if (!size || *size != plan.metadata.total_bytes) {
      mark_failed(id, "Completed transfer bytes could not be verified.");
      return;
    }
    const auto digest_before = sha256_file(found->second.partial_path);
    if (!digest_before) {
      mark_blocked(id, "Wardveil verification could not bind the staged download bytes.");
      return;
    }
    const auto record = queue_.find(id);
    if (!record) {
      mark_blocked(id, "Wardveil verification could not bind the download identity.");
      return;
    }

    const WardveilDownloadScanRequest request{
        .resource_id = browser_download_resource_id(id),
        .resource_digest_sha256 = *digest_before,
        .size_bytes = *size,
        .private_session = record->request.private_session,
    };
    auto decision = evaluate_wardveil_download_scan(
        request, wardveil_scanner_->scan(request, found->second.partial_path));
    security_decisions_[id] = decision;

    if (decision.disposition == DownloadSecurityDisposition::hold_review) {
      mark_held(id, "Held for Wardveil Security review.");
      return;
    }
    if (!decision.can_release) {
      mark_blocked(id, decision.quarantine_required
                           ? "Blocked by Wardveil Security; quarantine authorization is required."
                           : "Wardveil Security could not verify this download for release.");
      return;
    }

    const auto digest_after = sha256_file(found->second.partial_path);
    if (!digest_after || *digest_after != *digest_before) {
      decision.disposition = DownloadSecurityDisposition::block_unverified;
      decision.can_release = false;
      decision.can_open = false;
      decision.quarantine_required = false;
      decision.reason_codes = {"staged_content_changed_after_scan"};
      security_decisions_[id] = decision;
      mark_blocked(id, "Download bytes changed after security verification; release was blocked.");
      return;
    }

    const auto evidence = file_store_.commit(found->second, plan.metadata.total_bytes);
    if (!evidence.committed) {
      mark_failed(id, evidence.message);
      return;
    }
    progress.completed_bytes = evidence.final_size;
    progress.total_bytes = evidence.final_size;
    progress.bytes_per_second = 0.0;
    progress.state = DownloadState::completed;
    progress.message = "Download verified by Wardveil Security and committed to local storage.";
    queue_.set_state(id, DownloadState::completed);
    checkpoints_.erase(id);
    scheduled_waiting_.erase(id);
    persist_queue();
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

      if (record.state == DownloadState::held || record.state == DownloadState::blocked) {
        const auto size = file_store_.partial_size(*paths).value_or(0);
        progress_[restored->download_id] = {
            .completed_bytes = size,
            .total_bytes = size,
            .bytes_per_second = 0.0,
            .state = record.state,
            .message = record.state == DownloadState::held
                           ? "Held for Wardveil Security review."
                           : "Download remains blocked in staging by Wardveil Security.",
        };
        continue;
      }

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

  void mark_held(std::string_view download_id, std::string message) {
    const auto id = std::string{download_id};
    auto& progress = progress_[id];
    progress.state = DownloadState::held;
    progress.bytes_per_second = 0.0;
    progress.message = std::move(message);
    checkpoints_.erase(id);
    scheduled_waiting_.erase(id);
    queue_.set_state(id, DownloadState::held);
    persist_queue();
  }

  void mark_blocked(std::string_view download_id, std::string message) {
    const auto id = std::string{download_id};
    auto& progress = progress_[id];
    progress.state = DownloadState::blocked;
    progress.bytes_per_second = 0.0;
    progress.message = std::move(message);
    checkpoints_.erase(id);
    scheduled_waiting_.erase(id);
    queue_.set_state(id, DownloadState::blocked);
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
  WardveilDownloadScanner* wardveil_scanner_{nullptr};
  HttpDownloadTransport transport_;
  DownloadTransferScheduler scheduler_;
  std::unordered_map<std::string, DownloadFilePaths> files_;
  std::unordered_map<std::string, LiveDownloadProgress> progress_;
  std::unordered_map<std::string, ProgressSample> progress_samples_;
  std::unordered_map<std::string, DownloadSecurityDecision> security_decisions_;
  std::unordered_set<std::string> scheduled_waiting_;
};

}  // namespace goreecloud::browser
