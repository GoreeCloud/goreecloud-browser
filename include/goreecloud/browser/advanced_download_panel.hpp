#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "goreecloud/browser/advanced_download_runtime.hpp"

namespace goreecloud::browser {

enum class DownloadPanelFilter {
  all,
  active,
  queued,
  paused,
  completed,
  failed,
  cancelled,
};

enum class DownloadPanelSort {
  queue_order,
  filename,
  size,
  progress,
  speed,
};

struct DownloadPanelRow {
  std::string download_id;
  std::string filename;
  std::string source_url;
  DownloadState state{DownloadState::queued};
  std::uint64_t completed_bytes{0};
  std::uint64_t total_bytes{0};
  double progress_fraction{0.0};
  double bytes_per_second{0.0};
  std::optional<std::uint64_t> estimated_seconds_remaining;
  bool can_pause{false};
  bool can_resume{false};
  bool can_cancel{false};
  bool can_open{false};
  bool private_session{false};
  std::string status_message;
};

struct AdvancedDownloadPanelModel {
  std::vector<DownloadPanelRow> rows;
  std::size_t active_count{0};
  std::size_t queued_count{0};
  std::size_t paused_count{0};
  std::size_t completed_count{0};
  std::size_t failed_count{0};
  double aggregate_bytes_per_second{0.0};
};

class AdvancedDownloadPanelBuilder {
 public:
  static AdvancedDownloadPanelModel build(
      const AdvancedDownloadRuntimeService& runtime,
      DownloadPanelFilter filter = DownloadPanelFilter::all,
      DownloadPanelSort sort = DownloadPanelSort::queue_order) {
    AdvancedDownloadPanelModel model;
    const auto records = runtime.snapshot();
    model.rows.reserve(records.size());

    for (const auto& record : records) {
      const auto live = runtime.progress(record.download_id);
      const auto state = live ? live->state : record.state;
      if (!matches(filter, state)) continue;

      DownloadPanelRow row;
      row.download_id = record.download_id;
      row.filename = record.request.suggested_filename.value_or(filename_from_url(record.request.source_url));
      row.source_url = record.request.source_url;
      row.state = state;
      row.private_session = record.request.private_session;
      if (live) {
        row.completed_bytes = live->completed_bytes;
        row.total_bytes = live->total_bytes;
        row.bytes_per_second = live->bytes_per_second;
        row.status_message = live->message;
      }
      if (row.total_bytes > 0) {
        row.progress_fraction = std::clamp(
            static_cast<double>(row.completed_bytes) / static_cast<double>(row.total_bytes),
            0.0, 1.0);
      }
      if (row.bytes_per_second > 0.0 && row.total_bytes > row.completed_bytes) {
        const auto remaining = static_cast<double>(row.total_bytes - row.completed_bytes);
        row.estimated_seconds_remaining = static_cast<std::uint64_t>(
            std::ceil(remaining / row.bytes_per_second));
      }

      row.can_pause = state == DownloadState::running || state == DownloadState::queued;
      row.can_resume = state == DownloadState::paused;
      row.can_cancel = state == DownloadState::running || state == DownloadState::queued ||
                       state == DownloadState::paused || state == DownloadState::failed;
      row.can_open = state == DownloadState::completed;

      switch (state) {
        case DownloadState::running: ++model.active_count; break;
        case DownloadState::queued: ++model.queued_count; break;
        case DownloadState::paused: ++model.paused_count; break;
        case DownloadState::completed: ++model.completed_count; break;
        case DownloadState::failed: ++model.failed_count; break;
        case DownloadState::cancelled: break;
      }
      model.aggregate_bytes_per_second += row.bytes_per_second;
      model.rows.push_back(std::move(row));
    }

    sort_rows(model.rows, sort);
    return model;
  }

 private:
  static bool matches(DownloadPanelFilter filter, DownloadState state) {
    switch (filter) {
      case DownloadPanelFilter::all: return true;
      case DownloadPanelFilter::active: return state == DownloadState::running;
      case DownloadPanelFilter::queued: return state == DownloadState::queued;
      case DownloadPanelFilter::paused: return state == DownloadState::paused;
      case DownloadPanelFilter::completed: return state == DownloadState::completed;
      case DownloadPanelFilter::failed: return state == DownloadState::failed;
      case DownloadPanelFilter::cancelled: return state == DownloadState::cancelled;
    }
    return false;
  }

  static void sort_rows(std::vector<DownloadPanelRow>& rows, DownloadPanelSort sort) {
    if (sort == DownloadPanelSort::queue_order) return;
    std::stable_sort(rows.begin(), rows.end(), [sort](const auto& left, const auto& right) {
      switch (sort) {
        case DownloadPanelSort::filename: return left.filename < right.filename;
        case DownloadPanelSort::size: return left.total_bytes > right.total_bytes;
        case DownloadPanelSort::progress: return left.progress_fraction > right.progress_fraction;
        case DownloadPanelSort::speed: return left.bytes_per_second > right.bytes_per_second;
        case DownloadPanelSort::queue_order: return false;
      }
      return false;
    });
  }

  static std::string filename_from_url(std::string_view url) {
    auto end = url.find_first_of("?#");
    if (end == std::string_view::npos) end = url.size();
    const auto slash = url.rfind('/', end == 0 ? 0 : end - 1);
    const auto start = slash == std::string_view::npos ? 0 : slash + 1;
    auto filename = std::string{url.substr(start, end - start)};
    if (filename.empty()) filename = "download";
    return filename;
  }
};

}  // namespace goreecloud::browser
