# Building GoreeCloud Browser

GoreeCloud Browser is in active development. The repository currently supports a buildable Browser-owned core and an optional compiled Chromium adapter scaffold. A successful build is not evidence of production browser readiness.

## Default development build

The default build uses `DevelopmentEngine`, which is intentionally non-rendering.

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

This mode is intended for validating Browser-owned application, tab, private-session, toolbar, internal-page, settings, and service-integration logic while Chromium integration is incomplete.

## Chromium integration build

Enable the compiled Chromium adapter boundary with:

```sh
cmake -S . -B build-chromium \
  -DGOREECLOUD_ENABLE_CHROMIUM=ON \
  -DBUILD_TESTING=ON
cmake --build build-chromium
```

At the current milestone, this compiles the GoreeCloud Chromium adapter scaffold and enforces sandbox/site-isolation requirements, but the concrete Chromium `EngineContext` and `EngineView` are not yet implemented. Therefore this build is not yet render-capable.

The next Chromium milestone must implement:

- Chromium runtime and subprocess initialization;
- native Browser view creation;
- navigation callbacks;
- renderer lifecycle and crash reporting;
- private/in-memory Browser contexts;
- origin-scoped and full-context data cleanup;
- permission interception;
- certificate/security inspection;
- download interception into GoreeCloud Download Manager;
- GPU and accessibility plumbing;
- Linux native-window attachment first, followed by Windows and Android platform hosts.

## Security requirements

GoreeCloud Browser must not initialize the production Chromium adapter with sandboxing or site isolation disabled. Private contexts must not use persistent profile storage.

## Distribution direction

The same accepted Browser source is intended to feed:

- Linux Flatpak;
- Windows signed `.exe` installer;
- Android signed APK;
- other approved GoreeCloud Browser artifacts later.

Packaging success does not override runtime, security, accessibility, recovery, or release-integrity acceptance requirements.
