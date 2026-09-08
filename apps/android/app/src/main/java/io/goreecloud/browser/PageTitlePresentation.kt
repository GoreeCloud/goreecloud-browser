package io.goreecloud.browser

import java.net.URI

/**
 * Keeps remote page-controlled title text inside a bounded Browser chrome
 * presentation contract.
 *
 * A page title is display metadata only. It must not be allowed to present raw
 * URL/resource-path text or Unicode directionality controls as Browser identity.
 */
object PageTitlePresentation {
    private const val FALLBACK = "GoreeCloud Browser"
    private const val MAX_TITLE_LENGTH = 120

    private val bidiFormattingControls = Regex("[\\u061C\\u200E\\u200F\\u202A-\\u202E\\u2066-\\u2069]")
    private val controlCharacters = Regex("[\\u0000-\\u001F\\u007F]")
    private val repeatedWhitespace = Regex("\\s+")

    fun display(rawTitle: String?, currentUrl: String): String {
        val fallback = fallbackFor(currentUrl)
        val cleaned = rawTitle
            ?.replace(bidiFormattingControls, "")
            ?.replace(controlCharacters, " ")
            ?.replace(repeatedWhitespace, " ")
            ?.trim()
            .orEmpty()

        if (cleaned.isBlank()) return fallback
        if (looksLikeRawNavigationText(cleaned)) return fallback

        return cleaned.take(MAX_TITLE_LENGTH)
    }

    private fun fallbackFor(currentUrl: String): String {
        val uri = runCatching { URI(currentUrl) }.getOrNull() ?: return FALLBACK
        if (uri.scheme?.lowercase() !in setOf("http", "https")) return FALLBACK
        return uri.host?.takeIf { it.isNotBlank() } ?: FALLBACK
    }

    private fun looksLikeRawNavigationText(value: String): Boolean {
        if (value.startsWith("/")) return true
        if (value.startsWith("http://", ignoreCase = true)) return true
        if (value.startsWith("https://", ignoreCase = true)) return true
        if (value.startsWith("file:", ignoreCase = true)) return true
        if (value.startsWith("content:", ignoreCase = true)) return true
        if (value.startsWith("data:", ignoreCase = true)) return true
        return false
    }
}
