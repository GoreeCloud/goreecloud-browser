#pragma once

#include <string>
#include <vector>

namespace goreecloud::browser {

enum class CaptureDestination {
  bookmarks,
  read_later,
  notes,
  memos,
  tasks,
};

enum class CaptureContentKind { page, link, selection, excerpt };

enum class CaptureResult { accepted, rejected, unavailable, failed };

struct CaptureIntent {
  CaptureDestination destination{CaptureDestination::bookmarks};
  CaptureContentKind content_kind{CaptureContentKind::page};
  std::string source_title;
  std::string source_url;
  std::string selected_text;
  bool private_context{false};
};

struct CaptureReceipt {
  CaptureResult result{CaptureResult::unavailable};
  std::string destination_object_id;
  std::string error;
};

// The Browser owns capture intent and presentation. The destination service
// owns stored data. A successful UI state requires an accepted destination
// write; intent dispatch alone is not success.
class CaptureService {
 public:
  virtual ~CaptureService() = default;
  [[nodiscard]] virtual CaptureReceipt capture(const CaptureIntent& intent) = 0;
};

}  // namespace goreecloud::browser
