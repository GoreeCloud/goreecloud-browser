#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "goreecloud/browser/advanced_download_runtime.hpp"

namespace goreecloud::browser {

enum class DownloadPanelFilter { all, active, queued, paused, verifying, held, blocked, completed, failed, cancelled };
enum class DownloadPanelSort { queue_order, filename, size, progress, speed };

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
  std::size_t verifying_count{0};
  std::size_t held_count{0};
  std::size_t blocked_count{0};
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
            static_cast<double>(row.completed_bytes) / static_cast<double>(row.total_bytes), 0.0, 1.0);
      }
      if (row.bytes_per_second > 0.0 && row.total_bytes > row.completed_bytes) {
        const auto remaining = static_cast<double>(row.total_bytes - row.completed_bytes);
        row.estimated_seconds_remaining = static_cast<std::uint64_t>(std::ceil(remaining / row.bytes_per_second));
      }
      row.can_pause = state == DownloadState::running || state == DownloadState::queued;
      row.can_resume = state == DownloadState::paused;
      row.can_cancel = state == DownloadState::running || state == DownloadState::queued ||
                       state == DownloadState::paused || state == DownloadState::failed ||
                       state == DownloadState::held || state == DownloadState::blocked;
      row.can_open = state == DownloadState::completed;
      switch (state) {
        case DownloadState::running: ++model.active_count; break;
        case DownloadState::queued: ++model.queued_count; break;
        case DownloadState::paused: ++model.paused_count; break;
        case DownloadState::verifying: ++model.verifying_count; break;
        case DownloadState::held: ++model.held_count; break;
        case DownloadState::blocked: ++model.blocked_count; break;
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

  static std::string format_text(const AdvancedDownloadPanelModel& model) {
    std::ostringstream out;
    out << "Advanced Download Manager\n"
        << "Active " << model.active_count
        << "  Queued " << model.queued_count
        << "  Paused " << model.paused_count
        << "  Verifying " << model.verifying_count
        << "  Held " << model.held_count
        << "  Blocked " << model.blocked_count
        << "  Completed " << model.completed_count
        << "  Failed " << model.failed_count << '\n';
    if (model.aggregate_bytes_per_second > 0.0) {
      out << "Total speed " << format_rate(model.aggregate_bytes_per_second) << '\n';
    }
    if (model.rows.empty()) {
      out << "No downloads.";
      return out.str();
    }
    for (const auto& row : model.rows) {
      out << "\n" << row.filename << " — " << state_label(row.state);
      if (row.total_bytes > 0) {
        out << " — " << static_cast<int>(std::round(row.progress_fraction * 100.0)) << "%"
            << " (" << format_bytes(row.completed_bytes) << " / " << format_bytes(row.total_bytes) << ")";
      }
      if (row.bytes_per_second > 0.0) out << " — " << format_rate(row.bytes_per_second);
      if (row.estimated_seconds_remaining) out << " — ETA " << format_eta(*row.estimated_seconds_remaining);
      if (!row.status_message.empty()) out << "\n  " << row.status_message;
      if (row.private_session) out << "\n  Private session";
    }
    return out.str();
  }

 private:
  static bool matches(DownloadPanelFilter filter, DownloadState state) {
    switch (filter) {
      case DownloadPanelFilter::all: return true;
      case DownloadPanelFilter::active: return state == DownloadState::running || state == DownloadState::verifying;
      case DownloadPanelFilter::queued: return state == DownloadState::queued;
      case DownloadPanelFilter::paused: return state == DownloadState::paused;
      case DownloadPanelFilter::verifying: return state == DownloadState::verifying;
      case DownloadPanelFilter::held: return state == DownloadState::held;
      case DownloadPanelFilter::blocked: return state == DownloadState::blocked;
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

  static const char* state_label(DownloadState state) {
    switch (state) {
      case DownloadState::queued: return "Queued";
      case DownloadState::running: return "Downloading";
      case DownloadState::paused: return "Paused";
      case DownloadState::completed: return "Completed";
      case DownloadState::failed: return "Failed";
      case DownloadState::cancelled: return "Cancelled";
      case DownloadState::verifying: return "Verifying";
      case DownloadState::held: return "Held for review";
      case DownloadState::blocked: return "Blocked";
    }
    return "Unknown";
  }

  static std::string format_bytes(std::uint64_t bytes) {
    constexpr double kb = 1024.0;
    constexpr double mb = kb * 1024.0;
    constexpr double gb = mb * 1024.0;
    std::ostringstream out;
    out << std::fixed << std::setprecision(1);
    if (bytes >= static_cast<std::uint64_t>(gb)) out << static_cast<double>(bytes) / gb << " GB";
    else if (bytes >= static_cast<std::uint64_t>(mb)) out << static_cast<double>(bytes) / mb << " MB";
    else if (bytes >= static_cast<std::uint64_t>(kb)) out << static_cast<double>(bytes) / kb << " KB";
    else out << bytes << " B";
    return out.str();
  }

  static std::string format_rate(double bytes_per_second) {
    if (bytes_per_second < 0.0) bytes_per_second = 0.0;
    return format_bytes(static_cast<std::uint64_t>(bytes_per_second)) + "/s";
  }

  static std::string format_eta(std::uint64_t seconds) {
    const auto hours = seconds / 3600;
    const auto minutes = (seconds % 3600) / 60;
    const auto remaining = seconds % 60;
    std::ostringstream out;
    if (hours > 0) out << hours << "h ";
    if (minutes > 0 || hours > 0) out << minutes << "m ";
    out << remaining << "s";
    return out.str();
  }
};

}  // namespace goreecloud::browser
