package io.goreecloud.browser

import android.app.Activity
import android.os.Bundle

class BrowserActivity : Activity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // Development scaffold only. The production Android client will host
        // the GoreeCloud Browser runtime and Glaze UI rather than a generic
        // WebView shell.
    }
}
