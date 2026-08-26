# GoreeCloud Browser — First-Party New Tab, Home, and Settings

GoreeCloud Browser owns these three surfaces completely. They are not restyled Chromium/Firefox pages and must not fall back to inherited consumer-product UI.

## GoreeCloud New Tab

`goreecloud://new-tab` is a fast, low-friction starting surface optimized for opening the next destination.

Primary hierarchy:

1. GoreeCloud Search field.
2. User shortcuts/favorites.
3. Recent GoreeCloud Bookmarks.
4. Read Later items.
5. Recent download summary.
6. Compact Privacy Shield status.

New Tab should feel intentionally lighter than Home. It must open quickly, avoid unnecessary network requests, and remain useful when GoreeCloud services are unavailable.

No advertising, sponsored tiles, promoted stories, paid placement, third-party recommendations, or silently substituted external search provider is permitted.

## GoreeCloud Home

`goreecloud://home` is a richer GoreeCloud Browser dashboard and is the fixed destination of the Home toolbar button.

The Home button does not navigate to New Tab and does not depend on the current search provider or external web service.

Primary Home areas may include:

- GoreeCloud Search.
- Continue Browsing/session restoration.
- GoreeCloud Bookmarks and Read Later.
- Recent GoreeCloud Download Manager transfers.
- GoreeCloud service shortcuts/status.
- Privacy Shield protection summary.
- Wardveil Security state summary backed by actual evidence.
- GoreeCloud Sync/device state where operational.

Home must degrade gracefully when an integrated service is offline. An unavailable service should appear unavailable rather than being replaced by an unrelated third-party feature.

## GoreeCloud Settings

`goreecloud://settings` is rebuilt around GoreeCloud product concepts rather than inherited upstream preference taxonomy.

The top-level information architecture is:

- General
- Appearance
- Search
- Privacy & Blocking
- Wardveil Security
- Passwords & Vault
- Bookmarks & Data
- Downloads
- Network & DNS
- Advanced Proxy Manager
- Permissions
- Containers
- Sync & Identity
- Memory Management
- Private Browsing
- Accessibility
- Advanced
- About

Settings must support search, semantic grouping, responsive master-detail navigation, progressive disclosure, clear descriptions of consequences, reset/recovery paths where appropriate, and direct deep links to individual sections.

Settings must never present a planned integration as active merely because the UI exists. Controls that depend on unavailable service adapters must expose the real state.

## Glaze UI requirements

All three surfaces use the latest approved Stable Glaze UI baseline.

Required behavior includes:

- semantic Glaze color roles;
- Canvas/Solid/Raised/Functional Glass/Overlay materials according to purpose;
- responsive Compact, Medium, Expanded, and Wide compositions;
- 44 px minimum and 48 px comfortable interaction targets;
- visible keyboard focus;
- reduced motion;
- reduced transparency with solid fallback;
- increased contrast;
- forced-color compatibility where supported;
- scalable text;
- localization and text expansion resilience;
- safe-area and virtual-keyboard awareness;
- touch, pointer, and keyboard support.

Glaze treatment is subordinate to legibility, accessibility, security truth, and privacy truth.

## Navigation contract

- Opening a new tab loads `goreecloud://new-tab` unless an explicit destination was supplied.
- The Home toolbar control always loads `goreecloud://home` in the active normal tab unless platform behavior explicitly requires a new surface.
- The Settings toolbar control loads `goreecloud://settings`.
- Internal GoreeCloud destinations are owned by the Browser and must not be sent to GoreeCloud Search as search queries.

## Data and privacy

New Tab and Home must not create behavioral advertising profiles or remote analytics pipelines.

Private Browsing receives a private-appropriate internal surface without leaking ordinary recent history, ordinary clipboard history, or non-private session summaries.

Any surfaced browsing history, bookmarks, downloads, or synchronized state must respect the authority and retention policy of the owning subsystem.

## Production acceptance

These surfaces are not production-ready until representative flows are validated on supported screen classes and input modes, including offline/service-failure states, Private Browsing, accessibility modes, localization stress, and current Glaze UI conformance.
