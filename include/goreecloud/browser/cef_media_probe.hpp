#pragma once

#include <cstdint>
#include <functional>
#include <optional>

#include "goreecloud/browser/media_target_detector.hpp"

namespace goreecloud::browser {

struct MediaProbePoint {
  int x{0};
  int y{0};
  std::uint64_t sequence{0};
};

class CefMediaProbeBridge {
 public:
  using ResultCallback = std::function<void(std::uint64_t, std::optional<RawMediaHitTest>)>;

  virtual ~CefMediaProbeBridge() = default;

  // Requests a renderer-side point probe. The implementation must inspect the
  // element at the given viewport coordinates and return normalized metadata
  // asynchronously. The probe itself must not download, OCR, analyze, or send
  // media off-device.
  virtual bool request_probe(const MediaProbePoint& point, ResultCallback callback) = 0;
};

inline constexpr bool kMediaProbeMayTriggerNetworkProcessing = false;
inline constexpr bool kMediaProbeMayTriggerAi = false;
inline constexpr bool kMediaProbeMayTriggerOcr = false;

static_assert(!kMediaProbeMayTriggerNetworkProcessing);
static_assert(!kMediaProbeMayTriggerAi);
static_assert(!kMediaProbeMayTriggerOcr);

}  // namespace goreecloud::browser
