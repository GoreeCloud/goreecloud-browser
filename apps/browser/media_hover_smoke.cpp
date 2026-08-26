#include <cassert>
#include <chrono>
#include <optional>
#include <utility>

#include "goreecloud/browser/live_media_hover_coordinator.hpp"

namespace {

class FakeAsyncProvider final : public goreecloud::browser::AsyncMediaHitTestProvider {
 public:
  bool request_media_hit_test(goreecloud::browser::MediaHitTestPoint point,
                              std::uint64_t sequence,
                              ResultCallback callback) override {
    last_point = point;
    last_sequence = sequence;
    pending = std::move(callback);
    return true;
  }

  void respond(std::uint64_t sequence,
               std::optional<goreecloud::browser::EngineMediaHitTest> hit) {
    assert(pending);
    auto callback = std::move(pending);
    pending = {};
    callback(sequence, std::move(hit));
  }

  goreecloud::browser::MediaHitTestPoint last_point{};
  std::uint64_t last_sequence{0};
  ResultCallback pending;
};

}  // namespace

int main() {
  using namespace goreecloud::browser;
  using Clock = LiveMediaHoverCoordinator::Clock;

  FakeAsyncProvider provider;
  LiveMediaHoverCoordinator coordinator(std::chrono::milliseconds{75});

  bool presented = false;
  bool hidden = false;
  MediaTarget presented_target;
  MediaHitTestPoint presented_point{};

  coordinator.set_present_callback(
      [&](const MediaTarget& target,
          const MediaHoverViewModel& model,
          MediaHitTestPoint point) {
        presented = true;
        presented_target = target;
        presented_point = point;
        assert(model.visible);
        assert(model.quick_actions[0].label == "Preview");
        assert(model.quick_actions[1].label == "Search");
        assert(model.quick_actions[2].label == "Save");
        assert(model.quick_actions[3].label == "More");
      });
  coordinator.set_hide_callback([&] { hidden = true; });

  const auto start = Clock::now();
  assert(coordinator.probe(provider, {.viewport_x = 100, .viewport_y = 80}, start));
  const auto first_sequence = provider.last_sequence;

  // Throttle prevents a second renderer probe inside the configured interval.
  assert(!coordinator.probe(provider,
                            {.viewport_x = 101, .viewport_y = 81},
                            start + std::chrono::milliseconds{20}));

  EngineMediaHitTest image;
  image.kind = EngineMediaElementKind::image;
  image.page_url = "https://example.test/gallery";
  image.media_url = "https://example.test/image.jpg";
  image.mime_type = "image/jpeg";
  image.intrinsic_width = 1600;
  image.intrinsic_height = 900;
  image.displayed_width = 800;
  image.displayed_height = 450;
  image.secure_resource = true;
  image.downloadable = true;
  image.copyable = true;
  image.same_origin_with_page = true;

  provider.respond(first_sequence, image);
  assert(presented);
  assert(coordinator.has_visible_target());
  assert(presented_target.media_url == image.media_url);
  assert(presented_point.viewport_x == 100);
  assert(presented_point.viewport_y == 80);

  // A newer request makes an older response stale.
  presented = false;
  const auto later = start + std::chrono::milliseconds{100};
  assert(coordinator.probe(provider, {.viewport_x = 200, .viewport_y = 160}, later));
  const auto second_sequence = provider.last_sequence;
  coordinator.invalidate();
  provider.respond(second_sequence, image);
  assert(!presented);
  assert(!coordinator.has_visible_target());

  // No media hides the current overlay.
  assert(coordinator.probe(provider,
                           {.viewport_x = 210, .viewport_y = 170},
                           later + std::chrono::milliseconds{100}));
  const auto third_sequence = provider.last_sequence;
  provider.respond(third_sequence, image);
  assert(coordinator.has_visible_target());
  hidden = false;

  assert(coordinator.probe(provider,
                           {.viewport_x = 220, .viewport_y = 180},
                           later + std::chrono::milliseconds{200}));
  provider.respond(provider.last_sequence, std::nullopt);
  assert(hidden);
  assert(!coordinator.has_visible_target());

  // Disabled policy never requests renderer work.
  MediaHoverSitePolicy disabled;
  disabled.enabled = false;
  coordinator.set_policy(disabled);
  assert(!coordinator.probe(provider,
                            {.viewport_x = 1, .viewport_y = 1},
                            later + std::chrono::milliseconds{300}));

  return 0;
}
