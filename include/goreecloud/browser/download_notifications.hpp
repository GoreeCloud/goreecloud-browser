#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace goreecloud::browser {

enum class DownloadNotificationKind { progress, completed, failed, paused, resumed };

struct DownloadNotification {
  DownloadNotificationKind kind{DownloadNotificationKind::progress};
  std::string download_id;
  std::string title;
  std::uint64_t bytes_per_second{};
  std::uint8_t percent{};
  bool persistent{false};
  bool sound{false};
  bool vibration{false};
};

class DownloadNotifier {
 public:
  virtual ~DownloadNotifier() = default;
  virtual void publish(const DownloadNotification& notification) = 0;
  virtual void dismiss(std::string_view download_id) = 0;
};

}  // namespace goreecloud::browser
