#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace goreecloud::browser {

enum class MediaKind { video, audio, image, document, archive, application, other };
enum class StreamKind { direct_file, hls, dash, unknown };

struct DetectedMedia {
  std::string id;
  std::string page_url;
  std::string resource_url;
  std::string title;
  std::string mime_type;
  MediaKind media_kind{MediaKind::other};
  StreamKind stream_kind{StreamKind::unknown};
  std::optional<std::uint64_t> content_length;
  bool authorized_in_current_context{false};
};

class MediaDetector {
 public:
  virtual ~MediaDetector() = default;
  [[nodiscard]] virtual std::vector<DetectedMedia> detected_for_page(
      std::string_view page_id) const = 0;
  virtual void clear_page(std::string_view page_id) = 0;
};

// Detection only surfaces media/resources already exposed to the current
// browsing context. It must not defeat DRM, authentication, paywalls,
// authorization boundaries, signed-URL policy, or other technical controls.

}  // namespace goreecloud::browser
