package io.goreecloud.browser

import java.net.URI

/**
 * Browser-owned disclosure policy for page addresses leaving GoreeCloud Browser.
 *
 * Navigation and focused omnibox editing retain the exact authoritative URL.
 * Clipboard/share output removes only explicit HTTP(S) userinfo so embedded
 * credentials are not silently disclosed to another application.
 */
object AddressDisclosurePolicy {
    fun forExternalUse(url: String): String {
        val uri = runCatching { URI(url) }.getOrNull() ?: return url
        val scheme = uri.scheme?.lowercase() ?: return url
        if (scheme != "http" && scheme != "https") return url

        val authority = uri.rawAuthority ?: return url
        val userInfoEnd = authority.lastIndexOf('@')
        if (userInfoEnd < 0) return url

        val schemeSeparator = url.indexOf("://")
        if (schemeSeparator < 0) return url

        val authorityStart = schemeSeparator + 3
        val authorityEnd = firstAuthorityTerminator(url, authorityStart)
        if (url.substring(authorityStart, authorityEnd) != authority) return url

        val safeAuthority = authority.substring(userInfoEnd + 1)
        if (safeAuthority.isEmpty()) return url

        return url.substring(0, authorityStart) + safeAuthority + url.substring(authorityEnd)
    }

    private fun firstAuthorityTerminator(url: String, authorityStart: Int): Int {
        val candidates = listOf(
            url.indexOf('/', authorityStart),
            url.indexOf('?', authorityStart),
            url.indexOf('#', authorityStart),
        ).filter { it >= 0 }
        return candidates.minOrNull() ?: url.length
    }
}
