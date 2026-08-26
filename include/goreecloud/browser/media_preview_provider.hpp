#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "goreecloud/browser/media_hover.hpp"

namespace goreecloud::browser {

struct MediaPreviewFrame {
  int width{0};
  int height{0};
  std::string mime_type;
  std::vector<std::uint8_t> encoded_bytes;
};

struct MediaPreviewRequest {
  MediaTarget target;
  int maximum_width{960};
  int maximum_height{720};
  bool allow_animation{true};
};

class AsyncMediaPreviewProvider {
 public:
  using PreviewCallback =
      std::function<void(std::optional<MediaPreviewFrame> frame, std::string error)>;

  virtual ~AsyncMediaPreviewProvider() = default;

  // Preview must reuse the engine's existing authorization/network context.
  // Hosts must not perform a second unauthenticated fetch of the media URL.
  virtual bool request_media_preview(const MediaPreviewRequest& request,
                                     PreviewCallback callback) = 0;
};

inline constexpr bool kMediaPreviewMayRefetchOutsideEngineContext = false;
inline constexpr bool kProtectedMediaPreviewMayBypassEngineRestrictions = false;
static_assert(!kMediaPreviewMayRefetchOutsideEngineContext);
static_assert(!kProtectedMediaPreviewMayBypassEngineRestrictions);

}  // namespace goreecloud::browser
