# GoreeCloud Browser Android Beta APK

**Target:** Android installable beta APK  
**Package:** `io.goreecloud.browser.beta`  
**Candidate version:** `0.1.0-beta.1+android.5`  
**Candidate versionCode:** `10005`  
**Minimum Android:** 8.0 / API 26  
**Target Android API:** 35  
**Current Glaze UI target:** 2.2.0 Stable  
**Glaze UI Stable release revision:** `6731098b28dd0393faa878c70d989a221d714a20`  
**Production approved:** No

## Purpose

This target is a real installable GoreeCloud Browser beta for Android. Android System WebView/Chromium is a replaceable web-engine dependency; GoreeCloud owns the Android browser chrome, navigation/search policy, privacy defaults, security gates, design-system mapping, and product behavior.

The `+android.5` slice is a bounded repository-local migration from Browser's previously integrated Glaze UI 2.0 Android mapping to the current Glaze UI 2.2.0 Stable contract. It deliberately preserves the successful `+android.4` mobile chrome instead of visually rewriting the product for novelty. The migration adds current Stable provenance and the 2.2 semantic constraints that apply to this Android scope. It is not a claim of rendered/native-device Glaze acceptance.

The previous authoritative `+android.4` evidence remains historical rollback and comparison input. Central Glaze UI 2.2 Stable promotion does not auto-promote Browser.

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
- unfocused long addresses reset to the leading edge so the hostname remains visible;
- Android Back dismisses omnibox editing before navigating page history;
- the expanded fixed normal chrome budget is 128dp before system bars;
- after meaningful downward page scrolling, the top omnibox can collapse so only the 56dp bottom toolbar remains fixed; upward scrolling, returning to the top, focusing the omnibox, or starting navigation restores the top chrome;
- the Browser menu is a Browser-owned Glaze bottom sheet rather than an Android platform `PopupMenu`;
- the menu removes the redundant Search Home action because Home already has a dedicated toolbar control, and retains bounded actions for copying the current address, sharing the page, and viewing beta information.

The Browser menu is not a substitute for future full Settings, tabs, private-browsing, permissions, downloads, security, privacy, or account surfaces. Those features remain separately capability-gated.

## Glaze UI 2.2 Android mapping

The Android shell maps Browser-owned chrome to the current Glaze UI 2.2.0 Stable contract using native Android controls.

The source contract records:

- Stable version `2.2.0`;
- Stable release revision `6731098b28dd0393faa878c70d989a221d714a20`;
- accepted Glaze UI visual source `0411b0f6dd877aea30e2c5674e1acde0105fd97b`;
- application background → Canvas;
- top chrome → Canvas;
- omnibox capsule → Soft Glaze;
- address field semantics → Surface;
- bottom navigation chrome → Surface;
- Browser menu sheet → Soft Glaze with Surface actions;
- Browser-owned chrome → **Application** scope in the Glaze UI 2.2 System Shell hierarchy;
- no claim that the local omnibox/search path is Glaze UI Universal Search;
- no claim that the Browser menu is Control Center, System Panel, or Critical System UI;
- ordinary System Glaze budget → at most one dominant Glaze panel plus up to three small floating Glaze controls;
- general touch-oriented target floor → 48dp;
- Touch Assistance/far-view target floor where applicable → 56dp;
- Calm expression and Balanced clarity;
- Glaze UI 2.2 semantic state priority with disabled above error, pressed, focus, selected, hover, and rest;
- native light/dark appearance adaptation;
- pressed/focused state treatment;
- semantic accessibility labels;
- effects-free fallback that does not require blur, transparency, or animation;
- vector icons for Browser-owned chrome;
- an explicit no-action-bar contract;
- an explicit contract that development-status text is not part of normal browsing chrome;
- an explicit contract that the Browser menu is not the platform-default popup surface;
- an explicit scroll-aware top-chrome contract that can reduce fixed Browser chrome from 128dp expanded to 56dp collapsed.

The 56dp Touch Assistance/far-view floor is now represented in the Browser source contract, but this branch does **not** claim that Android has a complete GoreeCloud Touch Assistance runtime preference/OS mapping. That wiring and its native evidence remain a promotion gate.

This is **not** a claim of complete Glaze UI Android acceptance. Native-device visual quality, TalkBack behavior, 200% text/large-text reflow, RTL/localization, contrast, focus, Reduced Motion, Reduced Transparency/effects-free behavior, applicable high-contrast/forced-color behavior, orientation, foldable/safe-area behavior, performance, Touch Assistance behavior, and representative hardware acceptance remain separate gates.

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

The authoritative `+android.4` predecessor was integrated before the current Glaze UI 2.2 migration and retains its own exact-revision evidence. The `+android.5` migration candidate is materially changed and must obtain its own exact-head workflow evidence; prior `+android.4` success is not inherited automatically.

A successful `+android.5` CI run will prove only that the exact candidate source built a structurally valid debug-signed APK and passed the checks actually run. It will not by itself satisfy rendered visual, accessibility, real-device, production-signing, or Stable acceptance.

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
- complete Glaze UI 2.2 Android native-device acceptance;
- product-specific Human Visual Excellence acceptance for this exact adoption revision;
- complete Touch Assistance runtime mapping/acceptance;
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
2. Exact-revision Glaze UI 2.2 Android visual/accessibility/form-factor acceptance, including product-specific Human Visual Excellence where required.
3. Wardveil Browser-to-scan authenticated transport and Android download release/quarantine evidence.
4. Privacy Shield Android policy/consent/filtering integration.
5. Everkeep Android recovery/continuity integration.
6. Private Browsing request-context/storage isolation and Close & Forget behavior.
7. GoreeCloud Identity, Vault, Sync, DNS, Network, Mesh, and other required adapters.
8. Browser-owned permission prompts and Android runtime-permission mapping.
9. Full mobile browser surfaces required for the supported release scope, including accepted tab/session/settings behavior.
10. Real-device tests across supported Android versions, screen sizes, WebView versions, network transitions, background/restore, and sustained use.
11. 200% text, RTL/localization, Reduced Motion, Reduced Transparency, Increased Contrast, applicable forced-color/high-contrast behavior, and Touch Assistance/far-view acceptance where supported.
12. Signed upgrade/downgrade/rollback and application-data migration tests.
13. Release artifact provenance, checksums, release notes, and production acceptance evidence.

## Status language

A passing CI workflow proves that the exact source revision produced a structurally valid installable debug-signed APK and passed the checks actually run. It does not by itself prove production security, privacy, performance, compatibility, accessibility, recovery, Glaze UI native-device acceptance, or Stable qualification.
