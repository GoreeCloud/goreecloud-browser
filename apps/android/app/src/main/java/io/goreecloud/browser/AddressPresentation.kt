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
        val safeFallback = withoutBidiControls(url)
        val uri = runCatching { URI(url) }.getOrNull() ?: return safeFallback
        val host = uri.host ?: return safeFallback
        val path = uri.rawPath.orEmpty().takeUnless { it == "/" }.orEmpty()
        val query = uri.rawQuery?.let { "?$it" }.orEmpty()
        return withoutBidiControls(host + path + query)
    }

    /**
     * Remove only Unicode bidi-formatting controls from presentation text.
     * Ordinary RTL letters remain intact. Navigation always keeps the original
     * URL; this helper is strictly for the unfocused Browser-owned display.
     */
    internal fun withoutBidiControls(value: String): String = buildString(value.length) {
        value.forEach { character ->
            if (character !in BIDI_CONTROL_CHARACTERS) append(character)
        }
    }

    private val BIDI_CONTROL_CHARACTERS = setOf(
        '\u061C', // Arabic Letter Mark
        '\u200E', // Left-to-Right Mark
        '\u200F', // Right-to-Left Mark
        '\u202A', // Left-to-Right Embedding
        '\u202B', // Right-to-Left Embedding
        '\u202C', // Pop Directional Formatting
        '\u202D', // Left-to-Right Override
        '\u202E', // Right-to-Left Override
        '\u2066', // Left-to-Right Isolate
        '\u2067', // Right-to-Left Isolate
        '\u2068', // First Strong Isolate
        '\u2069', // Pop Directional Isolate
    )
}
