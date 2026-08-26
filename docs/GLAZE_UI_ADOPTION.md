# GoreeCloud Browser — Glaze UI Adoption

GoreeCloud Browser is built against the **latest approved Stable Glaze UI release**.

## Current baseline

- Glaze UI Stable version: `1.5.0`
- Canonical Glaze UI repository: `GoreeCloud/glaze-ui`
- Current Stable source revision: `2e1618397f6ebcdd254a76bfdd7e98846f2c5aa3`
- Browser policy: `latest-approved-stable`

The current version is an adoption baseline, not a permanent pin. When Glaze UI promotes a newer Stable release, GoreeCloud Browser becomes migration-required until its Glaze integration, native mappings, and product-specific acceptance are updated and validated.

## Production rule

A superseded Glaze UI release cannot satisfy GoreeCloud Browser production readiness.

Before a Stable Browser release, Browser-owned user-facing surfaces must demonstrate application-specific conformance against the then-current Glaze UI Stable release. Design-system Stable status does not automatically certify the Browser.

## Browser-owned surfaces

The requirement applies to GoreeCloud-controlled Browser experiences including browser chrome, navigation, address/search, New Tab, Home, Settings, Downloads, Library, Bookmarks, Private Browsing landing surfaces, print and PDF controls where Browser-owned, image download controls, Clipboard Manager, Reader Mode, context menus, Proxy Manager, Network & DNS settings, memory controls, Wayfinder features, and GoreeCloud service integrations.

Security-critical, OS-owned, certificate, permission, Developer Tools, and other sensitive surfaces may preserve required platform or engine behavior where aggressive restyling would reduce safety, accessibility, or compatibility. Such exceptions do not permit an older Browser design language to become the normal product UI.

## Glaze UI 1.5 requirements consumed by Browser

The 1.5 adoption baseline includes:

- layered adaptive semantic color and protected truth families;
- governed iconography, construction, optical sizing, and identity rules;
- purpose-driven interruptible motion and reduced-motion substitutions;
- Canvas, Solid, Raised, Functional Glass, Clear Glass, and Overlay material hierarchy;
- semantic spacing, responsive gutters, bounded measures, density modes, safe areas, target floors, localization/order requirements, and bounded intrinsic overflow;
- focus-visible, hover, pressed, selected, expanded, disabled, read-only, loading, invalid, success, and mixed-input interaction semantics;
- retained Mobile, Tablet, Desktop, Wide Desktop, and TV form-factor contracts where applicable.

## Authority boundary

Glaze UI governs presentation and interaction semantics. It does not manufacture application truth.

Privacy Shield remains authoritative for privacy state. Wardveil Security remains authoritative for evidence-backed security presentation. Everkeep remains authoritative for resilience requirements. GoreeCloud Mesh remains authoritative for coordination. Browser and service logic remain authoritative for availability, completion, validation, workflow, navigation, and data state.

## Upgrade process

For every future Glaze UI Stable promotion, GoreeCloud Browser must:

1. record the new Stable semantic version and exact reviewed source revision;
2. audit changed Glaze contracts used by the Browser;
3. update Browser-local Glaze compatibility metadata and mappings;
4. update Glaze UI components and native platform mappings;
5. run Browser-specific accessibility, responsive, form-factor, interaction, and visual acceptance;
6. validate representative Linux and Android Browser task flows and other supported family clients as applicable;
7. keep the Browser development-only if required current-Stable adoption is incomplete.

There are no production exceptions for knowingly shipping a superseded Glaze UI baseline on a GoreeCloud-controlled Browser surface.
