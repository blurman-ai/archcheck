---
title: "openwiki — open questions"
type: backlog
status: partial
last_checked_commit: "241b07e"
source_paths:
  - "scripts/check_openwiki.py"
  - "include/archcheck/scan/duplication/duplication_scanner.h"
  - "src/scan/duplication/duplication_scanner.cpp"
  - "src/scan/new_clone_drift.cpp"
  - "src/cli/diff_command.cpp"
related_pages:
  - "../index.md"
  - "../schema.md"
  - "../features/duplication.md"
external_sources: []
---

# openwiki — open questions

## Purpose

Genuine gaps this fan-out pass could not close, with where I looked. Not
padding — items are only listed here if a concrete check left an unresolved
answer.

## Verified facts

(N/A — this page records open questions, not closed claims. See "Open
questions" below.)

## For an agent

Before re-investigating an item below, check whether a later commit already
answers it (`git log` on the cited paths) — these questions are only open as
of `last_checked_commit`.

## Open questions

1. **New pages are orphaned from `index.md` by construction of this task.**
   Running `python3 scripts/check_openwiki.py` after this pass reports 14
   `[orphan]` warnings, including `schema.md` and `features/duplication.md`
   (this pass's own deliverables) plus sibling pages added in the same
   fan-out (`features/{bool_field_drift,flag_argument_drift,...}.md`,
   `rules/drift_*.md`, `rules/gate_policy.md`). The task instructions for this
   pass explicitly forbid editing `index.md` (hard rule #3), and `index.md`'s
   "Pilot scope" section (`index.md:32-46`) only lists the original five
   default-rule pages. Open question: when/who updates `index.md`'s scope list
   and "Where to look" table to link the fanned-out pages, so the orphan
   warnings clear? Left for a follow-up task that is explicitly scoped to edit
   `index.md`.
2. **Is `--duplication`'s similarity threshold configurable?** The default
   gate is `ScannerOptions::simThreshold = 0.60`
   (`include/archcheck/scan/duplication/duplication_scanner.h:33`), compared
   at `src/scan/duplication/duplication_scanner.cpp:54`. README's "Default
   thresholds" table (`README.md:161-166`) lists only include-chain-length and
   god-header fan-in as `.archcheck.yml`-overridable; `grep -rn simThreshold
   src/ include/` finds only the default and the comparison site — no
   config-loader wiring (`grep -rn duplication src/config/
   include/archcheck/config/ docs/config_format.md` returns nothing). Left
   open whether this is a deliberate v0.1 scope decision (parallel to
   ADR-001's module-rule-enforcement deferral) or simply undocumented; did not
   find an ADR or CHANGELOG line stating it either way.
3. **Does `--diff`'s "new-clone drift" signal share the token-detector
   pipeline with `--duplication`, or is it independent?** Both are advisory
   duplicate-detection features. `--diff` wires `collectNewClones` →
   `archcheck::scan::detectNewClones` (`src/cli/diff_command.cpp:151-163`,
   implemented in `src/scan/new_clone_drift.cpp`), separate from
   `scanForDuplication` (`src/cli/preview_commands.cpp:130`, the `--duplication`
   entry point). I confirmed both files exist and both use the
   `scan::duplication::` namespace's primitives somewhere, but did not read
   `src/scan/new_clone_drift.cpp` in full to confirm whether it reuses
   `fragmenter`/`clone_index`/`similarity` or only the shared lexer. Out of
   scope for the thin pointer page in `features/duplication.md` — flagged here
   instead of guessed at.
4. **Is the front-matter `type` field a closed enum?** `scripts/check_openwiki.py`
   validates `status` against a fixed set (`VALID_STATUS`,
   `scripts/check_openwiki.py:28`) but does **not** validate `type` — any
   string passes as long as the key is present and non-empty
   (`REQUIRED_KEYS`, `scripts/check_openwiki.py:27`). `schema.md` lists the
   `type` values observed in this tree as descriptive, not enforced. Open
   question: should the checker gain a closed `type` enum, and if so what's
   the canonical list (the ones already in use: `index`, `source-map`, `cli`,
   `rule`, `feature`, `schema`, `backlog`, `source`)?

### Checked and resolved (not open)

- Whether YAML module-rule enforcement (`layers`/`independence`/`forbidden`)
  has shipped: **no**, by design — parsing/validation ship in v0.1, enforcement
  is deferred to v0.2 (`docs/decisions/001-config-rules-deferred-to-v0.2.md`,
  `README.md:215`). Checked, not listed above as open.

## Evidence

- `scripts/check_openwiki.py` run output (14 orphan warnings, 0 errors) as of
  this pass.
- `include/archcheck/scan/duplication/duplication_scanner.h:33`,
  `src/scan/duplication/duplication_scanner.cpp:54` — `simThreshold` default
  and use site.
- `README.md:161-166` — the overridable-thresholds table (duplication absent).
- `src/cli/diff_command.cpp:151-163` — `collectNewClones` wiring.
- `scripts/check_openwiki.py:27-28` — required keys vs. validated `status` enum.

## Related pages

- [[../index.md]] · [[../schema.md]] · [[../features/duplication.md]]
