#pragma once

#include <string>
#include <string_view>

#include "goreecloud/browser/chrome_shell.hpp"
#include "goreecloud/browser/engine.hpp"

namespace goreecloud::browser {

struct NativeWindowMetrics {
  int width{};
  int height{};
  float scale_factor{1.0F};
};

class NativeWindowHost {
 public:
  virtual ~NativeWindowHost() = default;

  virtual bool create() = 0;
  virtual void show() = 0;
  virtual void close() = 0;

  virtual void set_title(std::string_view title) = 0;
  virtual void render_chrome(const ChromeShellState& state) = 0;
  virtual void attach_engine_view(EngineView& view) = 0;
  virtual void detach_engine_view() = 0;
  virtual void show_internal_surface(std::string_view internal_url) = 0;
  virtual void show_panel(std::string_view panel_id) = 0;

  [[nodiscard]] virtual NativeWindowMetrics metrics() const = 0;
};

}  // namespace goreecloud::browser
