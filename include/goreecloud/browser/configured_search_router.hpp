#pragma once

#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "goreecloud/browser/omnibox_controller.hpp"

namespace goreecloud::browser {

class ConfiguredGoreeCloudSearchRouter final : public GoreeCloudSearchRouter {
 public:
  explicit ConfiguredGoreeCloudSearchRouter(std::string endpoint)
      : endpoint_(std::move(endpoint)) {}

  [[nodiscard]] std::string search_url(std::string_view query) const override {
    if (endpoint_.empty()) {
      throw std::runtime_error(
          "GoreeCloud Search is not configured; no alternate search provider is permitted");
    }
    std::string result = endpoint_;
    result += endpoint_.find('?') == std::string::npos ? "?q=" : "&q=";
    result += percent_encode(query);
    return result;
  }

  [[nodiscard]] bool configured() const noexcept { return !endpoint_.empty(); }

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

inline ConfiguredGoreeCloudSearchRouter search_router_from_environment() {
  const char* endpoint = std::getenv("GOREECLOUD_SEARCH_URL");
  return ConfiguredGoreeCloudSearchRouter(endpoint ? std::string{endpoint} : std::string{});
}

}  // namespace goreecloud::browser
