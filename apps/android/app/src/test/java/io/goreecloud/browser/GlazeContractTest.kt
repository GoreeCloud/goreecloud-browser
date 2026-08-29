package io.goreecloud.browser

import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test

class GlazeContractTest {
    @Test
    fun androidBrowserTargetsCurrentStableGlazeContract() {
        assertEquals("2.0.0", GlazeContract.VERSION)
        assertEquals(
            "ff3fff4306bd53ea9c0715a7c0d64265bb038617",
            GlazeContract.STABLE_PROMOTION_REVISION,
        )
    }

    @Test
    fun generalInteractiveTargetFloorIsEnforced() {
        assertTrue(GlazeContract.satisfiesGeneralTargetFloor(48))
        assertTrue(GlazeContract.satisfiesGeneralTargetFloor(56))
        assertFalse(GlazeContract.satisfiesGeneralTargetFloor(47))
    }

    @Test
    fun mobileChromeRemovesDevelopmentScaffoldingFromNormalBrowsing() {
        val mapping = GlazeContract.ANDROID_BROWSER_MAPPING

        assertEquals(GlazeContract.MaterialLevel.Canvas, mapping.canvas)
        assertEquals(GlazeContract.MaterialLevel.Canvas, mapping.topChrome)
        assertEquals(GlazeContract.MaterialLevel.SoftGlaze, mapping.omnibox)
        assertEquals(GlazeContract.MaterialLevel.Surface, mapping.addressField)
        assertEquals(GlazeContract.MaterialLevel.Surface, mapping.bottomChrome)
        assertEquals(GlazeContract.Clarity.Balanced, mapping.clarity)
        assertEquals(GlazeContract.Expression.Calm, mapping.expression)
        assertTrue(mapping.noActionBar)
        assertFalse(mapping.developerStatusInNormalChrome)
        assertTrue(mapping.effectsFreeFallback)
        assertTrue(mapping.usesNativeControls)
        assertTrue(mapping.usesVectorChromeIcons)
    }

    @Test
    fun fixedMobileChromeStaysWithinFirstBetaViewportBudget() {
        assertEquals(128, GlazeContract.fixedChromeHeightDp())
    }
}
