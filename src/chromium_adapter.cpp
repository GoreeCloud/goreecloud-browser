#include "goreecloud/browser/chromium_adapter.hpp"

#include <stdexcept>
#include <utility>

namespace goreecloud::browser {

class ChromiumEngineAdapter::Impl {
 public:
  explicit Impl(ChromiumAdapterOptions options) : options_(std::move(options)) {}

  void initialize() {
    if (initialized_) return;
    if (options_.runtime_root.empty()) {
      throw std::runtime_error("Chromium runtime root is not configured");
    }
    if (!options_.enable_sandbox) {
      throw std::runtime_error("GoreeCloud Browser refuses to initialize Chromium with the sandbox disabled");
    }
    if (!options_.enable_site_isolation) {
      throw std::runtime_error("GoreeCloud Browser refuses to initialize Chromium with site isolation disabled");
    }

    // TODO(chromium): initialize Chromium/CEF/content runtime, subprocess
    // plumbing, message loop, resources, locales, GPU, sandbox, and site
    // isolation. This source file is intentionally compiled only when
    // GOREECLOUD_ENABLE_CHROMIUM=ON.
    initialized_ = true;
  }

  void shutdown() noexcept {
    if (!initialized_) return;
    // TODO(chromium): close contexts/views before runtime shutdown.
    initialized_ = false;
  }

  [[nodiscard]] bool initialized() const noexcept { return initialized_; }
  [[nodiscard]] const ChromiumAdapterOptions& options() const noexcept { return options_; }

 private:
  ChromiumAdapterOptions options_;
  bool initialized_{false};
};

ChromiumEngineAdapter::ChromiumEngineAdapter(ChromiumAdapterOptions options)
    : impl_(std::make_unique<Impl>(std::move(options))) {}

ChromiumEngineAdapter::~ChromiumEngineAdapter() = default;

void ChromiumEngineAdapter::initialize() { impl_->initialize(); }
void ChromiumEngineAdapter::shutdown() { impl_->shutdown(); }

std::string_view ChromiumEngineAdapter::name() const noexcept {
  return "GoreeCloud Browser Engine Layer (Chromium)";
}

std::string_view ChromiumEngineAdapter::version() const noexcept {
  return "integration-scaffold";
}

EngineCapabilities ChromiumEngineAdapter::capabilities() const noexcept {
  return capability(EngineCapability::WebView) |
         capability(EngineCapability::PrivateContexts) |
         capability(EngineCapability::SiteDataControl) |
         capability(EngineCapability::CookieInterception) |
         capability(EngineCapability::PermissionInterception) |
         capability(EngineCapability::DownloadInterception) |
         capability(EngineCapability::CertificateInspection) |
         capability(EngineCapability::RendererHealth) |
         capability(EngineCapability::HardwareAcceleration) |
         capability(EngineCapability::WebRTC) |
         capability(EngineCapability::Printing) |
         capability(EngineCapability::Accessibility) |
         capability(EngineCapability::PrivateContextCleanup) |
         capability(EngineCapability::OriginScopedCleanup) |
         capability(EngineCapability::AuthenticationStateCleanup) |
         capability(EngineCapability::PermissionStateCleanup);
}

std::unique_ptr<EngineContext> ChromiumEngineAdapter::create_context(
    const EngineContextOptions& options) {
  if (!impl_->initialized()) {
    throw std::logic_error("Chromium adapter must be initialized before creating contexts");
  }

  if (options.private_context && options.persistent_storage) {
    throw std::invalid_argument("Private Chromium contexts must use non-persistent storage");
  }

  // The adapter is now a compiled integration boundary, but a concrete
  // Chromium EngineContext is the next milestone. Returning nullptr is an
  // explicit not-yet-render-capable signal and must not be interpreted as a
  // production browser implementation.
  return nullptr;
}

}  // namespace goreecloud::browser
