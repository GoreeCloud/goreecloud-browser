# GoreeCloud Browser Android Beta APK

**Target:** Android installable beta APK  
**Package:** `io.goreecloud.browser.beta`  
**Candidate version:** `0.1.0-beta.1+android.14`  
**Candidate versionCode:** `10014`  
**Minimum Android:** 8.0 / API 26  
**Target Android API:** 35  
**Current GLAZE UI target:** 1.2.0 Stable  
**GLAZE UI Stable promotion merge revision:** `f285b9145e27e6e7027b075c37299d101945c272`  
**GLAZE UI V1.2 source-qualification anchor:** `b0eadf9a60f73d45caffb62ffc7e9e0334cddc97`  
**Production approved:** No

## Purpose

This target is a real installable GoreeCloud Browser beta for Android. Android System WebView/Chromium is a replaceable web-engine dependency; GoreeCloud owns the Android browser chrome, navigation/search policy, privacy defaults, security gates, design-system mapping, and product behavior.

The `+android.14` slice is stacked on the V1.2 migration, `+android.8` accessibility-chrome hardening, `+android.9` large-text content-height hardening, `+android.10` RTL directionality hardening, `+android.11` localization-resource foundation, `+android.12` debug pseudolocale testability, and `+android.13` bidi-presentation hardening. It preserves every explicitly parsed URL port in the condensed unfocused address presentation, including explicitly written default ports, while retaining the prior bidi-control sanitizer and leaving the authoritative full `currentUrl` unchanged for focused editing and navigation.

The inherited debug build still generates Android `en-XA` and `ar-XB` pseudolocales only for the debug beta build type. Android CI verifies the generated debug APK itself: `aapt` badging must contain pseudolocalized `application-label-en-XA` and `application-label-ar-XB` resources and list both `en-XA` and `ar-XB` locales before the artifact can be uploaded.

This remains localization/directionality testability and source/runtime presentation hardening only. Pseudolocales are synthetic Android test resources, not translations. Bidi sanitization and explicit-port preservation are presentation-only and are not a complete URL-spoofing, origin-safety, IDN/confusable, or bidirectional-editing solution. This slice does not establish rendered pseudolocale behavior, translation completeness/quality, locale fallback/plural/grammar behavior, rendered RTL layout, combined locale + large-text behavior, or representative locale/device acceptance.

Historical `+android.5` / Glaze UI 2.2, intermediate `+android.6` / V1.1, parent `+android.7` / V1.2-only mapping, `+android.8` accessibility-chrome candidate, `+android.9` large-text candidate, `+android.10` RTL-directionality candidate, `+android.11` localization-resource candidate, `+android.12` pseudolocale-testability candidate, and `+android.13` bidi-presentation candidate remain rollback/comparison inputs. Central GLAZE UI V1.2 Stable promotion and these source hardening slices do not auto-promote Browser.

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
- application-level Android RTL support and native auto-mirroring for directional Back/Forward vector controls;
- Browser-owned visible/accessibility natural-language chrome copy sourced from Android string resources, with protocol/status tokens explicitly non-translatable;
- debug-only Android pseudolocale generation for `en-XA` and `ar-XB`;
- presentation-only removal of Unicode bidi-formatting controls from the condensed unfocused address, including LRM/RLM, Arabic Letter Mark, embedding/override controls, and isolate controls;
- preservation of ordinary RTL letters in presentation text;
- preservation of explicitly parsed URL ports in condensed presentation, including non-default and explicitly written default ports;
- preservation of the untouched authoritative `currentUrl` when the omnibox is focused;
- unit tests for unified address/search policy, address presentation including bidi-control sanitization and explicit-port preservation, accessibility-aware chrome visibility, GLAZE UI Android mapping/large-text source contracts, bounded RTL directionality source requirements, localization-resource source requirements, debug-only pseudolocale build boundary, and focused-vs-unfocused URL source separation;
- CI build, lint, unit-test, APK signature verification, package/label/version/pseudolocale verification, SHA-256 generation, exact-source revision recording, and artifact upload.

The presence of string resources and pseudolocales is not translated-locale acceptance. The default Browser resource set remains English until separately governed translations are added and validated. The address presentation helper does not rewrite navigation, search, clipboard/share URLs, WebView requests, persisted state, permissions, telemetry, or network traffic.

