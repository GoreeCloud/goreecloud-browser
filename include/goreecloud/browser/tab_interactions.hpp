#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace goreecloud::browser {

enum class TabDragTargetKind { reorder, window, group, workspace, split_pane };

struct TabDragTarget {
  TabDragTargetKind kind{TabDragTargetKind::reorder};
  std::string target_id;
  std::optional<std::size_t> index;
};

struct TabSelectionIntent {
  enum class Kind { replace, toggle, range, all, left, right, group, same_site };
  Kind kind{Kind::replace};
  std::string anchor_tab_id;
  std::string target_tab_id;
};

class TabInteractionController {
 public:
  virtual ~TabInteractionController() = default;
  virtual bool apply_selection(const TabSelectionIntent& intent) = 0;
  virtual bool drag_tabs(const std::vector<std::string>& tab_ids,
                         const TabDragTarget& target) = 0;
  virtual bool reorder_tab(const std::string& tab_id, std::size_t new_index) = 0;
};

inline constexpr bool kTabDragDropUsesGlazeUi = true;
inline constexpr bool kDesktopCtrlClickSelectionSupported = true;
inline constexpr bool kDesktopShiftRangeSelectionSupported = true;

}  // namespace goreecloud::browser
