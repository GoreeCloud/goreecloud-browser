# GoreeCloud Browser — GLAZE UI Adoption

GoreeCloud Browser tracks the **current approved Stable GLAZE UI release**.

## Current baseline

- GLAZE UI Stable version: `1.2.0`
- Canonical repository: `GoreeCloud/goreecloud-glaze-ui`
- Stable promotion merge revision: `f285b9145e27e6e7027b075c37299d101945c272`
- V1.2 source-qualification anchor: `b0eadf9a60f73d45caffb62ffc7e9e0334cddc97`
- Stable contract: `GLAZE_UI_V1_2.md`
- Optical foundation: `tokens/glaze-v1.2-optical-foundation.candidate.json`
- Stable web entrypoint: `css/glaze-v1.2.0.css`
- Stable runtime entrypoint: `js/glaze-v1.2.0.mjs`
- Browser policy: `latest-approved-stable`
- Browser Android source-mapping state on this branch: stacked Development candidate
- Browser beta identity for this mapping/hardening stack: `0.1.0-beta.1+android.16` / versionCode `10016`
- Browser production eligibility from this source stack alone: No

This is an adoption baseline, not a permanent pin. A newer Stable promotion makes Browser migration-required until source contracts, native mappings, and product-specific acceptance are updated.

GLAZE UI V1.2 / `1.2.0` became the current Stable consumer target on September 6, 2026. Browser's earlier Glaze UI 2.2 and V1.1 mappings remain historical source/rollback evidence only. The Android Development stack now layers the V1.2 source migration (`+android.7`), accessibility-aware top-chrome persistence (`+android.8`), large-text content-height hardening (`+android.9`), bounded RTL history-control directionality hardening (`+android.10`), Browser-owned Android string-resource localization readiness (`+android.11`), debug pseudolocale testability (`+android.12`), bounded unfocused address bidi-presentation hardening (`+android.13`), explicit-port presentation correctness (`+android.14`), Browser-owned navigation-authority validation (`+android.15`), and malformed-web external-handoff fail-closed hardening (`+android.16`). Central design-system Stable status and these source changes do not auto-promote Browser or satisfy Browser-specific downstream acceptance.

## V1.2 contract applied by Browser

The governing V1.2 material rule is: **Neutral glass is the material. Color is an accent.**

Browser preserves that rule together with the inherited reading-first boundary: durable reading and consequential-decision surfaces remain solid or near-solid where required, while transient Browser-owned navigation, command, search, control, and feedback chrome may use bounded neutral glaze.

The current Android source mapping records these V1.2 constraints:

- Browser-owned chrome remains in **Application** System Shell scope; Browser does not manufacture Universal Search, Control Center, System Panel, or Critical System authority.
- The inherited material budget remains at most one dominant Glaze panel plus three small floating Glaze controls. Browser's current mobile composition stays below that limit.
- Ordinary interaction targets remain at least 48dp, with a 56dp Touch Assistance floor where applicable.
- Text-bearing top/menu chrome uses content-height measurement rather than exact fixed row heights, so native Android `sp` metrics may increase measured height as system font scale grows.
- The normal 56dp omnibox / 128dp expanded-chrome values are baseline dimensions at ordinary text scale, not hard maximum heights under large text.
- 2.0 font scale is recorded in the source contract as the downstream large-text acceptance target; source/build evidence is not rendered 200% acceptance.
- Android application RTL support is declared, directional Back and Forward Browser vectors opt into Android auto-mirroring, and the current programmatic chrome source contract avoids physical left/right horizontal-margin fields.
- Browser-owned natural-language chrome/accessibility copy is sourced from Android string resources rather than embedded directly in BrowserActivity logic; HTTPS/HTTP/WEB protocol/status tokens are explicitly non-translatable.
- The debug beta generates Android `en-XA` and `ar-XB` pseudolocales for later expansion/accent and RTL localization stress testing; the release build type is not opted into this debug testability contract.
- Android CI verifies both generated pseudolocale variants in the debug APK's `aapt` badging before artifact upload.
- The condensed unfocused address presentation removes Unicode bidi-formatting controls while preserving ordinary RTL letters; focusing the omnibox exposes the untouched authoritative full URL.
- The condensed unfocused address presentation preserves every explicitly parsed URL port, including explicitly written default ports, so a visually condensed host does not hide an explicit endpoint component.
- The address-presentation hardening is explicitly presentation-only and does not rewrite navigation, search, clipboard/share URLs, WebView requests, persistence, permissions, telemetry, or network traffic.
- Browser-owned navigation applies a separate local HTTP(S) authority-syntax boundary before direct navigation is allowed. Malformed explicit HTTP(S) input—including explicit `http:` / `https:` forms without `//`—fails to GoreeCloud Search Home; optional authority ports must be decimal and within `0..65535`; valid `localhost:port`, bracketed IPv6-with-port, and Unicode HTTP(S) host inputs remain supported within this bounded source contract.
- Malformed HTTP(S) page-link attempts remain Browser-owned and are not eligible for Android external-app handoff merely because a user gesture occurred. External handoff remains eligible only for a user-gestured non-web scheme.
- Navigation-authority and external-handoff validation are **not** Glaze authorities. They do not perform DNS resolution, certificate validation, IDN normalization/confusable analysis, origin-trust classification, Wardveil classification, complete scheme/intent/deep-link validation, external-application trust classification, or network requests, and Glaze presentation must never convert them into such claims.
- The localization-resource, pseudolocale, bidi-presentation, explicit-port, navigation-authority, and malformed-web handoff hardening create testing/source boundaries only. They do not establish actual translations, translation completeness/quality, locale fallback/plural/grammar behavior, DNS validity, certificate/origin trust, complete Unicode/IDN/confusable handling, complete scheme/intent/deep-link or external-application trust, bidirectional URL/address editing, rendered pseudolocale behavior, rendered RTL layout, or representative locale/device acceptance.
- Light, Dark, and Deep Dark are explicit structural appearance targets.
- The optical light origin remains upper-left; Frost White is the primary neutral material reference and Ice Blue is a bounded atmospheric accent rather than a substrate authority.
- Deep Teal, Soft Amber, green, aqua, or other chromatic atmosphere must not become the default V1.2 material substrate.
- Protected semantic meaning, focus, accessibility, and required boundaries override atmosphere.
- Nested backdrop blur is not enabled.
- Environmental Color Memory is not required, no content sampling is performed by this mapping, and no remote color-derivation path is introduced.
- Effects-free operation remains a first-class path for Reduced Transparency, platform constraints, and performance constraints.
- Web/content reading planes remain solid; Soft Glaze remains bounded to Browser-owned interaction chrome such as the omnibox and menu sheet.
- Page-scroll auto-hide cannot remove the top omnibox while Android reports an accessibility service enabled; the policy uses only the platform boolean enabled state and does not enumerate services or inspect accessibility event/user content.

