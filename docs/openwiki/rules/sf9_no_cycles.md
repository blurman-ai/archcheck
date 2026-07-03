---
title: "SF.9 — no include cycles"
type: rule
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "src/rules/sf9_no_cycles.cpp"
  - "include/archcheck/rules/sf9_no_cycles.h"
  - "src/rules/rule_set.cpp"
  - "src/graph/algorithms.cpp"
  - "tests/unit/rules/sf9_no_cycles_test.cpp"
  - "fixtures/sf9_no_cycles/"
related_pages:
  - "../index.md"
  - "../cli-contract.md"
  - "../source-map.md"
external_sources: []
---

# SF.9 — no include cycles

## Purpose

Detects cyclic `#include` dependencies between components (Core Guidelines SF.9:
"Don't declare / include cyclically").

## Verified facts

- **Rule id / message**: emits `{"SF.9", <path>, 0, "cycle: A → B → … → A"}`
  (`sf9_no_cycles.cpp:139`, message built at `:102-115`). `line = 0` — it is a
  graph-level rule (`Violation.line` comment, `include/archcheck/rules/violation.h:13`).
- **Algorithm**: SCCs of the include graph (`graph::computeScc`,
  `src/graph/algorithms.cpp`); an SCC of size ≥ 2 is a cycle (`sf9_no_cycles.cpp:123-140`).
- **Deliberate exclusions** (not cycles):
  - inline-split 2-node SCC `foo.h ↔ foo.inl` (same dir + stem, one side an inline
    impl) — `isInlineSplitScc`, `:91-100` (#088);
  - cycles where **every** edge is conditional (`#if`-guarded) — `allEdgesConditional`, `:46-58`.
- **Registration**: `makeDefaultRuleSet` → `push_back(make_unique<Sf9NoCycles>())`
  (`rule_set.cpp:19`). No threshold.
- **Gate status**: **core gate** — exit `1` in check mode; in `--diff`, new/grown
  cycles gate (README gate table, `README.md:52`).
- **Attribution**: C++ Core Guidelines SF.9.

## For an agent

- Change detection → `src/rules/sf9_no_cycles.cpp` (+ header). Cycle enumeration
  itself is in `src/graph/algorithms.cpp` (`computeScc`) — touching that is
  `Risk: high` (graph logic).
- After changing: rebuild Debug, run `unit/rules/sf9_no_cycles_test.cpp`, exercise
  `fixtures/sf9_no_cycles/{pass,fail_*}`, then run the standard post-change checks (see [schema.md](../schema.md)).

## Evidence

- `src/rules/sf9_no_cycles.cpp:139` — the emitted `SF.9` violation.
- `src/rules/sf9_no_cycles.cpp:91-100,46-58` — the two exclusion carve-outs.
- `src/rules/rule_set.cpp:19` — default-set registration.
- `README.md:52` — core-gate classification.

## Related pages

- [[../index.md]] · [[../cli-contract.md]] · [[../source-map.md]]
