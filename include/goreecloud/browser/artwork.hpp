#pragma once

#include <string>
#include <vector>

namespace goreecloud::browser {

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
  std::string name{"Glaze Navigation Ribbon"};
  std::string canonical_svg_sha256;
  std::string monochrome_source_sha256;
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

// Every product derivative must remain traceable to the approved canonical
// GoreeCloud source. Upstream browser artwork is not GoreeCloud identity.

}  // namespace goreecloud::browser
