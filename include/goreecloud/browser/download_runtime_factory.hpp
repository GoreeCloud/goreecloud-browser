#pragma once

#include <cstdlib>
#include <filesystem>
#include <memory>

#include "goreecloud/browser/advanced_download_manager_service.hpp"

#if defined(GOREECLOUD_ENABLE_CURL_DOWNLOAD_TRANSPORT) && GOREECLOUD_ENABLE_CURL_DOWNLOAD_TRANSPORT
#include "goreecloud/browser/advanced_download_runtime.hpp"
#include "goreecloud/browser/curl_http_download_client.hpp"
#endif

namespace goreecloud::browser {

class DownloadRuntimeHandle {
 public:
  virtual ~DownloadRuntimeHandle() = default;
  virtual AdvancedDownloadManagerService& service() = 0;
  virtual void pump() = 0;
};

class QueueOnlyDownloadRuntime final : public DownloadRuntimeHandle {
 public:
  AdvancedDownloadManagerService& service() override { return queue_; }
  void pump() override {}
 private:
  InProcessAdvancedDownloadManagerService queue_;
};

#if defined(GOREECLOUD_ENABLE_CURL_DOWNLOAD_TRANSPORT) && GOREECLOUD_ENABLE_CURL_DOWNLOAD_TRANSPORT
class CurlDownloadRuntime final : public DownloadRuntimeHandle {
 public:
  explicit CurlDownloadRuntime(std::filesystem::path directory)
      : runtime_(client_, std::move(directory)) {}
  AdvancedDownloadManagerService& service() override { return runtime_; }
  void pump() override { runtime_.pump(); }
  [[nodiscard]] AdvancedDownloadRuntimeService& runtime() noexcept { return runtime_; }
 private:
  CurlHttpDownloadClient client_;
  AdvancedDownloadRuntimeService runtime_;
};
#endif

inline std::filesystem::path default_download_directory() {
#if defined(_WIN32)
  if (const char* profile = std::getenv("USERPROFILE")) {
    return std::filesystem::path{profile} / "Downloads";
  }
#else
  if (const char* home = std::getenv("HOME")) {
    return std::filesystem::path{home} / "Downloads";
  }
#endif
  return std::filesystem::current_path() / "Downloads";
}

inline std::unique_ptr<DownloadRuntimeHandle> make_download_runtime() {
#if defined(GOREECLOUD_ENABLE_CURL_DOWNLOAD_TRANSPORT) && GOREECLOUD_ENABLE_CURL_DOWNLOAD_TRANSPORT
  return std::make_unique<CurlDownloadRuntime>(default_download_directory());
#else
  return std::make_unique<QueueOnlyDownloadRuntime>();
#endif
}

}  // namespace goreecloud::browser
