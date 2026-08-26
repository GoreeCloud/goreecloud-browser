#pragma once

#include <string>
#include <vector>

namespace goreecloud::browser {

enum class ContextActionKind {
  search_selected_text,
  save_page_bookmark,
  save_link_bookmark,
  copy_clean_link,
  download_highest_quality_image,
  open_reader_mode,
  copy_page_title_and_clean_url,
  capture_screenshot,
  copy_link_to_highlight,
  create_memo,
  create_task,
  save_to_notes,
  capture_contact,
  create_calendar_event,
};

struct ContextSnapshot {
  bool has_selection{false};
  bool has_link{false};
  bool has_image{false};
  bool reader_eligible{false};
  bool private_context{false};
  std::string page_url;
  std::string link_url;
  std::string selected_text;
};

struct ContextAction {
  ContextActionKind kind{ContextActionKind::search_selected_text};
  bool enabled{true};
};

class ContextMenuPolicy {
 public:
  virtual ~ContextMenuPolicy() = default;
  [[nodiscard]] virtual std::vector<ContextAction> actions_for(
      const ContextSnapshot& context) const = 0;
};

// Context actions are shown only where relevant. Service-write actions must
// fail closed until an authenticated destination adapter can verify success.

}  // namespace goreecloud::browser
