#pragma once

#include <cctype>
#include <optional>
#include <string>
#include <string_view>

#include "goreecloud/browser/internal_pages.hpp"

namespace goreecloud::browser {

enum class OmniboxIntent {
  empty,
  direct_navigation,
  goreecloud_search,
  internal_page,
};

struct OmniboxResolution {
  OmniboxIntent intent{OmniboxIntent::empty};
  std::string value;
};

class GoreeCloudSearchRouter {
 public:
  virtual ~GoreeCloudSearchRouter() = default;
  [[nodiscard]] virtual std::string search_url(std::string_view query) const = 0;
};

class OmniboxController {
 public:
  explicit OmniboxController(const GoreeCloudSearchRouter& search_router)
      : search_router_(search_router) {}

  [[nodiscard]] OmniboxResolution resolve(std::string_view input) const {
    const auto value = trim(input);
    if (value.empty()) {
      return {};
    }

    if (value.rfind("goreecloud://", 0) == 0) {
      return {OmniboxIntent::internal_page, value};
    }

    if (looks_like_url(value)) {
      return {OmniboxIntent::direct_navigation, normalize_url(value)};
    }

    return {OmniboxIntent::goreecloud_search, search_router_.search_url(value)};
  }

 private:
  [[nodiscard]] static std::string trim(std::string_view input) {
    std::size_t first = 0;
    while (first < input.size() && std::isspace(static_cast<unsigned char>(input[first]))) {
      ++first;
    }
    std::size_t last = input.size();
    while (last > first && std::isspace(static_cast<unsigned char>(input[last - 1]))) {
      --last;
    }
    return std::string{input.substr(first, last - first)};
  }

  [[nodiscard]] static bool looks_like_url(std::string_view value) {
    if (value.rfind("http://", 0) == 0 || value.rfind("https://", 0) == 0) {
      return true;
    }
    if (value.find(' ') != std::string_view::npos) {
      return false;
    }
    return value.find('.') != std::string_view::npos || value == "localhost" ||
           value.rfind("localhost:", 0) == 0;
  }

  [[nodiscard]] static std::string normalize_url(std::string_view value) {
    if (value.rfind("http://", 0) == 0 || value.rfind("https://", 0) == 0) {
      return std::string{value};
    }
    return std::string{"https://"} + std::string{value};
  }

  const GoreeCloudSearchRouter& search_router_;
};

inline constexpr bool kOmniboxAlternateSearchProviderAllowed = false;
inline constexpr bool kOmniboxSilentFallbackAllowed = false;

static_assert(!kOmniboxAlternateSearchProviderAllowed);
static_assert(!kOmniboxSilentFallbackAllowed);

}  // namespace goreecloud::browser
