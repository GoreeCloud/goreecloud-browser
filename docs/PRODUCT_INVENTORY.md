# GoreeCloud Browser — Planned Inventory, Features, and Product Architecture

> **Product status:** GoreeCloud Browser is in active development. This document defines the intended product architecture, current design direction, planned capabilities, implementation boundaries, and production-acceptance requirements. Unless explicitly identified as implemented, a listed capability should be understood as a product requirement or planned capability rather than a claim of production readiness.

## 2. GoreeCloud-Only Product Identity

GoreeCloud Browser is designed to present a singular, first-party GoreeCloud product identity across all normal user-facing Browser surfaces.

The GoreeCloud product identity includes:

- GoreeCloud Browser name
- GoreeCloud application identity
- GoreeCloud executable identity
- GoreeCloud package identity
- GoreeCloud launcher
- GoreeCloud About experience
- GoreeCloud Help experience
- GoreeCloud New Tab
- GoreeCloud Home
- GoreeCloud Settings
- GoreeCloud menus
- GoreeCloud download interfaces
- GoreeCloud privacy presentation
- GoreeCloud security presentation
- GoreeCloud default-browser registration
- GoreeCloud application artwork
- GoreeCloud system-integration identity

Firefox, Mozilla, Opera, Google, Chromium, Gecko, SpiderMonkey, and other upstream or third-party identifiers are prohibited from appearing as GoreeCloud product branding.

They must not be presented as:

- Co-brands
- Secondary product brands
- Visible inherited product identities
- User-facing Browser identity
- Default product names
- Consumer-facing service identities
- GoreeCloud feature names

Upstream or third-party implementation identifiers may appear only where technically, legally, or operationally required, including:

- Licensing
- Copyright notices
- Source attribution
- Provenance records
- Diagnostics
- Debugging
- Compliance records
- Technical documentation
- Legal documentation
- Security investigation
- Redistribution documentation

Underlying implementation technology does not acquire product-brand status simply because it contributes code or infrastructure to GoreeCloud Browser.

**GoreeCloud Browser is the sole normal user-facing Browser product identity.**

## 3. Glaze UI Design Language

GoreeCloud Browser is built around the **latest approved version of the Glaze UI Design Language**.

Glaze UI is the authoritative design system for GoreeCloud-controlled Browser surfaces and governs:

- Visual identity
- Layout
- Navigation
- Surface hierarchy
- Component design
- Typography
- Spacing
- Motion
- Input behavior
- Responsive behavior
- Adaptive behavior
- Accessibility
- Focus behavior
- Contrast
- Transparency
- Interaction feedback
- Platform adaptation

Glaze UI applies across Browser-owned experiences including:

- Main Browser chrome
- Navigation controls
- Address and Search interface
- New Tab
- Home
- Settings
- Downloads
- Library
- Bookmarks
- Private Browsing landing page
- Print Preview
- Browser print controls
- PDF viewer controls
- Image-hover download interface
- Clipboard Manager
- Reader Mode
- Context-menu components
- Proxy Manager
- Network & DNS settings
- Memory-management controls
- GoreeCloud service-integration surfaces

The inherited Add-ons Manager is not part of the GoreeCloud Browser experience and is removed.

The current Glaze capability model includes:

- Light mode
- Dark mode
- Canvas surfaces
- Solid surfaces
- Raised surfaces
- Glaze surfaces
- Overlay surfaces
- Adaptive layouts
- Compact layouts
- Medium layouts
- Expanded layouts
- Wide layouts
- 44 px minimum interactive targets
- 48 px comfortable interaction targets
- Standardized motion timing
- Visible keyboard focus
- Reduced-motion support
- Reduced-transparency support
- Increased-contrast support
- Forced-colors support
- Solid fallback when transparency is unavailable
- Accessibility-conscious spacing
- Accessibility-conscious interaction behavior

Glaze is not applied in ways that weaken security, accessibility, or operating-system compatibility.

Security interstitials, Developer Tools, OS-owned dialogs, certificate interfaces, permission surfaces, or similar technically sensitive components may remain material exceptions where aggressive restyling would increase risk.

The design principle is:

**GoreeCloud owns the experience, while security-critical or platform-owned surfaces may preserve necessary implementation behavior beneath the Glaze presentation layer.**

## 4. GoreeCloud Wayfinder

**GoreeCloud Wayfinder** is the first-party native Browser feature family.

Wayfinder encompasses GoreeCloud-owned capabilities relating to:

- Navigation
- Productivity
- Organization
- Downloads
- Transfers
- Capture
- Sessions
- Synchronization
- Browser utilities
- Content handling
- Workflow integration

Features retain functional names while belonging to the broader Wayfinder family.

Examples include:

- GoreeCloud Clipboard Manager
- GoreeCloud Download Manager
- GoreeCloud Proxy Manager
- GoreeCloud Reader Mode
- GoreeCloud Web Clipper
- Memory Manager
- Container controls
- Browser utility actions

Wayfinder is not intended to become a vague replacement name for every Browser subsystem. Individual features remain identifiable by function.

**Privacy Shield remains deliberately outside Wayfinder** because it represents an independent privacy and content-protection authority.

Wardveil Security, GoreeCloud Vault, GoreeCloud DNS, GoreeCloud Network, GoreeCloud Search, and other major GoreeCloud systems likewise retain their own authority boundaries.

## 5. GoreeCloud Search

**GoreeCloud Search** is the sole and default search authority within GoreeCloud Browser.

