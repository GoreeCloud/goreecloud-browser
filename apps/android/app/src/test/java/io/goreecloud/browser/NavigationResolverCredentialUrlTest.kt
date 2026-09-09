package io.goreecloud.browser

import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test

class NavigationResolverCredentialUrlTest {
    @Test
    fun directHttpUrlsWithUserInfoFailClosed() {
        val credentialUrls = listOf(
            "https://user@example.test/path",
            "https://user:secret@example.test/path",
            "http://user:secret@example.test:8080/path",
        )

        credentialUrls.forEach { url ->
            assertFalse(NavigationResolver.isAllowedWebUrl(url))
            assertEquals(NavigationResolver.SEARCH_HOME, NavigationResolver.resolve(url))
        }
    }

    @Test
    fun ordinaryCredentialFreeWebUrlsRemainAllowed() {
        assertTrue(NavigationResolver.isAllowedWebUrl("https://example.test/path"))
        assertTrue(NavigationResolver.isAllowedWebUrl("http://example.test:8080/path"))
    }
}
