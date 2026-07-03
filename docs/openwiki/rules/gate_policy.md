---
title: "Gate policy — gate vs. advisory, and the two independent gate mechanisms"
type: rule
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "src/rules/gate_policy.cpp"
  - "include/archcheck/rules/gate_policy.h"
  - "tests/unit/rules/gate_policy_test.cpp"
  - "src/cli/check_command.cpp"
  - "src/cli/diff_command.cpp"
  - "src/diff/regression_report.cpp"
  - "include/archcheck/diff/regression_report.h"
  - "README.md"
related_pages:
  - "../index.md"
  - "../cli-contract.md"
  - "../source-map.md"
external_sources: []
---

# Gate policy — gate vs. advisory, and the two independent gate mechanisms

## Purpose

Explains the mechanism that decides whether a finding fails the run (exit `1`)
or is merely reported (exit `0`). This is **not one mechanism** — the codebase
has two independent gate computations that both exist today, keyed by different
data (rule ids vs. structural deltas) and invoked from different CLI code
paths. This page documents both, and where each one is used.

## Verified facts

### Mechanism 1 — `gate_policy.cpp`: `classifyForGate` / `isGating` / `countGating`

Operates on a `ViolationList` (i.e. rule-id-tagged findings) and a `GateMode`
enum with two values, `Check` and `Drift` (`include/archcheck/rules/gate_policy.h:11-15`).

- **`classifyForGate(ruleId, mode, failOnUnresolvedLocal)`**
  (`src/rules/gate_policy.cpp:8-21`):
  - `mode == GateMode::Check` (`:10-17`):
    - `"SF.9"` or `"CASE_MISMATCH_INCLUDE"` → `Gating` (`:12-13`).
    - `"UNRESOLVED_LOCAL_INCLUDE"` → `Gating` only if `failOnUnresolvedLocal`
      is true, else `Advisory` (`:14-15`).
    - anything else → `Advisory` (`:16`).
  - `mode == GateMode::Drift` (`:19-20`, the function's final `return`, reached
    when `mode != GateMode::Check`): `"DRIFT.1"`, `"DRIFT.2"`, or
    `"DRIFT.4.CYCLE"` → `Gating`; anything else (including `"DRIFT.3"`,
    `"DRIFT.4.SDP"`, `"DRIFT.4.NEW"`) → `Advisory`.
  - `failOnUnresolvedLocal` defaults to `false` (header default arg,
    `include/archcheck/rules/gate_policy.h:27-28`) so existing call sites that
    omit it keep prior behavior (comment `:23-25`).
- **`isGating`** (`:23-26`) — boolean wrapper over `classifyForGate`.
- **`countGating`** (`:28-32`) — counts violations in a `ViolationList` where
  `isGating` is true, via `std::count_if`.
- Every branch above is exercised in
  `tests/unit/rules/gate_policy_test.cpp:13-36` (e.g. `:13` SF.9 gates in
  Check, `:16` `Lakos.GodHeader` is advisory in Check, `:31-33` DRIFT.1/2/4.CYCLE
  gate in Drift, `:34-36` DRIFT.3/4.NEW/4.SDP do not gate in Drift).
- **Where `GateMode::Check` is used**: the plain `check` command with no graph
  baseline. `src/cli/check_command.cpp:97-107` (`reportCheckGate`) calls
  `countGating(all, GateMode::Check, failOnUnresolvedLocal)` (`:99`); the
  function's own comment states the rationale — Lakos chain-length/god-header
  and SF.7/SF.8 "flood header-heavy libraries on a naive first run (abseil:
  211 chain-length, exit 1)" so they stay advisory, and only SF.9 (cycles) and
  CASE_MISMATCH_INCLUDE gate in plain check mode (`:90-96`).