Search entry points include:

- Address bar
- New Tab
- GoreeCloud Home
- Dedicated Browser search fields
- Approved Browser search surfaces
- Search-selected-text actions
- Context-menu search commands
- Reader Mode search actions where supported
- Other explicitly approved Browser search entry points

External providers must not silently become fallback search engines.

If GoreeCloud Search becomes unavailable:

- Direct URL navigation continues to work.
- Existing web pages remain accessible where otherwise reachable.
- Ordinary browsing remains possible.
- Search attempts visibly fail through GoreeCloud Search.
- The Browser does not silently redirect search traffic to another provider.
- The Browser does not silently substitute Google, Bing, DuckDuckGo, Perplexity, or another third-party search service.

This establishes a clear architectural separation between:

**Web navigation** — reaching a known network destination.

and

**Search authority** — interpreting a search request and returning search results.

Search-service failure must not be disguised through an unauthorized fallback provider.

## 6. GoreeCloud Privacy Shield

**Privacy Shield** is GoreeCloud Browser's native privacy and content-protection system.

It provides, or is designed to provide:

- Native advertising-request blocking
- Native tracker blocking
- GoreeCloud-controlled filtering rules
- Default-on protection
- Master protection control
- Per-component controls
- Per-site compatibility exceptions
- Behavioral third-party tracker detection
- Local tracker-learning evidence
- No remote behavioral-learning service
- URL tracking-parameter cleaning
- Clean navigation URLs
- Clean copied URLs
- Clean shared URLs
- Preservation of required functional parameters
- Compatibility exceptions
- Local substitution of reviewed common CDN resources
- Exact-version resource matching
- Resource-integrity verification
- Resource-provenance verification
- Safe network fallback when an appropriate local resource is unavailable
- Private Browsing protection
- Protection-state presentation
- Site-specific control where approved

Privacy Shield replaces the earlier dependency on uBlock Origin as a managed Browser requirement.

uBlock Origin is not part of the managed GoreeCloud Browser product architecture.

Privacy Shield must not falsely claim protections that are actually implemented by unrelated systems.

Its authority is privacy and content protection, not credential management, DNS authority, network routing, certificate validation, or sandboxing.

## 7. Wardveil Security

**Wardveil Security by GoreeCloud** is the Browser's security-presentation, security-classification, and protection-status identity.

Wardveil may provide:

- Security-status presentation
- Security controls
- Protection status
- Security warnings
- Security-event classification
- Security-facing Browser UI
- Security-state explanations
- Security-event history where permitted
- `wardveil.security` structured events

Wardveil does not pretend to replace the technical security mechanisms beneath the Browser.

Security mechanisms remain governed by their actual technical implementations, including:

- TLS
- Certificate validation
- Safe Browsing
- Sandboxing
- Site isolation
- Process isolation
- Permissions
- Redirect protections
- Download protections
- Update security
- Browser security policy

Security authority remains explicitly separated:

- **GoreeCloud Vault** — credentials and secrets
- **Privacy Shield** — privacy and content protection
- **Wardveil Security** — security presentation, classification, and protection-state experience
- **GoreeCloud DNS** — DNS and resolver authority
- **GoreeCloud Network** — networking and private-connectivity authority

Statements such as **“Protected by Wardveil”** are permitted only where actual technical evidence supports the claim.

Wardveil presentation must describe real protection state rather than act as a marketing substitute for technical security.

## 8. GoreeCloud Sync

**GoreeCloud Sync** is the native GoreeCloud Browser synchronization system.

It replaces the inherited Firefox Sync and Firefox Accounts product experience.

Planned synchronized Browser state includes:

- Bookmarks
- Open tabs
- Approved preferences
- Container definitions
- Container assignment rules
- Approved integration state
- Optional browsing history
- Approved Browser personalization
- Approved device-scoped metadata

The architecture includes:

- Offline-first local queueing
- Device identity
- Device enrollment
- Device revocation
- Conflict handling
- Multi-device convergence
- Explicit data ownership
- Recovery behavior
- Synchronization-state visibility
- Failure handling

Sensitive website state is excluded from indiscriminate Browser synchronization.

This includes:

- Passwords
- Passkeys
- Authentication tokens
- Cookies
- Website sessions
- Website local storage
- Session storage
- Website secrets
- Private authentication state
- Other high-risk site data

Credential and secret synchronization, where supported, is governed by **GoreeCloud Vault**, not ordinary Browser Sync.

Container configuration may synchronize.

Container website sessions do not.

Signing in to GoreeCloud Sync must not silently convert local-only data into synchronized data where explicit user action is required.

## 9. GoreeCloud Containers

GoreeCloud Browser includes native contextual-identity and session isolation.

The architecture does not depend on the Multi-Account Containers extension.

Capabilities include:

- Separate cookies
- Separate sessions
- Separate site identities
- User-created containers
- Rename containers
- Reorder containers
- Delete containers
- Container colors
- Container icons
- Explicit **Open in Container**
- Move a tab between containers
- Move a page between containers
- Persistent hostname assignments
- Per-container data clearing
- Visible container identity
- Private Browsing separation
- Proxy routing by container
- Site assignment rules
- Container-aware Browser actions

Initial identities include:

- Personal
- Work
- Banking
- Shopping

Container definitions and assignment rules may synchronize between approved devices.

The following remain local:

- Cookies
- Sessions
- Local storage
- Authentication state
- Website tokens
- Other sensitive site state

