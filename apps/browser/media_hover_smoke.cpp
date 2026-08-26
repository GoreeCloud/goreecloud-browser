#include <cassert>
#include <chrono>
#include <optional>
#include <string>
#include <utility>

#include "goreecloud/browser/browser_media_action_backend.hpp"
#include "goreecloud/browser/live_media_hover_coordinator.hpp"
#include "goreecloud/browser/media_visual_search_router.hpp"

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
    if (!pending) return;
    auto callback = std::move(pending);
    pending = {};
    callback(sequence, std::move(hit));
  }

  goreecloud::browser::MediaHitTestPoint last_point{};
  std::uint64_t last_sequence{0};
  ResultCallback pending;
};

class FakeDownloads final : public goreecloud::browser::AdvancedDownloadManagerService {
 public:
  goreecloud::browser::DownloadEnqueueResult enqueue(
      goreecloud::browser::DownloadEnqueueRequest request) override {
    last = std::move(request);
    return {.accepted = true, .download_id = "download-1", .message = "Queued locally."};
  }

  std::optional<goreecloud::browser::DownloadEnqueueRequest> last;
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
  const bool first_probe = coordinator.probe(
      provider, {.viewport_x = 100, .viewport_y = 80}, start);
  assert(first_probe);
  const auto first_sequence = provider.last_sequence;

  const bool throttled_probe = coordinator.probe(
      provider,
      {.viewport_x = 101, .viewport_y = 81},
      start + std::chrono::milliseconds{20});
  assert(!throttled_probe);

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

  presented = false;
  const auto later = start + std::chrono::milliseconds{100};
  const bool second_probe = coordinator.probe(
      provider, {.viewport_x = 200, .viewport_y = 160}, later);
  assert(second_probe);
  const auto second_sequence = provider.last_sequence;
  coordinator.invalidate();
  provider.respond(second_sequence, image);
  assert(!presented);
  assert(!coordinator.has_visible_target());

  const bool third_probe = coordinator.probe(
      provider,
      {.viewport_x = 210, .viewport_y = 170},
      later + std::chrono::milliseconds{100});
  assert(third_probe);
  const auto third_sequence = provider.last_sequence;
  provider.respond(third_sequence, image);
  assert(coordinator.has_visible_target());
  hidden = false;

  const bool fourth_probe = coordinator.probe(
      provider,
      {.viewport_x = 220, .viewport_y = 180},
      later + std::chrono::milliseconds{200});
  assert(fourth_probe);
  provider.respond(provider.last_sequence, std::nullopt);
  assert(hidden);
  assert(!coordinator.has_visible_target());

  MediaHoverSitePolicy disabled;
  disabled.enabled = false;
  coordinator.set_policy(disabled);
  const bool disabled_probe = coordinator.probe(
      provider,
      {.viewport_x = 1, .viewport_y = 1},
      later + std::chrono::milliseconds{300});
  assert(!disabled_probe);

  MediaTarget target;
  target.kind = MediaKind::image;
  target.page_url = "https://example.test/gallery";
  target.media_url = "https://example.test/image.jpg";
  target.can_download = true;

  ConfiguredGoreeCloudVisualSearchRouter visual_search(
      "https://search.goreecloud.test/visual");
  FakeDownloads downloads;
  std::string navigated;
  std::string opened;
  std::string copied;
  BrowserMediaActionBackend backend(
      visual_search,
      downloads,
      [&](std::string_view url) { navigated = std::string{url}; },
      [&](std::string_view url) { opened = std::string{url}; },
      [&](std::string_view text) {
        copied = std::string{text};
        return true;
      },
      [](const MediaTarget&) { return false; });
  MediaActionExecutor executor(backend);

  MediaHoverSitePolicy policy;
  policy.allow_remote_processing = true;

  MediaActionRequest search_request;
  search_request.action = MediaAction::search;
  search_request.target = target;
  search_request.explicit_user_action = true;
  search_request.processing_destination = MediaProcessingDestination::goreecloud_hosted;
  search_request.privacy_authorized = true;
  const auto search_result = executor.execute(search_request, policy);
  assert(search_result.disposition == MediaActionDisposition::completed);
  assert(navigated.find("https://search.goreecloud.test/visual?media_url=") == 0);
  assert(navigated.find("source_page=") != std::string::npos);

  MediaActionRequest save_request;
  save_request.action = MediaAction::save;
  save_request.target = target;
  save_request.save_destination = MediaSaveDestination::local_device;
  save_request.explicit_user_action = true;
  const auto save_result = executor.execute(save_request, policy);
  assert(save_result.disposition == MediaActionDisposition::completed);
  assert(downloads.last);
  assert(downloads.last->source_url == target.media_url);
  assert(downloads.last->referrer_url == target.page_url);

  MediaActionRequest copy_request;
  copy_request.action = MediaAction::copy_media_url;
  copy_request.target = target;
  copy_request.explicit_user_action = true;
  const auto copy_result = executor.execute(copy_request, policy);
  assert(copy_result.disposition == MediaActionDisposition::completed);
  assert(copied == target.media_url);

  MediaActionRequest open_request;
  open_request.action = MediaAction::open_media_new_tab;
  open_request.target = target;
  open_request.explicit_user_action = true;
  const auto open_result = executor.execute(open_request, policy);
  assert(open_result.disposition == MediaActionDisposition::completed);
  assert(opened == target.media_url);

  return 0;
}
