package io.goreecloud.browser

import java.net.URI
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

        if (hasHttpScheme(input)) {
            return if (isAllowedWebUrl(input)) input else SEARCH_HOME
        }
        if (looksLikeHost(input)) return "https://$input"

        return SEARCH_ENDPOINT + encodeQuery(input)
    }

    fun isAllowedWebUrl(url: String): Boolean {
        if (!hasHttpScheme(url)) return false

        val uri = runCatching { URI(url) }.getOrNull() ?: return false
        val scheme = uri.scheme?.lowercase() ?: return false
        if (scheme != "https" && scheme != "http") return false
        if (uri.rawUserInfo != null) return false

        val authority = uri.rawAuthority ?: return false
        return authorityHost(authority) != null
    }

    private fun hasHttpScheme(value: String): Boolean {
        return value.startsWith("https://", ignoreCase = true) ||
            value.startsWith("http://", ignoreCase = true)
    }

    private fun looksLikeHost(value: String): Boolean {
        if (value.any(Char::isWhitespace)) return false
        if (value.contains("://")) return false

        val authority = value.substringBefore('/').substringBefore('?').substringBefore('#')
        val hostPart = authorityHost(authority) ?: return false

        return hostPart.equals("localhost", ignoreCase = true) ||
            hostPart.contains('.') ||
            IPV4_REGEX.matches(hostPart) ||
            (hostPart.startsWith('[') && hostPart.endsWith(']'))
    }

    /**
     * Extract the host-like portion of a URI authority while validating only the
     * optional port syntax/range needed by Browser routing. Host normalization,
     * DNS resolution, IDN/confusable policy, and trust remain separate gates.
     *
     * This helper can still parse user-info-shaped host text for scheme-less host
     * classification; fully qualified HTTP(S) URLs are rejected earlier when URI
     * user-info is present and therefore cannot use embedded credentials.
     */
    private fun authorityHost(authority: String): String? {
        val hostPort = authority.substringAfterLast('@')
        if (hostPort.isEmpty()) return null

        if (hostPort.startsWith('[')) {
            val closingBracket = hostPort.indexOf(']')
            if (closingBracket <= 1) return null

            val host = hostPort.substring(0, closingBracket + 1)
            val suffix = hostPort.substring(closingBracket + 1)
            if (!validPortSuffix(suffix)) return null
            return host
        }

        val colon = hostPort.lastIndexOf(':')
        if (colon < 0) return hostPort
        if (hostPort.indexOf(':') != colon) return null

        val host = hostPort.substring(0, colon)
        if (host.isEmpty()) return null
        if (!validPortSuffix(hostPort.substring(colon))) return null
        return host
    }

    private fun validPortSuffix(suffix: String): Boolean {
        if (suffix.isEmpty()) return true
        if (!suffix.startsWith(':')) return false

        val portText = suffix.substring(1)
        if (portText.isEmpty() || portText.any { !it.isDigit() }) return false
        val port = portText.toIntOrNull() ?: return false
        return port in 0..65535
    }

    private fun encodeQuery(value: String): String =
        URLEncoder.encode(value, StandardCharsets.UTF_8.name()).replace("+", "%20")

    private val IPV4_REGEX = Regex("""^(?:\d{1,3}\.){3}\d{1,3}$""")
}
