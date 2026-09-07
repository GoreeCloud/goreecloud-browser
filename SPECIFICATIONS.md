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
- Android V1.2 source mapping: Development migration parent on `+android.7`; accessibility-chrome hardening stacked as `+android.8`; large-text content-height hardening stacked as `+android.9`; RTL history-control directionality hardening stacked as `+android.10`; Android string-resource localization-readiness hardening stacked as `+android.11`; debug pseudolocale testability stacked as `+android.12`; unfocused address bidi-presentation hardening stacked as `+android.13`; explicit-port presentation correctness stacked as `+android.14`; navigation-authority validation stacked as `+android.15`; malformed-web external-handoff hardening stacked as `+android.16`; bare-userinfo search-routing hardening stacked as `+android.17`; local/active-content external-handoff deny hardening stacked as `+android.18`; outward page-address userinfo disclosure hardening stacked as `+android.19`; downstream rendered/native-device/localization/security acceptance pending
- Canonical source repository: `GoreeCloud/goreecloud-browser`

## Android beta

- User-facing identity: GoreeCloud Browser Beta
- Debug-beta package: `io.goreecloud.browser.beta`
- Debug-beta version on this stacked Development branch: `0.1.0-beta.1+android.19`
- Android versionCode: `10019`
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

The unified address/search field and page-link routing resolve according to Browser-owned policy:

1. Empty input opens GoreeCloud Search home.
2. Explicit HTTP/HTTPS input navigates directly only when its URI authority is structurally valid for this bounded Browser contract.
3. Malformed explicit HTTP/HTTPS input fails closed to GoreeCloud Search home; it is not forwarded to WebView and is not silently submitted to GoreeCloud Search as query text. This includes explicit `http:` / `https:` forms that omit `//`.
4. Recognized bare host input is upgraded to HTTPS. The current resolver recognizes the existing domain/IPv4/localhost/bracketed-IPv6 host shapes and supports valid explicit ports, including `localhost:port` and bracketed IPv6-with-port input.
5. Unschemed input whose authority-shaped prefix contains literal `@` is not inferred as a bare host. Email-shaped text such as `person@example.com` remains ordinary GoreeCloud Search input; explicit HTTP(S) userinfo URLs remain governed by the explicit-URL contract rather than this bare-host inference rule.
6. Optional authority ports must be decimal and in the inclusive `0..65535` range.
7. Other text is sent only to GoreeCloud Search.
8. Non-web schemes are not accepted as ordinary Browser web navigation.
9. A page-link HTTP(S) attempt remains Browser-owned even when malformed; rejection does not make it eligible for Android external-app handoff.
10. External Android handoff is eligible only when a page request has an explicit user gesture and its target has a parseable eligible non-web scheme.
11. `about:`, `blob:`, `content:`, `data:`, `file:`, and `javascript:` remain Browser-owned and are never eligible for Android external-app handoff under this source contract.

The authority, bare-host inference, and handoff validators are local syntax/routing boundaries. They deliberately do not perform DNS resolution, certificate validation, email-address validation, IDN normalization, confusable analysis, origin-trust classification, Wardveil classification, external-application trust classification, or network requests. Valid Unicode HTTP(S) host input remains permitted without converting this source boundary into an IDN safety claim. Explicit HTTP(S) userinfo remains outside the bare-input correction. The known local/active-content deny set narrows externalization but is not complete scheme, intent, or deep-link safety acceptance.

## Android outward page-address disclosure contract

Browser keeps the exact authoritative `currentUrl` for WebView navigation, history state, focused omnibox editing, and Browser-owned navigation semantics. Outward Browser menu actions apply a separate disclosure boundary:

1. Copy page address and Share page must pass the current page URL through the Browser-owned `AddressDisclosurePolicy` before exposing it to Android clipboard/share surfaces.
2. For structurally parseable HTTP(S) URLs with explicit userinfo, only the raw userinfo portion and its trailing `@` are removed from outward disclosure.
3. The remaining scheme, authority host, explicit port, path, query, fragment, and raw escaping are preserved; the policy does not normalize the URL.
4. Ordinary HTTP(S) URLs without userinfo are returned unchanged.
5. Non-web or malformed values are returned unchanged by this bounded policy rather than being reinterpreted as navigation.
6. The focused omnibox continues to expose the exact authoritative URL, including explicit HTTP(S) userinfo, because this slice changes disclosure rather than navigation/editing authority.