Appearance, material, motion, color, layout, accessibility-source, directionality-source, localization-resource, pseudolocale-testability, bidi-address-presentation, and explicit-port-presentation contracts are presentation/interaction/test contracts only. Browser navigation-authority and malformed-web external-handoff validation are separate routing-policy source contracts. Neither class of contract can manufacture Wardveil security state, Privacy Shield privacy state, Everkeep continuity state, Mesh coordination state, Identity authorization, Search authority, Sync authority, DNS validity, certificate trust, origin trust, scheme/intent safety, or external-application trust.

## Superseded mappings

The historical Glaze UI 2.2 and V1.1 mappings are no longer the current design-system target. V1.2 Stable superseded them as the required consumer target. Historical source and evidence remain useful only as rollback/audit material and must not be relabeled as V1.2 Browser acceptance.

## Acceptance boundary

Importing V1.2 metadata, hardening source layout/directionality/localization/address-presentation boundaries, generating pseudolocales, preserving explicit address ports, validating local navigation-authority syntax, keeping malformed HTTP(S) links inside Browser, or passing source/build tests does not establish Browser conformance. Before a Stable Browser release, exact Browser revisions must pass the applicable rendered/native visual, accessibility, interaction, responsive/form-factor, platform, performance, localization/directionality, security, and production gates.

For Android this includes representative physical-device review, TalkBack, Switch Access, Voice Access, accessibility semantics/focus/announcement quality, rendered 200% text, Reduced Motion, Reduced Transparency/effects-free behavior, increased contrast/high-contrast behavior, rendered `en-XA` / `ar-XB` pseudolocale stress review, actual translated/localized resources and translation-quality/completeness review, locale fallback/plural/grammar behavior, DNS/origin/certificate/Unicode/IDN/confusable review, complete scheme/intent/deep-link and external-application handoff review, bidirectional text and URL/address editing behavior, rendered RTL directionality, Touch Assistance mapping where supported, orientation/form-factor behavior, and sustained-use/performance evidence. The Glaze design-system's own reference/emulator evidence is not downstream Browser acceptance.

The V1.3 qualification work deferred by the shared design-system release is likewise not automatically satisfied by Browser. Browser must produce its own applicable consumer/platform evidence for the exact Browser revision it proposes to ship.

## Browser-owned scope

The requirement covers Browser chrome, navigation, unified address/search, tabs, New Tab, Home, Settings, Downloads, Library, Bookmarks, Private Browsing landing surfaces, Browser-owned print/PDF controls, media actions, Clipboard Manager, Reader Mode, context menus, Proxy Manager, Network and DNS settings, memory controls, Wayfinder features, and GoreeCloud service integrations.

OS-owned, certificate, permission, engine-critical, and Developer Tools surfaces may preserve required native or engine presentation where replacement would reduce security, accessibility, or compatibility. That exception cannot be used to keep ordinary GoreeCloud-owned UI on a superseded design language.

## Upgrade and rollback process

For each future Stable promotion, Browser must record the new semantic version and exact reviewed revision, audit changed contracts, update local metadata and native mappings, run Browser-specific acceptance, validate representative supported clients, and remain Development wherever required adoption evidence is incomplete.

The authoritative Browser revision before the Glaze migration program is `b47a56a0109ec508532753c8af7f25cfe2545fa9`, whose Android mapping targeted historical Glaze UI 2.2.0 and produced beta `+android.5`. The subsequent V1.1 migration commits remain intermediate historical evidence. The current V1.2/accessibility/large-text/RTL-directionality/localization-resource/pseudolocale-testability/bidi-address-presentation/explicit-port-presentation/navigation-authority-validation/malformed-web-external-handoff stack remains Development until reviewed and integrated through the source-control process. A failed migration or hardening slice rolls Browser back to a separately verified Browser integration revision; it does not rewrite Glaze lifecycle history or manufacture conformance.
