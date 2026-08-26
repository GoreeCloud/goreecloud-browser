# Browser Engine Layer

## Purpose

The Browser Engine Layer prevents GoreeCloud Browser from being architecturally synonymous with Chromium.

It defines the minimum engine capabilities the GoreeCloud-owned browser needs while keeping engine-specific object models, callbacks, profile formats, and lifecycle rules out of the application domain.

## Core rules

- GoreeCloud domain objects never expose raw Chromium object pointers in public interfaces.
- Engine-specific types stay inside `browser/engine/<engine>/`.
- Engine events are translated into GoreeCloud event types before entering the browser domain.
- Browser-owned identifiers and persistence remain stable across engine replacements.
- Capability discovery is explicit. The application must not assume every engine implements every optional feature.
- Engine process failures are reported through normalized crash and health events.

## Initial interface surface

The initial engine layer should expose:

- engine initialization and shutdown
- engine context/profile creation
- web-view creation and destruction
- navigation and reload controls
- back/forward history traversal
- title, URL, favicon, loading, and progress events
- new-window and popup requests
- permission requests
- download events
- certificate and transport-security state
- renderer crash state
- zoom and find-in-page
- developer tools availability
- cookie and storage controls required by Privacy Shield
- user-agent and language configuration
- private-context configuration
- engine feature/capability queries

## Capability model

Rather than letting product code branch on engine names, the adapter exposes capability flags such as:

```text
WebView
PrivateContexts
SiteDataControl
CookieInterception
PermissionInterception
DownloadInterception
CertificateInspection
RendererHealth
DevTools
Extensions
HardwareAcceleration
WebRTC
Printing
Accessibility
```

This allows GoreeCloud Browser to select behavior based on implemented capabilities instead of hard-coding Chromium assumptions.

## Chromium adapter

The initial Chromium adapter is responsible for translating between the chosen Chromium embedding API and GoreeCloud interfaces.

It may contain:

- Chromium lifecycle wiring
- browser-process and renderer-process integration
- request/context setup
- native web-view hosting
- navigation callbacks
- permission callbacks
- download callbacks
- certificate/security normalization
- cookie/storage adapters
- renderer crash reporting

The rest of GoreeCloud Browser should not need to know which Chromium embedding mechanism is in use.

## Future engines

A later engine implementation should be addable as a sibling adapter, for example:

```text
browser/engine/chromium/
browser/engine/webkit/
browser/engine/platform/
browser/engine/goree/
```

No future engine is promised merely by this structure. New adapters should be implemented only when there is a real platform or product need.

## Patch policy

When GoreeCloud must patch the underlying engine directly, each patch should include:

- upstream engine version/range
- technical reason
- affected GoreeCloud requirement
- security/privacy impact
- test coverage
- upstream issue/reference when applicable
- removal or rebase criteria

The default preference is adapter-level customization over deep source modification.