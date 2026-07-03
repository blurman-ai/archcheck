---
title: "Lakos chain length — include depth threshold"
type: rule
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "src/rules/lakos_chain_length.cpp"
  - "include/archcheck/rules/lakos_chain_length.h"
  - "src/rules/rule_set.cpp"
  - "src/graph/algorithms.cpp"
  - "include/archcheck/config/config.h"
  - "tests/unit/rules/lakos_chain_length_test.cpp"
  - "fixtures/lakos_chain_length/"
related_pages:
  - "../index.md"
  - "../cli-contract.md"
external_sources: []
---

# Lakos chain length — include depth threshold

## Purpose

Flags a component whose transitive `#include` depth is too deep — long include
chains lengthen builds and couple layers (Lakos physical design).

## Verified facts

- **Rule id / message**: emits
  `{"Lakos.ChainLength", <path>, 0, "include chain depth N exceeds threshold T"}`
  (`lakos_chain_length.cpp:20-22`). `line = 0` (graph-level).
- **Metric**: per-node include depth from `graph::computeIncludeDepths`
  (`src/graph/algorithms.cpp`); triggers when `depths[i] > threshold_`
  (`lakos_chain_length.cpp:14-18`).
- **Threshold**: `threshold_` = `config.thresholds.chainLength`, wired at
  `rule_set.cpp:23`. **Default 10** (`include/archcheck/config/config.h:42`, SSOT);
  overridable via the `thresholds:` block in `.archcheck.yml`.
- **Gate status**: **structural advisory** in check mode (exit `0`); in `--diff`,
  chain growth is advisory (`README.md:53`).
- **Attribution**: Lakos physical design.

## For an agent

- Change detection → `src/rules/lakos_chain_length.cpp`. The depth computation lives
  in `src/graph/algorithms.cpp` (`computeIncludeDepths`) — changing it is `Risk: high`.
- Change the **default** → `Thresholds` in `include/archcheck/config/config.h` (SSOT).
- After changing: rebuild Debug, run `unit/rules/lakos_chain_length_test.cpp`, exercise
  `fixtures/lakos_chain_length/{pass,fail_*}`, then run the standard post-change checks (see [schema.md](../schema.md)).

## Evidence

- `src/rules/lakos_chain_length.cpp:14-22` — depth metric + emitted violation.
- `include/archcheck/config/config.h:42` — `chainLength = 10` default (SSOT).
- `src/rules/rule_set.cpp:23` — threshold wiring.

## Related pages

- [[../index.md]] · [[../cli-contract.md]]