- **Where `GateMode::Drift` is used**: when a graph baseline file is supplied
  to the `check` command. `src/cli/check_command.cpp:109-112`
  (`gateModeFor`) selects `GateMode::Drift` when `baseline.driftFile` is set,
  else `GateMode::Check`; `reportDriftGate` (`:76-88`) then calls
  `countGating(all, GateMode::Drift)` (`:82`) and prints
  `"drift gate: " << gating << " gating regression(s) (DRIFT.1/DRIFT.2/DRIFT.4.CYCLE); pre-existing and advisory findings (DRIFT.3/DRIFT.4.SDP/DRIFT.4.NEW) are advisory"`
  (`:84-86`, literal confirmed by reading the source). This is the code path
  the four `DRIFT.*` rule pages (`drift_no_shortcut_edge.md`,
  `drift_bidirectional_coupling.md`, `drift_lateral_module_dependency.md`,
  `drift_no_cycle_growth.md`) describe as "drift mode" / "graph baseline
  mode" — it is triggered by a **saved graph baseline file**, not by
  `--diff <revspec>`.

### Mechanism 2 — `RegressionReport::gates()`: the `--diff <revspec>` PR workflow

A **structurally separate** gate computation that does **not** call
`classifyForGate`/`isGating`/`countGating` and does not operate on rule ids at
all — it operates on typed struct fields.

- `RegressionReport::gates()` (`include/archcheck/diff/regression_report.h:80`):
  `bool gates() const { return !grownCycles.empty() || !newGodHeaders.empty(); }`
  (literal confirmed by reading the header). The comment directly above (`:77-79`)
  states: "Gated regressions — the only findings that fail the run (exit 1).
  Mirrors check-mode rule violations: new/grown cycles (SF.9) and new
  god-headers (Lakos.GodHeader). Everything else is advisory."
- `grownCycles` is populated by `collectEdgesAndCycles()` calling
  `graph::grownSccs(baseline, current)`
  (`src/diff/regression_report.cpp:127-146`, the SCC-growth call at `:136`) —
  the same underlying graph primitive DRIFT.2 (`DriftNoCycleGrowth`) uses, but
  invoked independently, not through the `DriftNoCycleGrowth` rule class.
