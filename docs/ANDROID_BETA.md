# GoreeCloud Browser Android Beta APK

**Target:** Android installable beta APK  
**Package:** `io.goreecloud.browser.beta`  
**Candidate version:** `0.1.0-beta.1+android.6`  
**Candidate versionCode:** `10006`  
**Minimum Android:** 8.0 / API 26  
**Target Android API:** 35  
**Current GLAZE UI target:** 1.1.0 Stable  
**GLAZE UI Stable tag:** `v1.1.0`  
**GLAZE UI Stable release revision:** `15cc76d2bcd4065552dc31c77145b63f34d9e7b2`  
**Production approved:** No

## Purpose

This target is a real installable GoreeCloud Browser beta for Android. Android System WebView/Chromium is a replaceable web-engine dependency; GoreeCloud owns the Android browser chrome, navigation/search policy, privacy defaults, security gates, design-system mapping, and product behavior.

The `+android.6` slice is a bounded repository-local migration from Browser's historical Glaze UI 2.2 Android mapping to the current GLAZE UI V1.1 Stable contract. It deliberately preserves the successful mobile chrome instead of visually rewriting the product for version churn. It updates design-system authority and records the V1.1 semantic, optical, appearance, accessibility, and effects-fallback constraints applicable to the current Android scope.

The previous authoritative `+android.5` / Glaze UI 2.2 revision remains historical rollback and comparison input. Central GLAZE UI V1.1 Stable promotion does not auto-promote Browser.

## Implemented beta behavior

The Android beta provides:

- GoreeCloud Browser Beta application identity and launcher activity;
- HTTP/HTTPS browser intent handling;
- direct URL navigation independent from search;
- bare-host HTTPS upgrade;
- GoreeCloud Search as the sole query authority for non-URL input;
- JavaScript and DOM storage for modern web compatibility;
- cleartext application traffic disabled by the Android manifest;
- mixed-content loading disabled;
- Android Safe Browsing enabled with return-to-safety behavior;
- TLS/certificate errors cancelled rather than bypassed;
- third-party cookies disabled by default;
- WebView file/content access disabled;
- website permission requests and geolocation denied by default until Browser-owned permission and platform-policy surfaces are accepted;
- downloads blocked until Android can satisfy the existing Wardveil download verification/release contract;
- unit tests for unified address/search policy, unfocused address presentation, and GLAZE UI Android mapping contracts;
- CI build, lint, unit-test, APK signature verification, package/label verification, SHA-256 generation, and artifact upload.

## Mobile browser chrome

The beta preserves the current compact native mobile structure:

- no Android Activity action bar in normal browsing;
- no developer/status banner occupying the browsing viewport;
- a dedicated top omnibox with most horizontal space reserved for address/search editing;
- a compact `HTTPS` / `HTTP` / `WEB` scheme indicator that does not manufacture security evidence;
- a vector Go control;
- full-width web content between chrome regions;
- a 56dp bottom toolbar for Back, Forward, GoreeCloud Search Home, Reload/Stop, and Browser menu;
- vector navigation icons;
- page progress overlaid at the top of content;
- full URL exposure on omnibox focus and leading-hostname anchoring when unfocused;
- Android Back dismissing omnibox editing before page-history navigation;
- 128dp expanded fixed Browser chrome before system bars;
- scroll-aware top-chrome collapse to a 56dp bottom-toolbar-only state;
- a Browser-owned Glaze bottom-sheet menu rather than platform `PopupMenu`.

The Browser menu is not a substitute for future full Settings, tabs, private-browsing, permissions, downloads, security, privacy, or account surfaces.

## GLAZE UI V1.1 Android mapping

The current Browser source mapping records:

- Stable version `1.1.0` and tag `v1.1.0`;
- Stable release revision `15cc76d2bcd4065552dc31c77145b63f34d9e7b2`;
- approved V1.1 visual source `8ea1f789bbabf943c3359514dc1506b24fa3c51b`;
- optical contract `contracts/v1.1/optical-refinement.json`;
- application background → Canvas;
- top chrome → Canvas;
- omnibox capsule → Soft Glaze;
- address-field semantics → Surface;
- bottom navigation chrome → Surface;
- Browser menu sheet → Soft Glaze;
- Browser-owned chrome → Application System Shell scope;
- no claim that the local omnibox/search path is Universal Search;
- no claim that the Browser menu is Control Center, System Panel, or Critical System UI;
- at most one dominant Glaze panel plus three small floating Glaze controls;
- 48dp ordinary interaction floor and 56dp Touch Assistance floor where applicable;
- Light, Dark, and Deep Dark appearance targets;
- Calm expression and Balanced clarity;
- upper-left optical light direction;
- bounded Deep Teal + Soft Amber atmosphere with neutral structure dominant;
- no nested backdrop-blur stack;
- no required Environmental Color Memory or content sampling;
- visible semantic focus/state treatment;
- effects-free fallback that does not require blur, transparency, or animation;
- vector icons for Browser-owned chrome;
- explicit no-action-bar, no-development-status, Browser-owned-menu, and scroll-aware-chrome contracts.

