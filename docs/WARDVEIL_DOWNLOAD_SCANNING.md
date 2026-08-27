# Wardveil Security download scanning

GoreeCloud Browser owns the download experience and enforcement boundary. Wardveil Security owns security evidence and malware-protection policy. ClamAV remains a replaceable scanner behind Wardveil Scan and is never a Browser-facing dependency.

## Release flow

The Advanced Download Manager already writes transfer bytes to a private `.goreecloud-part-<download_id>` staging file. The security release path is:

`network transfer -> Browser staging file -> SHA-256 binding -> Wardveil Scan -> validated scan_finding -> staging re-hash -> atomic Browser commit`

A normal download filename is created only after Wardveil returns a decision that the Browser can validate as current, authoritative, clean, correctly scoped, and bound to the exact staged digest.

## Resource identity and evidence

Browser downloads use the internal resource identity `browser:download:<download_id>` and Wardveil resource type `browser_download`. Shared security evidence does not require the source URL, referrer URL, user-facing filename, credentials, cookies, or session tokens.

Raw file content may be supplied to the authorized Wardveil Scan operation when necessary for malware inspection, but raw content is not part of the shared security record. Browser never submits the download directly to `clamd`.

## Result behavior

- `clean` — release is allowed only when the scan record is authoritative, correctly scoped, bound to the exact SHA-256 digest, current and unexpired, and carries evidence references.
- `suspicious` — the file remains in staging in the `held` state for review; it is not opened or committed to the normal destination.
- `malicious` — the file remains blocked in staging and Browser may emit a non-destructive Wardveil Quarantine handoff. The handoff still requires explicit executor authority.
- `unknown` or `unsupported` — release fails closed.
- unavailable transport, malformed evidence, non-authoritative evidence, digest mismatch, scope mismatch, future-dated evidence, expired clean evidence, or missing clean evidence references — release fails closed.

A positive malicious finding bound to the exact immutable digest remains blocking after the finding's normal validity window expires. Expiry never converts known malicious content into a clean result.

## Staging integrity

Browser computes the staged SHA-256 digest before Wardveil Scan and computes it again after a clean decision. If the bytes changed during verification, Browser blocks release rather than trusting the earlier finding.

The current source foundation does not yet prove that an external process cannot modify the staging file in the very small interval between the final re-hash and atomic rename. Production acceptance therefore still requires a single-writer, file-locking, descriptor-based commit, or equivalent target-platform synchronization guarantee.

## User-visible states

The Advanced Download Manager distinguishes:

- `Verifying` — transfer finished; Wardveil verification is in progress.
- `Held for review` — suspicious security evidence prevents release.
- `Blocked` — malicious or unverifiable security state prevents release.
- `Completed` — Wardveil verification allowed release and the staged bytes were committed.

Only `Completed` downloads can be opened through the Browser download controller.

## Private Browsing

Private Browsing does not bypass Wardveil verification. Private download queue state remains non-persistent, matching the existing Browser privacy boundary. Security records should still use minimized internal resource identity and digest evidence rather than unrelated browsing data.

## Quarantine authority

A blocked Browser staging file is not automatically canonical Wardveil Quarantine. Browser owns its temporary staging area; Wardveil Quarantine owns canonical containment only after an explicitly authorized executor performs that action. Quarantine is not deletion.

## Production acceptance

This integration is source-level evidence only. It does not establish deployed malware protection or authorize a broad `Protected by Wardveil` claim.

Production acceptance still requires deployed authenticated Browser-to-Wardveil transport, current deployed scanner/signature health, controlled clean and EICAR/malicious runtime tests, unavailable/timeout tests, concurrency-safe staging release, authorized quarantine execution and recovery evidence, Glaze UI acceptance for security states, and Privacy Shield data-minimization acceptance.
