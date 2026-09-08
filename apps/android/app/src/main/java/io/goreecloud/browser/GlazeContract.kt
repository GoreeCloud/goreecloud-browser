package io.goreecloud.browser

/**
 * Browser-owned Android-native mapping metadata for GLAZE UI V1.2 Stable.
 *
 * This records the semantic and optical contract consumed by the Android shell.
 * It is repository-local source mapping evidence only; rendered/native-device
 * visual, accessibility, form-factor, performance, and production acceptance
 * remain separate promotion gates.
 */
object GlazeContract {
    const val VERSION = "1.2.0"
    const val STABLE_RELEASE_REVISION = "f285b9145e27e6e7027b075c37299d101945c272"
    const val OPTICAL_CONTRACT = "tokens/glaze-v1.2-optical-foundation.candidate.json"
    const val STABLE_WEB_ENTRYPOINT = "css/glaze-v1.2.0.css"
    const val STABLE_RUNTIME_ENTRYPOINT = "js/glaze-v1.2.0.mjs"
    const val MATERIAL_RULE = "neutral-glass-is-material-color-is-accent"

    const val GENERAL_TARGET_DP = 48
    const val TOUCH_ASSISTANCE_TARGET_DP = 56
    const val CHROME_GUTTER_DP = 8
    const val CONTROL_GAP_DP = 6
    const val OMNIBOX_HEIGHT_DP = 56
    const val BOTTOM_TOOLBAR_HEIGHT_DP = 56
    const val OMNIBOX_CORNER_DP = 28
    const val CHROME_CONTROL_CORNER_DP = 22
    const val MENU_CORNER_DP = 28
    const val MENU_ACTION_HEIGHT_DP = 56
    const val PROGRESS_HEIGHT_DP = 2
    const val AUTO_HIDE_SCROLL_THRESHOLD_DP = 72
    const val SCROLL_DIRECTION_SLOP_DP = 6

    const val MAX_DOMINANT_GLAZE_PANELS = 1
    const val MAX_SMALL_FLOATING_GLAZE_CONTROLS = 3
    const val OPTICAL_LIGHT_ORIGIN = "upper-left"
    const val MATERIAL_PRIMARY = "frost-white"
    const val ATMOSPHERE_ACCENT = "ice-blue"

    enum class MaterialLevel {
        Canvas,
        Surface,
        SoftGlaze,
        Glaze,
        DeepGlaze,
        LiveGlaze,
    }

    enum class Clarity {
        Clear,
        Balanced,
        Solid,
    }

    enum class Expression {
        Calm,
        Balanced,
        Expressive,
    }

    enum class ButtonRole {
        Quiet,
        Soft,
        Glaze,
        Emphasis,
    }

    enum class Appearance {
        Light,
        Dark,
        DeepDark,
    }

    enum class DensityProfile {
        Comfortable,
        Standard,
        Productive,
        Immersive,
    }

    /** Browser chrome remains an Application surface, not a system authority. */
    enum class ShellSurface {
        Workspace,
        Application,
        SystemOverlay,
        SystemPanel,
        CriticalSystem,
    }

    data class ResilienceProfile(
        val clarity: Clarity,
        val transparencyAllowed: Boolean,
        val outlineWidthDp: Int,
    )

    enum class InteractionState {
        Rest,
        Hover,
        Focus,
        Pressed,
        Selected,
        Disabled,
        Loading,
        Error,
    }

    data class AndroidBrowserMapping(
        val canvas: MaterialLevel,
        val topChrome: MaterialLevel,
        val omnibox: MaterialLevel,
        val addressField: MaterialLevel,
        val bottomChrome: MaterialLevel,
        val browserMenu: MaterialLevel,
        val shellSurface: ShellSurface,
        val clarity: Clarity,
        val expression: Expression,
        val density: DensityProfile,
        val supportedAppearances: Set<Appearance>,
        val dominantGlazePanels: Int,
        val smallFloatingGlazeControls: Int,
        val declaresUniversalSearch: Boolean,
        val declaresControlCenter: Boolean,
        val noActionBar: Boolean,
        val developerStatusInNormalChrome: Boolean,
        val effectsFreeFallback: Boolean,
        val usesNativeControls: Boolean,
        val usesVectorChromeIcons: Boolean,
        val usesPlatformPopupMenu: Boolean,
        val scrollAwareTopChrome: Boolean,
        val nestedBackdropBlur: Boolean,
        val environmentalColorSamplingRequired: Boolean,
    )

    val ANDROID_BROWSER_MAPPING = AndroidBrowserMapping(
        canvas = MaterialLevel.Canvas,
        topChrome = MaterialLevel.Canvas,
        omnibox = MaterialLevel.SoftGlaze,
        addressField = MaterialLevel.Surface,
        bottomChrome = MaterialLevel.Surface,
        browserMenu = MaterialLevel.SoftGlaze,
        shellSurface = ShellSurface.Application,
        clarity = Clarity.Balanced,
        expression = Expression.Calm,
        density = DensityProfile.Standard,
        supportedAppearances = setOf(Appearance.Light, Appearance.Dark, Appearance.DeepDark),
        dominantGlazePanels = 1,
        smallFloatingGlazeControls = 1,
        declaresUniversalSearch = false,
        declaresControlCenter = false,
        noActionBar = true,
        developerStatusInNormalChrome = false,
        effectsFreeFallback = true,
        usesNativeControls = true,
        usesVectorChromeIcons = true,
        usesPlatformPopupMenu = false,
        scrollAwareTopChrome = true,
        nestedBackdropBlur = false,
        environmentalColorSamplingRequired = false,
    )

    fun targetFloorDp(touchAssistance: Boolean): Int =
        if (touchAssistance) TOUCH_ASSISTANCE_TARGET_DP else GENERAL_TARGET_DP

    fun satisfiesGeneralTargetFloor(sizeDp: Int): Boolean = sizeDp >= GENERAL_TARGET_DP

    fun satisfiesTouchAssistanceTargetFloor(sizeDp: Int): Boolean =
        sizeDp >= TOUCH_ASSISTANCE_TARGET_DP

    fun satisfiesSystemGlazeBudget(mapping: AndroidBrowserMapping): Boolean =
        mapping.dominantGlazePanels in 0..MAX_DOMINANT_GLAZE_PANELS &&
            mapping.smallFloatingGlazeControls in 0..MAX_SMALL_FLOATING_GLAZE_CONTROLS

    fun resilienceProfile(
        reducedTransparency: Boolean,
        increasedContrast: Boolean,
    ): ResilienceProfile = ResilienceProfile(
        clarity = if (reducedTransparency || increasedContrast) Clarity.Solid else Clarity.Balanced,
        transparencyAllowed = !reducedTransparency,
        outlineWidthDp = if (increasedContrast) 2 else 1,
    )

    /** Higher value means higher preserved semantic presentation priority. */
    fun statePriority(state: InteractionState): Int = when (state) {
        InteractionState.Rest -> 0
        InteractionState.Hover -> 1
        InteractionState.Selected -> 2
        InteractionState.Focus -> 3
        InteractionState.Pressed -> 4
        InteractionState.Loading -> 5
        InteractionState.Error -> 6
        InteractionState.Disabled -> 7
    }

    fun fixedChromeHeightDp(): Int =
        OMNIBOX_HEIGHT_DP + (CHROME_GUTTER_DP * 2) + BOTTOM_TOOLBAR_HEIGHT_DP

    fun collapsedChromeHeightDp(): Int = BOTTOM_TOOLBAR_HEIGHT_DP
}
