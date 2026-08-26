# GoreeCloud Browser Media Hover

**Status:** Beta-target architecture and product contract  
**Subsystem:** Browser contextual media interaction  
**UI authority:** latest approved Stable Glaze UI  
**Privacy authority:** Privacy Shield  
**Security authority:** Wardveil Security

## Purpose

Media Hover is GoreeCloud Browser's first-party contextual interaction layer for webpage media. It detects supported visual media and exposes a minimal Glaze UI control without replacing website playback controls or unnecessarily obscuring page content. The complete action set remains available through Media Actions and the Browser context menu so hover is never the only access path.

## Supported targets

The detector may identify images, video, GIF/animated images, thumbnails, embedded media, CSS background images, linked media, and supported current video frames. Detection must preserve separate identities for the media resource, containing page, and hyperlink destination.

Activation paths include pointer hover, keyboard focus, touch-and-hold, accessibility navigation, and optional modifier-key activation. Controls automatically reposition at viewport edges and dismiss when interaction ends unless a menu, preview, selection, or accessibility flow remains active.

## Default Glaze UI interaction

The collapsed affordance is a subtle corner control. The default expanded quick row is:

**Preview | Search | Save | More**

Advanced commands belong in **More**. Video playback controls remain visually and semantically distinct from GoreeCloud actions. Reduced motion, larger controls, sufficient contrast, visible focus, screen-reader labels, and non-hover alternatives are required.

Users may independently configure image and video quick actions, hover delay, preview behavior, modifier requirements, save destination, OCR, visual search, AI processing, local/remote processing preferences, and website exceptions.

## Image actions

Image Media Actions include open, new-tab open, preview, enlarge, zoom, actual size, fit, copy, copy address/data where supported, destination-aware saving, sharing, source/details, and developer inspection.

First-party workflows include GoreeCloud Search visual/similar/object/region search; OCR, copy, translation, read-aloud and Advanced Reader Mode; GoreeCloud AI analysis and accessible descriptions; Notes, Memos and Tasks capture; and authorized Drive or Photos storage.

## Video actions

Video Media Actions include play/pause/restart, mute/volume, seek, speed, picture-in-picture, fullscreen, open/new-tab open, permitted download/save, address copy, sharing, details/source, and developer inspection.

Frame workflows include current-frame search, capture/copy/save, region selection, OCR/translation and AI analysis. Transcript generation is exposed only when technically and legally available. DRM, protected-media restrictions, website restrictions and underlying platform capabilities are authoritative.

## Region selection

Select Area creates a rectangular selection over an image or capturable video frame. Supported downstream operations are Search, Copy, Save, OCR, Translate, GoreeCloud AI, Notes, Memos and Tasks. Selection must not bypass DRM, protected surfaces, cross-origin restrictions that the engine must enforce, or Privacy Shield policy.

## Search integration

GoreeCloud Search is the Browser visual-search authority. Subject to capability and permission evidence, it may search complete images, selected regions, video frames, visible text, detected objects, products, landmarks, symbols/logos and concepts, find visually similar content, and combine visual and textual query context.

Visual search must pass Privacy Shield authorization before media leaves the device. UI must identify remote processing when applicable.

## AI integration

GoreeCloud AI may answer questions, explain visual content, identify objects, summarize, produce accessibility descriptions/captions, extract structured information, interpret text, and analyze selected regions.

Every AI action must identify its processing class as **Local**, **GoreeCloud-hosted**, or another explicitly identified remote destination. GoreeCloud Identity, application permissions and Privacy Shield authorization are checked before media is transmitted. Local processing is preferred when configured and supported.

## OCR and Advanced Reader Mode

OCR can process images, screenshots, scanned or photographed pages, infographics, text-heavy images, selected regions, and capturable video frames. Extracted content may flow to copy, translation, read-aloud and Advanced Reader Mode. Privacy Shield controls extracted-text retention and remote processing.

## First-party destination contracts

### Notes

Capture an image, selected region, video reference, captured frame or extracted text with available source page URL, media source, page title, timestamp, attribution and optional user annotation.

### Memos

Create lightweight media references from images, regions, video references or extracted text while preserving source context.

### Tasks

Create a task from media or a region, attach source-page/media references, include extracted text, and accept a user annotation.

### Drive

Save authorized media to a selected GoreeCloud Drive folder with optional rename and source metadata. Drive is labeled **Synchronized** and may participate in separately authorized Search and Everkeep workflows.

