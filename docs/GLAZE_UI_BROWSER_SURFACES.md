# GoreeCloud Browser — Glaze UI Surface Model

This document translates the current Glaze UI Stable contract into Browser-owned product surfaces. It is an implementation specification, not a claim that every surface is complete.

## Browser chrome

The primary Browser frame uses Glaze semantic layout, spacing, color, material, iconography, motion, focus, and interaction contracts. The frame must remain visually GoreeCloud-owned even when Chromium/Blink supplies the underlying web engine.

### Top-level hierarchy

- **Canvas** — root application/background plane.
- **Solid** — high-legibility controls and regions where translucency would reduce clarity.
- **Raised** — elevated toolbars, cards, panels, and persistent secondary regions where separation is required.
- **Functional Glass** — bounded translucent browser chrome where underlying context is useful and legibility remains deterministic.
- **Clear Glass** — limited decorative/contextual treatment; never a substitute for readable functional surfaces.
- **Overlay** — menus, popovers, dialogs, permission prompts, command surfaces, and transient layers owned by Browser.

## Address and search surface

The address/search surface is a primary Browser task surface. It must support keyboard-first, touch-first, pointer, and mixed-input use without changing its semantic meaning.

Required states include idle, hover where available, focus-visible, pressed, selected suggestion, expanded suggestions, loading, invalid input where applicable, and disabled/read-only states where policy requires them.

Search results and suggestions remain governed by GoreeCloud Search authority. Glaze UI controls their presentation, hierarchy, animation, spacing, focus, and responsive behavior.

## Tabs and workspaces

Tabs must preserve clear active, inactive, hover, focus, loading, attention, pinned, grouped, suspended, and closing semantics without relying on color alone. Motion must be interruptible and must not obscure tab identity during reorder, detach, restore, or close operations.

Compact layouts may restructure tab presentation, but they must preserve the underlying tab model and task continuity.

## Navigation controls

Back, forward, reload/stop, home where enabled, site information, privacy/security state, bookmarks, downloads, profile, and menu controls use governed Glaze iconography and minimum target contracts. Disabled navigation state must remain distinguishable without becoming illegible.

## New Tab and Home

New Tab and Home are first-party GoreeCloud surfaces, not Chromium-themed pages. They should use Glaze responsive composition and integrate GoreeCloud Search, Bookmarks, approved Wayfinder capabilities, and other first-party services only where their actual implementation and authority permit.

## Settings

Settings use semantic grouping, progressive disclosure, search, stable navigation, responsive panes, and accessible control relationships. Privacy Shield, Wardveil Security, Everkeep, Identity, Vault, DNS, Network, Sync, Search, and other service settings must clearly preserve their authority boundaries.

Glaze UI must never turn a planned or unavailable service state into an apparently active control.

## Downloads and Library

Downloads, Bookmarks, history/library experiences, saved content, and related Wayfinder surfaces use consistent collection, selection, progress, error, completion, filtering, sorting, and bulk-action patterns. Destructive operations require appropriately explicit interaction treatment.

## Private Browsing

Private Browsing uses the same current Glaze design system while remaining unmistakably distinct in state and persistence expectations. Visual differentiation must not make unsupported privacy claims. Privacy Shield is authoritative for privacy behavior.

## Security and permissions

Browser-owned security presentation consumes Wardveil evidence-backed state. Certificate details, operating-system dialogs, engine-critical warnings, and platform permission surfaces may retain native or engine-required presentation when replacement would reduce security, accessibility, or compatibility.

Browser-owned permission prompts must prioritize origin identity, requested capability, consequence clarity, keyboard navigation, and safe dismissal over decorative effects.

## Responsive behavior

Glaze layout classes map Browser composition rather than merely scaling controls:

- **Compact** — phone/narrow-window composition; prioritize content and primary navigation.
- **Medium** — tablet/small-window composition with selective secondary panes.
- **Expanded** — desktop composition with persistent Browser chrome and richer parallel context.
- **Wide** — large desktop/TV-capable composition where additional space improves task context rather than stretching content without bound.

Safe areas, system bars, window controls, virtual keyboards, display cutouts, gesture regions, and bounded intrinsic overflow must be handled explicitly.

## Accessibility and adaptation

Every Browser-owned surface must support visible keyboard focus, target floors, increased contrast, reduced transparency, reduced motion, forced-color compatibility where supported, scalable text, localization, and directionality. Functional Glass and Clear Glass must degrade to an appropriate solid treatment when transparency is reduced or readability cannot be guaranteed.

## Production acceptance

A Browser release cannot be accepted as production-ready solely because it compiles against Glaze metadata. Representative task flows must demonstrate current-Stable Glaze conformance across supported platforms and layout/input/accessibility modes.
