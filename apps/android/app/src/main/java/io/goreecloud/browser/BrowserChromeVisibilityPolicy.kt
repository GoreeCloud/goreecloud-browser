package io.goreecloud.browser

/**
 * Pure policy for Browser-owned top-chrome visibility during page scrolling.
 *
 * Accessibility-service state is intentionally represented only as a boolean.
 * The Browser does not need service identities, capabilities, or user content
 * to keep its primary address/search control discoverable.
 */
internal object BrowserChromeVisibilityPolicy {
    enum class Decision {
        SHOW,
        HIDE,
        KEEP,
    }

    fun forScroll(
        addressFocused: Boolean,
        accessibilityEnabled: Boolean,
        scrollY: Int,
        oldScrollY: Int,
        chromeGutterPx: Int,
        directionSlopPx: Int,
        autoHideThresholdPx: Int,
    ): Decision {
        if (addressFocused || accessibilityEnabled) return Decision.SHOW

        val delta = scrollY - oldScrollY
        return when {
            scrollY <= chromeGutterPx -> Decision.SHOW
            delta > directionSlopPx && scrollY > autoHideThresholdPx -> Decision.HIDE
            delta < -directionSlopPx -> Decision.SHOW
            else -> Decision.KEEP
        }
    }
}
