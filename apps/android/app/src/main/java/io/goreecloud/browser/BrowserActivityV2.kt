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
import android.webkit.WebResourceRequest
import android.webkit.WebView
import android.webkit.WebViewClient
import android.widget.EditText
import android.widget.FrameLayout
import android.widget.ImageButton
import android.widget.LinearLayout
import android.widget.ProgressBar
import android.widget.TextView
import android.widget.Toast

/**
 * Second-generation Android shell for GoreeCloud Browser.
 *
 * The browser engine remains Android System WebView/Chromium. GoreeCloud owns
 * the visible browser chrome, start surface, navigation behavior, privacy
 * defaults and Glaze UI presentation.
 */
class BrowserActivityV2 : Activity() {
    private lateinit var glaze: GlazeNativeStyle
    private lateinit var webView: WebView
    private lateinit var addressField: EditText
    private lateinit var schemeBadge: TextView
    private lateinit var pageTitle: TextView
    private lateinit var tabCount: TextView
    private lateinit var backButton: ImageButton
    private lateinit var forwardButton: ImageButton
    private lateinit var reloadButton: ImageButton
    private lateinit var progressBar: ProgressBar

    private var currentUrl: String = INTERNAL_HOME
    private var loading = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        glaze = GlazeNativeStyle(this)
        glaze.applyWindow(this)
        buildBrowserSurface()
        configureWebView()

        if (savedInstanceState != null && webView.restoreState(savedInstanceState) != null) {
            currentUrl = webView.url ?: INTERNAL_HOME
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

        val topChrome = LinearLayout(this).apply { orientation = LinearLayout.VERTICAL }
        glaze.styleTopChrome(topChrome)

        val identityRow = LinearLayout(this).apply {
            orientation = LinearLayout.HORIZONTAL
            gravity = Gravity.CENTER_VERTICAL
            setPadding(dp(8), 0, dp(2), dp(6))
        }

        pageTitle = TextView(this).apply {
            text = "GoreeCloud Browser"
            textSize = 14f
            setTextColor(glaze.palette.textPrimary)
            maxLines = 1
        }
        identityRow.addView(
            pageTitle,
            LinearLayout.LayoutParams(0, dp(GlazeContract.GENERAL_TARGET_DP), 1f).apply {
                gravity = Gravity.CENTER_VERTICAL
            },
        )

        tabCount = TextView(this).apply {
            text = "1 tab"
            gravity = Gravity.CENTER
            textSize = 12f
            setTextColor(glaze.palette.textSecondary)
            minWidth = dp(58)
            setPadding(dp(10), 0, dp(10), 0)
            background = android.graphics.drawable.GradientDrawable().apply {
                cornerRadius = dp(18).toFloat()
                setColor(glaze.palette.surface)
                setStroke(dp(1), glaze.palette.outline)
            }
            contentDescription = "One open tab"
            setOnClickListener {
                Toast.makeText(this@BrowserActivityV2, "Tab switcher is being integrated.", Toast.LENGTH_SHORT).show()
            }
        }
        identityRow.addView(
            tabCount,
            LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, dp(38)).apply {
                gravity = Gravity.CENTER_VERTICAL
                marginEnd = dp(6)
            },
        )

        val menuButton = chromeButton(R.drawable.ic_more, "Browser menu") {
            showSimpleMenu()
        }
        identityRow.addView(
            menuButton,
            LinearLayout.LayoutParams(dp(GlazeContract.GENERAL_TARGET_DP), dp(GlazeContract.GENERAL_TARGET_DP)),
        )
        topChrome.addView(identityRow)

        val omnibox = LinearLayout(this).apply { orientation = LinearLayout.HORIZONTAL }
        glaze.styleOmniboxCapsule(omnibox)

