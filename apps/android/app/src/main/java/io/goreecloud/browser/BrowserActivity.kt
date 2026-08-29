package io.goreecloud.browser

import android.annotation.TargetApi
import android.app.Activity
import android.content.Intent
import android.graphics.Bitmap
import android.net.Uri
import android.net.http.SslError
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
import android.widget.Button
import android.widget.EditText
import android.widget.LinearLayout
import android.widget.ProgressBar
import android.widget.TextView
import android.widget.Toast

/**
 * Installable Android beta shell for GoreeCloud Browser.
 *
 * Android System WebView/Chromium is used only as the web-engine dependency.
 * GoreeCloud owns the chrome, navigation/search policy, permission defaults,
 * download gate, Glaze UI native mapping, and product behavior implemented here.
 */
class BrowserActivity : Activity() {
    private lateinit var webView: WebView
    private lateinit var addressField: EditText
    private lateinit var backButton: Button
    private lateinit var forwardButton: Button
    private lateinit var reloadButton: Button
    private lateinit var progressBar: ProgressBar
    private lateinit var glaze: GlazeNativeStyle

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        title = "GoreeCloud Browser Beta"
        glaze = GlazeNativeStyle(this)
        glaze.applyWindow(this)

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
            setPadding(
                dp(GlazeContract.ROOT_GUTTER_DP),
                dp(GlazeContract.ROOT_GUTTER_DP),
                dp(GlazeContract.ROOT_GUTTER_DP),
                dp(GlazeContract.ROOT_GUTTER_DP),
            )
        }
        glaze.styleCanvas(root)

        val status = TextView(this).apply {
            text = "Android Beta • Glaze UI 2.0 mapping candidate • Wardveil download release pending"
            contentDescription = text
        }
        glaze.styleStatus(status)
        root.addView(
            status,
            LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                LinearLayout.LayoutParams.WRAP_CONTENT,
            ).apply {
                bottomMargin = dp(GlazeContract.CONTROL_GAP_DP)
            },
        )

        val navigationCapsule = LinearLayout(this).apply {
            orientation = LinearLayout.HORIZONTAL
        }
        glaze.styleNavigationCapsule(navigationCapsule)

        backButton = Button(this).apply {
            text = "‹"
            contentDescription = "Back"
            isEnabled = false
            setOnClickListener { if (webView.canGoBack()) webView.goBack() }
        }
        glaze.styleButton(backButton, GlazeContract.ButtonRole.Quiet)
        navigationCapsule.addView(backButton, compactButtonParams())

        forwardButton = Button(this).apply {
            text = "›"
            contentDescription = "Forward"
            isEnabled = false
            setOnClickListener { if (webView.canGoForward()) webView.goForward() }
        }
        glaze.styleButton(forwardButton, GlazeContract.ButtonRole.Quiet)
        navigationCapsule.addView(forwardButton, compactButtonParams().withStartMargin())

        reloadButton = Button(this).apply {
            text = "↻"
            contentDescription = "Reload"
            setOnClickListener { webView.reload() }
        }
        glaze.styleButton(reloadButton, GlazeContract.ButtonRole.Soft)
        navigationCapsule.addView(reloadButton, compactButtonParams().withStartMargin())

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
        glaze.styleAddressField(addressField)
        navigationCapsule.addView(
            addressField,
            LinearLayout.LayoutParams(
                0,
                dp(GlazeContract.GENERAL_TARGET_DP),
                1f,
            ).apply {
                marginStart = dp(GlazeContract.CONTROL_GAP_DP)
                marginEnd = dp(GlazeContract.CONTROL_GAP_DP)
            },
        )

        val goButton = Button(this).apply {
            text = "Go"
            contentDescription = "Navigate"
            setOnClickListener { navigate(addressField.text.toString()) }
        }
        glaze.styleButton(goButton, GlazeContract.ButtonRole.Emphasis)
        navigationCapsule.addView(goButton, compactButtonParams(dp(56)))

        root.addView(
            navigationCapsule,
            LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                LinearLayout.LayoutParams.WRAP_CONTENT,
            ),
        )

        progressBar = ProgressBar(this, null, android.R.attr.progressBarStyleHorizontal).apply {
            max = 100
            progress = 0
            visibility = View.GONE
            contentDescription = "Page loading progress"
            importantForAccessibility = View.IMPORTANT_FOR_ACCESSIBILITY_NO
        }
        root.addView(
            progressBar,
            LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                dp(3),
            ).apply {
                topMargin = dp(6)
            },
        )

        webView = WebView(this)
        glaze.styleWebContent(webView)
        root.addView(
            webView,
            LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                0,
                1f,
            ).apply {
                topMargin = dp(6)
            },
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

    private fun compactButtonParams(width: Int = dp(GlazeContract.GENERAL_TARGET_DP)) =
        LinearLayout.LayoutParams(width, dp(GlazeContract.GENERAL_TARGET_DP))

    private fun LinearLayout.LayoutParams.withStartMargin(): LinearLayout.LayoutParams = apply {
        marginStart = dp(GlazeContract.CONTROL_GAP_DP / 2)
    }

    private fun dp(value: Int): Int = glaze.dp(value)
}
