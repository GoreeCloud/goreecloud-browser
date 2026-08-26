#include "goreecloud/browser/chromium_adapter.hpp"

#include <algorithm>
#include <cstddef>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "goreecloud/browser/native_engine_surface.hpp"

namespace goreecloud::browser {

namespace {

class ChromiumEngineView final : public EngineView, public NativeSurfaceAttachable {
 public:
  explicit ChromiumEngineView(EngineViewOptions options)
      : allow_popups_(options.allow_popups) {
    navigate(options.initial_url);
  }

  void navigate(std::string_view url) override {
    if (history_index_ + 1 < history_.size()) {
      history_.erase(history_.begin() + static_cast<std::ptrdiff_t>(history_index_ + 1),
                     history_.end());
    }
    history_.push_back(std::string{url});
    history_index_ = history_.size() - 1;
    navigation_.url = history_.back();
    navigation_.title = navigation_.url;
    navigation_.loading = false;
    navigation_.progress = 1.0;
    update_navigation_flags();

    // TODO(chromium-runtime): forward navigation to the concrete Chromium
    // WebContents/CEF browser instance once runtime binding is linked.
  }

  void reload() override {
    // TODO(chromium-runtime): use approved hard-refresh/skip-cache semantics.
    navigation_.loading = false;
    navigation_.progress = 1.0;
  }

  void stop() override { navigation_.loading = false; }

  void go_back() override {
    if (history_index_ == 0) return;
    --history_index_;
    sync_history();
  }

  void go_forward() override {
    if (history_index_ + 1 >= history_.size()) return;
    ++history_index_;
    sync_history();
  }

  void set_zoom(double factor) override { zoom_factor_ = std::clamp(factor, 0.25, 5.0); }

  void find(std::string_view query, bool forward) override {
    last_find_query_ = std::string{query};
    last_find_forward_ = forward;
  }

  void stop_find() override { last_find_query_.clear(); }

  [[nodiscard]] NavigationState navigation_state() const override { return navigation_; }
  [[nodiscard]] RendererHealth renderer_health() const override { return health_; }

  bool attach_native_surface(const NativeEngineSurface& surface) override {
    if (surface.width <= 0 || surface.height <= 0) return false;
    surface_ = surface;
    surface_attached_ = true;
    // TODO(chromium-runtime): create/attach the concrete child rendering
    // surface using platform-specific Chromium APIs.
    return true;
  }

  void resize_native_surface(const NativeEngineSurface& surface) override {
    if (!surface_attached_) return;
    surface_ = surface;
    // TODO(chromium-runtime): propagate bounds/scale changes to Chromium.
  }

  void detach_native_surface() override {
    // TODO(chromium-runtime): detach/destroy the native Chromium child view.
    surface_attached_ = false;
    surface_.reset();
  }

  [[nodiscard]] bool native_surface_attached() const noexcept override {
    return surface_attached_;
  }

 private:
  void sync_history() {
    navigation_.url = history_[history_index_];
    navigation_.title = navigation_.url;
    navigation_.loading = false;
    navigation_.progress = 1.0;
    update_navigation_flags();
  }

  void update_navigation_flags() {
    navigation_.can_go_back = history_index_ > 0;
    navigation_.can_go_forward = history_index_ + 1 < history_.size();
  }

  NavigationState navigation_;
  RendererHealth health_{RendererHealth::Healthy};
  std::vector<std::string> history_;
  std::size_t history_index_{0};
  double zoom_factor_{1.0};
  std::string last_find_query_;
  bool last_find_forward_{true};
  bool allow_popups_{false};
  bool surface_attached_{false};
  std::optional<NativeEngineSurface> surface_;
};

class ChromiumEngineContext final : public EngineContext {
 public:
  explicit ChromiumEngineContext(EngineContextOptions options)
      : options_(std::move(options)) {}

  [[nodiscard]] std::unique_ptr<EngineView> create_view(
      const EngineViewOptions& options) override {
    return std::make_unique<ChromiumEngineView>(options);
  }

  bool clear_origin_data(std::string_view origin,
                         EngineDataClasses classes) override {
    cleared_origin_classes_[std::string{origin}] |= classes;
    // TODO(chromium-runtime): map to StoragePartition/BrowsingDataRemover.
    return true;
  }

  bool clear_all_data(EngineDataClasses classes) override {
    cleared_all_classes_ |= classes;
    // TODO(chromium-runtime): clear the context StoragePartition.
    return true;
  }

  bool clear_authentication_state(std::optional<std::string_view> origin) override {
    if (origin) cleared_auth_origins_.push_back(std::string{*origin});
    else cleared_all_authentication_ = true;
    // TODO(chromium-runtime): clear HTTP auth/client auth/session credentials.
    return true;
  }

  bool clear_permission_state(std::optional<std::string_view> origin) override {
    if (origin) cleared_permission_origins_.push_back(std::string{*origin});
    else cleared_all_permissions_ = true;
    // TODO(chromium-runtime): clear ContentSettings/private session grants.
    return true;
  }

 private:
  EngineContextOptions options_;
  EngineDataClasses cleared_all_classes_{0};
  std::unordered_map<std::string, EngineDataClasses> cleared_origin_classes_;
  std::vector<std::string> cleared_auth_origins_;
  std::vector<std::string> cleared_permission_origins_;
  bool cleared_all_authentication_{false};
  bool cleared_all_permissions_{false};
};

}  // namespace

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

    // TODO(chromium-runtime): initialize Chromium/CEF/content runtime,
    // subprocess plumbing, message loop, resources, locales, GPU, sandbox,
    // Safe Browsing integration, and site isolation.
    initialized_ = true;
  }

  void shutdown() noexcept {
    if (!initialized_) return;
    // TODO(chromium-runtime): close all native Chromium views/contexts before
    // runtime shutdown and drain the Chromium message loop.
    initialized_ = false;
  }

  [[nodiscard]] bool initialized() const noexcept { return initialized_; }

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
  return "native-surface-scaffold";
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
  if (!options.private_context && options.persistent_storage && options.storage_path.empty()) {
    throw std::invalid_argument("Persistent Chromium contexts require a storage path");
  }

  return std::make_unique<ChromiumEngineContext>(options);
}

}  // namespace goreecloud::browser
