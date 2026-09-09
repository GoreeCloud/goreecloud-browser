package io.goreecloud.browser

internal data class BrowserFailureState(
    val retryUrl: String,
)

/**
 * Browser-owned recreation contract for the local main-frame failure surface.
 *
 * Only the already-allowed retry URL is eligible for Activity instance-state
 * persistence. Remote error details and arbitrary page/title text are never
 * accepted into this state. The visible failure title is reconstructed from a
 * Browser-owned constant after the URL is revalidated.
 */
internal object BrowserFailureStatePolicy {
    const val BUNDLE_RETRY_URL_KEY = "io.goreecloud.browser.failure.retry_url"
    const val PAGE_UNAVAILABLE_TITLE = "Page unavailable"

    fun restorable(retryUrl: String?): BrowserFailureState? {
        val allowedUrl = retryUrl
            ?.takeIf(NavigationResolver::isAllowedWebUrl)
            ?: return null
        return BrowserFailureState(retryUrl = allowedUrl)
    }
}
