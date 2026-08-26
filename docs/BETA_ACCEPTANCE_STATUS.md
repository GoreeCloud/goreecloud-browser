# GoreeCloud Browser — Beta 0.1 Acceptance Status

**Version:** 0.1.0-beta.1  
**Channel:** Beta  
**Production approved:** No

This record separates completed source/native-shell beta evidence from renderer and production gates that still require real runtime validation.

## Accepted for the Beta 0.1 source/native-shell milestone

- Browser version/channel metadata identifies `0.1.0-beta.1` and explicitly denies production approval.
- GoreeCloud Browser remains the sole normal user-facing product identity.
- Glaze UI 1.5.0 source revision `2e1618397f6ebcdd254a76bfdd7e98846f2c5aa3` is the enforced Browser baseline.
- Engine-independent Browser core builds and CTest smoke tests are gated in GitHub Actions on Linux Release and Debug configurations.
- The GTK3/X11 GoreeCloud/Glaze native shell has a dedicated GitHub Actions compile/test gate.
- The executable identity is normalized to `goreecloud-browser`.
- Linux desktop and AppStream metadata are present.
- The approved GoreeCloud Browser SVG is installed under the `io.goreecloud.Browser` application identity.
- New Tab, Home, Settings, and Private Start use GoreeCloud internal routes.
- The exact 12-control toolbar and the three integrated unified-search controls are represented by the native shell.
- GoreeCloud Search remains the sole search authority and fails closed when its endpoint is unavailable or unconfigured.
- Direct URL navigation is independent from Search availability.
- Normal and private Browser startup paths are represented, including `--private` and `--isolated-private`.
- Browser Engine Layer, Chromium adapter, CEF runtime delegate, subprocess dispatch, request-context separation, and native-surface contracts are present in source.
- The Flatpak manifest builds the visible beta shell while explicitly leaving CEF disabled until a reviewed runtime module is available.

## Not yet accepted as a render-capable desktop beta

The following still require real CEF runtime evidence:

- Successful build against a reviewed concrete CEF binary distribution.
- Visible CEF/Chromium web-content child surface inside the GoreeCloud GTK shell.
- HTTPS page rendering and ordinary web navigation.
- Real Back, Forward, Refresh/Stop, and renderer navigation callbacks.
- Multi-tab engine-view lifetime under the native shell.
- Private request-context cookie/storage isolation verified against actual websites.
- Per-origin private-data cleanup and permission cleanup. These remain fail-closed where not yet implemented.
- CEF sandbox and site-isolation validation in the packaged runtime.
- Renderer crash/unresponsive recovery behavior.
- X11/XWayland runtime acceptance and later native Wayland support.

## Not accepted for Stable

Stable remains blocked by the full production validation matrix: feature completion where required, Privacy Shield/Wardveil evidence, GoreeCloud service adapters, accessibility, security testing, compatibility, packaging, signing, update/rollback, recovery, artifact provenance, real-device Android acceptance, Windows acceptance, Flatpak runtime acceptance, and sustained daily use.

## Promotion rule

The **Beta 0.1 source/native-shell milestone** may be used for development and testing. A binary must not be described as a **render-capable GoreeCloud Browser beta** until every applicable renderer gate above has direct runtime evidence. Neither beta designation authorizes production claims.
