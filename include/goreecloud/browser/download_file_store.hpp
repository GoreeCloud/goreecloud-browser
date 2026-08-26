#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace goreecloud::browser {

struct DownloadFilePaths {
  std::filesystem::path partial_path;
  std::filesystem::path final_path;
};

struct DownloadCommitEvidence {
  bool committed{false};
  std::uint64_t final_size{0};
  std::string message;
};

class DownloadFileStore {
 public:
  virtual ~DownloadFileStore() = default;
  virtual std::optional<DownloadFilePaths> prepare(std::string_view download_id,
                                                   std::string_view filename,
                                                   std::uint64_t expected_size) = 0;
  virtual bool write_at(const DownloadFilePaths& paths,
                        std::uint64_t offset,
                        std::span<const std::byte> data) = 0;
  virtual std::optional<std::uint64_t> partial_size(const DownloadFilePaths& paths) const = 0;
  virtual DownloadCommitEvidence commit(const DownloadFilePaths& paths,
                                        std::uint64_t expected_size) = 0;
  virtual bool discard(const DownloadFilePaths& paths) = 0;
};

class LocalDownloadFileStore final : public DownloadFileStore {
 public:
  explicit LocalDownloadFileStore(std::filesystem::path download_directory)
      : download_directory_(std::move(download_directory)) {}

  std::optional<DownloadFilePaths> prepare(std::string_view download_id,
                                           std::string_view filename,
                                           std::uint64_t expected_size) override {
    if (download_id.empty() || filename.empty()) return std::nullopt;
    std::error_code error;
    std::filesystem::create_directories(download_directory_, error);
    if (error) return std::nullopt;

    const auto safe_filename = sanitize_filename(filename);
    if (safe_filename.empty()) return std::nullopt;

    DownloadFilePaths paths;
    paths.final_path = download_directory_ / safe_filename;
    paths.partial_path = download_directory_ /
                         (safe_filename + ".goreecloud-part-" + std::string{download_id});

    if (!std::filesystem::exists(paths.partial_path)) {
      std::ofstream stream(paths.partial_path, std::ios::binary | std::ios::trunc);
      if (!stream) return std::nullopt;
      if (expected_size > 0) {
        stream.seekp(static_cast<std::streamoff>(expected_size - 1));
        const char zero = 0;
        stream.write(&zero, 1);
      }
      if (!stream.good()) return std::nullopt;
    }
    return paths;
  }

  bool write_at(const DownloadFilePaths& paths,
                std::uint64_t offset,
                std::span<const std::byte> data) override {
    std::fstream stream(paths.partial_path,
                        std::ios::in | std::ios::out | std::ios::binary);
    if (!stream) return false;
    stream.seekp(static_cast<std::streamoff>(offset));
    if (!stream.good()) return false;
    stream.write(reinterpret_cast<const char*>(data.data()),
                 static_cast<std::streamsize>(data.size()));
    stream.flush();
    return stream.good();
  }

  std::optional<std::uint64_t> partial_size(const DownloadFilePaths& paths) const override {
    std::error_code error;
    const auto size = std::filesystem::file_size(paths.partial_path, error);
    if (error) return std::nullopt;
    return size;
  }

  DownloadCommitEvidence commit(const DownloadFilePaths& paths,
                                std::uint64_t expected_size) override {
    const auto size = partial_size(paths);
    if (!size) return {false, 0, "Partial download file is unavailable."};
    if (*size != expected_size) {
      return {false, *size, "Partial download size does not match expected size."};
    }

    std::error_code error;
    if (std::filesystem::exists(paths.final_path, error) && !error) {
      std::filesystem::remove(paths.final_path, error);
      if (error) return {false, *size, "Existing destination file could not be replaced."};
    }
    error.clear();
    std::filesystem::rename(paths.partial_path, paths.final_path, error);
    if (error) return {false, *size, "Partial download could not be committed atomically."};

    return {true, *size, "Download committed to local storage."};
  }

  bool discard(const DownloadFilePaths& paths) override {
    std::error_code error;
    if (!std::filesystem::exists(paths.partial_path, error)) return !error;
    std::filesystem::remove(paths.partial_path, error);
    return !error;
  }

 private:
  static std::string sanitize_filename(std::string_view filename) {
    std::string result;
    result.reserve(filename.size());
    for (const char ch : filename) {
      if (ch == '/' || ch == '\\' || ch == '\0') {
        result.push_back('_');
      } else {
        result.push_back(ch);
      }
    }
    while (!result.empty() && (result == "." || result == "..")) {
      result.insert(result.begin(), '_');
    }
    return result;
  }

  std::filesystem::path download_directory_;
};

}  // namespace goreecloud::browser
