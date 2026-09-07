package io.goreecloud.browser

import org.junit.Assert.assertEquals
import org.junit.Test

class BrowserChromeVisibilityPolicyTest {
    @Test
    fun accessibilityServiceKeepsTopChromeVisibleDuringDownwardScroll() {
        assertEquals(
            BrowserChromeVisibilityPolicy.Decision.SHOW,
            decision(accessibilityEnabled = true, scrollY = 500, oldScrollY = 450),
        )
    }

    @Test
    fun focusedAddressKeepsTopChromeVisible() {
        assertEquals(
            BrowserChromeVisibilityPolicy.Decision.SHOW,
            decision(addressFocused = true, scrollY = 500, oldScrollY = 450),
        )
    }

    @Test
    fun ordinaryDownwardScrollBeyondThresholdHidesTopChrome() {
        assertEquals(
            BrowserChromeVisibilityPolicy.Decision.HIDE,
            decision(scrollY = 500, oldScrollY = 450),
        )
    }

    @Test
    fun ordinaryUpwardScrollShowsTopChrome() {
        assertEquals(
            BrowserChromeVisibilityPolicy.Decision.SHOW,
            decision(scrollY = 450, oldScrollY = 500),
        )
    }

    @Test
    fun pageTopShowsTopChrome() {
        assertEquals(
            BrowserChromeVisibilityPolicy.Decision.SHOW,
            decision(scrollY = 8, oldScrollY = 40),
        )
    }

    @Test
    fun subSlopScrollKeepsCurrentChromeState() {
        assertEquals(
            BrowserChromeVisibilityPolicy.Decision.KEEP,
            decision(scrollY = 300, oldScrollY = 296),
        )
    }

    private fun decision(
        addressFocused: Boolean = false,
        accessibilityEnabled: Boolean = false,
        scrollY: Int,
        oldScrollY: Int,
    ): BrowserChromeVisibilityPolicy.Decision = BrowserChromeVisibilityPolicy.forScroll(
        addressFocused = addressFocused,
        accessibilityEnabled = accessibilityEnabled,
        scrollY = scrollY,
        oldScrollY = oldScrollY,
        chromeGutterPx = 16,
        directionSlopPx = 8,
        autoHideThresholdPx = 96,
    )
}
