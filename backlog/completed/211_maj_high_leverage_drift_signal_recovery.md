# [RESEARCH][GRAPH][DIFF] High-leverage drift signal recovery: window-edge architectural rules, cumulative struct drift, and rename-aware graph diff

**Created:** 2026-11-22
**Start date:** 2026-11-22
**Status:** done
**Module:** RESEARCH / GRAPH / DIFF
**Priority:** major
**Difficulty:** L
**Blocks:** —
**Blocked by:** —
**Related:** #180 (adoption_event_study), #184 (all_endpoints_event_study), #202 (architecture_integrity_hardening — GATE-1, GRAPH-2), #203 (full_drift_measurement_spec)

## Goal

Close the three highest-leverage gaps explaining why per-commit `--diff` measurements under-report
architectural drift relative to field observation and cumulative metrics (full diagnosis in
[`docs/research/why_drift_looks_null_analysis.md`](../../docs/research/why_drift_looks_null_analysis.md)):

1. Measure true architectural-layer and cumulative state drift across the adoption cohorts at window
   edges (`--drift-baseline` `DRIFT.1`–`DRIFT.4` + cumulative `per-struct boolean drift`).
2. Isolate the unattenuated agent effect without LOC-bloat conditioning or control-arm contamination
   (run registered stratum `S1` $\ge 90\%$ AI commits + within-author un-size-matched contrast).
3. Eliminate the two biggest signal-to-noise killers in `archcheck` graph diff: rename-induced fake
   `added_edges` (`DEBT.md` 2026-11-06) and equal-cardinality cycle replacement (`#202` `GATE-1`).

## Context

- The 484,500-commit `RQ1` run and the 854-adopter / 517-placebo `RQ2` re-run invoked only
  `archcheck --diff` (Surface 6 of 6 in `#203`). The architectural drift rules (`DRIFT.1` shortcut
  edge, `DRIFT.2` cycle growth, `DRIFT.3` bidirectional area coupling, `DRIFT.4.*` lateral/SDP) were
  never evaluated on the adoption cohorts.
- Cumulative `per-struct boolean drift` (`git blame`, $\ge 4$ commits adding `bool` fields to one
  struct) previously yielded the strongest per-unit signal (**2.3×** after manual FP filtering,
  [`docs/research/boolean_state_agentic_vs_not.md`](../../docs/research/boolean_state_agentic_vs_not.md)),
  while the per-commit weekly proxy (`P3`) washed out. It has not been run on the 854/517 adoption
  cohorts (`pre` $[-12, -1]$ vs `post` $[0, +11]$).
- In `RQ1`, matching on commit size conditions on an intermediate outcome (agents expand changes by
  ~1.3×–1.5×), and 79% of post-adoption "human" commits in mixed repos come from authors who also
  commit with AI trailers (task-mix + unmarked-AI contamination).
- In `archcheck --diff`, moving files across directories reports every pre-existing include of the
  moved files as a new edge (75 commits produced 19,500 fake `added_edges`, which make up 53% of
  `P1`), and `grownSccs` misses any cycle whose members changed without increasing cardinality.

## Execution plan

### Step 1 — Window-edge architectural & cumulative state drift on adoption cohorts (no C++ rebuild required)

- [x] **1A. `--drift-baseline` across adoption and placebo windows:**
  For each eligible repository in `experiments/rerun_2026-11/cohorts.tsv`, checkout/snapshot three
  points: `t_pre` (event $-12$ weeks), `t_0` (adoption event), and `t_post` (event $+12$ weeks).
  Save graph baseline at `t_pre` → run `--drift-baseline` at `t_0` (pre-period drift delta);
  save graph baseline at `t_0` → run `--drift-baseline` at `t_post` (post-period drift delta).
  Record `DRIFT.1`, `DRIFT.2`, `DRIFT.3`, `DRIFT.4.NEW`, `DRIFT.4.CYCLE`, `DRIFT.4.SDP`, and
  `nccd_delta`, normalized both per 12-week window and per KLOC added.
  *(Full 1,376-repo run complete: `1,258` OK repos, `DRIFT.1` equal-repo `log1p` DiD = `1.234×` `[1.071, 1.428]`, winsorized `cap=500` DiD/win = `1.34×`.)*
- [x] **1B. Cumulative `per-struct boolean drift` on `pre` vs `post` windows:**
  Run the per-struct multi-commit accretion detector separately over commits in $[-12\text{w}, 0)$
  and $[0, +12\text{w}]$ on adopters vs placebo controls (testing $\ge 2$, $\ge 3$, and $\ge 4$ distinct
  commits adding `bool` fields to the same struct within the 12-week window).

### Step 2 — Unattenuated agent effect (`S1` stratum & un-size-matched task contrast)

