# GoreeCloud Browser Android Beta APK

**Target:** Android installable beta APK  
**Package:** `io.goreecloud.browser.beta`  
**Version:** `0.1.0-beta.1+android.1`  
**Minimum Android:** 8.0 / API 26  
**Target Android API:** 35  
**Production approved:** No

## Purpose

This target turns the existing Android scaffold into a real installable GoreeCloud Browser beta that can be built and tested on Android devices while preserving the Browser's native-product and platform-authority boundaries.

The Android System WebView/Chromium runtime is treated as a replaceable web-engine dependency. It is not the GoreeCloud product layer. The Android activity owns the GoreeCloud browser chrome, navigation/search policy, privacy defaults, permission behavior, security fail-closed behavior, and beta capability gates.

## Implemented beta behavior

The first installable Android beta provides:

- GoreeCloud Browser Beta application identity and Android launcher activity.
- HTTP/HTTPS browser intent handling.
- Browser-owned Back, Forward, Reload, unified address/search bar, Go control, progress state, and web-content region.
- Direct URL navigation independent from search.
- Bare-host HTTPS upgrade.
- GoreeCloud Search as the sole query authority through `https://search.goreecloud.com/search?q=...`.
- Android WebView/Chromium rendering as the beta engine dependency.
- JavaScript and DOM storage needed for modern web compatibility.
- Cleartext application traffic disabled by the Android manifest.
- Mixed-content loading disabled.
- Android Safe Browsing enabled; Safe Browsing hits return to safety.
- TLS/certificate errors cancelled rather than bypassed.
- Third-party cookies disabled by default.
- File/content access from the WebView disabled.
- Website permission requests and geolocation denied by default until Browser-owned permission and platform-policy surfaces are integrated.
- Browser downloads blocked until the Android path can satisfy the existing Wardveil download verification/release contract.
- Unit tests for the unified address/search resolver.
- CI build, lint, unit-test, APK signature verification, package/label verification, SHA-256 generation, and artifact upload.

## APK build

GitHub Actions workflow: `.github/workflows/android-beta.yml`

The workflow uses Java 17, Android API 35, Android Build Tools 35.0.0, and Gradle 8.9. It runs:

```text
gradle --no-daemon clean testDebugUnitTest lintDebug assembleDebug
```

It then verifies the generated APK with Android `apksigner`, checks the package and application identity with `aapt`, records a SHA-256 checksum, and uploads the APK as a GitHub Actions artifact.

The expected artifact file is:

```text
apps/android/app/build/outputs/apk/debug/app-debug.apk
```

## Signing boundary

The CI beta uses Android debug signing so that the generated APK is installable without storing a GoreeCloud production private key in source control.

Debug signing is **not** the production signing authority. A fresh CI environment may generate a different debug key, so upgrade compatibility between arbitrary CI artifacts is not guaranteed. Until a controlled GoreeCloud beta signing key is provisioned through protected GitHub secrets or another approved signing service, testers should treat CI APKs as fresh-install artifacts.

Production signing material must never be committed to the repository.

## Deliberate beta restrictions

The Android beta does not yet claim:

- production or Stable readiness;
- complete current-Stable Glaze UI Android acceptance;
- production GoreeCloud Identity/Vault/Sync integration;
- private-browsing runtime isolation and Close & Forget acceptance;
- Android Wardveil download verification/release or quarantine handoff;
- complete Privacy Shield filtering and consent surfaces;
- Everkeep backup/recovery acceptance;
- Android DNS/Network service integration;
- Browser-owned website permission UI;
- file upload/download acceptance;
- production signing, Play/App Store packaging, update, rollback, or migration acceptance;
- sustained real-device, accessibility, performance, battery, or compatibility evidence.

These restrictions are fail-closed where practical. In particular, the beta blocks downloads and website permission grants rather than silently bypassing GoreeCloud security/privacy authority.

## Promotion gates

Before the Android package can be described as production-approved or Stable, GoreeCloud must add and accept at minimum:

1. Controlled GoreeCloud beta/production signing and key-recovery procedures.
2. Current approved Stable Glaze UI Android integration and accessibility evidence.
3. Wardveil Browser-to-scan authenticated transport and download release/quarantine evidence.
4. Privacy Shield Android policy/consent/filtering integration.
5. Private Browsing request-context/storage isolation and Close & Forget behavior.
6. GoreeCloud Identity, Vault, Sync, Everkeep, DNS, Network, and Mesh adapters where required.
7. Browser-owned permission prompts and Android runtime-permission mapping.
8. Real-device tests across supported Android versions, screen sizes, WebView versions, network transitions, background/restore, and sustained use.
9. Signed upgrade/downgrade/rollback and application-data migration tests.
10. Release artifact provenance, checksums, release notes, and production acceptance evidence.

## Status language

A passing CI workflow proves that the exact source revision produced a structurally valid, installable debug-signed APK and passed the checks described above. It does not by itself prove production security, privacy, performance, compatibility, or Stable qualification.
