package io.goreecloud.browser

import org.junit.Assert.assertEquals
import org.junit.Assert.assertNull
import org.junit.Test

class BrowserFailureStatePolicyTest {
    @Test
    fun allowsOnlyValidatedWebRetryUrls() {
        assertEquals(
            "https://example.com/path?retry=1",
            BrowserFailureStatePolicy.restorable("https://example.com/path?retry=1")?.retryUrl,
        )
        assertEquals(
            "http://localhost:8080/status",
            BrowserFailureStatePolicy.restorable("http://localhost:8080/status")?.retryUrl,
        )
    }

    @Test
    fun rejectsMissingMalformedNonWebAndCredentialBearingRetryState() {
        assertNull(BrowserFailureStatePolicy.restorable(null))
        assertNull(BrowserFailureStatePolicy.restorable(""))
        assertNull(BrowserFailureStatePolicy.restorable("https://"))
        assertNull(BrowserFailureStatePolicy.restorable("https://example.com:70000/"))
        assertNull(BrowserFailureStatePolicy.restorable("goreecloud://start"))
        assertNull(BrowserFailureStatePolicy.restorable("file:///tmp/page.html"))
        assertNull(BrowserFailureStatePolicy.restorable("https://user:secret@example.com/private"))
        assertNull(BrowserFailureStatePolicy.restorable("https://user@example.com/private"))
    }

    @Test
    fun failureTitleIsBrowserOwnedAndFixed() {
        assertEquals("Page unavailable", BrowserFailureStatePolicy.PAGE_UNAVAILABLE_TITLE)
    }
}
