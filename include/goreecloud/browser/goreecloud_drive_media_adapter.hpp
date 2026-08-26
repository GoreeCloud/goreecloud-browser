#pragma once

#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "goreecloud/browser/media_destination_service.hpp"

namespace goreecloud::browser {

struct GoreeCloudDriveMediaWriteRequest {
  std::string source_url;
  std::string source_page_url;
  std::string source_page_title;
  std::optional<std::string> destination_folder_id;
  std::optional<std::string> preferred_filename;
  std::optional<std::string> user_annotation;
  bool preserve_source_attribution{true};
  bool private_session{false};
};

struct GoreeCloudDriveMediaWriteResult {
  bool accepted{false};
  bool persisted{false};
  std::string item_id;
  std::string message;
};

class GoreeCloudDriveMediaTransport {
 public:
  virtual ~GoreeCloudDriveMediaTransport() = default;
  virtual GoreeCloudDriveMediaWriteResult write_media(
      GoreeCloudDriveMediaWriteRequest request,
      std::string_view identity_authorization) = 0;
};

class GoreeCloudDriveMediaDestinationAdapter final : public MediaDestinationService {
 public:
  using AuthorizationProvider = std::function<std::optional<std::string>()>;
  using FolderProvider = std::function<std::optional<std::string>()>;
  using FilenameProvider = std::function<std::optional<std::string>(const MediaTarget&)>;

  GoreeCloudDriveMediaDestinationAdapter(GoreeCloudDriveMediaTransport& transport,
                                         AuthorizationProvider authorization,
                                         FolderProvider folder = {},
                                         FilenameProvider filename = {})
      : transport_(transport),
        authorization_(std::move(authorization)),
        folder_(std::move(folder)),
        filename_(std::move(filename)) {}

  MediaDestinationResult store(MediaDestinationRequest request) override {
    if (request.destination != MediaDestinationKind::drive) {
      return {.accepted = false,
              .persisted = false,
              .item_id = {},
              .message = "GoreeCloud Drive adapter cannot handle this destination."};
    }
    if (request.target.media_url.empty()) {
      return {.accepted = false,
              .persisted = false,
              .item_id = {},
              .message = "GoreeCloud Drive media source is empty."};
    }
    if (!authorization_) {
      return {.accepted = false,
              .persisted = false,
              .item_id = {},
              .message = "GoreeCloud Identity authorization is unavailable."};
    }

    auto authorization = authorization_();
    if (!authorization || authorization->empty()) {
      return {.accepted = false,
              .persisted = false,
              .item_id = {},
              .message = "GoreeCloud Identity did not authorize the Drive write."};
    }

    GoreeCloudDriveMediaWriteRequest drive_request;
    drive_request.source_url = request.target.media_url;
    drive_request.source_page_url = request.target.page_url;
    drive_request.source_page_title = request.target.page_title;
    drive_request.destination_folder_id = folder_ ? folder_() : std::nullopt;
    drive_request.preferred_filename = filename_ ? filename_(request.target) : std::nullopt;
    drive_request.user_annotation = request.user_annotation;
    drive_request.preserve_source_attribution = request.source_attribution_allowed;
    drive_request.private_session = request.private_session;

    auto result = transport_.write_media(std::move(drive_request), *authorization);
    if (result.accepted && !result.persisted) {
      result.accepted = false;
      if (result.message.empty()) {
        result.message = "GoreeCloud Drive did not confirm persistence.";
      }
    }
    return {.accepted = result.accepted,
            .persisted = result.persisted,
            .item_id = std::move(result.item_id),
            .message = std::move(result.message)};
  }

 private:
  GoreeCloudDriveMediaTransport& transport_;
  AuthorizationProvider authorization_;
  FolderProvider folder_;
  FilenameProvider filename_;
};

inline constexpr bool kDriveMediaWriteRequiresIdentityAuthorization = true;
inline constexpr bool kDriveMediaWriteRequiresPersistenceEvidence = true;
static_assert(kDriveMediaWriteRequiresIdentityAuthorization);
static_assert(kDriveMediaWriteRequiresPersistenceEvidence);

}  // namespace goreecloud::browser
