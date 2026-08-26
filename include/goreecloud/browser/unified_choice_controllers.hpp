#pragma once

#include <optional>

#include "goreecloud/browser/bookmarks.hpp"
#include "goreecloud/browser/unified_vault.hpp"

namespace goreecloud::browser {

class BookmarkDestinationController {
 public:
  [[nodiscard]] constexpr std::optional<BookmarkAuthority> choose(
      bool user_confirmed,
      BookmarkAuthority destination) const noexcept {
    if (!user_confirmed) {
      return std::nullopt;
    }
    return destination;
  }
};

class VaultDestinationController {
 public:
  [[nodiscard]] constexpr std::optional<VaultAuthority> choose(
      bool user_confirmed,
      VaultAuthority destination) const noexcept {
    if (!user_confirmed) {
      return std::nullopt;
    }
    return destination;
  }
};

inline constexpr bool kBookmarkDestinationMustBeExplicit = true;
inline constexpr bool kVaultDestinationMustBeExplicit = true;
inline constexpr bool kLocalBookmarkAutoSyncAllowed = false;
inline constexpr bool kLocalVaultAutoSyncAllowed = false;

static_assert(kBookmarkDestinationMustBeExplicit);
static_assert(kVaultDestinationMustBeExplicit);
static_assert(!kLocalBookmarkAutoSyncAllowed);
static_assert(!kLocalVaultAutoSyncAllowed);

}  // namespace goreecloud::browser
