#pragma once

#include <cstdint>
#include <string_view>

namespace goreecloud::browser {

inline constexpr std::string_view kGlazeUiPolicy = "latest-approved-stable";
inline constexpr std::string_view kCurrentGlazeUiStableVersion = "2.0.0";
inline constexpr std::string_view kCurrentGlazeUiStableRevision =
    "ff3fff4306bd53ea9c0715a7c0d64265bb038617";

enum class GlazeSurface {
  canvas,
  surface,
  soft_glaze,
  glaze,
  deep_glaze,
  live_glaze,
};

enum class LayoutClass { compact, medium, expanded, wide };
enum class AppearanceMode { system, light, dark, deep_dark };
enum class ClarityMode { clear, balanced, solid };
enum class ExpressionMode { calm, balanced, expressive };
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
  std::uint16_t minimum_target_px{48};
  std::uint16_t television_target_px{56};
  bool visible_keyboard_focus{true};
  bool solid_transparency_fallback{true};
  bool adaptive_layouts{true};
  bool semantic_color{true};
  bool governed_iconography{true};
  bool interruptible_motion{true};
  bool safe_area_support{true};
  bool mixed_input_modality{true};
  bool connected_transformation{true};
  bool live_surfaces{true};
  bool navigation_capsule{true};
};

struct GlazeContext {
  AppearanceMode appearance{AppearanceMode::system};
  ClarityMode clarity{ClarityMode::balanced};
  ExpressionMode expression{ExpressionMode::balanced};
  LayoutClass layout{LayoutClass::medium};
  DensityMode density{DensityMode::standard};
  AccessibilityPreferences accessibility;
};

inline constexpr GlazeCapabilities kBrowserGlazeCapabilities{};

static_assert(kBrowserGlazeCapabilities.minimum_target_px >= 48);
static_assert(kBrowserGlazeCapabilities.television_target_px >= 56);
static_assert(kBrowserGlazeCapabilities.visible_keyboard_focus);
static_assert(kBrowserGlazeCapabilities.solid_transparency_fallback);
static_assert(kBrowserGlazeCapabilities.semantic_color);
static_assert(kBrowserGlazeCapabilities.governed_iconography);
static_assert(kBrowserGlazeCapabilities.interruptible_motion);
static_assert(kBrowserGlazeCapabilities.connected_transformation);
static_assert(kBrowserGlazeCapabilities.live_surfaces);
static_assert(kBrowserGlazeCapabilities.navigation_capsule);

// Browser-owned user-facing surfaces track the current approved Stable Glaze UI
// release. Version and revision are intentionally pinned so CI and acceptance
// evidence cannot silently drift to a different design-system baseline.

}  // namespace goreecloud::browser
