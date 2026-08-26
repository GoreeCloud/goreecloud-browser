#pragma once

#include <cstdint>
#include <optional>

#include "goreecloud/browser/media_target_detector.hpp"

namespace goreecloud::browser {

class MediaProbeResultTracker {
 public:
  [[nodiscard]] std::uint64_t next_sequence() noexcept { return ++latest_requested_; }

  [[nodiscard]] std::uint64_t latest_requested() const noexcept {
    return latest_requested_;
  }

  [[nodiscard]] bool accept(std::uint64_t sequence,
                            std::optional<EngineMediaHitTest> result) {
    if (sequence != latest_requested_) return false;
    latest_accepted_ = sequence;
    latest_result_ = std::move(result);
    return true;
  }

  [[nodiscard]] std::uint64_t latest_accepted() const noexcept {
    return latest_accepted_;
  }

  [[nodiscard]] const std::optional<EngineMediaHitTest>& latest_result() const noexcept {
    return latest_result_;
  }

  void clear() noexcept { latest_result_.reset(); }

 private:
  std::uint64_t latest_requested_{0};
  std::uint64_t latest_accepted_{0};
  std::optional<EngineMediaHitTest> latest_result_;
};

}  // namespace goreecloud::browser
