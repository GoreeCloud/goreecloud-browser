package io.goreecloud.browser

import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertNotEquals
import org.junit.Assert.assertTrue
import org.junit.Test

class NavigationResolverTest {
    @Test
    fun emptyInputUsesGoreeCloudSearchHome() {
        assertEquals(NavigationResolver.SEARCH_HOME, NavigationResolver.resolve("   "))
    }

    @Test
    fun directHttpsNavigationRemainsIndependentFromSearch() {
        assertEquals(
            "https://example.com/path?q=1",
            NavigationResolver.resolve("https://example.com/path?q=1"),
        )
    }

    @Test
    fun bareHostUpgradesToHttps() {
        assertEquals("https://example.com", NavigationResolver.resolve("example.com"))
    }

    @Test
    fun textQueryUsesOnlyGoreeCloudSearch() {
        assertEquals(
            "https://search.goreecloud.com/search?q=privacy%20browser",
            NavigationResolver.resolve("privacy browser"),
        )
    }

    @Test
    fun alternateSchemesAreNotAcceptedAsWebNavigation() {
        assertTrue(NavigationResolver.isAllowedWebUrl("https://example.com"))
        assertTrue(NavigationResolver.isAllowedWebUrl("http://example.com"))
        assertFalse(NavigationResolver.isAllowedWebUrl("file:///sdcard/example.html"))
        assertFalse(NavigationResolver.isAllowedWebUrl("javascript:alert(1)"))
        assertFalse(NavigationResolver.isAllowedWebUrl("intent://example"))
    }

    @Test
    fun malformedHttpAddressesAreNotPassedToWebView() {
        assertFalse(NavigationResolver.isAllowedWebUrl("https://"))
        assertFalse(NavigationResolver.isAllowedWebUrl("http://?query=missing-host"))
        assertFalse(NavigationResolver.isAllowedWebUrl("https://:443/path"))
        assertFalse(NavigationResolver.isAllowedWebUrl("https://user@"))
        assertFalse(NavigationResolver.isAllowedWebUrl("https://example.com/\nnext"))
    }

    @Test
    fun malformedExplicitAddressFallsBackToGoreeCloudSearchResolution() {
        val resolved = NavigationResolver.resolve("https://")
        assertNotEquals("https://", resolved)
        assertEquals(
            "https://search.goreecloud.com/search?q=https%3A%2F%2F",
            resolved,
        )
    }

    @Test
    fun validAuthorityFormsRemainAccepted() {
        assertTrue(NavigationResolver.isAllowedWebUrl("http://localhost:8080/path"))
        assertTrue(NavigationResolver.isAllowedWebUrl("https://user:pass@example.com/path"))
        assertTrue(NavigationResolver.isAllowedWebUrl("https://[2001:db8::1]/path"))
    }
}
