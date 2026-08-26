#pragma once

#include <functional>
#include <mutex>
#include <string>

#include "goreecloud/browser/engine.hpp"

#if GOREECLOUD_ENABLE_CEF
#include "include/cef_client.h"
#include "include/wrapper/cef_helpers.h"
#endif

namespace goreecloud::browser {

#if GOREECLOUD_ENABLE_CEF

class GoreeCloudCefClient final : public CefClient,
                                  public CefDisplayHandler,
                                  public CefLifeSpanHandler,
                                  public CefLoadHandler {
 public:
  using NavigationCallback = std::function<void(const NavigationState&)>;
  using ClosedCallback = std::function<void()>;

  GoreeCloudCefClient(NavigationCallback navigation_callback,
                      ClosedCallback closed_callback)
      : navigation_callback_(std::move(navigation_callback)),
        closed_callback_(std::move(closed_callback)) {}

  CefRefPtr<CefDisplayHandler> GetDisplayHandler() override { return this; }
  CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
  CefRefPtr<CefLoadHandler> GetLoadHandler() override { return this; }

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

  [[nodiscard]] CefRefPtr<CefBrowser> browser() const { return browser_; }

 private:
  void publish() {
    if (navigation_callback_) navigation_callback_(state_);
  }

  CefRefPtr<CefBrowser> browser_;
  NavigationState state_;
  NavigationCallback navigation_callback_;
  ClosedCallback closed_callback_;

  IMPLEMENT_REFCOUNTING(GoreeCloudCefClient);
  DISALLOW_COPY_AND_ASSIGN(GoreeCloudCefClient);
};

#endif

}  // namespace goreecloud::browser
