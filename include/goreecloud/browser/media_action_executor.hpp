#pragma once

#include <optional>
#include <string>

#include "goreecloud/browser/media_hover.hpp"

namespace goreecloud::browser {

enum class MediaActionDisposition {
  completed,
  pending,
  denied,
  unsupported,
  failed,
};

struct MediaActionRequest {
  MediaAction action{MediaAction::more};
  MediaTarget target;
  std::optional<MediaSaveDestination> save_destination;
  MediaProcessingDestination processing_destination{MediaProcessingDestination::local};
  bool explicit_user_action{false};
  bool privacy_authorized{false};
  bool persistence_warning_accepted{false};
};

struct MediaActionResult {
  MediaActionDisposition disposition{MediaActionDisposition::unsupported};
  std::string message;
  bool destination_confirmed{false};
};

class MediaActionBackend {
 public:
  virtual ~MediaActionBackend() = default;
  virtual MediaActionResult preview(const MediaActionRequest&) = 0;
  virtual MediaActionResult search(const MediaActionRequest&) = 0;
  virtual MediaActionResult save(const MediaActionRequest&) = 0;
  virtual MediaActionResult copy(const MediaActionRequest&) = 0;
  virtual MediaActionResult open(const MediaActionRequest&) = 0;
  virtual MediaActionResult process(const MediaActionRequest&) = 0;
  virtual MediaActionResult send_to_service(const MediaActionRequest&) = 0;
};

class MediaActionExecutor {
 public:
  explicit MediaActionExecutor(MediaActionBackend& backend) : backend_(backend) {}

  [[nodiscard]] MediaActionResult execute(const MediaActionRequest& request,
                                          const MediaHoverSitePolicy& policy) {
    if (!request.explicit_user_action) {
      return {MediaActionDisposition::denied,
              "Media actions require an explicit user action.", false};
    }

    if (is_remote_processing_action(request.action)) {
      const auto decision = MediaProcessingPolicy::decide(
          request.action, request.processing_destination, policy);
      if (!decision.allowed ||
          (decision.disclosure_required && !request.privacy_authorized)) {
        return {MediaActionDisposition::denied,
                "Privacy Shield has not authorized this media processing action.", false};
      }
    }

    if (request.target.protected_media && is_protected_media_extraction(request.action)) {
      return {MediaActionDisposition::denied,
              "Protected media cannot be extracted by this action.", false};
    }

    switch (request.action) {
      case MediaAction::preview:
        return backend_.preview(request);
      case MediaAction::search:
      case MediaAction::search_similar:
      case MediaAction::search_region:
      case MediaAction::search_frame:
        return backend_.search(request);
      case MediaAction::save:
      case MediaAction::download_media:
      case MediaAction::save_region:
      case MediaAction::save_frame:
        return backend_.save(request);
      case MediaAction::copy_media:
      case MediaAction::copy_media_url:
      case MediaAction::copy_region:
      case MediaAction::copy_frame:
        return backend_.copy(request);
      case MediaAction::open_media:
      case MediaAction::open_media_new_tab:
      case MediaAction::open_link:
      case MediaAction::open_link_new_tab:
        return backend_.open(request);
      case MediaAction::ocr:
      case MediaAction::translate_text:
      case MediaAction::read_aloud:
      case MediaAction::ask_ai:
      case MediaAction::describe_media:
        return backend_.process(request);
      case MediaAction::save_to_notes:
      case MediaAction::save_to_memos:
      case MediaAction::create_task:
      case MediaAction::save_to_drive:
      case MediaAction::save_to_photos:
      case MediaAction::save_to_video:
        if (!request.persistence_warning_accepted) {
          return {MediaActionDisposition::denied,
                  "Persistent destination requires explicit confirmation.", false};
        }
        return require_destination_confirmation(backend_.send_to_service(request));
      default:
        return {MediaActionDisposition::unsupported,
                "This Media Hover action has no executable backend yet.", false};
    }
  }

 private:
  [[nodiscard]] static bool is_remote_processing_action(MediaAction action) {
    switch (action) {
      case MediaAction::search:
      case MediaAction::search_similar:
      case MediaAction::search_region:
      case MediaAction::search_frame:
      case MediaAction::ocr:
      case MediaAction::translate_text:
      case MediaAction::ask_ai:
      case MediaAction::describe_media:
        return true;
      default:
        return false;
    }
  }

  [[nodiscard]] static bool is_protected_media_extraction(MediaAction action) {
    switch (action) {
      case MediaAction::download_media:
      case MediaAction::copy_frame:
      case MediaAction::save_frame:
      case MediaAction::search_frame:
      case MediaAction::save_region:
      case MediaAction::copy_region:
        return true;
      default:
        return false;
    }
  }

  [[nodiscard]] static MediaActionResult require_destination_confirmation(
      MediaActionResult result) {
    if (result.disposition == MediaActionDisposition::completed &&
        !result.destination_confirmed) {
      result.disposition = MediaActionDisposition::failed;
      result.message = "Destination did not confirm the media write.";
    }
    return result;
  }

  MediaActionBackend& backend_;
};

inline constexpr bool kMediaActionMaySucceedWithoutDestinationConfirmation = false;
static_assert(!kMediaActionMaySucceedWithoutDestinationConfirmation);

}  // namespace goreecloud::browser
