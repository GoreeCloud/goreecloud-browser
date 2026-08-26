#pragma once

#include <cstdint>
#include <functional>
#include <optional>

#include "goreecloud/browser/media_target_detector.hpp"

namespace goreecloud::browser {

inline constexpr char kMediaProbeRequestMessage[] = "goreecloud.media_probe.request";
inline constexpr char kMediaProbeResponseMessage[] = "goreecloud.media_probe.response";
inline constexpr char kMediaPreviewRequestMessage[] = "goreecloud.media_preview.request";
inline constexpr char kMediaPreviewResponseMessage[] = "goreecloud.media_preview.response";

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
inline constexpr bool kMediaPreviewMayBypassCanvasSecurity = false;

static_assert(!kMediaProbeMayTriggerNetworkProcessing);
static_assert(!kMediaProbeMayTriggerAi);
static_assert(!kMediaProbeMayTriggerOcr);
static_assert(!kMediaPreviewMayBypassCanvasSecurity);

}  // namespace goreecloud::browser
