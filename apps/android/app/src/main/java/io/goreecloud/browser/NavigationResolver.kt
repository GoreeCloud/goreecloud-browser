package io.goreecloud.browser

import java.net.URLEncoder
import java.nio.charset.StandardCharsets

/**
 * GoreeCloud-owned resolution policy for the Android unified address/search bar.
 * Direct URLs remain independent from search. Non-URL input is sent only to
 * GoreeCloud Search; no alternate provider or silent fallback is permitted.
 */
object NavigationResolver {
    const val SEARCH_HOME = "https://search.goreecloud.com/"
    private const val SEARCH_ENDPOINT = "https://search.goreecloud.com/search?q="

    fun resolve(rawInput: String): String {
        val input = rawInput.trim()
        if (input.isEmpty()) return SEARCH_HOME

        if (hasHttpScheme(input)) return input
        if (looksLikeHost(input)) return "https://$input"

        return SEARCH_ENDPOINT + encodeQuery(input)
    }

    fun isAllowedWebUrl(url: String): Boolean = hasHttpScheme(url)

    private fun hasHttpScheme(value: String): Boolean {
        return value.startsWith("https://", ignoreCase = true) ||
            value.startsWith("http://", ignoreCase = true)
    }

    private fun looksLikeHost(value: String): Boolean {
        if (value.any(Char::isWhitespace)) return false
        if (value.contains("://")) return false

        val hostPart = value.substringBefore('/').substringBefore('?').substringBefore('#')
        if (hostPart.isEmpty()) return false

        return hostPart.equals("localhost", ignoreCase = true) ||
            hostPart.contains('.') ||
            IPV4_REGEX.matches(hostPart) ||
            (hostPart.startsWith('[') && hostPart.endsWith(']'))
    }

    private fun encodeQuery(value: String): String =
        URLEncoder.encode(value, StandardCharsets.UTF_8.name()).replace("+", "%20")

    private val IPV4_REGEX = Regex("""^(?:\d{1,3}\.){3}\d{1,3}$""")
}