- `newGodHeaders` is populated by `detectNewGodHeaders(baseline, current, fanInThreshold)`
  (`:148-166`): computes fan-in on both graphs via `graph::computeFanIn`
  (`:151-152`), collects baseline nodes already over `fanInThreshold` into
  `baselineGodSet` (`:153-156`), then reports current-graph nodes over the
  threshold whose path is **not** in `baselineGodSet` (`:158-164`) — i.e. only
  *newly crossing* the threshold, not pre-existing god-headers. Threshold
  comes from `MetricThresholds::godHeaderFanIn`, default `50`
  (`include/archcheck/diff/regression_report.h:50-55`; the comment there notes
  it "Must match `config::Thresholds::godHeaderFanIn` (`config.h`) — `check`
  and `--diff` share one contract; equality is pinned by a test").
- **Call sites** — all three fail the whole `--diff` invocation on
  `report.gates()`, never on a rule-id count:
  - `emitJsonDiff` (`src/cli/diff_command.cpp:347-355`): `return report.gates() ? 1 : 0;` (`:354`).
  - `emitMarkdownDiff` (`:357-363`): `return report.gates() ? 1 : 0;` (`:362`).
  - the bulk/aggregate diff path: `return graph.report.gates() ? 1 : 0;` (`:478`).
- **Text-report gate line**: `writeTextReport` in
  `src/diff/regression_report.cpp:254-268` prints
  `"\ngate: " << (r.gates() ? "fail" : "ok") << " (gating: cycles, god-headers)\n"`
  (`:267`, literal confirmed by reading the source) and labels the two
  contributing sections `"\ngrown_cycles (gating):\n"` (`:206`) and
  `"\nnew_god_headers (gating):\n"` (`:242`) — everything else in that report
  (`added`/`removed` edges, `new_cross_area_dependencies`, `chain_length_grown`,
  `nccd_grown`) is explicitly labelled `"(advisory)"` in its own writer
  function (`:188,197,220,235,251`).

### Why the two mechanisms disagree on god-headers

`GateMode::Drift` in Mechanism 1 does **not** gate on any god-header signal —
`classifyForGate`'s Drift branch only checks `"DRIFT.1"`, `"DRIFT.2"`,
`"DRIFT.4.CYCLE"` (`src/rules/gate_policy.cpp:19`); there is no
`Lakos.GodHeader`-equivalent rule id in `makeDriftRuleSet`
(`src/rules/rule_set.cpp:27-35` registers only the four `Drift*` rule classes,
no god-header rule). Mechanism 2, by contrast, gates on `newGodHeaders`
directly. `check_command.cpp:96` makes this explicit in its own comment: "This
mirrors the narrow-gate trust model, but not the exact `--diff` gate: diff also
gates new god-headers. #133." So: **a graph-baseline `check --graph-baseline`
run and a `--diff <revspec>` run are not the same gate**, even though both are
informally called "drift" — only `--diff` escalates new god-headers to gating.

### README cross-check

- `README.md:45`: "Tracks architectural drift between graph baselines:
  `DRIFT.1`, `DRIFT.2`, and `DRIFT.4.CYCLE` gate; `DRIFT.3`, `DRIFT.4.NEW`,
  `DRIFT.4.SDP`, and pre-existing findings are advisory" — describes
  Mechanism 1 (`GateMode::Drift`).
- `README.md:46`: "Runs the canonical PR workflow with `--diff <revspec>`:
  new/grown cycles and new god-headers gate, while added edges, chain/NCCD
  growth, SATD, test co-evolution, local complexity, flag-argument drift, and
  new clone drift are advisory" — describes Mechanism 2
  (`RegressionReport::gates()`).
- `README.md:52` (gate table row "Core gate"): "SF.9 cycles,
  CASE_MISMATCH_INCLUDE, DRIFT.1/2/4.CYCLE, `--diff` new/grown cycles and new
  god-headers | exit `1`" — lists both mechanisms' gating conditions in one
  row, which is why they must not be conflated into a single rule set when
  read literally.

## For an agent

- Changing what gates in **plain `check`** (no baseline) or in
  **`check --graph-baseline`** → edit `classifyForGate` in
  `src/rules/gate_policy.cpp` and update
  `tests/unit/rules/gate_policy_test.cpp` in lockstep.
- Changing what gates in **`--diff <revspec>`** → edit `RegressionReport::gates()`
  in `include/archcheck/diff/regression_report.h:80` (and/or the fields feeding
  it, `newGodHeaders`/`grownCycles`, populated in
  `src/diff/regression_report.cpp`). This is a **different file** from the one
  above — a change to one does not affect the other, and a PR that "adds a
  gating rule" needs to state explicitly which of the two commands it targets.
- Both mechanisms must stay conceptually aligned with the README gate table
  (`README.md:45-46,52-53`) — update the README in the same change if the gate
  set moves.
- No fixtures directory maps 1:1 to `gate_policy.cpp` per
  `docs/openwiki/source-map.md` (only a unit test); this page's task map
  correctly lists no `fixtures/` entry for gate policy itself — gate behavior
  is instead exercised end-to-end through each individual rule's fixtures
  (`fixtures/sf9_no_cycles/`, `fixtures/drift_*` etc.) plus
  `tests/unit/rules/gate_policy_test.cpp` for the classification table itself.

## Evidence

- `src/rules/gate_policy.cpp:8-21` — `classifyForGate`, the rule-id-based gate table.
- `include/archcheck/rules/gate_policy.h:11-21` — `GateMode` / `FindingDisposition` enums.
- `tests/unit/rules/gate_policy_test.cpp:13-36` — full classification coverage.
- `src/cli/check_command.cpp:76-112` — `reportDriftGate`, `reportCheckGate`, `gateModeFor`.
- `include/archcheck/diff/regression_report.h:77-80` — `RegressionReport::gates()`.
- `src/diff/regression_report.cpp:148-166` — `detectNewGodHeaders`.
- `src/cli/diff_command.cpp:354,362,478` — the three `--diff` exit-code call sites.
- `README.md:45,46,52` — the two mechanisms as documented publicly.

## Related pages

- [[../index.md]] · [[../cli-contract.md]] · [[../source-map.md]]