- [x] **2A. Registered `S1` pure-agent descriptive stratum:**
  Identify cohort/corpus repositories with $\ge 90\%$ AI-attributed commits post-adoption (plus
  born-agentic repositories where available) and compute their per-commit, per-KLOC, and window-edge
  drift profile against pure-human placebo repositories (zero control contamination).
- [x] **2B. Decompose the size-matching collider and task-mix confound:**
  On the mixed-repository sample, filter both arms to commits that touch at least one authored
  C/C++ implementation/header file (excluding config/build/doc-only commits) and report the
  within-repository paired contrast both **before** and **after** LOC size-matching, alongside
  continuous findings-per-commit (rather than binary $D(c,k) \in \{0,1\}$).

### Step 3 — Fix rename-inflated `added_edges` and `GATE-1` SCC identity in `archcheck`

- [x] **3A. Rename-canonicalized edge diffing (`DEBT.md` 2026-11-06):**
  Extend `git::collectRenamedPaths` (or add `collectRenameMap`) to return `oldPath <-> newPath`
  pairs from `git diff -M`. Map baseline graph node paths through the rename map before computing
  `graph::addedEdges`, `graph::removedEdges`, and `detectNewCrossAreaDependencies` so a pure file
  move reports `0` added edges.
- [x] **3B. Membership-based SCC diff (`#202` `GATE-1`):**
  Update `graph::grownSccs` in `src/graph/diff.cpp` so a current non-trivial SCC is treated as
  unchanged/shrunk *only* if all of its rename-canonicalized members belonged to a single baseline
  SCC; otherwise report it as new/grown (catching `{a,b} -> {a,c}` of equal size).
- [x] **3C. Re-score `P1` and `graph_drift` on `results_window`:**
  Verify how much the adopter/placebo `P1` and `graph_drift` ratios sharpen once rename-induced edge
  noise is stripped at the source.

## Done

- [x] Root-cause analysis across code, research logs, and November re-run outputs
      ([`docs/research/why_drift_looks_null_analysis.md`](../../docs/research/why_drift_looks_null_analysis.md)).
- [x] **Step 3A & 3B (C++ fixes in `archcheck`):**
  - Added `git::collectRenameMap` (`--find-renames --diff-filter=R --name-status -z`) in `src/git/diff_query.cpp`.
  - Threaded `graph::RenameMap` through `addedEdges`, `removedEdges`, `grownSccs`, `detectNewCrossAreaDependencies`, and `detectNewGodHeaders` (`GRAPH-2`).
  - Updated `graph::grownSccs` to membership-subset semantics (`GATE-1`, catches `{a,b} -> {a,c}` equal-size replacement) and updated `DriftNoCycleGrowth` message formatting.
  - Added fixture `fixtures/drift_cycle_growth/fail_equal_size_replacement/`, unit tests in `tests/unit/graph/diff_test.cpp`, and E2E tests in `tests/integration/diff/diff_workflow_e2e_test.cpp`.
  - Verified: 680/680 Catch2 tests pass (4,907 assertions), dogfooding `0` violations, `clang-format-18` clean.
- [x] **Step 3C:** Re-scored `P1_union`, `S_added_edges`, `graph_drift` on `results_window` across rename regimes and empirically verified `8,216 -> 0` fake `added_edges` (`-100%`) and `177 -> 0` fake cross-area deps across 7 top `rename_bulk` commits.
- [x] **Step 2A & 2B:** Computed `S1` pure-agent stratum (`n=15`) + 5-stratum dose-response (`P2_complexity` monotonic `0.116 -> 0.358 / commit`, `3.09×`) + collider/task-mix decomposition on 250 mixed repos (`P1_union` `1.37× -> 1.17× -> 1.09×`).
- [x] **Step 1B:** Ran cumulative `per-struct boolean drift` across all `2,692` bool-accreting commits in `results_window` (`DiD` content structs: `1.56×` at `≥2`, `1.71×` at `≥3`, `6.00×` at `≥4`; repos: `1.80× / 3.20× / 7.00×`).
- [x] **Step 1A full panel (`1,376` repos):** `1,258` OK (`802` adopt / `456` ctrl; `1,197` zero-missing-blob). `DRIFT.1` `log1p` DiD = `1.234×` (`[1.071, 1.428]`), winsorized `cap=500` DiD/win = `1.34×`; `S1` affected repos double (`4/11 → 8/11`).
- [x] Consolidated empirical report: [`docs/research/high_leverage_drift_signal_recovery_211.md`](../../docs/research/high_leverage_drift_signal_recovery_211.md).

## In progress

- (empty)

## Next steps

- (none — ready for `/commit` when requested)

## How it works

