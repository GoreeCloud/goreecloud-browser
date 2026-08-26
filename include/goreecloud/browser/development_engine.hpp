#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "goreecloud/browser/engine.hpp"

namespace goreecloud::browser {

class DevelopmentEngineView final : public EngineView {
 public:
  explicit DevelopmentEngineView(std::string initial_url) {
    navigate(initial_url);
  }

  void navigate(std::string_view url) override {
    if (history_index_ + 1 < history_.size()) {
      history_.erase(history_.begin() + static_cast<std::ptrdiff_t>(history_index_ + 1),
                     history_.end());
    }
    history_.push_back(std::string{url});
    history_index_ = history_.size() - 1;
    state_.url = history_.back();
    state_.title = state_.url;
    state_.loading = false;
    state_.progress = 1.0;
    update_navigation_flags();
  }

  void reload() override {
    state_.loading = false;
    state_.progress = 1.0;
  }

  void stop() override { state_.loading = false; }

  void go_back() override {
    if (history_index_ > 0) {
      --history_index_;
      sync_from_history();
    }
  }

  void go_forward() override {
    if (history_index_ + 1 < history_.size()) {
      ++history_index_;
      sync_from_history();
    }
  }

  void set_zoom(double factor) override { zoom_ = std::clamp(factor, 0.25, 5.0); }
  void find(std::string_view query, bool forward) override {
    last_find_query_ = std::string{query};
    last_find_forward_ = forward;
  }
  void stop_find() override { last_find_query_.clear(); }

  [[nodiscard]] NavigationState navigation_state() const override { return state_; }
  [[nodiscard]] RendererHealth renderer_health() const override {
    return RendererHealth::Healthy;
  }

 private:
  void sync_from_history() {
    state_.url = history_[history_index_];
    state_.title = state_.url;
    state_.loading = false;
    state_.progress = 1.0;
    update_navigation_flags();
  }

  void update_navigation_flags() {
    state_.can_go_back = history_index_ > 0;
    state_.can_go_forward = history_index_ + 1 < history_.size();
  }

  NavigationState state_;
  std::vector<std::string> history_;
  std::size_t history_index_{0};
  double zoom_{1.0};
  std::string last_find_query_;
  bool last_find_forward_{true};
};

class DevelopmentEngineContext final : public EngineContext {
 public:
  [[nodiscard]] std::unique_ptr<EngineView> create_view(
      const EngineViewOptions& options) override {
    return std::make_unique<DevelopmentEngineView>(options.initial_url);
  }

  bool clear_origin_data(std::string_view, EngineDataClasses) override { return true; }
  bool clear_all_data(EngineDataClasses) override { return true; }
  bool clear_authentication_state(std::optional<std::string_view>) override { return true; }
  bool clear_permission_state(std::optional<std::string_view>) override { return true; }
};

// DevelopmentEngine exists only to make the GoreeCloud-owned application
// core runnable before Chromium integration. It renders no web content and
// must never be represented as a production browser engine.
class DevelopmentEngine final : public BrowserEngine {
 public:
  void initialize() override { initialized_ = true; }
  void shutdown() override { initialized_ = false; }

  [[nodiscard]] std::string_view name() const noexcept override {
    return "GoreeCloud Development Engine";
  }
  [[nodiscard]] std::string_view version() const noexcept override { return "0.1"; }
  [[nodiscard]] EngineCapabilities capabilities() const noexcept override {
    return capability(EngineCapability::WebView) |
           capability(EngineCapability::PrivateContexts) |
           capability(EngineCapability::PrivateContextCleanup) |
           capability(EngineCapability::OriginScopedCleanup) |
           capability(EngineCapability::AuthenticationStateCleanup) |
           capability(EngineCapability::PermissionStateCleanup);
  }

  [[nodiscard]] std::unique_ptr<EngineContext> create_context(
      const EngineContextOptions&) override {
    if (!initialized_) {
      return nullptr;
    }
    return std::make_unique<DevelopmentEngineContext>();
  }

 private:
  bool initialized_{false};
};

}  // namespace goreecloud::browser
