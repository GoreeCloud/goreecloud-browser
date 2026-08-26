#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace goreecloud::browser {

enum class BookmarkAuthority {
  local,
  goreecloud,
};

enum class BookmarkKind {
  page,
  folder,
  collection,
  read_later,
};

struct BookmarkNode {
  std::string id;
  BookmarkAuthority authority{BookmarkAuthority::local};
  BookmarkKind kind{BookmarkKind::page};
  std::string parent_id;
  std::string title;
  std::string url;
  std::uint64_t order{};
  std::vector<std::string> tags;
};

struct BookmarkSaveChoice {
  BookmarkAuthority destination{BookmarkAuthority::local};
  std::optional<std::string> parent_id;
  bool explicit_user_choice{true};
};

struct BookmarkMigration {
  std::string source_id;
  BookmarkAuthority source{BookmarkAuthority::local};
  BookmarkAuthority destination{BookmarkAuthority::goreecloud};
  bool explicit_user_action{false};
};

class BookmarkStore {
 public:
  virtual ~BookmarkStore() = default;
  virtual bool save(BookmarkNode node) = 0;
  virtual bool remove(const std::string& id) = 0;
  [[nodiscard]] virtual std::vector<BookmarkNode> children(
      const std::string& parent_id, BookmarkAuthority authority) const = 0;
  [[nodiscard]] virtual std::vector<BookmarkNode> search(
      const std::string& query, BookmarkAuthority authority) const = 0;
  virtual bool migrate(const BookmarkMigration& migration) = 0;
};

inline constexpr bool kOnlyTwoBookmarkAuthorities = true;
inline constexpr bool kLocalBookmarksSync = false;
inline constexpr bool kGoreeCloudBookmarksSync = true;
inline constexpr bool kSigningInUploadsLocalBookmarks = false;
inline constexpr bool kBookmarkSaveRequiresExplicitDestination = true;

static_assert(kOnlyTwoBookmarkAuthorities);
static_assert(!kLocalBookmarksSync);
static_assert(kGoreeCloudBookmarksSync);
static_assert(!kSigningInUploadsLocalBookmarks);
static_assert(kBookmarkSaveRequiresExplicitDestination);

}  // namespace goreecloud::browser
