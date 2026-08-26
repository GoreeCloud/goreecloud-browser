#pragma once

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "goreecloud/browser/advanced_download_manager_service.hpp"

namespace goreecloud::browser {

class DownloadQueueStore {
 public:
  virtual ~DownloadQueueStore() = default;
  virtual bool save(const std::vector<DownloadRecord>& records) = 0;
  [[nodiscard]] virtual std::vector<DownloadRecord> load() const = 0;
};

class FileDownloadQueueStore final : public DownloadQueueStore {
 public:
  explicit FileDownloadQueueStore(std::filesystem::path directory)
      : directory_(std::move(directory)) {}

  bool save(const std::vector<DownloadRecord>& records) override {
    std::error_code error;
    std::filesystem::create_directories(directory_, error);
    if (error) return false;
    const auto path = directory_ / "queue.tsv";
    const auto tmp = directory_ / "queue.tsv.tmp";
    std::ofstream stream(tmp, std::ios::trunc);
    if (!stream) return false;
    for (const auto& record : records) {
      stream << escape(record.download_id) << '\t'
             << escape(record.request.source_url) << '\t'
             << escape(record.request.referrer_url) << '\t'
             << escape(record.request.suggested_filename.value_or(std::string{})) << '\t'
             << (record.request.private_session ? 1 : 0) << '\t'
             << static_cast<int>(record.state) << '\t'
             << record.segment_limit << '\t'
             << (record.resumable ? 1 : 0) << '\t'
             << (record.request.scheduled_start_unix_seconds
                     ? std::to_string(*record.request.scheduled_start_unix_seconds)
                     : std::string{}) << '\n';
    }
    stream.flush();
    if (!stream.good()) return false;
    stream.close();
    std::filesystem::rename(tmp, path, error);
    if (error) {
      error.clear();
      std::filesystem::remove(path, error);
      error.clear();
      std::filesystem::rename(tmp, path, error);
    }
    return !error;
  }

  [[nodiscard]] std::vector<DownloadRecord> load() const override {
    std::vector<DownloadRecord> result;
    const auto path = directory_ / "queue.tsv";
    std::ifstream stream(path);
    if (!stream) return result;
    std::string line;
    while (std::getline(stream, line)) {
      const auto fields = split(line);
      if (fields.size() != 8 && fields.size() != 9) continue;
      DownloadRecord record;
      record.download_id = unescape(fields[0]);
      record.request.source_url = unescape(fields[1]);
      record.request.referrer_url = unescape(fields[2]);
      const auto filename = unescape(fields[3]);
      if (!filename.empty()) record.request.suggested_filename = filename;
      record.request.private_session = fields[4] == "1";
      try {
        record.state = static_cast<DownloadState>(std::stoi(fields[5]));
        record.segment_limit = static_cast<std::size_t>(std::stoull(fields[6]));
        if (fields.size() == 9 && !fields[8].empty()) {
          record.request.scheduled_start_unix_seconds = std::stoll(fields[8]);
        }
      } catch (...) {
        continue;
      }
      record.resumable = fields[7] == "1";
      if (!record.download_id.empty() && !record.request.source_url.empty()) result.push_back(std::move(record));
    }
    return result;
  }

 private:
  static std::string escape(std::string_view value) {
    std::string out;
    for (char ch : value) {
      if (ch == '\\') out += "\\\\";
      else if (ch == '\t') out += "\\t";
      else if (ch == '\n') out += "\\n";
      else out += ch;
    }
    return out;
  }

  static std::string unescape(std::string_view value) {
    std::string out;
    bool escape_next = false;
    for (char ch : value) {
      if (!escape_next && ch == '\\') { escape_next = true; continue; }
      if (escape_next) {
        if (ch == 't') out += '\t';
        else if (ch == 'n') out += '\n';
        else out += ch;
        escape_next = false;
      } else out += ch;
    }
    if (escape_next) out += '\\';
    return out;
  }

  static std::vector<std::string> split(std::string_view line) {
    std::vector<std::string> fields;
    std::string current;
    bool escape_next = false;
    for (char ch : line) {
      if (!escape_next && ch == '\\') {
        escape_next = true;
        current += ch;
        continue;
      }
      if (!escape_next && ch == '\t') {
        fields.push_back(current);
        current.clear();
        continue;
      }
      current += ch;
      escape_next = false;
    }
    fields.push_back(current);
    return fields;
  }

  std::filesystem::path directory_;
};

}  // namespace goreecloud::browser
