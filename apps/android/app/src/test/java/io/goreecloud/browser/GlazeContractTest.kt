package io.goreecloud.browser

import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test

class GlazeContractTest {
    @Test
    fun androidBrowserTargetsCurrentStableGlazeContract() {
        assertEquals("1.2.0", GlazeContract.VERSION)
        assertEquals(
            "f285b9145e27e6e7027b075c37299d101945c272",
            GlazeContract.STABLE_RELEASE_REVISION,
        )
        assertEquals(
            "b0eadf9a60f73d45caffb62ffc7e9e0334cddc97",
            GlazeContract.SOURCE_QUALIFICATION_ANCHOR,
        )
        assertEquals(
            "tokens/glaze-v1.2-optical-foundation.candidate.json",
            GlazeContract.OPTICAL_CONTRACT,
        )
        assertEquals("css/glaze-v1.2.0.css", GlazeContract.STABLE_WEB_ENTRYPOINT)
        assertEquals("js/glaze-v1.2.0.mjs", GlazeContract.STABLE_RUNTIME_ENTRYPOINT)
        assertEquals(
            "neutral-glass-is-material-color-is-accent",
            GlazeContract.MATERIAL_RULE,
        )
    }

    @Test
    fun touchTargetFloorsMatchV12AccessibilityContract() {
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
    fun v12OpticalMaterialRemainsNeutralAndAccentBounded() {
        val mapping = GlazeContract.ANDROID_BROWSER_MAPPING

        assertEquals("upper-left", GlazeContract.OPTICAL_LIGHT_ORIGIN)
        assertEquals("frost-white", GlazeContract.MATERIAL_PRIMARY)
        assertEquals("ice-blue", GlazeContract.ATMOSPHERE_ACCENT)
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
    fun textBearingChromeCanGrowForLargeText() {
        val mapping = GlazeContract.ANDROID_BROWSER_MAPPING

        assertEquals(2.0f, GlazeContract.LARGE_TEXT_ACCEPTANCE_FONT_SCALE, 0.0f)
        assertTrue(mapping.textBearingChromeUsesContentHeight)
        assertEquals(128, GlazeContract.baselineExpandedChromeHeightDp())
        assertEquals(56, GlazeContract.collapsedChromeHeightDp())
        assertTrue(
            GlazeContract.collapsedChromeHeightDp() < GlazeContract.baselineExpandedChromeHeightDp(),
        )
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
}
