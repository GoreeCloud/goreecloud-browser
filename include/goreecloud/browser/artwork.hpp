#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace goreecloud::browser {

inline constexpr std::string_view kBrowserArtworkName =
    "Glaze Navigation Ribbon";
inline constexpr std::string_view kBrowserArtworkCanonicalPath =
    "assets/branding/goreecloud-browser.svg";
inline constexpr std::string_view kBrowserArtworkMonochromePath =
    "assets/branding/goreecloud-browser-monochrome.svg";
inline constexpr std::string_view kBrowserArtworkCanonicalSha256 =
    "bfe541b8721cdacb68c0ff4a84d96f60612bbeb6cbccd352fa911bc2c9fa30ee";
inline constexpr std::string_view kBrowserArtworkMonochromeSha256 =
    "1e22eabf32341d672f0c79368a45013d9d84a181ed53ee1ae750d8103a4b501a";

enum class ArtworkTarget {
  web,
  favicon,
  linux,
  appimage,
  portable_linux,
  android_adaptive,
  android_round,
  android_monochrome,
};

struct ArtworkSource {
  std::string name{std::string{kBrowserArtworkName}};
  std::string canonical_svg_sha256{std::string{kBrowserArtworkCanonicalSha256}};
  std::string monochrome_source_sha256{std::string{kBrowserArtworkMonochromeSha256}};
};

struct ArtworkDerivative {
  ArtworkTarget target{ArtworkTarget::web};
  std::string filename;
  std::string sha256;
  std::string source_sha256;
};

struct ArtworkManifest {
  ArtworkSource source;
  std::vector<ArtworkDerivative> derivatives;
  bool deterministic_generation{true};
};

class ArtworkValidator {
 public:
  virtual ~ArtworkValidator() = default;
  [[nodiscard]] virtual bool validate(const ArtworkManifest& manifest) const = 0;
};

static_assert(kBrowserArtworkName == std::string_view{"Glaze Navigation Ribbon"});
static_assert(!kBrowserArtworkCanonicalSha256.empty());
static_assert(!kBrowserArtworkMonochromeSha256.empty());

// Every product derivative must remain traceable to the approved canonical
// GoreeCloud source. Upstream browser artwork is not GoreeCloud identity.

}  // namespace goreecloud::browser
