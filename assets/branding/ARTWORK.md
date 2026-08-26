# GoreeCloud Browser Artwork

## Official identity

The official GoreeCloud Browser artwork is **Glaze Navigation Ribbon**.

Canonical sources:

- `goreecloud-browser.svg` — full-color master artwork.
- `goreecloud-browser-monochrome.svg` — monochrome identity representation.

Both sources use the Glaze UI 1.5 Stable 1024 × 1024 master application-icon coordinate system.

## Identity lock

The Browser identity lock is the combination of:

1. the rounded application foundation;
2. the circular navigation ribbon wrapping the visual center;
3. the forward/upward navigation pointer integrated into the ribbon;
4. the central navigation pivot;
5. the blue-to-violet GoreeCloud identity relationship in the full-color master.

These characteristics define GoreeCloud Browser recognition and should survive future Glaze UI rendering, material, depth, lighting, and platform adaptations.

The artwork intentionally combines three Browser concepts without becoming a literal browser-engine logo:

- **navigation** — directional pointer and forward movement;
- **continuity** — the surrounding ribbon implies persistent browsing/session flow;
- **GoreeCloud identity** — Glaze material, rounded geometry, controlled depth, and GoreeCloud blue/violet presentation.

## Glaze UI construction

The source follows the Glaze UI application-icon anatomy:

- Foundation — rounded 1024-master application field.
- Material — restrained gradient, atmospheric glaze, and soft depth.
- Identity — navigation ribbon and pointer.
- Detail — central pivot and bounded environmental highlight.
- Light — restrained broad highlight and separation shadow.

The primary identity geometry remains centered within the protected icon region. Decorative light is subordinate to recognition.

## Adaptation rules

Platform exports may adapt masking, background treatment, material intensity, optical correction, and detail density, but must preserve the identity lock.

At small sizes, secondary atmospheric detail and shadow may be removed before primary geometry is simplified.

Dark mode must not be produced through simple color inversion. Platform-specific dark presentation may adapt foundation luminosity and material response while preserving the recognizable blue/violet relationship.

Reduced-transparency and high-contrast variants should favor solid construction and stronger separation rather than attempting to preserve translucent effects.

The monochrome source is an authored identity representation and should be used as the basis for Android themed/monochrome presentation and other single-color contexts.

## Prohibited substitutions

Do not use Chromium, Chrome, Firefox, Mozilla, Opera, Google, Blink, Gecko, or other upstream artwork as GoreeCloud Browser product identity.

Do not replace the Navigation Ribbon with a generic globe, compass, letter G, cloud, or upstream browser mark without a separately approved identity revision.

## Provenance

Canonical SHA-256 values:

- `goreecloud-browser.svg`: `bfe541b8721cdacb68c0ff4a84d96f60612bbeb6cbccd352fa911bc2c9fa30ee`
- `goreecloud-browser-monochrome.svg`: `1e22eabf32341d672f0c79368a45013d9d84a181ed53ee1ae750d8103a4b501a`

Future generated derivatives must record their own SHA-256 and identify the canonical source hash from which they were produced.

## Planned derivatives

The production artwork pipeline should deterministically generate the required Browser assets, including web/favicons, Linux icon sizes, AppImage assets, portable Linux assets, and Android adaptive, round, and monochrome resources. Derivative generation does not itself constitute platform acceptance; rendered review and real-platform validation remain required.
