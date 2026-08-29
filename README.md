# GoreeCloud Browser

GoreeCloud Browser is GoreeCloud's first-party native web browser. The browser product, user experience, data model, privacy controls, security services, synchronization behavior, session model, and GoreeCloud integrations are owned by GoreeCloud. A mature web engine is used underneath as an implementation dependency rather than as the identity of the product.

## Beta status

The repository is on the **0.1.0-beta.1** development channel. This is a build-tested GoreeCloud Browser **source and native-shell beta milestone**. The engine-independent core and Linux GTK/X11 shell have CI build gates. The CEF/Chromium render-capable desktop path remains under integration and must pass the real-runtime gates in [`docs/BETA_0_1.md`](docs/BETA_0_1.md) before a desktop artifact is described as a render-capable beta.

The repository also includes a **real installable Android beta target**. The Android CI path builds the debug-signed package `io.goreecloud.browser.beta` as version `0.1.0-beta.1+android.2` for Android API 26 and newer, targeting API 35. It runs unit tests, Android lint, APK assembly, signature verification, package/label verification, SHA-256 generation, and GitHub Actions artifact upload. Android System WebView/Chromium is used only as the web-engine dependency; GoreeCloud owns the Android browser chrome, navigation/search policy, privacy defaults, permission behavior, Glaze UI mapping, and beta capability gates. Downloads remain fail-closed until the Android path can satisfy the existing Wardveil release contract. See [`docs/ANDROID_BETA.md`](docs/ANDROID_BETA.md).

The `+android.2` shell has a **source-level Glaze UI 2.0.0 native mapping candidate** using native Android controls, current material semantics, a 48dp general target floor, light/dark adaptation, focus/pressed states, and effects-free fallback behavior. That mapping is not yet native-device Glaze acceptance; representative visual, accessibility, form-factor, and real-device evidence remains required.

Beta does **not** mean production-approved. Stable release acceptance still requires real runtime, security, privacy, accessibility, packaging, production signing, recovery, compatibility, and sustained-use evidence. The Android APK is an installable test beta, not a production-signed or Stable artifact.

## Product identity

GoreeCloud Browser is the sole normal user-facing Browser product identity. Upstream and third-party implementation technology may appear where technically, legally, operationally, diagnostically, or compliance-wise required, but it does not become GoreeCloud product branding.

## Glaze UI requirement

GoreeCloud Browser is built against the **latest approved Stable Glaze UI release**. The current Stable baseline is **Glaze UI 2.0.0**, promoted from `GoreeCloud/goreecloud-glaze-ui` at source revision `ff3fff4306bd53ea9c0715a7c0d64265bb038617`.

This is a moving production requirement rather than a permanent version pin. When a newer Glaze UI release becomes Stable, GoreeCloud Browser becomes migration-required until Browser-owned surfaces have adopted and validated that current Stable contract. A superseded Glaze UI baseline cannot satisfy Browser production readiness. Source mapping alone also does not establish product-specific native or real-device acceptance.

See [`docs/GLAZE_UI_ADOPTION.md`](docs/GLAZE_UI_ADOPTION.md) for the Browser-specific adoption and validation policy.

## Architectural direction

GoreeCloud Browser is **not** intended to be a conventional Chromium skin or a permanently deep Chromium fork.

The browser is structured so that GoreeCloud-owned application layers remain separated from the underlying rendering engine behind explicit interfaces. Chromium/Blink is the initial engine foundation, while the architecture leaves room for platform-native engines, alternative engines, and a future GoreeCloud-controlled engine or fork when there is a concrete technical reason to do so.

### Core systems

