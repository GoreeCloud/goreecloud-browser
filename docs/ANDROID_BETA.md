# GoreeCloud Browser Android Beta APK

**Target:** Android installable beta APK  
**Package:** `io.goreecloud.browser.beta`  
**Candidate version:** `0.1.0-beta.1+android.4`  
**Candidate versionCode:** `10004`  
**Minimum Android:** 8.0 / API 26  
**Target Android API:** 35  
**Current Glaze UI target:** 2.0.0 Stable  
**Production approved:** No

## Purpose

This target is a real installable GoreeCloud Browser beta for Android. Android System WebView/Chromium is a replaceable web-engine dependency; GoreeCloud owns the Android browser chrome, navigation/search policy, privacy defaults, security gates, design-system mapping, and product behavior.

The `+android.4` slice is a real-device polish iteration based on screenshots from the installed `+android.3` beta. It preserves the successful compact two-region shell while correcting long-address presentation, reclaiming page viewport during scrolling, and replacing the remaining platform-default Browser menu with a Browser-owned Glaze surface. These changes remain beta UI work and do not establish production design acceptance.

## Implemented beta behavior

The Android beta provides:

- GoreeCloud Browser Beta application identity and launcher activity.
- HTTP/HTTPS browser intent handling.
- direct URL navigation independent from search.
- bare-host HTTPS upgrade.
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
- unit tests for unified address/search policy, unfocused address presentation, and Glaze Android mapping contracts.
- CI build, lint, unit-test, APK signature verification, package/label verification, SHA-256 generation, and artifact upload.

## Mobile browser chrome candidate

The Android beta uses a compact native mobile structure:

- no Android Activity action bar in the normal browsing surface;
- no developer/status banner occupying the browsing viewport;
- a dedicated top omnibox with the majority of horizontal space reserved for address/search editing;
- a compact scheme indicator that reports `HTTPS`, `HTTP`, or generic web state without manufacturing security evidence;
- an integrated Go control using a vector icon rather than a large text button;
- full-width web content between the two chrome regions;
- a separate 56dp bottom toolbar for Back, Forward, GoreeCloud Search Home, Reload/Stop, and Browser menu;
- vector navigation icons rather than font glyphs;
- Reload switching to Stop while a page is loading;
- loading progress overlaid at the top of web content instead of consuming another row;
- omnibox display text condensed when unfocused while the complete URL is exposed for editing on focus;
- unfocused long addresses are reset to the leading edge so the hostname remains visible rather than horizontally scrolling to the path suffix;
- Android Back dismisses omnibox editing before navigating page history;
- the expanded fixed normal chrome budget is 128dp before system bars;
- after meaningful downward page scrolling, the top omnibox can collapse so only the 56dp bottom toolbar remains fixed; upward scrolling, returning to the top, focusing the omnibox, or starting navigation restores the top chrome;
- the Browser menu is a Browser-owned Glaze bottom sheet rather than an Android platform `PopupMenu`;
- the menu removes the redundant Search Home action because Home already has a dedicated toolbar control, and retains bounded actions for copying the current address, sharing the page, and viewing beta information.

The Browser menu is not a substitute for the future full Settings, tabs, private-browsing, permissions, downloads, security, privacy, or account surfaces. Those features remain separately capability-gated.

## Glaze UI 2.0 Android mapping

The Android shell maps Browser-owned chrome to the current Glaze UI 2.0.0 Stable contract using native Android controls.

The `+android.4` source mapping uses:

- application background → Canvas;
- top chrome → Canvas;
- omnibox capsule → Soft Glaze;
- address field semantics → Surface;
- bottom navigation chrome → Surface;
- Browser menu sheet → Soft Glaze with Surface actions;
- native Android controls for platform semantics and ergonomics;
- general interactive target floor → 48dp;
- Calm expression and Balanced clarity;
- native light/dark appearance adaptation;
- pressed/focused state treatment;
- semantic accessibility labels;
- effects-free fallback that does not require blur, transparency, or animation;
- vector icons for Browser-owned chrome;
- an explicit no-action-bar contract;
- an explicit contract that development-status text is not part of normal browsing chrome;
- an explicit contract that the Browser menu is not the platform-default popup surface;
- an explicit scroll-aware top-chrome contract that can reduce fixed Browser chrome from 128dp expanded to 56dp collapsed.

This is **not** a claim of complete Glaze UI Android acceptance. Native-device visual quality, TalkBack behavior, large-text reflow, contrast, focus, orientation, foldable/safe-area behavior, performance, and representative hardware acceptance remain separate gates.

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

The authoritative `+android.3` predecessor was produced from signed `main` revision `d1e386c41665a3dce0de41145fae32752e7a1bc3`. Post-merge Android workflow run `33269086720` passed unit tests, lint, APK assembly, signature/package verification, checksum generation, and artifact upload; Browser Core CI run `33269086689` passed all four declared jobs. The `+android.4` real-device polish candidate is materially changed and must obtain its own exact-head and post-merge evidence; prior success is not inherited automatically.

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

These behaviors do not establish complete Wardveil Security or Privacy Shield acceptance. Android engine-level Safe Browsing is also not a substitute for GoreeCloud Wardveil runtime evidence. The omnibox scheme indicator is presentation of the parsed URL scheme, not a security-verification badge.

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
- multi-tab product acceptance or tab-management UI in this Android slice;
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
9. Full mobile browser surfaces required for the supported release scope, including accepted tab/session/settings behavior.
10. Real-device tests across supported Android versions, screen sizes, WebView versions, network transitions, background/restore, and sustained use.
11. Signed upgrade/downgrade/rollback and application-data migration tests.
12. Release artifact provenance, checksums, release notes, and production acceptance evidence.

## Status language

A passing CI workflow proves that the exact source revision produced a structurally valid installable debug-signed APK and passed the checks actually run. It does not by itself prove production security, privacy, performance, compatibility, accessibility, recovery, or Stable qualification.
