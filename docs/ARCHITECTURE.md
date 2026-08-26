# GoreeCloud Browser Architecture

## Goal

GoreeCloud Browser is a GoreeCloud-owned browser product that uses an external web engine without allowing that engine to define the product architecture.

The application layer owns browser behavior, persistent data, policy, user experience, service integration, and product-level state. Engine-specific code is kept behind the Browser Engine Layer wherever practical.

## Top-level architecture

```text
+--------------------------------------------------------------+
|                     GoreeCloud Browser                       |
|  Glaze UI | Tabs | Windows | Sessions | Profiles | Settings |
+--------------------------------------------------------------+
|                    Browser Domain Layer                      |
| History | Permissions | Downloads | Navigation | Collections|
+--------------------------------------------------------------+
|                  GoreeCloud Service Layer                    |
| Search | Bookmarks | Sync | Identity | Vault | Mesh         |
| Privacy Shield | Wardveil Security | Everkeep               |
+--------------------------------------------------------------+
|                   Browser Engine Layer                       |
| EngineContext | WebView | Navigation | Events | Capabilities|
+--------------------------------------------------------------+
| Chromium/Blink initially | other supported engines later     |
+--------------------------------------------------------------+
```

## Ownership boundary

### GoreeCloud-owned

GoreeCloud Browser must treat the following as canonical GoreeCloud product responsibilities:

- browser chrome and interaction model
- tab groups, tab lifecycle, split views, windows, workspaces, and sessions
- profiles and account linkage
- preferences and policy evaluation
- browser history and navigation metadata
- bookmarks, read-later, collections, and library semantics
- permissions UX and GoreeCloud permission policy
- search experience and GoreeCloud Search integration
- download UX, metadata, policy, and security workflow
- synchronization schemas and conflict behavior
- browser state persistence and portability
- privacy reporting and privacy-control surfaces
- security reporting and malicious-content protection surfaces
- encrypted recovery and preservation workflows
- credential, passkey, and secure-autofill integration
- GoreeCloud inter-application coordination

### Engine-provided initially

The initial engine may provide implementations for:

- HTML and CSS processing
- JavaScript and WebAssembly execution
- DOM and browser Web APIs
- layout and paint
- graphics and GPU compositing
- network primitives required by the web platform
- media playback and capture primitives
- WebRTC primitives
- accessibility foundations
- process isolation and renderer sandbox foundations
- standards compatibility

Engine implementation details must not become the canonical storage format or public GoreeCloud API unless explicitly adopted.

## Browser domain objects

The first stable domain model should include:

- `BrowserApplication`
- `BrowserWindow`
- `Tab`
- `TabGroup`
- `BrowsingSession`
- `BrowserProfile`
- `NavigationEntry`
- `PermissionDecision`
- `DownloadItem`
- `HistoryEntry`
- `BrowserPreference`
- `EngineContext`
- `EngineView`

Domain identifiers should be GoreeCloud-owned UUIDs rather than engine object identifiers.

## Service contracts

Browser features integrate with GoreeCloud platform systems through narrow service interfaces. Examples include:

- `SearchService`
- `BookmarkService`
- `SyncService`
- `IdentityService`
- `VaultService`
- `PrivacyService`
- `SecurityService`
- `RecoveryService`
- `MeshService`

The browser must remain operable in a meaningful local mode when remote GoreeCloud services are unavailable, except for capabilities that inherently require those services.

## Privacy boundary

Privacy Shield is responsible for product-level privacy policy and evidence-backed privacy state. The Browser Engine Layer exposes the controls and observations needed to enforce that policy, including storage, cookies, tracker intervention points, permission requests, private-context state, and site-data clearing.

Private browsing must be modeled as a GoreeCloud contract rather than merely toggling an engine incognito flag. The contract should define which browser-owned systems may persist data, which services receive events, and what is excluded from synchronization, recovery, history, suggestions, and analytics.

## Security boundary

Wardveil Security evaluates and communicates product-level security state. Engine security mechanisms remain essential, but their state is normalized through GoreeCloud interfaces so the browser can reason about:

- transport and certificate state
- unsafe navigation decisions
- malicious or suspicious downloads
- phishing and deceptive-site signals
- renderer isolation and sandbox health
- mixed-content state
- permission abuse signals
- security incidents and user-facing reports

## Storage boundary

GoreeCloud-owned databases are canonical for product data. Engine profile databases may exist as implementation details but should not be the source of truth for GoreeCloud tabs, bookmarks, history semantics, sessions, preferences, collections, or synchronization records.

Initial local storage should use explicit versioned schemas and migrations.

## Engine upgrades

The engine adapter should be the primary compatibility boundary for engine upgrades. GoreeCloud-specific engine patches, when unavoidable, must be:

1. minimal,
2. documented,
3. covered by regression tests,
4. tracked against the upstream engine version, and
5. reviewed for possible removal on every major engine upgrade.

## Platform systems

Glaze UI, Privacy Shield, Wardveil Security, Everkeep, and GoreeCloud Mesh are platform systems with functional responsibilities. Browser integrations must expose implemented behavior and evidence rather than treating these systems as decorative branding.

## Initial implementation target

The first implementation target is a native desktop shell with an engine-neutral C++ domain/API layer and an initial Chromium-family adapter. The precise embedding technology can evolve without changing the browser-owned contracts above.