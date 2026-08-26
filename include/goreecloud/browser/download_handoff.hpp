#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "goreecloud/browser/download.hpp"

namespace goreecloud::browser {

enum class DownloadHandoffSource {
  browser_intercept,
  context_menu,
  clipboard,
  manual_entry,
  system_share,
  media_detector,
};

struct AuthorizedSessionContext {
  std::string origin;
  std::vector<std::string> cookie_headers;
  std::optional<std::string> authorization_header;
  std::string user_agent;
};

struct DownloadHandoff {
  DownloadHandoffSource source{DownloadHandoffSource::browser_intercept};
  DownloadRequest request;
  std::optional<AuthorizedSessionContext> authorized_session;
};

class DownloadHandoffService {
 public:
  virtual ~DownloadHandoffService() = default;
  [[nodiscard]] virtual std::optional<DownloadRecord> submit(
      const DownloadHandoff& handoff) = 0;
};

// Session material may be handed off only when it belongs to an already
// authorized user session and is needed for the server's normal download flow.
// This is not an authentication, authorization, DRM, or paywall bypass.

}  // namespace goreecloud::browser
