# GoreeCloud Browser User Manual

## Current availability

GoreeCloud Browser is in active beta development. The Android target is a real installable test APK, but it is not production-approved or Stable.

Current Android beta identity for this source revision:

- Application: **GoreeCloud Browser Beta**
- Package: `io.goreecloud.browser.beta`
- Version: `0.1.0-beta.1+android.19`
- versionCode: `10019`
- Minimum Android: Android 8.0 / API 26
- Target API: 35
- Rendering dependency: Android System WebView/Chromium
- Current Browser-owned design-system target: GLAZE UI V1.2 / `1.2.0` Stable

GoreeCloud owns the Browser product layer, navigation/search behavior, mobile browser chrome, privacy defaults, security gates, and GoreeCloud integrations. Android System WebView/Chromium provides the web rendering/runtime foundation and is not the product identity.

The `+android.19` Development candidate is stacked on the V1.2 source migration, `+android.8` accessibility-chrome hardening, `+android.9` large-text reflow work, `+android.10` RTL directionality hardening, `+android.11` string-resource localization readiness, `+android.12` debug pseudolocale testability, `+android.13` bidi-presentation hardening, `+android.14` explicit-port presentation correctness, `+android.15` navigation-authority validation, `+android.16` malformed-web external-handoff hardening, `+android.17` bare-userinfo search-routing hardening, and `+android.18` local/active-content external-handoff deny hardening. It adds a separate outward page-address disclosure rule: Browser keeps the exact authoritative HTTP(S) URL for navigation and focused editing, but Copy page address and Share page remove explicit userinfo before the address is exposed to the Android clipboard or share targets.

The parent debug beta still generates synthetic Android pseudolocales `en-XA` and `ar-XB` for localization stress testing. Pseudolocales are test resources, not translations. Their presence in the APK does not establish rendered pseudolocale behavior, translation quality, or localization acceptance. Likewise, the address-presentation, navigation-authority, external-handoff, bare-input classification, bounded local-scheme, and outward-disclosure hardening do not establish complete URL/userinfo or credential safety, email detection, phishing protection, clipboard privacy, Android share-target trust, URL-spoofing, DNS, origin, IDN/confusable, certificate, complete scheme/intent/deep-link safety, external-application trust, or bidirectional-editing acceptance.

## Installing the Android beta

The CI-generated APK is debug-signed for testing. Use an APK supplied from the GoreeCloud Browser Android Beta workflow or another explicitly supplied GoreeCloud beta artifact tied to an exact source revision.

Android may require you to allow installation from the app used to open the APK. Enable that Android setting only for the trusted installation source you intend to use.

The current CI beta signing key is not the production signing authority. Fresh CI environments can produce different debug certificates. If Android reports that an update cannot be installed because signatures differ, remove the older beta package and perform a fresh install. Removing the beta can remove its local application data.

Do not treat the beta APK as a production release, managed update channel, or long-term data-preservation target.

## Mobile browser chrome

The Android shell uses two Browser-owned chrome regions:

- a top omnibox; and
- a bottom navigation toolbar.

The web page occupies the full region between them.

### Omnibox

The top omnibox reserves most of its width for the unified address/search field. When the field is not being edited, Browser presents a condensed address with the hostname kept at the leading edge. The condensed presentation removes Unicode bidirectional-formatting controls that can affect visual ordering. Ordinary RTL letters are not removed by this presentation step.

If the parsed URL contains an explicit port, the condensed presentation keeps that port visible. For example, `https://example.com:8443/path` is presented as `example.com:8443/path`, and an explicitly written `https://example.com:443/` remains distinguishable as `example.com:443`. Browser does not silently hide an explicitly specified port merely because it is the scheme's default port.

Focus the field to expose and select the complete original current URL for editing. The unfocused presentation helper does **not** rewrite the authoritative URL used for navigation, search resolution, WebView requests, or persisted Browser state. The `+android.19` outward-disclosure policy is separate: Copy page address and Share page may remove explicit HTTP(S) userinfo before disclosure without changing the authoritative focused/navigation URL.

The leading `HTTPS`, `HTTP`, or `WEB` label reports the parsed address scheme. It is not a Wardveil verdict, certificate-verification badge, or claim that a page is trustworthy.

Use the arrow action beside the address field or the Android keyboard's Go action to navigate.

