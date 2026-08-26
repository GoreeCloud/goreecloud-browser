# GoreeCloud Browser — Implementation Status

GoreeCloud Browser remains in active development. This document distinguishes implemented development scaffolding from planned production capability.

## Implemented development core

The repository currently contains GoreeCloud-owned contracts and initial runtime scaffolding for:

- Browser engine abstraction (`BrowserEngine`, `EngineContext`, `EngineView`).
- Browser application lifecycle (`BrowserApplication`).
- Browser window and tab lifecycle (`WindowController`, `Tab`).
- Development-only navigation history and view state (`DevelopmentEngine`).
- Engine-independent sessions, containers, bookmarks, downloads, proxy, memory, permissions, capture, Reader Mode, Identity, recovery, platform integration, security state, Glaze UI acceptance, networking policy, and service integration contracts.
- Latest-approved-Stable Glaze UI build baseline enforcement.
- Approved Tabmark artwork and artwork governance.
- Flatpak packaging scaffold for Linux.
- WiX installer scaffold for Windows `.exe` distribution.
- Android Gradle application scaffold with HTTP/HTTPS browser-role intent declarations for future APK builds.

## Development engine

`DevelopmentEngine` is intentionally non-rendering. It exists only so the GoreeCloud Browser-owned runtime can be compiled and exercised before Chromium integration.

It must not be represented as:

- a production rendering engine;
- a Chromium replacement;
- evidence of web compatibility;
- evidence of production browser readiness.

## Chromium integration

`ChromiumEngineAdapter` defines the current integration seam. Chromium/Blink implementation types must remain behind that boundary.

The first adapter milestone requires:

- Chromium runtime initialization and shutdown;
- process/subprocess integration;
- sandbox preservation;
- site isolation preservation;
- GPU integration;
- profile/context creation;
- native web-view creation;
- navigation callbacks;
- download interception;
- permission interception;
- certificate/security-state inspection;
- private-context support;
- site-data clearing;
- renderer crash/health reporting;
- accessibility integration;
- printing and PDF plumbing where applicable.

## Distribution scaffolds

### Linux Flatpak

`packaging/flatpak/io.goreecloud.Browser.yml` establishes the initial Flatpak application identity and permission surface. It is not production-ready until Chromium runtime packaging, sandbox behavior, portals, codecs, desktop metadata, Tabmark derivatives, signing, and installation/upgrade validation are complete.

### Windows

`packaging/windows/GoreeCloudBrowser.wxs` establishes the initial WiX installer definition for an eventual signed Windows installer executable. Production readiness additionally requires signed executable/installer artifacts, supported default-browser registration, application associations, upgrade/repair/uninstall validation, Chromium runtime packaging, Tabmark resources, and release-integrity evidence.

### Android APK

`apps/android` establishes a native Android application module using `io.goreecloud.browser`. The manifest declares launcher behavior and HTTP/HTTPS browsing intents. It deliberately does not implement the Browser as a generic WebView shell. Production APK acceptance requires native Browser runtime integration, Glaze UI, Chromium/approved engine integration, Tabmark adaptive/round/monochrome assets, external signing configuration, upgrade validation, and real-device evidence.

Signing secrets must remain outside source control for every platform.

## Next implementation milestones

1. Implement the Linux Chromium adapter and native application window host.
2. Connect Glaze UI Browser chrome to `WindowController` and `EngineView` state.
3. Implement the address/search controller with GoreeCloud Search authority and direct-navigation separation.
4. Add local profile/session persistence and crash-safe restoration.
5. Implement Privacy Shield interception boundaries and evidence reporting.
6. Route downloads into GoreeCloud Download Manager.
7. Add GoreeCloud Bookmarks local storage and synchronized-tree adapters.
8. Add GoreeCloud Identity, Sync, and Vault service adapters.
9. Add GoreeCloud DNS and Network integration.
10. Make Flatpak, Windows installer, and Android APK pipelines consume exact accepted Browser source and signed release metadata.

## Production rule

Nothing in this file overrides the product acceptance requirements in `PRODUCT_INVENTORY.md`. A feature is production-ready only when its required implementation and acceptance evidence exist.
