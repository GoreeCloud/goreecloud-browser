# GoreeCloud Browser Android Beta APK

**Target:** Android installable beta APK  
**Package:** `io.goreecloud.browser.beta`  
**Candidate version:** `0.1.0-beta.1+android.9`  
**Candidate versionCode:** `10009`  
**Minimum Android:** 8.0 / API 26  
**Target Android API:** 35  
**Current GLAZE UI target:** 1.2.0 Stable  
**GLAZE UI Stable promotion merge revision:** `f285b9145e27e6e7027b075c37299d101945c272`  
**GLAZE UI V1.2 source-qualification anchor:** `b0eadf9a60f73d45caffb62ffc7e9e0334cddc97`  
**Production approved:** No

## Purpose

This target is a real installable GoreeCloud Browser beta for Android. Android System WebView/Chromium is a replaceable web-engine dependency; GoreeCloud owns the Android browser chrome, navigation/search policy, privacy defaults, security gates, design-system mapping, and product behavior.

The `+android.9` slice is stacked on the `+android.8` accessibility-chrome hardening and the repository-local GLAZE UI V1.2 Stable migration. It removes fixed exact heights from text-bearing Browser chrome so Android-native `sp` text measurement can grow the top omnibox, scheme/address controls, and Browser-menu actions when system font scaling requires more vertical content space. Existing 48dp/56dp minimum interaction floors remain intact, icon-only bottom navigation remains fixed, and the unified address field intentionally remains a single-line browser control.

The source contract records 2.0 font scale as the large-text acceptance target, but this branch does not claim that rendered 200% text behavior has been accepted on representative Android devices. It is source/runtime layout hardening only until target-environment evidence exists.

Historical `+android.5` / Glaze UI 2.2, intermediate `+android.6` / V1.1, parent `+android.7` / V1.2-only mapping, and `+android.8` accessibility-chrome candidate remain rollback/comparison inputs. Central GLAZE UI V1.2 Stable promotion and these source hardening slices do not auto-promote Browser.

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
- accessibility-aware top-chrome visibility that keeps the omnibox available while Android accessibility services are enabled without enumerating those services;
- content-height text-bearing chrome so Android font scaling can expand the omnibox and menu actions rather than forcing them into fixed 56dp boxes;
- unit tests for unified address/search policy, unfocused address presentation, accessibility-aware chrome visibility, and GLAZE UI Android mapping contracts including the large-text source contract;
- CI build, lint, unit-test, APK signature verification, package/label/version verification, SHA-256 generation, exact-source revision recording, and artifact upload.

## Mobile browser chrome

The beta preserves the current compact native mobile structure:

- no Android Activity action bar in normal browsing;
- no developer/status banner occupying the browsing viewport;
- a dedicated top omnibox with most horizontal space reserved for address/search editing;
- a compact `HTTPS` / `HTTP` / `WEB` scheme indicator that does not manufacture security evidence;
- a vector Go control;
- full-width web content between chrome regions;
- a fixed 56dp bottom toolbar for Back, Forward, GoreeCloud Search Home, Reload/Stop, and Browser menu;
- vector navigation icons;
- page progress overlaid at the top of content;
- full URL exposure on omnibox focus and leading-hostname anchoring when unfocused;
- Android Back dismissing omnibox editing before page-history navigation;
- a 56dp minimum omnibox baseline inside 8dp top/bottom chrome gutters, with text-bearing top chrome allowed to grow above the baseline under Android font scaling;
- a 128dp baseline expanded Browser-chrome budget at normal text scale before system bars rather than a hard maximum height;
- scroll-aware top-chrome collapse to a 56dp bottom-toolbar-only state when accessibility services are not active;
- top-chrome persistence while Android reports an accessibility service enabled, including immediate restoration if accessibility is enabled while the Activity is active;
- Browser-menu title/subtitle/actions measured by content height, with action rows retaining at least the 56dp baseline target;
- a Browser-owned Glaze bottom-sheet menu rather than platform `PopupMenu`.

The Browser menu is not a substitute for future full Settings, tabs, private-browsing, permissions, downloads, security, privacy, or account surfaces.

The accessibility-aware collapse policy and large-text layout changes are source/runtime hardening, not downstream accessibility acceptance. Representative TalkBack, Switch Access, Voice Access, focus-order, announcement-quality, rendered 200% text behavior, and physical-device behavior remain separate validation gates.

## GLAZE UI V1.2 Android mapping

The current Browser source mapping records:

