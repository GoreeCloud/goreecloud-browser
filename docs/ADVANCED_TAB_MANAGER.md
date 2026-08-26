# GoreeCloud Browser — Advanced Tab Manager

**Status:** active development. This document defines product and implementation requirements; it does not claim production readiness.

Advanced Tab Manager is GoreeCloud Browser's first-party authority for tab creation, organization, selection, grouping, workspaces, Split View, protection, resource management, search, bulk operations, media controls, and session recovery. It is rendered with the latest approved Glaze UI Design Language and does not depend on an extension.

## Core model

The Browser maintains Browser-owned tab identity independently of the rendering engine. Each managed tab may carry window, Workspace, Tab Group, Split View, pin, lock, protection, sleeping, media, and resource state.

Locked tabs prevent ordinary navigation away from the current page until unlocked. Protected tabs resist accidental closure and are excluded from ordinary bulk-close operations; explicit protected-tab closure remains available.

## Split View

Supported layouts include two-pane vertical and horizontal layouts, three-pane primary/secondary layouts, and four-quadrant layouts. Panes have adjustable proportions. Tabs may be inserted, removed, replaced, swapped, temporarily expanded, or dragged into/out of a split without destroying the underlying tabs. Split layouts are part of recoverable session state.

## Groups and Workspaces

Tab Groups provide named, colored/icon-bearing, collapsible collections. Workspaces represent complete Browser working environments and preserve tab ordering, groups, pinned/protected state, and Split View configuration. Groups and Workspaces can be closed and recovered through session history.

## Multi-selection and bulk actions

Desktop selection supports individual and range selection plus programmatic selection by position, group, or site. Bulk actions include move, duplicate, reload, close, sleep/wake, pin, protect, mute, grouping, Workspace movement, window movement, Split View arrangement, bookmarking, and URL copying.

Protected-tab semantics always override an ordinary bulk-close request.

## Search and duplicate detection

Tab Search spans windows and Workspaces and can match title, URL, group, Workspace, sleeping state, pin/protection state, audio state, and Split View participation. Search results support direct tab-management actions.

Duplicate detection uses normalized page identity and can report duplicate URL sets across windows and Workspaces. Optional duplicate prevention may offer switching to an existing tab rather than opening another copy. It must never silently destroy user tabs.

## Resource management

Advanced Tab Manager integrates with Browser memory management to expose sleeping state and approximate per-tab memory, CPU, network, audio/media, download, and process activity where technically available. Automatic sleeping excludes workloads that the Browser determines should remain active, including active media/download activity and user-defined never-sleep rules. Pinned/protected exclusion remains configurable.

Resource figures are diagnostic approximations and must not be presented as exact measurements when the underlying engine cannot provide exact per-tab attribution.

## Recovery

Recoverable state includes recently closed tabs, windows, Tab Groups, Workspaces, Split View layouts, tab ordering, pinned state, protected state, and the previous browsing session. Crash recovery is coordinated with Everkeep resilience requirements. Recovery must preserve privacy boundaries; Private Browsing state must not be persisted as ordinary recoverable session data.

## Tab context menu

The Glaze UI tab context menu adapts to one/multiple tabs and pinned, locked, protected, sleeping, grouped, audio, and Split View states. It exposes applicable actions for duplication, reload, pin/lock/protect, sleeping, media, groups, Workspaces, Split View, movement, selection, copy/bookmark, Tab Information, and close operations.

The Split View submenu exposes placement, layout creation, swapping, pane movement/replacement, temporary expansion, restoration, removal, and exit operations.

## Tab Information

The optional Tab Information panel may show page title, URL/domain, Group, Workspace, pin/protection/lock/sleep state, media state, Split View participation, approximate memory, CPU/network activity, and Browser process status.

## Glaze UI and accessibility

Advanced Tab Manager uses Glaze UI for the tab strip, context menus, Split View affordances, group surfaces, Workspace switcher, Tab Search, resource indicators, and information panels. Desktop interactions support keyboard/mouse power workflows; touch targets and adaptive layouts support tablets, foldables, and other touch surfaces. All state communicated visually must have an accessible semantic equivalent.

## Engine boundary

Chromium or another rendering engine supplies web-content/process primitives only. GoreeCloud owns tab identity, organization, groups, Workspaces, Split View, protection semantics, selection, recovery policy, search, and Glaze presentation.
