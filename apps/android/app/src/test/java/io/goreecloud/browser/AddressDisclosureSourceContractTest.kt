package io.goreecloud.browser

import java.io.File
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test

class AddressDisclosureSourceContractTest {
    @Test
    fun clipboardAndShareUseBrowserDisclosurePolicy() {
        val activity = sourceText("src/main/java/io/goreecloud/browser/BrowserActivity.kt")
        val safeCall = "AddressDisclosurePolicy.forExternalUse(currentUrl)"

        assertEquals(2, activity.windowed(safeCall.length).count { it == safeCall })
        assertFalse(activity.contains("putExtra(Intent.EXTRA_TEXT, currentUrl)"))
        assertFalse(
            activity.contains(
                "ClipData.newPlainText(getString(R.string.page_address_clip_label), currentUrl)",
            ),
        )
        assertTrue(activity.contains("setText(currentUrl)"))
        assertTrue(activity.contains("webView.loadUrl(target)"))
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
