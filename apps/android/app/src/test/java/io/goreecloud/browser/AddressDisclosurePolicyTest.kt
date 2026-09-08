package io.goreecloud.browser

import org.junit.Assert.assertEquals
import org.junit.Test

class AddressDisclosurePolicyTest {
    @Test
    fun httpUserinfoIsRemovedBeforeExternalDisclosure() {
        assertEquals(
            "https://example.com:8443/path?q=browser#section",
            AddressDisclosurePolicy.forExternalUse(
                "https://user:password@example.com:8443/path?q=browser#section",
            ),
        )
    }

    @Test
    fun encodedUserinfoIsRemovedWithoutRewritingRemainingUrl() {
        assertEquals(
            "https://example.com/path?next=%2Fhome#done",
            AddressDisclosurePolicy.forExternalUse(
                "https://user%40name:p%40ss@example.com/path?next=%2Fhome#done",
            ),
        )
    }

    @Test
    fun explicitHttpUserinfoNavigationCanBeDisclosedWithoutUserinfo() {
        assertEquals(
            "https://example.com/path",
            AddressDisclosurePolicy.forExternalUse("https://person@example.com/path"),
        )
    }

    @Test
    fun ordinaryHttpAddressRemainsByteForByteUnchanged() {
        val address = "HTTPS://example.com:443/path?q=A%2FB#Frag"
        assertEquals(address, AddressDisclosurePolicy.forExternalUse(address))
    }

    @Test
    fun nonWebAndMalformedAddressesRemainOutsideThisPolicy() {
        assertEquals(
            "mailto:user@example.com",
            AddressDisclosurePolicy.forExternalUse("mailto:user@example.com"),
        )
        assertEquals("not a url", AddressDisclosurePolicy.forExternalUse("not a url"))
    }
}
