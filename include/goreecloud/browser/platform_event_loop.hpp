#pragma once

#include <chrono>

namespace goreecloud::browser {

class PlatformEventLoop {
 public:
  virtual ~PlatformEventLoop() = default;

  virtual void run() = 0;
  virtual void request_quit() = 0;
  virtual void pump_once(std::chrono::milliseconds max_wait) = 0;
  [[nodiscard]] virtual bool quit_requested() const noexcept = 0;
};

}  // namespace goreecloud::browser
