#pragma once

#include <cstdint>
#include <string>

#include "goreecloud/browser/cef_media_probe.hpp"

#if GOREECLOUD_ENABLE_CEF
#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_frame.h"
#include "include/cef_process_message.h"
#include "include/cef_v8.h"
#endif

namespace goreecloud::browser {

#if GOREECLOUD_ENABLE_CEF

class GoreeCloudCefRenderApp final : public CefApp, public CefRenderProcessHandler {
 public:
  CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override { return this; }

  bool OnProcessMessageReceived(CefRefPtr<CefBrowser>,
                                CefRefPtr<CefFrame> frame,
                                CefProcessId source_process,
                                CefRefPtr<CefProcessMessage> message) override {
    if (source_process != PID_BROWSER || !message || !frame) return false;
    if (message->GetName() != kMediaProbeRequestMessage) return false;

    auto args = message->GetArgumentList();
    if (!args || args->GetSize() < 3) return true;

    const auto sequence = static_cast<std::uint64_t>(args->GetDouble(0));
    const int x = args->GetInt(1);
    const int y = args->GetInt(2);

    auto context = frame->GetV8Context();
    if (!context || !context->Enter()) {
      send_empty(frame, sequence);
      return true;
    }

    static constexpr const char* kProbeScript = R"JS((function(x,y){
      const el = document.elementFromPoint(x,y);
      if (!el) return null;
      const closest = (node, selector) => node && node.closest ? node.closest(selector) : null;
      const img = closest(el, 'img');
      const video = closest(el, 'video');
      const media = img || video || el;
      const link = closest(el, 'a[href]');
      const style = window.getComputedStyle(media);
      let bg = '';
      if (!img && !video && style && style.backgroundImage && style.backgroundImage !== 'none') {
        const match = style.backgroundImage.match(/^url\(["']?(.*?)["']?\)$/);
        if (match) bg = match[1];
      }
      let kind = '';
      let src = '';
      let mime = '';
      let alt = '';
      let intrinsicWidth = 0;
      let intrinsicHeight = 0;
      let duration = 0;
      let animated = false;
      if (img) {
        kind = 'image';
        src = img.currentSrc || img.src || '';
        alt = img.alt || img.title || '';
        intrinsicWidth = img.naturalWidth || 0;
        intrinsicHeight = img.naturalHeight || 0;
      } else if (video) {
        kind = 'video';
        src = video.currentSrc || video.src || '';
        intrinsicWidth = video.videoWidth || 0;
        intrinsicHeight = video.videoHeight || 0;
        duration = Number.isFinite(video.duration) ? video.duration : 0;
      } else if (bg) {
        kind = 'background_image';
        src = bg;
      } else {
        return null;
      }
      const rect = media.getBoundingClientRect();
      return {
        kind, src, link: link ? link.href : '', page: location.href,
        mime, alt, animated,
        displayedWidth: Math.max(0, Math.round(rect.width)),
        displayedHeight: Math.max(0, Math.round(rect.height)),
        intrinsicWidth, intrinsicHeight, duration,
        secure: location.protocol === 'https:' && (!src || src.startsWith('https:') || src.startsWith('data:') || src.startsWith('blob:')),
        crossOrigin: !!src && (function(){ try { return new URL(src, location.href).origin !== location.origin; } catch (_) { return false; } })()
      };
    }))JS";

    CefRefPtr<CefV8Value> retval;
    CefRefPtr<CefV8Exception> exception;
    const std::string script = std::string{"("} + kProbeScript + ")(" + std::to_string(x) + "," + std::to_string(y) + ")";
    const bool ok = context->Eval(script, frame->GetURL(), 0, retval, exception);
    context->Exit();

    if (!ok || !retval || retval->IsNull() || !retval->IsObject()) {
      send_empty(frame, sequence);
      return true;
    }

    auto response = CefProcessMessage::Create(kMediaProbeResponseMessage);
    auto out = response->GetArgumentList();
    out->SetDouble(0, static_cast<double>(sequence));
    out->SetBool(1, true);
    out->SetString(2, get_string(retval, "kind"));
    out->SetString(3, get_string(retval, "page"));
    out->SetString(4, get_string(retval, "src"));
    out->SetString(5, get_string(retval, "link"));
    out->SetString(6, get_string(retval, "mime"));
    out->SetString(7, get_string(retval, "alt"));
    out->SetInt(8, get_int(retval, "displayedWidth"));
    out->SetInt(9, get_int(retval, "displayedHeight"));
    out->SetInt(10, get_int(retval, "intrinsicWidth"));
    out->SetInt(11, get_int(retval, "intrinsicHeight"));
    out->SetDouble(12, get_double(retval, "duration"));
    out->SetBool(13, get_bool(retval, "secure"));
    out->SetBool(14, get_bool(retval, "crossOrigin"));
    out->SetBool(15, get_bool(retval, "animated"));
    frame->SendProcessMessage(PID_BROWSER, response);
    return true;
  }

 private:
  static std::string get_string(CefRefPtr<CefV8Value> object, const char* key) {
    auto value = object->GetValue(key);
    return value && value->IsString() ? value->GetStringValue().ToString() : std::string{};
  }
  static int get_int(CefRefPtr<CefV8Value> object, const char* key) {
    auto value = object->GetValue(key);
    return value && value->IsInt() ? value->GetIntValue() : 0;
  }
  static double get_double(CefRefPtr<CefV8Value> object, const char* key) {
    auto value = object->GetValue(key);
    return value && (value->IsDouble() || value->IsInt()) ? value->GetDoubleValue() : 0.0;
  }
  static bool get_bool(CefRefPtr<CefV8Value> object, const char* key) {
    auto value = object->GetValue(key);
    return value && value->IsBool() ? value->GetBoolValue() : false;
  }

  static void send_empty(CefRefPtr<CefFrame> frame, std::uint64_t sequence) {
    auto response = CefProcessMessage::Create(kMediaProbeResponseMessage);
    auto out = response->GetArgumentList();
    out->SetDouble(0, static_cast<double>(sequence));
    out->SetBool(1, false);
    frame->SendProcessMessage(PID_BROWSER, response);
  }

  IMPLEMENT_REFCOUNTING(GoreeCloudCefRenderApp);
  DISALLOW_COPY_AND_ASSIGN(GoreeCloudCefRenderApp);
};

#endif

}  // namespace goreecloud::browser
