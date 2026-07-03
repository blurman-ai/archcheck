---
title: "DRIFT.3 — bidirectional module coupling"
type: rule
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "src/rules/drift_bidirectional_coupling.cpp"
  - "include/archcheck/rules/drift_bidirectional_coupling.h"
  - "src/rules/rule_set.cpp"
  - "include/archcheck/rules/area_of.h"
  - "src/rules/gate_policy.cpp"
  - "tests/integration/rules/drift_fixtures_test.cpp"
  - "fixtures/drift_bidirectional/"
related_pages:
  - "../index.md"
  - "../cli-contract.md"
  - "../source-map.md"
external_sources: []
---

# DRIFT.3 — bidirectional module coupling

## Purpose

A drift rule at the **module (area)** level, not the file level. It fires when,
after a diff, two modules `A` and `B` end up depending on each other in both
directions — a mutual coupling that did **not** exist in the baseline. Because
the two directions can go through *different* files, no single-file include cycle
exists (that is SF.9 / DRIFT.2 territory), yet the two modules can no longer be
built or tested in isolation — Martin's Acyclic Dependencies Principle violated
at the aggregate level.

## Verified facts

- **Rule id / message**: emits
  `{"DRIFT.3", <path of e.from>, 0, "bidirectional module coupling: '" + a + "' <-> '" + b + "' (modules now depend on each other; was not mutual in baseline)"}`
  (`src/rules/drift_bidirectional_coupling.cpp:67-69`, literal confirmed with
  `grep -F "bidirectional module coupling" src/rules/drift_bidirectional_coupling.cpp`).
  `line = 0` — graph-level rule.
- **"Area" definition**: computed by the shared `areaOf()` helper
  (`include/archcheck/rules/area_of.h:62-84`, used at `:26,31,54,55` of this
  rule's `.cpp`) — first path component after stripping wrapper dirs
  `src, source, sources, include, includes, inc, lib, libs`
  (`include/archcheck/rules/area_of.h:20-21`) and skipping noise segments
  (build/vendor/test/generated dirs, `area_of.h:39-56`); returns `""` for
  root-level or noise-only paths, which this rule then treats as "no area"
  (`:27-28,56`).
- **Detection logic** (`check()`, `:45-72`):
  1. Compute the set of cross-area dependency pairs `"A\nB"` present in the
     baseline (`baseDeps`) and in the current graph (`curDeps`) via the local
     `areaDeps()` helper (`:20-37`).
  2. For each added edge (`graph::addedEdges(baseline_, graph)`, `:52`) whose
     endpoints are in different, non-empty areas (`:54-57`):
     - skip if `B` does not currently depend back on `A` (`:58-59`, "not mutual now");
     - skip if `A<->B` was **already** mutual in the baseline (`:60-61`, not new drift);
     - skip if there is a **direct** two-file cycle `e.to -> e.from` (`:62-63`,
       explicitly deferred to DRIFT.2/SF.9 so the two rules never double-report
       the same edge);
     - de-duplicate by an order-independent module-pair key so each `A<->B`
       pair is reported once (`:64-66`).
- **Class shape**: `DriftBidirectionalCoupling(graph::DependencyGraph baseline)`
  stores `baseline_` (`:41-43`, header `:26,32`). `id()` returns `"DRIFT.3"`
  (header `:29`).
- **Registration**: `makeDriftRuleSet` → `push_back(make_unique<DriftBidirectionalCoupling>(baseline))`
  (`src/rules/rule_set.cpp:31`).
- **Gate status**: **advisory**, in both drift-related gate mechanisms. In
  `check`-mode drift (`GateMode::Drift`), `classifyForGate` gates only
  `"DRIFT.1"`, `"DRIFT.2"`, `"DRIFT.4.CYCLE"` (`src/rules/gate_policy.cpp:19`) —
  `"DRIFT.3"` is not in that list, so it falls through to
  `FindingDisposition::Advisory`. Confirmed by
  `tests/unit/rules/gate_policy_test.cpp:34` (`CHECK_FALSE(isGating("DRIFT.3", GateMode::Drift))`).
  Also stated in README: `` `DRIFT.3`, `DRIFT.4.NEW`, `DRIFT.4.SDP`, and
  pre-existing findings are advisory `` (`README.md:45`).
- **Suppression**: update the graph baseline (`--save-graph-baseline`) to accept
  the coupling (header comment, `include/archcheck/rules/drift_bidirectional_coupling.h:22`).
- **No threshold** — structural (mutual-dependency) check, not a metric compared
  against a number.

## For an agent

- Change detection → `src/rules/drift_bidirectional_coupling.cpp` (+ header). The
  area classifier is shared with other rules via `src/rules/area_of.h`
  (`tests/unit/rules/area_of_test.cpp` per `docs/openwiki/source-map.md`) —
  changing it changes every module-level drift rule.
- After changing: rebuild Debug, run
  `tests/integration/rules/drift_fixtures_test.cpp` (this rule has no dedicated
  unit test file — it is exercised only through the fixtures integration test,
  per the task map), exercise
  `fixtures/drift_bidirectional/{fail_new_coupling,pass_one_directional,pass_file_cycle}/`, then run the standard post-change checks (see [schema.md](../schema.md)).
- Note the deliberate non-overlap with DRIFT.2/SF.9 (`:62-63`) — a change here
  that removes the direct-two-file-cycle guard would start double-reporting the
  same regression under two rule ids.

## Evidence

- `src/rules/drift_bidirectional_coupling.cpp:67-69` — the emitted `DRIFT.3` violation.
- `src/rules/drift_bidirectional_coupling.cpp:58-63` — the three skip conditions
  (not mutual now / already mutual in baseline / direct two-file cycle).
- `src/rules/rule_set.cpp:31` — drift-set registration.
- `src/rules/gate_policy.cpp:19` and `tests/unit/rules/gate_policy_test.cpp:34` —
  advisory classification.
- `README.md:45` — README's advisory listing for `DRIFT.3`.

## Related pages

- [[../index.md]] · [[../cli-contract.md]] · [[../source-map.md]]
