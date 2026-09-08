package io.goreecloud.browser

import android.app.Activity
import android.content.Context
import android.content.res.ColorStateList
import android.content.res.Configuration
import android.graphics.Color
import android.graphics.drawable.GradientDrawable
import android.graphics.drawable.StateListDrawable
import android.os.Build
import android.view.Gravity
import android.view.View
import android.webkit.WebView
import android.widget.EditText
import android.widget.ImageButton
import android.widget.ImageView
import android.widget.LinearLayout
import android.widget.ProgressBar
import android.widget.TextView

/**
 * Effects-optional Android-native GLAZE UI V1.2 mapping for Browser-owned chrome.
 *
 * V1.2 rule: neutral glass is the material; color is an accent. Browser chrome
 * therefore uses neutral Frost White / graphite material levels instead of
 * tinting the material substrate with the accent color. Native Android controls
 * retain their semantics, focus behavior, hit targets, and effects-free fallback.
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
        val accent: Int,
    )

    private val night = context.resources.configuration.uiMode and
        Configuration.UI_MODE_NIGHT_MASK == Configuration.UI_MODE_NIGHT_YES

    val palette: Palette = buildPalette()

    fun applyWindow(activity: Activity) {
        activity.window.statusBarColor = palette.canvas
        activity.window.navigationBarColor = if (!night && Build.VERSION.SDK_INT < Build.VERSION_CODES.O_MR1) {
            Color.BLACK
        } else {
            palette.canvas
        }

        var flags = activity.window.decorView.systemUiVisibility
        if (!night) {
            flags = flags or View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O_MR1) {
                flags = flags or View.SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR
            }
        }
        activity.window.decorView.systemUiVisibility = flags
    }

    fun styleCanvas(view: View) {
        view.setBackgroundColor(palette.canvas)
    }

    fun styleTopChrome(view: LinearLayout) {
        view.gravity = Gravity.CENTER_VERTICAL
        view.setPadding(
            dp(GlazeContract.CHROME_GUTTER_DP),
            dp(GlazeContract.CHROME_GUTTER_DP),
            dp(GlazeContract.CHROME_GUTTER_DP),
            dp(GlazeContract.CHROME_GUTTER_DP),
        )
        view.background = material(GlazeContract.MaterialLevel.Canvas, 0, outlined = false)
    }

    fun styleOmniboxCapsule(view: LinearLayout) {
        view.gravity = Gravity.CENTER_VERTICAL
        view.minimumHeight = dp(GlazeContract.OMNIBOX_HEIGHT_DP)
        view.setPadding(dp(4), dp(4), dp(4), dp(4))
        view.background = material(
            GlazeContract.MaterialLevel.SoftGlaze,
            dp(GlazeContract.OMNIBOX_CORNER_DP),
            outlined = true,
        )
    }

    fun styleSchemeBadge(view: TextView) {
        view.gravity = Gravity.CENTER
        view.setTextColor(palette.textSecondary)
        view.textSize = 10f
        view.setAllCaps(true)
        view.minWidth = dp(44)
        view.minimumWidth = dp(44)
        view.setPadding(dp(8), 0, dp(8), 0)
        view.background = material(GlazeContract.MaterialLevel.Surface, dp(18), outlined = false)
    }

    fun styleChromeButton(button: ImageButton, role: GlazeContract.ButtonRole) {
        val levels = when (role) {
            GlazeContract.ButtonRole.Quiet ->
                GlazeContract.MaterialLevel.Canvas to GlazeContract.MaterialLevel.SoftGlaze
            GlazeContract.ButtonRole.Soft ->
                GlazeContract.MaterialLevel.Surface to GlazeContract.MaterialLevel.Glaze
            GlazeContract.ButtonRole.Glaze ->
                GlazeContract.MaterialLevel.Glaze to GlazeContract.MaterialLevel.DeepGlaze
            GlazeContract.ButtonRole.Emphasis ->
                GlazeContract.MaterialLevel.Glaze to GlazeContract.MaterialLevel.DeepGlaze
        }

        button.minimumWidth = dp(GlazeContract.GENERAL_TARGET_DP)
        button.minimumHeight = dp(GlazeContract.GENERAL_TARGET_DP)
        button.setPadding(dp(12), dp(12), dp(12), dp(12))
        button.scaleType = ImageView.ScaleType.CENTER
        button.imageTintList = ColorStateList.valueOf(palette.textPrimary)
        button.background = interactiveBackground(
            levels.first,
            levels.second,
            GlazeContract.CHROME_CONTROL_CORNER_DP,
        )
    }

    fun styleAddressField(field: EditText) {
        field.setTextColor(palette.textPrimary)
        field.setHintTextColor(palette.textSecondary)
        field.textSize = 16f
        field.minHeight = dp(GlazeContract.GENERAL_TARGET_DP)
        field.minimumHeight = dp(GlazeContract.GENERAL_TARGET_DP)
        field.setPadding(dp(10), 0, dp(8), 0)
        field.background = null
        field.setSelectAllOnFocus(false)
    }

    fun styleBottomToolbar(view: LinearLayout) {
        view.gravity = Gravity.CENTER_VERTICAL
        view.setPadding(dp(6), 0, dp(6), 0)
        view.background = material(GlazeContract.MaterialLevel.Surface, 0, outlined = false)
    }

    fun styleMenuSheet(view: LinearLayout) {
        view.orientation = LinearLayout.VERTICAL
        view.setPadding(dp(12), dp(12), dp(12), dp(16))
        view.background = material(
            GlazeContract.MaterialLevel.SoftGlaze,
            dp(GlazeContract.MENU_CORNER_DP),
            outlined = true,
        )
    }

    fun styleMenuTitle(view: TextView) {
        view.setTextColor(palette.textPrimary)
        view.textSize = 20f
        view.setPadding(dp(12), dp(8), dp(12), dp(2))
    }

    fun styleMenuSubtitle(view: TextView) {
        view.setTextColor(palette.textSecondary)
        view.textSize = 13f
        view.setPadding(dp(12), 0, dp(12), dp(10))
    }

    fun styleMenuAction(view: TextView) {
        view.gravity = Gravity.CENTER_VERTICAL
        view.setTextColor(palette.textPrimary)
        view.textSize = 17f
        view.minimumHeight = dp(GlazeContract.MENU_ACTION_HEIGHT_DP)
        view.setPadding(dp(18), 0, dp(18), 0)
        view.isClickable = true
        view.isFocusable = true
        view.background = interactiveBackground(
            GlazeContract.MaterialLevel.Surface,
            GlazeContract.MaterialLevel.Glaze,
            GlazeContract.CHROME_CONTROL_CORNER_DP,
        )
    }

    fun styleProgress(progressBar: ProgressBar) {
        progressBar.progressTintList = ColorStateList.valueOf(palette.accent)
        progressBar.progressBackgroundTintList = ColorStateList.valueOf(Color.TRANSPARENT)
    }

    fun styleWebContent(webView: WebView) {
        webView.setBackgroundColor(palette.canvas)
    }

    private fun interactiveBackground(
        resting: GlazeContract.MaterialLevel,
        active: GlazeContract.MaterialLevel,
        cornerDp: Int,
    ): StateListDrawable = StateListDrawable().apply {
        addState(
            intArrayOf(android.R.attr.state_pressed),
            material(active, dp(cornerDp), focused = false, outlined = true),
        )
        addState(
            intArrayOf(android.R.attr.state_focused),
            material(active, dp(cornerDp), focused = true, outlined = true),
        )
        addState(intArrayOf(), material(resting, dp(cornerDp), outlined = false))
    }

    private fun material(
        level: GlazeContract.MaterialLevel,
        cornerRadiusPx: Int,
        focused: Boolean = false,
        outlined: Boolean = false,
    ): GradientDrawable = GradientDrawable().apply {
        shape = GradientDrawable.RECTANGLE
        cornerRadius = cornerRadiusPx.toFloat()
        setColor(colorFor(level))
        if (focused || outlined) {
            setStroke(
                dp(if (focused) 3 else 1),
                if (focused) palette.accent else palette.outline,
            )
        }
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
        val canvas = if (night) Color.rgb(11, 13, 17) else Color.rgb(245, 247, 250)
        val text = if (night) Color.rgb(245, 247, 250) else Color.rgb(21, 26, 35)
        val secondary = if (night) Color.rgb(176, 183, 195) else Color.rgb(93, 102, 117)
        val accent = Color.rgb(120, 167, 255)

        return if (night) {
            Palette(
                canvas = canvas,
                surface = 0x9E19191B.toInt(),
                softGlaze = 0xA61C1D20.toInt(),
                glaze = 0xB31A1C21.toInt(),
                deepGlaze = 0xCC17191D.toInt(),
                liveGlaze = 0xD91A1C21.toInt(),
                textPrimary = text,
                textSecondary = secondary,
                outline = 0x1AFFFFFF,
                accent = accent,
            )
        } else {
            Palette(
                canvas = canvas,
                surface = 0x94FFFFFF.toInt(),
                softGlaze = 0xA8FFFFFF.toInt(),
                glaze = 0xBFFFFFFF.toInt(),
                deepGlaze = 0xD9FFFFFF.toInt(),
                liveGlaze = 0xE8FFFFFF.toInt(),
                textPrimary = text,
                textSecondary = secondary,
                outline = 0x1A505050,
                accent = accent,
            )
        }
    }

    fun dp(value: Int): Int = (value * context.resources.displayMetrics.density).toInt()
}
