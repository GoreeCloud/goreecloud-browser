package io.goreecloud.browser

import android.annotation.TargetApi
import android.app.Activity
import android.content.ClipData
import android.content.ClipboardManager
import android.content.Context
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
import android.webkit.GeolocationPermissions
import android.webkit.PermissionRequest
import android.webkit.SafeBrowsingResponse
import android.webkit.SslErrorHandler
import android.webkit.WebChromeClient
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
 * Installable Android beta shell for GoreeCloud Browser.
 *
 * Android System WebView/Chromium is used only as the web-engine dependency.
 * GoreeCloud owns the mobile chrome, navigation/search policy, permission
 * defaults, download gate, Glaze UI mapping, and product behavior here.
 */
class BrowserActivity : Activity() {
    private lateinit var webView: WebView
    private lateinit var addressField: EditText
    private lateinit var schemeBadge: TextView
    private lateinit var backButton: ImageButton
    private lateinit var forwardButton: ImageButton
    private lateinit var reloadButton: ImageButton
    private lateinit var progressBar: ProgressBar
    private lateinit var glaze: GlazeNativeStyle

    private var currentUrl: String = NavigationResolver.SEARCH_HOME
    private var pageLoading = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        glaze = GlazeNativeStyle(this)
        glaze.applyWindow(this)

        buildBrowserSurface()
        configureEngine()

