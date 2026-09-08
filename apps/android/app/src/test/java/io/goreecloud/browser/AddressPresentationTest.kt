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
    fun condensedAddressPreservesFragment() {
        assertEquals(
            "example.com/path?q=browser#section-a",
            AddressPresentation.condensed("https://example.com/path?q=browser#section-a"),
        )
    }

    @Test
    fun condensedAddressPreservesRawEscapingInFragment() {
        assertEquals(
            "example.com/path#section%20one",
            AddressPresentation.condensed("https://example.com/path#section%20one"),
        )
    }

    @Test
    fun condensedAddressPreservesExplicitPort() {
        assertEquals(
            "example.com:8443/path?q=browser",
            AddressPresentation.condensed("https://example.com:8443/path?q=browser"),
        )
    }

    @Test
    fun condensedAddressPreservesExplicitDefaultPort() {
        assertEquals(
            "example.com:443",
            AddressPresentation.condensed("https://example.com:443/"),
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

    @Test
    fun presentationRemovesUnicodeBidiControls() {
        val controlled = "example.com/\u202Epayload\u202C?q=\u2067value\u2069"
        assertEquals(
            "example.com/payload?q=value",
            AddressPresentation.withoutBidiControls(controlled),
        )
    }

    @Test
    fun condensedAddressRemovesBidiControlsFromFragment() {
        assertEquals(
            "example.com/path#section",
            AddressPresentation.condensed("https://example.com/path#\u202Esection\u202C"),
        )
    }

    @Test
    fun presentationRemovesBidiControlsFromFallbackText() {
        assertEquals(
            "about:blank",
            AddressPresentation.condensed("about:\u202Eblank\u202C"),
        )
    }

    @Test
    fun presentationPreservesOrdinaryRtlLetters() {
        val address = "example.com/مرحبا?q=اختبار"
        assertEquals(address, AddressPresentation.withoutBidiControls(address))
    }
}
