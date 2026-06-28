# [RULES][CLI] First-run noise floor: ChainLength/GodHeader flood on header-heavy libs

**Created:** 2026-06-19
**Started:** 2026-06-19
**Status:** done — main fix (advisory in check-mode) shipped + committed; `--diff` mass-rename guard implemented, tested, verified e2e (2026-06-28). Pending: `/commit`.
**Module:** RULES / GRAPH / CLI
**Priority:** major (go-public quality — first contact)

## Done (2026-06-19) — the "advisory in check-mode" decision

User's choice: **gating (exit 1) in check-mode only for a cycle (SF.9)**; ChainLength,
GodHeader, SF.7/SF.8 — reported but advisory (exit 0). This is recommendation §7 of the report
("gate = cycles; god-header and the rest advisory") and a mirror of the `--diff`/drift model.

- `src/cli/check_command.cpp`: added `reportCheckGate` (modeled on `reportDriftGate`)
  — exit 1 only if there's an `SF.9`; prints "N advisory finding(s) … not gated …
  use --baseline". Replaced `return all.empty() ? 0 : 1;`.
- Test `cli_smoke_e2e_test.cpp`: a new cycle fixture (gating exit 1) + the case "SF.7
  reported but exit 0"; json test → exit 0.
- **Verification (re-run sanity):** abseil 219 findings → **exit 0**, spdlog 40 → **0**,
  fmt → **0**; curl → **exit 1** (the cycle `curl.h↔multi.h` gates — TP, as it should).
  547/547 tests, dogfood 0, lizard/format clean. NOT committed (parallel sessions).
