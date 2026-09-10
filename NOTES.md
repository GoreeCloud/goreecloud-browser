# GoreeCloud Browser — Notes

**Lifecycle:** Development  
**Last reconciled:** September 10, 2026

This file records implementation notes subordinate to the repository specifications, feature roadmap, governing GoreeCloud instructions, Tasks Management, and verified runtime evidence.

## Current verified development state

- The Android unified navigation resolver keeps direct web navigation independent from GoreeCloud Search.
- Explicit `http://` and `https://` inputs are accepted only when the URI has an allowed web scheme, no URI user-info, and a valid host-like authority.
- Scheme-less credential-shaped authority input containing user-info is rejected before Browser can prepend HTTPS or forward the value to Search.
- Ordinary non-URL input is routed only to GoreeCloud Search; no alternate search provider or silent fallback is authorized by the current resolver.
- Current URL hardening is Development evidence only and is not a claim of complete IDN/confusable protection, OEM/process restoration acceptance, physical-device navigation acceptance, or production security review.

## Open acceptance work

Physical-device navigation, IDN/confusable policy, restoration behavior across process/OEM variations, accessibility/localization/RTL acceptance, current Stable Glaze UI acceptance, and the applicable Wardveil, Privacy Shield, Everkeep, Identity, Mesh, Manager, and release gates remain open.

## Documentation rule

Do not use this notes file to promote Browser lifecycle or protection state. Material feature and acceptance changes must be reconciled through the applicable roadmap, conformance evidence, Drive records, and Tasks Management.
