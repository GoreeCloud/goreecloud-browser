package io.goreecloud.browser

import org.junit.Assert.assertEquals
import org.junit.Test

class AddressPresentationTest {
    @Test
    fun condensedAddressKeepsHostAtLeadingEdge() {
        assertEquals(
            "search.goreecloud.com/preferences",
            AddressPresentation.condensed("https://search.goreecloud.com/preferences"),
        )
    }

    @Test
    fun condensedAddressPreservesPathAndQueryWithoutScheme() {
        assertEquals(
            "example.com/path/to/page?q=browser",
            AddressPresentation.condensed("https://example.com/path/to/page?q=browser"),
        )
    }

    @Test
    fun rootPathIsNotShown() {
        assertEquals(
            "search.goreecloud.com",
            AddressPresentation.condensed("https://search.goreecloud.com/"),
        )
    }

    @Test
    fun nonHostAddressFallsBackToOriginalText() {
        assertEquals("about:blank", AddressPresentation.condensed("about:blank"))
    }
}
