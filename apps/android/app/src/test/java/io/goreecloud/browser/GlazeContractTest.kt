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
    fun androidBrowserMappingUsesNativeAccessibleFallbackSemantics() {
        val mapping = GlazeContract.ANDROID_BROWSER_MAPPING

        assertEquals(GlazeContract.MaterialLevel.Canvas, mapping.canvas)
        assertEquals(GlazeContract.MaterialLevel.Glaze, mapping.navigation)
        assertEquals(GlazeContract.MaterialLevel.Surface, mapping.addressField)
        assertEquals(GlazeContract.Clarity.Balanced, mapping.clarity)
        assertEquals(GlazeContract.Expression.Calm, mapping.expression)
        assertTrue(mapping.effectsFreeFallback)
        assertTrue(mapping.usesNativeControls)
    }
}
