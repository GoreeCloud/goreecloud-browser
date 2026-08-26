#pragma once

#include <array>
#include <string_view>

#include "goreecloud/browser/glaze.hpp"

namespace goreecloud::browser {

enum class GlazeAcceptanceArea {
  semantic_color,
  material_hierarchy,
  iconography,
  motion,
  accessibility,
  responsive_layout,
  safe_areas,
  density,
  mixed_input,
  localization,
  security_truth,
  privacy_truth,
};

struct GlazeAcceptanceRequirement {
  GlazeAcceptanceArea area;
  std::string_view evidence;
};

inline constexpr std::array kGlazeAcceptanceRequirements{
    GlazeAcceptanceRequirement{GlazeAcceptanceArea::semantic_color,
                               "Browser surfaces use Glaze semantic roles rather than ad-hoc color meaning."},
    GlazeAcceptanceRequirement{GlazeAcceptanceArea::material_hierarchy,
                               "Canvas, Solid, Raised, Functional Glass, Clear Glass, and Overlay are used by purpose."},
    GlazeAcceptanceRequirement{GlazeAcceptanceArea::iconography,
                               "Browser-owned icons follow current Glaze iconography contracts."},
    GlazeAcceptanceRequirement{GlazeAcceptanceArea::motion,
                               "Motion is purposeful, interruptible, and has reduced-motion substitutions."},
    GlazeAcceptanceRequirement{GlazeAcceptanceArea::accessibility,
                               "Focus, contrast, transparency fallbacks, target floors, and assistive semantics are validated."},
    GlazeAcceptanceRequirement{GlazeAcceptanceArea::responsive_layout,
                               "Browser chrome and owned surfaces adapt across supported Glaze layout classes."},
    GlazeAcceptanceRequirement{GlazeAcceptanceArea::safe_areas,
                               "System insets, cutouts, gesture regions, and window chrome constraints are respected."},
    GlazeAcceptanceRequirement{GlazeAcceptanceArea::density,
                               "Density changes preserve hierarchy, target floors, and task usability."},
    GlazeAcceptanceRequirement{GlazeAcceptanceArea::mixed_input,
                               "Touch, pointer, keyboard, and other supported input modes retain complete interaction states."},
    GlazeAcceptanceRequirement{GlazeAcceptanceArea::localization,
                               "Layout tolerates localization, text expansion, directionality, and intrinsic overflow."},
    GlazeAcceptanceRequirement{GlazeAcceptanceArea::security_truth,
                               "Glaze presents Wardveil evidence-backed state without inventing security truth."},
    GlazeAcceptanceRequirement{GlazeAcceptanceArea::privacy_truth,
                               "Glaze presents Privacy Shield state without inventing privacy truth."},
};

inline constexpr bool kGlazeAcceptanceRequiredForProduction = true;

static_assert(kCurrentGlazeUiStableVersion == std::string_view{"1.5.0"});
static_assert(kGlazeAcceptanceRequiredForProduction);

}  // namespace goreecloud::browser
