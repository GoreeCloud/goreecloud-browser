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
    fun directUnicodeHostRemainsIndependentFromSearch() {
        val address = "https://例え.テスト/path"
        assertTrue(NavigationResolver.isAllowedWebUrl(address))
        assertEquals(address, NavigationResolver.resolve(address))
    }

    @Test
    fun malformedExplicitHttpNavigationFailsClosedToSearchHome() {
        assertFalse(NavigationResolver.isAllowedWebUrl("https://"))
        assertFalse(NavigationResolver.isAllowedWebUrl("https://not a url"))
        assertFalse(NavigationResolver.isAllowedWebUrl("https://example.com:bad"))
        assertFalse(NavigationResolver.isAllowedWebUrl("https://example.com:65536"))
        assertFalse(NavigationResolver.isAllowedWebUrl("https:example.com"))

        assertEquals(NavigationResolver.SEARCH_HOME, NavigationResolver.resolve("https://"))
        assertEquals(
            NavigationResolver.SEARCH_HOME,
            NavigationResolver.resolve("https://example.com:bad"),
        )
        assertEquals(
            NavigationResolver.SEARCH_HOME,
            NavigationResolver.resolve("https:example.com"),
        )
    }

    @Test
    fun bareHostUpgradesToHttps() {
        assertEquals("https://example.com", NavigationResolver.resolve("example.com"))
    }

    @Test
    fun bareEmailLikeInputUsesSearchInsteadOfUserinfoNavigation() {
        assertEquals(
            "https://search.goreecloud.com/search?q=person%40example.com",
            NavigationResolver.resolve("person@example.com"),
        )
        assertEquals(
            "https://search.goreecloud.com/search?q=person%40localhost%3A8080",
            NavigationResolver.resolve("person@localhost:8080"),
        )
    }

    @Test
    fun explicitHttpUserinfoRemainsOutsideBareHostInferenceChange() {
        val address = "https://person@example.com/path"
        assertTrue(NavigationResolver.isAllowedWebUrl(address))
        assertEquals(address, NavigationResolver.resolve(address))
    }

    @Test
    fun bareLocalhostWithPortUpgradesToHttps() {
        assertEquals(
            "https://localhost:8080/path",
            NavigationResolver.resolve("localhost:8080/path"),
        )
    }

    @Test
    fun bareBracketedIpv6WithPortUpgradesToHttps() {
        assertEquals(
            "https://[::1]:8080/path",
            NavigationResolver.resolve("[::1]:8080/path"),
        )
    }

    @Test
    fun invalidBarePortIsTreatedAsSearchText() {
        assertEquals(
            "https://search.goreecloud.com/search?q=example.com%3Abad",
            NavigationResolver.resolve("example.com:bad"),
        )
    }

    @Test
    fun textQueryUsesOnlyGoreeCloudSearch() {
        assertEquals(
            "https://search.goreecloud.com/search?q=privacy%20browser",
            NavigationResolver.resolve("privacy browser"),
        )
    }

    @Test
    fun malformedWebSchemesNeverQualifyForExternalHandoff() {
        assertFalse(
            NavigationResolver.shouldHandOffExternally(
                "https://example.com:bad",
                hasUserGesture = true,
            ),
        )
        assertFalse(
            NavigationResolver.shouldHandOffExternally(
                "https:example.com",
                hasUserGesture = true,
            ),
        )
        assertFalse(
            NavigationResolver.shouldHandOffExternally(
                "HTTP://example.com:65536",
                hasUserGesture = true,
            ),
        )
    }

    @Test
    fun localAndActiveContentSchemesNeverQualifyForExternalHandoff() {
        val blocked = listOf(
            "about:blank",
            "blob:https://example.com/id",
            "content://com.example.provider/item/1",
            "data:text/plain,hello",
            "file:///sdcard/example.html",
            "javascript:alert(1)",
        )

        blocked.forEach { target ->
            assertFalse(
                "Local/active-content scheme escaped Browser: $target",
                NavigationResolver.shouldHandOffExternally(target, hasUserGesture = true),
            )
        }
    }

    @Test
    fun externalSchemeRequiresParsedSchemeAndUserGesture() {
        assertTrue(
            NavigationResolver.shouldHandOffExternally(
                "mailto:person@example.com",
                hasUserGesture = true,
            ),
        )
        assertTrue(
            NavigationResolver.shouldHandOffExternally(
                "tel:+15551234567",
                hasUserGesture = true,
            ),
        )
        assertTrue(
            NavigationResolver.shouldHandOffExternally(
                "intent://example",
                hasUserGesture = true,
            ),
        )
        assertFalse(
            NavigationResolver.shouldHandOffExternally(
                "intent://example",
                hasUserGesture = false,
            ),
        )
        assertFalse(
            NavigationResolver.shouldHandOffExternally(
                "not a scheme",
                hasUserGesture = true,
            ),
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
