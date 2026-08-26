# GoreeCloud Browser — Native Advanced Download Manager Architecture

GoreeCloud Download Manager is a first-party Browser subsystem. It replaces the conventional Browser download manager rather than acting as a separate application, extension, or add-on.

## Core transfer engine

Planned capabilities include:

- up to five simultaneous downloads by default;
- adaptive segmented HTTP downloading with up to 16 parts per eligible transfer;
- intelligent acceleration with safe single-connection fallback;
- normal, accelerated, and turbo transfer modes;
- pause, resume, restart, start, stop, and queue controls;
- background operation;
- resumable transfers;
- retry/backoff and automatic recovery after transient failures;
- restart recovery;
- large-file support;
- global and per-download bandwidth limits;
- Wi-Fi-only and metered-network policies;
- connection-specific behavior;
- per-download settings;
- automatic size detection;
- filename resolution and duplicate-name handling;
- batch URL import;
- multiple sequential or parallel queues.

## Browser ingestion

Download requests may originate from:

- intercepted Browser downloads;
- page and link context actions;
- manually entered or pasted URLs;
- Browser-copied URLs offered through the Clipboard Manager workflow;
- supported operating-system share intents;
- batch URL lists;
- authorized media detected in the current browsing context.

The Browser must not continuously monitor unrelated operating-system clipboard activity merely to discover downloads.

## Downloadable content

The manager is intended to handle ordinary authorized web-hosted content including video, audio, images, documents, archives, applications, large files, and other downloadable resources.

Support for a content type or service does not imply permission to bypass authentication, authorization, DRM, paywalls, signed-URL policy, hotlink restrictions, or other technical access controls.

## Media detection

Browser-owned media detection may surface resources already exposed to the active browsing context, including:

- direct media files;
- video/audio resources;
- HLS/M3U8 manifests;
- MPEG-DASH manifests;
- downloadable image/document/application resources;
- associated metadata where legitimately available.

Detected media may be transferred into GoreeCloud Download Manager only within the authorization context in which the Browser received it.

## Torrent subsystem

Torrent support is integrated as a general-purpose transfer subsystem and includes planned support for:

- `.torrent` files;
- magnet links;
- torrent creation from approved local files or URLs;
- metadata inspection;
- file/folder selection;
- content browsing;
- searching, filtering, and sorting;
- tracker configuration;
- seeding;
- share-ratio limits;
- seeding-time limits;
- torrent-specific settings;
- configurable torrent storage locations.

Torrent functionality is subject to the same user-intent, authorization, platform-policy, and lawful-content requirements as other Browser transfers.

## Scheduling and automation

The subsystem supports planned:

- one-time schedules;
- recurring schedules;
- queue-specific schedules;
- automatic resume after network recovery;
- optional low-battery pause behavior;
- configurable post-download actions;
- batch importing;
- download-list backups;
- settings backups;
- recovery and restoration.

## Storage and organization

Storage policy supports:

- configurable default download directories;
- file-type destination rules;
- automatic categorization;
- torrent-specific directories;
- supported removable-storage destinations;
- direct opening of completed files with the operating system's appropriate handler;
- opening the containing directory where supported.

## Interface requirements

The Glaze UI download experience should provide:

- filtering by type and state;
- sorting by order, size, filename, progress, speed, and related properties;
- queue views;
- active and completed views;
- per-download settings;
- live speed, size, ETA, segment, retry, and progress information;
- immediate pause, resume, restart, stop, open, and remove actions;
- responsive desktop, tablet, foldable, and phone layouts;
- dark/light mode, keyboard focus, reduced motion/transparency, and accessibility support.

## Notifications

Platform and Browser notifications may expose:

- progress;
- transfer speed;
- completion;
- failure;
- pause/resume state;
- optional sounds;
- optional vibration where supported;
- persistent progress indicators where supported by the operating system.

## Security and access-control boundary

GoreeCloud Download Manager preserves the Browser's actual security mechanisms including TLS, certificate validation, redirect policy, Safe Browsing, dangerous-file protections, authentication, authorization, and origin/security policy.

It is explicitly prohibited from becoming an access-control bypass. Segmentation, media discovery, custom user agents, retries, torrenting, or other transfer features must not be used to defeat DRM, paywalls, authentication, authorization, service restrictions, or technical controls designed to limit access to protected content.

## Implementation status

The repository now contains first-party contracts for HTTP download management, queues, transfer policy, torrent management, media detection, scheduling/automation, storage routing, backup/restore hooks, and download notifications.

These contracts are architecture and implementation scaffolding. They are not evidence that production transfer engines, torrent protocol support, HLS/DASH extraction, background execution, or platform notifications are complete.