- **GoreeCloud Browser** — native browser application, browser chrome, tab model, profiles, preferences, history, permissions, downloads, sessions, and product experience.
- **Browser Engine Layer** — GoreeCloud-owned abstraction between the application and the underlying web engine.
- **Chromium/Blink** — initial rendering, JavaScript, WebAssembly, Web APIs, media, graphics, accessibility, and web-platform foundation. On Android this role is provided through Android System WebView/Chromium.
- **Glaze UI** — authoritative GoreeCloud presentation and interaction system for Browser-owned surfaces; Browser must track the current approved Stable release.
- **GoreeCloud Wayfinder** — first-party Browser feature family for navigation, productivity, organization, downloads, transfers, capture, sessions, synchronization, Browser utilities, content handling, and workflow integration.
- **Media Hover** — first-party contextual media interaction layer for images, video, regions, visual search, destination-aware saving, AI/OCR workflows, source attribution, privacy enforcement, and security evidence.
- **GoreeCloud Search** — sole and default integrated search authority for approved Browser search entry points.
- **GoreeCloud Bookmarks** — first-class bookmarks, read-later, collections, and library subsystem.
- **GoreeCloud Sync** — synchronization for approved Browser state while excluding indiscriminate synchronization of sensitive website state.
- **Privacy Shield** — privacy and content-protection authority for tracking protection, filtering, URL cleaning, private-browsing protection, and related controls.
- **Wardveil Security** — security presentation, classification, protection-state experience, and evidence-backed Browser security status.
- **Everkeep** — encrypted browser-state backup, recovery, portability, session preservation, and long-term preservation where appropriate.
- **GoreeCloud Identity** — browser profiles, device authorization, synchronization identity, account-bound preferences, and passkey identity integration.
- **GoreeCloud Vault** — credentials, passkeys, secrets, secure autofill, and other protected authentication material.
- **GoreeCloud DNS** — DNS resolution, resolver policy, DNS privacy, filtering where approved, and Browser DNS integration.
- **GoreeCloud Network** — networking and private-connectivity authority.
- **GoreeCloud Mesh** — coordination and governance layer between GoreeCloud Browser and other first-party GoreeCloud applications and services.

## Design principles

1. GoreeCloud owns the browser product layer.
2. The rendering engine is replaceable infrastructure, not product identity.
3. GoreeCloud-specific behavior should not be scattered through engine source code when a stable adapter boundary is practical.
4. Privacy Shield, Wardveil Security, Everkeep, Glaze UI, GoreeCloud Mesh, and GoreeCloud Identity are substantive platform systems; public behavior and status must remain tied to implemented capabilities and evidence.
5. Browser state uses GoreeCloud-owned schemas and services instead of depending on Chromium profile formats as the canonical model.
6. Security updates from the underlying engine must remain practical to consume.
7. Engine-specific exceptions must be documented and isolated.
8. Planned capabilities must not be presented as production-ready until implementation and acceptance evidence exist.
9. Security, privacy, credential, identity, DNS, networking, search, and synchronization authority boundaries remain explicit.
10. Browser-owned user-facing surfaces must conform to the latest approved Stable Glaze UI release before production acceptance.

## Wardveil download security

The Advanced Download Manager has a source-level Wardveil Scan release gate. Completed transfer bytes remain in the Browser-owned `.goreecloud-part-*` staging file until an authoritative, current, correctly scoped Wardveil `clean` finding is bound to the exact SHA-256 digest. The staging file is re-hashed before the existing atomic final rename.

Suspicious downloads are held for review. Malicious or unverifiable downloads remain blocked and cannot be opened through the download controller. A malicious result may produce a non-destructive Wardveil Quarantine handoff, but Browser does not execute canonical quarantine itself and never connects directly to ClamAV. Private Browsing retains malware verification while private download queue state remains non-persistent.

This is source integration evidence, not a deployed malware-protection claim. Production acceptance still requires authenticated Browser-to-Wardveil transport, deployed scanner/signature health, controlled runtime malware tests, concurrency-safe staging release, authorized quarantine evidence, and accepted Glaze UI and Privacy Shield behavior. The Android beta blocks downloads until its own path can satisfy this contract rather than creating a bypass. See [`docs/WARDVEIL_DOWNLOAD_SCANNING.md`](docs/WARDVEIL_DOWNLOAD_SCANNING.md).

