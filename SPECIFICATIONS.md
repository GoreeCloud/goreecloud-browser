# GoreeCloud Browser Specifications

## Product

GoreeCloud Browser is an original GoreeCloud-owned native web browser. GoreeCloud owns the product experience, application architecture, browser chrome, state models, navigation/search behavior, privacy/security integrations, synchronization behavior, and service boundaries. Mature rendering engines may be used as narrowly scoped technical foundations.

## Current lifecycle

- Browser channel: `0.1.0-beta.1`
- Production approved: No
- Stable: No
- Current mandatory GLAZE UI target: `1.2.0`
- GLAZE UI Stable promotion merge revision: `f285b9145e27e6e7027b075c37299d101945c272`
- GLAZE UI V1.2 source-qualification anchor: `b0eadf9a60f73d45caffb62ffc7e9e0334cddc97`
- GLAZE UI optical foundation: `tokens/glaze-v1.2-optical-foundation.candidate.json`
- Android V1.2 source mapping: Development migration parent on `+android.7`; accessibility-chrome hardening stacked as `+android.8`; large-text content-height hardening stacked as `+android.9`; RTL history-control directionality hardening stacked as `+android.10`; Android string-resource localization-readiness hardening stacked as `+android.11`; debug pseudolocale testability stacked as `+android.12`; unfocused address bidi-presentation hardening stacked as `+android.13`; downstream rendered/native-device/localization acceptance pending
- Canonical source repository: `GoreeCloud/goreecloud-browser`

## Android beta

- User-facing identity: GoreeCloud Browser Beta
- Debug-beta package: `io.goreecloud.browser.beta`
- Debug-beta version on this stacked Development branch: `0.1.0-beta.1+android.13`
- Android versionCode: `10013`
- Minimum Android: API 26
- Compile/target API: 35
- Java/Kotlin target: 17
- Rendering runtime: Android System WebView/Chromium as an engine dependency
- Beta signing: Android debug signing for fresh-install testing
- Debug pseudolocale generation: enabled for Android `en-XA` and `ar-XB`
- Release pseudolocale opt-in from this contract: No
- Production signing material: not stored in source control and not yet accepted

The Android beta must remain installable without implying production readiness. CI validates unit tests, Android lint, APK assembly, APK signature/package/version identity, generated `en-XA`/`ar-XB` pseudolocale presence in debug APK badging, checksum generation, exact-source revision recording, and artifact upload.

## Native session recovery contract

The engine-independent native core includes a source-level session-recovery checkpoint/candidate contract for normal Browser windows.

The current contract:

- models normal, Private, and Isolated Private window privacy modes but persists only normal-window state;
- deliberately exposes no policy switch that can enable private-window checkpoint persistence;
- requires every accepted checkpoint to have a non-empty checkpoint identifier and nonzero capture timestamp;
- sanitizes checkpoint windows before handing state to a persistence implementation;
- distinguishes running, clean-shutdown, and unclean-shutdown checkpoint states;
- treats a newest running or unclean normal-window checkpoint as a recovery candidate;
- never searches past the newest checkpoint for an older crash candidate;
- masks a newest checkpoint containing only private state into no recoverable candidate;
- supports explicit checkpoint discard through the abstract recovery-store boundary.

This is a tested source contract only. Durable authenticated-encrypted recovery persistence, platform-backed recovery-key protection, complete application lifecycle wiring, restore execution, user-facing GLAZE UI recovery UI, Everkeep recovery acceptance, and production recovery behavior remain pending.

## First-party service capability consumer contract

Authoritative Browser source contains a fail-closed first-party capability consumer boundary. A capability is usable through this generic Browser gate only when the service is available and exactly one matching capability record is present, versioned, current, authoritative, explicitly production-accepted, and—when requested—an exact contract-version match.

