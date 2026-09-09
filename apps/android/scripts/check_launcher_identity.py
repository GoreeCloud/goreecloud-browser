#!/usr/bin/env python3
"""Fail-closed source validation for the GoreeCloud Browser Android launcher identity."""

from __future__ import annotations

import json
import re
from pathlib import Path
import xml.etree.ElementTree as ET

ANDROID = "{http://schemas.android.com/apk/res/android}"
ROOT = Path(__file__).resolve().parents[1]
PROVENANCE = ROOT / "branding" / "browser-launcher-icon-provenance.json"
MANIFEST = ROOT / "app" / "src" / "main" / "AndroidManifest.xml"
RES = ROOT / "app" / "src" / "main" / "res"


def fail(message: str) -> None:
    raise SystemExit(f"Browser launcher identity guard failed: {message}")


def require(condition: bool, message: str) -> None:
    if not condition:
        fail(message)


def parse_xml(path: Path) -> ET.Element:
    require(path.is_file(), f"missing {path.relative_to(ROOT)}")
    try:
        return ET.parse(path).getroot()
    except ET.ParseError as exc:
        fail(f"invalid XML in {path.relative_to(ROOT)}: {exc}")


def adaptive_layers(path: Path) -> dict[str, str]:
    root = parse_xml(path)
    require(root.tag == "adaptive-icon", f"{path.name} must be an adaptive-icon")
    layers: dict[str, str] = {}
    for child in root:
        key = child.tag
        if key in {"background", "foreground", "monochrome"}:
            layers[key] = child.attrib.get(f"{ANDROID}drawable", "")
    return layers


def main() -> None:
    require(PROVENANCE.is_file(), "missing branding provenance record")
    data = json.loads(PROVENANCE.read_text(encoding="utf-8"))

    require(data.get("schema") == "goreecloud.android.launcher-icon-provenance/1", "unexpected provenance schema")
    require(data.get("product") == "GoreeCloud Browser", "provenance product mismatch")
    require(data.get("consumer_repository") == "GoreeCloud/goreecloud-browser", "consumer repository mismatch")
    require(data.get("canonical_repository") == "GoreeCloud/goreecloud-branding-assets", "canonical repository must remain GoreeCloud Branding Assets")
    require(data.get("canonical_path") == "products/browser/app-icon.svg", "canonical Browser icon path changed without review")
    require(re.fullmatch(r"[0-9a-f]{40}", str(data.get("canonical_revision_observed", ""))) is not None, "canonical revision must be a full Git SHA")
    require(data.get("canonical_blob_sha") == "3f502e5a6dae8ff3cb86dfdc3ae5d2eb2e8b54e1", "canonical Browser icon blob changed; refresh derivatives and review provenance")

    derivatives = data.get("derivatives")
    require(isinstance(derivatives, dict), "derivatives map is missing")
    for name, relative in derivatives.items():
        require(isinstance(relative, str) and relative, f"invalid derivative path for {name}")
        candidate = (ROOT / relative).resolve()
        require(ROOT in candidate.parents, f"derivative escapes Android root: {name}")
        require(candidate.is_file(), f"missing declared derivative: {relative}")

    manifest = parse_xml(MANIFEST)
    application = manifest.find("application")
    require(application is not None, "manifest application element is missing")
    require(application.attrib.get(f"{ANDROID}icon") == "@mipmap/ic_launcher", "manifest must use @mipmap/ic_launcher")
    require(application.attrib.get(f"{ANDROID}roundIcon") == "@mipmap/ic_launcher_round", "manifest must use @mipmap/ic_launcher_round")
    require(application.attrib.get(f"{ANDROID}label") == "@string/app_name", "manifest label must remain resource-backed")

    v26_expected = {
        "background": "@drawable/ic_launcher_background",
        "foreground": "@drawable/ic_launcher_foreground",
    }
    v33_expected = {
        **v26_expected,
        "monochrome": "@drawable/ic_launcher_monochrome",
    }
    for name in ("ic_launcher.xml", "ic_launcher_round.xml"):
        v26 = adaptive_layers(RES / "mipmap-anydpi-v26" / name)
        require(v26 == v26_expected, f"API 26 adaptive layers drifted for {name}: {v26}")
        v33 = adaptive_layers(RES / "mipmap-anydpi-v33" / name)
        require(v33 == v33_expected, f"API 33 adaptive/monochrome layers drifted for {name}: {v33}")

    background = (RES / "drawable" / "ic_launcher_background.xml").read_text(encoding="utf-8")
    foreground = (RES / "drawable" / "ic_launcher_foreground.xml").read_text(encoding="utf-8")
    monochrome = (RES / "drawable" / "ic_launcher_monochrome.xml").read_text(encoding="utf-8")

    require('#3B82F6' in background and '#174EA6' in background, "canonical Browser blue gradient is missing from adaptive background")
    require('#174EA6' in foreground, "canonical Browser dark-blue compass center is missing")
    require('M68,39 L60,60 L39,70 L47,48 Z' in foreground, "Browser compass needle derivative geometry drifted")
    require('M54,22 A32,32' in monochrome and 'M67.5,34.5 L59.8,58.6 L35.5,72.5 L44.2,48.2 Z' in monochrome, "Browser themed-icon silhouette drifted")

    print(
        "Browser launcher identity guard passed: canonical branding provenance, manifest refs, "
        "adaptive/round layers, monochrome layer, and reviewed compass derivative are intact."
    )


if __name__ == "__main__":
    main()
