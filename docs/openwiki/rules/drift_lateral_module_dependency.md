---
title: "DRIFT.4 — lateral module dependency (graded: CYCLE / SDP / NEW)"
type: rule
status: partial
last_checked_commit: "241b07e"
source_paths:
  - "src/rules/drift_lateral_module_dependency.cpp"
  - "include/archcheck/rules/drift_lateral_module_dependency.h"
  - "src/rules/rule_set.cpp"
  - "include/archcheck/rules/area_of.h"
  - "src/rules/gate_policy.cpp"
  - "tests/unit/rules/drift_lateral_module_dependency_test.cpp"
  - "fixtures/drift_lateral/"
related_pages:
  - "../index.md"
  - "../cli-contract.md"
  - "../source-map.md"
external_sources: []
---

# DRIFT.4 — lateral module dependency (graded: CYCLE / SDP / NEW)

## Purpose

A drift rule that catches the **first** dependency between two sibling ("peer")
modules that had no prior coupling in the baseline, and grades how bad the new
coupling is using Martin's instability metric and MacCormack's shared-layer
heuristic. Only the worst grade (a module cycle) gates; the other two grades are
advisory signal about emerging architectural risk.

## Verified facts

- **`id()` vs. emitted rule ids — important distinction**: the class's
  `id()` method returns the literal `"DRIFT.4"`
  (`include/archcheck/rules/drift_lateral_module_dependency.h:29`), but
  **no violation is ever emitted with ruleId `"DRIFT.4"`**. Every emitted
  violation carries one of three graded ids, computed per-edge by the local
  `gradeEdge()` helper (`src/rules/drift_lateral_module_dependency.cpp:118-128`)
  and pushed at `:174`:
  - `"DRIFT.4.CYCLE"` — reverse edge `B -> A` already existed in the baseline
    module-pair set (`:121-122`, `basePairs.count(b + "\n" + a)`).
  - `"DRIFT.4.SDP"` — `instability(B) > instability(A) + kSdpDelta` (`:127`,
    `kSdpDelta = 0.10` at `:24`) — the new dependency goes against Martin's
    Stable Dependencies Principle gradient.
  - `"DRIFT.4.NEW"` — neither of the above: first lateral pair, no structural
    issue yet (`:127`, else branch).
- **Message literals** (`buildMsg()`, `:131-143`, confirmed with
  `grep -F` against each of the three strings below):
  - CYCLE: `"module '" + a + "' -> '" + b + "': lateral dependency creates module cycle" " ('" + b + "' already depends on '" + a + "' in baseline)" + via` (`:136-139`).
  - SDP: `"module '" + a + "' -> '" + b + "': lateral dependency violates stability" + via` (`:141`).
  - NEW: `"module '" + a + "' -> '" + b + "': first lateral dependency between peer modules" + via` (`:142`).
  - `via = " (via " + fromPath + " -> " + toPath + ")"` (`:134`).
  `line = 0` in all three (graph-level).
- **Instability metric**: Martin's `I = Ce / (Ca + Ce)` — here
  `fod / (fid + fod)` per module, `0` = stable, `1` = instable/leaf
  (`instability()`, `:63-67`; comment `:61-62`). `fid`/`fod` = count of distinct
  *other* modules a module is included-by / includes (direct edges only,
  de-duplicated per module pair, `computeMetrics()`, `:35-59`).
- **Shared-layer exclusion**: a candidate target module `B` is skipped entirely
  (never graded/reported) if `isShared(B, metrics)` — `FID(B) > 0.50 * maxFID`
  **and** `FOD(B) <= medianFOD` (`:71-95`, `kSharedFidRatio = 0.50` at `:23`).
  Comment at `:92-94` explains the strict `>` (not `>=`) guards against integer
  truncation misclassifying low-FID modules as shared in small graphs.
- **Mass-touch guard**: the whole rule is skipped (returns `{}`) when the number
  of added edges exceeds `kMassTouchEdges = 150` (`:27,157-158`) — a reorg/vendor
  drop is not lateral-dependency drift.
- **Scope of edges considered**: direct module-to-module edges only, "to avoid
  counting parallel file-edges between same modules" (`computeMetrics` comment
  `:38`) and "All counts are on DIRECT edges only (transitive would contaminate
  FID via include chains)" (`:20-22`, cites
  `docs/research/lateral_module_drift_criterion.md §2` — not independently
  re-verified here).
