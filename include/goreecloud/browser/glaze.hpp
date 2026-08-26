#pragma once

#include <cstdint>

namespace goreecloud::browser {

enum class GlazeSurface { canvas, solid, raised, glaze, overlay };
enum class LayoutClass { compact, medium, expanded, wide };

enum class ColorMode { light, dark, system };

struct AccessibilityPreferences {
  bool reduced_motion{false};
  bool reduced_transparency{false};
  bool increased_contrast{false};
  bool forced_colors{false};
};

struct GlazeCapabilities {
  std::uint16_t minimum_target_px{44};
  std::uint16_t comfortable_target_px{48};
  bool visible_keyboard_focus{true};
  bool solid_transparency_fallback{true};
  bool adaptive_layouts{true};
};

struct GlazeContext {
  ColorMode color_mode{ColorMode::system};
  LayoutClass layout{LayoutClass::medium};
  AccessibilityPreferences accessibility;
};

inline constexpr GlazeCapabilities kBrowserGlazeCapabilities{};

static_assert(kBrowserGlazeCapabilities.minimum_target_px >= 44);
static_assert(kBrowserGlazeCapabilities.visible_keyboard_focus);
static_assert(kBrowserGlazeCapabilities.solid_transparency_fallback);

}  // namespace goreecloud::browser