Enter a structurally valid complete `https://` or `http://` URL to navigate directly. Browser validates the local HTTP(S) URI authority shape before allowing direct navigation. Malformed explicit HTTP(S) input—for example an empty authority, a non-numeric/out-of-range explicit port, or an explicit `http:` / `https:` form that omits `//`—fails closed to **GoreeCloud Search Home** instead of being forwarded to WebView or silently sent to GoreeCloud Search as query text.

A recognized bare host such as `example.com` is upgraded to HTTPS before navigation. The current resolver also recognizes valid explicit ports on bare hosts, including inputs such as `localhost:8080/path` and `[::1]:8080/path`, which become HTTPS navigation targets. Optional authority ports must be decimal values from `0` through `65535`.

Unschemed input whose authority-shaped prefix contains literal `@` is not inferred as a bare host. For example, `person@example.com` and `person@localhost:8080` remain ordinary GoreeCloud Search input rather than being transformed into `https://person@...` navigation targets. This is a narrow bare-input classification rule, not email-address validation. An explicit URL such as `https://person@example.com/path` remains governed by the explicit HTTP(S) URL contract and is not rewritten for navigation by this slice.

Valid Unicode HTTP(S) host input remains eligible for direct navigation in this bounded source contract. This does **not** mean Browser has completed IDN normalization, Unicode-confusable review, DNS validation, certificate validation, or origin-trust acceptance.

Text that is not interpreted as a URL is sent to **GoreeCloud Search**, which is the sole integrated search authority for this beta. Browser does not silently fall back to another search provider.

### Large text and font scaling

The normal omnibox baseline is 56dp inside Browser's top chrome, but text-bearing top chrome is no longer constrained to that exact height. Android-native text measurement can grow the scheme/address area when system font scaling requires additional vertical space.

The unified address field intentionally remains a single-line browser control. Its row can become taller as the Android font grows; long addresses continue to use normal single-line horizontal editing rather than becoming multi-line web addresses.

Browser-menu action rows also use content height with a 56dp minimum, so a scaled or wrapped action label can request more vertical room instead of being clipped into a fixed 56dp row.

The source contract records 2.0 font scale as the downstream large-text acceptance target. The current source/build state does **not** prove that 200% text has passed representative device, screen-size, orientation, or localization acceptance.

### RTL directionality

The Android application declares RTL support. The directional Back and Forward vector controls use Android auto-mirroring so their glyph direction follows the platform layout direction rather than remaining permanently left/right facing.

The current programmatic Browser chrome also uses logical start/end horizontal margins in the validated source path rather than physical left/right margin fields.

For the address field, the current source removes Unicode bidi-formatting controls only from the **unfocused condensed presentation**, preserves ordinary RTL letters, and keeps any explicitly parsed URL port visible. Focusing the omnibox exposes the untouched original URL. This bounded behavior is intended to reduce presentation ambiguity without changing navigation authority.

This does **not** mean the beta is fully localized or that URL spoofing/origin/bidirectional behavior is completely solved. Complete DNS/origin semantics, IDN/confusable handling, bidirectional URL/address editing, rendered RTL layout, representative device review, and combined RTL + large-text + accessibility behavior remain open.

### Localization readiness and pseudolocales

Browser-owned natural-language Android chrome copy comes from `res/values/strings.xml` rather than being embedded directly in `BrowserActivity.kt`. This includes the omnibox hint and accessibility description, navigation-control descriptions, menu labels, safety/download/link feedback, clipboard text, chooser title, and beta information.

`HTTPS`, `HTTP`, and `WEB` remain explicitly non-translatable because they are protocol/status tokens rather than natural-language UI copy.

The debug beta generates Android pseudolocales:

- `en-XA` — synthetic accented/expanded text for layout and string-expansion testing; and
- `ar-XB` — synthetic bidirectional/RTL-oriented text for directionality stress testing.

CI verifies that both generated pseudolocale variants are present in the built debug APK. This is a testability feature only. The current default resource set is still English, and no actual translation pack is established by this work. Rendered pseudolocale review on supported test devices remains pending.

This source/build boundary does **not** establish translated resources, translation completeness or quality, locale fallback, plural/grammar behavior, complete DNS/origin/Unicode/IDN/confusable handling, bidirectional address editing, rendered RTL locale acceptance, or combined locale + 200% text + assistive-technology/device acceptance.

### Scroll-aware and accessibility-aware chrome

When a page has been scrolled meaningfully downward, Browser can hide the top omnibox to return more vertical space to the page. The 56dp bottom toolbar remains available.

The top omnibox returns when you:

- scroll upward;
- return near the top of the page;
- focus the omnibox; or
- start a new navigation.

