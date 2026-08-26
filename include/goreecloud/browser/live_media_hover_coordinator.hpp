#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <optional>
#include <utility>

#include "goreecloud/browser/media_hit_test_provider.hpp"
#include "goreecloud/browser/media_hover_ui.hpp"
#include "goreecloud/browser/media_target_detector.hpp"

namespace goreecloud::browser {

class LiveMediaHoverCoordinator {
 public:
  using Clock = std::chrono::steady_clock;
  using PresentCallback = std::function<void(const MediaTarget&,
                                             const MediaHoverViewModel&,
                                             MediaHitTestPoint)>;
  using HideCallback = std::function<void()>;

  explicit LiveMediaHoverCoordinator(
      std::chrono::milliseconds probe_interval = std::chrono::milliseconds{75})
      : probe_interval_(probe_interval) {}

  void set_policy(MediaHoverSitePolicy policy) { policy_ = std::move(policy); }
  void set_reduced_motion(bool reduced_motion) noexcept {
    reduced_motion_ = reduced_motion;
  }
  void set_present_callback(PresentCallback callback) {
    present_callback_ = std::move(callback);
  }
  void set_hide_callback(HideCallback callback) {
    hide_callback_ = std::move(callback);
  }

  [[nodiscard]] std::uint64_t latest_requested_sequence() const noexcept {
    return latest_requested_;
  }

  [[nodiscard]] std::uint64_t latest_accepted_sequence() const noexcept {
    return latest_accepted_;
  }

  [[nodiscard]] bool has_visible_target() const noexcept {
    return current_target_.has_value();
  }

  [[nodiscard]] const std::optional<MediaTarget>& current_target() const noexcept {
    return current_target_;
  }

  bool probe(AsyncMediaHitTestProvider& provider,
             MediaHitTestPoint point,
             Clock::time_point now = Clock::now()) {
    if (!policy_.enabled) {
      hide();
      return false;
    }
    if (last_probe_time_ && now - *last_probe_time_ < probe_interval_) {
      return false;
    }

    last_probe_time_ = now;
    const auto sequence = ++latest_requested_;
    latest_point_ = point;

    return provider.request_media_hit_test(
        point,
        sequence,
        [this](std::uint64_t response_sequence,
               std::optional<EngineMediaHitTest> hit) {
          accept(response_sequence, std::move(hit));
        });
  }

  void pointer_left_content() { hide(); }

  void invalidate() noexcept {
    ++latest_requested_;
    current_target_.reset();
  }

 private:
  void accept(std::uint64_t sequence, std::optional<EngineMediaHitTest> hit) {
    if (sequence != latest_requested_) return;
    latest_accepted_ = sequence;

    if (!hit) {
      hide();
      return;
    }

    auto target = MediaTargetDetector::normalize(*hit);
    if (!target) {
      hide();
      return;
    }

    current_target_ = std::move(target);
    const auto model = MediaHoverViewModelBuilder::build(
        *current_target_,
        policy_,
        false,
        reduced_motion_,
        false,
        std::nullopt,
        "Privacy Shield",
        current_target_->protected_media ? "Wardveil: Protected media" : "Wardveil Security");

    if (present_callback_) {
      present_callback_(*current_target_, model, latest_point_);
    }
  }

  void hide() {
    if (current_target_) {
      current_target_.reset();
      if (hide_callback_) hide_callback_();
    }
  }

  MediaHoverSitePolicy policy_{};
  std::chrono::milliseconds probe_interval_{75};
  std::optional<Clock::time_point> last_probe_time_;
  std::uint64_t latest_requested_{0};
  std::uint64_t latest_accepted_{0};
  MediaHitTestPoint latest_point_{};
  std::optional<MediaTarget> current_target_;
  PresentCallback present_callback_;
  HideCallback hide_callback_;
  bool reduced_motion_{false};
};

}  // namespace goreecloud::browser
