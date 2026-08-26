#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <thread>

#include "goreecloud/browser/platform_event_loop.hpp"

namespace goreecloud::browser::platform {

class LinuxEventLoop final : public PlatformEventLoop {
 public:
  using Pump = std::function<void()>;

  explicit LinuxEventLoop(Pump pump = {}) : pump_(std::move(pump)) {}

  void run() override {
    quit_requested_ = false;
    while (!quit_requested_) {
      pump_once(std::chrono::milliseconds{8});
    }
  }

  void request_quit() override { quit_requested_ = true; }

  void pump_once(std::chrono::milliseconds max_wait) override {
    if (pump_) pump_();
    if (max_wait.count() > 0) std::this_thread::sleep_for(max_wait);
  }

  [[nodiscard]] bool quit_requested() const noexcept override {
    return quit_requested_.load();
  }

 private:
  Pump pump_;
  std::atomic_bool quit_requested_{false};
};

}  // namespace goreecloud::browser::platform
