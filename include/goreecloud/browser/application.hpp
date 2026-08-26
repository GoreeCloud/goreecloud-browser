#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "goreecloud/browser/engine.hpp"
#include "goreecloud/browser/window_controller.hpp"

namespace goreecloud::browser {

struct BrowserApplicationOptions {
  std::string profile_id{"default"};
  std::string storage_path{"profile"};
  std::string locale{"en-US"};
  std::string initial_url{"about:blank"};
};

class BrowserApplication {
 public:
  BrowserApplication(std::unique_ptr<BrowserEngine> engine,
                     BrowserApplicationOptions options = {})
      : engine_(std::move(engine)), options_(std::move(options)) {
    if (!engine_) {
      throw std::invalid_argument("BrowserApplication requires a BrowserEngine");
    }
  }

  ~BrowserApplication() {
    if (initialized_) {
      shutdown();
    }
  }

  BrowserApplication(const BrowserApplication&) = delete;
  BrowserApplication& operator=(const BrowserApplication&) = delete;

  void initialize() {
    if (initialized_) {
      return;
    }

    engine_->initialize();

    EngineContextOptions context_options;
    context_options.profile_id = options_.profile_id;
    context_options.storage_path = options_.storage_path;
    context_options.locale = options_.locale;
    context_options.private_context = false;
    context_options.persistent_storage = true;

    default_context_ = engine_->create_context(context_options);
    if (!default_context_) {
      engine_->shutdown();
      throw std::runtime_error("Browser engine failed to create default context");
    }

    initialized_ = true;
    auto& window = new_window(false);
    (void)window.new_tab(options_.initial_url);
  }

  void shutdown() noexcept {
    windows_.clear();
    default_context_.reset();
    private_context_.reset();
    engine_->shutdown();
    initialized_ = false;
  }

  [[nodiscard]] WindowController& new_window(bool private_window) {
    require_initialized();

    EngineContext* context = default_context_.get();
    if (private_window) {
      if (!private_context_) {
        EngineContextOptions private_options;
        private_options.profile_id = options_.profile_id + "-private";
        private_options.locale = options_.locale;
        private_options.private_context = true;
        private_options.persistent_storage = false;
        private_context_ = engine_->create_context(private_options);
      }
      context = private_context_.get();
    }

    if (!context) {
      throw std::runtime_error("Browser context unavailable");
    }

    windows_.push_back(std::make_unique<WindowController>(*context, private_window));
    return *windows_.back();
  }

  [[nodiscard]] BrowserEngine& engine() noexcept { return *engine_; }
  [[nodiscard]] const BrowserEngine& engine() const noexcept { return *engine_; }
  [[nodiscard]] std::size_t window_count() const noexcept { return windows_.size(); }
  [[nodiscard]] bool initialized() const noexcept { return initialized_; }

 private:
  void require_initialized() const {
    if (!initialized_) {
      throw std::logic_error("BrowserApplication is not initialized");
    }
  }

  std::unique_ptr<BrowserEngine> engine_;
  BrowserApplicationOptions options_;
  std::unique_ptr<EngineContext> default_context_;
  std::unique_ptr<EngineContext> private_context_;
  std::vector<std::unique_ptr<WindowController>> windows_;
  bool initialized_{false};
};

}  // namespace goreecloud::browser
