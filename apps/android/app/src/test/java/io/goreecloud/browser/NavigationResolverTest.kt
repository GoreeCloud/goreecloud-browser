package io.goreecloud.browser

import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
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
}
