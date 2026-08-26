#pragma once

#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "goreecloud/browser/media_hover.hpp"

namespace goreecloud::browser {

class GoreeCloudVisualSearchRouter {
 public:
  virtual ~GoreeCloudVisualSearchRouter() = default;
  [[nodiscard]] virtual std::string search_url(const MediaTarget& target) const = 0;
};

class ConfiguredGoreeCloudVisualSearchRouter final : public GoreeCloudVisualSearchRouter {
 public:
  explicit ConfiguredGoreeCloudVisualSearchRouter(std::string endpoint)
      : endpoint_(std::move(endpoint)) {}

  [[nodiscard]] bool configured() const noexcept { return !endpoint_.empty(); }

  [[nodiscard]] std::string search_url(const MediaTarget& target) const override {
    if (endpoint_.empty()) {
      throw std::runtime_error(
          "GoreeCloud visual search is not configured; no alternate provider is permitted");
    }
    if (target.media_url.empty()) throw std::invalid_argument("Visual search requires a media URL");

    std::string result = endpoint_;
    result += endpoint_.find('?') == std::string::npos ? "?media_url=" : "&media_url=";
    result += percent_encode(target.media_url);
    if (!target.page_url.empty()) {
      result += "&source_page=";
      result += percent_encode(target.page_url);
    }
    return result;
  }

 private:
  [[nodiscard]] static std::string percent_encode(std::string_view value) {
    std::ostringstream out;
    out << std::uppercase << std::hex;
    for (const unsigned char ch : value) {
      if (std::isalnum(ch) || ch == '-' || ch == '_' || ch == '.' || ch == '~') {
        out << static_cast<char>(ch);
      } else {
        out << '%' << std::setw(2) << std::setfill('0') << static_cast<int>(ch);
      }
    }
    return out.str();
  }

  std::string endpoint_;
};

inline ConfiguredGoreeCloudVisualSearchRouter visual_search_router_from_environment() {
  const char* endpoint = std::getenv("GOREECLOUD_VISUAL_SEARCH_URL");
  return ConfiguredGoreeCloudVisualSearchRouter(endpoint ? std::string{endpoint} : std::string{});
}

inline constexpr bool kAlternateVisualSearchProvidersAllowed = false;
static_assert(!kAlternateVisualSearchProvidersAllowed);

}  // namespace goreecloud::browser
