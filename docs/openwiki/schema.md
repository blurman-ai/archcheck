---
title: "openwiki page schema"
type: schema
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "docs/openwiki/index.md"
related_pages:
  - "index.md"
  - "source-map.md"
  - "cli-contract.md"
  - "log.md"
external_sources: []
---

# openwiki page schema

## Purpose

This page promotes `index.md`'s "Page conventions (mini-schema)" section
(`index.md:69-78`) into a full reference. Every page under `docs/openwiki/`
(except the append-only `log.md`) must satisfy this contract. `index.md` keeps
only a short pointer to this page — the schema itself lives here so it can grow
without bloating the entry map.

## Verified facts

- **Front-matter fields** (`index.md:73-75`):
  - `title` — quoted human-readable page title.
  - `type` — page category. Values observed in this tree: `index`, `source-map`,
    `cli`, `rule`, `feature`, `schema`, `backlog`, `source`. This is a
    descriptive list of what exists, not a closed enum enforced anywhere found
    in the repo (see Open questions in `backlog/open-questions.md`).
  - `status` — one of `verified`, `partial`, `unverified` (`index.md:73-74`).
  - `last_checked_commit` — the short commit hash the page's claims were last
    verified against.
  - `source_paths` — list of real repo paths (files or directories) backing the
    page's claims; **non-empty when `status: verified`** (`index.md:74-75`).
    Every entry must exist in the tree at `last_checked_commit`.
  - `related_pages` — list of other openwiki pages this page links to; mirrored
    in the body's `## Related pages` section.
  - `external_sources` — optional list of URLs outside the repo (methodology
    references, published guidelines); `[]` when none. Not called out in
    `index.md`'s mini-schema text, but present in every page's front matter
    observed in this tree (e.g. `index.md:19-21`, `rules/sf9_no_cycles.md:17`).
- **The evidence rule** (`index.md:76-77`): every factual claim in the body
  cites a repo `path` or `path:line`. No claim without a source. Anything not
  verifiable from the repo is marked `unverified` (parked in
  `backlog/unverified.md`), never asserted as fact.
- **Body section order** (`index.md:78`): `Purpose` → `Verified facts` →
  `For an agent` → `Evidence` → `Related pages`.
  - `Purpose` — one short paragraph: what the page is for and its scope.
  - `Verified facts` — the sourced claims, each ending in a citation.
  - `For an agent` — task-oriented guidance: what to read/change, what to run
    afterward (build, tests, fixtures, `lizard`, dogfood), and risk callouts
    (e.g. the `Risk: high` pattern used in `cli-contract.md`'s "For an agent").
  - `Evidence` — a flat list of the exact paths/lines cited above, for a fast
    independent re-check.
  - `Related pages` — wiki-style links (`[[path.md]]`), mirroring the
    `related_pages` front-matter list.

## For an agent

- Adding a new openwiki page: follow the section order above, fill every
  front-matter field, and verify each `source_paths` entry actually exists
  before treating the page as done.
- This repo has a deterministic checker, `scripts/check_openwiki.py` (also
  exposed as the `openwiki-check` skill), that enforces the required keys
  (`title`, `type`, `status`, `last_checked_commit`, `source_paths`), the
  `status` enum, non-empty `source_paths` for `verified` pages, that every
  cited `source_paths` entry exists on disk, that inline markdown links to
  other pages resolve, and flags pages unreachable from `index.md` (except
  `backlog/`, `sources/`, and `index.md`/`log.md` themselves) as orphan
  warnings. Run it
  after adding or editing a page.
- Do not restate content that already lives in an authoritative doc
  (CHANGELOG.md, docs/duplication_architecture.md, ADRs) — link to it instead;
  restated lists drift (this mirrors CLAUDE.md's own scope-discipline rule).

## Standard checks after any change

Single source for the post-change recipe. Each page's `For an agent` section names
only its own test + fixture, then points here instead of repeating the generic steps:

1. Rebuild Debug — `cmake --build build/debug`.
2. Run the tests — `./build/debug/tests/archcheck_tests` (or the specific
   `*_test.cpp` + `fixtures/<name>/` the changed page names).
3. `lizard --CCN 15 --length 30 --arguments 5 --warnings_only src/ include/ tests/`.
4. Dogfood — `./build/debug/src/archcheck src/ include/ tests/` must report 0
   (archcheck passes its own rules).

The commands are canonical in CLAUDE.md / AGENTS.md; this is the per-change
checklist, not a second source of truth for the commands themselves.

## Evidence

- `index.md:69-78` — the mini-schema this page promotes.
- `index.md:19-21`, `rules/sf9_no_cycles.md:17` — `external_sources: []` usage.
- `scripts/check_openwiki.py:27-32` — the enforced required keys, valid
  `status` values, and the orphan-exemption list (`backlog/`, `sources/`,
  `index.md`, `log.md`).

## Related pages

- [[index.md]] · [[source-map.md]] · [[cli-contract.md]] · [[log.md]]
