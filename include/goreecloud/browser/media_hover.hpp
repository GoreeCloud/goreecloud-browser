#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace goreecloud::browser {

enum class MediaKind {
  unknown,
  image,
  animated_image,
  video,
  thumbnail,
  background_image,
  embedded_media,
};

enum class MediaAction {
  preview,
  search,
  search_similar,
  search_region,
  search_frame,
  save,
  save_region,
  save_frame,
  more,
  open_media,
  open_media_new_tab,
  copy_media,
  copy_media_url,
  copy_region,
  copy_frame,
  download_media,
  select_region,
  extract_text,
  ocr,
  translate_text,
  read_aloud,
  ask_goreecloud_ai,
  ask_ai,
  describe_media,
  media_details,
  security_details,
  open_link,
  open_link_new_tab,
  copy_link,
  save_link,
  save_to_notes,
  save_to_memos,
  create_task,
  save_to_drive,
  save_to_photos,
  save_to_video,
  inspect_media,
};

enum class MediaProcessingDestination {
  local,
  goreecloud_hosted,
  remote_other,
};

enum class MediaSaveDestination {
  local_device,
  goreecloud_drive,
  goreecloud_photos,
  goreecloud_video,
};

struct MediaTarget {
  std::string page_url;
  std::string page_title;
  std::string media_url;
  std::optional<std::string> link_url;
  std::string mime_type;
  std::string alt_text;
  MediaKind kind{MediaKind::image};
  int intrinsic_width{0};
  int intrinsic_height{0};
  int displayed_width{0};
  int displayed_height{0};
  double duration_seconds{0.0};
  bool animated{false};
  bool secure_resource{false};
  bool protected_media{false};
  bool same_origin_with_page{false};
  bool can_copy{true};
  bool can_download{true};
  bool can_capture_frame{false};
  bool can_select_region{true};
  bool can_ocr{true};
  bool can_visual_search{true};
};

struct MediaHoverSitePolicy {
  bool enabled{true};
  bool basic_controls_only{false};
  bool modifier_required{false};
  bool allow_visual_search{true};
  bool allow_ai{true};
  bool allow_ocr{true};
  bool allow_remote_processing{false};
};

struct MediaProcessingDecision {
  bool allowed{false};
  MediaProcessingDestination destination{MediaProcessingDestination::local};
  bool disclosure_required{false};
};

inline constexpr std::array<MediaAction, 4> kDefaultMediaHoverQuickActions{
    MediaAction::preview,
    MediaAction::search,
    MediaAction::save,
    MediaAction::more,
};

class MediaActionRegistry {
 public:
  [[nodiscard]] static std::vector<MediaAction> actions_for(
      const MediaTarget& target,
      const MediaHoverSitePolicy& policy) {
    if (!policy.enabled) return {};

    std::vector<MediaAction> actions{
        MediaAction::preview,
        MediaAction::save,
        MediaAction::open_media,
        MediaAction::open_media_new_tab,
        MediaAction::media_details,
    };

    if (target.can_visual_search && policy.allow_visual_search) {
      actions.push_back(MediaAction::search);
      actions.push_back(MediaAction::search_similar);
    }
    if (target.can_copy) {
      actions.push_back(MediaAction::copy_media);
      actions.push_back(MediaAction::copy_media_url);
    }
    if (target.can_download && !target.protected_media) actions.push_back(MediaAction::download_media);
    if (target.can_select_region) {
      actions.push_back(MediaAction::select_region);
      actions.push_back(MediaAction::search_region);
      actions.push_back(MediaAction::copy_region);
      actions.push_back(MediaAction::save_region);
    }
    if (target.can_capture_frame && !target.protected_media) {
      actions.push_back(MediaAction::search_frame);
      actions.push_back(MediaAction::copy_frame);
      actions.push_back(MediaAction::save_frame);
    }
    if (target.can_ocr && policy.allow_ocr) {
      actions.push_back(MediaAction::extract_text);
      actions.push_back(MediaAction::ocr);
      actions.push_back(MediaAction::translate_text);
      actions.push_back(MediaAction::read_aloud);
    }
    if (policy.allow_ai) {
      actions.push_back(MediaAction::ask_goreecloud_ai);
      actions.push_back(MediaAction::describe_media);
    }
    if (target.link_url) {
      actions.push_back(MediaAction::open_link);
      actions.push_back(MediaAction::open_link_new_tab);
      actions.push_back(MediaAction::copy_link);
      actions.push_back(MediaAction::save_link);
    }
    actions.push_back(MediaAction::save_to_notes);
    actions.push_back(MediaAction::save_to_memos);
    actions.push_back(MediaAction::create_task);
    actions.push_back(MediaAction::security_details);
    actions.push_back(MediaAction::inspect_media);

    if (policy.basic_controls_only) {
      std::vector<MediaAction> basic;
      for (const auto action : kDefaultMediaHoverQuickActions) {
        if (action == MediaAction::search && (!target.can_visual_search || !policy.allow_visual_search)) continue;
        basic.push_back(action);
      }
      return basic;
    }

    return actions;
  }
};

class MediaProcessingPolicy {
 public:
  [[nodiscard]] static MediaProcessingDecision decide(
      MediaAction action,
      MediaProcessingDestination requested_destination,
      const MediaHoverSitePolicy& policy) {
    const bool remote = requested_destination != MediaProcessingDestination::local;
    if ((action == MediaAction::search || action == MediaAction::search_similar ||
         action == MediaAction::search_region || action == MediaAction::search_frame) &&
        !policy.allow_visual_search) return {};
    if ((action == MediaAction::ask_goreecloud_ai || action == MediaAction::ask_ai ||
         action == MediaAction::describe_media) && !policy.allow_ai) return {};
    if ((action == MediaAction::extract_text || action == MediaAction::ocr ||
         action == MediaAction::translate_text || action == MediaAction::read_aloud) &&
        !policy.allow_ocr) return {};
    if (remote && !policy.allow_remote_processing) return {};
    return {.allowed = true, .destination = requested_destination, .disclosure_required = remote};
  }
};

[[nodiscard]] constexpr std::string_view save_destination_label(MediaSaveDestination destination) {
  switch (destination) {
    case MediaSaveDestination::local_device: return "Local Device — Local";
    case MediaSaveDestination::goreecloud_drive: return "GoreeCloud Drive — Synchronized";
    case MediaSaveDestination::goreecloud_photos: return "GoreeCloud Photos — Synchronized";
    case MediaSaveDestination::goreecloud_video: return "GoreeCloud Video — Synchronized";
  }
  return "Unknown destination";
}

inline constexpr bool kMediaHoverPassiveUploadAllowed = false;
inline constexpr bool kMediaHoverPassiveAiAnalysisAllowed = false;
inline constexpr bool kMediaHoverPassiveOcrAllowed = false;
inline constexpr bool kMediaAndLinkUrlsAreDistinct = true;

static_assert(!kMediaHoverPassiveUploadAllowed);
static_assert(!kMediaHoverPassiveAiAnalysisAllowed);
static_assert(!kMediaHoverPassiveOcrAllowed);
static_assert(kMediaAndLinkUrlsAreDistinct);

}  // namespace goreecloud::browser
