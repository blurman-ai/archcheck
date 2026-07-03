# [DOCS] Thin source-backed agent-navigation layer (openwiki) for archcheck

**Created:** 2026-07-03
**Start date:** —
**Status:** new
**Module:** DOCS
**Priority:** maj
**Difficulty:** unknown
**Blocks:** —
**Blocked by:** —
**Related:** —

## Goal

Add a small `docs/openwiki/` layer that lets a coding agent answer "I want to
change X — which files, tests, and fixtures do I read first?" from **source-backed
navigation pages**, without re-documenting what the repo already documents.

## Context — this repo is NOT a bare repo

The original external spec (`task_create_openwiki_for_archcheck.md`) was written
for a repository with no docs. archcheck is the opposite: it already has a mature,
cross-linked doc corpus. So the openwiki layer must be **thin and non-duplicative**.

Already present (do **not** re-create — link into these):

- `AGENTS.md` + `CLAUDE.md` — agent-instruction files, already cross-linked and
  already pointing into CHANGELOG/spec/MVP. (The external spec's "create AGENTS.md"
  step is already done — we only add a one-line pointer.)
- `CHANGELOG.md` — **authoritative** for what shipped (rules, thresholds, modes).
- `docs/GLOSSARY.md` — project vocabulary. (Replaces the spec's `glossary/terms.md`.)
- `docs/architecture-spec.md`, `docs/MVP.md`, `docs/ROADMAP.md`, `docs/product_vision.md`.
- `docs/decisions/00{1,2,3}-*.md` — ADRs. (Replaces the spec's `decisions/`.)
- `docs/config_format.md`, `docs/ci_usage.md`, `docs/ci_integration.md`,
  `docs/baseline_format.md` — (replace the spec's `cli/`, `workflows/` pages).
- `docs/duplication_architecture.md`, `docs/duplication_fp_analysis.md`.
- `backlog/completed/` — knowledge archive; `docs/dev/` — dev process.

**The only non-duplicative value of openwiki here is:**

1. **An agent entry map** (`index.md`): "touching area X → read these pages/files/tests."
2. **A single source→rule→test→fixture matrix** (`source-map.md` + `rules/*.md`) —
   today this mapping is scattered across `src/rules/`, `fixtures/`, `tests/`.
3. **Explicit evidence status** per claim (verified / partial / unverified).

Hard rule, matching the repo's own norm: **do not restate shipped rule lists or
thresholds** — CHANGELOG is authoritative and such lists drift. Link, don't copy.

## Non-negotiable rule: no guessing

Every claim about archcheck must be one of:

1. **Verified** — cites a repo path (source / test / fixture / CLI output / CHANGELOG
   or docs section / commit).
2. **External-method** — cites an OpenWiki / LLM Wiki source and is about wiki
   *process*, not archcheck behavior.
3. **Unverified** — explicitly marked and parked in `docs/openwiki/backlog/`.

Do not turn ideas/conversations/assumptions into factual docs.

## Right-sized structure (grounded in this repo)

```text
docs/openwiki/
  index.md          — agent entry map; links to CHANGELOG, GLOSSARY, spec, decisions, and the pages below
  schema.md         — page front-matter + evidence rules (the discipline doc)
  log.md            — append-only record of creation / updates
  source-map.md     — subsystem matrix: src/<mod> ↔ include/archcheck/<mod> ↔ tests/* ↔ fixtures/*
                      for cli, config, diff, git, graph, report, rules, scan, scan/duplication
  rules/
    <rule>.md       — one page per SHIPPED rule/check (see below)
  cli-contract.md   — modes + flags + exit-code contract; links README/--help, does NOT restate thresholds
  backlog/
    open-questions.md
    unverified.md   — candidate terms/features not found in the repo
  sources/
    external-openwiki-practices.md  — the adopted method + the 5 external links, in one page
```

Notes:
- Location is `docs/openwiki/` (fits the repo's `docs/` discipline), not a new
  top-level dir. If you prefer root-level `openwiki/` per OpenWiki convention,
  that's a maintainer call — pick one and be consistent. (Key decision below.)
- No separate `glossary/`, `decisions/`, `workflows/`, `verification/` trees —
  those already live under `docs/`. `index.md` links to them.

## Rule pages — the core deliverable

The shipped rule/check set is concrete and enumerable, not a guess. Discover it from
(authoritative → implementation):

- `CHANGELOG.md` — what actually shipped and its gate/advisory status;
- `src/rules/rule_set.cpp` — `makeDefaultRuleSet` / `makeDriftRuleSet` registration;
- `src/rules/*.cpp` — one class per rule (sf7/sf8/sf9, lakos god-headers/chain-length,
  drift shortcut-edge/bidirectional/lateral/cycle-growth, gate_policy, …);
- `fixtures/*` — the fixture dirs map ~1:1 to rules and are the discovery anchor;
- `tests/*` — the covering tests.

One page per shipped rule, answering only what the repo proves:
- what it detects; gate vs advisory (from CHANGELOG/README);
- implementing file(s) in `src/rules/`; covering `fixtures/<rule>/` and `tests/`;
- CLI flag / config field that affects it; the exact `file:line: [rule] message` it emits;
- documented FP/FN, if any; what to read before changing it; what to run after
  (build + `fixtures` + `lizard` + dogfood `archcheck src/ include/ tests/`).

Do not claim motivation unless it appears in repo docs.

## Page front matter (minimal)

```yaml
title: "..."
type: index | schema | source-map | rule | cli | backlog | source
status: verified | partial | unverified
last_checked_commit: "<git-sha>"
source_paths: ["path/to/file", ...]     # required; empty is illegal for status: verified
related_pages: ["../page.md", ...]
```

Body: `# Title` → `## Purpose` → `## Verified facts` (each bullet cites a source
path) → `## For an agent` (what to read/run, what not to change blindly) →
`## Evidence` → `## Open questions` (optional) → `## Related`.

## Agent-instruction pointer (already-existing files)

Add a **one-line** pointer to both `AGENTS.md` and `CLAUDE.md`, e.g.:
> Agent navigation map: `docs/openwiki/index.md` (derived — never trust it over
> code/tests/CHANGELOG; update affected pages in the same PR when behavior changes).

Do not embed the wiki into either file (respect CLAUDE.md scope discipline).

## Optional / deferred (do NOT do in this task unless trivially safe)

- **OpenWiki CLI** (`npm i -g openwiki && openwiki --init`): unverified whether it
  exists/applies to a C++ repo or works offline. The **primary path is the manual
  source-backed build above.** Do not run any provider-calling tool without
  maintainer OK; if attempted and blocked, record it in `log.md`.
- **`.github/workflows/openwiki-update.yml`** (LLM-calling CI): **out of scope.**
  The repo's CI/release philosophy is fixed and product is CLI/CI-first (YAGNI).
  At most, note a proposed workflow in `sources/external-openwiki-practices.md`
  marked `status: partial`. No scheduled external LLM calls without maintainer review.
- **No verifier script.** The external spec assumed a large auto-generated wiki
  needing a lint tool; this is ~15 hand-authored pages, so a checker is YAGNI.
  Review links and front-matter by eye at PR time; add tooling only if the wiki
  actually grows large enough to need it.

## Execution plan

- [ ] Capture the adopted method + 5 external links in `sources/external-openwiki-practices.md`.
- [ ] Build `source-map.md` from `src/` ↔ `include/archcheck/` ↔ `tests/` ↔ `fixtures/`.
- [ ] Enumerate shipped rules from CHANGELOG + `rule_set.cpp` + `src/rules/*.cpp` + `fixtures/`.
- [ ] Write one `rules/<rule>.md` per shipped rule (verified, with evidence paths).
- [ ] Write `cli-contract.md` (modes/flags + exit codes 0/1/2/3), linking README/--help — no restated thresholds.
- [ ] Write `index.md` (entry map + links to CHANGELOG/GLOSSARY/spec/decisions/pages) and `schema.md`.
- [ ] Start `log.md` (append-only) with the creation pass.
- [ ] Park anything unverifiable in `backlog/open-questions.md` / `backlog/unverified.md`.
- [ ] Add the one-line pointer to `AGENTS.md` and `CLAUDE.md`.
- [ ] Review links + front-matter by eye; fix or park broken ones in `backlog/`.
- [ ] Final summary: pages created, rules covered, unverified items, known gaps.

## Done
- (empty)

## In progress
- (empty)

## Next steps
1. Build `source-map.md` + enumerate shipped rules from CHANGELOG/`rule_set.cpp`/`fixtures/`.

## Key decisions

| Decision | Reason |
|---------|---------|
| `docs/openwiki/`, not root `openwiki/` | Fits the repo's existing `docs/` home; avoids a new top-level tree. (Flip only by maintainer call.) |
| Link into existing docs, don't duplicate | `docs/GLOSSARY.md`, `docs/decisions/`, `docs/config_format.md`, `docs/ci_usage.md`, `docs/baseline_format.md`, `docs/duplication_architecture.md` already exist; restated lists drift (CHANGELOG is authoritative). |
| One-line pointer only in AGENTS.md/CLAUDE.md | Both files already exist and do the agent-instruction job; scope discipline forbids embedding lists. |
| No verifier script | ~15 hand-authored pages — a lint tool is YAGNI. The external spec's `verify_openwiki.py` assumed a large generated wiki. Review links by eye; add tooling only if it grows. |
| No LLM-calling CI / OpenWiki CLI in this task | Product is CLI/CI-first, YAGNI; no scheduled external LLM calls without maintainer review. |

## Changed files

| File | Change |
|------|-----------|
| ... | ... |

## Acceptance criteria

- [ ] `docs/openwiki/index.md` exists and links the pages below + CHANGELOG/GLOSSARY/spec/decisions.
- [ ] `docs/openwiki/source-map.md` maps each `src/` subsystem to its `include/`, `tests/`, `fixtures/`.
- [ ] One `rules/<rule>.md` per shipped rule, each with `source_paths` (impl + fixture + test).
- [ ] `docs/openwiki/cli-contract.md` states modes/flags + exit codes and links README/--help without restating thresholds.
- [ ] `docs/openwiki/schema.md` + `log.md` exist; `sources/external-openwiki-practices.md` links the 5 sources.
- [ ] Unverifiable items are isolated in `docs/openwiki/backlog/`, not asserted as fact.
- [ ] No page with `status: verified` has empty `source_paths`; local links resolve.
- [ ] `AGENTS.md` + `CLAUDE.md` each get a one-line pointer; wiki is not embedded.
- [ ] No secrets/API keys/provider config committed.
- [ ] PR description states the wiki is **derived** documentation, not to be trusted over code/tests/CHANGELOG.

## Out of scope

- Re-creating glossary/decisions/CI/config/baseline docs that already exist under `docs/`.
- Restating shipped rule lists or thresholds (link CHANGELOG instead).
- Adding archcheck checks/features; changing analyzer behavior or CLI output.
- LLM-calling GitHub Actions or scheduled external LLM calls (needs maintainer review).
- Treating OpenWiki-generated text as automatically correct.

---

_Adapted from the user-provided external spec `task_create_openwiki_for_archcheck.md`
(2026-07-03). The external spec assumed a docs-less repo; this version is retargeted
to archcheck's existing docs and shipped rule set — speculative structure and
made-up assumptions were removed._
