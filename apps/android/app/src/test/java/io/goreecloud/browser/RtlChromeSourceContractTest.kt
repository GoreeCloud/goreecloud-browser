package io.goreecloud.browser

import java.io.File
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test

class RtlChromeSourceContractTest {
    @Test
    fun manifestDeclaresRtlSupport() {
        val manifest = sourceText("src/main/AndroidManifest.xml")
        assertTrue(manifest.contains("android:supportsRtl=\"true\""))
    }

    @Test
    fun directionalHistoryIconsAutoMirror() {
        val back = sourceText("src/main/res/drawable/ic_back.xml")
        val forward = sourceText("src/main/res/drawable/ic_forward.xml")

        assertTrue(back.contains("android:autoMirrored=\"true\""))
        assertTrue(forward.contains("android:autoMirrored=\"true\""))
    }

    @Test
    fun programmaticChromeAvoidsPhysicalHorizontalMargins() {
        val activity = sourceText("src/main/java/io/goreecloud/browser/BrowserActivity.kt")

        assertTrue(activity.contains("marginStart"))
        assertTrue(activity.contains("marginEnd"))
        assertFalse(activity.contains("leftMargin"))
        assertFalse(activity.contains("rightMargin"))
    }

    @Test
    fun focusedAddressKeepsAuthorityWhileUnfocusedUsesPresentationHelper() {
        val activity = sourceText("src/main/java/io/goreecloud/browser/BrowserActivity.kt")

        assertTrue(activity.contains("setText(currentUrl)"))
        assertTrue(activity.contains("addressField.setText(AddressPresentation.condensed(currentUrl))"))
    }

    private fun sourceText(relativePath: String): String {
        val candidates = listOf(
            File(relativePath),
            File("app/$relativePath"),
        )
        val source = candidates.firstOrNull { it.isFile }
            ?: error("Unable to resolve Android source fixture: $relativePath from ${File(".").absolutePath}")
        return source.readText()
    }
}