Private Browsing remains separate from normal containers.

A container is not treated as a substitute for Private Browsing.

## 10. Unified Bookmarks

GoreeCloud Browser replaces the inherited user-facing split between Mobile Bookmarks, Toolbar, Menu, and Other Bookmarks with a unified GoreeCloud model.

Two principal bookmark trees exist.

### Local Bookmarks

Local Bookmarks are device-local.

They:

- Remain available while signed out
- Support folders
- Support nested folders
- Preserve ordering
- Remain local unless explicitly moved
- Do not silently synchronize

### GoreeCloud Bookmarks

GoreeCloud Bookmarks are the synchronized authoritative bookmark tree.

They support:

- Synchronization across supported devices
- Folders
- Nested folders
- Saved pages
- Collections
- Tags
- Read Later
- Cross-device organization
- Approved integration workflows

Additional capabilities include:

- Bookmark search
- Sidebar integration
- Panel integration
- Recent bookmarks
- Favorites
- Import
- Export
- Legacy bookmark migration
- Preservation of hierarchy
- Preservation of ordering
- Bookmarks Toolbar as a view rather than an independent storage root

Signing in does not silently upload Local Bookmarks.

Moving Local Bookmarks into GoreeCloud Bookmarks requires explicit user action.

Migration should preserve:

- Folder hierarchy
- Ordering
- Titles
- URLs
- Metadata where supported

## 11. GoreeCloud Vault

**GoreeCloud Vault** is intended to become the Browser's exclusive authority for credentials and approved secret material.

Its authority includes:

- Passwords
- Passkeys
- Credentials
- Secure notes
- Secrets
- Approved authentication material
- Proxy credentials
- Tokens where appropriate
- Other protected GoreeCloud secret data

Browser workflows include:

- Credential autofill
- Password generation
- Passkey use where supported
- Save credential
- Update credential
- Vault lock
- Vault unlock
- Direct GoreeCloud Vault access
- Least-privilege Browser integration
- Secure credential retrieval
- Secure credential handoff
- Approved authentication workflows

Inherited Firefox password-management product surfaces are being removed as GoreeCloud Vault reaches validated operational readiness.

The transition must be evidence-driven.

Inherited password functionality must not be removed before GoreeCloud Vault satisfies required acceptance for:

- Security
- Migration
- Recovery
- Data integrity
- Autofill
- Passkeys
- Locking
- Unlocking
- Failure behavior
- Upgrade behavior
- Real-world daily use

GoreeCloud Vault remains separate from GoreeCloud Sync.

Secrets must not be stored in normal Browser preference files.

## 12. GoreeCloud Download Manager

**GoreeCloud Download Manager** is the Browser's first-party download and transfer authority.

It is designed to be substantially more capable than a conventional Browser download panel.

Ordinary Browser downloads are intended to be routed through the GoreeCloud Download Manager.

Capabilities include:

- Adaptive segmented HTTP downloading
- Up to 16 segments per eligible download
- Safe single-connection fallback
- Five simultaneous downloads by default
- Configurable concurrency
- Pause
- Resume
- Browser-restart recovery
- Automatic retry
- Backoff
- Transient-failure recovery
- Background downloads
- Large-file support
- Multiple named queues
- Manual queue ordering
- High priority
- Normal priority
- Low priority
- Sequential queues
- Parallel queues
- Start All
- Stop All
- One-time scheduling
- Recurring scheduling
- Queue-specific schedules
- Global bandwidth limits
- Per-download bandwidth limits
- Connection-profile bandwidth limits
- Wi-Fi-only downloading
- Metered-network policy
- Connection-type profiles
- Battery eligibility
- Network eligibility
- Automatic pause on disallowed networks
- Automatic resume
- Custom download folders
- File-type destination routing
- Duplicate-name handling
- Batch URL addition
- URL-list import
- SHA-256 verification
- SHA-512 verification
- Content-length validation
- Filtering
- Sorting
- Download properties
- ETA
- Speed information
- Connection information
- Download history
- Restart recovery
- Queue recovery

The Download Manager preserves relevant Browser protections including:

- TLS
- Certificate validation
- Redirect handling
- Safe Browsing
- Dangerous-file protections
- Authentication boundaries
- Authorization boundaries

The Download Manager is explicitly prohibited from circumventing:

- DRM
- Paywalls
- Authentication
- Authorization
- Access controls
- Technical restrictions designed to control access to protected content

Performance features must not become access-control bypass mechanisms.

## 13. High-Quality Image Downloading

GoreeCloud Browser includes a first-party workflow for downloading the highest-quality authorized version of a web image.

When the user hovers over an eligible image, the Browser may expose a direct download control.

Rather than automatically saving only the displayed thumbnail, the Browser evaluates page-authorized candidates such as:

- `currentSrc`
- `src`
- `srcset`
- `<picture>` sources
- Linked original images
- Same-page image metadata
- Other explicitly available image candidates

Selection prioritizes factors such as:

- Intrinsic resolution
- Original-source availability
- Page-provided candidate quality
- File suitability
- Valid authorization context

The selected image is transferred through GoreeCloud Download Manager.

The feature must not bypass:

- Authentication
- DRM
- Paywalls
- Hotlink protections
- Authorization boundaries
- Technical access controls

Its purpose is intelligent selection among content already made available to the user, not circumvention.

## 14. GoreeCloud Proxy Manager