- **Class shape**: `DriftLateralModuleDependency(graph::DependencyGraph baseline)`
  stores `baseline_` (`:147-150`).
- **Registration**: `makeDriftRuleSet` → `push_back(make_unique<DriftLateralModuleDependency>(baseline))`
  (`src/rules/rule_set.cpp:32`).
- **Gate status**: only `"DRIFT.4.CYCLE"` gates. `classifyForGate` lists
  `"DRIFT.4.CYCLE"` explicitly in its gating set for `GateMode::Drift`
  (`src/rules/gate_policy.cpp:19`); `"DRIFT.4.SDP"` / `"DRIFT.4.NEW"` fall
  through to advisory. Confirmed by
  `tests/unit/rules/gate_policy_test.cpp:33,35,36`
  (`CHECK(isGating("DRIFT.4.CYCLE", ...))`,
  `CHECK_FALSE(isGating("DRIFT.4.NEW", ...))`,
  `CHECK_FALSE(isGating("DRIFT.4.SDP", ...))`). Also stated in the header
  comment: "Only DRIFT.4.CYCLE is gating; SDP and NEW are advisory."
  (`include/archcheck/rules/drift_lateral_module_dependency.h:19`) and in
  README (`README.md:45`).
- **Research provenance claim in the header** (not independently re-verified —
  flagging as documented, not confirmed, since it cites an external corpus run
  rather than in-repo code): "Research validation: 479 repos, TP 85%, CYCLE
  precision 92% (post-#117)." (`include/archcheck/rules/drift_lateral_module_dependency.h:21`).
- **Suppression**: `unverified`. Unlike the DRIFT.1/DRIFT.2/DRIFT.3 headers,
  which each state explicitly "update the graph baseline
  (`--save-graph-baseline`) to accept" the finding, the DRIFT.4 header
  (`include/archcheck/rules/drift_lateral_module_dependency.h:9-22`, checked in
  full) contains no such sentence. The mechanism likely still applies (all four
  drift rules take the same baseline graph in their constructor), but this is
  not stated in code/comments for DRIFT.4, so it is not asserted as fact here.

## For an agent

- Change detection / grading → `src/rules/drift_lateral_module_dependency.cpp`
  (+ header). The three thresholds (`kSharedFidRatio`, `kSdpDelta`,
  `kMassTouchEdges`) are file-local `constexpr`, not wired through
  `config::Config` — changing them means editing this `.cpp` directly, not
  `include/archcheck/config/config.h`.
- After changing: rebuild Debug, run
  `tests/unit/rules/drift_lateral_module_dependency_test.cpp` (190 lines),
  exercise
  `fixtures/drift_lateral/{fail_lateral_cycle,fail_lateral_sdp,fail_lateral_new,pass_existing_pair,pass_shared_target}/`, then run the standard post-change checks (see [schema.md](../schema.md)).
- When quoting "the DRIFT.4 rule id" to a user, be precise: the *rule class* is
  `DRIFT.4`/`DriftLateralModuleDependency`, but every *emitted finding* carries a
  graded id (`DRIFT.4.CYCLE`/`.SDP`/`.NEW`) — gate logic keys off the graded id,
  not the bare `"DRIFT.4"` string, which never appears in a `Violation`.

## Evidence

- `src/rules/drift_lateral_module_dependency.cpp:118-128` — `gradeEdge()`, the
  three-way grading logic.
- `src/rules/drift_lateral_module_dependency.cpp:131-143` — `buildMsg()`, exact
  message literals per grade.
- `src/rules/drift_lateral_module_dependency.cpp:174` — where the graded id and
  message are pushed into the result.
- `include/archcheck/rules/drift_lateral_module_dependency.h:29` — `id()` returns
  the bare (never-emitted) `"DRIFT.4"`.
- `src/rules/gate_policy.cpp:19` and
  `tests/unit/rules/gate_policy_test.cpp:33,35,36` — gate classification per
  graded id.
- `src/rules/rule_set.cpp:32` — drift-set registration.

## Related pages

- [[../index.md]] · [[../cli-contract.md]] · [[../source-map.md]]
