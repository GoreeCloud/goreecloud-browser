# GoreeCloud Browser Specifications

## Product

GoreeCloud Browser is an original GoreeCloud-owned native web browser. GoreeCloud owns the product experience, application architecture, browser chrome, state models, navigation/search behavior, privacy/security integrations, synchronization behavior, and service boundaries. Mature rendering engines may be used as narrowly scoped technical foundations.

## Current lifecycle

- Browser channel: `0.1.0-beta.1`
- Production approved: No
- Stable: No
- Current mandatory Glaze UI target: `2.2.0`
- Glaze UI Stable release revision: `6731098b28dd0393faa878c70d989a221d714a20`
- Android Glaze UI 2.2 source mapping: migration candidate on `+android.5`; native-device acceptance pending
- Canonical source repository: `GoreeCloud/goreecloud-browser`

## Android beta

- User-facing identity: GoreeCloud Browser Beta
- Debug-beta package: `io.goreecloud.browser.beta`
- Debug-beta version on this migration branch: `0.1.0-beta.1+android.5`
- Android versionCode: `10005`
- Minimum Android: API 26
- Compile/target API: 35
- Java/Kotlin target: 17
- Rendering runtime: Android System WebView/Chromium as an engine dependency
- Beta signing: Android debug signing for fresh-install testing
- Production signing material: not stored in source control and not yet accepted

The Android beta must remain installable without implying production readiness. CI validates unit tests, Android lint, APK assembly, APK signature/package identity, checksum generation, and artifact upload.

## Native session recovery contract

The engine-independent native core includes a source-level session-recovery checkpoint/candidate contract for normal Browser windows.

The current contract:

- models normal, Private, and Isolated Private window privacy modes but persists only normal-window state;
- deliberately exposes no policy switch that can enable private-window checkpoint persistence;
- requires every accepted checkpoint to have a non-empty checkpoint identifier and nonzero capture timestamp;
- sanitizes checkpoint windows before handing state to a persistence implementation;
- distinguishes running, clean-shutdown, and unclean-shutdown checkpoint states;
- treats a newest running or unclean normal-window checkpoint as a recovery candidate;
- never searches past the newest checkpoint for an older crash candidate, preventing stale crash state from silently resurfacing after a newer clean or private-only checkpoint;
- masks a newest checkpoint containing only private state into no recoverable candidate;
- supports explicit checkpoint discard through the abstract recovery-store boundary.

This is a tested source contract only. The repository does not yet claim durable authenticated-encrypted recovery persistence on the current mainline, platform-backed recovery-key protection, complete application lifecycle checkpoint wiring, restore execution, user-facing Glaze recovery UI, Everkeep recovery acceptance, or production recovery behavior.

## First-party service capability consumer contract

Authoritative Browser main contains a fail-closed first-party capability consumer boundary integrated through PR #16 as `974786cdccceac7a0198881d4bd5f4e5d4b28c58` from exact validated source `0de830edfdc21fe77de33bf5ec00986510ce4dc2`.

A capability is usable through this generic Browser gate only when the service is available and exactly one matching capability record is present, versioned, current, authoritative, explicitly production-accepted, and—when requested—an exact contract-version match. Duplicate or ambiguous evidence, unversioned records, stale or non-authoritative evidence, wrong capability/version, degraded services, and pre-Stable producer evidence fail closed.

The consumer boundary does not manufacture producer authority, automatically wire every first-party service adapter, establish a live production integration, or qualify Browser as Stable.

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
- page-load progress overlaid on web content rather than allocated its own chrome row;
- unfocused address presentation that removes the scheme while keeping the hostname at the leading edge;
- complete URL exposure and selection when the omnibox receives focus;
- scroll-aware top chrome that collapses after meaningful downward page scrolling and returns on upward scrolling, page-top return, omnibox focus, or new navigation;
- Browser-owned Glaze menu presentation rather than the Android platform-default popup menu.

Expanded fixed Browser chrome is 128dp before Android system bars. The collapsed scrolling state retains only the 56dp bottom navigation toolbar.

## Android Glaze UI contract

The current migration branch maps Browser-owned Android chrome to Glaze UI 2.2.0 Stable and records Stable release revision `6731098b28dd0393faa878c70d989a221d714a20` plus accepted Glaze UI visual source `0411b0f6dd877aea30e2c5674e1acde0105fd97b`.