When Android reports that an accessibility service is enabled, Browser keeps the top omnibox visible and restores it immediately if accessibility becomes enabled while Browser is active. Browser uses only Android's boolean accessibility-enabled state for this behavior; it does not enumerate service identities or inspect accessibility event/user content.

This source behavior improves primary-control discoverability but does not itself establish TalkBack, Switch Access, Voice Access, focus-order, announcement-quality, or physical-device accessibility acceptance.

### Bottom navigation

The fixed 56dp bottom toolbar contains:

- Back;
- Forward;
- GoreeCloud Search Home;
- Reload, which becomes Stop while a page is loading; and
- Browser menu.

Back and Forward visibly disable when no matching history action is available. Their directional glyphs follow Android layout direction through native vector auto-mirroring.

Page-loading progress is drawn at the top of the web-content region rather than using a separate toolbar row.

### Browser menu

The Browser menu uses a Browser-owned Glaze bottom sheet rather than Android's platform-default popup menu.

The current bounded menu actions are:

- Copy page address;
- Share page; and
- About this beta.

For a normal page URL without explicit userinfo, Copy page address and Share page disclose the existing page URL unchanged. For a structurally parseable HTTP(S) URL with explicit userinfo, such as `https://user:password@example.com:8443/path?q=1#part`, Browser removes only `user:password@` before the address leaves Browser. The disclosed value retains the remaining scheme, host, explicit port, path, query, fragment, and raw escaping. The focused omnibox and navigation state still retain the exact original URL. Non-web or malformed values are not reinterpreted by this disclosure helper.

This reduces accidental export of explicit HTTP(S) userinfo, but it does not guarantee complete credential safety, clipboard privacy, Android share-target trust, phishing protection, credential-manager integration, or complete URL/userinfo safety.

Search Home is intentionally not duplicated in the menu because it already has a dedicated control in the bottom toolbar. The current menu is not yet the final Browser settings/menu system.

## Android Back behavior

When the omnibox is being edited, Android Back first leaves omnibox editing and dismisses the software keyboard. Otherwise, Browser Back navigates web history when history is available; if not, Android handles leaving the activity.

## GLAZE UI on Android

The Android beta maps Browser-owned chrome to **GLAZE UI V1.2 (`1.2.0`) Stable** using native Android controls.

The current source mapping includes Canvas, Surface, and Soft Glaze roles, the rule **Neutral glass is the material. Color is an accent.**, a 48dp minimum general interaction target, a recorded 56dp Touch Assistance floor where applicable, Calm expression, Balanced clarity, Light/Dark/Deep Dark structural targets, resource-backed semantic control labels, vector Browser chrome icons, visible pressed/focus treatment, an effects-free fallback that does not depend on blur, transparency, or animation, bounded RTL directionality for directional history vectors, debug pseudolocale test resources for later localization stress review, presentation-only stripping of Unicode bidi-formatting controls from the condensed unfocused address, and explicit-port preservation in that condensed presentation. The navigation resolver's local authority-shape, bare-host inference, external-handoff validation, and the independent outward-address disclosure policy are separate Browser boundaries and do not turn Glaze presentation into security, email, URL/userinfo, credential, clipboard, or complete scheme-safety truth.

The normal browsing surface removes the platform action bar and developer-status banner. At ordinary text scale the baseline expanded Browser chrome is 128dp before Android system bars; this is no longer a hard maximum because text-bearing top chrome can grow with native font metrics. The scroll-collapsed state retains only the fixed 56dp bottom toolbar. The Browser menu is Browser-owned rather than a platform `PopupMenu` surface.

This remains source/build-level V1.2 mapping and testability evidence. It is **not yet native-device GLAZE UI, accessibility, localization, pseudolocale, or RTL conformance acceptance**. Representative visual review, TalkBack, Switch Access, Voice Access, rendered 200% text, actual translated/localized copy and translation-quality review, pseudolocale stress review, locale fallback/grammar behavior, complete DNS/origin/Unicode/IDN/confusable handling, complete URL/userinfo/credential-disclosure safety, clipboard/share-target privacy, complete scheme/intent/deep-link and external-application handoff review, bidirectional URL/address editing, rendered RTL directionality, contrast/high-contrast, Reduced Motion, Reduced Transparency, input, performance, orientation, foldable/form-factor, Touch Assistance where supported, and physical-hardware validation remain required before production approval.

## Security behavior

The Android beta intentionally fails closed in several areas while the full GoreeCloud platform integrations are being completed:

