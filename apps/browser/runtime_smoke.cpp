#include <algorithm>
#include <cassert>
#include <memory>
#include <string>

#include "goreecloud/browser/application.hpp"
#include "goreecloud/browser/command_line.hpp"
#include "goreecloud/browser/configured_search_router.hpp"
#include "goreecloud/browser/development_engine.hpp"
#include "goreecloud/browser/in_memory_tab_manager.hpp"
#include "goreecloud/browser/internal_pages.hpp"
#include "goreecloud/browser/media_hover.hpp"
#include "goreecloud/browser/media_hover_controller.hpp"
#include "goreecloud/browser/media_hover_ui.hpp"
#include "goreecloud/browser/media_probe_result_tracker.hpp"
#include "goreecloud/browser/media_target_detector.hpp"
#include "goreecloud/browser/omnibox_controller.hpp"
#include "goreecloud/browser/toolbar.hpp"
#include "goreecloud/browser/unified_search_bar.hpp"
#include "goreecloud/browser/version.hpp"
#include "goreecloud/browser/window_controller.hpp"

int main() {
  using namespace goreecloud::browser;

  static_assert(kDefaultToolbar.size() == 12);
  static_assert(kUnifiedSearchBarControls.size() == 3);
  static_assert(!kAlternateSearchProvidersAllowed);
  static_assert(!kSilentSearchFallbackAllowed);
  static_assert(kBetaChannel);
  static_assert(!kProductionApproved);
  static_assert(!kMediaHoverPassiveUploadAllowed);
  static_assert(!kMediaHoverPassiveAiAnalysisAllowed);
  static_assert(!kMediaHoverPassiveOcrAllowed);
  static_assert(!kMediaDetectorMayOverrideProtectedMedia);
  static_assert(kDefaultMediaHoverQuickLabels.size() == 4);

  {
    char executable[] = "goreecloud-browser";
    char private_flag[] = "--private";
    char url[] = "https://example.com/";
    char* argv[]{executable, private_flag, url};
    const auto launch = parse_browser_launch_request(3, argv);
    assert(launch.private_window);
    assert(!launch.isolated_private_window);
    assert(launch.urls.size() == 1);
    assert(launch.urls.front() == "https://example.com/");
  }

  ConfiguredGoreeCloudSearchRouter search_router("https://search.goreecloud.test/search");
  OmniboxController omnibox(search_router);
  const auto url_resolution = omnibox.resolve("example.com");
  assert(url_resolution.intent == OmniboxIntent::direct_navigation);
  assert(url_resolution.value == "https://example.com");
  const auto search_resolution = omnibox.resolve("goreecloud browser beta");
  assert(search_resolution.intent == OmniboxIntent::goreecloud_search);
  assert(search_resolution.value.find("https://search.goreecloud.test/search?q=") == 0);

  {
    EngineMediaHitTest hit;
    hit.kind = EngineMediaElementKind::image;
    hit.page_url = "https://example.com/gallery";
    hit.page_title = "Gallery";
    hit.media_url = "https://cdn.example.com/image.jpg";
    hit.link_url = "https://example.com/destination";
    hit.mime_type = "image/jpeg";
    hit.alt_text = "Example image";
    hit.intrinsic_width = 1920;
    hit.intrinsic_height = 1080;
    hit.displayed_width = 640;
    hit.displayed_height = 360;
    hit.secure_resource = true;
    hit.downloadable = true;
    hit.copyable = true;

    const auto normalized = MediaTargetDetector::normalize(hit);
    assert(normalized);
    assert(normalized->kind == MediaKind::image);
    assert(normalized->media_url == hit.media_url);
    assert(normalized->link_url == hit.link_url);
    assert(normalized->downloadable);
    assert(normalized->copyable);

    hit.drm_protected = true;
    hit.capturable_frame = true;
    const auto protected_target = MediaTargetDetector::normalize(hit);
    assert(protected_target);
    assert(protected_target->protected_media);
    assert(!protected_target->downloadable);
    assert(!protected_target->capturable_frame);
  }

  {
    MediaProbeResultTracker tracker;
    const auto first_sequence = tracker.next_sequence();
    const auto second_sequence = tracker.next_sequence();
    assert(second_sequence > first_sequence);

    RawMediaHitTest stale;
    stale.kind = MediaKind::image;
    stale.media_url = "https://example.com/stale.jpg";
    assert(!tracker.accept(first_sequence, stale));
    assert(!tracker.latest_result());

    RawMediaHitTest current;
    current.kind = MediaKind::image;
    current.media_url = "https://example.com/current.jpg";
    assert(tracker.accept(second_sequence, current));
    assert(tracker.latest_result());
    assert(tracker.latest_result()->media_url == current.media_url);
    assert(tracker.latest_accepted() == second_sequence);
  }

  {
    MediaTarget image;
    image.page_url = "https://example.com/gallery";
    image.media_url = "https://cdn.example.com/image.jpg";
    image.link_url = "https://example.com/destination";
    image.kind = MediaKind::image;
    image.secure_resource = true;

    MediaHoverSitePolicy policy;
    policy.allow_remote_processing = false;

    const auto actions = MediaActionRegistry::actions_for(image, policy);
    assert(std::find(actions.begin(), actions.end(), MediaAction::preview) != actions.end());
    assert(std::find(actions.begin(), actions.end(), MediaAction::search) != actions.end());
    assert(std::find(actions.begin(), actions.end(), MediaAction::open_link) != actions.end());
    assert(std::find(actions.begin(), actions.end(), MediaAction::copy_media_url) != actions.end());

    const auto denied_remote = MediaProcessingPolicy::decide(
        MediaAction::search, MediaProcessingDestination::goreecloud_hosted, policy);
    assert(!denied_remote.allowed);

    policy.allow_remote_processing = true;
    const auto allowed_remote = MediaProcessingPolicy::decide(
        MediaAction::search, MediaProcessingDestination::goreecloud_hosted, policy);
    assert(allowed_remote.allowed);
    assert(allowed_remote.disclosure_required);

    const auto view_model = MediaHoverViewModelBuilder::build(
        image,
        policy,
        false,
        true,
        true,
        MediaSaveDestination::goreecloud_drive,
        "Privacy Shield: Remote processing allowed",
        "Wardveil: Secure resource");
    assert(view_model.visible);
    assert(view_model.reduced_motion);
    assert(view_model.keyboard_focus_visible);
    assert(view_model.quick_actions[0].label == "Preview");
    assert(view_model.quick_actions[1].label == "Search");
    assert(view_model.quick_actions[2].label == "Save");
    assert(view_model.quick_actions[3].label == "More");
    assert(view_model.destination.visible);
    assert(view_model.destination.label.find("Synchronized") != std::string::npos);
    assert(view_model.privacy.visible);
    assert(view_model.security.visible);

    image.protected_media = true;
    const auto protected_actions = MediaActionRegistry::actions_for(image, policy);
    assert(std::find(protected_actions.begin(), protected_actions.end(),
                     MediaAction::download_media) == protected_actions.end());

    assert(save_destination_label(MediaSaveDestination::local_device).find("Local") !=
           std::string_view::npos);
    assert(save_destination_label(MediaSaveDestination::goreecloud_drive).find("Synchronized") !=
           std::string_view::npos);

    MediaHoverController hover;
    assert(hover.activate(image, MediaHoverActivation::keyboard_focus, policy, false));
    assert(hover.visible());
    const auto placement = MediaHoverController::place(
        MediaRect{.x = 980, .y = 20, .width = 300, .height = 200},
        MediaViewport{.width = 1024, .height = 768},
        220,
        48);
    assert(placement.visible);
    assert(placement.x >= 8);
    assert(placement.x + 220 <= 1024 - 8);
    hover.pointer_left_media(false);
    assert(!hover.visible());

    policy.modifier_required = true;
    assert(!hover.activate(image, MediaHoverActivation::pointer_hover, policy, false));
    assert(hover.activate(image, MediaHoverActivation::pointer_hover, policy, true));
  }

  DevelopmentEngine engine;
  engine.initialize();

  EngineContextOptions normal_options;
  normal_options.profile_id = "smoke-normal";
  normal_options.storage_path = "smoke-profile";
  normal_options.private_context = false;
  normal_options.persistent_storage = true;

  auto normal_context = engine.create_context(normal_options);
  assert(normal_context);

  InMemoryAdvancedTabManager tab_manager;
  WindowController window(*normal_context, false, &tab_manager, "window-smoke");

  auto& first = window.new_tab(std::string{kNewTabUrl});
  tab_manager.register_tab({first.id(), window.window_id(), "workspace-main"});
  auto& second = window.new_tab("https://example.com/");
  tab_manager.register_tab({second.id(), window.window_id(), "workspace-main"});

  assert(window.tab_count() == 2);
  assert(window.select_tab(first.id(), false));
  assert(window.select_tab(second.id(), true));
  assert(window.pin_selected_tabs(true));
  assert(window.protect_selected_tabs(true));
  assert(!window.close_tab(second.id()));
  assert(window.close_tab(second.id(), true));
  assert(window.tab_count() == 1);

  window.open_home();
  assert(window.active_tab());
  assert(window.active_tab()->engine_view().navigation_state().url == kHomeUrl);
  window.open_settings();
  assert(window.active_tab()->engine_view().navigation_state().url == kSettingsUrl);

  engine.shutdown();

  BrowserApplicationOptions private_options;
  private_options.initial_private_window = true;
  private_options.initial_private_session_id = "smoke-private";
  private_options.initial_url = std::string{kPrivateStartUrl};
  BrowserApplication private_browser(std::make_unique<DevelopmentEngine>(), private_options);
  private_browser.initialize();
  assert(private_browser.window_count() == 1);
  assert(private_browser.first_window());
  assert(private_browser.first_window()->private_window());
  assert(private_browser.first_window()->active_tab());
  assert(private_browser.first_window()->active_tab()->engine_view().navigation_state().url ==
         kPrivateStartUrl);
  assert(private_browser.has_private_session_context("smoke-private"));
  private_browser.shutdown();

  return 0;
}
