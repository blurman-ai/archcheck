---
title: "DRIFT.1 — no shortcut edge (baseline-to-baseline)"
type: rule
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "src/rules/drift_no_shortcut_edge.cpp"
  - "include/archcheck/rules/drift_no_shortcut_edge.h"
  - "src/rules/rule_set.cpp"
  - "src/rules/gate_policy.cpp"
  - "tests/unit/rules/drift_no_shortcut_edge_test.cpp"
  - "fixtures/drift_shortcut_edge/"
related_pages:
  - "../index.md"
  - "../cli-contract.md"
  - "../source-map.md"
external_sources: []
---

# DRIFT.1 — no shortcut edge (baseline-to-baseline)

## Purpose

A drift rule: it does not judge a single snapshot, it compares the **current**
include graph against a saved **baseline** graph and flags a new architectural
coupling. DRIFT.1 fires when two files that both already existed in the baseline
acquire a brand-new `#include` edge between them — the classic "I'll just
`#include` the other module's header, it's convenient right now" shortcut that
erodes physical design one PR at a time.

## Verified facts

- **Rule id / message**: emits
  `{"DRIFT.1", fromPath, 0, "shortcut edge: " + fromPath + " -> " + toPath}`
  (`src/rules/drift_no_shortcut_edge.cpp:29`, literal confirmed with
  `grep -F '"DRIFT.1", fromPath, 0, "shortcut edge: "'
  src/rules/drift_no_shortcut_edge.cpp`). `line = 0` — graph-level rule.
- **Detection logic** (`check()`, `:15-32`):
  1. Build the set of paths present in the baseline graph (`:18-20`).
  2. Iterate `graph::addedEdges(baseline_, graph)` — edges present in the current
     graph but not the baseline (`:23`, algorithm in `include/archcheck/graph/diff.h` / `src/graph/diff.cpp`,
     not re-verified here).
  3. Skip the edge unless **both** endpoints were already in the baseline
     (`:26-28`) — i.e. this rule only reports old-file-to-old-file new coupling,
     not edges touching a brand-new file.
- **Class shape**: `DriftNoShortcutEdge` takes the baseline graph by value in its
  constructor (`:13`, header `include/archcheck/rules/drift_no_shortcut_edge.h:16`)
  and stores it in `baseline_` (header `:22`). `id()` returns `"DRIFT.1"`
  (header `:19`).
- **Registration**: `makeDriftRuleSet` → `push_back(make_unique<DriftNoShortcutEdge>(baseline))`
  (`src/rules/rule_set.cpp:30`). This rule set is separate from
  `makeDefaultRuleSet` — it only runs when a graph baseline is supplied
  (see `rules/gate_policy.md` for the two code paths that consume drift
  findings: `GateMode::Drift` in `check` mode vs. `RegressionReport` in
  `--diff` mode).
- **Gate status**: **gating** — `classifyForGate` treats `"DRIFT.1"` as
  `FindingDisposition::Gating` when `mode == GateMode::Drift`
  (`src/rules/gate_policy.cpp:19`). Confirmed by
  `tests/unit/rules/gate_policy_test.cpp:31` (`CHECK(isGating("DRIFT.1", GateMode::Drift))`).
  Also listed in the README core-gate table (`README.md:45,52`).
- **Suppression**: per the header comment, update the graph baseline
  (`--save-graph-baseline`) to accept the edge (`include/archcheck/rules/drift_no_shortcut_edge.h:12`).
- **No threshold** — this is a structural (present/absent edge) check, not a
  metric compared against a number.

## For an agent

- Change detection → `src/rules/drift_no_shortcut_edge.cpp` (+ header). The edge-diff
  primitive (`graph::addedEdges`) lives in `include/archcheck/graph/diff.h` / `src/graph/diff.cpp` — touching that
  affects every drift rule (`Risk: high`, shared graph logic).
- After changing: rebuild Debug, run `tests/unit/rules/drift_no_shortcut_edge_test.cpp`
  (94 lines), exercise `fixtures/drift_shortcut_edge/{pass,fail_new_coupling}/`, then run the standard post-change checks (see [schema.md](../schema.md)).
- This rule only fires in **drift** contexts (a saved graph baseline is present);
  it is never part of `makeDefaultRuleSet` (`src/rules/rule_set.cpp:16-25`).

## Evidence

- `src/rules/drift_no_shortcut_edge.cpp:29` — the emitted `DRIFT.1` violation.
- `src/rules/drift_no_shortcut_edge.cpp:26-28` — the both-endpoints-in-baseline guard.
- `src/rules/rule_set.cpp:30` — drift-set registration.
- `src/rules/gate_policy.cpp:19` — gating classification for `GateMode::Drift`.
- `tests/unit/rules/gate_policy_test.cpp:31` — test confirming `DRIFT.1` gates in drift mode.

## Related pages

- [[../index.md]] · [[../cli-contract.md]] · [[../source-map.md]]
