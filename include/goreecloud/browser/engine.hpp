#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace goreecloud::browser {

enum class EngineCapability : std::uint64_t {
  WebView = 1ull << 0,
  PrivateContexts = 1ull << 1,
  SiteDataControl = 1ull << 2,
  CookieInterception = 1ull << 3,
  PermissionInterception = 1ull << 4,
  DownloadInterception = 1ull << 5,
  CertificateInspection = 1ull << 6,
  RendererHealth = 1ull << 7,
  DevTools = 1ull << 8,
  Extensions = 1ull << 9,
  HardwareAcceleration = 1ull << 10,
  WebRTC = 1ull << 11,
  Printing = 1ull << 12,
  Accessibility = 1ull << 13,
  PrivateContextCleanup = 1ull << 14,
  OriginScopedCleanup = 1ull << 15,
  AuthenticationStateCleanup = 1ull << 16,
  PermissionStateCleanup = 1ull << 17,
};

using EngineCapabilities = std::uint64_t;

constexpr EngineCapabilities capability(EngineCapability value) noexcept {
  return static_cast<EngineCapabilities>(value);
}

struct EngineContextOptions {
  std::string profile_id;
  std::string storage_path;
  std::string locale;
  bool private_context{false};
  bool persistent_storage{true};
};

struct EngineViewOptions {
  std::string initial_url{"about:blank"};
  bool allow_popups{false};
};

struct NavigationState {
  std::string url;
  std::string title;
  bool loading{false};
  bool can_go_back{false};
  bool can_go_forward{false};
  double progress{0.0};
};

enum class RendererHealth {
  Healthy,
  Unresponsive,
  Crashed,
  Terminated,
};

enum class EngineDataClass : std::uint64_t {
  Cookies = 1ull << 0,
  HttpCache = 1ull << 1,
  LocalStorage = 1ull << 2,
  IndexedDb = 1ull << 3,
  ServiceWorkers = 1ull << 4,
  AuthenticationState = 1ull << 5,
  Permissions = 1ull << 6,
  History = 1ull << 7,
};

using EngineDataClasses = std::uint64_t;
constexpr EngineDataClasses data_class(EngineDataClass value) noexcept {
  return static_cast<EngineDataClasses>(value);
}

inline constexpr EngineDataClasses kAllTemporaryEngineData =
    data_class(EngineDataClass::Cookies) |
    data_class(EngineDataClass::HttpCache) |
    data_class(EngineDataClass::LocalStorage) |
    data_class(EngineDataClass::IndexedDb) |
    data_class(EngineDataClass::ServiceWorkers) |
    data_class(EngineDataClass::AuthenticationState) |
    data_class(EngineDataClass::Permissions) |
    data_class(EngineDataClass::History);

class EngineView {
 public:
  virtual ~EngineView() = default;

  virtual void navigate(std::string_view url) = 0;
  virtual void reload() = 0;
  virtual void stop() = 0;
  virtual void go_back() = 0;
  virtual void go_forward() = 0;
  virtual void set_zoom(double factor) = 0;
  virtual void find(std::string_view query, bool forward) = 0;
  virtual void stop_find() = 0;

  [[nodiscard]] virtual NavigationState navigation_state() const = 0;
  [[nodiscard]] virtual RendererHealth renderer_health() const = 0;
};

class EngineContext {
 public:
  virtual ~EngineContext() = default;

  [[nodiscard]] virtual std::unique_ptr<EngineView> create_view(
      const EngineViewOptions& options) = 0;

  virtual bool clear_origin_data(std::string_view origin,
                                 EngineDataClasses classes) = 0;
  virtual bool clear_all_data(EngineDataClasses classes) = 0;
  virtual bool clear_authentication_state(std::optional<std::string_view> origin) = 0;
  virtual bool clear_permission_state(std::optional<std::string_view> origin) = 0;

  virtual void clear_site_data(std::string_view origin) {
    (void)clear_origin_data(origin, kAllTemporaryEngineData);
  }

  virtual void clear_all_site_data() {
    (void)clear_all_data(kAllTemporaryEngineData);
  }
};

class BrowserEngine {
 public:
  virtual ~BrowserEngine() = default;

  virtual void initialize() = 0;
  virtual void shutdown() = 0;

  // Platform hosts call this regularly when the selected engine integrates
  // with an external UI/event loop. Engines that do not need explicit pumping
  // use the default no-op implementation.
  virtual void pump_events() {}

  [[nodiscard]] virtual std::string_view name() const noexcept = 0;
  [[nodiscard]] virtual std::string_view version() const noexcept = 0;
  [[nodiscard]] virtual EngineCapabilities capabilities() const noexcept = 0;

  [[nodiscard]] virtual bool supports(EngineCapability feature) const noexcept {
    return (capabilities() & capability(feature)) != 0;
  }

  [[nodiscard]] virtual std::unique_ptr<EngineContext> create_context(
      const EngineContextOptions& options) = 0;
};

}  // namespace goreecloud::browser