## Mobile browser chrome

The beta preserves the current compact native mobile structure:

- no Android Activity action bar in normal browsing;
- no developer/status banner occupying the browsing viewport;
- a dedicated top omnibox with most horizontal space reserved for address/search editing;
- a compact `HTTPS` / `HTTP` / `WEB` scheme indicator that does not manufacture security evidence and remains non-translatable;
- a vector Go control;
- full-width web content between chrome regions;
- a fixed 56dp bottom toolbar for Back, Forward, GoreeCloud Search Home, Reload/Stop, and Browser menu;
- vector navigation icons, with directional Back and Forward glyphs opting into Android auto-mirroring for RTL layout direction;
- programmatic horizontal margins expressed with logical start/end fields in the current validated Browser chrome source;
- Browser-owned natural-language labels/descriptions/messages resolved through Android string resources;
- debug `en-XA` and `ar-XB` resource variants available for later expansion/RTL stress testing;
- page progress overlaid at the top of content;
- full original URL exposure on omnibox focus and leading-hostname anchoring when unfocused;
- unfocused condensed address presentation with Unicode bidi-formatting controls removed, ordinary RTL letters retained, and explicitly parsed ports preserved;
- Android Back dismissing omnibox editing before page-history navigation;
- a 56dp minimum omnibox baseline inside 8dp top/bottom chrome gutters, with text-bearing top chrome allowed to grow above the baseline under Android font scaling;
- a 128dp baseline expanded Browser-chrome budget at normal text scale before system bars rather than a hard maximum height;
- scroll-aware top-chrome collapse to a 56dp bottom-toolbar-only state when accessibility services are not active;
- top-chrome persistence while Android reports an accessibility service enabled, including immediate restoration if accessibility is enabled while the Activity is active;
- Browser-menu title/subtitle/actions measured by content height, with action rows retaining at least the 56dp baseline target;
- a Browser-owned Glaze bottom-sheet menu rather than platform `PopupMenu`.

The Browser menu is not a substitute for future full Settings, tabs, private-browsing, permissions, downloads, security, privacy, or account surfaces.

The accessibility-aware collapse policy, large-text layout changes, RTL-directionality changes, localization-resource extraction, pseudolocale generation, bidi-address presentation hardening, and explicit-port presentation hardening are source/build/runtime hardening and testability. Representative TalkBack, Switch Access, Voice Access, focus-order, announcement-quality, rendered 200% text behavior, rendered pseudolocale behavior, actual translated/localized copy, translation quality/completeness, complete origin/Unicode/IDN/confusable behavior, bidirectional URL/address editing, rendered RTL directionality, and physical-device behavior remain separate validation gates.

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
- Android RTL support plus source-validated native auto-mirroring for directional Back/Forward vectors and logical horizontal-margin usage;
- Browser-owned natural-language chrome/accessibility copy sourced from Android resources for future localization, with protocol/status tokens explicitly non-translatable;
- debug-only `en-XA` and `ar-XB` pseudolocales for later expansion/RTL stress testing;
- condensed unfocused address presentation strips Unicode bidi-formatting controls, preserves ordinary RTL letters and explicitly parsed ports, and does not modify the authoritative full URL exposed on focus;
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
- explicit no-action-bar, no-development-status, Browser-owned-menu, accessibility-aware scroll-chrome, text-content-height, bounded RTL-directionality, localization-resource, debug-pseudolocale, bidi-address-presentation, and explicit-port-presentation source contracts.

Protected semantic meaning, focus, accessibility, and required boundaries override atmosphere. GLAZE UI presentation does not manufacture security, privacy, identity, recovery, coordination, Search, or Sync truth.

The 56dp Touch Assistance floor is represented in source, but this branch does not claim a complete GoreeCloud Touch Assistance runtime preference or OS mapping.

This is **not** complete downstream GLAZE UI acceptance. Native-device visual quality, TalkBack, Switch Access, Voice Access, rendered 200% text/large-text reflow, rendered pseudolocale stress review, actual translations and translation-quality/completeness review, locale fallback/grammar behavior, complete origin/Unicode/IDN/confusable review, bidirectional URL/address editing, rendered RTL directionality, Reduced Motion, Reduced Transparency/effects-free behavior, contrast/high-contrast behavior, orientation, foldable/safe-area behavior, performance, Touch Assistance behavior, and representative physical-hardware acceptance remain separate gates.