## Initial development phases

### 0.x — GoreeCloud-owned browser shell

Build the native application, current-Stable Glaze UI browser chrome, tabs, windows, sessions, profiles, settings, permissions UI, history, bookmarks integration, Search integration, downloads, local browser-state storage, Media Hover, and the engine adapter.

### 0.x — First-party browser services

Integrate GoreeCloud Sync, Identity, Vault, Privacy Shield, Wardveil Security, Everkeep, DNS, Network, Wayfinder, Mesh, and capability-gated Media Hover destinations/processors through explicit service contracts.

### Later — selective engine ownership

Replace, patch, or take greater control of engine subsystems only when GoreeCloud has a concrete privacy, security, performance, compatibility, platform, or product requirement.

## Repository documentation

The repository root includes the mandatory application documentation set:

- [`README.md`](README.md) — repository entry point and current state.
- [`SPECIFICATIONS.md`](SPECIFICATIONS.md) — version-coupled product specification.
- [`FEATURES.md`](FEATURES.md) — functionality and implementation state.
- [`BENEFITS.md`](BENEFITS.md) — supportable product value.
- [`COMPETITIVE-OBJECTIVES.md`](COMPETITIVE-OBJECTIVES.md) — benchmark and differentiation objectives.
- [`USER-MANUAL.md`](USER-MANUAL.md) — current user-facing behavior and beta operating guidance.

## Repository layout

```text
apps/browser/                 Native desktop application shell
apps/android/                 Installable Android beta application target
browser/core/                 Engine-independent browser domain model
browser/engine/               Browser Engine Layer interfaces
browser/engine/chromium/      Chromium adapter implementation
browser/services/             GoreeCloud service contracts and adapters
browser/storage/              GoreeCloud-owned browser data stores
browser/ui/                   Current-Stable Glaze UI browser components
include/goreecloud/browser/   Public C++ interfaces
src/                          Implementation and platform hosts
docs/                         Architecture and engineering documentation
packaging/                    Linux, Flatpak, Windows, and other packaging work
```

## Status

GoreeCloud Browser **0.1.0-beta.1** is in active development and is not production-approved. The beta milestone establishes build-tested Browser-owned architecture, a visible Linux native shell, and a CI-built installable Android test APK target. Unless explicitly identified as implemented and validated, documented capabilities remain product requirements or planned capabilities rather than claims of production readiness.

See:

- [`docs/BETA_0_1.md`](docs/BETA_0_1.md) — beta scope, render-capable gates, deferred work, and promotion requirements.
- [`docs/ANDROID_BETA.md`](docs/ANDROID_BETA.md) — Android package, Glaze mapping, runtime restrictions, CI evidence, signing boundary, and promotion requirements.
- [`docs/MEDIA_HOVER.md`](docs/MEDIA_HOVER.md) — Media Hover interaction, search/AI/OCR, destination, privacy, security, accessibility, and integration contract.
- [`docs/WARDVEIL_DOWNLOAD_SCANNING.md`](docs/WARDVEIL_DOWNLOAD_SCANNING.md) — download release, evidence binding, fail-closed security states, privacy, quarantine, and production-acceptance boundaries.
- [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) — architectural boundaries and ownership model.
- [`docs/ENGINE_LAYER.md`](docs/ENGINE_LAYER.md) — Browser Engine Layer contracts.
- [`docs/GLAZE_UI_ADOPTION.md`](docs/GLAZE_UI_ADOPTION.md) — current-Stable Glaze UI adoption and acceptance policy.
- [`docs/PRODUCT_INVENTORY.md`](docs/PRODUCT_INVENTORY.md) — planned inventory, feature requirements, authority boundaries, and production-acceptance direction.