        schemeBadge = TextView(this).apply {
            text = "START"
            contentDescription = "Browser start page"
        }
        glaze.styleSchemeBadge(schemeBadge)
        omnibox.addView(
            schemeBadge,
            LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, dp(GlazeContract.GENERAL_TARGET_DP)),
        )

        addressField = EditText(this).apply {
            hint = "Search or enter address"
            contentDescription = "Search or address bar"
            isSingleLine = true
            imeOptions = EditorInfo.IME_ACTION_GO
            inputType = android.text.InputType.TYPE_CLASS_TEXT or android.text.InputType.TYPE_TEXT_VARIATION_URI
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
            LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.MATCH_PARENT, 1f).apply {
                marginStart = dp(2)
            },
        )

        val goButton = chromeButton(R.drawable.ic_go, "Go") { navigate(addressField.text.toString()) }
        omnibox.addView(
            goButton,
            LinearLayout.LayoutParams(dp(GlazeContract.GENERAL_TARGET_DP), dp(GlazeContract.GENERAL_TARGET_DP)),
        )
        topChrome.addView(
            omnibox,
            LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, dp(GlazeContract.OMNIBOX_HEIGHT_DP)),
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
            FrameLayout.LayoutParams(FrameLayout.LayoutParams.MATCH_PARENT, dp(GlazeContract.PROGRESS_HEIGHT_DP), Gravity.TOP),
        )
        root.addView(
            content,
            LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, 0, 1f),
        )

        val bottom = LinearLayout(this).apply { orientation = LinearLayout.HORIZONTAL }
        glaze.styleBottomToolbar(bottom)

        backButton = chromeButton(R.drawable.ic_back, "Back") { if (webView.canGoBack()) webView.goBack() }
        forwardButton = chromeButton(R.drawable.ic_forward, "Forward") { if (webView.canGoForward()) webView.goForward() }
        val homeButton = chromeButton(R.drawable.ic_home, "Start page") { showStartPage() }
        reloadButton = chromeButton(R.drawable.ic_reload, "Reload") {
            if (loading) webView.stopLoading() else if (currentUrl == INTERNAL_HOME) showStartPage() else webView.reload()
        }

        listOf(backButton, forwardButton, homeButton, reloadButton).forEach {
            bottom.addView(it, LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.MATCH_PARENT, 1f))
        }
        root.addView(
            bottom,
            LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, dp(GlazeContract.BOTTOM_TOOLBAR_HEIGHT_DP)),
        )

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
                return if (NavigationResolver.isAllowedWebUrl(request.url.toString())) {
                    false
                } else {
                    true
                }
            }

            override fun onPageStarted(view: WebView, url: String, favicon: Bitmap?) {
                if (!isInternalStartUrl(url)) currentUrl = url
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

            override fun onReceivedSslError(view: WebView, handler: SslErrorHandler, error: SslError) {
                handler.cancel()
                Toast.makeText(this@BrowserActivityV2, "Blocked: site certificate could not be verified.", Toast.LENGTH_LONG).show()
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

        currentUrl = target
        addressField.clearFocus()
        hideKeyboard()
        webView.loadUrl(target)
        webView.requestFocus()
        refreshChrome()
    }

    private fun showStartPage() {
        currentUrl = INTERNAL_HOME
        addressField.clearFocus()
        hideKeyboard()
        webView.loadDataWithBaseURL(START_BASE_URL, startHtml(), "text/html", "UTF-8", null)
        refreshChrome()
    }

    private fun showSearchUnavailable(query: String) {
        currentUrl = INTERNAL_HOME
        val escaped = android.text.TextUtils.htmlEncode(query)
        val html = """
            <!doctype html><html><head><meta name="viewport" content="width=device-width,initial-scale=1">
            <style>${baseCss()}</style></head><body><main>
            <div class="mark">G</div><h1>Search is being connected</h1>
            <p>GoreeCloud Search did not return a usable endpoint, so Browser stopped instead of showing the broken proxy page.</p>
            <p class="query">$escaped</p><p>Enter a website address above to browse directly.</p>
            </main></body></html>
        """.trimIndent()
        webView.loadDataWithBaseURL(START_BASE_URL, html, "text/html", "UTF-8", null)
        refreshChrome()
    }

    private fun startHtml(): String = """
        <!doctype html><html><head><meta name="viewport" content="width=device-width,initial-scale=1">
        <style>${baseCss()}</style></head><body><main>
        <div class="mark">G</div>
        <h1>GoreeCloud Browser</h1>
        <p>Browse directly from the address bar. Third-party cookies are disabled and site permissions are denied by default in this Development build.</p>
        <section><strong>Private by default</strong><span>Clear browser chrome, local start page, no advertising surface.</span></section>
        <section><strong>GoreeCloud Search</strong><span>The search service is temporarily held behind a local status page until its endpoint is healthy.</span></section>
        </main></body></html>
    """.trimIndent()

    private fun baseCss(): String = """
        :root{color-scheme:light dark}*{box-sizing:border-box}body{margin:0;font-family:system-ui,-apple-system,sans-serif;background:#f7f9fd;color:#172033}
        main{min-height:100vh;padding:64px 28px 48px;max-width:720px;margin:auto}.mark{width:64px;height:64px;border-radius:22px;display:grid;place-items:center;background:linear-gradient(145deg,#3b82f6,#174ea6);color:white;font-weight:800;font-size:28px;box-shadow:0 12px 34px #174ea633}
        h1{font-size:32px;margin:24px 0 10px}p{font-size:17px;line-height:1.55;color:#5b6577}section{margin-top:18px;padding:18px;border:1px solid #dce3ef;border-radius:22px;background:#ffffffcc}section strong,section span{display:block}section span{margin-top:6px;color:#687386;line-height:1.45}.query{font-weight:700;color:#174ea6}
        @media(prefers-color-scheme:dark){body{background:#0b0f16;color:#f4f7fb}p,section span{color:#aeb8c7}section{background:#111821;border-color:#253043}}
    """.trimIndent()

    private fun refreshChrome() {
        if (!::addressField.isInitialized) return
        val uri = runCatching { Uri.parse(currentUrl) }.getOrNull()
        schemeBadge.text = when {
            currentUrl == INTERNAL_HOME -> "START"
            uri?.scheme.equals("https", true) -> "HTTPS"
            uri?.scheme.equals("http", true) -> "HTTP"
            else -> "WEB"
        }
        pageTitle.text = when {
            currentUrl == INTERNAL_HOME -> "GoreeCloud Browser"
            webView.title.isNullOrBlank() -> uri?.host ?: "GoreeCloud Browser"
            else -> webView.title
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
        button.alpha = if (enabled) 1f else 0.36f
    }

    private fun showSimpleMenu() {
        Toast.makeText(
            this,
            "GoreeCloud Browser • Development\nTabs, history and download integration are being expanded.",
            Toast.LENGTH_LONG,
        ).show()
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
