#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace goreecloud::browser {

enum class PostDownloadAction { none, notify, open_file, open_folder, stop_queue, shutdown_when_supported };

enum class ScheduleKind { one_time, recurring };

struct DownloadSchedule {
  std::string id;
  ScheduleKind kind{ScheduleKind::one_time};
  std::string expression;
  bool enabled{true};
  std::optional<std::string> queue_id;
};

struct BatteryPolicy {
  bool pause_on_low_battery{false};
  std::uint8_t minimum_percent{15};
};

struct AutomationPolicy {
  BatteryPolicy battery;
  bool resume_after_network_recovery{true};
  bool backup_download_list{true};
  bool backup_settings{true};
  PostDownloadAction default_post_action{PostDownloadAction::notify};
};

class DownloadAutomation {
 public:
  virtual ~DownloadAutomation() = default;
  [[nodiscard]] virtual AutomationPolicy policy() const = 0;
  virtual void set_policy(AutomationPolicy policy) = 0;
  [[nodiscard]] virtual std::vector<DownloadSchedule> schedules() const = 0;
  virtual bool add_schedule(DownloadSchedule schedule) = 0;
  virtual bool remove_schedule(std::string_view id) = 0;
  virtual bool import_batch_urls(const std::vector<std::string>& urls) = 0;
  virtual bool backup_state(std::string_view destination) = 0;
  virtual bool restore_state(std::string_view source) = 0;
};

}  // namespace goreecloud::browser