**GoreeCloud Proxy Manager** is built directly into GoreeCloud Browser.

Supported profile types include:

- Direct
- System
- HTTP
- HTTPS
- SOCKS4
- SOCKS5
- PAC

Capabilities include:

- Named proxy profiles
- Quick switching
- Per-site routing
- Per-container routing
- Hostname bypass rules
- CIDR bypass rules
- Fail-closed behavior
- Explicit direct fallback
- Temporary proxy disablement
- Non-secret profile import
- Non-secret profile export
- Local change history
- Planned health monitoring
- Planned latency monitoring
- SOCKS5 remote-DNS option
- Route visibility
- Explicit activation state

Routing precedence is:

**Bypass → Site Rule → Container Rule → Global Profile**

Proxy passwords, tokens, and other secret material are not stored in normal Browser configuration.

They remain under **GoreeCloud Vault** authority.

The default remains Direct/System.

A proxy must never silently activate itself without an approved policy or explicit user action.

## 15. GoreeCloud DNS & GoreeCloud Network

GoreeCloud Browser integrates directly with **GoreeCloud DNS** and **GoreeCloud Network**.

### GoreeCloud DNS

GoreeCloud DNS completely replaces the previous AdGuard Home and Unbound architecture.

GoreeCloud DNS is the GoreeCloud authority for:

- DNS resolution
- GoreeCloud resolver policy
- DNS privacy
- DNS filtering where approved
- Resolver configuration
- GoreeCloud-managed DNS behavior
- Browser DNS integration
- DNS policy enforcement

Browser capabilities include:

- GoreeCloud DNS integration
- Approved system-resolver behavior where required
- Browser DNS cache management
- **Clear DNS Cache**
- **Settings → Network & DNS**
- Optional toolbar **Clear DNS Cache** control
- GoreeCloud-managed DNS policy awareness
- Controlled DNS-over-HTTPS behavior

The Browser must not silently bypass GoreeCloud DNS policy by selecting an unauthorized external resolver.

A Browser-level **Clear DNS Cache** action affects Browser-owned DNS state unless a separate, explicitly authorized GoreeCloud DNS action is invoked.

A normal Browser cache clear does not automatically:

- Reconfigure GoreeCloud DNS
- Replace nameservers
- Change global resolver policy
- Modify operating-system networking
- Disable DNS security controls

### GoreeCloud Network

GoreeCloud Network completely replaces NetBird within the GoreeCloud architecture.

GoreeCloud Network is responsible for applicable:

- GoreeCloud private connectivity
- Network policy
- Private routing
- Secure GoreeCloud service connectivity
- Device-network integration
- Network-state awareness
- Approved network segmentation
- Browser network-policy integration

GoreeCloud Browser may use GoreeCloud Network state for capabilities such as:

- Download eligibility
- Proxy behavior
- Private service connectivity
- Synchronization policy
- Security presentation
- Managed DNS behavior
- Connection classification

GoreeCloud DNS and GoreeCloud Network are related but distinct authorities.

**GoreeCloud DNS governs name resolution.**

**GoreeCloud Network governs GoreeCloud networking and private-connectivity architecture.**

## 16. Built-In Memory Management

GoreeCloud Browser includes first-party memory-management capabilities.

Operating profiles include:

- **Balanced** — default
- **Performance**
- **Memory Saver**

Memory reclamation is designed around staged behavior.

### Stage 1 — Safe Cache Reclamation

The Browser first attempts to recover memory through low-impact cache and resource reclamation.

### Stage 2 — Background-Tab Freezing

Eligible background tabs may be frozen where technically supported.

### Stage 3 — Background-Tab Unloading

Under stronger pressure, eligible background tabs may be unloaded.

Protected workloads include, where detectable:

- Foreground tab
- Active-download tabs
- Tabs containing unsaved work
- Active audio
- Active video
- Picture-in-Picture
- WebRTC
- Pinned tabs
- Recently used tabs
- Explicitly protected tabs

Unloaded tabs retain their identity and navigation history and restore when selected.

Ranking may consider:

- Least-recently-used state
- Estimated process memory
- Estimated tab memory
- User protections
- Activity state
- Restoration cost

The Browser also provides local memory diagnostics.

Memory telemetry and history are not uploaded or synchronized.

Memory-management decisions should remain explainable and recoverable rather than silently destroying user state.

## 17. GoreeCloud Clipboard Manager

The GoreeCloud Browser toolbar includes a first-party **Clipboard Manager**.

The current design supports:

- Last 10 unpinned Browser-copied text entries
- Up to 20 pinned entries
- Copy again
- Pin
- Unpin
- Remove
- Clear recent list
- Exact-duplicate promotion
- Blank-entry rejection
- Oversized-entry rejection

Privacy properties include:

- Clipboard history is memory-only.
- Clipboard history is cleared when the Browser process exits.
- The Browser does not continuously poll the operating-system clipboard.
- Clipboard activity from unrelated applications is not collected.
- Clipboard history is not synchronized.
- Clipboard history produces no behavioral telemetry.
- Clipboard history requires no network access.
- Private Browsing copies are not retained.
- Ordinary-window clipboard history is not shown inside private windows.

The Clipboard Manager captures Browser-initiated copy actions rather than attempting to become a system-wide clipboard surveillance mechanism.

## 18. GoreeCloud Web Clipper

**GoreeCloud Web Clipper** is built directly into GoreeCloud Browser.

It does not require a third-party extension.

