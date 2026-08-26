#pragma once

#include <algorithm>
#include <optional>

#include "goreecloud/browser/media_hover.hpp"

namespace goreecloud::browser {

enum class MediaHoverActivation {
  pointer_hover,
  keyboard_focus,
  touch_hold,
  accessibility_navigation,
};

struct MediaRect {
  int x{0};
  int y{0};
  int width{0};
  int height{0};
};

struct MediaViewport {
  int width{0};
  int height{0};
};

struct MediaOverlayPlacement {
  int x{0};
  int y{0};
  bool visible{false};
};

class MediaHoverController {
 public:
  bool activate(const MediaTarget& target,
                MediaHoverActivation activation,
                const MediaHoverSitePolicy& policy,
                bool modifier_held) {
    if (!policy.enabled) return false;
    if (policy.modifier_required && !modifier_held) return false;
    target_ = target;
    activation_ = activation;
    visible_ = true;
    return true;
  }

  void pointer_left_media(bool persistent_interaction_active) {
    if (!persistent_interaction_active) {
      visible_ = false;
      target_.reset();
    }
  }

  void dismiss() {
    visible_ = false;
    target_.reset();
  }

  [[nodiscard]] bool visible() const noexcept { return visible_; }
  [[nodiscard]] const std::optional<MediaTarget>& target() const noexcept {
    return target_;
  }

  [[nodiscard]] static MediaOverlayPlacement place(
      const MediaRect& media,
      const MediaViewport& viewport,
      int overlay_width,
      int overlay_height,
      int inset = 8) {
    if (media.width <= 0 || media.height <= 0 ||
        viewport.width <= 0 || viewport.height <= 0) {
      return {};
    }

    const int max_x = std::max(inset, viewport.width - overlay_width - inset);
    const int max_y = std::max(inset, viewport.height - overlay_height - inset);
    const int desired_x = media.x + media.width - overlay_width - inset;
    const int desired_y = media.y + inset;

    return {
        .x = std::clamp(desired_x, inset, max_x),
        .y = std::clamp(desired_y, inset, max_y),
        .visible = true,
    };
  }

 private:
  std::optional<MediaTarget> target_;
  std::optional<MediaHoverActivation> activation_;
  bool visible_{false};
};

}  // namespace goreecloud::browser