This is a bounded outward-disclosure policy only. It does not establish complete credential safety, clipboard privacy, Android share-target trust, phishing protection, URL/userinfo safety, credential-manager integration, origin trust, certificate trust, DNS validity, IDN/confusable safety, or Wardveil Security acceptance.

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
- unfocused address presentation that removes the scheme, keeps the hostname at the leading edge, removes Unicode bidi-formatting controls from the condensed presentation string while preserving ordinary RTL letters, and preserves any explicitly parsed URL port including explicitly written default ports;
- complete original `currentUrl` exposure and selection when the omnibox receives focus; presentation condensing/sanitization and outward disclosure hardening do not mutate navigation authority;
- a deliberately single-line address/search editor whose height can grow with native Android text metrics while horizontal address editing remains a browser-control behavior;
- Browser-menu action rows that use content height with a 56dp minimum so scaled or wrapped action labels are not constrained to a hard 56dp height;
- Browser-owned Copy page address and Share page actions that disclose a userinfo-stripped HTTP(S) page address when explicit userinfo is present;
- scroll-aware top chrome that collapses after meaningful downward page scrolling and returns on upward scrolling, page-top return, omnibox focus, or new navigation when Android accessibility services are not active;
- top chrome that is kept visible while Android reports an accessibility service enabled and is restored immediately if accessibility becomes enabled while the Activity is active;
- Browser-owned Glaze menu presentation rather than the Android platform-default popup menu.

At normal text scale, the baseline expanded Browser-chrome budget is 128dp before Android system bars. This value is no longer a hard maximum: text-bearing top chrome may grow above the baseline when Android font scaling requires additional measured height. The collapsed scrolling state retains only the fixed 56dp bottom navigation toolbar and is allowed only while Android does not report an accessibility service enabled.

The accessibility visibility policy consumes only `AccessibilityManager.isEnabled` and its boolean state-change callback. It does not enumerate service identities, inspect accessibility event/user content, persist accessibility state, add permissions, or create telemetry/network behavior. The large-text layout change uses Android-native text/layout measurement and existing system font scaling and adds no new user-data processing. The RTL directionality slice uses Android manifest/vector/layout semantics only. The localization-resource slice moves Browser-owned display/accessibility copy into Android resources. The pseudolocale slice is a build-time debug resource-generation setting. The bidi-presentation slice changes only the condensed unfocused display string. The explicit-port slice uses the already parsed `URI.port` value only for condensed display. The navigation-authority slice performs local URI/authority syntax validation and host-shape routing only. The malformed-web handoff slice applies a local scheme/gesture decision and adds a source-wiring contract requiring `BrowserActivity` to consume that policy. The bare-userinfo routing slice changes only unschemed bare-host inference by refusing automatic host upgrade when the authority-shaped prefix contains literal `@`. The local-scheme handoff slice parses the local target scheme and keeps the bounded `about`/`blob`/`content`/`data`/`file`/`javascript` set inside Browser. The outward-disclosure slice locally parses page-address text and removes explicit HTTP(S) userinfo only from Browser clipboard/share output. None of these slices adds permissions, persistence, telemetry, network behavior, remote classification, or user-content collection.

The source contract records 2.0 font scale as the downstream large-text acceptance target. This source/runtime hardening is not representative 200% rendered acceptance, TalkBack, Switch Access, Voice Access, focus-order, announcement-quality, or physical-device acceptance. The RTL source contract likewise does not establish translated Browser copy, bidirectional address-field acceptance, rendered RTL layout, combined RTL + large-text behavior, or representative locale/device acceptance. The localization-resource source contract creates translation-ready resource boundaries only. The pseudolocale contract and APK badging proof establish debug test-resource generation only. The bidi/explicit-port presentation contracts prove bounded source behavior only. The navigation-authority, bare-userinfo routing, external-handoff, and outward-disclosure contracts prove bounded local syntax/routing/disclosure behavior only: none establish email detection semantics, complete URL/userinfo or credential safety, clipboard privacy, Android share-target trust, DNS validity, complete origin/Unicode/IDN/confusable/spoofing safety, certificate trust, complete scheme/intent/deep-link safety, trust in external applications, bidirectional editing acceptance, rendered address ordering, or representative locale/device acceptance.

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
- unfocused condensed address presentation strips Unicode bidi-formatting controls, preserves ordinary RTL letters and explicitly parsed ports, and does not modify the authoritative full URL exposed on focus;
- Browser-owned navigation resolution, bare-host inference, external-handoff routing, and outward-address disclosure apply their independent fail-closed/bounded contracts without turning Glaze presentation into security, origin, complete scheme/intent/deep-link, userinfo, credential, email, clipboard, or external-application authority;
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
- explicit no-action-bar, no-development-banner, Browser-owned-menu, accessibility-aware scroll-chrome, large-text content-height, RTL-directionality, localization-resource, debug-pseudolocale, bidi-address-presentation, explicit-port-presentation, navigation-authority, malformed-web external-handoff, bare-userinfo search-routing, local-scheme external-handoff, and userinfo-address-disclosure source contracts.

The inherited semantic-state ordering remains represented in the Browser contract. Disabled and error semantics continue to override lower-priority interaction presentation. V1.2 material/atmosphere cannot change security, privacy, identity, recovery, coordination, Search, or Sync truth.

This mapping is not native-device downstream acceptance. Production acceptance requires exact-revision rendered visual review, TalkBack/Switch Access/Voice Access and broader accessibility evidence, rendered 200% text behavior, contrast/high-contrast behavior, Reduced Motion, Reduced Transparency/effects-free behavior, actual translated/localized copy and translation-quality acceptance, locale fallback/grammar behavior, rendered pseudolocale stress review, complete DNS/origin/Unicode/IDN/confusable review, complete URL/userinfo/credential-disclosure and scheme/intent/deep-link/external-handoff review, bidirectional text/address editing behavior, rendered RTL directionality, responsive/form-factor behavior, performance, Touch Assistance mapping where supported, and representative physical-device evidence.

