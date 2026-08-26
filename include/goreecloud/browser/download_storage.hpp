#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace goreecloud::browser {

enum class DownloadCategory { video, audio, image, document, archive, application, torrent, other };

struct DownloadDestinationRule {
  DownloadCategory category{DownloadCategory::other};
  std::string directory;
};

struct StorageLocation {
  std::string id;
  std::string path;
  bool removable{false};
  bool writable{false};
  bool available{false};
};

struct StoragePolicy {
  std::string default_directory;
  std::string torrent_directory;
  std::vector<DownloadDestinationRule> routing_rules;
  bool auto_categorize{true};
};

class DownloadStorageManager {
 public:
  virtual ~DownloadStorageManager() = default;
  [[nodiscard]] virtual StoragePolicy policy() const = 0;
  virtual void set_policy(StoragePolicy policy) = 0;
  [[nodiscard]] virtual std::vector<StorageLocation> locations() const = 0;
  [[nodiscard]] virtual std::optional<std::string> resolve_destination(
      DownloadCategory category, std::string_view suggested_filename) const = 0;
  virtual bool open_completed_file(std::string_view download_id) = 0;
  virtual bool open_containing_folder(std::string_view download_id) = 0;
};

}  // namespace goreecloud::browser