Duplicate or ambiguous evidence, unversioned records, stale/non-authoritative evidence, wrong capability/version, degraded services, and pre-Stable producer evidence fail closed. The consumer boundary does not manufacture producer authority, automatically wire every first-party service adapter, establish live production integration, or qualify Browser as Stable.

## Android navigation contract

The unified address/search field resolves input according to Browser-owned policy:

1. Empty input opens GoreeCloud Search home.
2. Explicit HTTP/HTTPS URLs navigate directly.
3. Bare host input is upgraded to HTTPS.
4. Other text is sent only to GoreeCloud Search.
5. Non-web schemes are not accepted as ordinary Browser web navigation.

## Android mobile chrome contract

The installed beta uses a Browser-owned two-region mobile shell:

- a 56dp minimum top omnibox baseline inside an 8dp top/bottom chrome gutter;
- text-bearing top chrome measured with content height so Android font scaling may expand it above the normal baseline rather than clipping it into an exact 56dp box;
- a 56dp persistent icon-only bottom navigation toolbar;
- full-width web content between those regions;
- Browser-owned vector controls for Back, Forward, Search Home, Reload/Stop, and Browser menu;
- Android RTL support declared at the application level, with directional Back and Forward vector controls configured for native auto-mirroring;
- programmatic Browser chrome using logical start/end horizontal margins rather than physical left/right margin fields in the current source contract;
- Browser-owned visible/accessibility natural-language chrome copy sourced from Android string resources rather than embedded English UI literals in `BrowserActivity.kt`;
- HTTPS, HTTP, and WEB protocol/status tokens explicitly marked non-translatable;
- debug-only Android pseudolocale generation for `en-XA` and `ar-XB`, with a source contract keeping the release build type outside this debug testability boundary;
- page-load progress overlaid on web content;
- unfocused address presentation that removes the scheme, keeps the hostname at the leading edge, and removes Unicode bidi-formatting controls from the condensed presentation string while preserving ordinary RTL letters;
- complete original `currentUrl` exposure and selection when the omnibox receives focus; the presentation sanitizer does not mutate navigation authority;
- a deliberately single-line address/search editor whose height can grow with native Android text metrics while horizontal address editing remains a browser-control behavior;
- Browser-menu action rows that use content height with a 56dp minimum so scaled or wrapped action labels are not constrained to a hard 56dp height;
- scroll-aware top chrome that collapses after meaningful downward page scrolling and returns on upward scrolling, page-top return, omnibox focus, or new navigation when Android accessibility services are not active;
- top chrome that is kept visible while Android reports an accessibility service enabled and is restored immediately if accessibility becomes enabled while the Activity is active;
- Browser-owned Glaze menu presentation rather than the Android platform-default popup menu.

At normal text scale, the baseline expanded Browser-chrome budget is 128dp before Android system bars. This value is no longer a hard maximum: text-bearing top chrome may grow above the baseline when Android font scaling requires additional measured height. The collapsed scrolling state retains only the fixed 56dp bottom navigation toolbar and is allowed only while Android does not report an accessibility service enabled.

The accessibility visibility policy consumes only `AccessibilityManager.isEnabled` and its boolean state-change callback. It does not enumerate service identities, inspect accessibility event/user content, persist accessibility state, add permissions, or create telemetry/network behavior. The large-text layout change uses Android-native text/layout measurement and existing system font scaling and adds no new user-data processing. The RTL directionality slice uses Android manifest/vector/layout semantics only. The localization-resource slice moves Browser-owned display/accessibility copy into Android resources. The pseudolocale slice is a build-time debug resource-generation setting. The bidi-presentation slice changes only the condensed unfocused display string. None of these slices adds permissions, persistence, telemetry, network behavior, or user-content processing.

