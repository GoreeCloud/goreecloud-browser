package io.goreecloud.browser

import java.io.File
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test

class LocalizationSourceContractTest {
    @Test
    fun browserOwnedUiCopyLivesInAndroidResources() {
        val activity = sourceText("src/main/java/io/goreecloud/browser/BrowserActivity.kt")
        val strings = sourceText("src/main/res/values/strings.xml")

        val requiredResources = listOf(
            "browser_name",
            "address_scheme_description",
            "omnibox_hint",
            "omnibox_description",
            "navigate",
            "page_loading_progress",
            "back",
            "forward",
            "search_home",
            "reload",
            "stop_loading",
            "browser_menu",
            "ssl_error_blocked",
            "safe_browsing_blocked",
            "downloads_unavailable",
            "no_app_can_open_link",
            "copy_page_address",
            "share_page",
            "about_beta",
            "current_page",
            "page_address_clip_label",
            "page_address_copied",
            "beta_info",
        )
        requiredResources.forEach { name ->
            assertTrue("Missing Android string resource: $name", strings.contains("name=\"$name\""))
        }

        assertTrue(strings.contains("name=\"scheme_https\" translatable=\"false\""))
        assertTrue(strings.contains("name=\"scheme_http\" translatable=\"false\""))
        assertTrue(strings.contains("name=\"scheme_web\" translatable=\"false\""))

        val formerUiLiterals = listOf(
            "Search GoreeCloud or enter address",
            "Unified search and address bar",
            "Page loading progress",
            "GoreeCloud Search home",
            "Blocked: the site certificate could not be verified.",
            "Blocked by Android Safe Browsing.",
            "Downloads are unavailable until the Android Wardveil release gate is integrated.",
            "No app can open this link.",
            "Copy page address",
            "About this beta",
            "Current page",
            "Page address copied.",
        )
        formerUiLiterals.forEach { literal ->
            assertFalse("BrowserActivity still embeds user-facing copy: $literal", activity.contains("\"$literal\""))
        }

        assertTrue(activity.contains("chromeButton(R.drawable.ic_back, R.string.back)"))
        assertTrue(activity.contains("menuAction(R.string.copy_page_address"))
        assertTrue(activity.contains("getString(R.string.beta_info, BuildConfig.VERSION_NAME)"))
        assertTrue(activity.contains("getString(R.string.address_scheme_description, schemeBadge.text)"))
        assertTrue(
            activity.contains(
                "NavigationResolver.shouldHandOffExternally(target, request.hasGesture())",
            ),
        )
        assertFalse(activity.contains("if (request.hasGesture()) openExternalUri(request.url)"))
    }

    @Test
    fun debugBetaGeneratesPseudolocalesWithoutChangingReleaseContract() {
        val build = sourceText("build.gradle.kts")
        val debugBlock = build.substringAfter("getByName(\"debug\") {")
            .substringBefore("getByName(\"release\") {")
        val releaseBlock = build.substringAfter("getByName(\"release\") {")

        assertTrue(debugBlock.contains("isPseudoLocalesEnabled = true"))
        assertFalse(releaseBlock.contains("isPseudoLocalesEnabled = true"))
        assertTrue(debugBlock.contains("versionNameSuffix = \"+android.16\""))
    }

    private fun sourceText(relativePath: String): String {
        val candidates = listOf(
            File(relativePath),
            File("app/$relativePath"),
        )
        val source = candidates.firstOrNull { it.isFile }
            ?: error("Unable to resolve Android source fixture: $relativePath from ${File(".").absolutePath}")
        return source.readText()
    }
}