- ⚠️ **The exit-code contract shifted** (check-mode: 1 = cycles only, not "any
  violation") — needs to be recorded in CHANGELOG (v0.1, pre-tag, acceptable).

**Remainder:** only the `--diff` mass-rename guard (below) — narrow, doesn't block go-public.
**Blocks:** the announcement / public release (the first foreign repo must not drown in noise)
**Blocked by:** —
**Related:** #127 (vendored exclusion — removes PART of the noise, but abseil isn't vendored), #126 (SF.9 component collapse), #057 (cheap graph signals), MVP.md §"--baseline from day one"

## Evidence (first-run sanity, 2026-06-19)

`archcheck <repo>` (check-mode, no `--baseline`) on well-known C++ repos:

| Repo | Total | Breakdown | exit |
|------|-------|-----------|------|
| fmt | 1 | SF.8 ×1 | — |
| nlohmann_json | 2 | ChainLength ×2 | — |
| spdlog | 40 | **ChainLength ×39**, SF.8 ×1 | 1 |
| curl | 14 | ChainLength ×5, **GodHeader ×8**, SF.9 ×1 (TP) | 1 |
| **abseil** | **219** | **ChainLength ×211**, GodHeader ×8 | 1 |

**Conclusion:** first-run noise is **ChainLength** (chain threshold 10) and secondarily
**GodHeader** (fan-in 50). Cycles do NOT make noise (abseil/spdlog — 0 cycles; the curl cycle
`curl.h↔multi.h` is a genuine TP). This is exactly the "5000 violations on first run" that the
spec defends against with `--baseline` — but a **naive first `archcheck <repo>`** (which is
exactly how a person probes the tool) yields abseil = 219, exit 1, and it gets written off.

## Why this is a go-public blocker

The first thing a skeptic from HN does: `archcheck` on their own/a known repo **with no flags**.
If the answer is "219 violations, mostly include chain depth 11 > 10" — they close the
tab. The narrow, indisputable value (a cycle/copy-paste introduced by a PR) drowns in ChainLength noise.

## What to decide (this is design, not patched in haste)

1. **ChainLength threshold 10 — too aggressive for modern header-heavy C++.**
   abseil/spdlog/nlohmann deep chains are the norm, not debt. Options:
   - raise the default threshold (15? 20? — measure the distribution across the corpus);
   - make ChainLength **advisory** (not gating/exit-1) in check-mode — keep
     gating only for cycle/god-header (as in `--diff`);
   - keep the threshold, but in check-mode without `--baseline` print an explicit hint
     "N existing findings — run with --baseline to gate only new drift".
2. **GodHeader on config/logging hubs** (`curl_setup.h` fan-in 309) — structurally-but-
   legitimate (see the showcase history #003, removed as weak). A fan-in-only proxy.
   Option: allowlist of known wide headers / lower severity to advisory.
3. **First-run UX:** perhaps the default `archcheck <repo>` without `--baseline` should
   itself suggest `--baseline` ("this is a snapshot of all debt; for the CI gate use --diff").

## Cycle-gate mass-rename (DONE 2026-06-28)

In check-mode cycles are clean. In `--diff` ~19% of cycle-fires are artifacts of mass
include-rewrite/move (coal 252 files, allwpilib 2477).

**Decision (refined from the original ">N rename edges" plan):** a global rename count
needs an arbitrary threshold (the self-check below warns against that). Instead the guard is
**per-cycle and precise** — a grown cycle is dropped iff *every* member is a freshly-renamed
path (a pre-existing cycle re-pathed by `git mv`; SCC membership is keyed on path, so a moved
cycle looks brand-new). A cycle that mixes moved + unmoved files (a genuine new edge) still
gates. No threshold to tune; the artifact is identified directly.

**Implementation:**
- `git/diff_query.{h,cpp}`: `collectRenamedPaths` — `git diff --find-renames --diff-filter=R
  --name-only`, returns new-side paths (worktree-ref aware).
- `diff/regression_report.{h,cpp}`: pure `dropRenameArtifactCycles(report, renamedNewPaths)`
  — erases grown cycles whose members ⊆ renamed set; returns count (unit-testable, no git).
- `cli/diff_command.cpp`: `applyRenameArtifactGuard` wires it after the graph build (both
  memory + disk modes, before gate/JSON). Surfaced as a `note:` (text) and
  `advisory.rename_suppressed_cycles` (JSON).
- Tests: 3 unit (`[diff][report][rename]`) + 1 git e2e (`[diff][git][integration][rename]`).
- e2e on the real binary: pure mass-move → gate ok / exit 0 / note; genuine new cycle (no
  rename) → gate fail / exit 1; mixed cycle (one unmoved member) → gate fail / exit 1.

The `.tmpl/_impl` idiom is already excluded (#088/#126) and untouched (the curl cycle and the
JANA2 gem stay genuine TP).

## Verification (fixtures mandatory)

- [x] Decision on ChainLength fixed → **advisory** in check-mode (exit 0); gate = SF.9 only.
- [x] Tests: check-gate (`cli_smoke_e2e`, `gate_policy_test`) + mass-rename guard (4 cases).
- [x] Re-run sanity: abseil 219 / spdlog 40 / fmt → exit 0; curl cycle → exit 1 (TP).
- [x] Cycle/copy-paste TP (the curl cycle, JANA2) NOT suppressed — verified: a cycle with any
      unmoved member still gates; only fully-renamed cycles drop.

## Self-check

Not "lower the thresholds so the numbers look quiet" — that's the inverse deception. The goal is for the
**remaining** firings to be defensible, while existing debt goes through `--baseline`,
as intended. Measure the chain distribution across the corpus before shifting the threshold.

Held to: the check-mode fix made physical-design proxies advisory (not silenced — still
reported, exit 0) and kept the cycle gate; the `--diff` rename guard rejected the arbitrary
">N rename edges" threshold for a precise per-cycle predicate, so nothing was tuned to look
quiet — only the provable artifact (a fully-renamed cycle) drops.

## How it works

Two independent pieces under one go-public goal — "the first naive run must not drown in noise":
1. **Check-mode advisory gate** (`gate_policy.cpp`): a plain `archcheck <repo>` gates (exit 1)
   only on a dependency cycle (SF.9); ChainLength / GodHeader / SF.7-8 are reported but advisory
   (exit 0). Existing debt belongs behind `--baseline`, not a hard exit.
2. **`--diff` mass-rename guard**: a `git mv` re-paths a pre-existing cycle, and because SCC
   membership is keyed on path the move looks like a brand-new gating cycle. The guard drops a
   grown cycle iff *every* member is a freshly-renamed path; a cycle mixing moved + unmoved
   files still gates. No threshold.

## Key decisions

- **ChainLength → advisory, not a higher threshold**: header-heavy modern C++ (abseil, spdlog)
  has deep chains as the norm; raising N just moves the arbitrary line. Advisory + `--baseline`
  is the honest model and mirrors `--diff`.
- **Rename guard is per-cycle, not a global rename count**: a count needs an arbitrary N and
  mislabels large-but-genuine refactors; the "all members renamed" predicate identifies the
  artifact directly and needs no tuning.
- **Suppression is surfaced, not silent** (`note:` in text, `rename_suppressed_cycles` in JSON):
  a dropped finding must be visible.

## Changed files

- `src/rules/gate_policy.cpp`, `src/cli/check_command.cpp` — check-mode advisory gate (committed
  earlier in the task; see `reportCheckGate` / `GateMode::Check`).
- `src/git/diff_query.{h,cpp}` — `collectRenamedPaths` (commit 206acad).
- `src/diff/regression_report.{h,cpp}` — `dropRenameArtifactCycles` (commit 206acad).
- `src/cli/diff_command.cpp` — `applyRenameArtifactGuard` + text/JSON surfacing (commit 206acad).
- `src/diff/diff_json_report.{h,cpp}` — `rename_suppressed_cycles` field (commit 206acad).
- `tests/unit/diff/regression_report_test.cpp`, `tests/integration/diff/git_diff_test.cpp`
  — 3 unit + 1 e2e rename tests (commit 206acad).
- `CHANGELOG.md` — both the check-mode contract shift and the rename guard (commit 206acad).

## Summary

**Status:** completed
**Completion date:** 2026-06-28
First-run noise floor closed on both fronts: check-mode gates only on cycles (the rest advisory,
debt → `--baseline`), and `--diff` no longer fails a refactor whose cycles are pure rename
artifacts. 595 tests pass, coverage PASS, dogfood 0; e2e verified pure-move→exit 0,
genuine/mixed cycle→exit 1.
