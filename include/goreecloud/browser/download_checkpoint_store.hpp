#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "goreecloud/browser/advanced_download_transfer_engine.hpp"

namespace goreecloud::browser {

struct DownloadSegmentCheckpoint {
  std::size_t index{0};
  DownloadByteRange range;
  std::uint64_t completed_bytes{0};
  unsigned retry_count{0};
  bool finished{false};
};

struct DownloadCheckpoint {
  std::string download_id;
  std::string source_url;
  std::string referrer_url;
  std::uint64_t total_bytes{0};
  std::uint64_t completed_bytes{0};
  std::optional<std::string> etag;
  std::optional<std::string> last_modified;
  std::vector<DownloadSegmentCheckpoint> segments;
};

class DownloadCheckpointStore {
 public:
  virtual ~DownloadCheckpointStore() = default;
  virtual bool save(DownloadCheckpoint checkpoint) = 0;
  [[nodiscard]] virtual std::optional<DownloadCheckpoint> load(
      std::string_view download_id) const = 0;
  virtual bool erase(std::string_view download_id) = 0;
};

class InMemoryDownloadCheckpointStore final : public DownloadCheckpointStore {
 public:
  bool save(DownloadCheckpoint checkpoint) override {
    if (checkpoint.download_id.empty()) return false;
    checkpoints_.insert_or_assign(checkpoint.download_id, std::move(checkpoint));
    return true;
  }

  [[nodiscard]] std::optional<DownloadCheckpoint> load(
      std::string_view download_id) const override {
    const auto found = checkpoints_.find(std::string{download_id});
    if (found == checkpoints_.end()) return std::nullopt;
    return found->second;
  }

  bool erase(std::string_view download_id) override {
    return checkpoints_.erase(std::string{download_id}) > 0;
  }

 private:
  std::unordered_map<std::string, DownloadCheckpoint> checkpoints_;
};

inline DownloadCheckpoint make_download_checkpoint(const DownloadTransferPlan& plan) {
  DownloadCheckpoint checkpoint;
  checkpoint.download_id = plan.download_id;
  checkpoint.source_url = plan.source_url;
  checkpoint.referrer_url = plan.referrer_url;
  checkpoint.total_bytes = plan.metadata.total_bytes;
  checkpoint.completed_bytes = plan.completed_bytes;
  checkpoint.etag = plan.metadata.etag;
  checkpoint.last_modified = plan.metadata.last_modified;
  checkpoint.segments.reserve(plan.segments.size());
  for (const auto& segment : plan.segments) {
    checkpoint.segments.push_back({
        .index = segment.index,
        .range = segment.range,
        .completed_bytes = segment.completed_bytes,
        .retry_count = segment.retry_count,
        .finished = segment.finished,
    });
  }
  return checkpoint;
}

inline bool apply_download_checkpoint(DownloadTransferPlan& plan,
                                      const DownloadCheckpoint& checkpoint) {
  if (plan.download_id != checkpoint.download_id ||
      plan.source_url != checkpoint.source_url ||
      plan.metadata.total_bytes != checkpoint.total_bytes ||
      plan.metadata.etag != checkpoint.etag ||
      plan.metadata.last_modified != checkpoint.last_modified ||
      plan.segments.size() != checkpoint.segments.size()) {
    return false;
  }

  std::uint64_t total_completed = 0;
  for (std::size_t i = 0; i < plan.segments.size(); ++i) {
    auto& segment = plan.segments[i];
    const auto& saved = checkpoint.segments[i];
    if (segment.index != saved.index ||
        segment.range.begin != saved.range.begin ||
        segment.range.end_inclusive != saved.range.end_inclusive) {
      return false;
    }
    const auto segment_size = segment.range.end_inclusive - segment.range.begin + 1;
    if (saved.completed_bytes > segment_size) return false;
    segment.completed_bytes = saved.completed_bytes;
    segment.retry_count = saved.retry_count;
    segment.finished = saved.finished || saved.completed_bytes == segment_size;
    total_completed += segment.completed_bytes;
  }
  plan.completed_bytes = total_completed;
  return true;
}

}  // namespace goreecloud::browser
