#include "goreecloud/browser/cef_runtime.hpp"

#include <stdexcept>
#include <utility>

namespace goreecloud::browser {

namespace {

class CefRuntimeDelegateScaffold final : public ChromiumRuntimeDelegate {
 public:
  explicit CefRuntimeDelegateScaffold(CefRuntimeOptions options)
      : options_(std::move(options)) {}

  void initialize() override {
    if (initialized_) return;
    if (options_.root.empty()) {
      throw std::runtime_error("CEF runtime root is not configured");
    }
    if (options_.subprocess_path.empty()) {
      throw std::runtime_error("CEF subprocess path is not configured");
    }
    if (!options_.enable_sandbox) {
      throw std::runtime_error("GoreeCloud Browser refuses to initialize CEF with sandboxing disabled");
    }

    // TODO(cef):
    // 1. Construct CefMainArgs at the executable entry point.
    // 2. Call CefExecuteProcess for renderer/GPU/other subprocesses.
    // 3. Configure CefSettings with cache/resource/locales paths and
    //    external_message_pump as selected by the GoreeCloud platform host.
    // 4. Call CefInitialize in the browser process.
    // 5. Create BrowserProcessHandler/LifeSpanHandler plumbing that feeds
    //    navigation and lifecycle callbacks into ChromiumRuntimeView objects.
    initialized_ = true;
  }

  void shutdown() override {
    if (!initialized_) return;
    // TODO(cef): close all browsers, drain callbacks, then call CefShutdown.
    initialized_ = false;
  }

  [[nodiscard]] std::unique_ptr<ChromiumRuntimeContext> create_context(
      const EngineContextOptions& options) override {
    if (!initialized_) {
      throw std::logic_error("CEF runtime must be initialized before creating contexts");
    }
    if (options.private_context && options.persistent_storage) {
      throw std::invalid_argument("Private CEF contexts must be ephemeral");
    }

    // TODO(cef): create a CefRequestContext with a persistent cache_path for
    // normal profiles and an empty/ephemeral cache_path for private sessions.
    // Return a concrete ChromiumRuntimeContext that owns that request context.
    return nullptr;
  }

  void do_message_loop_work() override {
    if (!initialized_) return;
    // TODO(cef): call CefDoMessageLoopWork() when using the external pump.
  }

  [[nodiscard]] std::string_view runtime_version() const noexcept override {
    return "cef-runtime-scaffold";
  }

 private:
  CefRuntimeOptions options_;
  bool initialized_{false};
};

}  // namespace

std::unique_ptr<ChromiumRuntimeDelegate> create_cef_runtime_delegate(
    CefRuntimeOptions options) {
  return std::make_unique<CefRuntimeDelegateScaffold>(std::move(options));
}

}  // namespace goreecloud::browser
