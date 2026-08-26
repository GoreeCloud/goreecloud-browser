#pragma once

#include <cstdint>
#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

#include "goreecloud/browser/cef_media_probe.hpp"
#include "goreecloud/browser/engine.hpp"
#include "goreecloud/browser/media_target_detector.hpp"

#if GOREECLOUD_ENABLE_CEF
#include "include/cef_client.h"
#include "include/cef_context_menu_handler.h"
#include "include/cef_process_message.h"
#include "include/wrapper/cef_helpers.h"
#endif

namespace goreecloud::browser {

#if GOREECLOUD_ENABLE_CEF

class GoreeCloudCefClient final : public CefClient,
                                  public CefDisplayHandler,
                                  public CefLifeSpanHandler,
                                  public CefLoadHandler,
                                  public CefContextMenuHandler {
 public:
  using NavigationCallback = std::function<void(const NavigationState&)>;
  using ClosedCallback = std::function<void()>;
  using MediaContextCallback = std::function<void(const RawMediaHitTest&)>;
  using MediaProbeCallback =
      std::function<void(std::uint64_t, std::optional<RawMediaHitTest>)>;

  GoreeCloudCefClient(NavigationCallback navigation_callback,
                      ClosedCallback closed_callback,
                      MediaContextCallback media_context_callback = {})
      : navigation_callback_(std::move(navigation_callback)),
        closed_callback_(std::move(closed_callback)),
        media_context_callback_(std::move(media_context_callback)) {}

  CefRefPtr<CefDisplayHandler> GetDisplayHandler() override { return this; }
  CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
  CefRefPtr<CefLoadHandler> GetLoadHandler() override { return this; }
  CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override { return this; }

  void OnAfterCreated(CefRefPtr<CefBrowser> browser) override {
    CEF_REQUIRE_UI_THREAD();
    browser_ = browser;
    publish();
  }

  void OnBeforeClose(CefRefPtr<CefBrowser> browser) override {
    CEF_REQUIRE_UI_THREAD();
    if (browser_ && browser_->IsSame(browser)) browser_ = nullptr;
    fail_all_pending_probes();
    if (closed_callback_) closed_callback_();
  }

  void OnTitleChange(CefRefPtr<CefBrowser>, const CefString& title) override {
    CEF_REQUIRE_UI_THREAD();
    state_.title = title.ToString();
    publish();
  }

  void OnLoadingStateChange(CefRefPtr<CefBrowser>,
                            bool is_loading,
                            bool can_go_back,
                            bool can_go_forward) override {
    CEF_REQUIRE_UI_THREAD();
    state_.loading = is_loading;
    state_.can_go_back = can_go_back;
    state_.can_go_forward = can_go_forward;
    if (!is_loading) state_.progress = 1.0;
    publish();
  }

  void OnLoadStart(CefRefPtr<CefBrowser>,
                   CefRefPtr<CefFrame> frame,
                   TransitionType) override {
    CEF_REQUIRE_UI_THREAD();
    if (!frame || !frame->IsMain()) return;
    state_.url = frame->GetURL().ToString();
    state_.loading = true;
    state_.progress = 0.0;
    publish();
  }

  void OnLoadEnd(CefRefPtr<CefBrowser>,
                 CefRefPtr<CefFrame> frame,
                 int) override {
    CEF_REQUIRE_UI_THREAD();
    if (!frame || !frame->IsMain()) return;
    state_.url = frame->GetURL().ToString();
    state_.loading = false;
    state_.progress = 1.0;
    publish();
  }

  bool request_media_probe(int viewport_x,
                           int viewport_y,
                           std::uint64_t sequence,
                           MediaProbeCallback callback) {
    CEF_REQUIRE_UI_THREAD();
    if (!browser_ || !callback) return false;
    auto frame = browser_->GetMainFrame();
    if (!frame) return false;

    {
      std::scoped_lock lock(media_probe_mutex_);
      pending_media_probes_[sequence] = std::move(callback);
    }

    auto message = CefProcessMessage::Create(kMediaProbeRequestMessage);
    auto args = message->GetArgumentList();
    args->SetDouble(0, static_cast<double>(sequence));
    args->SetInt(1, viewport_x);
    args->SetInt(2, viewport_y);
    if (!frame->SendProcessMessage(PID_RENDERER, message)) {
      MediaProbeCallback failed;
      {
        std::scoped_lock lock(media_probe_mutex_);
        auto it = pending_media_probes_.find(sequence);
        if (it != pending_media_probes_.end()) {
          failed = std::move(it->second);
          pending_media_probes_.erase(it);
        }
      }
      if (failed) failed(sequence, std::nullopt);
      return false;
    }
    return true;
  }

  bool OnProcessMessageReceived(CefRefPtr<CefBrowser>,
                                CefRefPtr<CefFrame>,
                                CefProcessId source_process,
                                CefRefPtr<CefProcessMessage> message) override {
    CEF_REQUIRE_UI_THREAD();
    if (source_process != PID_RENDERER || !message ||
        message->GetName() != kMediaProbeResponseMessage) {
      return false;
    }

    auto args = message->GetArgumentList();
    if (!args || args->GetSize() < 2) return true;

    const auto sequence = static_cast<std::uint64_t>(args->GetDouble(0));
    MediaProbeCallback callback;
    {
      std::scoped_lock lock(media_probe_mutex_);
      const auto it = pending_media_probes_.find(sequence);
      if (it == pending_media_probes_.end()) return true;
      callback = std::move(it->second);
      pending_media_probes_.erase(it);
    }

    if (!callback) return true;
    if (!args->GetBool(1) || args->GetSize() < 16) {
      callback(sequence, std::nullopt);
      return true;
    }

    RawMediaHitTest hit;
    const auto kind = args->GetString(2).ToString();
    if (kind == "image") hit.kind = MediaKind::image;
    else if (kind == "video") hit.kind = MediaKind::video;
    else if (kind == "background_image") hit.kind = MediaKind::background_image;
    else hit.kind = MediaKind::unknown;

    hit.page_url = args->GetString(3).ToString();
    hit.media_url = args->GetString(4).ToString();
    hit.link_url = args->GetString(5).ToString();
    hit.mime_type = args->GetString(6).ToString();
    hit.alt_text = args->GetString(7).ToString();
    hit.displayed_width = args->GetInt(8);
    hit.displayed_height = args->GetInt(9);
    hit.intrinsic_width = args->GetInt(10);
    hit.intrinsic_height = args->GetInt(11);
    hit.duration_seconds = args->GetDouble(12);
    hit.secure_resource = args->GetBool(13);
    hit.cross_origin = args->GetBool(14);
    hit.animated = args->GetBool(15);
    hit.linked = !hit.link_url.empty();

    switch (hit.kind) {
      case MediaKind::image:
      case MediaKind::animated_image:
      case MediaKind::background_image:
        hit.copyable = true;
        hit.downloadable = !hit.media_url.empty();
        hit.region_selectable = true;
        hit.ocr_supported = true;
        break;
      case MediaKind::video:
        hit.copyable = !hit.media_url.empty();
        hit.downloadable = !hit.media_url.empty();
        hit.frame_capture_supported = true;
        hit.region_selectable = true;
        hit.ocr_supported = true;
        break;
      default:
        break;
    }

    callback(sequence, hit.kind == MediaKind::unknown || hit.media_url.empty()
                           ? std::nullopt
                           : std::optional<RawMediaHitTest>{std::move(hit)});
    return true;
  }

  void OnBeforeContextMenu(CefRefPtr<CefBrowser>,
                           CefRefPtr<CefFrame> frame,
                           CefRefPtr<CefContextMenuParams> params,
                           CefRefPtr<CefMenuModel>) override {
    CEF_REQUIRE_UI_THREAD();
    if (!params) return;

    RawMediaHitTest hit;
    hit.page_url = frame ? frame->GetURL().ToString() : state_.url;
    hit.media_url = params->GetSourceUrl().ToString();
    hit.link_url = params->GetLinkUrl().ToString();
    hit.mime_type = params->GetSourceMimeType().ToString();
    hit.alt_text = params->GetTitleText().ToString();

    const auto media_type = params->GetMediaType();
    switch (media_type) {
      case CM_MEDIATYPE_IMAGE:
        hit.kind = MediaKind::image;
        hit.copyable = true;
        hit.downloadable = !hit.media_url.empty();
        hit.region_selectable = true;
        hit.ocr_supported = true;
        break;
      case CM_MEDIATYPE_VIDEO:
        hit.kind = MediaKind::video;
        hit.copyable = !hit.media_url.empty();
        hit.downloadable = !hit.media_url.empty();
        hit.frame_capture_supported = true;
        hit.region_selectable = true;
        hit.ocr_supported = true;
        break;
      default:
        hit.kind = MediaKind::unknown;
        break;
    }

    const auto flags = params->GetTypeFlags();
    hit.linked = (flags & CM_TYPEFLAG_LINK) != 0 && !hit.link_url.empty();
    hit.secure_resource = hit.media_url.rfind("https://", 0) == 0;

    {
      std::scoped_lock lock(media_mutex_);
      last_media_context_ = hit;
    }
    if (media_context_callback_) media_context_callback_(hit);
  }

  [[nodiscard]] CefRefPtr<CefBrowser> browser() const { return browser_; }

  [[nodiscard]] std::optional<RawMediaHitTest> last_media_context() const {
    std::scoped_lock lock(media_mutex_);
    return last_media_context_;
  }

 private:
  void publish() {
    if (navigation_callback_) navigation_callback_(state_);
  }

  void fail_all_pending_probes() {
    std::unordered_map<std::uint64_t, MediaProbeCallback> pending;
    {
      std::scoped_lock lock(media_probe_mutex_);
      pending.swap(pending_media_probes_);
    }
    for (auto& [sequence, callback] : pending) {
      if (callback) callback(sequence, std::nullopt);
    }
  }

  CefRefPtr<CefBrowser> browser_;
  NavigationState state_;
  NavigationCallback navigation_callback_;
  ClosedCallback closed_callback_;
  MediaContextCallback media_context_callback_;
  mutable std::mutex media_mutex_;
  std::optional<RawMediaHitTest> last_media_context_;
  std::mutex media_probe_mutex_;
  std::unordered_map<std::uint64_t, MediaProbeCallback> pending_media_probes_;

  IMPLEMENT_REFCOUNTING(GoreeCloudCefClient);
  DISALLOW_COPY_AND_ASSIGN(GoreeCloudCefClient);
};

#endif

}  // namespace goreecloud::browser
