# GoreeCloud Browser

GoreeCloud Browser is GoreeCloud's first-party native web browser. The browser product, user experience, data model, privacy controls, security services, synchronization behavior, session model, and GoreeCloud integrations are owned by GoreeCloud. A mature web engine is used underneath as an implementation dependency rather than as the identity of the product.

## Product identity

GoreeCloud Browser is the sole normal user-facing Browser product identity. Upstream and third-party implementation technology may appear where technically, legally, operationally, diagnostically, or compliance-wise required, but it does not become GoreeCloud product branding.

## Architectural direction

GoreeCloud Browser is **not** intended to be a conventional Chromium skin or a permanently deep Chromium fork.

The browser is structured so that GoreeCloud-owned application layers remain separated from the underlying rendering engine behind explicit interfaces. Chromium/Blink is the initial engine foundation, while the architecture leaves room for platform-native engines, alternative engines, and a future GoreeCloud-controlled engine or fork when there is a concrete technical reason to do so.

### Core systems

- **GoreeCloud Browser** — native browser application, browser chrome, tab model, profiles, preferences, history, permissions, downloads, sessions, and product experience.
- **Browser Engine Layer** — GoreeCloud-owned abstraction between the application and the underlying web engine.
- **Chromium/Blink** — initial rendering, JavaScript, WebAssembly, Web APIs, media, graphics, accessibility, and web-platform foundation.
- **Glaze UI** — GoreeCloud visual, interaction, responsive, and cross-device interface system.
- **GoreeCloud Wayfinder** — first-party Browser feature family for navigation, productivity, organization, downloads, transfers, capture, sessions, synchronization, Browser utilities, content handling, and workflow integration.
- **GoreeCloud Search** — sole and default integrated search authority for approved Browser search entry points.
- **GoreeCloud Bookmarks** — first-class bookmarks, read-later, collections, and library subsystem.
- **GoreeCloud Sync** — synchronization for approved Browser state while excluding indiscriminate synchronization of sensitive website state.
- **Privacy Shield** — privacy and content-protection authority for tracking protection, filtering, URL cleaning, private-browsing protection, and related controls.
- **Wardveil Security** — security presentation, classification, protection-state experience, and evidence-backed Browser security status.
- **Everkeep** — encrypted browser-state backup, recovery, portability, session preservation, and long-term preservation where appropriate.
- **GoreeCloud Identity** — browser profiles, device authorization, synchronization identity, account-bound preferences, and passkey identity integration.
- **GoreeCloud Vault** — credentials, passkeys, secrets, secure autofill, and other protected authentication material.
- **GoreeCloud DNS** — DNS resolution, resolver policy, DNS privacy, filtering where approved, and Browser DNS integration.
- **GoreeCloud Network** — networking and private-connectivity authority.
- **GoreeCloud Mesh** — coordination and governance layer between GoreeCloud Browser and other first-party GoreeCloud applications and services.

## Design principles

1. GoreeCloud owns the browser product layer.
2. The rendering engine is replaceable infrastructure, not product identity.
3. GoreeCloud-specific behavior should not be scattered through engine source code when a stable adapter boundary is practical.
4. Privacy Shield, Wardveil Security, Everkeep, Glaze UI, and GoreeCloud Mesh are substantive platform systems and their public behavior must stay tied to implemented capabilities.
5. Browser state uses GoreeCloud-owned schemas and services instead of depending on Chromium profile formats as the canonical model.
6. Security updates from the underlying engine must remain practical to consume.
7. Engine-specific exceptions must be documented and isolated.
8. Planned capabilities must not be presented as production-ready until implementation and acceptance evidence exist.
9. Security, privacy, credential, DNS, networking, search, and synchronization authority boundaries remain explicit.

## Initial development phases

### 0.x — GoreeCloud-owned browser shell

Build the native application, Glaze UI browser chrome, tabs, windows, sessions, profiles, settings, permissions UI, history, bookmarks integration, Search integration, downloads, local browser-state storage, and the engine adapter.

### 0.x — First-party browser services

Integrate GoreeCloud Sync, Identity, Vault, Privacy Shield, Wardveil Security, Everkeep, DNS, Network, Wayfinder, and Mesh through explicit service contracts.

### Later — selective engine ownership

Replace, patch, or take greater control of engine subsystems only when GoreeCloud has a concrete privacy, security, performance, compatibility, platform, or product requirement.

## Repository layout

```text
apps/browser/                 Native application shell
browser/core/                 Engine-independent browser domain model
browser/engine/               Browser Engine Layer interfaces
browser/engine/chromium/      Chromium adapter implementation
browser/services/             GoreeCloud service contracts and adapters
browser/storage/              GoreeCloud-owned browser data stores
browser/ui/                   Glaze UI browser components
include/goreecloud/browser/   Public C++ interfaces
src/                          Initial implementation scaffolding
docs/                         Architecture and engineering documentation
```

## Status

GoreeCloud Browser is in active development. Unless explicitly identified as implemented, documented capabilities are product requirements or planned capabilities rather than claims of production readiness.

See:

- [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) — architectural boundaries and ownership model.
- [`docs/ENGINE_LAYER.md`](docs/ENGINE_LAYER.md) — Browser Engine Layer contracts.
- [`docs/PRODUCT_INVENTORY.md`](docs/PRODUCT_INVENTORY.md) — planned inventory, feature requirements, authority boundaries, and production-acceptance direction.