- Stable version `1.2.0`;
- Stable promotion merge revision `f285b9145e27e6e7027b075c37299d101945c272`;
- V1.2 source-qualification anchor `b0eadf9a60f73d45caffb62ffc7e9e0334cddc97`;
- optical foundation `tokens/glaze-v1.2-optical-foundation.candidate.json` and Stable web/runtime entrypoints; the `.candidate` filename is upstream promoted-source lineage, not current lifecycle state;
- governing rule **Neutral glass is the material. Color is an accent.**;
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
- text-bearing Browser chrome uses content height rather than an exact fixed height so native Android text metrics may expand it;
- source contract records 2.0 font scale as the large-text downstream acceptance target;
- Light, Dark, and Deep Dark appearance targets;
- Calm expression and Balanced clarity;
- upper-left optical light direction;
- Frost White as a neutral material reference with bounded Ice Blue atmosphere rather than chromatic substrate tinting;
- no default Deep Teal, Soft Amber, green, aqua, or other chromatic base material;
- no nested backdrop-blur stack;
- no required Environmental Color Memory or content sampling;
- visible semantic focus/state treatment;
- effects-free fallback that does not require blur, transparency, or animation;
- vector icons for Browser-owned chrome;
- explicit no-action-bar, no-development-status, Browser-owned-menu, accessibility-aware scroll-chrome, and text-content-height contracts.

Protected semantic meaning, focus, accessibility, and required boundaries override atmosphere. GLAZE UI presentation does not manufacture security, privacy, identity, recovery, coordination, Search, or Sync truth.

The 56dp Touch Assistance floor is represented in source, but this branch does not claim a complete GoreeCloud Touch Assistance runtime preference or OS mapping.

This is **not** complete downstream GLAZE UI acceptance. Native-device visual quality, TalkBack, Switch Access, Voice Access, rendered 200% text/large-text reflow, RTL/localization, Reduced Motion, Reduced Transparency/effects-free behavior, contrast/high-contrast behavior, orientation, foldable/safe-area behavior, performance, Touch Assistance behavior, and representative physical-hardware acceptance remain separate gates.

## APK build and evidence

GitHub Actions workflow: `.github/workflows/android-beta.yml`

The workflow uses Java 17, Android API 35, Android Build Tools 35.0.0, and Gradle 8.9. It runs:

```text
gradle --no-daemon clean testDebugUnitTest lintDebug assembleDebug
```

For pull requests it explicitly selects the caller PR head SHA, checks out that exact revision, verifies `git rev-parse HEAD` against `SOURCE_REVISION`, then verifies the generated APK with Android `apksigner`, checks package/application/version identity with `aapt`, records a SHA-256 checksum and source revision, and uploads the APK under an artifact name tied to that exact source revision.

Expected APK path:

```text
apps/android/app/build/outputs/apk/debug/app-debug.apk
```

Historical `+android.5`, intermediate `+android.6`, parent `+android.7`, and `+android.8` evidence are not inherited by this materially changed `+android.9` candidate. Fresh exact-source workflow evidence is required again.

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

The accessibility chrome policy reads only `AccessibilityManager.isEnabled` and listens only for enabled/disabled state changes. It does not enumerate installed or enabled accessibility services, retain accessibility state, inspect accessibility event content, or add network/telemetry behavior. The large-text reflow change uses Android-native layout measurement and existing system font scaling; it adds no user-data collection, service enumeration, network traffic, telemetry, permissions, or persistent state.

These behaviors do not establish complete Wardveil Security or Privacy Shield acceptance. Android engine-level Safe Browsing is not a substitute for GoreeCloud Wardveil runtime evidence. The omnibox scheme indicator is parsed-URL presentation, not a Wardveil or certificate-verification badge.

## Deliberate beta restrictions

The Android beta does not yet claim production/Stable readiness, complete GLAZE UI V1.2 downstream acceptance, complete Touch Assistance runtime mapping, production first-party service integration, Private Browsing acceptance, Android Wardveil download release, complete Privacy Shield behavior, Everkeep acceptance, Browser-owned website permission UI, accepted multi-tab product UI, controlled production signing, managed update/rollback, store publication, or sustained real-device acceptance.

## Promotion gates

Before Android can be described as production-approved or Stable, GoreeCloud must add and accept at minimum:

1. Controlled GoreeCloud beta/production signing and key-recovery procedures.
2. Exact-revision GLAZE UI V1.2 Browser visual/accessibility/form-factor acceptance.
3. Wardveil Browser-to-scan authenticated transport and Android download release/quarantine evidence.
4. Privacy Shield Android policy/consent/filtering integration.
5. Everkeep Android recovery/continuity integration.
6. Private Browsing request-context/storage isolation and Close & Forget behavior.
7. Required Identity, Vault, Sync, DNS, Network, Mesh, Search, and other adapters with their own accepted producer evidence.
8. Browser-owned permission prompts and Android runtime-permission mapping.
9. Full mobile surfaces required for the release scope, including accepted tab/session/settings behavior.
10. Representative real-device tests across supported Android versions, screen sizes, WebView versions, network transitions, background/restore, and sustained use.
11. TalkBack, Switch Access, Voice Access, rendered 200% text, RTL/localization, Reduced Motion, Reduced Transparency, Increased Contrast/high-contrast behavior, and Touch Assistance acceptance where supported.
12. Signed upgrade/downgrade/rollback and application-data migration tests.
13. Release artifact provenance, checksums, release notes, and production acceptance evidence.

## Status language

A passing CI workflow proves only that the exact source revision produced a structurally valid installable debug-signed APK and passed the checks actually run. It does not by itself prove production security, privacy, performance, compatibility, accessibility, recovery, GLAZE UI native-device acceptance, or Stable qualification.
