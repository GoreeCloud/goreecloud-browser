plugins {
    id("com.android.application")
    kotlin("android")
}

android {
    namespace = "io.goreecloud.browser"
    compileSdk = 35

    defaultConfig {
        applicationId = "io.goreecloud.browser"
        minSdk = 26
        targetSdk = 35
        versionCode = 1
        versionName = "0.1.0"
    }

    buildTypes {
        getByName("release") {
            isMinifyEnabled = false
            signingConfig = null
        }
    }
}

// Development scaffold only. Production APKs require a GoreeCloud signing
// configuration supplied outside source control, real Chromium/native browser
// runtime integration, Glaze UI surfaces, Tabmark launcher assets, upgrade
// validation, and real-device acceptance evidence.
