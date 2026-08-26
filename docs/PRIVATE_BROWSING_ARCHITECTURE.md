# GoreeCloud Browser — Private Browsing Architecture

**Status:** active development. These are product and implementation requirements, not production-readiness claims.

Private Browsing is a first-party GoreeCloud Browser isolation mode built with the latest approved Glaze UI. It provides temporary local browsing state, explicit private-session lifecycle controls, Privacy Shield and Wardveil protection, and transparent boundaries between temporary and persistent GoreeCloud services.

## Session model

Private browsing supports a shared temporary private session and independently isolated private-window sessions. Normal windows never inherit private cookies, storage, authentication, permissions, history, or search state. Private state is excluded from ordinary startup/session recovery and GoreeCloud Sync.

Private session storage includes isolated cookies, cache, local storage, IndexedDB, service workers, authentication state, permissions, and temporary navigation history. Applicable state is destroyed when its owning private session is forgotten.

## Glaze identity and start page

Private windows use a distinct Glaze treatment across window chrome, tab strip, unified search bar, menus, and the dedicated private start page. A persistent private indicator opens the Private Session Dashboard. The start page explains both active protections and the limits of private browsing; GoreeCloud Browser never describes private mode as guaranteed anonymity.

## Protection authorities

Privacy Shield remains active and reports concrete tracking/cookie/fingerprinting/referrer/link protections. Wardveil remains active for malicious-site, phishing, certificate, redirect, mixed-content, permission-abuse, and download-security state. GoreeCloud DNS and GoreeCloud Network status remain independently visible. Private Browsing never weakens TLS, certificate, DNS, network, sandbox, or other security controls merely to create isolation.

## Search and synchronization

GoreeCloud Search is the sole private search authority. Private searches do not enter normal Browser history, normal personalization, or GoreeCloud Sync. Private tabs, recently closed tabs, cookies, storage, history, and search state remain outside ordinary synchronization.

## Advanced Tab Manager

Private tabs support ordinary applicable tab-management operations plus moving to new/isolated private windows, Forget This Tab, Close & Forget Window, and Close & Forget Session. Recently closed private tabs exist only inside the active owning private session. Private tabs are never serialized into normal Advanced Tab Manager recovery.

## Forget and inspection

The Private Session Dashboard exposes open tab count, trackers blocked, temporary permissions, private downloads, sites holding temporary data, and the current Privacy Shield, Wardveil, DNS, Network, and Sync states.

Users can selectively clear cookies, cache, site storage, authentication, permissions, history, search state, per-site state, or all session state. The data inspector exposes temporary cookies, cache, local storage, IndexedDB, service workers, permissions, and authentication state by site where technically available.

## Downloads

Private downloads are marked as private-session transfers and their Browser history is temporary. File persistence is independently configurable as Keep File, Delete When Session Closes, or Ask Each Time. The UI must warn that a downloaded file can survive outside private browsing. Wardveil download protections remain active.

## Bookmarks and persistent-service boundaries

Private pages may be explicitly saved to Local Bookmarks or GoreeCloud Bookmarks. The destination is always explicit and the Browser warns that bookmarking preserves the address after the private session ends.

Sending private content to Notes, Memos, Tasks, Documents, other GoreeCloud applications, or a persistent AI conversation is an explicit private-to-persistent transition. The Browser must disclose this boundary before committing the write and must not claim success until the destination service confirms it.

## Unified Vault and autofill

Private sessions may access Local Vault, GoreeCloud Vault, both, or neither according to policy. Credential source is visible. Optional per-session authorization/private-window authentication can gate Vault access. Private form input does not automatically train or populate persistent autofill history.

## Permissions and Location

Sensitive permissions default to temporary lifetimes where practical: Allow Once, Allow for Private Session, or Block. This applies to camera, microphone, location, notifications, Bluetooth, USB, MIDI, clipboard, and other supported sensitive capabilities. GoreeCloud Location can expose Blocked, Approximate, or Precise location with temporary authorization semantics.

## Clipboard and OS privacy

Private Browser copies never enter ordinary GoreeCloud Browser clipboard history or synchronized clipboard state. Supported platforms may provide configurable screenshot, task-switcher, recent-app, and window-preview protection. These capabilities are reported only when the operating system actually enforces them.

## Window locking

Private windows may be locked after inactivity, device lock, Browser backgrounding, or user switching. Unlock can use authorized GoreeCloud Identity or platform authentication. An optional higher-security policy closes private windows when the Browser locks.

## GoreeCloud AI and Reader Mode

Advanced Reader Mode remains temporary unless the user explicitly saves content. Private AI interactions default to no persistent AI history, no persistent memory, no content indexing, and no cross-device context. Private-page access is permission-bound and temporary unless explicitly preserved.

## End-of-session evidence

Close & Forget destroys the applicable private session state. A non-persistent cleanup summary may report categories removed and deliberately preserved items such as downloaded files or bookmarks. The summary itself must not become a durable activity record.

## Settings

Settings → Privacy & Security → Private Browsing controls protection level, Privacy Shield, Wardveil, private Search/Sync/DNS/Network behavior, Vault/autofill access, download persistence, screenshot/task-switcher protection, window locking, permissions, Location, clipboard protection, AI, Reader Mode, shared/isolated sessions, automatic clearing, Close & Forget, and keyboard shortcuts.

## Guarantees

Private state is temporary by default, local by default, and excluded from synchronization by default. Persistent transitions require explicit user intent. Privacy Shield and Wardveil remain active. Integrated GoreeCloud services receive no privilege to bypass permissions. Protection claims must be evidence-backed, and Private Browsing is presented as local browsing-state privacy and isolation rather than network anonymity.
