#pragma once

#include <optional>
#include <string>

#include "goreecloud/browser/media_hover.hpp"

namespace goreecloud::browser {

enum class MediaDestinationKind {
  drive,
  photos,
  video,
  notes,
  memos,
  tasks,
};

struct MediaDestinationRequest {
  MediaDestinationKind destination{MediaDestinationKind::drive};
  MediaTarget target;
  std::optional<std::string> user_annotation;
  bool source_attribution_allowed{true};
  bool private_session{false};
};

struct MediaDestinationResult {
  bool accepted{false};
  bool persisted{false};
  std::string item_id;
  std::string message;
};

class MediaDestinationService {
 public:
  virtual ~MediaDestinationService() = default;
  virtual MediaDestinationResult store(MediaDestinationRequest request) = 0;
};

class UnavailableMediaDestinationService final : public MediaDestinationService {
 public:
  MediaDestinationResult store(MediaDestinationRequest request) override {
    std::string name;
    switch (request.destination) {
      case MediaDestinationKind::drive: name = "GoreeCloud Drive"; break;
      case MediaDestinationKind::photos: name = "GoreeCloud Photos"; break;
      case MediaDestinationKind::video: name = "GoreeCloud Video"; break;
      case MediaDestinationKind::notes: name = "GoreeCloud Notes"; break;
      case MediaDestinationKind::memos: name = "GoreeCloud Memos"; break;
      case MediaDestinationKind::tasks: name = "GoreeCloud Tasks"; break;
    }
    return {.accepted = false,
            .persisted = false,
            .item_id = {},
            .message = name + " media adapter is not connected yet."};
  }
};

inline constexpr bool kMediaDestinationMayClaimPersistenceWithoutConfirmation = false;
static_assert(!kMediaDestinationMayClaimPersistenceWithoutConfirmation);

}  // namespace goreecloud::browser
