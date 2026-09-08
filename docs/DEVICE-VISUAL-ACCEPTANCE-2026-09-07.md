# Device Visual Acceptance — 2026-09-07

Status: Failed — browser information architecture redesign required
Device: OnePlus Nord N200 (`dre`)
ROM: LineageOS 23.2 / Android 16

## Physical-device findings

The current Android beta renders web content but does not yet present a mature browser shell.

Observed problems:

- The address bar and bottom controls feel like a thin WebView wrapper rather than a complete browser.
- There is no visible tab model or tab-switching affordance.
- Browser identity/state is weak: current page title, security/site controls, and browsing context are not clearly represented.
- The bottom toolbar devotes equal weight to five controls without stronger task hierarchy.
- The default `https://search.goreecloud.com/` start destination currently returns `404 (006) pixie proxy, no wildcard URL forwarding configured` on the physical device.
- The failure page becomes the user's first-run/home experience, which is unacceptable even though direct URL navigation remains functional.

## Required redesign

### Browser chrome

- Provide a compact but unmistakable browser toolbar.
- Show site identity/security state without relying on a large literal `HTTPS` label.
- Add a visible tab-count/tab-switching control and a new-tab path.
- Keep Back/Forward/Reload available while reducing bottom-bar visual mass.
- Keep the overflow menu as a secondary control, not the only place browser-level actions live.
- Preserve Glaze UI material hierarchy and one-handed ergonomics.

### Start/new-tab page

- Do not use a broken remote URL as the only browser home surface.
- Provide a GoreeCloud-owned local/native start page or another resilient first-party surface that remains useful when GoreeCloud Search is unavailable.
- The start page should expose the omnibox/search entry, recent/favorite site affordances when implemented, privacy/status cues, and a route to tabs.
- Remote GoreeCloud Search may remain the search provider, but the browser must not become visually broken because the provider endpoint is temporarily misconfigured.

### Reliability

- Separate browser-shell availability from remote search-service availability.
- Preserve direct URL navigation when GoreeCloud Search is unavailable.
- Surface a human-readable search-service-unavailable state rather than a raw proxy 404 as the default experience.

## Acceptance criteria

- On launch, the app is immediately recognizable as a browser.
- A user can identify/open tabs, navigate, reload/stop, open a new tab, and access the menu without guessing.
- Search service failure does not make the browser home surface unusable.
- Direct URLs continue to load when search is unavailable.
- The physical Nord N200 is used for final layout acceptance.
- CI success alone is not sufficient for visual acceptance.