The source contract records 2.0 font scale as the downstream large-text acceptance target. This source/runtime hardening is not representative 200% rendered acceptance, TalkBack, Switch Access, Voice Access, focus-order, announcement-quality, or physical-device acceptance. The RTL source contract likewise does not establish translated Browser copy, bidirectional address-field acceptance, rendered RTL layout, combined RTL + large-text behavior, or representative locale/device acceptance. The localization-resource source contract creates translation-ready resource boundaries only. The pseudolocale contract and APK badging proof establish debug test-resource generation only. The bidi-presentation contract proves bounded source behavior only: it does not establish complete Unicode/IDN/confusable handling, bidirectional editing acceptance, rendered address ordering, or representative locale/device acceptance.

## GLAZE UI V1.2 Android contract

This migration maps Browser-owned Android chrome to the current Stable consumer target, GLAZE UI V1.2 (`1.2.0`). The source records Stable promotion merge revision `f285b9145e27e6e7027b075c37299d101945c272`, V1.2 source-qualification anchor `b0eadf9a60f73d45caffb62ffc7e9e0334cddc97`, optical foundation `tokens/glaze-v1.2-optical-foundation.candidate.json`, and Stable web/runtime entrypoints.

The `.candidate` suffix in the promoted optical-foundation source path is upstream provenance and does not make V1.2 a Candidate. Live lifecycle authority is V1.2 Stable / `1.2.0`.

Browser preserves the working mobile-shell structure while applying the current authority contract:

- governing material rule: **Neutral glass is the material. Color is an accent.**
- Canvas for the root background and top chrome;
- Soft Glaze for the omnibox and Browser menu sheet;
- Surface for the address-field role and bottom navigation;
- Application System Shell scope; Browser menu/search chrome does not claim Universal Search, Control Center, System Panel, or Critical System authority;
- Calm expression and Balanced clarity;
- 48dp ordinary interaction floor;
- 56dp Touch Assistance floor where applicable, without claiming an unwired Android preference/OS mapping;
- text-bearing Browser chrome uses content height rather than exact fixed heights so native Android `sp` metrics can expand it;
- 2.0 font scale is recorded as the downstream large-text acceptance target, without claiming rendered acceptance from source alone;
- Android RTL support plus native auto-mirroring for directional Back/Forward Browser vectors, without claiming localization or rendered RTL acceptance;
- Browser-owned natural-language chrome/accessibility strings are Android-resource-backed for future localization, while protocol/status tokens remain non-translatable;
- debug Android `en-XA` / `ar-XB` pseudolocale generation for later expansion/RTL stress testing, with release behavior outside that debug source contract;
- unfocused condensed address presentation strips Unicode bidi-formatting controls without modifying the authoritative full URL exposed on focus;
- at most one dominant Glaze panel plus three small floating Glaze controls;
- Light, Dark, and Deep Dark structural appearance targets;
- upper-left optical light direction;
- Frost White neutral material reference and bounded Ice Blue atmosphere outside semantic authority;
- no default Deep Teal, Soft Amber, green, aqua, or other chromatic substrate mapping;
- no nested backdrop blur;
- no required Environmental Color Memory, environmental-content sampling, or remote color derivation;
- semantic native labels and visible focus/state treatment;
- effects-free operation without requiring blur/transparency;
- Browser-owned vector icons;
- explicit no-action-bar, no-development-banner, Browser-owned-menu, accessibility-aware scroll-chrome, large-text content-height, RTL-directionality, localization-resource, debug-pseudolocale, and bidi-address-presentation source contracts.

The inherited semantic-state ordering remains represented in the Browser contract. Disabled and error semantics continue to override lower-priority interaction presentation. V1.2 material/atmosphere cannot change security, privacy, identity, recovery, coordination, Search, or Sync truth.

This mapping is not native-device downstream acceptance. Production acceptance requires exact-revision rendered visual review, TalkBack/Switch Access/Voice Access and broader accessibility evidence, rendered 200% text behavior, contrast/high-contrast behavior, Reduced Motion, Reduced Transparency/effects-free behavior, actual translated/localized copy and translation-quality acceptance, locale fallback/grammar behavior, rendered pseudolocale stress review, complete Unicode/IDN/confusable review, bidirectional text/address editing behavior, rendered RTL directionality, responsive/form-factor behavior, performance, Touch Assistance mapping where supported, and representative physical-device evidence.

