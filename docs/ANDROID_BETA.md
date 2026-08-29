# GoreeCloud Browser Android Beta APK

**Target:** Android installable beta APK  
**Package:** `io.goreecloud.browser.beta`  
**Version:** `0.1.0-beta.1+android.2`  
**versionCode:** `10002`  
**Minimum Android:** 8.0 / API 26  
**Target Android API:** 35  
**Current Glaze UI target:** 2.0.0 Stable  
**Production approved:** No

## Purpose

This target is a real installable GoreeCloud Browser beta for Android. Android System WebView/Chromium is a replaceable web-engine dependency; GoreeCloud owns the Android browser chrome, navigation/search policy, privacy defaults, security gates, design-system mapping, and product behavior.

## Implemented beta behavior

The Android beta provides:

- GoreeCloud Browser Beta application identity and launcher activity.
- HTTP/HTTPS browser intent handling.
- Browser-owned Back, Forward, Reload, unified address/search field, Go action, progress state, and web-content region.
- Direct URL navigation independent from search.
- Bare-host HTTPS upgrade.
- GoreeCloud Search as the sole query authority for non-URL input.
- JavaScript and DOM storage for modern web compatibility.
- cleartext application traffic disabled by the Android manifest.
- mixed-content loading disabled.
- Android Safe Browsing enabled; Safe Browsing hits return to safety.
- TLS/certificate errors cancelled rather than bypassed.
- third-party cookies disabled by default.
- WebView file/content access disabled.
- website permission requests and geolocation denied by default until Browser-owned permission and platform-policy surfaces are accepted.
- downloads blocked until Android can satisfy the existing Wardveil download verification/release contract.
- unit tests for unified address/search policy.
- CI build, lint, unit-test, APK signature verification, package/label verification, SHA-256 generation, and artifact upload.

## Glaze UI 2.0 Android mapping candidate

The `+android.2` beta advances Browser-owned Android chrome onto a source-level native mapping of the current Glaze UI 2.0.0 Stable contract.

Implemented source mapping:

- root application content → Canvas;
- beta/status presentation → Soft Glaze;
- primary navigation chrome → Glaze Navigation Capsule;
- unified address/search field → Surface;
- native Android controls retained for platform semantics and ergonomics;
- general interactive target floor → 48dp;
- Calm expression and Balanced clarity for the first mobile browser shell;
- native light/dark appearance adaptation;
- pressed/focused state treatment;
- semantic accessibility labels;
- effects-free fallback that does not require blur, transparency, or animation;
- contract tests for the 2.0.0 version/revision, target floor, and Android semantic mapping.

This is **not** a claim of complete Glaze UI Android acceptance. Native-device visual quality, screen-reader behavior, large-text reflow, contrast, focus, orientation, foldable/safe-area behavior, performance, and representative hardware acceptance remain separate gates.

## APK build and evidence

GitHub Actions workflow: `.github/workflows/android-beta.yml`

The workflow uses Java 17, Android API 35, Android Build Tools 35.0.0, and Gradle 8.9. It runs:

```text
gradle --no-daemon clean testDebugUnitTest lintDebug assembleDebug
```

It then verifies the generated APK with Android `apksigner`, checks package/application identity with `aapt`, records a SHA-256 checksum, and uploads the APK as a GitHub Actions artifact tied to the exact source revision.

Expected APK path:

```text
apps/android/app/build/outputs/apk/debug/app-debug.apk
```

The first authoritative post-merge installable `+android.1` baseline was produced from `main` revision `0b64c47440205fb988910841ac89a76c49ea86aa` and completed the full workflow successfully. The `+android.2` Glaze mapping is a new release candidate and must obtain its own exact-head and post-merge evidence; prior success is not inherited automatically.

## Signing boundary

The CI beta uses Android debug signing so the APK is installable without placing a GoreeCloud production private key in source control.

Debug signing is **not** the production signing authority. Fresh CI environments may use different debug certificates, so arbitrary CI artifacts are not guaranteed to upgrade one another. Until controlled GoreeCloud beta signing is provisioned through an approved secret/signing service, CI APKs should be treated as fresh-install beta artifacts.

Production signing material must never be committed to the repository.

## Security and privacy boundaries

The beta deliberately fails closed where platform integration is incomplete:

- downloads remain unavailable until Wardveil verification/release is integrated;
- site permission and geolocation requests are denied rather than silently granted;
- certificate errors cannot be bypassed by this shell;
- mixed content is not permitted;
- third-party cookies are disabled;
- local file/content access from WebView is disabled.

These behaviors do not establish complete Wardveil Security or Privacy Shield acceptance. Android engine-level Safe Browsing is also not a substitute for GoreeCloud Wardveil runtime evidence.

## Deliberate beta restrictions

The Android beta does not yet claim:

- production or Stable readiness;
- complete Glaze UI 2.0 Android acceptance;
- production GoreeCloud Identity/Vault/Sync integration;
- private-browsing runtime isolation and Close & Forget acceptance;
- Android Wardveil download verification/release or quarantine handoff;
- complete Privacy Shield filtering and consent surfaces;
- Everkeep backup/recovery acceptance;
- Android DNS/Network/Mesh service integration;
- Browser-owned website permission UI;
- file upload/download acceptance;
- controlled production signing or managed beta signing continuity;
- Play Store or other store publication;
- signed update/downgrade/rollback and application-data migration acceptance;
- sustained real-device accessibility, performance, battery, or compatibility evidence.

## Promotion gates

Before Android can be described as production-approved or Stable, GoreeCloud must add and accept at minimum:

1. Controlled GoreeCloud beta/production signing and key-recovery procedures.
2. Exact-revision Glaze UI 2.0 Android visual/accessibility/form-factor acceptance.
3. Wardveil Browser-to-scan authenticated transport and Android download release/quarantine evidence.
4. Privacy Shield Android policy/consent/filtering integration.
5. Everkeep Android recovery/continuity integration.
6. Private Browsing request-context/storage isolation and Close & Forget behavior.
7. GoreeCloud Identity, Vault, Sync, DNS, Network, Mesh, and other required adapters.
8. Browser-owned permission prompts and Android runtime-permission mapping.
9. Real-device tests across supported Android versions, screen sizes, WebView versions, network transitions, background/restore, and sustained use.
10. Signed upgrade/downgrade/rollback and application-data migration tests.
11. Release artifact provenance, checksums, release notes, and production acceptance evidence.

## Status language

A passing CI workflow proves that the exact source revision produced a structurally valid installable debug-signed APK and passed the checks actually run. It does not by itself prove production security, privacy, performance, compatibility, accessibility, recovery, or Stable qualification.
