#pragma once

#include <functional>
#include <mutex>
#include <optional>
#include <string>

#include "goreecloud/browser/engine.hpp"
#include "goreecloud/browser/media_target_detector.hpp"

#if GOREECLOUD_ENABLE_CEF
#include "include/cef_client.h"
#include "include/cef_context_menu_handler.h"
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

  CefRefPtr<CefBrowser> browser_;
  NavigationState state_;
  NavigationCallback navigation_callback_;
  ClosedCallback closed_callback_;
  MediaContextCallback media_context_callback_;
  mutable std::mutex media_mutex_;
  std::optional<RawMediaHitTest> last_media_context_;

  IMPLEMENT_REFCOUNTING(GoreeCloudCefClient);
  DISALLOW_COPY_AND_ASSIGN(GoreeCloudCefClient);
};

#endif

}  // namespace goreecloud::browser
