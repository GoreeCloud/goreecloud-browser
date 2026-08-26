#pragma once

#include <cstddef>
#include <string>
#include <string_view>

namespace goreecloud::browser {

enum class LogSeverity { debug, info, warning, error, critical };

struct OperationalEvent {
  LogSeverity severity{LogSeverity::info};
  std::string category;
  std::string code;
  std::string detail;
};

struct LoggingPolicy {
  std::size_t active_file_limit_bytes{2U * 1024U * 1024U};
  std::size_t rotated_backups{3};
  bool remote_exporter_enabled{false};
  bool durable_private_browsing_logs{false};
};

// Implementations must reject sensitive/behavioral fields including URLs,
// queries, titles, page/form content, credentials, tokens, cookies, IPs,
// referrers, browsing history, and user file/download paths.
class OperationalLogger {
 public:
  virtual ~OperationalLogger() = default;
  [[nodiscard]] virtual LoggingPolicy policy() const = 0;
  virtual bool record(const OperationalEvent& event, bool private_context) = 0;
};

}  // namespace goreecloud::browser
