# GoreeCloud Browser — GLAZE UI Adoption

GoreeCloud Browser tracks the **latest approved Stable GLAZE UI release**.

## Current baseline

- GLAZE UI Stable version: `1.1.0`
- Stable tag: `v1.1.0`
- Canonical repository: `GoreeCloud/goreecloud-glaze-ui`
- Stable release revision: `15cc76d2bcd4065552dc31c77145b63f34d9e7b2`
- Accepted V1.1 visual source: `8ea1f789bbabf943c3359514dc1506b24fa3c51b`
- Optical contract: `contracts/v1.1/optical-refinement.json`
- Browser policy: `latest-approved-stable`
- Browser Android source-mapping state on this branch: migration candidate
- Browser production eligibility from this migration alone: No

This is an adoption baseline, not a permanent pin. A newer Stable promotion makes Browser migration-required until source contracts, native mappings, and product-specific acceptance are updated.

GLAZE UI V1.1 became the sole current consumer target on September 3, 2026. Browser's previously integrated Glaze UI 2.2.0 source mapping is therefore historical. Central design-system Stable status does not auto-promote Browser. The `+android.6` branch records a fresh repository-local migration and still requires exact-revision Browser evidence.

## V1.1 contract applied by Browser

Browser preserves the core rule: **solid where users read or make explicit critical decisions; glazed where users interact with transient navigation, command, search, control, or feedback chrome.**

The current Android source mapping records the applicable V1.1 constraints:

- Browser-owned chrome remains in **Application** System Shell scope; Browser does not manufacture Universal Search, Control Center, System Panel, or Critical System authority.
- The inherited material budget remains at most one dominant Glaze panel plus three small floating Glaze controls. Browser's current mobile composition stays below that limit.
- Ordinary interaction targets remain at least 48dp, with a 56dp Touch Assistance floor where applicable.
- Light, Dark, and Deep Dark are explicit structural appearance targets.
- The V1.1 optical light origin is upper-left and the bounded atmospheric identity is Deep Teal + Soft Amber over neutral structure.
- Protected semantic meaning, focus, accessibility, and required boundaries override atmosphere.
- Nested backdrop blur is not enabled.
- Environmental Color Memory is not required, no content sampling is performed by this mapping, and no remote color-derivation path is introduced.
- Effects-free operation remains a first-class path for Reduced Transparency, platform constraints, and performance constraints.
- Web/content reading planes remain solid; Soft Glaze remains bounded to Browser-owned interaction chrome such as the omnibox and menu sheet.

Appearance, material, motion, and color are presentation contracts only. They cannot manufacture Wardveil security state, Privacy Shield privacy state, Everkeep continuity state, Mesh coordination state, Identity authorization, Search authority, Sync authority, or Browser workflow success.

## Acceptance boundary

Importing V1.1 metadata or passing source tests does not establish Browser conformance. Before a Stable Browser release, exact Browser revisions must pass the applicable rendered/native visual, accessibility, interaction, responsive/form-factor, platform, performance, localization/directionality, and production gates.

For Android this includes representative physical-device review, TalkBack/accessibility semantics, 200% text, Reduced Motion, Reduced Transparency/effects-free behavior, increased contrast/high-contrast behavior, RTL/localization, Touch Assistance mapping where supported, orientation/form-factor behavior, and sustained-use/performance evidence. The Glaze design-system's own Android emulator evidence is reference evidence and is not downstream Browser acceptance.

## Browser-owned scope

The requirement covers Browser chrome, navigation, unified address/search, tabs, New Tab, Home, Settings, Downloads, Library, Bookmarks, Private Browsing landing surfaces, Browser-owned print/PDF controls, media actions, Clipboard Manager, Reader Mode, context menus, Proxy Manager, Network and DNS settings, memory controls, Wayfinder features, and GoreeCloud service integrations.

OS-owned, certificate, permission, engine-critical, and Developer Tools surfaces may preserve required native or engine presentation where replacement would reduce security, accessibility, or compatibility. That exception cannot be used to keep ordinary GoreeCloud-owned UI on a superseded design language.

## Upgrade and rollback process

For each future Stable promotion, Browser must record the new semantic version and exact reviewed revision, audit changed contracts, update local metadata and native mappings, run Browser-specific acceptance, validate representative supported clients, and remain development-only wherever required adoption evidence is incomplete.

The authoritative Browser revision before this migration is `b47a56a0109ec508532753c8af7f25cfe2545fa9`, whose Android mapping targeted Glaze UI 2.2.0 and produced beta `+android.5`. That revision remains historical rollback/comparison evidence only and does not qualify Browser against current Stable V1.1.