Protected semantic meaning, focus, accessibility, and required boundaries override atmosphere. GLAZE UI presentation does not manufacture security, privacy, identity, recovery, coordination, Search, or Sync truth.

The 56dp Touch Assistance floor is represented in source, but this branch does not claim a complete GoreeCloud Touch Assistance runtime preference or OS mapping.

This is **not** complete downstream GLAZE UI acceptance. Native-device visual quality, TalkBack, 200% text/large-text reflow, RTL/localization, Reduced Motion, Reduced Transparency/effects-free behavior, contrast/high-contrast behavior, orientation, foldable/safe-area behavior, performance, Touch Assistance behavior, and representative physical-hardware acceptance remain separate gates.

## APK build and evidence

GitHub Actions workflow: `.github/workflows/android-beta.yml`

The workflow uses Java 17, Android API 35, Android Build Tools 35.0.0, and Gradle 8.9. It runs:

```text
gradle --no-daemon clean testDebugUnitTest lintDebug assembleDebug
```

It verifies the generated APK with Android `apksigner`, checks package/application/version identity with `aapt`, records a SHA-256 checksum, and uploads the APK as a GitHub Actions artifact tied to the exact source revision.

Expected APK path:

```text
apps/android/app/build/outputs/apk/debug/app-debug.apk
```

Historical `+android.5` build evidence is not inherited by this materially changed `+android.6` candidate. Exact-head workflow evidence is required again.

## Signing boundary

CI uses Android debug signing so the APK is installable without placing a GoreeCloud production private key in source control. Debug signing is not production signing authority, and arbitrary CI artifacts may not have upgrade-compatible certificates. Production signing material must never be committed to the repository.

## Security and privacy boundaries

The beta fails closed where platform integration is incomplete:

- downloads remain unavailable until Wardveil verification/release is integrated;
- site permission and geolocation requests are denied rather than silently granted;
- certificate errors cannot be bypassed by this shell;
- mixed content is not permitted;
- third-party cookies are disabled;
- local file/content access from WebView is disabled.

These behaviors do not establish complete Wardveil Security or Privacy Shield acceptance. Android engine-level Safe Browsing is not a substitute for GoreeCloud Wardveil runtime evidence. The omnibox scheme indicator is parsed-URL presentation, not a Wardveil or certificate-verification badge.

## Deliberate beta restrictions

The Android beta does not yet claim production/Stable readiness, complete GLAZE UI V1.1 downstream acceptance, complete Touch Assistance runtime mapping, production first-party service integration, Private Browsing acceptance, Android Wardveil download release, complete Privacy Shield behavior, Everkeep acceptance, Browser-owned website permission UI, accepted multi-tab product UI, controlled production signing, managed update/rollback, store publication, or sustained real-device acceptance.

## Promotion gates

Before Android can be described as production-approved or Stable, GoreeCloud must add and accept at minimum:

1. Controlled GoreeCloud beta/production signing and key-recovery procedures.
2. Exact-revision GLAZE UI V1.1 Browser visual/accessibility/form-factor acceptance.
3. Wardveil Browser-to-scan authenticated transport and Android download release/quarantine evidence.
4. Privacy Shield Android policy/consent/filtering integration.
5. Everkeep Android recovery/continuity integration.
6. Private Browsing request-context/storage isolation and Close & Forget behavior.
7. Required Identity, Vault, Sync, DNS, Network, Mesh, Search, and other adapters with their own accepted producer evidence.
8. Browser-owned permission prompts and Android runtime-permission mapping.
9. Full mobile surfaces required for the release scope, including accepted tab/session/settings behavior.
10. Representative real-device tests across supported Android versions, screen sizes, WebView versions, network transitions, background/restore, and sustained use.
11. 200% text, RTL/localization, Reduced Motion, Reduced Transparency, Increased Contrast/high-contrast behavior, and Touch Assistance acceptance where supported.
12. Signed upgrade/downgrade/rollback and application-data migration tests.
13. Release artifact provenance, checksums, release notes, and production acceptance evidence.

## Status language

A passing CI workflow proves only that the exact source revision produced a structurally valid installable debug-signed APK and passed the checks actually run. It does not by itself prove production security, privacy, performance, compatibility, accessibility, recovery, GLAZE UI native-device acceptance, or Stable qualification.
