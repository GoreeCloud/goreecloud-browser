package io.goreecloud.browser

import android.app.Activity
import android.content.Intent
import android.graphics.Bitmap
import android.net.Uri
import android.net.http.SslError
import android.os.Build
import android.os.Bundle
import android.view.Gravity
import android.view.KeyEvent
import android.view.View
import android.view.inputmethod.EditorInfo
import android.view.inputmethod.InputMethodManager
import android.webkit.CookieManager
import android.webkit.PermissionRequest
import android.webkit.SslErrorHandler
import android.webkit.WebChromeClient
import android.webkit.WebResourceError
import android.webkit.WebResourceRequest
import android.webkit.WebView
import android.webkit.WebViewClient
import android.widget.EditText
import android.widget.FrameLayout
import android.widget.ImageButton
import android.widget.LinearLayout
import android.widget.PopupMenu
import android.widget.ProgressBar
import android.widget.TextView
import android.widget.Toast

/**
 * GoreeCloud Browser Android shell.
 *
 * Chromium/WebView remains the engine. GoreeCloud owns the visible browser
 * chrome, local start surface, navigation behavior, privacy defaults, and
 * Glaze UI presentation.
 */
class BrowserActivityV2 : Activity() {
    private lateinit var glaze: GlazeNativeStyle
    private lateinit var webView: WebView
    private lateinit var addressField: EditText
    private lateinit var pageTitle: TextView
    private lateinit var backButton: ImageButton
    private lateinit var forwardButton: ImageButton
    private lateinit var reloadButton: ImageButton
    private lateinit var progressBar: ProgressBar

    private var currentUrl: String = INTERNAL_HOME
    private var loading = false
    private var failedMainFrameUrl: String? = null
    private var chromeOverrideTitle: String? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        glaze = GlazeNativeStyle(this)
        glaze.applyWindow(this)
        buildBrowserSurface()
        configureWebView()

