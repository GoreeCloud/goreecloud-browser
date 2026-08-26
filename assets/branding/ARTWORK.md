# GoreeCloud Browser Artwork

## Official identity

The official GoreeCloud Browser artwork is **Horizon Portal**.

Canonical sources:

- `goreecloud-browser.svg` — full-color master artwork.
- `goreecloud-browser-monochrome.svg` — authored monochrome identity representation.

Both sources use the Glaze UI 1.5 Stable 1024 × 1024 master application-icon coordinate system.

The previous **Glaze Navigation Ribbon** design was rejected during product review and is not approved GoreeCloud Browser identity.

## Identity lock

Horizon Portal deliberately reduces the Browser mark to a small set of durable geometric characteristics:

1. a rounded GoreeCloud application foundation;
2. a large open circular portal forming the dominant silhouette;
3. a forward-moving horizon/path crossing the portal;
4. an integrated forward arrow at the path terminus;
5. a small central navigation focus;
6. a deep blue-to-violet GoreeCloud relationship in the full-color master.

The open portal represents access to the web without using a generic globe. The horizon path represents navigation and forward movement without borrowing a compass or upstream Browser symbol.

Recognition must remain based on the portal/path silhouette rather than gradients, shadows, or decorative effects.

## Design requirements

The mark is intentionally simpler than the rejected predecessor:

- one dominant silhouette;
- substantially less internal geometry;
- no decorative swooshes;
- no simulated browser-engine symbolism;
- no letterform logo;
- no cloud glyph;
- no globe;
- no dependency on translucent detail for recognition.

Material and depth are subordinate to the identity geometry. The full-color master uses a restrained foundation gradient and a single soft separation shadow. These may be simplified or removed at smaller optical sizes.

## Glaze UI construction

The source follows the Glaze UI application-icon anatomy:

- **Foundation** — rounded 1024-master application field.
- **Material** — restrained tonal depth.
- **Identity** — open portal and forward horizon path.
- **Detail** — central navigation focus.
- **Light** — minimal separation treatment only.

## Optical-size behavior

Display and Standard variants may retain the complete master presentation.

Compact variants should remove shadow before altering identity geometry.

Micro/favicons should prioritize the open portal and forward path. The central focus may be simplified when necessary for legibility, but the open-right portal and forward path must remain recognizable.

## Monochrome and accessibility

The monochrome source is independently authored from the same identity geometry rather than being an automatic grayscale conversion.

High-contrast and reduced-transparency presentations should use solid construction. Identity must remain recognizable without color.

## Platform adaptation

Platform exports may adapt masking, background treatment, optical correction, and detail density while preserving the identity lock.

Android adaptive, round, and monochrome assets must keep defining geometry inside platform-safe regions. Linux, AppImage, web, favicon, and portable derivatives must use the same canonical identity.

## Prohibited substitutions

Do not use Chromium, Chrome, Firefox, Mozilla, Opera, Google, Blink, Gecko, or other upstream artwork as GoreeCloud Browser product identity.

Do not substitute a generic globe, compass, cloud, letter G, or platform Browser glyph for Horizon Portal in normal GoreeCloud product surfaces.

## Provenance

Canonical source hashes are recorded and validated by the repository artwork pipeline. Every generated derivative must record its own SHA-256 and identify the canonical source revision/hash from which it was produced.

## Production acceptance

Artwork is not production-approved merely because canonical SVGs exist. Acceptance requires rendered review at Display, Standard, Compact, and Micro sizes; light/dark contextual review; monochrome and high-contrast review; Launcher/task-switcher comparison; Android adaptive-mask validation; Linux integration validation; and deterministic derivative verification.