Supported destinations include:

### GoreeCloud Bookmarks

- Save page
- Read Later

### GoreeCloud Notes

- Create note from page
- Create note from selected content
- Append to an existing note
- Save excerpts
- Save links
- Preserve source title
- Preserve source URL

Additional approved GoreeCloud destinations may be added over time.

The Browser acts as the capture interface.

The destination service remains the authoritative owner of the stored content.

Web Clipper therefore separates:

**capture intent** from **storage authority**.

The Browser must not claim a successful save until the destination service has actually accepted the operation.

## 19. GoreeCloud Context Menu

GoreeCloud Browser extends the page context menu with context-sensitive GoreeCloud actions.

Implemented or wired actions include:

- Search selected text with GoreeCloud Search
- Save page to GoreeCloud Bookmarks
- Save link to GoreeCloud Bookmarks
- Copy Clean Link
- Download highest-quality image
- Open in GoreeCloud Reader Mode
- Copy page title and cleaned URL
- Capture screenshot
- Copy link to highlighted text

Actions are displayed only where relevant.

Equivalent GoreeCloud actions may be suppressed when a safe native equivalent is already visible and duplication would provide no meaningful value.

Future actions may include:

- Create Memo
- Create Task
- Save to Notes
- Capture contact information
- Create Calendar event

Future service-write actions use a fail-closed capture-intent architecture.

They must not be represented as operationally complete until the required authenticated service adapter exists and successful destination writes can be verified.

## 20. GoreeCloud Reader Mode

**GoreeCloud Reader Mode** provides a first-party GoreeCloud reading experience.

It may retain mature readability-extraction and security foundations where replacing them would provide no meaningful product benefit or would increase compatibility risk.

Capabilities include:

- Detect eligible readable pages
- Enter Reader Mode
- Exit Reader Mode
- Glaze UI presentation
- Light mode
- Dark mode
- Keyboard accessibility
- Visible focus
- Reduced motion
- Reduced transparency
- Increased contrast
- Forced colors
- Comfortable controls
- Private Reader sessions
- No advertising
- No analytics

Reader Mode does not circumvent:

- Paywalls
- Authentication
- DRM
- Authorization
- Technical access controls

Future workflows may integrate Reader Mode with:

- GoreeCloud Bookmarks
- GoreeCloud Notes
- GoreeCloud Memos
- GoreeCloud Tasks
- GoreeCloud Web Clipper

Such integrations remain planned until their actual service adapters exist.

## 21. GoreeCloud New Tab & Home

**GoreeCloud New Tab** and **GoreeCloud Home** are separate first-party GoreeCloud destinations.

They replace inherited consumer-oriented Browser surfaces.

They support the broader GoreeCloud Browser direction through:

- GoreeCloud Search
- GoreeCloud shortcuts
- Approved recent content
- Favorite content
- GoreeCloud Bookmarks integration where approved
- Glaze UI
- GoreeCloud identity
- Privacy-conscious design

They explicitly prohibit:

- Sponsored tiles
- Sponsored stories
- Advertising
- Paid placement
- Sponsored recommendations
- Promoted downloads
- Third-party consumer-product promotion

The New Tab and Home surfaces are Browser utilities, not advertising inventory.

## 22. Curated Browser Settings

GoreeCloud Browser uses a curated settings architecture.

It does not simply expose every inherited preference as a first-class GoreeCloud setting.

Primary settings areas include:

- General
- Privacy & Blocking
- Search
- Passwords & Vault
- Bookmarks & Data
- Network & DNS
- Proxy Manager
- Permissions
- Downloads
- Memory Management
- Private Browsing
- Advanced Settings

Privacy-oriented defaults and policies include:

- HTTPS-Only direction
- Tracking protection
- Cookie controls
- Site-data controls
- Permission controls
- Clear-data controls
- Private Browsing controls
- History controls
- Reduced telemetry
- Studies disabled
- Sponsored content disabled
- Sponsored search suggestions disabled
- Network-prediction restrictions where appropriate
- Unwanted inherited AI or chat product surfaces disabled
- No unauthorized search-provider fallback
- No unauthorized external DNS bypass

Useful functionality may remain where it aligns with GoreeCloud requirements, including:

- Translation
- Accessibility
- Standards support
- Printing
- PDF handling
- Developer-facing compatibility where approved

Settings are intentionally organized around GoreeCloud product concepts rather than inherited upstream product taxonomy.

## 23. Privacy-First Operational Logging

GoreeCloud Browser includes local structured observability without creating a GoreeCloud behavioral-surveillance pipeline.

The design includes:

- Local JSON Lines logs
- Browser-profile storage
- Bounded 2 MiB active log file
- Three rotated backups
- Serialized writes
- No remote exporter
- No durable Private Browsing logs

Forbidden logged information includes:

- Browsing URLs
- Search queries
- Page titles
- Page content
- DOM values
- Form values
- Request bodies
- Response bodies
- Cookies
- Passwords
- API keys
- Tokens
- Authorization material
- Private keys
- Recovery codes
- File paths
- Download paths
- Browsing history
- Referrers
- IP addresses

Operational logging is intended for:

- Debugging
- Reliability
- Security-state investigation
- Local diagnostics
- Failure analysis

It is not intended for:

- Behavioral profiling
- Advertising
- Usage monetization
- Cross-site tracking
- Remote analytics

Private Browsing must not generate durable GoreeCloud operational logs containing private activity.

