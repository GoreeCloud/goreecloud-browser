#pragma once

#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "goreecloud/browser/media_hover.hpp"

namespace goreecloud::browser {

struct MediaHoverQuickAction {
  MediaAction action{MediaAction::more};
  std::string label;
  std::string accessible_label;
  bool enabled{true};
};

struct MediaHoverStatusChip {
  std::string label;
  bool visible{false};
};

struct MediaHoverViewModel {
  bool visible{false};
  bool expanded{false};
  bool reduced_motion{false};
  bool keyboard_focus_visible{false};
  std::array<MediaHoverQuickAction, 4> quick_actions{};
  std::vector<MediaAction> more_actions;
  MediaHoverStatusChip destination;
  MediaHoverStatusChip privacy;
  MediaHoverStatusChip security;
  std::string media_summary;
};

class MediaHoverViewModelBuilder {
 public:
  [[nodiscard]] static MediaHoverViewModel build(const MediaTarget& target,
                                                 const MediaHoverSitePolicy& policy,
                                                 bool expanded,
                                                 bool reduced_motion,
                                                 bool keyboard_focus_visible,
                                                 std::optional<MediaSaveDestination> destination = std::nullopt,
                                                 std::string privacy_status = {},
                                                 std::string security_status = {}) {
    MediaHoverViewModel model;
    model.visible = policy.mode != MediaHoverMode::disabled;
    model.expanded = expanded;
    model.reduced_motion = reduced_motion;
    model.keyboard_focus_visible = keyboard_focus_visible;

    const auto actions = MediaActionRegistry::actions_for(target, policy);
    model.quick_actions = {
        quick(MediaAction::preview, "Preview", actions),
        quick(MediaAction::search, "Search", actions),
        quick(MediaAction::save, "Save", actions),
        quick(MediaAction::more, "More", actions),
    };

    for (const auto action : actions) {
      if (action == MediaAction::preview || action == MediaAction::search ||
          action == MediaAction::save || action == MediaAction::more) {
        continue;
      }
      model.more_actions.push_back(action);
    }

    if (destination) {
      model.destination.visible = true;
      model.destination.label = std::string{save_destination_label(*destination)};
    }
    if (!privacy_status.empty()) {
      model.privacy.visible = true;
      model.privacy.label = std::move(privacy_status);
    }
    if (!security_status.empty()) {
      model.security.visible = true;
      model.security.label = std::move(security_status);
    }

    model.media_summary = target.kind == MediaKind::video ? "Video media" : "Image media";
    if (target.protected_media) model.media_summary += " · Protected";
    return model;
  }

 private:
  [[nodiscard]] static bool contains(const std::vector<MediaAction>& actions,
                                     MediaAction action) {
    for (const auto candidate : actions) {
      if (candidate == action) return true;
    }
    return false;
  }

  [[nodiscard]] static MediaHoverQuickAction quick(
      MediaAction action,
      std::string_view label,
      const std::vector<MediaAction>& actions) {
    MediaHoverQuickAction item;
    item.action = action;
    item.label = std::string{label};
    item.accessible_label = std::string{label} + " media";
    item.enabled = action == MediaAction::more || contains(actions, action);
    return item;
  }
};

inline constexpr std::array<std::string_view, 4> kDefaultMediaHoverQuickLabels{
    "Preview", "Search", "Save", "More"};

static_assert(kDefaultMediaHoverQuickLabels[0] == "Preview");
static_assert(kDefaultMediaHoverQuickLabels[1] == "Search");
static_assert(kDefaultMediaHoverQuickLabels[2] == "Save");
static_assert(kDefaultMediaHoverQuickLabels[3] == "More");

}  // namespace goreecloud::browser