- Malformed explicit HTTP(S) input fails to GoreeCloud Search Home instead of being forwarded to WebView or converted into a search query.
- Malformed HTTP(S) page-link attempts remain Browser-owned and are not handed to another Android application through `ACTION_VIEW` merely because a user gesture occurred.
- Unschemed input containing literal `@` is not inferred as a bare host and silently upgraded into an HTTPS URL containing userinfo.
- External handoff requires an explicit user gesture and a parseable eligible scheme.
- `about:`, `blob:`, `content:`, `data:`, `file:`, and `javascript:` remain Browser-owned and are not eligible for Android external-app handoff.
- Copy page address and Share page remove explicit HTTP(S) userinfo before outward disclosure while navigation/focused editing retain the exact authoritative URL.
- TLS/certificate errors are cancelled rather than bypassed.
- Android Safe Browsing is enabled; detected unsafe navigation returns to safety.
- Mixed-content loading is disabled.
- WebView file access and content access are disabled.
- Third-party cookies are disabled by default.
- Website permission requests are denied until Browser-owned permission and policy surfaces are integrated.
- Geolocation permission requests are denied.
- Downloads are blocked until the Android path can satisfy the authoritative Wardveil download verification and release contract.

Navigation-authority, bare-host inference, external-handoff validation, and outward-address disclosure are bounded local hardening only. They do not perform email validation, complete URL/userinfo or credential validation, phishing protection, clipboard privacy enforcement, Android share-target trust evaluation, DNS resolution, certificate verification, IDN normalization/confusable analysis, origin classification, complete scheme/intent/deep-link validation, external-application trust evaluation, or Wardveil security evaluation.

The condensed-address presentation hardening is presentation-only. Removing bidi controls and preserving explicitly parsed ports does not establish site trust, rewrite the loaded URL, replace Android/WebView security mechanisms, fully qualify origin safety, or manufacture Wardveil evidence.

These behaviors do not mean the beta has completed Wardveil Security production acceptance. Wardveil status must remain tied to actual authenticated runtime evidence.

## Privacy behavior

The beta uses privacy-protective defaults where a complete user-controlled Privacy Shield workflow does not yet exist. Third-party cookies are disabled and site permission grants fail closed.

The accessibility chrome policy consumes only Android's boolean accessibility-enabled state. The large-text layout change uses Android-native font/layout measurement. The RTL directionality slice uses Android application/vector/layout metadata. The localization-resource slice moves Browser-owned copy into Android resources. The pseudolocale slice generates debug test resources at build time. The bidi-presentation slice transforms only the local condensed display string. The explicit-port slice uses the already parsed local port value for that same display. The navigation-authority slice performs local parsing only; malformed explicit HTTP(S) input fails to Search Home specifically so the explicit navigation text is not silently disclosed to GoreeCloud Search as query text. The malformed-web external-handoff slice keeps rejected HTTP(S) link attempts inside Browser so they are not disclosed to another application through `ACTION_VIEW` merely because a user gesture occurred. The bare-userinfo routing slice performs local string classification only; unschemed `@` input remains on the existing GoreeCloud Search path rather than being transformed into an HTTPS userinfo navigation target. The local-scheme handoff slice performs local scheme classification only and prevents the bounded `about`/`blob`/`content`/`data`/`file`/`javascript` set from being disclosed to another application through `ACTION_VIEW`; eligible external schemes remain gesture-gated. The outward-address disclosure slice performs local URL parsing/text reconstruction only; Browser-owned Copy/Share actions remove explicit HTTP(S) userinfo before sending the page address to Android clipboard/share surfaces, while navigation and focused editing retain the exact URL. None of these changes adds accessibility-service enumeration, accessibility event capture, telemetry, remote calls, new network behavior beyond the already-selected Search path for ordinary non-URL input, persistent accessibility/locale state, or new user-content collection.

The Android beta does not yet provide complete production Privacy Shield filtering, consent, diagnostics, private-browsing isolation, clipboard/share-target privacy, or user-control surfaces required for Stable release.

## Website permissions

Camera, microphone, geolocation, and other website permission requests are currently denied. There is no beta override that silently grants them.

Browser-owned permission prompts and the required Privacy Shield/Wardveil policy integration remain future beta work. A site that depends on these permissions may therefore have reduced functionality in the current beta.

## Downloads

Downloads are currently blocked. This is intentional.

GoreeCloud Browser already has a Browser-to-Wardveil download release architecture in its native core. Android downloads remain unavailable until the Android transfer path can stage downloaded bytes, bind the exact digest to authoritative Wardveil evidence, and release or hold the file according to the accepted security decision without creating a bypass.

