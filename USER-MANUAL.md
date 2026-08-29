# GoreeCloud Browser User Manual

## Current availability

GoreeCloud Browser is in active beta development. The Android target is a real installable test APK, but it is not production-approved or Stable.

Current Android beta identity for this source revision:

- Application: **GoreeCloud Browser Beta**
- Package: `io.goreecloud.browser.beta`
- Version: `0.1.0-beta.1+android.3`
- versionCode: `10003`
- Minimum Android: Android 8.0 / API 26
- Target API: 35
- Rendering dependency: Android System WebView/Chromium

GoreeCloud owns the Browser product layer, navigation/search behavior, mobile browser chrome, privacy defaults, security gates, and GoreeCloud integrations. Android System WebView/Chromium provides the web rendering/runtime foundation and is not the product identity.

## Installing the Android beta

The CI-generated APK is debug-signed for testing. Use an APK supplied from the GoreeCloud Browser Android Beta workflow or another explicitly supplied GoreeCloud beta artifact tied to an exact source revision.

Android may require you to allow installation from the app used to open the APK. Enable that Android setting only for the trusted installation source you intend to use.

The current CI beta signing key is not the production signing authority. Fresh CI environments can produce different debug certificates. If Android reports that an update cannot be installed because signatures differ, remove the older beta package and perform a fresh install. Removing the beta can remove its local application data.

Do not treat the beta APK as a production release, managed update channel, or long-term data-preservation target.

## Mobile browser chrome

The `+android.3` Android shell is designed to use substantially less of the phone viewport than the first installable beta shell.

Normal browsing no longer places an Android Activity title bar or engineering-status banner above the page. The main Browser-owned chrome is split into two compact areas:

- a top omnibox; and
- a bottom navigation toolbar.

The web page occupies the full region between them.

### Omnibox

The top omnibox reserves most of its width for the unified address/search field. When the field is not being edited, Browser presents a condensed address. Focus the field to expose and select the complete current URL for editing.

The leading `HTTPS`, `HTTP`, or `WEB` label reports the parsed address scheme. It is not a Wardveil verdict, certificate-verification badge, or claim that a page is trustworthy.

Use the arrow action at the right edge of the omnibox or the Android keyboard's Go action to navigate.

Enter a complete `https://` or `http://` URL to navigate directly. A host such as `example.com` is upgraded to HTTPS before navigation.

Text that is not interpreted as a URL is sent to **GoreeCloud Search**, which is the sole integrated search authority for this beta. Browser does not silently fall back to another search provider.

### Bottom navigation

The bottom toolbar contains:

- Back;
- Forward;
- GoreeCloud Search Home;
- Reload, which becomes Stop while a page is loading; and
- Browser menu.

Back and Forward visibly disable when no matching history action is available.

The current Browser menu includes bounded beta actions for returning Home, copying the current page address, sharing the current page address through Android, and viewing beta information. It is not yet the final Browser settings/menu system.

Page-loading progress is drawn at the top of the web-content region rather than using a separate toolbar row.

## Android Back behavior

When the omnibox is being edited, Android Back first leaves omnibox editing and dismisses the software keyboard. Otherwise, Browser Back navigates web history when history is available; if not, Android handles leaving the activity.

## Glaze UI on Android

The Android beta maps Browser-owned chrome to the current Glaze UI 2.0.0 Stable semantics using native Android controls.

The current source mapping includes Canvas, Surface, and Soft Glaze roles, a 48dp minimum general interaction target, Calm expression, Balanced clarity, native light/dark adaptation, semantic control labels, vector Browser chrome icons, visible pressed/focus treatment, and an effects-free fallback that does not depend on blur, transparency, or animation.

The normal browsing surface explicitly removes the platform action bar and developer-status banner from the chrome. The fixed Browser-owned top/bottom chrome budget is 128dp before Android system bars.

This remains source-level Glaze 2.0 mapping evidence. It is **not yet native-device Glaze conformance acceptance**. Real-device visual, TalkBack, text-scaling, contrast, input, performance, orientation, foldable/form-factor, and representative hardware validation remain required before production approval.

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

These behaviors do not mean the beta has completed Wardveil Security production acceptance. Wardveil status must remain tied to actual authenticated runtime evidence.

## Privacy behavior

The beta uses privacy-protective defaults where a complete user-controlled Privacy Shield workflow does not yet exist. Third-party cookies are disabled and site permission grants fail closed.

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
- complete Glaze UI 2.0 native-device acceptance;
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
- sustained real-device performance, battery, compatibility, and accessibility acceptance.

## Reporting beta problems

When reporting an Android beta problem, include the Browser version, Android version, device model, Android System WebView version, what you attempted, the expected result, and the observed result. Do not include passwords, authentication tokens, private browsing content, or other reusable secrets in bug reports.

## Acceptance language

A successful GoreeCloud Browser Android CI run proves only the checks performed by that workflow for the exact source revision: unit tests, Android lint, APK assembly, signature/package verification, checksum generation, and artifact creation. It does not by itself establish production security, privacy, accessibility, real-device compatibility, recovery, or Stable qualification.
