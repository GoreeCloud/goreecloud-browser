package io.goreecloud.browser

import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test

class GlazeContractTest {
    @Test
    fun androidBrowserTargetsCurrentStableGlazeContract() {
        assertEquals("1.3.0", GlazeContract.VERSION)
        assertEquals("Adaptive Resonance", GlazeContract.RELEASE_THEME)
        assertEquals(
            "fc7cc91d2eace8da2371371c2855c24cbcb326a1",
            GlazeContract.STABLE_RELEASE_REVISION,
        )
        assertEquals(
            "tokens/glaze-v1.2-optical-foundation.candidate.json",
            GlazeContract.OPTICAL_CONTRACT,
        )
        assertEquals(
            "contracts/v1.3/adaptive-resonance.plan.json",
            GlazeContract.ADAPTIVE_CONTRACT,
        )
        assertEquals("css/glaze-v1.3.0.css", GlazeContract.STABLE_WEB_ENTRYPOINT)
        assertEquals("js/glaze-v1.3.0.mjs", GlazeContract.STABLE_RUNTIME_ENTRYPOINT)
        assertEquals("1.2.0", GlazeContract.ROLLBACK_BASELINE_VERSION)
        assertEquals(
            "neutral-glass-is-material-color-is-accent",
            GlazeContract.MATERIAL_RULE,
        )
    }

    @Test
    fun touchTargetFloorsMatchV13AccessibilityContract() {
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
    fun v13MaterialRetainsNeutralFoundationAndBoundedAccent() {
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
    fun mobileChromeKeepsProductionInteractionCharacteristics() {
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
    fun resilienceProfileFailsSolidForReducedTransparencyAndContrast() {
        val normal = GlazeContract.resilienceProfile(false, false)
        assertEquals(GlazeContract.Clarity.Balanced, normal.clarity)
        assertTrue(normal.transparencyAllowed)
        assertEquals(1, normal.outlineWidthDp)

        val reduced = GlazeContract.resilienceProfile(true, false)
        assertEquals(GlazeContract.Clarity.Solid, reduced.clarity)
        assertFalse(reduced.transparencyAllowed)

        val contrast = GlazeContract.resilienceProfile(false, true)
        assertEquals(GlazeContract.Clarity.Solid, contrast.clarity)
        assertTrue(contrast.transparencyAllowed)
        assertEquals(2, contrast.outlineWidthDp)
    }

    @Test
    fun mobileChromeBudgetsExpandedAndCollapsedViewportStates() {
        assertEquals(128, GlazeContract.fixedChromeHeightDp())
        assertEquals(56, GlazeContract.collapsedChromeHeightDp())
        assertTrue(GlazeContract.collapsedChromeHeightDp() < GlazeContract.fixedChromeHeightDp())
    }
}
