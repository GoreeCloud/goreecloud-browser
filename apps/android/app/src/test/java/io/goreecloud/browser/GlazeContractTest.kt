package io.goreecloud.browser

import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test

class GlazeContractTest {
    @Test
    fun androidBrowserTargetsCurrentStableGlazeContract() {
        assertEquals("1.1.0", GlazeContract.VERSION)
        assertEquals("v1.1.0", GlazeContract.STABLE_TAG)
        assertEquals(
            "15cc76d2bcd4065552dc31c77145b63f34d9e7b2",
            GlazeContract.STABLE_RELEASE_REVISION,
        )
        assertEquals(
            "8ea1f789bbabf943c3359514dc1506b24fa3c51b",
            GlazeContract.ACCEPTED_VISUAL_SOURCE,
        )
        assertEquals(
            "contracts/v1.1/optical-refinement.json",
            GlazeContract.OPTICAL_CONTRACT,
        )
    }

    @Test
    fun touchTargetFloorsMatchV11AccessibilityContract() {
        assertEquals(48, GlazeContract.targetFloorDp(touchAssistance = false))
        assertEquals(56, GlazeContract.targetFloorDp(touchAssistance = true))
        assertTrue(GlazeContract.satisfiesGeneralTargetFloor(48))
        assertFalse(GlazeContract.satisfiesGeneralTargetFloor(47))
        assertTrue(GlazeContract.satisfiesTouchAssistanceTargetFloor(56))
        assertFalse(GlazeContract.satisfiesTouchAssistanceTargetFloor(55))
    }

    @Test
    fun mobileChromePreservesApplicationAuthorityAndGlazeBudget() {
        val mapping = GlazeContract.ANDROID_BROWSER_MAPPING

        assertEquals(GlazeContract.MaterialLevel.Canvas, mapping.canvas)
        assertEquals(GlazeContract.MaterialLevel.Canvas, mapping.topChrome)
        assertEquals(GlazeContract.MaterialLevel.SoftGlaze, mapping.omnibox)
        assertEquals(GlazeContract.MaterialLevel.Surface, mapping.addressField)
        assertEquals(GlazeContract.MaterialLevel.Surface, mapping.bottomChrome)
        assertEquals(GlazeContract.MaterialLevel.SoftGlaze, mapping.browserMenu)
        assertEquals(GlazeContract.ShellSurface.Application, mapping.shellSurface)
        assertEquals(GlazeContract.Clarity.Balanced, mapping.clarity)
        assertEquals(GlazeContract.Expression.Calm, mapping.expression)
        assertEquals(GlazeContract.DensityProfile.Standard, mapping.density)
        assertTrue(GlazeContract.satisfiesSystemGlazeBudget(mapping))
        assertFalse(mapping.declaresUniversalSearch)
        assertFalse(mapping.declaresControlCenter)
    }

    @Test
    fun v11OpticalAtmosphereRemainsBoundedAndNonAuthoritative() {
        val mapping = GlazeContract.ANDROID_BROWSER_MAPPING

        assertEquals("upper-left", GlazeContract.OPTICAL_LIGHT_ORIGIN)
        assertEquals("deep-teal", GlazeContract.ATMOSPHERE_PRIMARY)
        assertEquals("soft-amber", GlazeContract.ATMOSPHERE_SECONDARY)
        assertEquals(
            setOf(
                GlazeContract.Appearance.Light,
                GlazeContract.Appearance.Dark,
                GlazeContract.Appearance.DeepDark,
            ),
            mapping.supportedAppearances,
        )
        assertFalse(mapping.nestedBackdropBlur)
        assertFalse(mapping.environmentalColorSamplingRequired)
    }

    @Test
    fun mobileChromeRemovesDevelopmentScaffoldingFromNormalBrowsing() {
        val mapping = GlazeContract.ANDROID_BROWSER_MAPPING

        assertTrue(mapping.noActionBar)
        assertFalse(mapping.developerStatusInNormalChrome)
        assertTrue(mapping.effectsFreeFallback)
        assertTrue(mapping.usesNativeControls)
        assertTrue(mapping.usesVectorChromeIcons)
        assertFalse(mapping.usesPlatformPopupMenu)
        assertTrue(mapping.scrollAwareTopChrome)
    }

    @Test
    fun semanticPriorityKeepsDisabledAboveErrorAndFocus() {
        assertTrue(
            GlazeContract.statePriority(GlazeContract.InteractionState.Disabled) >
                GlazeContract.statePriority(GlazeContract.InteractionState.Error),
        )
        assertTrue(
            GlazeContract.statePriority(GlazeContract.InteractionState.Error) >
                GlazeContract.statePriority(GlazeContract.InteractionState.Pressed),
        )
        assertTrue(
            GlazeContract.statePriority(GlazeContract.InteractionState.Pressed) >
                GlazeContract.statePriority(GlazeContract.InteractionState.Focus),
        )
        assertTrue(
            GlazeContract.statePriority(GlazeContract.InteractionState.Focus) >
                GlazeContract.statePriority(GlazeContract.InteractionState.Selected),
        )
        assertTrue(
            GlazeContract.statePriority(GlazeContract.InteractionState.Selected) >
                GlazeContract.statePriority(GlazeContract.InteractionState.Hover),
        )
        assertTrue(
            GlazeContract.statePriority(GlazeContract.InteractionState.Hover) >
                GlazeContract.statePriority(GlazeContract.InteractionState.Rest),
        )
    }

    @Test
    fun mobileChromeBudgetsExpandedAndCollapsedViewportStates() {
        assertEquals(128, GlazeContract.fixedChromeHeightDp())
        assertEquals(56, GlazeContract.collapsedChromeHeightDp())
        assertTrue(
            GlazeContract.collapsedChromeHeightDp() < GlazeContract.fixedChromeHeightDp(),
        )
    }
}
