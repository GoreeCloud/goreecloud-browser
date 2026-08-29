package io.goreecloud.browser

import android.annotation.TargetApi
import android.app.Activity
import android.content.Intent
import android.graphics.Bitmap
import android.net.Uri
import android.os.Build
import android.os.Bundle
import android.view.KeyEvent
import android.view.View
import android.view.inputmethod.EditorInfo
import android.webkit.CookieManager
import android.webkit.GeolocationPermissions
import android.webkit.PermissionRequest
import android.webkit.SafeBrowsingResponse
import android.webkit.SslErrorHandler
import android.webkit.WebChromeClient
import android.webkit.WebResourceRequest
import android.webkit.WebView
import android.webkit.WebViewClient
import android.net.http.SslError
import android.widget.Button
import android.widget.EditText
import android.widget.LinearLayout
import android.widget.ProgressBar
import android.widget.TextView
import android.widget.Toast

/**
 * First installable Android beta shell for GoreeCloud Browser.
 *
 * Android System WebView/Chromium is used only as the web-engine dependency.
 * GoreeCloud owns the chrome, navigation/search policy, permission defaults,
 * download gate, and product behavior implemented here.
 */
class BrowserActivity : Activity() {
    private lateinit var webView: WebView
    private lateinit var addressField: EditText
    private lateinit var backButton: Button
    private lateinit var forwardButton: Button
    private lateinit var reloadButton: Button
    private lateinit var progressBar: ProgressBar

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        title = "GoreeCloud Browser Beta"

        buildBrowserSurface()
        configureEngine()

        if (savedInstanceState == null) {
            navigate(initialUrlFromIntent(intent))
        } else if (webView.restoreState(savedInstanceState) == null) {
            navigate(initialUrlFromIntent(intent))
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
        if (webView.canGoBack()) {
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
            setPadding(dp(8), dp(8), dp(8), dp(8))
        }

        val status = TextView(this).apply {
            text = "Android Beta 0.1 • GoreeCloud Search • downloads security-gated"
            contentDescription = "GoreeCloud Browser Android beta status"
            setPadding(dp(4), 0, dp(4), dp(6))
        }
        root.addView(
            status,
            LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                LinearLayout.LayoutParams.WRAP_CONTENT,
            ),
        )

        val toolbar = LinearLayout(this).apply {
            orientation = LinearLayout.HORIZONTAL
            gravity = android.view.Gravity.CENTER_VERTICAL
        }

        backButton = Button(this).apply {
            text = "‹"
            contentDescription = "Back"
            isEnabled = false
            setOnClickListener { if (webView.canGoBack()) webView.goBack() }
        }
        toolbar.addView(backButton, compactButtonParams())

        forwardButton = Button(this).apply {
            text = "›"
            contentDescription = "Forward"
            isEnabled = false
            setOnClickListener { if (webView.canGoForward()) webView.goForward() }
        }
        toolbar.addView(forwardButton, compactButtonParams())

        reloadButton = Button(this).apply {
            text = "↻"
            contentDescription = "Reload"
            setOnClickListener { webView.reload() }
        }
        toolbar.addView(reloadButton, compactButtonParams())

        addressField = EditText(this).apply {
            hint = "Search GoreeCloud or enter address"
            contentDescription = "Unified search and address bar"
            isSingleLine = true
            imeOptions = EditorInfo.IME_ACTION_GO
            inputType = android.text.InputType.TYPE_CLASS_TEXT or
                android.text.InputType.TYPE_TEXT_VARIATION_URI
            setSelectAllOnFocus(true)
            setOnEditorActionListener { _, actionId, event ->
                val go = actionId == EditorInfo.IME_ACTION_GO ||
                    (event?.keyCode == KeyEvent.KEYCODE_ENTER &&
                        event.action == KeyEvent.ACTION_DOWN)
                if (go) {
                    navigate(addressField.text.toString())
                    true
                } else {
                    false
                }
            }
        }
        toolbar.addView(
            addressField,
            LinearLayout.LayoutParams(
                0,
                LinearLayout.LayoutParams.WRAP_CONTENT,
                1f,
            ).apply {
                marginStart = dp(6)
                marginEnd = dp(6)
            },
        )

        val goButton = Button(this).apply {
            text = "Go"
            contentDescription = "Navigate"
            setOnClickListener { navigate(addressField.text.toString()) }
        }
        toolbar.addView(goButton, compactButtonParams(dp(60)))

        root.addView(
            toolbar,
            LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                LinearLayout.LayoutParams.WRAP_CONTENT,
            ),
        )

        progressBar = ProgressBar(this, null, android.R.attr.progressBarStyleHorizontal).apply {
            max = 100
            progress = 0
            visibility = View.GONE
        }
        root.addView(
            progressBar,
            LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                dp(3),
            ),
        )

        webView = WebView(this)
        root.addView(
            webView,
            LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                0,
                1f,
            ),
        )

        setContentView(root)
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
                addressField.setText(url)
                progressBar.visibility = View.VISIBLE
                updateNavigationControls()
            }

            override fun onPageFinished(view: WebView, url: String) {
                super.onPageFinished(view, url)
                addressField.setText(url)
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
                progressBar.visibility = if (newProgress >= 100) View.GONE else View.VISIBLE
                updateNavigationControls()
            }

            override fun onPermissionRequest(request: PermissionRequest) {
                // The beta fails closed until Browser-owned permission surfaces
                // and Wardveil/Privacy Shield policy adapters are integrated.
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
            // Do not create an Android path that bypasses the existing
            // Browser-to-Wardveil download release contract.
            Toast.makeText(
                this,
                "Downloads are blocked in this beta until Wardveil verification is integrated.",
                Toast.LENGTH_LONG,
            ).show()
        }
    }

    private fun navigate(input: String) {
        val target = NavigationResolver.resolve(input)
        addressField.setText(target)
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

    private fun updateNavigationControls() {
        backButton.isEnabled = webView.canGoBack()
        forwardButton.isEnabled = webView.canGoForward()
    }

    private fun compactButtonParams(width: Int = dp(52)) = LinearLayout.LayoutParams(
        width,
        LinearLayout.LayoutParams.WRAP_CONTENT,
    )

    private fun dp(value: Int): Int =
        (value * resources.displayMetrics.density).toInt()
}
