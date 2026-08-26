#pragma once

#include <string>
#include <vector>

#include "goreecloud/browser/advanced_tab_manager.hpp"
#include "goreecloud/browser/window_controller.hpp"

namespace goreecloud::browser {

struct TabStripItem {
  std::string tab_id;
  std::string title;
  std::string url;
  bool active{false};
  bool selected{false};
  bool pinned{false};
  bool locked{false};
  bool protected_tab{false};
  bool sleeping{false};
  bool audio_active{false};
  bool in_split{false};
  std::string group_id;
  std::string workspace_id;
};

class TabStripModel {
 public:
  [[nodiscard]] std::vector<TabStripItem> build(
      const WindowController& window,
      const AdvancedTabManager& manager) const {
    const auto selected = window.selected_tab_ids();
    const auto states = manager.search(TabSearchQuery{});
    std::vector<TabStripItem> items;
    for (const auto& id : window.tab_ids()) {
      const auto* active = window.active_tab();
      TabStripItem item;
      item.tab_id = id;
      item.active = active && active->id() == id;
      item.selected = std::find(selected.begin(), selected.end(), id) != selected.end();

      for (const auto& state : states) {
        if (state.tab_id != id) continue;
        item.pinned = state.pinned;
        item.locked = state.protection == TabProtection::locked;
        item.protected_tab = state.protection == TabProtection::protected_tab;
        item.sleeping = state.resources.sleeping;
        item.audio_active = state.resources.audio_active;
        item.in_split = state.split_id.has_value();
        item.group_id = state.group_id.value_or("");
        item.workspace_id = state.workspace_id;
        break;
      }

      if (item.active && active) {
        const auto nav = active->engine_view().navigation_state();
        item.title = nav.title;
        item.url = nav.url;
      }
      items.push_back(std::move(item));
    }
    return items;
  }
};

}  // namespace goreecloud::browser
