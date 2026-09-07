# GoreeCloud Browser User Manual

## Current availability

GoreeCloud Browser is in active beta development. The Android target is a real installable test APK, but it is not production-approved or Stable.

Current Android beta identity for this source revision:

- Application: **GoreeCloud Browser Beta**
- Package: `io.goreecloud.browser.beta`
- Version: `0.1.0-beta.1+android.13`
- versionCode: `10013`
- Minimum Android: Android 8.0 / API 26
- Target API: 35
- Rendering dependency: Android System WebView/Chromium
- Current Browser-owned design-system target: GLAZE UI V1.2 / `1.2.0` Stable

GoreeCloud owns the Browser product layer, navigation/search behavior, mobile browser chrome, privacy defaults, security gates, and GoreeCloud integrations. Android System WebView/Chromium provides the web rendering/runtime foundation and is not the product identity.

The `+android.13` Development candidate is stacked on the V1.2 source migration, `+android.8` accessibility-chrome hardening, `+android.9` large-text reflow work, `+android.10` RTL directionality hardening, `+android.11` string-resource localization readiness, and `+android.12` debug pseudolocale testability. It hardens the condensed unfocused address presentation by removing Unicode bidi-formatting controls while preserving ordinary RTL letters and keeping the authoritative full URL unchanged for focused editing and navigation.

The parent debug beta still generates synthetic Android pseudolocales `en-XA` and `ar-XB` for localization stress testing. Pseudolocales are test resources, not translations. Their presence in the APK does not establish rendered pseudolocale behavior, translation quality, or localization acceptance. Likewise, the bidi presentation sanitizer is not complete URL-spoofing or bidirectional-editing acceptance.

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

Focus the field to expose and select the complete original current URL for editing. The unfocused presentation sanitizer does **not** rewrite the authoritative URL used for navigation, search resolution, sharing, clipboard actions, WebView requests, or persisted Browser state.

The leading `HTTPS`, `HTTP`, or `WEB` label reports the parsed address scheme. It is not a Wardveil verdict, certificate-verification badge, or claim that a page is trustworthy.

Use the arrow action beside the address field or the Android keyboard's Go action to navigate.

Enter a complete `https://` or `http://` URL to navigate directly. A host such as `example.com` is upgraded to HTTPS before navigation.

Text that is not interpreted as a URL is sent to **GoreeCloud Search**, which is the sole integrated search authority for this beta. Browser does not silently fall back to another search provider.

### Large text and font scaling

The normal omnibox baseline is 56dp inside Browser's top chrome, but text-bearing top chrome is no longer constrained to that exact height. Android-native text measurement can grow the scheme/address area when system font scaling requires additional vertical space.

The unified address field intentionally remains a single-line browser control. Its row can become taller as the Android font grows; long addresses continue to use normal single-line horizontal editing rather than becoming multi-line web addresses.

Browser-menu action rows also use content height with a 56dp minimum, so a scaled or wrapped action label can request more vertical room instead of being clipped into a fixed 56dp row.

The source contract records 2.0 font scale as the downstream large-text acceptance target. The current source/build state does **not** prove that 200% text has passed representative device, screen-size, orientation, or localization acceptance.

### RTL directionality

The Android application declares RTL support. The directional Back and Forward vector controls use Android auto-mirroring so their glyph direction follows the platform layout direction rather than remaining permanently left/right facing.

The current programmatic Browser chrome also uses logical start/end horizontal margins in the validated source path rather than physical left/right margin fields.

For the address field, the current `+android.13` source removes Unicode bidi-formatting controls only from the **unfocused condensed presentation**. Focusing the omnibox exposes the untouched original URL. This bounded behavior is intended to reduce presentation ambiguity without changing navigation authority.

This does **not** mean the beta is fully localized or that URL spoofing/bidirectional behavior is completely solved. IDN/confusable handling, bidirectional URL/address editing, rendered RTL layout, representative device review, and combined RTL + large-text + accessibility behavior remain open.

### Localization readiness and pseudolocales

Browser-owned natural-language Android chrome copy comes from `res/values/strings.xml` rather than being embedded directly in `BrowserActivity.kt`. This includes the omnibox hint and accessibility description, navigation-control descriptions, menu labels, safety/download/link feedback, clipboard text, chooser title, and beta information.

`HTTPS`, `HTTP`, and `WEB` remain explicitly non-translatable because they are protocol/status tokens rather than natural-language UI copy.

The debug beta generates Android pseudolocales:

- `en-XA` — synthetic accented/expanded text for layout and string-expansion testing; and
- `ar-XB` — synthetic bidirectional/RTL-oriented text for directionality stress testing.

CI verifies that both generated pseudolocale variants are present in the built debug APK. This is a testability feature only. The current default resource set is still English, and no actual translation pack is established by this work. Rendered pseudolocale review on supported test devices remains pending.

This source/build boundary does **not** establish translated resources, translation completeness or quality, locale fallback, plural/grammar behavior, complete Unicode/IDN/confusable handling, bidirectional address editing, rendered RTL locale acceptance, or combined locale + 200% text + assistive-technology/device acceptance.

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

Search Home is intentionally not duplicated in the menu because it already has a dedicated control in the bottom toolbar. The current menu is not yet the final Browser settings/menu system.

## Android Back behavior

When the omnibox is being edited, Android Back first leaves omnibox editing and dismisses the software keyboard. Otherwise, Browser Back navigates web history when history is available; if not, Android handles leaving the activity.

## GLAZE UI on Android

