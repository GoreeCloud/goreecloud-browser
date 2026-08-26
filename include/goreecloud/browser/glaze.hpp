#pragma once

#include <cstdint>
#include <string_view>

namespace goreecloud::browser {

inline constexpr std::string_view kGlazeUiPolicy = "latest-approved-stable";
inline constexpr std::string_view kCurrentGlazeUiStableVersion = "1.5.0";
inline constexpr std::string_view kCurrentGlazeUiStableRevision =
    "2e1618397f6ebcdd254a76bfdd7e98846f2c5aa3";

enum class GlazeSurface {
  canvas,
  solid,
  raised,
  functional_glass,
  clear_glass,
  overlay,
};

enum class LayoutClass { compact, medium, expanded, wide };
enum class ColorMode { light, dark, system };
enum class DensityMode { compact, standard, comfortable };

enum class InteractionState {
  idle,
  hover,
  pressed,
  selected,
  expanded,
  disabled,
  read_only,
  loading,
  invalid,
  success,
};

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
  bool semantic_color{true};
  bool governed_iconography{true};
  bool interruptible_motion{true};
  bool safe_area_support{true};
  bool mixed_input_modality{true};
};

struct GlazeContext {
  ColorMode color_mode{ColorMode::system};
  LayoutClass layout{LayoutClass::medium};
  DensityMode density{DensityMode::standard};
  AccessibilityPreferences accessibility;
};

inline constexpr GlazeCapabilities kBrowserGlazeCapabilities{};

static_assert(kBrowserGlazeCapabilities.minimum_target_px >= 44);
static_assert(kBrowserGlazeCapabilities.visible_keyboard_focus);
static_assert(kBrowserGlazeCapabilities.solid_transparency_fallback);
static_assert(kBrowserGlazeCapabilities.semantic_color);
static_assert(kBrowserGlazeCapabilities.governed_iconography);
static_assert(kBrowserGlazeCapabilities.interruptible_motion);

// Browser-owned user-facing surfaces must track the current approved Stable
// Glaze UI release. The concrete version/revision above is the present adoption
// baseline and must be advanced whenever Glaze UI promotes a newer Stable.
// A superseded Glaze UI baseline cannot satisfy production readiness.

}  // namespace goreecloud::browser
