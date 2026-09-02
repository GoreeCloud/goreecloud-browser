# GoreeCloud Browser Features

This file records Browser functionality and implementation state. A listed feature is not a production-readiness claim unless its acceptance state explicitly supports that conclusion.

## Implemented and build-tested foundations

- GoreeCloud-owned engine-independent browser core.
- Browser Engine Layer abstraction for replaceable rendering/runtime foundations.
- Linux GTK/X11 native beta shell build path.
- GoreeCloud Search integration as the sole integrated query authority.
- Transport-neutral first-party service capability evidence that keeps producer authority outside Browser and fails closed unless an exact capability is current, authoritative, explicitly production-accepted, and—when requested—on the exact expected contract version.
- Browser-owned Sync submission/retrieval contracts with privacy-safe tombstones, capability/schema validation, pagination, record-ID bounds, and signer-shape validation.
- Advanced Download Manager core with staged transfer and source-level Wardveil release-gate architecture.
- Privacy-safe native session-recovery checkpoint/candidate core that excludes Private and Isolated Private windows before persistence, validates bounded persisted window/tab topology and active-tab references, and only considers the newest checkpoint so older crash state cannot silently resurface after a newer clean or private-only session.
- Media Hover architecture and first-party Browser feature contracts.

The first-party capability gate is a consumer-side contract only. A healthy transport, a recognized service, or current authoritative producer evidence is insufficient by itself: Browser will not treat a capability as usable through this gate until the producer evidence explicitly states production acceptance. Browser does not create or strengthen Search, Vault, Sync, Identity, Mesh, Privacy Shield, Wardveil Security, Everkeep, DNS, Network, or Bookmarks authority.

The session-recovery core is not yet a user-ready recovery implementation. Durable authenticated-encrypted checkpoint storage, protected platform key integration, Browser lifecycle wiring, restore execution, Glaze recovery UI, Everkeep continuity integration, and runtime acceptance remain pending.

## Android beta — implemented

- Installable debug-signed APK target.
- Package `io.goreecloud.browser.beta`.
- Android API 26 minimum and API 35 target.
- Android System WebView/Chromium rendering dependency behind GoreeCloud-owned product behavior.
- Back, Forward, Reload, Go, unified address/search field, progress state, and web-content region.
- Direct HTTP/HTTPS navigation.
- HTTPS upgrade for bare hosts.
- GoreeCloud Search for non-URL input.
- Browser-intent handling for HTTP/HTTPS links.
- TLS certificate errors fail closed.
- Android Safe Browsing enabled with return-to-safety behavior.
- Mixed-content loading disabled.
- Third-party cookies disabled by default.
- WebView file/content access disabled.
- Website permissions and geolocation denied until Browser-owned policy surfaces are accepted.
- Downloads blocked until the Android path satisfies the authoritative Wardveil release contract.
- Unit tests for Browser-owned navigation resolution.
- CI unit test, Android lint, APK build, signature/package verification, SHA-256, and artifact upload.

## Android beta — Glaze UI 2.0 source mapping candidate

- Explicit current-Stable Glaze UI version/revision metadata.
- Canvas/Surface/Soft Glaze/Glaze semantic material mapping.
- Native Navigation Capsule composition.
- Native Android controls retained for platform semantics.
- 48dp minimum general interaction target.
- Light/dark appearance adaptation.
- Visible pressed/focus state mapping.
- Calm expression and Balanced clarity for the initial browser shell.
- Effects-free fallback that does not require blur/transparency.
- Contract tests for current Glaze version, target floor, and mapping semantics.

This section is source-level mapping evidence only. Native-device Glaze conformance and accessibility acceptance remain pending.

## Planned / incomplete Android capabilities

- Browser-owned tab strip/tab switcher and multi-tab lifecycle.
- Private Browsing and Close & Forget runtime isolation.
- Browser-owned website permission prompts.
- Wardveil-authenticated download staging, scan, release, hold, and quarantine handoff.
- Full Privacy Shield filtering, consent, data-use, and diagnostics integration.
- Everkeep backup/recovery/portability integration.
- GoreeCloud Identity profile/session integration.
- GoreeCloud Vault credential/passkey/autofill integration.
- GoreeCloud Sync runtime integration.
- GoreeCloud DNS and GoreeCloud Network runtime adapters.
- GoreeCloud Mesh capability coordination.
- Bookmarks, history, library, settings, downloads UI, Reader Mode, and Wayfinder mobile surfaces.
- Controlled beta/production signing, managed updates, rollback, and migration.
- Store packaging and publication.
- Representative real-device, accessibility, performance, battery, and compatibility acceptance.

## Acceptance principle

Implemented source, successful CI, an installable package, runtime integration, target-environment validation, security validation, accessibility acceptance, and production approval are separate states. Browser documentation must preserve those distinctions.
