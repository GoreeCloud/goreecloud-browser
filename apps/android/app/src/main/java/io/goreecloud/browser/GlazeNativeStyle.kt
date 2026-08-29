package io.goreecloud.browser

import android.app.Activity
import android.content.Context
import android.content.res.Configuration
import android.graphics.Color
import android.graphics.drawable.GradientDrawable
import android.graphics.drawable.StateListDrawable
import android.view.Gravity
import android.view.View
import android.webkit.WebView
import android.widget.Button
import android.widget.EditText
import android.widget.LinearLayout
import android.widget.TextView

/**
 * Effects-free Android-native Glaze UI 2.0 mapping for Browser-owned chrome.
 *
 * Native controls retain Android semantics while Browser maps the current Glaze
 * material hierarchy, target floor, focus/pressed state, spacing and appearance
 * behavior. This deliberately avoids blur/transparency so the beta has a usable
 * accessibility fallback by construction.
 */
class GlazeNativeStyle(private val context: Context) {
    data class Palette(
        val canvas: Int,
        val surface: Int,
        val softGlaze: Int,
        val glaze: Int,
        val deepGlaze: Int,
        val liveGlaze: Int,
        val textPrimary: Int,
        val textSecondary: Int,
        val outline: Int,
    )

    val palette: Palette = buildPalette()

    fun applyWindow(activity: Activity) {
        activity.window.statusBarColor = palette.canvas
        activity.window.navigationBarColor = palette.canvas
    }

    fun styleCanvas(view: View) {
        view.setBackgroundColor(palette.canvas)
    }

    fun styleStatus(view: TextView) {
        view.setTextColor(palette.textSecondary)
        view.textSize = 12f
        view.setPadding(dp(12), dp(8), dp(12), dp(8))
        view.background = material(GlazeContract.MaterialLevel.SoftGlaze, dp(16))
    }

    fun styleNavigationCapsule(view: LinearLayout) {
        view.gravity = Gravity.CENTER_VERTICAL
        view.minimumHeight = dp(GlazeContract.GENERAL_TARGET_DP + 12)
        view.setPadding(dp(6), dp(6), dp(6), dp(6))
        view.background = material(
            GlazeContract.MaterialLevel.Glaze,
            dp(GlazeContract.NAVIGATION_CAPSULE_CORNER_DP),
        )
    }

    fun styleButton(button: Button, role: GlazeContract.ButtonRole) {
        val levels = when (role) {
            GlazeContract.ButtonRole.Quiet ->
                GlazeContract.MaterialLevel.Surface to GlazeContract.MaterialLevel.SoftGlaze
            GlazeContract.ButtonRole.Soft ->
                GlazeContract.MaterialLevel.SoftGlaze to GlazeContract.MaterialLevel.Glaze
            GlazeContract.ButtonRole.Glaze ->
                GlazeContract.MaterialLevel.Glaze to GlazeContract.MaterialLevel.DeepGlaze
            GlazeContract.ButtonRole.Emphasis ->
                GlazeContract.MaterialLevel.DeepGlaze to GlazeContract.MaterialLevel.LiveGlaze
        }

        button.setAllCaps(false)
        button.gravity = Gravity.CENTER
        button.setTextColor(palette.textPrimary)
        button.textSize = 16f
        button.minWidth = dp(GlazeContract.GENERAL_TARGET_DP)
        button.minimumWidth = dp(GlazeContract.GENERAL_TARGET_DP)
        button.minHeight = dp(GlazeContract.GENERAL_TARGET_DP)
        button.minimumHeight = dp(GlazeContract.GENERAL_TARGET_DP)
        button.setPadding(dp(10), 0, dp(10), 0)
        button.background = interactiveBackground(levels.first, levels.second)
    }

    fun styleAddressField(field: EditText) {
        field.setTextColor(palette.textPrimary)
        field.setHintTextColor(palette.textSecondary)
        field.textSize = 16f
        field.minHeight = dp(GlazeContract.GENERAL_TARGET_DP)
        field.minimumHeight = dp(GlazeContract.GENERAL_TARGET_DP)
        field.setPadding(dp(16), 0, dp(16), 0)
        field.background = interactiveBackground(
            GlazeContract.MaterialLevel.Surface,
            GlazeContract.MaterialLevel.SoftGlaze,
            GlazeContract.FIELD_CORNER_DP,
        )
    }

