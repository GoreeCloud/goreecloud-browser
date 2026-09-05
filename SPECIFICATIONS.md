# GoreeCloud Browser Specifications

## Product

GoreeCloud Browser is an original GoreeCloud-owned native web browser. GoreeCloud owns the product experience, application architecture, browser chrome, state models, navigation/search behavior, privacy/security integrations, synchronization behavior, and service boundaries. Mature rendering engines may be used as narrowly scoped technical foundations.

## Current lifecycle

- Browser channel: `0.1.0-beta.1`
- Production approved: No
- Stable: No
- Current mandatory GLAZE UI target: `1.1.0`
- GLAZE UI Stable tag: `v1.1.0`
- GLAZE UI Stable release revision: `15cc76d2bcd4065552dc31c77145b63f34d9e7b2`
- Approved V1.1 visual source: `8ea1f789bbabf943c3359514dc1506b24fa3c51b`
- Android V1.1 source mapping: migration candidate on `+android.6`; downstream native-device acceptance pending
- Canonical source repository: `GoreeCloud/goreecloud-browser`

## Android beta

- User-facing identity: GoreeCloud Browser Beta
- Debug-beta package: `io.goreecloud.browser.beta`
- Debug-beta version on this migration branch: `0.1.0-beta.1+android.6`
- Android versionCode: `10006`
- Minimum Android: API 26
- Compile/target API: 35
- Java/Kotlin target: 17
- Rendering runtime: Android System WebView/Chromium as an engine dependency
- Beta signing: Android debug signing for fresh-install testing
- Production signing material: not stored in source control and not yet accepted

The Android beta must remain installable without implying production readiness. CI validates unit tests, Android lint, APK assembly, APK signature/package/version identity, checksum generation, and artifact upload.

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

- a 56dp top omnibox inside an 8dp top/bottom chrome gutter;
- a 56dp persistent bottom navigation toolbar;
- full-width web content between those regions;
- Browser-owned vector controls for Back, Forward, Search Home, Reload/Stop, and Browser menu;
- page-load progress overlaid on web content;
- unfocused address presentation that removes the scheme while keeping the hostname at the leading edge;
- complete URL exposure and selection when the omnibox receives focus;
- scroll-aware top chrome that collapses after meaningful downward page scrolling and returns on upward scrolling, page-top return, omnibox focus, or new navigation;
- Browser-owned Glaze menu presentation rather than the Android platform-default popup menu.

Expanded fixed Browser chrome is 128dp before Android system bars. The collapsed scrolling state retains only the 56dp bottom navigation toolbar.

## GLAZE UI V1.1 Android contract

This migration maps Browser-owned Android chrome to the sole current Stable consumer target, GLAZE UI V1.1 (`1.1.0`). The source records Stable release revision `15cc76d2bcd4065552dc31c77145b63f34d9e7b2`, approved V1.1 visual source `8ea1f789bbabf943c3359514dc1506b24fa3c51b`, and optical contract `contracts/v1.1/optical-refinement.json`.

Browser preserves the working mobile-shell structure while applying the current authority contract:

- Canvas for the root background and top chrome;
- Soft Glaze for the omnibox and Browser menu sheet;
- Surface for the address-field role and bottom navigation;
- Application System Shell scope; Browser menu/search chrome does not claim Universal Search, Control Center, System Panel, or Critical System authority;
- Calm expression and Balanced clarity;
- 48dp ordinary interaction floor;
- 56dp Touch Assistance floor where applicable, without claiming an unwired Android preference/OS mapping;
- at most one dominant Glaze panel plus three small floating Glaze controls;
- Light, Dark, and Deep Dark structural appearance targets;
- upper-left optical light direction;
- bounded Deep Teal + Soft Amber atmosphere subordinate to neutral structure and semantic/accessibility authority;
- no nested backdrop blur;
- no required Environmental Color Memory, environmental-content sampling, or remote color derivation;
- semantic native labels and visible focus/state treatment;
- effects-free operation without requiring blur/transparency;
- Browser-owned vector icons;
- explicit no-action-bar, no-development-banner, Browser-owned-menu, and scroll-aware top-chrome contracts.

The inherited semantic-state ordering remains represented in the Browser contract. Disabled and error semantics continue to override lower-priority interaction presentation. V1.1 atmosphere cannot change security, privacy, identity, recovery, coordination, Search, or Sync truth.

This mapping is not native-device downstream acceptance. Production acceptance requires exact-revision rendered visual review, TalkBack/accessibility evidence, 200% text, contrast/high-contrast behavior, Reduced Motion, Reduced Transparency/effects-free behavior, RTL/localization, responsive/form-factor behavior, performance, Touch Assistance mapping where supported, and representative physical-device evidence.

## Security boundary

Wardveil Security is the authoritative GoreeCloud security system. Android Browser must not manufacture Wardveil status.

Current Android beta security behavior includes TLS fail-closed handling, Android Safe Browsing, mixed-content blocking, disabled WebView file/content access, default-denied site permissions, and a blocked download path until Android can satisfy the Browser-to-Wardveil release contract.

Android System WebView remains responsible for engine/platform security mechanisms; Wardveil remains responsible for GoreeCloud security evaluation, protection, evidence, and response where integrated.

## Privacy boundary

Privacy Shield is the authoritative privacy and data-use governance system. The beta currently uses privacy-protective defaults including third-party-cookie blocking and denied permission/geolocation requests.

The native session-recovery core excludes Private and Isolated Private windows before persistence. This is a source-level privacy invariant, not complete private-browsing runtime acceptance.

Complete Privacy Shield consent, filtering, private-browsing, diagnostics, policy, and user-control acceptance remains pending.

## Continuity boundary

Everkeep is authoritative for resilience, recovery, preservation, portability, and continuity. The native session-recovery core establishes a Browser-owned checkpoint/candidate boundary that future Everkeep integration can consume, but durable recovery storage, lifecycle integration, restoration, backup, portability, and migration acceptance remain pending.

## Identity and integration boundaries

GoreeCloud Identity is authoritative for identity/authentication/authorization. GoreeCloud Mesh is authoritative for platform coordination and capability discovery. GoreeCloud Search is the sole approved integrated query authority for Browser search entry points. GoreeCloud Vault, Sync, DNS, Network, Bookmarks, Wayfinder, and other GoreeCloud services remain separate capability authorities according to their contracts.

## Production promotion blockers

At minimum, Stable Android promotion remains blocked by:

- controlled signing and update/rollback key operations;
- complete GLAZE UI V1.1 native-device visual/accessibility/form-factor acceptance on the exact Browser revision;
- authenticated Wardveil download verification/release integration;
- accepted Privacy Shield runtime integration;
- accepted Everkeep recovery/continuity integration;
- private-browsing isolation and Close & Forget evidence;
- Browser-owned permission workflows;
- required Identity/Vault/Sync/DNS/Network/Mesh/Search adapters with accepted producer evidence;
- representative supported-device testing;
- accessibility, RTL/localization, and text-scaling acceptance;
- Touch Assistance runtime mapping where the supported Android scope requires it;
- upgrade/downgrade/data-migration acceptance;
- release provenance and operational recovery evidence.

A lower acceptance state must never be represented as a higher one.
