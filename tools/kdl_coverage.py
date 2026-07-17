#!/usr/bin/env python3
"""KDL coverage audit: classify datamined MST JSONs by how many of their
field keys are already named in packet-generator KDL schemas.

Sources:
  - KDL key inventory: packet-generator/assets/{mst,net}/*.kdl  (`key "XXXXXXXX"`)
  - Reference MSTs:    C:/Users/Evan/BF/21900/mst-decoded/2-mst-decoded/*.json
  - Server runtime:    deploy/system/*.json

Output: tools/kdl_coverage_report.md (and a machine-readable .json next to it).
"""

import json
import re
import sys
from collections import defaultdict
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent
KDL_DIRS = [REPO / "packet-generator/assets/mst", REPO / "packet-generator/assets/net"]
REF_DIR = Path(r"C:\Users\Evan\BF\21900\mst-decoded\2-mst-decoded")
DEPLOY_DIR = REPO / "deploy/system"
OUT_MD = REPO / "tools/kdl_coverage_report.md"
OUT_JSON = REPO / "tools/kdl_coverage_report.json"

KEY_RE = re.compile(r'key\s+"([^"]{4,12})"')
JSON_KEY_RE = re.compile(r'"([A-Za-z0-9]{8})"\s*:')
COMMENT_RE = re.compile(r'//[^\n]*')


def collect_kdl_keys():
    """hash key -> list of 'file:struct' locations."""
    keys = defaultdict(list)
    struct_re = re.compile(r'^\s*json\s+(\S+)')
    for d in KDL_DIRS:
        for kdl in sorted(d.glob("*.kdl")):
            current = "?"
            for line in kdl.read_text(encoding="utf-8", errors="replace").splitlines():
                m = struct_re.match(line)
                if m:
                    current = m.group(1)
                for k in KEY_RE.findall(line):
                    loc = f"{kdl.parent.name}/{kdl.name}:{current}"
                    if loc not in keys[k]:
                        keys[k].append(loc)
    return keys


def json_field_keys(path):
    """All 8-char field keys used in a (possibly comment-annotated) JSON file."""
    text = path.read_text(encoding="utf-8", errors="replace")
    text = COMMENT_RE.sub("", text)  # some reference files carry // annotations
    return set(JSON_KEY_RE.findall(text))


def classify(files, kdl_keys):
    rows = []
    for path in files:
        fields = json_field_keys(path)
        if not fields:
            continue
        covered = {k for k in fields if k in kdl_keys}
        missing = sorted(fields - covered)
        pct = 100.0 * len(covered) / len(fields)
        rows.append({
            "file": path.name,
            "fields": len(fields),
            "covered": len(covered),
            "pct": round(pct, 1),
            "missing_keys": missing,
        })
    rows.sort(key=lambda r: (r["pct"], r["file"]))
    return rows


def bucket(rows):
    full, partial, none = [], [], []
    for r in rows:
        (full if r["pct"] == 100.0 else none if r["covered"] == 0 else partial).append(r)
    return full, partial, none


def render(title, rows, out, show_missing=True):
    out.append(f"\n### {title} ({len(rows)} files)\n")
    if not rows:
        out.append("_none_\n")
        return
    out.append("| File | Keys | In KDL | % | Missing keys |")
    out.append("|---|---:|---:|---:|---|")
    for r in rows:
        miss = ""
        if show_missing and r["missing_keys"]:
            shown = r["missing_keys"][:8]
            miss = "`" + "` `".join(shown) + "`"
            if len(r["missing_keys"]) > 8:
                miss += f" +{len(r['missing_keys']) - 8} more"
        out.append(f"| {r['file']} | {r['fields']} | {r['covered']} | {r['pct']} | {miss} |")


def main():
    kdl_keys = collect_kdl_keys()
    ref_rows = classify(sorted(REF_DIR.glob("*.json")), kdl_keys)
    dep_rows = classify(sorted(DEPLOY_DIR.glob("*.json")), kdl_keys)

    out = ["# KDL coverage report", "",
           f"KDL key inventory: **{len(kdl_keys)}** distinct hash keys across "
           f"{sum(1 for d in KDL_DIRS for _ in d.glob('*.kdl'))} schema files.", ""]

    out.append("\n## Reference MSTs (2-mst-decoded — the full datamined set)")
    full, partial, none = bucket(ref_rows)
    render("Uncovered — no keys in any KDL", none, out, show_missing=False)
    render("Partially covered", partial, out)
    render("Fully covered", full, out, show_missing=False)

    out.append("\n## deploy/system (what the server loads at boot)")
    full, partial, none = bucket(dep_rows)
    render("Uncovered", none, out, show_missing=False)
    render("Partially covered", partial, out)
    render("Fully covered", full, out, show_missing=False)

    OUT_MD.write_text("\n".join(out), encoding="utf-8")
    OUT_JSON.write_text(json.dumps({
        "kdl_key_count": len(kdl_keys),
        "reference": ref_rows,
        "deploy": dep_rows,
    }, indent=1), encoding="utf-8")
    print(f"wrote {OUT_MD}")
    print(f"wrote {OUT_JSON}")
    print(f"reference: {len(ref_rows)} files | deploy: {len(dep_rows)} files")


if __name__ == "__main__":
    sys.exit(main())
