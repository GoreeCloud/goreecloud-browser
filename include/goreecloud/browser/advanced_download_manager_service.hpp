#pragma once

#include <optional>
#include <string>

namespace goreecloud::browser {

struct DownloadEnqueueRequest {
  std::string source_url;
  std::string referrer_url;
  std::optional<std::string> suggested_filename;
  bool private_session{false};
};

struct DownloadEnqueueResult {
  bool accepted{false};
  std::string download_id;
  std::string message;
};

class AdvancedDownloadManagerService {
 public:
  virtual ~AdvancedDownloadManagerService() = default;
  virtual DownloadEnqueueResult enqueue(DownloadEnqueueRequest request) = 0;
};

class UnavailableAdvancedDownloadManagerService final : public AdvancedDownloadManagerService {
 public:
  DownloadEnqueueResult enqueue(DownloadEnqueueRequest) override {
    return {false, {}, "Advanced Download Manager runtime adapter is not available."};
  }
};

}  // namespace goreecloud::browser
