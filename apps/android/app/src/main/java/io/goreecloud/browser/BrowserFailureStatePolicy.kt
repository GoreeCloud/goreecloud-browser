package io.goreecloud.browser

import java.net.URI

internal data class BrowserFailureState(
    val retryUrl: String,
)

/**
 * Browser-owned recreation contract for the local main-frame failure surface.
 *
 * Only the already-allowed retry URL is eligible for Activity instance-state
 * persistence. Remote error details and arbitrary page/title text are never
 * accepted into this state. Credential-bearing URL user-info is also rejected
 * rather than copied into instance state. The visible failure title is
 * reconstructed from a Browser-owned constant after the URL is revalidated.
 */
internal object BrowserFailureStatePolicy {
    const val BUNDLE_RETRY_URL_KEY = "io.goreecloud.browser.failure.retry_url"
    const val PAGE_UNAVAILABLE_TITLE = "Page unavailable"

    fun restorable(retryUrl: String?): BrowserFailureState? {
        val allowedUrl = retryUrl
            ?.takeIf(NavigationResolver::isAllowedWebUrl)
            ?.takeIf(::hasNoUserInfo)
            ?: return null
        return BrowserFailureState(retryUrl = allowedUrl)
    }

    private fun hasNoUserInfo(url: String): Boolean =
        runCatching { URI(url).rawUserInfo == null }.getOrDefault(false)
}
