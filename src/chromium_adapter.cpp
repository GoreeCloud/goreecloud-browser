#include "goreecloud/browser/chromium_adapter.hpp"

#include <memory>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <utility>

#include "goreecloud/browser/chromium_runtime_delegate.hpp"
#include "goreecloud/browser/chromium_runtime_factory.hpp"
#include "goreecloud/browser/native_engine_surface.hpp"

namespace goreecloud::browser {
namespace {

class ChromiumEngineView final : public EngineView, public NativeSurfaceAttachable {
 public:
  explicit ChromiumEngineView(std::unique_ptr<ChromiumRuntimeView> runtime_view)
      : runtime_view_(std::move(runtime_view)) {
    if (!runtime_view_) {
      throw std::invalid_argument("ChromiumEngineView requires a runtime view");
    }
  }

  void navigate(std::string_view url) override { runtime_view_->navigate(url); }
  void reload() override { runtime_view_->reload(true); }
  void stop() override { runtime_view_->stop(); }
  void go_back() override { runtime_view_->go_back(); }
  void go_forward() override { runtime_view_->go_forward(); }
  void set_zoom(double factor) override { runtime_view_->set_zoom(factor); }
  void find(std::string_view query, bool forward) override {
    runtime_view_->find(query, forward);
  }
  void stop_find() override { runtime_view_->stop_find(); }

  [[nodiscard]] NavigationState navigation_state() const override {
    return runtime_view_->navigation_state();
  }

  [[nodiscard]] RendererHealth renderer_health() const override {
    return runtime_view_->renderer_health();
  }

  bool attach_native_surface(const NativeEngineSurface& surface) override {
    return runtime_view_->attach_surface(surface);
  }

  void resize_native_surface(const NativeEngineSurface& surface) override {
    runtime_view_->resize_surface(surface);
  }

  void detach_native_surface() override { runtime_view_->detach_surface(); }

  [[nodiscard]] bool native_surface_attached() const noexcept override {
    // Runtime surface state is intentionally kept behind the Chromium boundary;
    // the Browser host treats successful attach as authoritative until detach.
    return attached_;
  }

  bool attach_and_track(const NativeEngineSurface& surface) {
    attached_ = runtime_view_->attach_surface(surface);
    return attached_;
  }

 private:
  std::unique_ptr<ChromiumRuntimeView> runtime_view_;
  bool attached_{false};
};

class ChromiumEngineContext final : public EngineContext {
 public:
  explicit ChromiumEngineContext(std::unique_ptr<ChromiumRuntimeContext> runtime_context)
      : runtime_context_(std::move(runtime_context)) {
    if (!runtime_context_) {
      throw std::invalid_argument("ChromiumEngineContext requires a runtime context");
    }
  }

  [[nodiscard]] std::unique_ptr<EngineView> create_view(
      const EngineViewOptions& options) override {
    auto runtime_view = runtime_context_->create_view(options);
    if (!runtime_view) return nullptr;
    return std::make_unique<ChromiumEngineView>(std::move(runtime_view));
  }

  bool clear_origin_data(std::string_view origin,
                         EngineDataClasses classes) override {
    return runtime_context_->clear_origin_data(origin, classes);
  }

  bool clear_all_data(EngineDataClasses classes) override {
    return runtime_context_->clear_all_data(classes);
  }

  bool clear_authentication_state(std::optional<std::string_view> origin) override {
    return runtime_context_->clear_authentication_state(origin);
  }

  bool clear_permission_state(std::optional<std::string_view> origin) override {
    return runtime_context_->clear_permission_state(origin);
  }

 private:
  std::unique_ptr<ChromiumRuntimeContext> runtime_context_;
};

}  // namespace

class ChromiumEngineAdapter::Impl {
 public:
  explicit Impl(ChromiumAdapterOptions options)
      : options_(std::move(options)),
        runtime_(create_chromium_runtime_delegate(options_)) {
    if (!runtime_) {
      throw std::runtime_error("Failed to create Chromium runtime delegate");
    }
  }

  void initialize() {
    if (initialized_) return;
    if (options_.runtime_root.empty()) {
      throw std::runtime_error("Chromium runtime root is not configured");
    }
    if (!options_.enable_sandbox) {
      throw std::runtime_error(
          "GoreeCloud Browser refuses to initialize Chromium with the sandbox disabled");
    }
    if (!options_.enable_site_isolation) {
      throw std::runtime_error(
          "GoreeCloud Browser refuses to initialize Chromium with site isolation disabled");
    }
    runtime_->initialize();
    initialized_ = true;
  }

  void shutdown() noexcept {
    if (!initialized_) return;
    runtime_->shutdown();
    initialized_ = false;
  }

  [[nodiscard]] bool initialized() const noexcept { return initialized_; }
  [[nodiscard]] ChromiumRuntimeDelegate& runtime() noexcept { return *runtime_; }
  [[nodiscard]] const ChromiumRuntimeDelegate& runtime() const noexcept {
    return *runtime_;
  }

 private:
  ChromiumAdapterOptions options_;
  std::unique_ptr<ChromiumRuntimeDelegate> runtime_;
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
  return impl_->runtime().runtime_version();
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
    throw std::logic_error(
        "Chromium adapter must be initialized before creating contexts");
  }
  if (options.private_context && options.persistent_storage) {
    throw std::invalid_argument(
        "Private Chromium contexts must use non-persistent storage");
  }
  if (!options.private_context && options.persistent_storage &&
      options.storage_path.empty()) {
    throw std::invalid_argument(
        "Persistent Chromium contexts require a storage path");
  }

  auto runtime_context = impl_->runtime().create_context(options);
  if (!runtime_context) return nullptr;
  return std::make_unique<ChromiumEngineContext>(std::move(runtime_context));
}

}  // namespace goreecloud::browser
