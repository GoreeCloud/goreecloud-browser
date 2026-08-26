#pragma once

#include <functional>
#include <string>
#include <utility>

#include "goreecloud/browser/advanced_download_manager_service.hpp"
#include "goreecloud/browser/media_action_executor.hpp"
#include "goreecloud/browser/media_destination_service.hpp"
#include "goreecloud/browser/media_visual_search_router.hpp"

namespace goreecloud::browser {

class BrowserMediaActionBackend final : public MediaActionBackend {
 public:
  using NavigateFn = std::function<void(std::string_view)>;
  using OpenNewTabFn = std::function<void(std::string_view)>;
  using CopyTextFn = std::function<bool(std::string_view)>;
  using PreviewFn = std::function<bool(const MediaTarget&)>;

  BrowserMediaActionBackend(GoreeCloudVisualSearchRouter& visual_search,
                            AdvancedDownloadManagerService& downloads,
                            NavigateFn navigate,
                            OpenNewTabFn open_new_tab,
                            CopyTextFn copy_text,
                            PreviewFn preview,
                            MediaDestinationService* destinations = nullptr)
      : visual_search_(visual_search),
        downloads_(downloads),
        navigate_(std::move(navigate)),
        open_new_tab_(std::move(open_new_tab)),
        copy_text_(std::move(copy_text)),
        preview_(std::move(preview)),
        destinations_(destinations) {}

  MediaActionResult preview(const MediaActionRequest& request) override {
    if (!preview_) return unsupported("Media preview host is unavailable.");
    return preview_(request.target)
               ? completed("Media preview opened.")
               : failed("Media preview could not be opened.");
  }

  MediaActionResult search(const MediaActionRequest& request) override {
    if (!navigate_) return unsupported("Browser navigation is unavailable.");
    try {
      navigate_(visual_search_.search_url(request.target));
      return completed("Opened GoreeCloud visual search.");
    } catch (const std::exception& error) {
      return failed(error.what());
    }
  }

  MediaActionResult save(const MediaActionRequest& request) override {
    if (!request.save_destination) {
      return failed("A media save destination was not selected.");
    }
    if (*request.save_destination != MediaSaveDestination::local_device) {
      if (!destinations_) return unsupported("This synchronized media destination is not connected yet.");
      const auto kind = destination_kind(*request.save_destination);
      if (!kind) return unsupported("This media destination is not supported.");
      return destination_result(destinations_->store({
          .destination = *kind,
          .target = request.target,
          .user_annotation = std::nullopt,
          .source_attribution_allowed = true,
          .private_session = false,
      }));
    }
    if (!request.target.can_download || request.target.protected_media) {
      return {MediaActionDisposition::denied,
              "This media cannot be downloaded through the current protection state.", false};
    }
    auto result = downloads_.enqueue({
        .source_url = request.target.media_url,
        .referrer_url = request.target.page_url,
        .suggested_filename = std::nullopt,
        .private_session = false,
    });
    if (!result.accepted) return failed(result.message);
    return {MediaActionDisposition::completed,
            result.message.empty() ? "Added media to Advanced Download Manager." : result.message,
            true};
  }

  MediaActionResult copy(const MediaActionRequest& request) override {
    if (!copy_text_) return unsupported("Clipboard integration is unavailable.");
    std::string_view value;
    switch (request.action) {
      case MediaAction::copy_link:
        if (!request.target.link_url) return failed("This media has no link destination.");
        value = *request.target.link_url;
        break;
      default:
        value = request.target.media_url;
        break;
    }
    return copy_text_(value) ? completed("Copied media information.")
                             : failed("Clipboard operation failed.");
  }

  MediaActionResult open(const MediaActionRequest& request) override {
    const bool link_action = request.action == MediaAction::open_link ||
                             request.action == MediaAction::open_link_new_tab;
    const auto& target_url = link_action && request.target.link_url
                                 ? *request.target.link_url
                                 : request.target.media_url;
    const bool new_tab = request.action == MediaAction::open_media_new_tab ||
                         request.action == MediaAction::open_link_new_tab;
    if (new_tab) {
      if (!open_new_tab_) return unsupported("New-tab integration is unavailable.");
      open_new_tab_(target_url);
    } else {
      if (!navigate_) return unsupported("Browser navigation is unavailable.");
      navigate_(target_url);
    }
    return completed("Opened media destination.");
  }

  MediaActionResult process(const MediaActionRequest&) override {
    return unsupported("This processing adapter is not available yet.");
  }

  MediaActionResult send_to_service(const MediaActionRequest& request) override {
    if (!destinations_) return unsupported("This GoreeCloud destination adapter is not available yet.");
    const auto destination = destination_kind(request.action);
    if (!destination) return unsupported("This GoreeCloud destination is not supported.");
    return destination_result(destinations_->store({
        .destination = *destination,
        .target = request.target,
        .user_annotation = std::nullopt,
        .source_attribution_allowed = true,
        .private_session = false,
    }));
  }

 private:
  static std::optional<MediaDestinationKind> destination_kind(MediaSaveDestination destination) {
    switch (destination) {
      case MediaSaveDestination::goreecloud_drive: return MediaDestinationKind::drive;
      case MediaSaveDestination::goreecloud_photos: return MediaDestinationKind::photos;
      case MediaSaveDestination::goreecloud_video: return MediaDestinationKind::video;
      case MediaSaveDestination::local_device: return std::nullopt;
    }
    return std::nullopt;
  }

  static std::optional<MediaDestinationKind> destination_kind(MediaAction action) {
    switch (action) {
      case MediaAction::save_to_drive: return MediaDestinationKind::drive;
      case MediaAction::save_to_photos: return MediaDestinationKind::photos;
      case MediaAction::save_to_video: return MediaDestinationKind::video;
      case MediaAction::save_to_notes: return MediaDestinationKind::notes;
      case MediaAction::save_to_memos: return MediaDestinationKind::memos;
      case MediaAction::create_task: return MediaDestinationKind::tasks;
      default: return std::nullopt;
    }
  }

  static MediaActionResult destination_result(MediaDestinationResult result) {
    if (!result.accepted) return failed(result.message);
    if (!result.persisted) {
      return failed(result.message.empty()
                        ? "Destination accepted the request but did not confirm persistence."
                        : result.message);
    }
    return {MediaActionDisposition::completed,
            result.message.empty() ? "Media saved to GoreeCloud destination." : result.message,
            true};
  }

  static MediaActionResult completed(std::string message) {
    return {MediaActionDisposition::completed, std::move(message), true};
  }
  static MediaActionResult failed(std::string message) {
    return {MediaActionDisposition::failed, std::move(message), false};
  }
  static MediaActionResult unsupported(std::string message) {
    return {MediaActionDisposition::unsupported, std::move(message), false};
  }

  GoreeCloudVisualSearchRouter& visual_search_;
  AdvancedDownloadManagerService& downloads_;
  NavigateFn navigate_;
  OpenNewTabFn open_new_tab_;
  CopyTextFn copy_text_;
  PreviewFn preview_;
  MediaDestinationService* destinations_{nullptr};
};

}  // namespace goreecloud::browser