## 24. Upstream Consumer-Product Removal

GoreeCloud Browser removes or disables inherited consumer-product, sponsorship, advertising, account, recommendation, and cross-product surfaces that do not belong to the GoreeCloud architecture.

This includes applicable inherited:

- Account-product surfaces
- Sync-product surfaces
- VPN promotions
- Relay-style services
- Consumer subscription promotions
- Sponsored recommendations
- Sponsored Home content
- Sponsored search suggestions
- Content-recommendation integrations
- Advertising integrations
- Paid-placement systems
- Third-party consumer-service promotions
- Add-ons Manager product surfaces
- Extension-store promotion surfaces
- Other inherited product branding

The Add-ons Manager is stripped away and removed from GoreeCloud Browser.

This removal does not require the elimination of every internal extension-related subsystem if some low-level capability remains technically necessary.

However:

- Inherited extension-management branding is removed.
- Inherited add-on promotion is removed.
- Inherited extension-store identity is removed.
- The Firefox-style Add-ons Manager is not part of GoreeCloud Browser.

More generally, upstream product identities must not remain visible merely because inherited code still exists internally.

Security, rendering, networking, compatibility, codec, accessibility, and standards foundations may continue where retaining them is safer than replacement.

## 25. GoreeCloud Service Integrations

GoreeCloud Browser is intended to provide first-class workflows across the wider GoreeCloud platform.

Integrated or planned GoreeCloud services include:

- GoreeCloud Search
- GoreeCloud Bookmarks
- GoreeCloud Vault
- GoreeCloud Sync
- GoreeCloud Identity
- GoreeCloud DNS
- GoreeCloud Network
- GoreeCloud Memos
- GoreeCloud Notes
- GoreeCloud Tasks
- GoreeCloud Contacts
- GoreeCloud Calendar
- GoreeCloud Notify
- GoreeCloud Manager
- Future approved GoreeCloud services

Example Browser workflows include:

- Search selected text
- Save a page
- Save a link
- Clip selected content into Notes
- Create a Memo
- Create a Task from a page
- Capture supported contact information
- Create Calendar events from supported page content
- Access GoreeCloud service notifications
- View GoreeCloud service status
- Open GoreeCloud service content from Browser context

Each service retains ownership of its actual data.

The Browser functions as an integration, capture, navigation, and presentation layer rather than becoming the storage authority for every GoreeCloud service.

## 26. Desktop Toolbar

The required default desktop toolbar order is:

**Back → Forward → Home → Reload/Stop → Address & Search → Clipboard → Settings → Downloads**

Optional controls include:

- Clear DNS Cache
- Proxy quick switch
- Other user-customized controls
- Future approved GoreeCloud controls

The Reload control has a GoreeCloud-specific requirement.

While idle:

- Reload invokes the native skip-cache/hard-refresh path.

While a page is loading:

- The same control behaves as the normal Stop control.

Toolbar customization must preserve basic accessibility and navigation safety.

## 27. Extensions and Add-ons

The inherited Add-ons Manager is removed from GoreeCloud Browser.

GoreeCloud's architectural preference is to implement major first-party Browser capabilities natively rather than requiring extensions.

Native capabilities include or are intended to include:

- Privacy Shield
- Containers
- Clipboard Manager
- Download Manager
- Proxy Manager
- Reader Mode
- Web Clipper
- Memory-management functionality
- GoreeCloud service-integration actions

The existence of inherited low-level extension compatibility does not require exposing:

- Firefox Add-ons Manager
- Mozilla extension branding
- Upstream extension stores
- Extension recommendations
- Sponsored extension content
- Upstream add-on product identity

If user-installable extensions remain supported or return in a future GoreeCloud release, they must be exposed through a separately approved **GoreeCloud-owned extension architecture and management experience**.

Such an architecture would require its own:

- Security model
- Permission model
- Installation UX
- Update policy
- Review policy
- Compatibility policy
- Removal controls
- Recovery behavior
- GoreeCloud product identity

Until then, the inherited Add-ons Manager is not part of the GoreeCloud Browser product.

## 28. Private Browsing

Private Browsing remains a genuine isolated browsing mode.

GoreeCloud-specific requirements include:

- Privacy Shield remains active according to private-mode policy.
- No durable GoreeCloud operational logging of private activity.
- Clipboard Manager does not retain Private Browsing copies.
- Normal-window clipboard history is not displayed in private windows.
- Reader sessions remain private.
- Capture and export actions apply appropriate privacy restrictions.
- Container and Private Browsing boundaries remain explicit.
- Private state is not unnecessarily synchronized.
- Private sessions do not become ordinary Sync state.
- Private activity is not used for behavioral analytics.
- Private mode does not silently weaken DNS, network, certificate, or security protections.

Private Browsing is a technical privacy boundary, not merely an alternate color theme.

## 29. Default-Browser Capability

GoreeCloud Browser is intended to qualify as the system default browser on supported platforms.

### Linux

Linux support includes:

- HTTP handling
- HTTPS handling
- HTML handling
- XHTML handling
- Desktop launcher identity
- Default-browser registration
- GoreeCloud executable identity
- GoreeCloud package identity

### Android

Android support includes genuine browser-role behavior for HTTP and HTTPS intents.

GoreeCloud Browser must not behave as a generic WebView application masquerading as a Browser.

The Browser must never silently force itself to become the system default.

Default-browser selection remains an explicit:

- User decision
- Operating-system decision
- Platform-controlled registration process

