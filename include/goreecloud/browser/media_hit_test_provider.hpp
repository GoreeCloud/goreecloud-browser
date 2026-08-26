#pragma once

#include <optional>

#include "goreecloud/browser/media_target_detector.hpp"

namespace goreecloud::browser {

struct MediaHitTestPoint {
  int viewport_x{0};
  int viewport_y{0};
};

class MediaHitTestProvider {
 public:
  virtual ~MediaHitTestProvider() = default;

  [[nodiscard]] virtual std::optional<EngineMediaHitTest> media_hit_test(
      MediaHitTestPoint point) const = 0;
};

// EngineView implementations may optionally implement MediaHitTestProvider.
// Browser code discovers it through dynamic_cast and keeps CEF/Chromium types
// behind the engine adapter boundary.
inline constexpr bool kMediaHitTestingIsEngineOptionalCapability = true;
static_assert(kMediaHitTestingIsEngineOptionalCapability);

}  // namespace goreecloud::browser