## APK build and evidence

GitHub Actions workflow: `.github/workflows/android-beta.yml`

The workflow uses Java 17, Android API 35, Android Build Tools 35.0.0, and Gradle 8.9. It runs:

```text
gradle --no-daemon clean testDebugUnitTest lintDebug assembleDebug
```

For pull requests it explicitly selects the caller PR head SHA, checks out that exact revision, verifies `git rev-parse HEAD` against `SOURCE_REVISION`, then verifies the generated APK with Android `apksigner`, checks package/application/version identity and `en-XA` / `ar-XB` pseudolocale presence with `aapt`, records a SHA-256 checksum and source revision, and uploads the APK under an artifact name tied to that exact source revision.

Expected APK path:

```text
apps/android/app/build/outputs/apk/debug/app-debug.apk
```

Historical `+android.5`, intermediate `+android.6`, parent `+android.7`, `+android.8`, `+android.9`, `+android.10`, `+android.11`, `+android.12`, and `+android.13` evidence are not inherited by this materially changed `+android.14` candidate. Fresh exact-source workflow evidence is required again after documentation reconciliation.

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

The accessibility chrome policy reads only `AccessibilityManager.isEnabled` and listens only for enabled/disabled state changes. It does not enumerate installed or enabled accessibility services, retain accessibility state, inspect accessibility event content, or add network/telemetry behavior. The large-text reflow change uses Android-native layout measurement and existing system font scaling. The RTL directionality change uses Android manifest/vector/layout source semantics. The localization-resource change moves Browser-owned copy between application source/resource files. The pseudolocale change generates synthetic debug resources at build time. The bidi-presentation change transforms only the local condensed address string. The explicit-port change uses the already parsed local `URI.port` value for the same condensed presentation. These changes add no user-data collection, service enumeration, network traffic, telemetry, permissions, persistent accessibility/locale state, or user-content inspection.

These behaviors do not establish complete Wardveil Security or Privacy Shield acceptance. Android engine-level Safe Browsing is not a substitute for GoreeCloud Wardveil runtime evidence. The omnibox scheme indicator is parsed-URL presentation, not a Wardveil or certificate-verification badge. Bidi sanitization and explicit-port preservation likewise do not establish site trust, complete origin safety, or complete anti-spoofing acceptance.

## Deliberate beta restrictions

The Android beta does not yet claim production/Stable readiness, complete GLAZE UI V1.2 downstream acceptance, complete Touch Assistance runtime mapping, rendered pseudolocale acceptance, actual translation/localization acceptance, complete origin/Unicode/IDN/confusable or RTL/bidirectional acceptance, production first-party service integration, Private Browsing acceptance, Android Wardveil download release, complete Privacy Shield behavior, Everkeep acceptance, Browser-owned website permission UI, accepted multi-tab product UI, controlled production signing, managed update/rollback, store publication, or sustained real-device acceptance.

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
11. Rendered `en-XA` and `ar-XB` pseudolocale stress review across representative screen sizes/configurations.
12. Actual translated/localized Browser resources with translation completeness/quality, locale fallback/plural/grammar, and representative locale acceptance.
13. Complete origin/Unicode/IDN/confusable review plus bidirectional URL/address editing and rendered RTL directionality acceptance.
14. TalkBack, Switch Access, Voice Access, rendered 200% text, Reduced Motion, Reduced Transparency, Increased Contrast/high-contrast behavior, and Touch Assistance acceptance where supported across representative locales.
15. Signed upgrade/downgrade/rollback and application-data migration tests.
16. Release artifact provenance, checksums, release notes, and production acceptance evidence.

## Status language

A passing CI workflow proves only that the exact source revision produced a structurally valid installable debug-signed APK and passed the checks actually run, including generated debug pseudolocale presence, source-level bidi presentation tests, and explicit-port presentation tests. It does not by itself prove production security, privacy, performance, compatibility, accessibility, rendered pseudolocale behavior, translation/localization quality, complete origin/Unicode/IDN/confusable handling, directionality/editing acceptance, recovery, GLAZE UI native-device acceptance, or Stable qualification.
