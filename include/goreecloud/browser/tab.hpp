#pragma once

#include <chrono>
#include <memory>
#include <string>

#include "goreecloud/browser/engine.hpp"

namespace goreecloud::browser {

enum class TabLifecycleState {
  Active,
  Background,
  Suspended,
  Discarded,
  Closed,
};

class Tab {
 public:
  Tab(std::string id, std::unique_ptr<EngineView> engine_view)
      : id_(std::move(id)), engine_view_(std::move(engine_view)) {}

  [[nodiscard]] const std::string& id() const noexcept { return id_; }
  [[nodiscard]] TabLifecycleState lifecycle_state() const noexcept {
    return lifecycle_state_;
  }

  void set_lifecycle_state(TabLifecycleState state) noexcept {
    lifecycle_state_ = state;
  }

  [[nodiscard]] EngineView& engine_view() noexcept { return *engine_view_; }
  [[nodiscard]] const EngineView& engine_view() const noexcept {
    return *engine_view_;
  }

 private:
  std::string id_;
  TabLifecycleState lifecycle_state_{TabLifecycleState::Active};
  std::unique_ptr<EngineView> engine_view_;
};

}  // namespace goreecloud::browser
