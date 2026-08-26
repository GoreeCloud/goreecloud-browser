#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace goreecloud::browser {

struct PrivateCleanupEvidence {
  bool engine_cleanup_confirmed{false};
  bool browser_state_cleanup_confirmed{false};
  bool permission_cleanup_confirmed{false};
  bool authentication_cleanup_confirmed{false};
  bool context_destroyed{false};
  std::vector<std::string> removed_categories;
  std::size_t preserved_download_count{0};
  std::size_t preserved_bookmark_count{0};
};

struct PrivateSessionEndSummary {
  bool cleanup_complete{false};
  std::vector<std::string> removed_categories;
  std::size_t deliberately_preserved_items{0};
};

class PrivateSessionEndSummaryBuilder {
 public:
  [[nodiscard]] static PrivateSessionEndSummary build(
      const PrivateCleanupEvidence& evidence) {
    PrivateSessionEndSummary summary;
    summary.cleanup_complete = evidence.engine_cleanup_confirmed &&
                               evidence.browser_state_cleanup_confirmed &&
                               evidence.permission_cleanup_confirmed &&
                               evidence.authentication_cleanup_confirmed &&
                               evidence.context_destroyed;
    summary.removed_categories = evidence.removed_categories;
    summary.deliberately_preserved_items = evidence.preserved_download_count +
                                           evidence.preserved_bookmark_count;
    return summary;
  }
};

// The summary is presentation-only. Implementations must not serialize it into
// browsing history, operational logs, Sync, analytics, or session recovery.
inline constexpr bool kPrivateEndSummaryIsPersistent = false;
static_assert(!kPrivateEndSummaryIsPersistent);

}  // namespace goreecloud::browser
