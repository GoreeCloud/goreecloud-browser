#pragma once

#include <cstdint>
#include <functional>
#include <optional>

#include "goreecloud/browser/media_target_detector.hpp"

namespace goreecloud::browser {

inline constexpr char kMediaProbeRequestMessage[] = "goreecloud.media_probe.request";
inline constexpr char kMediaProbeResponseMessage[] = "goreecloud.media_probe.response";

struct MediaProbePoint {
  int x{0};
  int y{0};
  std::uint64_t sequence{0};
};

class CefMediaProbeBridge {
 public:
  using ResultCallback = std::function<void(std::uint64_t, std::optional<RawMediaHitTest>)>;

  virtual ~CefMediaProbeBridge() = default;

  virtual bool request_probe(const MediaProbePoint& point, ResultCallback callback) = 0;
};

inline constexpr bool kMediaProbeMayTriggerNetworkProcessing = false;
inline constexpr bool kMediaProbeMayTriggerAi = false;
inline constexpr bool kMediaProbeMayTriggerOcr = false;

static_assert(!kMediaProbeMayTriggerNetworkProcessing);
static_assert(!kMediaProbeMayTriggerAi);
static_assert(!kMediaProbeMayTriggerOcr);

}  // namespace goreecloud::browser
