# Android Browser Chrome Redesign

Status: Implementation target

This document defines the next Android browser-shell milestone following the failed 2026-09-07 physical-device visual acceptance pass.

## Primary layout

1. Compact top omnibox surface with site identity/security indicator, editable address/search text, reload/stop, and tab count.
2. Web content receives the majority of the viewport.
3. Bottom navigation remains one-handed but is reduced to the highest-frequency actions: Back, Forward, Home/New Tab, Tabs, Menu. Reload/Stop belongs with page state in the top chrome.
4. New-tab/home opens a GoreeCloud-native start surface rather than depending on a remote HTML page being available.

## Tab model

The Android app must introduce a first-party tab model rather than simulating tabs through WebView history. Minimum milestone requirements:

- one active tab;
- visible tab count;
- create new tab;
- switch between tabs;
- close tab;
- retain title + URL per tab;
- keep private/in-memory state separate from normal browsing once private mode is implemented.

A single WebView may initially be rebound/recreated per selected tab if necessary, but the data model must be explicit and testable.

## Start surface

The start surface must be rendered locally by the application and remain usable without network access. It should include:

- GoreeCloud Browser identity;
- large search/address entry affordance;
- status text when GoreeCloud Search is unavailable;
- optional favorites/recent sites only when those models are actually implemented;
- privacy-first messaging limited to implemented controls, not marketing claims.

The start surface must not silently fall back to a third-party search engine.

## Search-provider failure

If a search request returns a known provider/configuration failure, keep the browser chrome active and present a GoreeCloud-owned error surface explaining that Search is unavailable while direct URL navigation still works.

## Glaze UI

- reduce the amount of opaque white surface;
- use neutral translucent glass with strong text contrast;
- use compact target sizes while retaining accessibility;
- use accent color only for active/emphasis state;
- make the browser feel native to the same platform as Launcher and Gallery.
