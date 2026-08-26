#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "goreecloud/browser/advanced_download_manager_service.hpp"
#include "goreecloud/browser/advanced_download_transfer_engine.hpp"
#include "goreecloud/browser/download_checkpoint_store.hpp"
#include "goreecloud/browser/download_file_store.hpp"
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
      : file_store_(std::move(download_directory)),
        transport_(client, [this](std::string_view download_id,
                                  std::uint64_t offset,
                                  std::span<const std::byte> bytes) {
          const auto found = files_.find(std::string{download_id});
          if (found == files_.end()) return false;
          return file_store_.write_at(found->second, offset, bytes);
        }),
        scheduler_(transport_) {
    scheduler_.set_progress_callback([this](const DownloadTransferPlan& plan) {
      checkpoints_.save(make_download_checkpoint(plan));
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
        progress.state = DownloadState::failed;
        progress.message = "Download file state is unavailable.";
        return;
      }
      const auto evidence = file_store_.commit(found->second, plan.metadata.total_bytes);
      if (!evidence.committed) {
        progress.state = DownloadState::failed;
        progress.message = evidence.message;
        return;
      }
      progress.completed_bytes = evidence.final_size;
      progress.total_bytes = evidence.final_size;
      progress.bytes_per_second = 0.0;
      progress.state = DownloadState::completed;
      progress.message = evidence.message;
      checkpoints_.erase(plan.download_id);
    });
    scheduler_.set_failure_callback([this](std::string_view download_id) {
      auto& progress = progress_[std::string{download_id}];
      progress.state = DownloadState::failed;
      progress.message = "Download transfer failed.";
    });
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
      return {false, {}, "Download destination could not be prepared."};
    }
    files_[record->download_id] = *paths;
    progress_[record->download_id] = {.state = DownloadState::queued, .message = "Queued"};
    if (!scheduler_.queue(*record)) {
      file_store_.discard(*paths);
      queue_.cancel(record->download_id);
      return {false, {}, "Download scheduler rejected the request."};
    }
    return queued;
  }

  void pump() { scheduler_.pump(); }

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

  static std::string default_filename(std::string_view url) {
    auto end = url.find_first_of("?#");
    if (end == std::string_view::npos) end = url.size();
    const auto slash = url.rfind('/', end == 0 ? 0 : end - 1);
    auto name = std::string{url.substr(slash == std::string_view::npos ? 0 : slash + 1,
                                       end - (slash == std::string_view::npos ? 0 : slash + 1))};
    if (name.empty()) name = "download.bin";
    return name;
  }

  InProcessAdvancedDownloadManagerService queue_;
  LocalDownloadFileStore file_store_;
  InMemoryDownloadCheckpointStore checkpoints_;
  HttpDownloadTransport transport_;
  DownloadTransferScheduler scheduler_;
  std::unordered_map<std::string, DownloadFilePaths> files_;
  std::unordered_map<std::string, LiveDownloadProgress> progress_;
  std::unordered_map<std::string, ProgressSample> progress_samples_;
};

}  // namespace goreecloud::browser
