#pragma once

#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "goreecloud/browser/engine.hpp"
#include "goreecloud/browser/tab.hpp"

namespace goreecloud::browser {

class WindowController {
 public:
  explicit WindowController(EngineContext& context, bool private_window = false)
      : context_(context), private_window_(private_window) {}

  [[nodiscard]] Tab& new_tab(std::string initial_url = "about:blank") {
    EngineViewOptions options;
    options.initial_url = std::move(initial_url);

    auto view = context_.create_view(options);
    const auto id = std::string{"tab-"} + std::to_string(next_tab_id_++);
    tabs_.push_back(std::make_unique<Tab>(id, std::move(view)));
    activate(tabs_.size() - 1);
    return *tabs_.back();
  }

  [[nodiscard]] bool close_tab(std::string_view id) {
    const auto it = std::find_if(tabs_.begin(), tabs_.end(), [id](const auto& tab) {
      return tab->id() == id;
    });
    if (it == tabs_.end()) {
      return false;
    }

    const auto removed_index = static_cast<std::size_t>(std::distance(tabs_.begin(), it));
    (*it)->set_lifecycle_state(TabLifecycleState::Closed);
    tabs_.erase(it);

    if (tabs_.empty()) {
      active_index_.reset();
      return true;
    }

    if (!active_index_.has_value() || removed_index <= *active_index_) {
      activate(std::min(removed_index, tabs_.size() - 1));
    }
    return true;
  }

  [[nodiscard]] bool activate_tab(std::string_view id) {
    for (std::size_t index = 0; index < tabs_.size(); ++index) {
      if (tabs_[index]->id() == id) {
        activate(index);
        return true;
      }
    }
    return false;
  }

  [[nodiscard]] Tab* active_tab() noexcept {
    if (!active_index_.has_value() || *active_index_ >= tabs_.size()) {
      return nullptr;
    }
    return tabs_[*active_index_].get();
  }

  [[nodiscard]] const Tab* active_tab() const noexcept {
    if (!active_index_.has_value() || *active_index_ >= tabs_.size()) {
      return nullptr;
    }
    return tabs_[*active_index_].get();
  }

  [[nodiscard]] std::size_t tab_count() const noexcept { return tabs_.size(); }
  [[nodiscard]] bool private_window() const noexcept { return private_window_; }

  void navigate_active(std::string_view url) {
    if (auto* tab = active_tab()) {
      tab->engine_view().navigate(url);
    }
  }

  void back() {
    if (auto* tab = active_tab()) {
      tab->engine_view().go_back();
    }
  }

  void forward() {
    if (auto* tab = active_tab()) {
      tab->engine_view().go_forward();
    }
  }

  void reload_or_stop() {
    if (auto* tab = active_tab()) {
      if (tab->engine_view().navigation_state().loading) {
        tab->engine_view().stop();
      } else {
        // Browser-owned toolbar semantics. The engine adapter is responsible
        // for mapping this to the approved hard-refresh/skip-cache path.
        tab->engine_view().reload();
      }
    }
  }

 private:
  void activate(std::size_t index) {
    for (std::size_t i = 0; i < tabs_.size(); ++i) {
      tabs_[i]->set_lifecycle_state(
          i == index ? TabLifecycleState::Active : TabLifecycleState::Background);
    }
    active_index_ = index;
  }

  EngineContext& context_;
  bool private_window_{false};
  std::vector<std::unique_ptr<Tab>> tabs_;
  std::optional<std::size_t> active_index_;
  std::uint64_t next_tab_id_{1};
};

}  // namespace goreecloud::browser