GoreeCloud may explain the benefits of becoming default, but it must not seize the role without consent.

## 30. Platform Targets

Current product direction includes the following targets.

### First-Class Targets

- Linux desktop
- Android phones
- Android tablets
- Android foldables

### Required Family-Client Targets

- iPhone
- iPad

### Future Candidates

- Windows
- macOS

Android is intended to be a genuine native Browser client rather than a simple WebView shell.

Over time, the Android Browser is intended to support applicable GoreeCloud capabilities including:

- GoreeCloud Search
- GoreeCloud Sync
- GoreeCloud Vault
- GoreeCloud Bookmarks
- Privacy Shield
- Download Manager
- Web Clipper
- GoreeCloud DNS integration
- GoreeCloud Network integration
- Supported isolation capabilities
- Glaze UI
- Private Browsing
- Default-browser role

Platform implementations may differ internally where operating-system rules require it, while preserving GoreeCloud product identity and core architectural principles.

## 31. Linux Distribution

Linux distribution planning includes:

- GoreeCloud executable identity
- GoreeCloud package identity
- GoreeCloud desktop launcher
- Default-browser registration
- Debian packages
- AppImage
- Portable `tar.xz`
- Signing
- Upgrade
- Rollback
- Recovery
- Exact-source provenance
- Package-integrity evidence
- Artifact validation
- Installation validation

Distribution must maintain consistency between:

- Product identity
- Package identity
- Executable identity
- Desktop integration
- Artwork
- Versioning
- Accepted source revision

Release packaging must not reintroduce prohibited upstream product branding into normal GoreeCloud surfaces.

## 32. Android Distribution

Android requirements include:

- Signed APK
- GoreeCloud application identity
- Adaptive launcher icon
- Round launcher icon
- Monochrome launcher icon
- Phone layout
- Tablet layout
- Foldable layout
- Android default-browser eligibility
- Installation validation
- Upgrade validation
- Real-device acceptance
- Browser-role HTTP handling
- Browser-role HTTPS handling
- Glaze UI adaptation
- GoreeCloud product identity

Android production acceptance requires actual device evidence rather than emulator-only success.

## 33. GoreeCloud Browser Artwork

The approved GoreeCloud Browser icon is **Glaze Navigation Ribbon**.

The artwork pipeline includes:

- One canonical SVG source
- Approved monochrome source
- Web derivatives
- Favicon derivatives
- Linux icon sizes
- AppImage assets
- Portable Linux assets
- Android adaptive foreground
- Android round icon
- Android monochrome icon
- SHA-256 provenance
- Deterministic derivative generation

The current pipeline defines **17 cross-platform PNG derivatives** from the approved artwork.

Artwork derivatives must remain traceable to the canonical source.

The artwork pipeline is expected to preserve:

- Deterministic generation
- Exact-source provenance
- Platform suitability
- GoreeCloud identity consistency
- Integrity evidence

Prohibited upstream product artwork must not be used as GoreeCloud product identity.

## 34. Recovery & Resilience

Recovery is a first-class GoreeCloud Browser requirement.

Required acceptance areas include:

- Browser profile backup
- Browser profile migration
- Browser profile recovery
- GoreeCloud Vault migration
- GoreeCloud Vault recovery
- Bookmark synchronization recovery
- Bookmark import recovery
- Bookmark export recovery
- GoreeCloud DNS configuration recovery where applicable
- GoreeCloud Network configuration recovery where applicable
- Safe Browser rollback
- Package rollback
- Upgrade recovery
- Corruption recovery
- Failed-migration recovery
- Device replacement recovery where supported

These requirements align with **GoreeCloud Everkeep**.

Everkeep defines the broader resilience and preservation expectations for GoreeCloud Browser.

A capability is not considered operationally complete merely because it succeeds under ideal conditions.

Critical functionality must also define:

- Backup
- Restore
- Migration
- Failure handling
- Rollback
- Recovery
- Validation

Recovery procedures must not weaken security controls in order to recover data.

## 35. Security Architecture

GoreeCloud Browser is explicitly designed not to weaken mature Browser-security mechanisms for the sake of customization or product differentiation.

Protected technical boundaries include:

- TLS
- Certificate validation
- Safe Browsing
- Site isolation
- Process isolation
- Sandboxing
- Redirect security
- Download protections
- Permissions
- Application updates
- Security updates
- Required security Remote Settings
- Web compatibility
- Private Browsing
- Approved extension-security foundations
- Origin isolation
- Authentication boundaries

GoreeCloud customization must sit above or alongside these mechanisms rather than underneath them in ways that bypass protection.

The long-term preference for GoreeCloud-owned software does not justify replacing secure mature components where replacement would materially increase risk.

Low-level foundations may remain where replacing them would increase risk relating to:

- Security
- Web standards
- Rendering
- Networking protocols
- Cryptography
- Media codecs
- Accessibility
- Interoperability
- Platform compatibility
- Site compatibility

Product ownership and low-level implementation ownership are therefore treated as separate concerns.

## 36. No Advertising / No Sponsorship Business Model

GoreeCloud Browser explicitly prohibits Browser-provided:

- Advertising
- Sponsored tiles
- Sponsored stories
- Paid placement
- Sponsored downloads
- Sponsored search recommendations
- Paid search-provider substitution
- Promoted extensions
- Promoted add-ons
- Paid homepage placement
- Sponsored New Tab content

The Browser also avoids unnecessary behavioral analytics.

