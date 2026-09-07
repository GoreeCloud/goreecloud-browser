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
- Browser Android source-mapping state on this branch: Development migration candidate
- Browser beta identity for this mapping: `0.1.0-beta.1+android.7` / versionCode `10007`
- Browser production eligibility from this migration alone: No

This is an adoption baseline, not a permanent pin. A newer Stable promotion makes Browser migration-required until source contracts, native mappings, and product-specific acceptance are updated.

GLAZE UI V1.2 / `1.2.0` became the current Stable consumer target on September 6, 2026. Browser's earlier Glaze UI 2.2 and V1.1 mappings remain historical source/rollback evidence only. Central design-system Stable status does not auto-promote Browser or satisfy Browser-specific acceptance.

## V1.2 contract applied by Browser

The governing V1.2 material rule is: **Neutral glass is the material. Color is an accent.**

Browser preserves that rule together with the inherited reading-first boundary: durable reading and consequential-decision surfaces remain solid or near-solid where required, while transient Browser-owned navigation, command, search, control, and feedback chrome may use bounded neutral glaze.

The current Android source mapping records these V1.2 constraints:

- Browser-owned chrome remains in **Application** System Shell scope; Browser does not manufacture Universal Search, Control Center, System Panel, or Critical System authority.
- The inherited material budget remains at most one dominant Glaze panel plus three small floating Glaze controls. Browser's current mobile composition stays below that limit.
- Ordinary interaction targets remain at least 48dp, with a 56dp Touch Assistance floor where applicable.
- Light, Dark, and Deep Dark are explicit structural appearance targets.
- The optical light origin remains upper-left; Frost White is the primary neutral material reference and Ice Blue is a bounded atmospheric accent rather than a substrate authority.
- Deep Teal, Soft Amber, green, aqua, or other chromatic atmosphere must not become the default V1.2 material substrate.
- Protected semantic meaning, focus, accessibility, and required boundaries override atmosphere.
- Nested backdrop blur is not enabled.
- Environmental Color Memory is not required, no content sampling is performed by this mapping, and no remote color-derivation path is introduced.
- Effects-free operation remains a first-class path for Reduced Transparency, platform constraints, and performance constraints.
- Web/content reading planes remain solid; Soft Glaze remains bounded to Browser-owned interaction chrome such as the omnibox and menu sheet.

Appearance, material, motion, and color are presentation contracts only. They cannot manufacture Wardveil security state, Privacy Shield privacy state, Everkeep continuity state, Mesh coordination state, Identity authorization, Search authority, Sync authority, or Browser workflow success.

## Superseded V1.1 blocker

The V1.1 import-closure defect that blocked the earlier Browser migration is no longer the current design-system target blocker. V1.2 Stable superseded V1.1 as the required consumer target. Historical V1.1 source and evidence remain useful only as rollback/audit material and must not be relabeled as V1.2 Browser acceptance.

## Acceptance boundary

Importing V1.2 metadata or passing source tests does not establish Browser conformance. Before a Stable Browser release, exact Browser revisions must pass the applicable rendered/native visual, accessibility, interaction, responsive/form-factor, platform, performance, localization/directionality, and production gates.

For Android this includes representative physical-device review, TalkBack/accessibility semantics, 200% text, Reduced Motion, Reduced Transparency/effects-free behavior, increased contrast/high-contrast behavior, RTL/localization, Touch Assistance mapping where supported, orientation/form-factor behavior, and sustained-use/performance evidence. The Glaze design-system's own reference/emulator evidence is not downstream Browser acceptance.

The V1.3 qualification work deferred by the shared design-system release is likewise not automatically satisfied by Browser. Browser must produce its own applicable consumer/platform evidence for the exact Browser revision it proposes to ship.

## Browser-owned scope

The requirement covers Browser chrome, navigation, unified address/search, tabs, New Tab, Home, Settings, Downloads, Library, Bookmarks, Private Browsing landing surfaces, Browser-owned print/PDF controls, media actions, Clipboard Manager, Reader Mode, context menus, Proxy Manager, Network and DNS settings, memory controls, Wayfinder features, and GoreeCloud service integrations.

OS-owned, certificate, permission, engine-critical, and Developer Tools surfaces may preserve required native or engine presentation where replacement would reduce security, accessibility, or compatibility. That exception cannot be used to keep ordinary GoreeCloud-owned UI on a superseded design language.

## Upgrade and rollback process

For each future Stable promotion, Browser must record the new semantic version and exact reviewed revision, audit changed contracts, update local metadata and native mappings, run Browser-specific acceptance, validate representative supported clients, and remain Development wherever required adoption evidence is incomplete.

The authoritative Browser revision before the Glaze migration program is `b47a56a0109ec508532753c8af7f25cfe2545fa9`, whose Android mapping targeted historical Glaze UI 2.2.0 and produced beta `+android.5`. The subsequent V1.1 migration commits remain intermediate historical evidence. A failed V1.2 migration rolls Browser back to a separately verified Browser integration revision; it does not rewrite Glaze lifecycle history or manufacture conformance.
