# GoreeCloud Browser

GoreeCloud Browser is GoreeCloud's first-party native web browser. GoreeCloud owns the browser product layer, user experience, data model, privacy controls, security services, synchronization behavior, session model, and first-party integrations. Mature web engines remain implementation dependencies rather than the product identity.

## Current status

GoreeCloud Browser is on the **0.1.0-beta.1** development channel and is **not production-approved or Stable**.

The repository contains build-tested engine-independent Browser core, a Linux GTK/X11 native shell path, and a real installable Android beta target. The Android V1.1 migration candidate is:

- package: `io.goreecloud.browser.beta`
- version: `0.1.0-beta.1+android.6`
- versionCode: `10006`
- minimum Android API: 26
- target Android API: 35

Android System WebView/Chromium is used only as the rendering engine dependency. GoreeCloud owns the mobile chrome, URL/search routing, privacy defaults, permission behavior, design-system mapping, and product capability gates. Downloads remain fail-closed until the Android path satisfies the authoritative Wardveil release contract.

Passing source CI or producing an installable APK does not establish production readiness. Real-runtime, security, privacy, accessibility, current-Stable GLAZE UI, packaging/signing, recovery, compatibility, representative-device, and sustained-use acceptance remain separate gates.

## GLAZE UI requirement

GoreeCloud Browser must track the **current approved Stable GLAZE UI release**. The sole current consumer target is **GLAZE UI V1.1 (`1.1.0`)**:

- tag: `v1.1.0`
- Stable release revision: `15cc76d2bcd4065552dc31c77145b63f34d9e7b2`
- approved visual source: `8ea1f789bbabf943c3359514dc1506b24fa3c51b`
- optical contract: `contracts/v1.1/optical-refinement.json`

This branch explicitly migrates Browser away from its historical Glaze UI 2.2 mapping. The working mobile shell is preserved rather than rewritten for version churn, while Browser's source contract is reconciled to V1.1 authority.

The Browser Android mapping records the V1.1 rules applicable to the current shell:

- solid reading/critical-decision surfaces and glazed transient interaction chrome;
- Application-scope System Shell authority without claiming Universal Search, Control Center, System Panel, or Critical System authority;
- one dominant Glaze panel plus at most three small floating Glaze controls;
- 48dp ordinary interaction targets and 56dp Touch Assistance targets where applicable;
- Light, Dark, and Deep Dark structural appearances;
- upper-left optical lighting and bounded Deep Teal + Soft Amber atmosphere over neutral structure;
- no nested backdrop-blur stack;
- no required Environmental Color Memory or environmental-content sampling;
- effects-free operation before semantic/accessibility degradation.

Presentation never manufactures Wardveil security truth, Privacy Shield privacy truth, Everkeep continuity truth, Identity authorization, Mesh coordination, Search authority, Sync authority, or Browser workflow success.

This is repository-local source mapping only. Browser still requires exact-revision rendered/native visual review, TalkBack/accessibility, 200% text, RTL/localization, Reduced Motion, Reduced Transparency/effects-free behavior, contrast/high-contrast handling, Touch Assistance mapping where supported, form-factor behavior, performance, representative physical-device evidence, and product-specific production approval.

See [`docs/GLAZE_UI_ADOPTION.md`](docs/GLAZE_UI_ADOPTION.md).

## Product architecture

GoreeCloud Browser is not intended to be a conventional Chromium skin or a permanently deep Chromium fork. GoreeCloud-owned application layers are kept behind explicit engine and service boundaries so engine implementations can evolve without becoming the canonical Browser product model.

### Core systems

- **GoreeCloud Browser** — native browser application, chrome, tabs/windows, sessions, profiles, preferences, history, permissions, downloads, and Browser-owned state.
- **Browser Engine Layer** — GoreeCloud-owned interface between Browser application logic and rendering/runtime engines.
- **Chromium/Blink / Android System WebView** — initial rendering, JavaScript, Web APIs, media, graphics, accessibility, and web-platform foundations.
- **GLAZE UI** — authoritative presentation and interaction design system for Browser-owned surfaces.
- **GoreeCloud Search** — sole integrated web/current-information query authority for approved Browser search entry points.
- **GoreeCloud Bookmarks** — bookmarks, read-later, collections, and library subsystem.
- **GoreeCloud Sync** — synchronization for approved Browser state while excluding indiscriminate sensitive-site state.
- **Privacy Shield** — privacy and data-use authority.
- **Wardveil Security** — security classification, scanning, quarantine coordination, and evidence authority.
- **Everkeep** — backup, recovery, portability, and long-term preservation authority.
- **GoreeCloud Identity** — profiles, device authorization, account-bound preferences, and identity integration.
- **GoreeCloud Vault** — credentials, passkeys, secrets, and secure autofill authority.
- **GoreeCloud DNS / GoreeCloud Network** — name-resolution and network/private-connectivity authorities.
- **GoreeCloud Mesh** — first-party coordination and capability governance.

## Implemented source foundations

