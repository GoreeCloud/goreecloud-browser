#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace goreecloud::browser {

enum class ImageCandidateSource {
  current_src,
  src,
  srcset,
  picture,
  linked_original,
  page_metadata,
};

struct ImageCandidate {
  std::string url;
  ImageCandidateSource source{ImageCandidateSource::src};
  std::uint32_t intrinsic_width{};
  std::uint32_t intrinsic_height{};
  std::string mime_type;
  bool page_authorized{false};
};

class ImageCandidateSelector {
 public:
  virtual ~ImageCandidateSelector() = default;
  [[nodiscard]] virtual std::optional<ImageCandidate> select(
      const std::vector<ImageCandidate>& candidates) const = 0;
};

// Selection is restricted to content already made available in the current
// authorization context. Implementations must not bypass authentication,
// DRM, paywalls, hotlink controls, or other access-control boundaries.

}  // namespace goreecloud::browser
