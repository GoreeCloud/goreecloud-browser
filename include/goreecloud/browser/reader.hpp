#pragma once

#include <string>

namespace goreecloud::browser {

enum class ReaderTheme { light, dark, system };

struct ReaderEligibility {
  bool readable{false};
  std::string reason;
};

struct ReaderSession {
  std::string source_url;
  ReaderTheme theme{ReaderTheme::system};
  bool private_context{false};
  bool analytics_enabled{false};
  bool advertising_enabled{false};
};

class ReaderMode {
 public:
  virtual ~ReaderMode() = default;
  [[nodiscard]] virtual ReaderEligibility evaluate(
      const std::string& url) const = 0;
  [[nodiscard]] virtual bool enter(const ReaderSession& session) = 0;
  virtual void exit() = 0;
};

// Reader Mode may reuse mature readability extraction, but must not be used
// to circumvent paywalls, authentication, DRM, authorization, or technical
// access controls.

}  // namespace goreecloud::browser