The repository currently includes, among other development foundations:

- native Browser core and engine abstraction;
- Android installable beta shell with Browser-owned navigation/search behavior;
- fail-closed first-party service capability acceptance boundary;
- Browser-owned GoreeCloud Sync envelope/retrieval contracts with bounded pagination, record-ID limits, privacy-safe tombstones, and signer-shape validation;
- Advanced Download Manager staging with a source-level Wardveil release gate;
- privacy-safe session-recovery checkpoint/candidate core that excludes Private and Isolated Private windows and validates bounded topology;
- Media Hover and Wayfinder product contracts.

A source foundation is not the same as deployed producer connectivity or production acceptance. Search, Sync, Identity, Mesh, Privacy Shield, Wardveil Security, Everkeep, DNS, Network, Vault, Bookmarks, and other first-party systems retain independent authority and lifecycle evidence.

## Android beta behavior

The current Android beta provides Browser-owned Back, Forward, Home, Reload/Stop, a unified address/search field, page progress, direct HTTP/HTTPS navigation, bare-host HTTPS upgrade, GoreeCloud Search routing for non-URL input, browser-intent handling for web links, Browser-owned menu actions, and scroll-aware top chrome.

Security/privacy defaults remain deliberately conservative:

- TLS certificate errors fail closed;
- Android Safe Browsing is enabled;
- mixed content is disabled;
- third-party cookies are disabled by default;
- WebView file/content access is disabled;
- website permissions and geolocation remain default-deny until Browser-owned policy surfaces are accepted;
- Android downloads remain blocked until Wardveil release requirements are satisfied.

Private Browsing, full tab-management UI, accepted permission surfaces, production Wardveil downloads, full first-party runtime integrations, managed updates/rollback, production signing, store distribution, and Stable qualification remain incomplete.

## First-party capability boundary

Browser authoritative source includes a fail-closed generic consumer contract. A first-party capability is usable through that gate only when exactly one matching evidence record is present and is versioned, current, authoritative, available, explicitly production-accepted, and contract-compatible when an exact version is required.

Service reachability or an optimistic consumer assumption is not authorization. Browser does not create or strengthen producer authority.

## Wardveil download security

The Advanced Download Manager has a source-level Wardveil Scan release gate. Completed transfer bytes remain in Browser staging until authoritative, current, correctly scoped clean evidence is bound to the exact staged SHA-256 digest and the staged bytes are re-hashed before publication.

Suspicious content is held; malicious or unverifiable content remains blocked. Browser can hand malicious content to authorized Wardveil Quarantine handling but does not manufacture quarantine truth itself and never connects directly to ClamAV. Production malware-protection acceptance still requires authenticated live integration, deployed scanner/signature health, controlled runtime tests, safe staging publication, and accepted target-platform behavior.

## Development principles

1. GoreeCloud owns the Browser product layer.
2. Rendering engines are replaceable infrastructure, not product identity.
3. GoreeCloud-specific behavior belongs in explicit application/service boundaries rather than indiscriminate engine patches.
4. Privacy Shield, Wardveil Security, Everkeep, GLAZE UI, Mesh, Identity, Search, Sync, DNS, Network, Vault, and Bookmarks retain independent authority.
5. Browser state uses GoreeCloud-owned models and contracts where Browser is authoritative.
6. Security updates from underlying engines must remain practical to consume.
7. Planned capabilities are never presented as implemented or production-ready without evidence.
8. Browser-owned user-facing surfaces must satisfy the latest approved Stable GLAZE UI release before production acceptance.

## Repository documentation

- [`SPECIFICATIONS.md`](SPECIFICATIONS.md) — product/source specification and lifecycle boundaries.
- [`FEATURES.md`](FEATURES.md) — implemented and incomplete functionality.
- [`BENEFITS.md`](BENEFITS.md) — supportable product value.
- [`COMPETITIVE-OBJECTIVES.md`](COMPETITIVE-OBJECTIVES.md) — differentiation objectives.
- [`USER-MANUAL.md`](USER-MANUAL.md) — current user-facing beta behavior.
- [`docs/ANDROID_BETA.md`](docs/ANDROID_BETA.md) — Android package, behavior, restrictions, and acceptance gates.
- [`docs/GLAZE_UI_ADOPTION.md`](docs/GLAZE_UI_ADOPTION.md) — current-Stable design-system adoption contract.
- [`docs/BETA_0_1.md`](docs/BETA_0_1.md) — beta scope and runtime gates.
- [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) — architecture and ownership boundaries.
- [`docs/ENGINE_LAYER.md`](docs/ENGINE_LAYER.md) — engine abstraction contracts.
- [`docs/WARDVEIL_DOWNLOAD_SCANNING.md`](docs/WARDVEIL_DOWNLOAD_SCANNING.md) — download security boundary.

## Status

GoreeCloud Browser remains **Development / Beta**. The V1.1 migration candidate does not become conformant, Stable, production-signed, deployed, or production-approved merely by merging source changes. Those states require their own verified evidence.