    fun styleWebContent(webView: WebView) {
        webView.setBackgroundColor(palette.canvas)
    }

    private fun interactiveBackground(
        resting: GlazeContract.MaterialLevel,
        active: GlazeContract.MaterialLevel,
        cornerDp: Int = 24,
    ): StateListDrawable = StateListDrawable().apply {
        addState(
            intArrayOf(android.R.attr.state_pressed),
            material(active, dp(cornerDp), focused = true),
        )
        addState(
            intArrayOf(android.R.attr.state_focused),
            material(active, dp(cornerDp), focused = true),
        )
        addState(intArrayOf(), material(resting, dp(cornerDp)))
    }

    private fun material(
        level: GlazeContract.MaterialLevel,
        cornerRadiusPx: Int,
        focused: Boolean = false,
    ): GradientDrawable = GradientDrawable().apply {
        shape = GradientDrawable.RECTANGLE
        cornerRadius = cornerRadiusPx.toFloat()
        setColor(colorFor(level))
        setStroke(dp(if (focused) 2 else 1), palette.outline)
    }

    private fun colorFor(level: GlazeContract.MaterialLevel): Int = when (level) {
        GlazeContract.MaterialLevel.Canvas -> palette.canvas
        GlazeContract.MaterialLevel.Surface -> palette.surface
        GlazeContract.MaterialLevel.SoftGlaze -> palette.softGlaze
        GlazeContract.MaterialLevel.Glaze -> palette.glaze
        GlazeContract.MaterialLevel.DeepGlaze -> palette.deepGlaze
        GlazeContract.MaterialLevel.LiveGlaze -> palette.liveGlaze
    }

    private fun buildPalette(): Palette {
        val night = context.resources.configuration.uiMode and
            Configuration.UI_MODE_NIGHT_MASK == Configuration.UI_MODE_NIGHT_YES

        val fallbackCanvas = if (night) Color.rgb(18, 19, 22) else Color.rgb(248, 249, 252)
        val fallbackText = if (night) Color.WHITE else Color.rgb(27, 29, 33)
        val fallbackSecondary = if (night) Color.rgb(194, 198, 207) else Color.rgb(88, 92, 101)
        val fallbackAccent = if (night) Color.rgb(147, 179, 255) else Color.rgb(45, 96, 220)

        val canvas = themedColor(android.R.attr.colorBackground, fallbackCanvas)
        val text = themedColor(android.R.attr.textColorPrimary, fallbackText)
        val secondary = themedColor(android.R.attr.textColorSecondary, fallbackSecondary)
        val accent = themedColor(android.R.attr.colorAccent, fallbackAccent)

        return Palette(
            canvas = canvas,
            surface = blend(canvas, text, if (night) 0.07f else 0.035f),
            softGlaze = blend(canvas, accent, if (night) 0.12f else 0.07f),
            glaze = blend(canvas, accent, if (night) 0.18f else 0.11f),
            deepGlaze = blend(canvas, accent, if (night) 0.26f else 0.16f),
            liveGlaze = blend(canvas, accent, if (night) 0.36f else 0.24f),
            textPrimary = text,
            textSecondary = secondary,
            outline = blend(text, canvas, 0.70f),
        )
    }

    private fun themedColor(attribute: Int, fallback: Int): Int {
        val values = context.obtainStyledAttributes(intArrayOf(attribute))
        return try {
            values.getColor(0, fallback)
        } finally {
            values.recycle()
        }
    }

    private fun blend(base: Int, overlay: Int, amount: Float): Int {
        val inverse = 1f - amount
        return Color.rgb(
            (Color.red(base) * inverse + Color.red(overlay) * amount).toInt(),
            (Color.green(base) * inverse + Color.green(overlay) * amount).toInt(),
            (Color.blue(base) * inverse + Color.blue(overlay) * amount).toInt(),
        )
    }

    fun dp(value: Int): Int = (value * context.resources.displayMetrics.density).toInt()
}