### Photos / Video

Images may be imported into GoreeCloud Photos and videos into GoreeCloud Video with supported metadata, organization and source relationships. Captured video frames can be stored separately. Additional metadata remains subject to Privacy Shield.

### Local device

Local storage is labeled **Local** and must never imply synchronization. When destination matters, generic ambiguous `Save` wording is replaced by an explicit destination chooser or destination label. Preferred destinations may be remembered when the user enables that behavior.

## Privacy Shield contract

Privacy Shield governs media transmission to visual search and AI; local-vs-remote OCR/analysis preferences; processing-destination disclosure; extracted-text and AI-metadata retention; cloud synchronization; per-site and per-feature permissions; and application access to hovered media.

No hover event itself grants another GoreeCloud application access to media. A user action and applicable Identity/application authorization are required. Site policy can further restrict processing. Privacy Shield or higher-order policy may override user/site presentation preferences where enforcement requires it.

## Wardveil Security contract

Wardveil may evaluate media URLs, destination URLs, redirects, download names/extensions, mixed/insecure resource state, hosting-domain signals and download risk. Media Actions may expose **Security Details** with evidence-backed state. Unsupported security claims or decorative trust badges are prohibited.

## Linked media

When media is also a hyperlink, the UI maintains two command groups:

**Media:** Open Media, Copy Media, Search Media, Save Media, Media Details.  
**Link:** Open Link, Open Link in New Tab, Copy Link, Save Link, Search Link, Security Details.

The direct media URL and navigation destination URL must never be silently substituted for one another.

## Media information

When technically available, Media Details can expose type/format/MIME, intrinsic and displayed dimensions, resolution, size, source URL/domain, referring page, alternative text, animation/loading state, color profile, secure-resource state, video codec/duration/frame dimensions, and protected/restricted state. Missing metadata is reported as unavailable rather than inferred.

Source attribution can include containing page, direct source and available attribution metadata. Saved research media should preserve its source context where the destination supports it and policy permits it.

## Per-site controls

Modes are: Always Show on Hover, Quick-Action Button Only, Modifier Key Required, Basic Controls Only, and Disabled. Global defaults and site exceptions are persisted in Browser-owned settings. Privacy Shield and Wardveil enforcement may override presentation behavior when necessary.

## Context menu

Right-click exposes the same underlying command model with the complete action set. Media Hover is the fast path; the redesigned GoreeCloud Browser context menu is the complete path. Image, video, link, privacy, security and developer actions remain logically separated and commands must not be duplicated under conflicting implementations.

## Developer actions

Developer-only actions include Inspect Media Element, Open Media Resource, Copy Resource URL, request/loading information, MIME, dimensions, codec information where available, and direct navigation to the relevant Developer Tools element. These commands remain separated from normal Media Actions.

## Architecture

Media Hover is implemented as Browser-owned infrastructure rather than injected page branding. The proposed beta boundary is:

- `MediaTargetDetector` — engine-facing discovery and metadata normalization.
- `MediaTarget` — immutable identity for page, media resource, optional link destination and media capabilities.
- `MediaHoverController` — activation, dismissal, positioning and state transitions.
- `MediaActionRegistry` — capability- and policy-filtered command inventory.
- `MediaActionExecutor` — executes Browser/local/first-party actions through explicit service contracts.
- `MediaRegionController` — visual region selection and capture contract.
- `MediaProcessingPolicy` — Privacy Shield authorization and processing-destination disclosure.
- `MediaSecurityState` — Wardveil evidence/status adapter.
- `MediaDestinationService` — Local/Drive/Photos/Video destination semantics.
- `MediaAttribution` — source/context metadata propagated to supported workflows.

Page detection must not continuously upload, OCR or analyze media. Expensive or privacy-sensitive processing starts only after an authorized feature requires it.

## Beta acceptance requirements

A Media Hover beta is accepted only when the implemented subset has evidence for: target detection; linked-media URL separation; keyboard and pointer access; Glaze UI quick actions; viewport-safe placement/dismissal; image and video capability filtering; explicit Local/Synchronized destinations; Privacy Shield gating; Wardveil state plumbing; context-menu command reuse; per-site disablement; reduced-motion/accessibility behavior; and automated tests for policy and action routing.

AI, visual search, OCR, remote saving, transcripts, protected-media handling, and downstream GoreeCloud application actions must remain capability-gated until their corresponding services are actually available. Documentation must not convert planned integrations into implementation claims.