This is a product-level policy rather than merely a default preference.

GoreeCloud Browser surfaces are intended to serve the user rather than function as advertising inventory.

## 37. Release Integrity

Future Stable GoreeCloud Browser artifacts must be cryptographically and evidentially bound to the exact accepted source.

Covered release artifacts include:

- Linux AppImage
- Debian package
- Portable Linux archive
- Signed Android APK
- Other future approved production artifacts

Evidence records include:

- Filename
- Platform
- Artifact type
- SHA-256
- Byte size
- Signature scheme
- Signer identity
- Signature SHA-256
- Acceptance-record SHA-256
- Exact 40-character Git source revision

Signing secrets are prohibited from:

- Source repositories
- Release evidence records
- Build manifests
- Public artifacts
- Diagnostic output

The release-integrity chain is intended to establish traceability from:

**Accepted source revision → build output → artifact hash → signature → acceptance evidence**

A production artifact that cannot be tied to the exact accepted source is not considered a valid Stable release.

## 38. Production Validation

Continuous integration does not have authority to declare GoreeCloud Browser production-ready.

CI may establish that code:

- Compiles
- Passes automated tests
- Produces artifacts
- Satisfies defined static checks

Production acceptance requires real evidence.

Applicable validation areas include:

- Actual compiled Browser
- Linux runtime
- Android runtime
- Real-device behavior
- GoreeCloud Search
- Privacy Shield
- GoreeCloud Vault
- GoreeCloud Bookmarks
- GoreeCloud Sync
- GoreeCloud DNS
- GoreeCloud Network
- Download Manager
- Proxy Manager
- Clipboard Manager
- DNS behavior
- Network behavior
- Memory management
- Default-browser registration
- Latest approved Glaze UI Design Language
- Accessibility
- Private Browsing
- TLS
- Certificate validation
- Safe Browsing
- Sandbox
- Process isolation
- Site isolation
- Web compatibility
- Any approved extension-compatibility layer
- Installation
- Upgrade
- Rollback
- Recovery
- Artifact signing
- Release integrity
- Trademark review
- Redistribution review
- Sustained daily use

Validation must include failure scenarios, not only successful operation.

Examples include:

- GoreeCloud Search unavailable
- GoreeCloud DNS unavailable
- GoreeCloud Network unavailable
- Interrupted download
- Browser crash during transfer
- Sync conflict
- Device revocation
- Vault lockout
- Profile migration failure
- Upgrade failure
- Rollback
- Corrupted local state
- Network transition
- Private Browsing boundary checks

**GoreeCloud Browser remains officially in active development and is not production-approved until the required acceptance evidence exists.**

# Overall GoreeCloud Browser Product Architecture

GoreeCloud Browser is intended to be substantially more than a rebranded or cosmetically modified upstream browser.

Its planned architecture is:

**GoreeCloud Browser** → **Latest Glaze UI Design Language** — interface, interaction, accessibility, responsive design, and visual architecture → **GoreeCloud Wayfinder** — first-party Browser feature family → **Privacy Shield** — privacy and content protection → **Wardveil Security** — security presentation, classification, and protection-state experience → **Everkeep** — resilience, preservation, migration, rollback, and recovery requirements → **GoreeCloud Search** — Browser search authority → **GoreeCloud Sync + GoreeCloud Identity** — identity, devices, and synchronized Browser state → **GoreeCloud Vault** — credentials, passkeys, authentication material, and secrets → **GoreeCloud DNS** — DNS resolution, filtering, privacy, and resolver-policy authority → **GoreeCloud Network** — GoreeCloud networking, private connectivity, and network-policy authority → **GoreeCloud Bookmarks** — saved web-content authority → **Containers** — identity and session isolation → **GoreeCloud Download Manager** — Browser download and transfer authority → **GoreeCloud Proxy Manager** — Browser proxy and routing controls → **GoreeCloud Clipboard Manager** — privacy-conscious Browser clipboard history → **GoreeCloud Reader Mode** — focused reading environment → **GoreeCloud Web Clipper** — Browser capture interface → **Memory Manager** — Browser resource management → **GoreeCloud Mesh and service integrations** — integration with the broader GoreeCloud platform

## Architectural Direction

The central long-term direction is the transition toward **original GoreeCloud-owned Browser software and first-party GoreeCloud product architecture**.

Inherited complete-product implementations are treated as:

- Migration foundations
- Compatibility foundations
- Reference implementations
- Temporary implementation dependencies
- Sources of mature low-level technology where justified

They are not automatically treated as the permanent GoreeCloud product architecture.

At the same time, GoreeCloud Browser does not replace mature technology merely for ideological purity.

Low-level foundations may remain where replacing them would materially increase risk involving:

- Security
- Rendering
- Web standards
- Networking
- Cryptography
- Codec support
- Accessibility
- Platform integration
- Browser compatibility
- Interoperability

The architectural boundary is therefore:

**GoreeCloud owns the Browser product, product identity, design language, user experience, first-party features, service integration, privacy architecture, recovery requirements, and user-facing platform.**

**Underlying technology may remain where technically justified, but it does not retain independent product-brand status inside GoreeCloud Browser.**

Firefox, Mozilla, Opera, Google, Chromium, Gecko, SpiderMonkey, and other upstream or third-party identifiers are prohibited from GoreeCloud product branding and may appear only where required for technical, diagnostic, provenance, licensing, attribution, compliance, or legal purposes.