The prior authoritative `+android.4` / main `974786cdccceac7a0198881d4bd5f4e5d4b28c58` mapping targeted Glaze UI 2.0.0. That remains historical evidence and rollback input, not current-Stable conformance.

The Android shell maps native controls to:

- Canvas for the root application background and top chrome;
- Soft Glaze for the omnibox and Browser menu sheet;
- Surface for the address-field role, bottom navigation, and Browser menu actions;
- Application scope in the Glaze UI 2.2 System Shell hierarchy; Browser-owned menu/search chrome does not claim Universal Search, Control Center, System Panel, or Critical System authority;
- Calm expression and Balanced clarity for this beta shell;
- a 48dp minimum effective target for ordinary touch-oriented interactive controls;
- a recorded 56dp target floor for Touch Assistance/far-view contexts where applicable, without claiming an unwired Android Touch Assistance authority mapping;
- a bounded ordinary composition of at most one dominant Glaze panel and up to three small floating Glaze controls;
- Glaze UI 2.2 interaction-state priority with disabled state above error, pressed, focus, selected, hover, and rest;
- semantic native labels and focus/pressed states;
- effects-free operation without requiring blur/transparency;
- native light/dark appearance adaptation;
- Browser-owned vector icons;
- explicit no-action-bar and no-development-banner normal-chrome contracts;
- an explicit prohibition on using the platform-default `PopupMenu` as the Browser menu surface;
- an explicit scroll-aware top-chrome contract.

This source mapping is not equivalent to native-device Glaze acceptance. Production acceptance requires exact-revision rendered visual review, accessibility/TalkBack evidence, 200% text, contrast, Reduced Motion, Reduced Transparency/effects-free behavior, applicable forced-color/high-contrast behavior, RTL/localization, input, responsive/form-factor, performance, Touch Assistance mapping where supported, and representative real-device evidence.

## Security boundary

Wardveil Security is the authoritative GoreeCloud security system. Android Browser must not manufacture Wardveil status.

Current Android beta security behavior includes TLS fail-closed handling, Android Safe Browsing, mixed-content blocking, disabled WebView file/content access, default-denied site permissions, and a blocked download path until Android can satisfy the Browser-to-Wardveil release contract.

Android System WebView remains responsible for its engine/platform security mechanisms; Wardveil remains responsible for GoreeCloud security evaluation, protection, evidence, and response where integrated.

## Privacy boundary

Privacy Shield is the authoritative privacy and data-use governance system. The beta currently uses privacy-protective defaults including third-party-cookie blocking and denied permission/geolocation requests.

The native session-recovery core also excludes Private and Isolated Private windows before persistence. This exclusion is a source-level privacy invariant, not a claim that the complete private-browsing runtime has reached production acceptance.

Complete Privacy Shield consent, filtering, private-browsing, diagnostics, policy, and user-control acceptance remains pending.

## Continuity boundary

Everkeep is the authoritative resilience, recovery, preservation, portability, and continuity system. The native session-recovery core establishes a Browser-owned checkpoint/candidate boundary that future Everkeep integration can consume, but durable recovery storage, lifecycle integration, restoration, backup, portability, and migration acceptance remain pending.

## Identity and integration boundaries

GoreeCloud Identity is authoritative for identity/authentication/authorization. GoreeCloud Mesh is authoritative for platform coordination and capability discovery. GoreeCloud Search is the sole approved integrated query authority for Browser search entry points. GoreeCloud Vault, Sync, DNS, Network, Bookmarks, Wayfinder, and other GoreeCloud services remain separate capability authorities according to their contracts.

## Production promotion blockers

At minimum, Stable Android promotion remains blocked by:

- controlled signing and update/rollback key operations;
- complete Glaze UI 2.2 native-device visual/accessibility/form-factor acceptance on the exact Browser adoption revision;
- authenticated Wardveil download verification/release integration;
- accepted Privacy Shield runtime integration;
- accepted Everkeep recovery/continuity integration;
- private-browsing isolation and Close & Forget evidence;
- Browser-owned permission workflows;
- required Identity/Vault/Sync/DNS/Network/Mesh adapters;
- representative supported-device testing;
- accessibility, RTL/localization, and text-scaling acceptance;
- Touch Assistance / 56dp runtime mapping where the supported Android product scope requires it;
- upgrade/downgrade/data-migration acceptance;
- release provenance and operational recovery evidence.

A lower acceptance state must never be represented as a higher one.
