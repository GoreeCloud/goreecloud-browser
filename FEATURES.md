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
- Current stacked Development candidate identity `0.1.0-beta.1+android.11` / versionCode `10011`.
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
- Scroll-aware top Browser chrome remains available while Android reports an accessibility service enabled; Browser consumes only the boolean accessibility-enabled state and does not enumerate services or inspect accessibility/user content.
- Text-bearing top Browser chrome and Browser-menu actions use content height plus minimum interaction floors, allowing Android-native font scaling to increase their measured height rather than constraining scaled text to hard 56dp rows.
- The large-text source contract records 2.0 font scale as the downstream acceptance target while keeping rendered/device acceptance explicitly pending.
- Android application RTL support is declared; directional Back and Forward vector resources opt into native auto-mirroring; a JVM source contract verifies those flags plus logical start/end rather than physical left/right programmatic horizontal margins.
- Browser-owned visible/accessibility Android chrome copy is sourced from `res/values/strings.xml` rather than embedded English labels in `BrowserActivity.kt`; HTTPS/HTTP/WEB protocol/status tokens remain explicitly non-translatable.
- A localization source contract requires the reviewed Browser string-resource surface and fails if the prior embedded UI copy returns to the Activity.
- Unit tests for Browser-owned navigation resolution, address presentation, GLAZE UI mapping, accessibility-aware top-chrome visibility policy, large-text content-height source contract, bounded RTL directionality source contract, and Android string-resource localization foundation.
- CI unit test, Android lint, APK build, exact-source verification, signature/package/version verification, SHA-256/source-revision recording, and artifact upload.

The string-resource extraction is localization readiness only. It does not provide translations or establish translation completeness/quality, locale fallback/plural/grammar behavior, bidirectional URL/address handling, rendered RTL behavior, or representative locale/device acceptance.

## Android beta — GLAZE UI V1.2 source mapping candidate

- Current Stable GLAZE UI version `1.2.0` and Stable promotion merge revision `f285b9145e27e6e7027b075c37299d101945c272` recorded in source.
- V1.2 source-qualification anchor `b0eadf9a60f73d45caffb62ffc7e9e0334cddc97` recorded as design-system provenance, not Browser acceptance.
- V1.2 optical foundation `tokens/glaze-v1.2-optical-foundation.candidate.json` plus Stable web/runtime entrypoints recorded in the Android mapping; the Candidate filename is preserved upstream source lineage rather than current lifecycle status.
- Governing rule **Neutral glass is the material. Color is an accent.** recorded in source.
- Core solid/near-solid reading and consequential-decision boundary preserved while transient Browser-owned interaction chrome uses bounded neutral glaze.
- Browser-owned chrome explicitly remains in Application scope rather than claiming Universal Search, Control Center, System Panel, or Critical System authority.
- Inherited Glaze budget represented as at most one dominant Glaze panel and up to three small floating Glaze controls.
- Native Android controls retained for platform semantics.
- 48dp minimum general touch-oriented interaction target.
- 56dp Touch Assistance target floor recorded where applicable; complete Android runtime mapping remains pending.
- Text-bearing chrome uses content-height layout rather than exact fixed row heights, preserving the normal 56dp/128dp baseline while allowing scaled text metrics to expand top/menu chrome.
- 2.0 font scale recorded as the downstream large-text acceptance target; this is not rendered 200% acceptance.
- Android RTL support plus native auto-mirroring for directional Back/Forward vectors is source-validated.
- Browser-owned natural-language chrome strings are resource-backed for future localization; protocol/status tokens remain explicitly non-translatable.
- Light, Dark, and Deep Dark structural appearance targets recorded.
- Upper-left optical light direction, Frost White neutral material reference, and bounded Ice Blue atmospheric accent recorded without restoring chromatic substrate tinting.
- Nested backdrop blur remains disabled.
- Environmental Color Memory is not required and this mapping does not introduce environmental-content sampling or remote color derivation.
- Calm expression and Balanced clarity retained for the current Browser shell.
- Effects-free fallback remains first class and does not require blur/transparency.
- Contract tests cover current Stable provenance, target floors, Application authority, Glaze budget, V1.2 neutral-material/appearance bounds, no nested blur, no required environmental sampling, preserved semantic-state priority, text-content-height source contract, bounded RTL directionality source requirements, and the Android localization-resource foundation.

This section is repository-local source migration and hardening evidence only. A successful build does not establish Browser-specific V1.2 rendered/native-device visual, accessibility, localization, Human Visual Excellence, performance, or production acceptance. The source changes do not establish representative TalkBack, Switch Access, Voice Access, rendered 200% text, translated/localized copy, rendered RTL, focus-order, announcement-quality, or physical-device acceptance.

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
- Complete Touch Assistance Android runtime mapping and native acceptance where supported.
- Actual translated/localized Browser resources and locale coverage.
- Representative TalkBack, Switch Access, Voice Access, rendered 200% text, translation-quality/locale fallback, bidirectional URL/address handling, rendered RTL directionality, and accessibility announcement/focus-order acceptance.
- Bookmarks, history, library, settings, downloads UI, Reader Mode, and Wayfinder mobile surfaces.
- Controlled beta/production signing, managed updates, rollback, and migration.
- Store packaging and publication.
- Representative real-device, accessibility, performance, battery, and compatibility acceptance.

## Acceptance principle

Implemented source, successful CI, an installable package, runtime integration, target-environment validation, security validation, accessibility acceptance, design-system acceptance, localization/directionality acceptance, and production approval are separate states. Browser documentation must preserve those distinctions.
