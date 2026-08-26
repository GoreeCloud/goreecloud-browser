#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace goreecloud::browser {

enum class TorrentSourceKind { torrent_file, magnet_link, local_file, url };
enum class TorrentState { queued, metadata, downloading, paused, seeding, completed, failed, stopped };

struct TorrentSource {
  TorrentSourceKind kind{TorrentSourceKind::magnet_link};
  std::string value;
};

struct TorrentFileEntry {
  std::uint32_t index{};
  std::string path;
  std::uint64_t size_bytes{};
  bool selected{true};
  std::int8_t priority{0};
};

struct TorrentPolicy {
  std::uint8_t simultaneous_torrents{3};
  std::optional<double> share_ratio_limit;
  std::optional<std::uint64_t> seeding_time_limit_seconds;
  bool seed_after_completion{true};
  std::string storage_directory;
};

struct TorrentRecord {
  std::string id;
  std::string name;
  TorrentState state{TorrentState::queued};
  std::uint64_t downloaded_bytes{};
  std::uint64_t uploaded_bytes{};
  std::optional<std::uint64_t> total_bytes;
  double ratio{};
  std::vector<TorrentFileEntry> files;
  std::vector<std::string> trackers;
};

class TorrentManager {
 public:
  virtual ~TorrentManager() = default;

  [[nodiscard]] virtual TorrentPolicy policy() const = 0;
  virtual void set_policy(TorrentPolicy policy) = 0;
  [[nodiscard]] virtual std::optional<TorrentRecord> add(const TorrentSource& source) = 0;
  [[nodiscard]] virtual std::vector<TorrentRecord> records() const = 0;
  virtual bool select_file(std::string_view torrent_id, std::uint32_t index, bool selected) = 0;
  virtual bool set_trackers(std::string_view torrent_id,
                            std::vector<std::string> trackers) = 0;
  virtual bool pause(std::string_view torrent_id) = 0;
  virtual bool resume(std::string_view torrent_id) = 0;
  virtual bool stop(std::string_view torrent_id) = 0;
  virtual bool remove(std::string_view torrent_id, bool delete_data) = 0;
};

// Torrent support is a general-purpose transfer capability. It must respect
// applicable authorization, platform policy, user intent, and lawful content
// boundaries; it is not a mechanism for bypassing access controls.

}  // namespace goreecloud::browser