        if (savedInstanceState == null) {
            navigate(initialUrlFromIntent(intent))
        } else if (webView.restoreState(savedInstanceState) == null) {
            navigate(initialUrlFromIntent(intent))
        } else {
            currentUrl = webView.url ?: NavigationResolver.SEARCH_HOME
            updateOmniboxPresentation()
            updateNavigationControls()
        }
    }

    override fun onNewIntent(intent: Intent) {
        super.onNewIntent(intent)
        setIntent(intent)
        navigate(initialUrlFromIntent(intent))
    }

    override fun onSaveInstanceState(outState: Bundle) {
        webView.saveState(outState)
        super.onSaveInstanceState(outState)
    }

    @Deprecated("Android framework back dispatch remains supported for the API 26 beta floor")
    override fun onBackPressed() {
        if (::addressField.isInitialized && addressField.hasFocus()) {
            addressField.clearFocus()
            hideKeyboard()
            updateOmniboxPresentation()
            return
        }

        if (::webView.isInitialized && webView.canGoBack()) {
            webView.goBack()
        } else {
            super.onBackPressed()
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
        val root = LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
        }
        glaze.styleCanvas(root)

        val topChrome = LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
        }
        glaze.styleTopChrome(topChrome)

        val omnibox = LinearLayout(this).apply {
            orientation = LinearLayout.HORIZONTAL
        }
        glaze.styleOmniboxCapsule(omnibox)

        schemeBadge = TextView(this).apply {
            text = "HTTPS"
            contentDescription = "Address scheme HTTPS"
        }
        glaze.styleSchemeBadge(schemeBadge)
        omnibox.addView(
            schemeBadge,
            LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.WRAP_CONTENT,
                dp(GlazeContract.GENERAL_TARGET_DP),
            ),
        )

        addressField = EditText(this).apply {
            hint = "Search GoreeCloud or enter address"
            contentDescription = "Unified search and address bar"
            isSingleLine = true
            imeOptions = EditorInfo.IME_ACTION_GO
            inputType = android.text.InputType.TYPE_CLASS_TEXT or
                android.text.InputType.TYPE_TEXT_VARIATION_URI
            setOnFocusChangeListener { _, hasFocus ->
                if (hasFocus) {
                    setText(currentUrl)
                    selectAll()
                } else {
                    updateOmniboxPresentation()
                }
            }
            setOnEditorActionListener { _, actionId, event ->
                val go = actionId == EditorInfo.IME_ACTION_GO ||
                    (event?.keyCode == KeyEvent.KEYCODE_ENTER &&
                        event.action == KeyEvent.ACTION_DOWN)
                if (go) {
                    navigate(text.toString())
                    true
                } else {
                    false
                }
            }
        }
        glaze.styleAddressField(addressField)
        omnibox.addView(
            addressField,
            LinearLayout.LayoutParams(
                0,
                LinearLayout.LayoutParams.MATCH_PARENT,
                1f,
            ).apply {
                marginStart = dp(2)
            },
        )

        val goButton = ImageButton(this).apply {
            setImageResource(R.drawable.ic_go)
            contentDescription = "Navigate"
            setOnClickListener { navigate(addressField.text.toString()) }
        }
        glaze.styleChromeButton(goButton, GlazeContract.ButtonRole.Emphasis)
        omnibox.addView(
            goButton,
            LinearLayout.LayoutParams(
                dp(GlazeContract.GENERAL_TARGET_DP),
                dp(GlazeContract.GENERAL_TARGET_DP),
            ).apply {
                marginStart = dp(2)
            },
        )

        topChrome.addView(
            omnibox,
            LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                dp(GlazeContract.OMNIBOX_HEIGHT_DP),
            ),
        )
        root.addView(
            topChrome,
            LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                LinearLayout.LayoutParams.WRAP_CONTENT,
            ),
        )

        val contentFrame = FrameLayout(this)
        webView = WebView(this)
        glaze.styleWebContent(webView)
        contentFrame.addView(
            webView,
            FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.MATCH_PARENT,
            ),
        )

        progressBar = ProgressBar(this, null, android.R.attr.progressBarStyleHorizontal).apply {
            max = 100
            progress = 0
            visibility = View.GONE
            contentDescription = "Page loading progress"
            importantForAccessibility = View.IMPORTANT_FOR_ACCESSIBILITY_NO
        }
        glaze.styleProgress(progressBar)
        contentFrame.addView(
            progressBar,
            FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                dp(GlazeContract.PROGRESS_HEIGHT_DP),
                Gravity.TOP,
            ),
        )

        root.addView(
            contentFrame,
            LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                0,
                1f,
            ),
        )

        val bottomToolbar = LinearLayout(this).apply {
            orientation = LinearLayout.HORIZONTAL
        }
        glaze.styleBottomToolbar(bottomToolbar)

        backButton = chromeButton(R.drawable.ic_back, "Back") {
            if (webView.canGoBack()) webView.goBack()
        }
        bottomToolbar.addView(backButton, toolbarButtonParams())

        forwardButton = chromeButton(R.drawable.ic_forward, "Forward") {
            if (webView.canGoForward()) webView.goForward()
        }
        bottomToolbar.addView(forwardButton, toolbarButtonParams())

        val homeButton = chromeButton(R.drawable.ic_home, "GoreeCloud Search home") {
            navigate(NavigationResolver.SEARCH_HOME)
        }
        bottomToolbar.addView(homeButton, toolbarButtonParams())

        reloadButton = chromeButton(R.drawable.ic_reload, "Reload") {
            if (pageLoading) {
                webView.stopLoading()
                pageLoading = false
                progressBar.visibility = View.GONE
                updateNavigationControls()
            } else {
                webView.reload()
            }
        }
        bottomToolbar.addView(reloadButton, toolbarButtonParams())

        val menuButton = chromeButton(R.drawable.ic_more, "Browser menu") {
            showBrowserMenu(it)
        }
        bottomToolbar.addView(menuButton, toolbarButtonParams())

        root.addView(
            bottomToolbar,
            LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                dp(GlazeContract.BOTTOM_TOOLBAR_HEIGHT_DP),
            ),
        )

        setContentView(root)
        updateNavigationControls()
    }

    private fun configureEngine() {
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
            safeBrowsingEnabled = true
            userAgentString = "$userAgentString GoreeCloudBrowser/${BuildConfig.VERSION_NAME} Android"
        }

        CookieManager.getInstance().apply {
            setAcceptCookie(true)
            setAcceptThirdPartyCookies(webView, false)
        }

        webView.webViewClient = object : WebViewClient() {
            override fun shouldOverrideUrlLoading(view: WebView, request: WebResourceRequest): Boolean {
                val target = request.url.toString()
                if (NavigationResolver.isAllowedWebUrl(target)) return false

                if (request.hasGesture()) openExternalUri(request.url)
                return true
            }

            override fun onPageStarted(view: WebView, url: String, favicon: Bitmap?) {
                super.onPageStarted(view, url, favicon)
                currentUrl = url
                pageLoading = true
                progressBar.visibility = View.VISIBLE
                if (!addressField.hasFocus()) updateOmniboxPresentation()
                updateNavigationControls()
            }

            override fun onPageFinished(view: WebView, url: String) {
                super.onPageFinished(view, url)
                currentUrl = url
                pageLoading = false
                progressBar.visibility = View.GONE
                if (!addressField.hasFocus()) updateOmniboxPresentation()
                updateNavigationControls()
            }

            override fun onReceivedSslError(view: WebView, handler: SslErrorHandler, error: SslError) {
                handler.cancel()
                Toast.makeText(
                    this@BrowserActivity,
                    "Blocked: the site certificate could not be verified.",
                    Toast.LENGTH_LONG,
                ).show()
            }

            @TargetApi(Build.VERSION_CODES.O_MR1)
            override fun onSafeBrowsingHit(
                view: WebView,
                request: WebResourceRequest,
                threatType: Int,
                callback: SafeBrowsingResponse,
            ) {
                callback.backToSafety(true)
                Toast.makeText(
                    this@BrowserActivity,
                    "Blocked by Android Safe Browsing.",
                    Toast.LENGTH_LONG,
                ).show()
            }
        }

        webView.webChromeClient = object : WebChromeClient() {
            override fun onProgressChanged(view: WebView, newProgress: Int) {
                progressBar.progress = newProgress
                pageLoading = newProgress < 100
                progressBar.visibility = if (pageLoading) View.VISIBLE else View.GONE
                updateNavigationControls()
            }

            override fun onPermissionRequest(request: PermissionRequest) {
                request.deny()
            }

            override fun onGeolocationPermissionsShowPrompt(
                origin: String,
                callback: GeolocationPermissions.Callback,
            ) {
                callback.invoke(origin, false, false)
            }
        }

        webView.setDownloadListener { _, _, _, _, _ ->
            Toast.makeText(
                this,
                "Downloads are unavailable until the Android Wardveil release gate is integrated.",
                Toast.LENGTH_LONG,
            ).show()
        }
    }

    private fun navigate(input: String) {
        val target = NavigationResolver.resolve(input)
        currentUrl = target
        addressField.clearFocus()
        hideKeyboard()
        updateOmniboxPresentation()
        webView.loadUrl(target)
        webView.requestFocus()
    }

    private fun initialUrlFromIntent(intent: Intent?): String {
        val candidate = intent?.data?.toString().orEmpty()
        return if (NavigationResolver.isAllowedWebUrl(candidate)) {
            candidate
        } else {
            NavigationResolver.SEARCH_HOME
        }
    }

    private fun openExternalUri(uri: Uri) {
        val external = Intent(Intent.ACTION_VIEW, uri)
        if (external.resolveActivity(packageManager) != null) {
            startActivity(external)
        } else {
            Toast.makeText(this, "No app can open this link.", Toast.LENGTH_SHORT).show()
        }
    }

    private fun updateOmniboxPresentation() {
        val uri = runCatching { Uri.parse(currentUrl) }.getOrNull()
        val scheme = uri?.scheme?.lowercase()
        schemeBadge.text = when (scheme) {
            "https" -> "HTTPS"
            "http" -> "HTTP"
            else -> "WEB"
        }
        schemeBadge.contentDescription = "Address scheme ${schemeBadge.text}"

        if (!addressField.hasFocus()) {
            addressField.setText(displayAddress(currentUrl))
            addressField.setSelection(addressField.text.length)
        }
    }

    private fun displayAddress(url: String): String {
        val uri = runCatching { Uri.parse(url) }.getOrNull() ?: return url
        val host = uri.host ?: return url
        val path = uri.encodedPath.orEmpty().takeUnless { it == "/" }.orEmpty()
        val query = uri.encodedQuery?.let { "?$it" }.orEmpty()
        return host + path + query
    }

    private fun updateNavigationControls() {
        setButtonEnabled(backButton, ::webView.isInitialized && webView.canGoBack())
        setButtonEnabled(forwardButton, ::webView.isInitialized && webView.canGoForward())

        if (::reloadButton.isInitialized) {
            reloadButton.setImageResource(if (pageLoading) R.drawable.ic_stop else R.drawable.ic_reload)
            reloadButton.contentDescription = if (pageLoading) "Stop loading" else "Reload"
        }
    }

    private fun setButtonEnabled(button: ImageButton, enabled: Boolean) {
        button.isEnabled = enabled
        button.alpha = if (enabled) 1f else 0.38f
    }

    private fun chromeButton(icon: Int, description: String, action: (View) -> Unit): ImageButton =
        ImageButton(this).apply {
            setImageResource(icon)
            contentDescription = description
            setOnClickListener(action)
            glaze.styleChromeButton(this, GlazeContract.ButtonRole.Quiet)
        }

    private fun toolbarButtonParams() = LinearLayout.LayoutParams(
        0,
        LinearLayout.LayoutParams.MATCH_PARENT,
        1f,
    ).apply {
        marginStart = dp(1)
        marginEnd = dp(1)
    }

    private fun showBrowserMenu(anchor: View) {
        PopupMenu(this, anchor).apply {
            menu.add(0, MENU_HOME, 0, "GoreeCloud Search home")
            menu.add(0, MENU_COPY_ADDRESS, 1, "Copy page address")
            menu.add(0, MENU_SHARE_ADDRESS, 2, "Share page")
            menu.add(0, MENU_BETA_INFO, 3, "About this beta")
            setOnMenuItemClickListener { item ->
                when (item.itemId) {
                    MENU_HOME -> navigate(NavigationResolver.SEARCH_HOME)
                    MENU_COPY_ADDRESS -> copyCurrentAddress()
                    MENU_SHARE_ADDRESS -> shareCurrentAddress()
                    MENU_BETA_INFO -> showBetaInfo()
                    else -> return@setOnMenuItemClickListener false
                }
                true
            }
            show()
        }
    }

    private fun copyCurrentAddress() {
        val clipboard = getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
        clipboard.setPrimaryClip(ClipData.newPlainText("Page address", currentUrl))
        Toast.makeText(this, "Page address copied.", Toast.LENGTH_SHORT).show()
    }

    private fun shareCurrentAddress() {
        val share = Intent(Intent.ACTION_SEND).apply {
            type = "text/plain"
            putExtra(Intent.EXTRA_TEXT, currentUrl)
        }
        startActivity(Intent.createChooser(share, "Share page"))
    }

    private fun showBetaInfo() {
        Toast.makeText(
            this,
            "GoreeCloud Browser ${BuildConfig.VERSION_NAME}. Installable beta; production signing, downloads, site permissions, and full platform acceptance remain pending.",
            Toast.LENGTH_LONG,
        ).show()
    }

    private fun hideKeyboard() {
        val input = getSystemService(Context.INPUT_METHOD_SERVICE) as InputMethodManager
        input.hideSoftInputFromWindow(addressField.windowToken, 0)
    }

    private fun dp(value: Int): Int = glaze.dp(value)

    companion object {
        private const val MENU_HOME = 1
        private const val MENU_COPY_ADDRESS = 2
        private const val MENU_SHARE_ADDRESS = 3
        private const val MENU_BETA_INFO = 4
    }
}
