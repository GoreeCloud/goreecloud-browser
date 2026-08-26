#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "goreecloud/browser/engine.hpp"
#include "goreecloud/browser/native_engine_surface.hpp"

namespace goreecloud::browser {

class ChromiumRuntimeView {
 public:
  virtual ~ChromiumRuntimeView() = default;
  virtual void navigate(std::string_view url) = 0;
  virtual void reload(bool bypass_cache) = 0;
  virtual void stop() = 0;
  virtual void go_back() = 0;
  virtual void go_forward() = 0;
  virtual void set_zoom(double factor) = 0;
  virtual void find(std::string_view query, bool forward) = 0;
  virtual void stop_find() = 0;
  virtual bool attach_surface(const NativeEngineSurface& surface) = 0;
  virtual void detach_surface() = 0;
  virtual void resize_surface(const NativeEngineSurface& surface) = 0;
  [[nodiscard]] virtual NavigationState navigation_state() const = 0;
  [[nodiscard]] virtual RendererHealth renderer_health() const = 0;
};

class ChromiumRuntimeContext {
 public:
  virtual ~ChromiumRuntimeContext() = default;
  [[nodiscard]] virtual std::unique_ptr<ChromiumRuntimeView> create_view(
      const EngineViewOptions& options) = 0;
  virtual bool clear_origin_data(std::string_view origin, EngineDataClasses classes) = 0;
  virtual bool clear_all_data(EngineDataClasses classes) = 0;
  virtual bool clear_authentication_state(std::optional<std::string_view> origin) = 0;
  virtual bool clear_permission_state(std::optional<std::string_view> origin) = 0;
};

class ChromiumRuntimeDelegate {
 public:
  virtual ~ChromiumRuntimeDelegate() = default;
  virtual void initialize() = 0;
  virtual void shutdown() = 0;
  [[nodiscard]] virtual std::unique_ptr<ChromiumRuntimeContext> create_context(
      const EngineContextOptions& options) = 0;
  virtual void do_message_loop_work() = 0;
  [[nodiscard]] virtual std::string_view runtime_version() const noexcept = 0;
};

}  // namespace goreecloud::browser
