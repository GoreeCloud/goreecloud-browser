#pragma once

#include <string>
#include <string_view>

#include "goreecloud/browser/native_window_host.hpp"

namespace goreecloud::browser::platform {

// Development scaffold for the Linux/Flatpak native Glaze host.
// Platform widget toolkit integration is intentionally deferred until the
// approved Glaze UI host runtime is connected.
class LinuxGlazeWindowHost final : public NativeWindowHost {
 public:
  bool create() override {
    created_ = true;
    return true;
  }

  void show() override { visible_ = created_; }
  void close() override {
    visible_ = false;
    attached_view_ = nullptr;
  }

  void set_title(std::string_view title) override { title_ = std::string{title}; }
  void render_chrome(const ChromeShellState& state) override { chrome_state_ = state; }
  void attach_engine_view(EngineView& view) override { attached_view_ = &view; }
  void detach_engine_view() override { attached_view_ = nullptr; }
  void show_internal_surface(std::string_view internal_url) override {
    current_internal_surface_ = std::string{internal_url};
  }
  void show_panel(std::string_view panel_id) override {
    current_panel_ = std::string{panel_id};
  }

  [[nodiscard]] NativeWindowMetrics metrics() const override { return metrics_; }

 private:
  bool created_{false};
  bool visible_{false};
  std::string title_{"GoreeCloud Browser"};
  ChromeShellState chrome_state_{};
  EngineView* attached_view_{nullptr};
  std::string current_internal_surface_;
  std::string current_panel_;
  NativeWindowMetrics metrics_{1280, 800, 1.0F};
};

}  // namespace goreecloud::browser::platform
