package io.goreecloud.browser

import java.net.URI

/**
 * Presentation-only address helpers for Browser-owned chrome.
 *
 * The full URL remains the navigation authority. This object only determines
 * what an unfocused omnibox displays and deliberately keeps the host visible
 * from the leading edge instead of scrolling to the end of a long path.
 */
object AddressPresentation {
    fun condensed(url: String): String {
        val uri = runCatching { URI(url) }.getOrNull() ?: return url
        val host = uri.host ?: return url
        val path = uri.rawPath.orEmpty().takeUnless { it == "/" }.orEmpty()
        val query = uri.rawQuery?.let { "?$it" }.orEmpty()
        return host + path + query
    }
}