The Android beta maps Browser-owned chrome to **GLAZE UI V1.2 (`1.2.0`) Stable** using native Android controls.

The current source mapping includes Canvas, Surface, and Soft Glaze roles, the rule **Neutral glass is the material. Color is an accent.**, a 48dp minimum general interaction target, a recorded 56dp Touch Assistance floor where applicable, Calm expression, Balanced clarity, Light/Dark/Deep Dark structural targets, resource-backed semantic control labels, vector Browser chrome icons, visible pressed/focus treatment, an effects-free fallback that does not depend on blur, transparency, or animation, bounded RTL directionality for directional history vectors, debug pseudolocale test resources for later localization stress review, and presentation-only stripping of Unicode bidi-formatting controls from the condensed unfocused address.

The normal browsing surface removes the platform action bar and developer-status banner. At ordinary text scale the baseline expanded Browser chrome is 128dp before Android system bars; this is no longer a hard maximum because text-bearing top chrome can grow with native font metrics. The scroll-collapsed state retains only the fixed 56dp bottom toolbar. The Browser menu is Browser-owned rather than a platform `PopupMenu` surface.

This remains source/build-level V1.2 mapping and testability evidence. It is **not yet native-device GLAZE UI, accessibility, localization, pseudolocale, or RTL conformance acceptance**. Representative visual review, TalkBack, Switch Access, Voice Access, rendered 200% text, actual translated/localized copy and translation-quality review, pseudolocale stress review, locale fallback/grammar behavior, complete Unicode/IDN/confusable handling, bidirectional URL/address editing, rendered RTL directionality, contrast/high-contrast, Reduced Motion, Reduced Transparency, input, performance, orientation, foldable/form-factor, Touch Assistance where supported, and physical-hardware validation remain required before production approval.

## Security behavior

The Android beta intentionally fails closed in several areas while the full GoreeCloud platform integrations are being completed:

- TLS/certificate errors are cancelled rather than bypassed.
- Android Safe Browsing is enabled; detected unsafe navigation returns to safety.
- Mixed-content loading is disabled.
- WebView file access and content access are disabled.
- Third-party cookies are disabled by default.
- Website permission requests are denied until Browser-owned permission and policy surfaces are integrated.
- Geolocation permission requests are denied.
- Downloads are blocked until the Android path can satisfy the authoritative Wardveil download verification and release contract.

The condensed-address bidi sanitizer is a presentation hardening measure only. It does not establish site trust, rewrite the loaded URL, replace Android/WebView security mechanisms, or manufacture Wardveil evidence.

These behaviors do not mean the beta has completed Wardveil Security production acceptance. Wardveil status must remain tied to actual authenticated runtime evidence.

## Privacy behavior

The beta uses privacy-protective defaults where a complete user-controlled Privacy Shield workflow does not yet exist. Third-party cookies are disabled and site permission grants fail closed.

The accessibility chrome policy consumes only Android's boolean accessibility-enabled state. The large-text layout change uses Android-native font/layout measurement. The RTL directionality slice uses Android application/vector/layout metadata. The localization-resource slice moves Browser-owned copy into Android resources. The pseudolocale slice generates debug test resources at build time. The bidi-presentation slice transforms only the local condensed display string. None of these changes adds accessibility-service enumeration, accessibility event capture, telemetry, network behavior, persistent accessibility/locale state, or new user-content processing.

The Android beta does not yet provide the complete production Privacy Shield filtering, consent, diagnostics, private-browsing isolation, or user-control surface required for Stable release.

## Website permissions

Camera, microphone, geolocation, and other website permission requests are currently denied. There is no beta override that silently grants them.

Browser-owned permission prompts and the required Privacy Shield/Wardveil policy integration remain future beta work. A site that depends on these permissions may therefore have reduced functionality in the current beta.

## Downloads

Downloads are currently blocked. This is intentional.

GoreeCloud Browser already has a Browser-to-Wardveil download release architecture in its native core. Android downloads remain unavailable until the Android transfer path can stage downloaded bytes, bind the exact digest to authoritative Wardveil evidence, and release or hold the file according to the accepted security decision without creating a bypass.

## External links

Ordinary HTTP and HTTPS links stay in GoreeCloud Browser. A non-web URI triggered by an explicit user gesture may be handed to an installed Android application that can handle that URI. If no application can handle it, Browser reports that the link cannot be opened.

File, JavaScript, and other non-web schemes are not accepted as ordinary Browser web navigation.

## Current limitations

The Android beta does not yet claim:

- production signing or managed beta signing continuity;
- production or Stable readiness;
- complete GLAZE UI V1.2 native-device acceptance;
- representative rendered 200% text acceptance;
- actual translated/localized Browser resource acceptance or translation completeness/quality;
- rendered pseudolocale acceptance on representative devices;
- locale fallback/plural/grammar acceptance;
- complete Unicode/IDN/confusable-spoofing acceptance;
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

A successful GoreeCloud Browser Android CI run proves only the checks performed by that workflow for the exact source revision: unit tests, Android lint, APK assembly, signature/package/version verification, debug pseudolocale resource presence, checksum/source-revision generation, and artifact creation. For `+android.13`, unit tests also cover the bounded presentation-only bidi sanitizer and focused-vs-unfocused URL source separation. CI does not by itself establish production security, privacy, accessibility, rendered large-text behavior, localization, pseudolocale rendering, translation quality, complete Unicode/IDN/confusable handling, rendered RTL directionality, bidirectional editing, real-device compatibility, recovery, or Stable qualification.
