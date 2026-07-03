---
title: "Lakos god-headers — header fan-in threshold"
type: rule
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "src/rules/lakos_god_headers.cpp"
  - "include/archcheck/rules/lakos_god_headers.h"
  - "src/rules/rule_set.cpp"
  - "include/archcheck/config/config.h"
  - "tests/unit/rules/lakos_god_headers_test.cpp"
  - "fixtures/lakos_god_headers/"
related_pages:
  - "../index.md"
  - "../cli-contract.md"
external_sources: []
---

# Lakos god-headers — header fan-in threshold

## Purpose

Flags a header included by too many other components — a "god header" whose fan-in
makes it a rebuild/coupling bottleneck (Lakos, *Large-Scale C++ Software Design*).

## Verified facts

- **Rule id / message**: emits
  `{"Lakos.GodHeader", <path>, 0, "fan-in N exceeds threshold T"}`
  (`lakos_god_headers.cpp:32-33`). `line = 0` (graph-level).
- **Metric**: `fanIn = graph.predecessors(id).size()`; triggers when
  `fanIn > threshold_` (`:29-31`).
- **Threshold**: `threshold_` = `config.thresholds.godHeaderFanIn`, wired at
  `rule_set.cpp:22`. **Default 50** (`include/archcheck/config/config.h:43`, marked
  single-source-of-truth); overridable via the `thresholds:` block in `.archcheck.yml`
  (`docs/config_format.md`).
- **Deliberate exclusions**: precompiled-header names (`isPchName` →
  `scan::isKnownPchBasename`, `:11`) and any names in `extraExcludes_` (`:27`).
- **Gate status**: **structural advisory** in check mode (exit `0`); in `--diff`,
  **new** god-headers gate (`README.md:52-53`).
- **Attribution**: Lakos physical design.

## For an agent

- Change detection → `src/rules/lakos_god_headers.cpp`. Change the **default** → the
  `Thresholds` struct in `include/archcheck/config/config.h` (SSOT), not the rule.
- After changing: rebuild Debug, run `unit/rules/lakos_god_headers_test.cpp`, exercise
  `fixtures/lakos_god_headers/{pass,fail_*}`, then run the standard post-change checks (see [schema.md](../schema.md)).

## Evidence

- `src/rules/lakos_god_headers.cpp:29-33` — fan-in metric + emitted violation.
- `include/archcheck/config/config.h:43` — `godHeaderFanIn = 50` default (SSOT).
- `src/rules/rule_set.cpp:22` — threshold wiring.

## Related pages

- [[../index.md]] · [[../cli-contract.md]]
