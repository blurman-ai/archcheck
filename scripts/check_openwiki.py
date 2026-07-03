#!/usr/bin/env python3
"""Structural + drift check for the docs/openwiki/ agent wiki.

Run manually (e.g. each evening) from anywhere in the repo:

    python3 scripts/check_openwiki.py

Deterministic, no LLM, no network. Checks:
  - front matter present with required keys (log.md is exempt, append-only);
  - status in {verified, partial, unverified}; verified => source_paths non-empty;
  - every source_paths entry exists on disk;
  - every inline markdown link ](x.md) resolves;
  - orphans: every page reachable from index.md (backlog/log/sources exempt);
  - STALE: every page whose cited source_paths changed since its
    last_checked_commit (this is the doc<->code drift signal).

Exit 0 = clean (stale/orphan are warnings by default); 1 = structural error.
Pass --strict to also fail on warnings.
"""
from __future__ import annotations

import re
import subprocess
import sys
from pathlib import Path

REQUIRED_KEYS = ("title", "type", "status", "last_checked_commit", "source_paths")
VALID_STATUS = {"verified", "partial", "unverified"}
# Pages exempt from the front-matter / orphan rules.
NO_FRONTMATTER = {"log.md"}
ORPHAN_EXEMPT_DIRS = ("backlog/", "sources/")
ORPHAN_EXEMPT_FILES = {"index.md", "log.md"}

errors: list[str] = []
warnings: list[str] = []


def repo_root() -> Path:
    out = subprocess.run(
        ["git", "rev-parse", "--show-toplevel"], capture_output=True, text=True, check=True
    )
    return Path(out.stdout.strip())


def parse_front_matter(text: str) -> dict | None:
    """Minimal YAML front-matter parser for our controlled page format."""
    if not text.startswith("---"):
        return None
    end = text.find("\n---", 3)
    if end == -1:
        return None
    body = text[3:end].strip("\n")
    data: dict = {}
    key = None
    for raw in body.splitlines():
        if not raw.strip():
            continue
        item = re.match(r"\s+-\s+(.*)", raw)
        if item and key and isinstance(data.get(key), list):
            data[key].append(item.group(1).strip().strip('"'))
            continue
        m = re.match(r"([A-Za-z_]+):\s*(.*)", raw)
        if m:
            key, val = m.group(1), m.group(2).strip()
            if val in ("", "[]"):
                data[key] = []  # a bare "key:" heads a list; "[]" is an empty list
            else:
                data[key] = val.strip('"')
    return data


def git_changed(root: Path, sha: str, path: str) -> list[str]:
    """Files under `path` changed between sha..HEAD. Empty if none/invalid sha."""
    r = subprocess.run(
        ["git", "-C", str(root), "diff", "--name-only", f"{sha}..HEAD", "--", path],
        capture_output=True, text=True,
    )
    if r.returncode != 0:
        warnings.append(f"  git could not diff {sha}..HEAD for {path} (bad commit?)")
        return []
    return [ln for ln in r.stdout.splitlines() if ln.strip()]


def check() -> None:
    root = repo_root()
    wiki = root / "docs" / "openwiki"
    if not wiki.is_dir():
        errors.append("docs/openwiki/ not found")
        return

    pages = sorted(wiki.rglob("*.md"))
    link_graph: dict[Path, set[Path]] = {}

    for page in pages:
        rel = page.relative_to(wiki).as_posix()
        text = page.read_text(encoding="utf-8")

        # --- inline links (all pages, incl. log.md) ---
        targets: set[Path] = set()
        for link in re.findall(r"\]\(([^)]+\.md)\)", text):
            if link.startswith("http"):
                continue
            tgt = (page.parent / link).resolve()
            if not tgt.exists():
                errors.append(f"[link] {rel} -> {link} (broken)")
            elif wiki.resolve() in tgt.parents:
                targets.add(tgt)
        link_graph[page.resolve()] = targets

        if page.name in NO_FRONTMATTER:
            continue

        # --- front matter ---
        fm = parse_front_matter(text)
        if fm is None:
            errors.append(f"[frontmatter] {rel} has no YAML front matter")
            continue
        for k in REQUIRED_KEYS:
            if k not in fm or fm[k] in (None, ""):
                errors.append(f"[frontmatter] {rel} missing '{k}'")
        status = fm.get("status")
        if status not in VALID_STATUS:
            errors.append(f"[status] {rel} has status={status!r} (want {VALID_STATUS})")
        srcs = fm.get("source_paths") or []
        if not isinstance(srcs, list):
            srcs = []
        if status == "verified" and not srcs:
            errors.append(f"[status] {rel} is verified but source_paths is empty")

        # --- source_paths exist + staleness ---
        sha = fm.get("last_checked_commit")
        for sp in srcs:
            if sp.startswith("http"):
                continue
            if not (root / sp).exists():
                errors.append(f"[source] {rel} cites missing path: {sp}")
            elif sha:
                changed = git_changed(root, sha, sp)
                if changed:
                    warnings.append(
                        f"[stale] {rel} (checked @{sha}) cites {sp} — changed since: "
                        + ", ".join(changed[:4]) + (" …" if len(changed) > 4 else "")
                    )

    # --- orphan check: reachable from index.md ---
    index = (wiki / "index.md").resolve()
    if index.exists():
        seen: set[Path] = set()
        stack = [index]
        while stack:
            cur = stack.pop()
            if cur in seen:
                continue
            seen.add(cur)
            stack.extend(link_graph.get(cur, ()))
        for page in pages:
            r = page.relative_to(wiki).as_posix()
            if page.name in ORPHAN_EXEMPT_FILES or r.startswith(ORPHAN_EXEMPT_DIRS):
                continue
            if page.resolve() not in seen:
                warnings.append(f"[orphan] {r} is not reachable from index.md")

    print(f"checked {len(pages)} pages under docs/openwiki/")


def main() -> int:
    strict = "--strict" in sys.argv
    check()
    for w in warnings:
        print("WARN " + w)
    for e in errors:
        print("FAIL " + e)
    print(f"\n{len(errors)} error(s), {len(warnings)} warning(s)")
    if errors or (strict and warnings):
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
