#pragma once

#include <string>
#include <vector>

namespace goreecloud::browser {

enum class ContextActionKind {
  search_selected_text,
  open_link_new_tab,
  open_link_private,
  save_page_local_bookmark,
  save_page_goreecloud_bookmark,
  save_link_local_bookmark,
  save_link_goreecloud_bookmark,
  copy_clean_link,
  download_link,
  download_highest_quality_image,
  open_advanced_reader_mode,
  copy_page_title_and_clean_url,
  capture_screenshot,
  copy_link_to_highlight,
  create_memo,
  create_task,
  create_note_from_page,
  create_note_from_selection,
  append_to_existing_note,
  capture_contact,
  create_calendar_event,
  send_to_advanced_download_manager,
  inspect_privacy_shield,
  inspect_wardveil_security,
};

struct ContextSnapshot {
  bool has_selection{false};
  bool has_link{false};
  bool has_image{false};
  bool reader_eligible{false};
  bool downloadable_link{false};
  bool private_context{false};
  std::string page_url;
  std::string link_url;
  std::string selected_text;
};

struct ContextAction {
  ContextActionKind kind{ContextActionKind::search_selected_text};
  bool enabled{true};
  bool requires_service_write{false};
};

class ContextMenuPolicy {
 public:
  virtual ~ContextMenuPolicy() = default;
  [[nodiscard]] virtual std::vector<ContextAction> actions_for(
      const ContextSnapshot& context) const = 0;
};

inline constexpr bool kContextMenuUsesLatestApprovedGlaze = true;
inline constexpr bool kInheritedContextMenuProductIdentityAllowed = false;
inline constexpr bool kContextMenuServiceWritesFailClosed = true;

static_assert(kContextMenuUsesLatestApprovedGlaze);
static_assert(!kInheritedContextMenuProductIdentityAllowed);
static_assert(kContextMenuServiceWritesFailClosed);

// Context actions are shown only where relevant. GoreeCloud Memos, Notes,
// Tasks, Calendar, Contacts, Bookmarks, and Download Manager actions remain
// integration actions: the destination service retains authoritative storage.
// Service-write actions must fail closed until an authenticated adapter can
// verify that the destination accepted the write.

}  // namespace goreecloud::browser
