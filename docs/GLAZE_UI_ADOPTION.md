# GoreeCloud Browser — Glaze UI Adoption

GoreeCloud Browser tracks the **latest approved Stable Glaze UI release**.

## Current baseline

- Glaze UI Stable version: `2.2.0`
- Canonical repository: `GoreeCloud/goreecloud-glaze-ui`
- Stable release revision: `6731098b28dd0393faa878c70d989a221d714a20`
- Accepted Glaze UI visual source: `0411b0f6dd877aea30e2c5674e1acde0105fd97b`
- Browser policy: `latest-approved-stable`
- Browser Android source-mapping state on this branch: migration candidate
- Browser production eligibility from this migration alone: No

This is an adoption baseline, not a permanent pin. A newer Stable promotion makes Browser migration-required until source contracts, native mappings, and product-specific acceptance are updated.

Glaze UI 2.2.0 was promoted after Browser's previously integrated 2.0.0 mapping. The central design-system release does not auto-promote Browser. The `+android.5` branch therefore records repository-local adoption work and must obtain its own exact-source automated and native/device evidence.

## Preserved Glaze UI 2.x contracts

Browser retains the material hierarchy of Canvas, Surface, Soft Glaze, Glaze, Deep Glaze, and Live Glaze; native/platform authority; readable solid content; bounded Glaze interaction chrome; Balanced clarity; Calm expression; visible focus/pressed behavior; semantic accessibility labels; light/dark adaptation; vector Browser icons; and effects-free operation that does not require blur, transparency, or animation.

General touch-oriented targets remain at least 48dp. Glaze UI 2.2 also establishes a 56dp floor for Touch Assistance/far-view contexts where applicable. Browser now records both floors in its Android contract. The current Android shell does **not** claim that a product-wide Touch Assistance runtime preference or OS mapping is already wired merely because the 56dp contract is represented.

Appearance, material, motion, and color are presentation contracts only. They cannot manufacture Wardveil security state, Privacy Shield privacy state, Everkeep continuity state, Mesh coordination state, Identity authorization, or Browser workflow success.

## Glaze UI 2.2 migration review

The Browser Android mapping explicitly evaluates the 2.2 additions relevant to the current beta scope:

- **System Shell hierarchy:** Browser-owned chrome remains in Application scope. Local Browser menu and omnibox/search behavior are not relabeled as Universal Search, Control Center, System Panel, or Critical System UI.
- **System Glaze budget:** the source contract permits at most one dominant Glaze panel and up to three small floating Glaze controls for the ordinary mapped composition. The current Browser mapping remains inside that bound.
- **Target geometry:** 48dp is the ordinary touch floor; 56dp is recorded for Touch Assistance/far-view contexts where applicable.
- **State priority:** Browser records the Glaze UI 2.2 semantic ordering so disabled state outranks error, pressed, focus, selected, hover, and rest. Loading remains a distinct application state without weakening disabled/error authority.
- **Solid content / glazed interaction:** web/content reading planes remain solid; Soft Glaze remains bounded to transient Browser-owned interaction chrome such as the omnibox and menu sheet.
- **Accessibility fallbacks:** effects-free behavior remains a first-class path rather than a degraded product mode.
- **Universal Search / Control Center:** not claimed by this Android Browser slice. GoreeCloud Search remains the query authority for non-URL Browser input; that service relationship is not the same thing as adopting Glaze UI's system-level Universal Search shell.
- **Signature and Intelligence components:** no new claim is made merely because 2.2 defines them. Browser must adopt them only where a real product relationship requires them and their semantics are implemented.

## Production rule

A superseded Glaze UI release cannot satisfy Browser production readiness. Design-system Stable status also does not automatically certify Browser.

Before a Stable Browser release, representative Browser-owned task flows must demonstrate current-Stable conformance on supported platform families, layout classes, input modalities, accessibility modes, localization/directionality cases, safe-area/hinge constraints, and representative hardware that apply to the client.

For this Android migration specifically, source tests and a successful CI APK are necessary but not sufficient. Exact-revision visual review, TalkBack/accessibility semantics, 200% text, Reduced Motion, Reduced Transparency/effects-free behavior, Increased Contrast, Forced Colors or closest platform-equivalent behavior, RTL/localization, Touch Assistance mapping where supported, orientation/form-factor behavior, and performance remain acceptance gates.

## Browser-owned scope

The requirement covers browser chrome, navigation, unified address/search, tabs, New Tab, Home, Settings, Downloads, Library, Bookmarks, Private Browsing landing surfaces, Browser-owned print/PDF controls, media actions, Clipboard Manager, Reader Mode, context menus, Proxy Manager, Network and DNS settings, memory controls, Wayfinder features, and GoreeCloud service integrations.

OS-owned, certificate, permission, engine-critical, and Developer Tools surfaces may preserve required native or engine presentation where replacement would reduce security, accessibility, or compatibility. That exception cannot be used to keep ordinary GoreeCloud-owned UI on an older design language.

## Upgrade and rollback process

For each future Glaze UI Stable promotion, Browser must record the new semantic version and exact reviewed revision, audit changed contracts, update local metadata and native mappings, run Browser-specific acceptance, validate representative supported clients, and remain development-only wherever required adoption evidence is incomplete.

The Glaze UI 2.2 design-system rollback reference is 2.1.0. Browser's last integrated Android mapping before this migration was 2.0.0 in authoritative Browser main `974786cdccceac7a0198881d4bd5f4e5d4b28c58` / beta `+android.4`. Neither historical baseline qualifies Browser against current Stable 2.2.0. Rollback evidence is retained for diagnosis and recovery; history is not rewritten to imply earlier 2.2 conformance.
