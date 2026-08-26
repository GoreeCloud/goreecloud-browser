#pragma once

#include <string>
#include <vector>

namespace goreecloud::browser {

enum class TabContextCommand {
  new_tab, duplicate, reload, pin, lock, protect, sleep, mute,
  add_to_group, move_to_workspace, split_view, move_tab, select_tabs,
  copy_url, copy_title, copy_title_and_url, bookmark, tab_information,
  close, close_others, close_left, close_right
};

enum class SplitContextCommand {
  split_left, split_right, split_above, split_below, add_to_existing,
  create_two_way, create_three_way, create_four_way, swap_tabs,
  move_position, replace_tab, expand_pane, restore_layout,
  remove_from_split, exit_split
};

struct TabContextState {
  std::size_t selected_count{1};
  bool pinned{false};
  bool locked{false};
  bool protected_tab{false};
  bool sleeping{false};
  bool grouped{false};
  bool in_split{false};
  bool audio_active{false};
};

struct TabInformationModel {
  std::string title;
  std::string url;
  std::string domain;
  std::string group_name;
  std::string workspace_name;
  bool pinned{false};
  bool protected_tab{false};
  bool locked{false};
  bool sleeping{false};
  bool audio_active{false};
  bool in_split{false};
  std::string approximate_memory;
  std::string cpu_activity;
  std::string network_activity;
  std::string process_status;
};

inline constexpr bool kAdvancedTabManagerUsesLatestGlazeUi = true;
inline constexpr bool kTabContextMenusAreContextSensitive = true;
inline constexpr bool kSplitLayoutsPersistAcrossSessions = true;
inline constexpr bool kProtectedTabsExcludedFromBulkClose = true;

}  // namespace goreecloud::browser
