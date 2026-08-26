#pragma once

#include <functional>
#include <memory>
#include <stdexcept>

#include "goreecloud/browser/chromium_runtime_delegate.hpp"
#include "goreecloud/browser/platform/linux_event_loop.hpp"
#include "goreecloud/browser/platform/linux_glaze_host.hpp"
#include "goreecloud/browser/window_controller.hpp"

namespace goreecloud::browser::platform {

class LinuxDesktopRunner {
 public:
  LinuxDesktopRunner(LinuxGlazeWindowHost& host,
                     WindowController& window,
                     ChromiumRuntimeDelegate* chromium_runtime = nullptr)
      : host_(host), window_(window), chromium_runtime_(chromium_runtime) {}

  bool initialize() {
    if (!host_.create()) return false;
    host_.set_title("GoreeCloud Browser");

    if (auto* tab = window_.active_tab()) {
      host_.attach_engine_view(tab->engine_view());
    }

    event_loop_.set_chromium_pump([this] {
      if (chromium_runtime_) chromium_runtime_->do_message_loop_work();
    });

    host_.show();
    initialized_ = true;
    return true;
  }

  int run() {
    if (!initialized_) {
      throw std::logic_error("LinuxDesktopRunner must be initialized before run");
    }
    return event_loop_.run();
  }

  void request_quit() {
    event_loop_.request_quit();
    host_.detach_engine_view();
    host_.close();
  }

 private:
  LinuxGlazeWindowHost& host_;
  WindowController& window_;
  ChromiumRuntimeDelegate* chromium_runtime_{nullptr};
  LinuxEventLoop event_loop_;
  bool initialized_{false};
};

}  // namespace goreecloud::browser::platform
