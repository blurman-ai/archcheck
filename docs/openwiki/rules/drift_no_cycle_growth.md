---
title: "DRIFT.2 — no cycle growth (new or grown SCCs vs. baseline)"
type: rule
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "src/rules/drift_no_cycle_growth.cpp"
  - "include/archcheck/rules/drift_no_cycle_growth.h"
  - "src/rules/rule_set.cpp"
  - "src/rules/gate_policy.cpp"
  - "tests/unit/rules/drift_no_cycle_growth_test.cpp"
  - "fixtures/drift_cycle_growth/"
related_pages:
  - "../index.md"
  - "../cli-contract.md"
  - "../source-map.md"
external_sources: []
---

# DRIFT.2 — no cycle growth (new or grown SCCs vs. baseline)

## Purpose

A drift rule that reports include cycles (SCCs) that are either brand new or
have **grown** in membership compared to the baseline graph. A cycle growing
from N to M members means each newly trapped file lost the ability to compile
or be tested independently of the rest of the cycle — a clear structural
regression, distinct from SF.9 (which flags *any* cycle present in a single
snapshot, with no notion of "before").

## Verified facts

- **Rule id / message**: emits
  `{"DRIFT.2", <path of scc.members.front()>, 0, msg}`
  (`src/rules/drift_no_cycle_growth.cpp:23`, literal id confirmed with
  `grep -F '"DRIFT.2"' src/rules/drift_no_cycle_growth.cpp`). `line = 0` —
  graph-level rule.
- **Message construction** (`:18-22`, both branches confirmed with
  `grep -F`):
  - New cycle (`scc.baselineSize == 0`):
    `"new cycle (" + std::to_string(scc.currentSize) + " members):"` (`:18`).
  - Grown cycle (`scc.baselineSize != 0`):
    `"cycle grew from " + std::to_string(scc.baselineSize) + " to " + std::to_string(scc.currentSize) + " members:"` (`:19-20`).
  - Both branches then append each member path, space-prefixed:
    `msg += " " + std::string(graph.pathOf(m))` for `m` in `scc.members` (`:21-22`).
- **Detection source**: iterates `graph::grownSccs(baseline_, graph)` (`:16`),
  a graph-diff primitive in `include/archcheck/graph/diff.h` / `src/graph/diff.cpp` (not re-verified
  line-by-line in this page — see that file directly for how "grown" is
  computed, i.e. `baselineSize`/`currentSize`/`members` per SCC).
- **Class shape**: `DriftNoCycleGrowth(graph::DependencyGraph baseline)` stores
  `baseline_` (`:10`, header `:17,23`). `id()` returns `"DRIFT.2"`
  (header `:20`).
- **Registration**: `makeDriftRuleSet` →
  `push_back(make_unique<DriftNoCycleGrowth>(std::move(baseline)))`
  (`src/rules/rule_set.cpp:33` — note this is the only one of the four drift
  rules constructed with `std::move`, since it is the last consumer of
  `baseline` in that factory function).
- **Gate status**: **gating**. `classifyForGate` lists `"DRIFT.2"` explicitly
  in its gating set for `GateMode::Drift` (`src/rules/gate_policy.cpp:19`).
  Confirmed by `tests/unit/rules/gate_policy_test.cpp:32`
  (`CHECK(isGating("DRIFT.2", GateMode::Drift))`). Also in the README core-gate
  table (`README.md:45,52`).
- **Distinct related mechanism — do not conflate**: the `--diff <revspec>` PR
  workflow does **not** run this rule or go through `gate_policy.cpp` at all.
  It uses a separate `RegressionReport` built by
  `src/diff/regression_report.cpp` (`buildRegressionReport`, `:168-182`), whose
  `collectEdgesAndCycles()` (`:127-146`) independently calls
  `graph::grownSccs(baseline, current)` (`:136`) to populate
  `RegressionReport::grownCycles`, and whose `gates()` method
  (`include/archcheck/diff/regression_report.h:80`) gates when
  `!grownCycles.empty() || !newGodHeaders.empty()`. Both paths use the same
  underlying `graph::grownSccs` primitive but are two independent call sites —
  see `rules/gate_policy.md` for the full mechanism split.
- **Suppression**: update the graph baseline (`--save-graph-baseline`) after
  review (header comment, `include/archcheck/rules/drift_no_cycle_growth.h:13`).
- **No threshold** — structural (SCC membership) check, not a metric compared
  against a number.

## For an agent

- Change detection → `src/rules/drift_no_cycle_growth.cpp` (+ header). The SCC
  growth computation itself (`graph::grownSccs`) is shared with
  `src/diff/regression_report.cpp` — touching it is `Risk: high` (affects both
  the `check`-mode drift gate and the `--diff` PR workflow simultaneously).
- After changing: rebuild Debug, run
  `tests/unit/rules/drift_no_cycle_growth_test.cpp` (96 lines), exercise
  `fixtures/drift_cycle_growth/{pass,fail_new_cycle}/`, then run the standard post-change checks (see [schema.md](../schema.md)).

## Evidence

- `src/rules/drift_no_cycle_growth.cpp:23` — the emitted `DRIFT.2` violation.
- `src/rules/drift_no_cycle_growth.cpp:18-22` — the two message-branch literals.
- `src/rules/rule_set.cpp:33` — drift-set registration.
- `src/rules/gate_policy.cpp:19` and `tests/unit/rules/gate_policy_test.cpp:32` —
  gating classification.
- `src/diff/regression_report.cpp:136` and
  `include/archcheck/diff/regression_report.h:80` — the separate `--diff`
  gate path that also consumes `graph::grownSccs`.

## Related pages

- [[../index.md]] · [[../cli-contract.md]] · [[../source-map.md]]