## Security boundary

Wardveil Security is the authoritative GoreeCloud security system. Android Browser must not manufacture Wardveil status.

Current Android beta security behavior includes malformed explicit HTTP(S) authority fail-closed handling, malformed HTTP(S) page-link externalization blocking, unschemed bare-userinfo inference rejection, bounded local/active-content scheme externalization blocking, outward HTTP(S) userinfo disclosure reduction for Browser-owned Copy/Share actions, TLS fail-closed handling, Android Safe Browsing, mixed-content blocking, disabled WebView file/content access, default-denied site permissions, and a blocked download path until Android can satisfy the Browser-to-Wardveil release contract.

Malformed explicit HTTP(S) input fails to GoreeCloud Search Home instead of being forwarded to WebView or converted to a search query. A rejected HTTP(S) page-link attempt also remains Browser-owned instead of becoming eligible for Android `ACTION_VIEW` solely because a user gesture occurred. Unschemed email-shaped input containing literal `@` is not silently upgraded into an HTTPS URL containing userinfo. Known local/active-content targets using `about:`, `blob:`, `content:`, `data:`, `file:`, or `javascript:` remain Browser-owned even when the page request has a user gesture. Explicit HTTP(S) userinfo may still exist in valid navigation state, but Browser-owned Copy page address and Share page remove it before outward disclosure. These protect bounded Browser-owned routing/disclosure boundaries but are not email validation, complete URL/userinfo or credential safety, phishing protection, clipboard privacy, Android share-target trust, DNS validation, certificate verification, origin classification, IDN/confusable analysis, complete scheme/intent/deep-link safety, external-application trust, or Wardveil evidence.

Android System WebView remains responsible for engine/platform security mechanisms; Wardveil remains responsible for GoreeCloud security evaluation, protection, evidence, and response where integrated.

The address presentation hardening is not a complete anti-spoofing, origin-safety, or URL-safety system. It removes a bounded set of Unicode bidi-formatting controls and preserves explicitly parsed ports only in the condensed unfocused presentation. It does not rewrite navigation URLs, establish IDN/confusable safety, fully qualify origin presentation, or manufacture Wardveil trust.

## Privacy boundary

Privacy Shield is the authoritative privacy and data-use governance system. The beta currently uses privacy-protective defaults including third-party-cookie blocking and denied permission/geolocation requests.

The accessibility chrome policy uses only Android's boolean accessibility-enabled state and does not enumerate services or inspect accessibility event/user content. The large-text change relies only on Android-native layout/font metrics. The RTL directionality change is limited to Android source/resource semantics. The localization-resource slice moves Browser-owned UI copy between application source/resource files. The pseudolocale slice adds generated debug resources only. The bidi-presentation slice transforms only the local condensed display string. The explicit-port slice reads the already parsed local `URI.port` value for that same display. The navigation-authority slice performs local URI/authority parsing only; malformed explicit HTTP(S) input fails to Search Home specifically so it is not silently disclosed as GoreeCloud Search query text. The malformed-web external-handoff slice keeps rejected HTTP(S) link attempts inside Browser so they are not disclosed to another application through `ACTION_VIEW` merely because a user gesture occurred. The bare-userinfo routing slice performs local string classification only: unschemed text containing literal `@` is not automatically converted into an HTTPS userinfo navigation target and instead follows the existing non-URL GoreeCloud Search path. The local-scheme external-handoff slice performs local URI scheme classification only and prevents the bounded `about`/`blob`/`content`/`data`/`file`/`javascript` set from being disclosed to another app through `ACTION_VIEW`; eligible external schemes remain gesture-gated. The outward-address disclosure slice performs only local URL parsing/text reconstruction: Browser-owned Copy/Share actions remove explicit HTTP(S) userinfo before sending the page address to Android clipboard/share surfaces while focused editing/navigation retain the exact URL. These changes add no telemetry, service discovery, persisted accessibility/locale preference state, remote calls, new user-content collection, or new authority. They are bounded source/build behaviors, not complete Privacy Shield acceptance.

The native session-recovery core excludes Private and Isolated Private windows before persistence. This is a source-level privacy invariant, not complete private-browsing runtime acceptance.

Complete Privacy Shield consent, filtering, private-browsing, diagnostics, policy, clipboard/share-target privacy, and user-control acceptance remains pending.

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
- complete DNS/origin/Unicode/IDN/confusable review and bidirectional URL/address editing acceptance;
- complete URL/userinfo/credential-disclosure, clipboard/share-target, scheme/intent/deep-link, and external-application handoff acceptance;
- TalkBack, Switch Access, Voice Access, accessibility focus/announcement quality, rendered RTL behavior, and rendered 200% text-scaling acceptance across representative locales;
- Touch Assistance runtime mapping where the supported Android scope requires it;
- upgrade/downgrade/data-migration acceptance;
- release provenance and operational recovery evidence.

A lower acceptance state must never be represented as a higher one.