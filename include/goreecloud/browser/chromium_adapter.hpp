#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>

#include "goreecloud/browser/engine.hpp"

namespace goreecloud::browser {

struct ChromiumAdapterOptions {
  std::filesystem::path runtime_root;
  std::filesystem::path subprocess_path;
  std::filesystem::path resources_path;
  std::filesystem::path locales_path;
  bool enable_gpu{true};
  bool enable_sandbox{true};
  bool enable_site_isolation{true};
};

// ChromiumEngineAdapter is the implementation boundary between GoreeCloud
// Browser and Chromium/Blink. Chromium types must remain behind this boundary.
// The Browser core, Glaze UI, tabs, sessions, profiles, services, and policies
// must not depend directly on Chromium public/internal types.
class ChromiumEngineAdapter : public BrowserEngine {
 public:
  explicit ChromiumEngineAdapter(ChromiumAdapterOptions options);
  ~ChromiumEngineAdapter() override;

  void initialize() override;
  void shutdown() override;

  [[nodiscard]] std::string_view name() const noexcept override;
  [[nodiscard]] std::string_view version() const noexcept override;
  [[nodiscard]] EngineCapabilities capabilities() const noexcept override;
  [[nodiscard]] std::unique_ptr<EngineContext> create_context(
      const EngineContextOptions& options) override;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace goreecloud::browser
