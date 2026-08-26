#pragma once

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <optional>
#include <string>
#include <string_view>

#include "goreecloud/browser/download_checkpoint_store.hpp"

namespace goreecloud::browser {

class FileDownloadCheckpointStore final : public DownloadCheckpointStore {
 public:
  explicit FileDownloadCheckpointStore(std::filesystem::path directory)
      : directory_(std::move(directory)) {}

  bool save(DownloadCheckpoint checkpoint) override {
    if (checkpoint.download_id.empty()) return false;
    std::error_code error;
    std::filesystem::create_directories(directory_, error);
    if (error) return false;

    const auto final_path = path_for(checkpoint.download_id);
    const auto temp_path = final_path.string() + ".tmp";
    std::ofstream out(temp_path, std::ios::trunc);
    if (!out) return false;

    out << std::quoted(checkpoint.download_id) << '\n'
        << std::quoted(checkpoint.source_url) << '\n'
        << std::quoted(checkpoint.referrer_url) << '\n'
        << checkpoint.total_bytes << ' ' << checkpoint.completed_bytes << '\n'
        << std::quoted(checkpoint.etag.value_or("")) << '\n'
        << std::quoted(checkpoint.last_modified.value_or("")) << '\n'
        << checkpoint.segments.size() << '\n';
    for (const auto& segment : checkpoint.segments) {
      out << segment.index << ' '
          << segment.range.begin << ' '
          << segment.range.end_inclusive << ' '
          << segment.completed_bytes << ' '
          << segment.retry_count << ' '
          << (segment.finished ? 1 : 0) << '\n';
    }
    out.flush();
    if (!out.good()) return false;
    out.close();

    std::filesystem::rename(temp_path, final_path, error);
    if (error) {
      std::filesystem::remove(final_path, error);
      error.clear();
      std::filesystem::rename(temp_path, final_path, error);
    }
    return !error;
  }

  [[nodiscard]] std::optional<DownloadCheckpoint> load(
      std::string_view download_id) const override {
    std::ifstream in(path_for(download_id));
    if (!in) return std::nullopt;

    DownloadCheckpoint checkpoint;
    std::string etag;
    std::string last_modified;
    std::size_t segment_count = 0;
    if (!(in >> std::quoted(checkpoint.download_id))) return std::nullopt;
    if (!(in >> std::quoted(checkpoint.source_url))) return std::nullopt;
    if (!(in >> std::quoted(checkpoint.referrer_url))) return std::nullopt;
    if (!(in >> checkpoint.total_bytes >> checkpoint.completed_bytes)) return std::nullopt;
    if (!(in >> std::quoted(etag))) return std::nullopt;
    if (!(in >> std::quoted(last_modified))) return std::nullopt;
    if (!(in >> segment_count)) return std::nullopt;
    if (checkpoint.download_id != download_id) return std::nullopt;
    if (!etag.empty()) checkpoint.etag = std::move(etag);
    if (!last_modified.empty()) checkpoint.last_modified = std::move(last_modified);

    checkpoint.segments.reserve(segment_count);
    for (std::size_t i = 0; i < segment_count; ++i) {
      DownloadSegmentCheckpoint segment;
      int finished = 0;
      if (!(in >> segment.index >> segment.range.begin >> segment.range.end_inclusive >>
            segment.completed_bytes >> segment.retry_count >> finished)) {
        return std::nullopt;
      }
      segment.finished = finished != 0;
      checkpoint.segments.push_back(segment);
    }
    return checkpoint;
  }

  bool erase(std::string_view download_id) override {
    std::error_code error;
    if (!std::filesystem::exists(path_for(download_id), error)) return !error;
    return std::filesystem::remove(path_for(download_id), error) && !error;
  }

 private:
  [[nodiscard]] std::filesystem::path path_for(std::string_view download_id) const {
    std::string safe;
    safe.reserve(download_id.size());
    for (const char ch : download_id) {
      if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
          (ch >= '0' && ch <= '9') || ch == '-' || ch == '_') {
        safe.push_back(ch);
      } else {
        safe.push_back('_');
      }
    }
    return directory_ / (safe + ".checkpoint");
  }

  std::filesystem::path directory_;
};

}  // namespace goreecloud::browser
