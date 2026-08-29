# GoreeCloud Browser Specifications

## Product

GoreeCloud Browser is an original GoreeCloud-owned native web browser. GoreeCloud owns the product experience, application architecture, browser chrome, state models, navigation/search behavior, privacy/security integrations, synchronization behavior, and service boundaries. Mature rendering engines may be used as narrowly scoped technical foundations.

## Current lifecycle

- Browser channel: `0.1.0-beta.1`
- Production approved: No
- Stable: No
- Current mandatory Glaze UI target: `2.0.0`
- Canonical source repository: `GoreeCloud/goreecloud-browser`

## Android beta

- User-facing identity: GoreeCloud Browser Beta
- Debug-beta package: `io.goreecloud.browser.beta`
- Debug-beta version: `0.1.0-beta.1+android.1`
- Minimum Android: API 26
- Compile/target API: 35
- Java/Kotlin target: 17
- Rendering runtime: Android System WebView/Chromium as an engine dependency
- Beta signing: Android debug signing for fresh-install testing
- Production signing material: not stored in source control and not yet accepted

The Android beta must remain installable without implying production readiness. CI validates unit tests, Android lint, APK assembly, APK signature/package identity, checksum generation, and artifact upload.

## Android navigation contract

The unified address/search field resolves input according to Browser-owned policy:

1. Empty input opens GoreeCloud Search home.
2. Explicit HTTP/HTTPS URLs navigate directly.
3. Bare host input is upgraded to HTTPS.
4. Other text is sent only to GoreeCloud Search.
5. Non-web schemes are not accepted as ordinary Browser web navigation.

## Android Glaze UI contract

The current source mapping targets Glaze UI 2.0.0 Stable and records the canonical promotion revision `ff3fff4306bd53ea9c0715a7c0d64265bb038617`.

The Android shell maps native controls to:

- Canvas for the root application background;
- Soft Glaze for bounded status presentation;
- Glaze for the Navigation Capsule;
- Surface for the unified address/search field;
- Calm expression and Balanced clarity for this beta shell;
- a 48dp minimum effective target for general interactive controls;
- semantic native labels and focus/pressed states;
- effects-free operation without requiring blur/transparency;
- native light/dark appearance adaptation.

This source mapping is not equivalent to native-device Glaze acceptance. Production acceptance requires exact-revision visual, accessibility, text-scaling, contrast, input, responsive/form-factor, performance, and representative real-device evidence.

## Security boundary

Wardveil Security is the authoritative GoreeCloud security system. Android Browser must not manufacture Wardveil status.

Current Android beta security behavior includes TLS fail-closed handling, Android Safe Browsing, mixed-content blocking, disabled WebView file/content access, default-denied site permissions, and a blocked download path until Android can satisfy the Browser-to-Wardveil release contract.

Android System WebView remains responsible for its engine/platform security mechanisms; Wardveil remains responsible for GoreeCloud security evaluation, protection, evidence, and response where integrated.

## Privacy boundary

Privacy Shield is the authoritative privacy and data-use governance system. The beta currently uses privacy-protective defaults including third-party-cookie blocking and denied permission/geolocation requests.

Complete Privacy Shield consent, filtering, private-browsing, diagnostics, policy, and user-control acceptance remains pending.

## Continuity boundary

Everkeep is the authoritative resilience, recovery, preservation, portability, and continuity system. The Android beta has not yet completed Everkeep backup/recovery or application-state migration acceptance.

## Identity and integration boundaries

GoreeCloud Identity is authoritative for identity/authentication/authorization. GoreeCloud Mesh is authoritative for platform coordination and capability discovery. GoreeCloud Search is the sole approved integrated query authority for Browser search entry points. GoreeCloud Vault, Sync, DNS, Network, Bookmarks, Wayfinder, and other GoreeCloud services remain separate capability authorities according to their contracts.

## Production promotion blockers

At minimum, Stable Android promotion remains blocked by:

- controlled signing and update/rollback key operations;
- complete Glaze UI 2.0 native-device acceptance;
- authenticated Wardveil download verification/release integration;
- accepted Privacy Shield runtime integration;
- accepted Everkeep recovery/continuity integration;
- private-browsing isolation and Close & Forget evidence;
- Browser-owned permission workflows;
- required Identity/Vault/Sync/DNS/Network/Mesh adapters;
- representative supported-device testing;
- accessibility and text-scaling acceptance;
- upgrade/downgrade/data-migration acceptance;
- release provenance and operational recovery evidence.

A lower acceptance state must never be represented as a higher one.
