#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include "goreecloud/browser/native_engine_surface.hpp"
#include "goreecloud/browser/native_window_host.hpp"

namespace goreecloud::browser::platform {

class LinuxGlazeWindowHost final : public NativeWindowHost {
 public:
  bool create() override {
    created_ = true;
    return true;
  }

  void show() override { visible_ = created_; }
  void close() override {
    visible_ = false;
    detach_engine_view();
  }

  void set_title(std::string_view title) override { title_ = std::string{title}; }
  void render_chrome(const BrowserChromeState& state) override { chrome_state_ = state; }

  void attach_engine_view(EngineView& view) override {
    attached_view_ = &view;
    if (auto* surface_view = dynamic_cast<NativeSurfaceAttachable*>(&view)) {
      NativeEngineSurface surface;
      surface.window_handle = native_window_handle_;
      surface.display_handle = native_display_handle_;
      surface.x = 0;
      surface.y = chrome_content_top_;
      surface.width = metrics_.width;
      surface.height = metrics_.height - chrome_content_top_;
      surface.scale_factor = metrics_.scale_factor;
      engine_surface_attached_ = surface_view->attach_native_surface(surface);
    }
  }

  void detach_engine_view() override {
    if (attached_view_) {
      if (auto* surface_view = dynamic_cast<NativeSurfaceAttachable*>(attached_view_)) {
        surface_view->detach_native_surface();
      }
    }
    attached_view_ = nullptr;
    engine_surface_attached_ = false;
  }

  void show_internal_surface(std::string_view internal_url) override {
    current_internal_surface_ = std::string{internal_url};
  }
  void show_panel(std::string_view panel_id) override {
    current_panel_ = std::string{panel_id};
  }

  void set_native_handles(std::uintptr_t window_handle,
                          std::uintptr_t display_handle) {
    native_window_handle_ = window_handle;
    native_display_handle_ = display_handle;
  }

  void set_metrics(NativeWindowMetrics metrics) {
    metrics_ = metrics;
    if (attached_view_) {
      if (auto* surface_view = dynamic_cast<NativeSurfaceAttachable*>(attached_view_)) {
        NativeEngineSurface surface{native_window_handle_, native_display_handle_, 0,
                                    chrome_content_top_, metrics_.width,
                                    metrics_.height - chrome_content_top_,
                                    metrics_.scale_factor};
        surface_view->resize_native_surface(surface);
      }
    }
  }

  [[nodiscard]] bool engine_surface_attached() const noexcept {
    return engine_surface_attached_;
  }

  [[nodiscard]] NativeWindowMetrics metrics() const override { return metrics_; }

 private:
  bool created_{false};
  bool visible_{false};
  bool engine_surface_attached_{false};
  std::uintptr_t native_window_handle_{0};
  std::uintptr_t native_display_handle_{0};
  int chrome_content_top_{116};
  std::string title_{"GoreeCloud Browser"};
  BrowserChromeState chrome_state_{};
  EngineView* attached_view_{nullptr};
  std::string current_internal_surface_;
  std::string current_panel_;
  NativeWindowMetrics metrics_{1280, 800, 1.0F};
};

}  // namespace goreecloud::browser::platform