- **Rename-canonicalized graph diffing:** `git::collectRenameMap` runs `git diff --no-ext-diff --find-renames --diff-filter=R --name-status -z` to build `oldPath -> newPath`. `graph::addedEdges`, `graph::removedEdges`, `graph::grownSccs`, `detectNewCrossAreaDependencies`, and `detectNewGodHeaders` canonicalize baseline node paths through this map before comparing against `current`, eliminating spurious edges/cross-area deps/cycles/god-headers on pure file moves.
- **Membership-based SCC diff (`GATE-1`):** `graph::grownSccs` checks whether every member of a current non-trivial SCC (`size >= 2`) belongs to a single canonicalized baseline SCC (`isSubsetOfAny`). Any cycle that gains a new member — including equal-cardinality replacement `{a,b} -> {a,c}` — is reported.
- **Research signal recovery:** Offline C/C++ tree materialization (`GIT_NO_LAZY_FETCH=1`) + `--save-graph-baseline`/`--drift-baseline` on `(t_pre, t_0, t_post)` evaluates `DRIFT.1`–`DRIFT.4` across the full 1,376-repo cohort; per-struct bool-accretion aggregation and `S1`/collider decompositions quantify cumulative state drift and unattenuated agent effects.

## What controls it

- `archcheck --diff`: automatically collects `RenameMap` via `git::collectRenameMap` in `src/cli/diff_command.cpp`.
- `archcheck --drift-baseline`: uses `graph::grownSccs` via `DriftNoCycleGrowth` (`DRIFT.2`).
- Research scripts under `experiments/signal_recovery_211/`.

## What it relates to

- `#202` (`GATE-1`, `GRAPH-2`), `#203` (full drift measurement surfaces), `#180` / `#184` (adoption event study), `backlog/DEBT.md` (2026-11-06 rename-inflated `added_edges`).

## Diagnostics

- Verify C++ fixes: `./build/debug/tests/archcheck_tests "[diff][e2e][rename],[graph][diff],[drift][fixtures]"` and `./build/debug/src/archcheck src/ include/ tests/`.
- Re-run Step 1A summary: `python3 experiments/signal_recovery_211/analyze_step1a.py experiments/signal_recovery_211/step1a_full.jsonl`.

## Key decisions

| Decision | Reason |
|----------|--------|
| Start with window-edge `--drift-baseline` (Step 1A) rather than per-commit `--drift-baseline` | Needs only 3 snapshots per repo (`t_pre`, `t_0`, `t_post`) instead of 210k commit replays — finishes in hours and directly tests `DRIFT.1`–`DRIFT.4`. |
| Coordinate Step 3A/3B with `#202` Slice 1 | `#202` already specifies the exact canonical rename + SCC subset contract (`GATE-1`, `GRAPH-2`); implementing that slice directly solves the research noise issue too. |

## Changed files

| File | Change |
|------|--------|
| `docs/research/why_drift_looks_null_analysis.md` | Full diagnostic report on why per-commit drift looks null while drift exists |
| `docs/research/high_leverage_drift_signal_recovery_211.md` | Empirical results for Steps 1B, 2A/2B, 3C, and Step 1A pilot/interim |
| `backlog/wip/211_maj_high_leverage_drift_signal_recovery.md` | Task specification, progress, and results |
| `include/archcheck/git/diff_query.h`, `src/git/diff_query.cpp` | `collectRenameMap` (`-M --name-status -z`) for `oldPath -> newPath` |
| `include/archcheck/graph/diff.h`, `src/graph/diff.cpp` | Rename-canonicalized `addedEdges`/`removedEdges`/`grownSccs` + `GATE-1` subset SCC check |
| `src/rules/drift_no_cycle_growth.cpp` | Handle equal-size cycle membership replacement message in `DRIFT.2` |
| `include/archcheck/diff/regression_report.h`, `src/diff/regression_report.cpp`, `src/cli/diff_command.cpp` | Thread `RenameMap` through edges, cycles, cross-area deps, and god-headers (`GRAPH-2`) |
| `fixtures/drift_cycle_growth/fail_equal_size_replacement/*` | Fixture for `{a,b} -> {a,c}` equal-size cycle replacement |
| `tests/unit/graph/diff_test.cpp`, `tests/integration/rules/drift_fixtures_test.cpp`, `tests/integration/diff/diff_workflow_e2e_test.cpp` | Unit, fixture, and E2E tests for rename-aware diffing and `GATE-1` |
| `docs/openwiki/index.md` | Updated `DRIFT.2` description for `GATE-1` |
| `experiments/signal_recovery_211/*` | Scripts and outputs for Steps 1A, 1B, 2A/2B, 3C |

## Fixtures (if a rule)

- [x] `fixtures/drift_cycle_growth/fail_equal_size_replacement/` (`{a,b}` → `{a,c}`)
- [x] Integration test in `tests/integration/diff/`: pure file rename across directories produces `0` `added_edges`