## Security boundary

Wardveil Security is the authoritative GoreeCloud security system. Android Browser must not manufacture Wardveil status.

Current Android beta security behavior includes TLS fail-closed handling, Android Safe Browsing, mixed-content blocking, disabled WebView file/content access, default-denied site permissions, and a blocked download path until Android can satisfy the Browser-to-Wardveil release contract.

Android System WebView remains responsible for engine/platform security mechanisms; Wardveil remains responsible for GoreeCloud security evaluation, protection, evidence, and response where integrated.

The address presentation sanitizer is not a complete anti-spoofing or URL-safety system. It removes a bounded set of Unicode bidi-formatting controls only from the condensed unfocused presentation. It does not rewrite navigation URLs, establish IDN/confusable safety, or manufacture Wardveil trust.

## Privacy boundary

Privacy Shield is the authoritative privacy and data-use governance system. The beta currently uses privacy-protective defaults including third-party-cookie blocking and denied permission/geolocation requests.

The accessibility chrome policy uses only Android's boolean accessibility-enabled state and does not enumerate services or inspect accessibility event/user content. The large-text change relies only on Android-native layout/font metrics. The RTL directionality change is limited to Android source/resource semantics. The localization-resource slice moves Browser-owned UI copy between application source/resource files. The pseudolocale slice adds generated debug resources only. The bidi-presentation slice transforms only the local condensed display string. These changes add no telemetry, service discovery, persisted accessibility/locale preference state, user-content processing, or new authority. They are bounded source/build behaviors, not complete Privacy Shield acceptance.

The native session-recovery core excludes Private and Isolated Private windows before persistence. This is a source-level privacy invariant, not complete private-browsing runtime acceptance.

Complete Privacy Shield consent, filtering, private-browsing, diagnostics, policy, and user-control acceptance remains pending.

## Continuity boundary

Everkeep is authoritative for resilience, recovery, preservation, portability, and continuity. The native session-recovery core establishes a Browser-owned checkpoint/candidate boundary that future Everkeep integration can consume, but durable recovery storage, lifecycle integration, restoration, backup, portability, and migration acceptance remain pending.

## Identity and integration boundaries

GoreeCloud Identity is authoritative for identity/authentication/authorization. GoreeCloud Mesh is authoritative for platform coordination and capability discovery. GoreeCloud Search is the sole approved integrated query authority for Browser search entry points. GoreeCloud Vault, Sync, DNS, Network, Bookmarks, Wayfinder, and other GoreeCloud services remain separate capability authorities according to their contracts.

## Production promotion blockers

At minimum, Stable Android promotion remains blocked by:

- controlled signing and update/rollback key operations;
- complete GLAZE UI V1.2 native-device visual/accessibility/form-factor acceptance on the exact Browser revision;
- authenticated Wardveil download verification/release integration;
- accepted Privacy Shield runtime integration;
- accepted Everkeep recovery/continuity integration;
- private-browsing isolation and Close & Forget evidence;
- Browser-owned permission workflows;
- required Identity/Vault/Sync/DNS/Network/Mesh/Search adapters with accepted producer evidence;
- representative supported-device testing;
- actual translations plus translation completeness/quality and locale fallback/grammar acceptance;
- rendered pseudolocale stress testing across representative screen sizes and configurations;
- complete Unicode/IDN/confusable review and bidirectional URL/address editing acceptance;
- TalkBack, Switch Access, Voice Access, accessibility focus/announcement quality, rendered RTL behavior, and rendered 200% text-scaling acceptance across representative locales;
- Touch Assistance runtime mapping where the supported Android scope requires it;
- upgrade/downgrade/data-migration acceptance;
- release provenance and operational recovery evidence.

A lower acceptance state must never be represented as a higher one.
