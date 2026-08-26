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

struct ChromiumPrivateContextRequirements {
  bool in_memory_or_ephemeral_storage{true};
  bool isolate_cookies{true};
  bool isolate_http_cache{true};
  bool isolate_local_storage{true};
  bool isolate_indexed_db{true};
  bool isolate_service_workers{true};
  bool isolate_authentication_state{true};
  bool isolate_permissions{true};
  bool support_origin_scoped_cleanup{true};
  bool support_full_context_cleanup{true};
};

inline constexpr ChromiumPrivateContextRequirements kChromiumPrivateContextRequirements{};

// ChromiumEngineAdapter is the implementation boundary between GoreeCloud
// Browser and Chromium/Blink. Chromium types must remain behind this boundary.
// The Browser core, Glaze UI, tabs, sessions, profiles, services, and policies
// must not depend directly on Chromium public/internal types.
//
// Private EngineContext implementations must map GoreeCloud cleanup contracts
// onto Chromium storage partitions/browser contexts and must not report a
// successful cleanup unless the requested data classes were actually cleared.
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

static_assert(kChromiumPrivateContextRequirements.in_memory_or_ephemeral_storage);
static_assert(kChromiumPrivateContextRequirements.isolate_cookies);
static_assert(kChromiumPrivateContextRequirements.isolate_authentication_state);
static_assert(kChromiumPrivateContextRequirements.support_origin_scoped_cleanup);
static_assert(kChromiumPrivateContextRequirements.support_full_context_cleanup);

}  // namespace goreecloud::browser