## External links

Structurally allowed ordinary HTTP and HTTPS links stay in GoreeCloud Browser. Malformed HTTP(S) page-link attempts also stay Browser-owned and are blocked by Browser routing rather than being handed to another Android application solely because a user gesture occurred.

A non-web URI triggered by an explicit user gesture may be handed to an installed Android application only when Browser can parse an eligible external scheme. `mailto:`, `tel:`, and the inherited `intent:` path retain this gesture-gated eligibility. If no application can handle an eligible external URI, Browser reports that the link cannot be opened. A request without an explicit user gesture is not eligible for this external handoff path.

`about:`, `blob:`, `content:`, `data:`, `file:`, and `javascript:` remain Browser-owned under the current bounded policy and are not eligible for Android external-app handoff. These schemes are also not accepted as ordinary Browser web navigation. This source-level allow/deny and handoff boundary does not itself establish complete scheme safety, intent/deep-link safety, trust in an external application, URL trust, or Wardveil acceptance.

## Current limitations

The Android beta does not yet claim:

- production signing or managed beta signing continuity;
- production or Stable readiness;
- complete GLAZE UI V1.2 native-device acceptance;
- representative rendered 200% text acceptance;
- actual translated/localized Browser resource acceptance or translation completeness/quality;
- rendered pseudolocale acceptance on representative devices;
- locale fallback/plural/grammar acceptance;
- complete DNS/origin/Unicode/IDN/confusable-spoofing acceptance;
- complete URL/userinfo/credential-disclosure or email-classification/phishing acceptance;
- complete clipboard privacy or Android share-target trust acceptance;
- complete scheme/intent/deep-link or external-application handoff acceptance;
- rendered RTL directionality or bidirectional URL/address-field editing acceptance;
- representative TalkBack, Switch Access, Voice Access, focus-order, or announcement-quality acceptance;
- complete Wardveil Security runtime acceptance;
- complete Privacy Shield runtime acceptance;
- Everkeep backup/recovery acceptance;
- private-browsing and Close & Forget acceptance;
- Android download/file-upload acceptance;
- Browser-owned website-permission prompts;
- complete Android multi-tab/session/settings surfaces;
- production GoreeCloud Identity, Vault, Sync, DNS, Network, Mesh, or Everkeep adapters;
- Play Store or other store publication;
- signed update, downgrade, rollback, or application-data migration acceptance;
- sustained real-device performance, battery, compatibility, localization, directionality, and accessibility acceptance.

## Reporting beta problems

When reporting an Android beta problem, include the Browser version, Android version, device model, Android System WebView version, font-size/display-size settings and language/layout direction when relevant, what you attempted, the expected result, and the observed result. Do not include passwords, authentication tokens, private browsing content, or other reusable secrets in bug reports.

## Acceptance language

A successful GoreeCloud Browser Android CI run proves only the checks performed by that workflow for the exact source revision: unit tests, Android lint, APK assembly, signature/package/version verification, debug pseudolocale resource presence, checksum/source-revision generation, and artifact creation. For `+android.19`, unit tests cover the inherited malformed explicit HTTP(S) authority fail-closed behavior including explicit web schemes without `//`, bounded authority-port validation, valid Unicode direct navigation, bare `localhost:port` and bracketed-IPv6-with-port HTTPS upgrade, invalid bare-port handling, malformed HTTP(S) external-handoff rejection, bare email-shaped input remaining GoreeCloud Search text, explicit HTTP(S) userinfo remaining valid under the explicit-URL navigation contract, parseable-scheme requirements for external handoff, blocked `about:`/`blob:`/`content:`/`data:`/`file:`/`javascript:` handoff, gesture-gated `mailto:`/`tel:`/`intent:` eligibility, outward HTTP(S) userinfo stripping with preservation of explicit port/path/query/fragment/raw escaping, ordinary HTTP(S) non-regression, unchanged non-web/malformed disclosure values, Activity handoff-policy wiring, and Copy/Share disclosure-policy source wiring. CI does not by itself establish production security, privacy, accessibility, rendered large-text behavior, localization, pseudolocale rendering, translation quality, email validation, complete URL/userinfo/credential or phishing safety, clipboard privacy, Android share-target trust, DNS validity, certificate/origin trust, complete Unicode/IDN/confusable handling, complete scheme/intent/deep-link or external-application trust, rendered RTL directionality, bidirectional editing, real-device compatibility, recovery, or Stable qualification.