        if (savedInstanceState != null && webView.restoreState(savedInstanceState) != null) {
            currentUrl = webView.url ?: INTERNAL_HOME
            BrowserFailureStatePolicy.restorable(
                savedInstanceState.getString(BrowserFailureStatePolicy.BUNDLE_RETRY_URL_KEY),
            )?.let { restoredFailure ->
                failedMainFrameUrl = restoredFailure.retryUrl
                chromeOverrideTitle = BrowserFailureStatePolicy.PAGE_UNAVAILABLE_TITLE
                currentUrl = restoredFailure.retryUrl
            }
            refreshChrome()
        } else {
            val external = intent?.data?.toString().orEmpty()
            if (NavigationResolver.isAllowedWebUrl(external)) navigate(external) else showStartPage()
        }
    }

    override fun onNewIntent(intent: Intent) {
        super.onNewIntent(intent)
        setIntent(intent)
        val external = intent.data?.toString().orEmpty()
        if (NavigationResolver.isAllowedWebUrl(external)) navigate(external) else showStartPage()
    }

    override fun onSaveInstanceState(outState: Bundle) {
        webView.saveState(outState)
        BrowserFailureStatePolicy.restorable(failedMainFrameUrl)?.let { failureState ->
            outState.putString(
                BrowserFailureStatePolicy.BUNDLE_RETRY_URL_KEY,
                failureState.retryUrl,
            )
        }
        super.onSaveInstanceState(outState)
    }

    @Deprecated("Android framework back dispatch is retained for the current API floor")
    override fun onBackPressed() {
        when {
            addressField.hasFocus() -> {
                addressField.clearFocus()
                hideKeyboard()
                refreshChrome()
            }
            webView.canGoBack() -> webView.goBack()
            else -> super.onBackPressed()
        }
    }

    override fun onDestroy() {
        if (::webView.isInitialized) {
            webView.stopLoading()
            webView.webChromeClient = null
            webView.webViewClient = WebViewClient()
            webView.loadUrl("about:blank")
            webView.removeAllViews()
            webView.destroy()
        }
        super.onDestroy()
    }

    private fun buildBrowserSurface() {
        val root = LinearLayout(this).apply { orientation = LinearLayout.VERTICAL }
        glaze.styleCanvas(root)

        val topChrome = LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
            setPadding(dp(10), dp(8), dp(10), dp(8))
        }
        glaze.styleTopChrome(topChrome)

        val identityRow = LinearLayout(this).apply {
            orientation = LinearLayout.HORIZONTAL
            gravity = Gravity.CENTER_VERTICAL
        }

        pageTitle = TextView(this).apply {
            text = "GoreeCloud Browser"
            textSize = 13f
            setTextColor(glaze.palette.textSecondary)
            maxLines = 1
        }
        identityRow.addView(
            pageTitle,
            LinearLayout.LayoutParams(0, dp(32), 1f).apply { gravity = Gravity.CENTER_VERTICAL },
        )

        val menuButton = chromeButton(R.drawable.ic_more, "Browser menu") { anchor ->
            showBrowserMenu(anchor)
        }
        identityRow.addView(menuButton, LinearLayout.LayoutParams(dp(40), dp(40)))
        topChrome.addView(identityRow)

        val omnibox = LinearLayout(this).apply {
            orientation = LinearLayout.HORIZONTAL
            gravity = Gravity.CENTER_VERTICAL
        }
        glaze.styleOmniboxCapsule(omnibox)

        addressField = EditText(this).apply {
            hint = "Search or enter address"
            contentDescription = "Search or address bar"
            isSingleLine = true
            imeOptions = EditorInfo.IME_ACTION_GO
            inputType = android.text.InputType.TYPE_CLASS_TEXT or android.text.InputType.TYPE_TEXT_VARIATION_URI
            setPadding(dp(16), 0, dp(8), 0)
            setOnFocusChangeListener { _, focused ->
                if (focused) {
                    setText(if (currentUrl == INTERNAL_HOME) "" else currentUrl)
                    selectAll()
                } else {
                    refreshChrome()
                }
            }
            setOnEditorActionListener { _, actionId, event ->
                val go = actionId == EditorInfo.IME_ACTION_GO ||
                    (event?.keyCode == KeyEvent.KEYCODE_ENTER && event.action == KeyEvent.ACTION_DOWN)
                if (go) {
                    navigate(text.toString())
                    true
                } else false
            }
        }
        glaze.styleAddressField(addressField)
        omnibox.addView(
            addressField,
            LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.MATCH_PARENT, 1f),
        )

        val goButton = chromeButton(R.drawable.ic_go, "Go") { navigate(addressField.text.toString()) }
        omnibox.addView(goButton, LinearLayout.LayoutParams(dp(44), dp(44)))
        topChrome.addView(
            omnibox,
            LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, dp(52)),
        )
        root.addView(topChrome)

        val content = FrameLayout(this)
        webView = WebView(this)
        glaze.styleWebContent(webView)
        content.addView(
            webView,
            FrameLayout.LayoutParams(FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT),
        )

        progressBar = ProgressBar(this, null, android.R.attr.progressBarStyleHorizontal).apply {
            max = 100
            visibility = View.GONE
            importantForAccessibility = View.IMPORTANT_FOR_ACCESSIBILITY_NO
        }
        glaze.styleProgress(progressBar)
        content.addView(
            progressBar,
            FrameLayout.LayoutParams(FrameLayout.LayoutParams.MATCH_PARENT, dp(2), Gravity.TOP),
        )
        root.addView(content, LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, 0, 1f))

        val bottom = LinearLayout(this).apply { orientation = LinearLayout.HORIZONTAL }
        glaze.styleBottomToolbar(bottom)

        backButton = chromeButton(R.drawable.ic_back, "Back") { if (webView.canGoBack()) webView.goBack() }
        forwardButton = chromeButton(R.drawable.ic_forward, "Forward") { if (webView.canGoForward()) webView.goForward() }
        val homeButton = chromeButton(R.drawable.ic_home, "Start page") { showStartPage() }
        reloadButton = chromeButton(R.drawable.ic_reload, "Reload") {
            when {
                loading -> webView.stopLoading()
                failedMainFrameUrl != null -> navigate(failedMainFrameUrl.orEmpty())
                currentUrl == INTERNAL_HOME -> showStartPage()
                else -> webView.reload()
            }
        }
        val bottomMenu = chromeButton(R.drawable.ic_more, "Browser menu") { anchor -> showBrowserMenu(anchor) }

        listOf(backButton, forwardButton, homeButton, reloadButton, bottomMenu).forEach {
            bottom.addView(it, LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.MATCH_PARENT, 1f))
        }
        root.addView(bottom, LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, dp(60)))

        setContentView(root)
        updateNavigationButtons()
    }

    private fun configureWebView() {
        WebView.setWebContentsDebuggingEnabled(BuildConfig.DEBUG)
        webView.settings.apply {
            javaScriptEnabled = true
            domStorageEnabled = true
            loadsImagesAutomatically = true
            mediaPlaybackRequiresUserGesture = true
            allowFileAccess = false
            allowContentAccess = false
            builtInZoomControls = true
            displayZoomControls = false
            setSupportZoom(true)
            mixedContentMode = android.webkit.WebSettings.MIXED_CONTENT_NEVER_ALLOW
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) safeBrowsingEnabled = true
            userAgentString = "$userAgentString GoreeCloudBrowser/${BuildConfig.VERSION_NAME} Android"
        }

        CookieManager.getInstance().apply {
            setAcceptCookie(true)
            setAcceptThirdPartyCookies(webView, false)
        }

        webView.webViewClient = object : WebViewClient() {
            override fun shouldOverrideUrlLoading(view: WebView, request: WebResourceRequest): Boolean {
                return !NavigationResolver.isAllowedWebUrl(request.url.toString())
            }

            override fun onPageStarted(view: WebView, url: String, favicon: Bitmap?) {
                if (!isInternalStartUrl(url)) {
                    currentUrl = url
                    failedMainFrameUrl = null
                    chromeOverrideTitle = null
                }
                loading = true
                progressBar.visibility = View.VISIBLE
                refreshChrome()
            }

            override fun onPageFinished(view: WebView, url: String) {
                if (!isInternalStartUrl(url)) currentUrl = url
                loading = false
                progressBar.visibility = View.GONE
                refreshChrome()
            }

            override fun onReceivedError(
                view: WebView,
                request: WebResourceRequest,
                error: WebResourceError,
            ) {
                if (!request.isForMainFrame || isInternalStartUrl(request.url.toString())) return
                showPageUnavailable(request.url.toString())
            }

            override fun onReceivedSslError(view: WebView, handler: SslErrorHandler, error: SslError) {
                handler.cancel()
                showPageUnavailable(error.url)
            }
        }

        webView.webChromeClient = object : WebChromeClient() {
            override fun onProgressChanged(view: WebView, newProgress: Int) {
                progressBar.progress = newProgress
                loading = newProgress < 100
                progressBar.visibility = if (loading) View.VISIBLE else View.GONE
                updateNavigationButtons()
            }

            override fun onPermissionRequest(request: PermissionRequest) {
                request.deny()
            }
        }

        webView.setDownloadListener { _, _, _, _, _ ->
            Toast.makeText(this, "Downloads remain gated until Wardveil download integration is ready.", Toast.LENGTH_LONG).show()
        }
    }

    private fun navigate(raw: String) {
        val input = raw.trim()
        if (input.isEmpty()) {
            showStartPage()
            return
        }

        val target = NavigationResolver.resolve(input)
        if (target.startsWith("https://search.goreecloud.com/")) {
            showSearchUnavailable(input)
            return
        }

        failedMainFrameUrl = null
        chromeOverrideTitle = null
        currentUrl = target
        addressField.clearFocus()
        hideKeyboard()
        webView.loadUrl(target)
        webView.requestFocus()
        refreshChrome()
    }

    private fun showStartPage() {
        failedMainFrameUrl = null
        chromeOverrideTitle = null
        currentUrl = INTERNAL_HOME
        addressField.clearFocus()
        hideKeyboard()
        webView.loadDataWithBaseURL(START_BASE_URL, startHtml(), "text/html", "UTF-8", null)
        refreshChrome()
    }

    private fun showSearchUnavailable(query: String) {
        failedMainFrameUrl = null
        chromeOverrideTitle = null
        currentUrl = INTERNAL_HOME
        val escaped = android.text.TextUtils.htmlEncode(query)
        val html = """
            <!doctype html><html><head><meta name="viewport" content="width=device-width,initial-scale=1">
            <style>${baseCss()}</style></head><body><main>
            <div class="mark">G</div><h1>Search is temporarily unavailable</h1>
            <p>The GoreeCloud Search endpoint is not healthy yet. Browser stopped here instead of exposing the proxy error page.</p>
            <p class="query">$escaped</p><p>You can still enter a complete website address in the address bar.</p>
            </main></body></html>
        """.trimIndent()
        webView.loadDataWithBaseURL(START_BASE_URL, html, "text/html", "UTF-8", null)
        refreshChrome()
    }

    private fun showPageUnavailable(failedUrl: String?) {
        val retryUrl = failedUrl
            ?.takeIf(NavigationResolver::isAllowedWebUrl)
            ?: currentUrl.takeIf(NavigationResolver::isAllowedWebUrl)

        failedMainFrameUrl = retryUrl
        chromeOverrideTitle = BrowserFailureStatePolicy.PAGE_UNAVAILABLE_TITLE
        if (retryUrl != null) currentUrl = retryUrl
        loading = false
        progressBar.visibility = View.GONE
        addressField.clearFocus()
        hideKeyboard()
        webView.loadDataWithBaseURL(
            START_BASE_URL,
            BrowserLocalPages.pageUnavailableHtml(baseCss()),
            "text/html",
            "UTF-8",
            null,
        )
        refreshChrome()
    }

    private fun startHtml(): String = """
        <!doctype html><html><head><meta name="viewport" content="width=device-width,initial-scale=1">
        <style>${baseCss()}</style></head><body><main>
        <div class="mark">G</div>
        <h1>Browse the web</h1>
        <p>Enter a website address above. This Development build blocks third-party cookies and denies site permissions by default.</p>
        <section><strong>GoreeCloud Search</strong><span>Search remains held behind a local status page until the GoreeCloud endpoint is healthy.</span></section>
        </main></body></html>
    """.trimIndent()

    private fun baseCss(): String = """
        :root{color-scheme:light dark}*{box-sizing:border-box}body{margin:0;font-family:system-ui,-apple-system,sans-serif;background:#f5f7fb;color:#172033}
        main{min-height:100vh;padding:54px 24px 40px;max-width:720px;margin:auto}.mark{width:58px;height:58px;border-radius:18px;display:grid;place-items:center;background:linear-gradient(145deg,#3b82f6,#174ea6);color:white;font-weight:800;font-size:25px;box-shadow:0 12px 34px #174ea633}
        h1{font-size:30px;margin:22px 0 10px}p{font-size:16px;line-height:1.55;color:#5b6577}section{margin-top:18px;padding:18px;border:1px solid #dce3ef;border-radius:20px;background:#ffffffcc}section strong,section span{display:block}section span{margin-top:6px;color:#687386;line-height:1.45}.query{font-weight:700;color:#174ea6}
        @media(prefers-color-scheme:dark){body{background:#0b0f16;color:#f4f7fb}p,section span{color:#aeb8c7}section{background:#111821;border-color:#253043}}
    """.trimIndent()

    private fun refreshChrome() {
        if (!::addressField.isInitialized) return
        pageTitle.text = chromeOverrideTitle ?: if (currentUrl == INTERNAL_HOME) {
            "GoreeCloud Browser"
        } else {
            PageTitlePresentation.display(webView.title, currentUrl)
        }
        if (!addressField.hasFocus()) {
            addressField.setText(if (currentUrl == INTERNAL_HOME) "" else AddressPresentation.condensed(currentUrl))
            addressField.setSelection(0)
        }
        updateNavigationButtons()
    }

    private fun updateNavigationButtons() {
        if (::backButton.isInitialized) setEnabled(backButton, webView.canGoBack())
        if (::forwardButton.isInitialized) setEnabled(forwardButton, webView.canGoForward())
        if (::reloadButton.isInitialized) {
            reloadButton.setImageResource(if (loading) R.drawable.ic_stop else R.drawable.ic_reload)
            reloadButton.contentDescription = if (loading) "Stop loading" else "Reload"
        }
    }

    private fun setEnabled(button: ImageButton, enabled: Boolean) {
        button.isEnabled = enabled
        button.alpha = if (enabled) 1f else 0.34f
    }

    private fun showBrowserMenu(anchor: View) {
        PopupMenu(this, anchor).apply {
            menu.add("Copy page address").setOnMenuItemClickListener {
                val externalAddress = AddressDisclosurePolicy.forExternalUse(currentUrl)
                val clipboard = getSystemService(CLIPBOARD_SERVICE) as android.content.ClipboardManager
                clipboard.setPrimaryClip(android.content.ClipData.newPlainText("Page address", externalAddress))
                true
            }
            menu.add("Share page").setOnMenuItemClickListener {
                val externalAddress = AddressDisclosurePolicy.forExternalUse(currentUrl)
                startActivity(Intent.createChooser(Intent(Intent.ACTION_SEND).apply {
                    type = "text/plain"
                    putExtra(Intent.EXTRA_TEXT, externalAddress)
                }, "Share page"))
                true
            }
            menu.add("About this development build").setOnMenuItemClickListener {
                Toast.makeText(this@BrowserActivityV2, "GoreeCloud Browser ${BuildConfig.VERSION_NAME}", Toast.LENGTH_SHORT).show()
                true
            }
            show()
        }
    }

    private fun chromeButton(icon: Int, description: String, action: (View) -> Unit): ImageButton =
        ImageButton(this).apply {
            setImageResource(icon)
            contentDescription = description
            setOnClickListener(action)
            glaze.styleChromeButton(this, GlazeContract.ButtonRole.Quiet)
        }

    private fun hideKeyboard() {
        (getSystemService(INPUT_METHOD_SERVICE) as? InputMethodManager)
            ?.hideSoftInputFromWindow(addressField.windowToken, 0)
    }

    private fun isInternalStartUrl(url: String): Boolean =
        url.startsWith(START_BASE_URL) || url.startsWith("data:text/html")

    private fun dp(value: Int): Int = glaze.dp(value)

    companion object {
        private const val INTERNAL_HOME = "goreecloud://start"
        private const val START_BASE_URL = "https://start.goreecloud.local/"
    }
}
