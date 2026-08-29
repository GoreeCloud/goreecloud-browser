package io.goreecloud.browser

/**
 * Browser-owned native mapping metadata for the current Stable Glaze UI contract.
 *
 * This records the semantic contract consumed by the Android shell. It is source
 * mapping evidence only; native-device visual/accessibility acceptance remains a
 * separate promotion gate.
 */
object GlazeContract {
    const val VERSION = "2.0.0"
    const val STABLE_PROMOTION_REVISION = "ff3fff4306bd53ea9c0715a7c0d64265bb038617"

    const val GENERAL_TARGET_DP = 48
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

    data class AndroidBrowserMapping(
        val canvas: MaterialLevel,
        val topChrome: MaterialLevel,
        val omnibox: MaterialLevel,
        val addressField: MaterialLevel,
        val bottomChrome: MaterialLevel,
        val browserMenu: MaterialLevel,
        val clarity: Clarity,
        val expression: Expression,
        val noActionBar: Boolean,
        val developerStatusInNormalChrome: Boolean,
        val effectsFreeFallback: Boolean,
        val usesNativeControls: Boolean,
        val usesVectorChromeIcons: Boolean,
        val usesPlatformPopupMenu: Boolean,
        val scrollAwareTopChrome: Boolean,
    )

    val ANDROID_BROWSER_MAPPING = AndroidBrowserMapping(
        canvas = MaterialLevel.Canvas,
        topChrome = MaterialLevel.Canvas,
        omnibox = MaterialLevel.SoftGlaze,
        addressField = MaterialLevel.Surface,
        bottomChrome = MaterialLevel.Surface,
        browserMenu = MaterialLevel.SoftGlaze,
        clarity = Clarity.Balanced,
        expression = Expression.Calm,
        noActionBar = true,
        developerStatusInNormalChrome = false,
        effectsFreeFallback = true,
        usesNativeControls = true,
        usesVectorChromeIcons = true,
        usesPlatformPopupMenu = false,
        scrollAwareTopChrome = true,
    )

    fun satisfiesGeneralTargetFloor(sizeDp: Int): Boolean = sizeDp >= GENERAL_TARGET_DP

    fun fixedChromeHeightDp(): Int =
        OMNIBOX_HEIGHT_DP + (CHROME_GUTTER_DP * 2) + BOTTOM_TOOLBAR_HEIGHT_DP

    fun collapsedChromeHeightDp(): Int = BOTTOM_TOOLBAR_HEIGHT_DP
}
