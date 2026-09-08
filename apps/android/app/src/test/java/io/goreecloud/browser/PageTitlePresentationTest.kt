package io.goreecloud.browser

import org.junit.Assert.assertEquals
import org.junit.Test

class PageTitlePresentationTest {
    @Test
    fun `ordinary title remains visible`() {
        assertEquals(
            "Example News",
            PageTitlePresentation.display("  Example   News  ", "https://example.com/story"),
        )
    }

    @Test
    fun `control and bidi formatting characters cannot shape browser identity`() {
        assertEquals(
            "Trusted title alert",
            PageTitlePresentation.display("Trusted\u202E title\nalert", "https://example.com/"),
        )
    }

    @Test
    fun `raw resource path falls back to site identity`() {
        assertEquals(
            "example.com",
            PageTitlePresentation.display("/_proxy/cache/resource/42", "https://example.com/path"),
        )
    }

    @Test
    fun `url shaped page title falls back to site identity`() {
        assertEquals(
            "example.com",
            PageTitlePresentation.display("https://proxy.invalid/error?id=7", "https://example.com/path"),
        )
    }

    @Test
    fun `invalid current url uses browser identity fallback`() {
        assertEquals(
            "GoreeCloud Browser",
            PageTitlePresentation.display(null, "goreecloud://start"),
        )
    }

    @Test
    fun `remote title length is bounded`() {
        assertEquals(
            120,
            PageTitlePresentation.display("A".repeat(200), "https://example.com/").length,
        )
    }
}
