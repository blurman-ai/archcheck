---
title: "archcheck — one-page agent map"
type: index
status: verified
last_checked_commit: "850911f"
source_paths:
  - "src/rules/gate_policy.cpp"
  - "src/rules/rule_set.cpp"
  - "include/archcheck/config/config.h"
  - "src/rules/"
  - "src/scan/"
  - "tests/CMakeLists.txt"
  - "fixtures/"
related_pages: []
external_sources: []
---

# archcheck — one-page agent map

## Purpose

Everything an agent usually looks up — rule ids, exact messages, thresholds, gate
status, and which files/tests/fixtures implement what — in **one read**. This page
is **derived**: when it disagrees with code, tests, or CHANGELOG, the code wins and
this page gets fixed (same PR). Health: `python3 scripts/check_openwiki.py`
(`/openwiki-check`) — front matter, source-path existence, and staleness vs
`last_checked_commit`.

## Routing (what this page does NOT restate)

| Topic | Authority |
|---|---|
| What shipped (rules, thresholds, modes) | [CHANGELOG.md](../../CHANGELOG.md) |
| Project vocabulary | [docs/GLOSSARY.md](../GLOSSARY.md) |
| CLI usage / flags / examples | [README.md](../../README.md) §Usage + `--help` |
| Duplication subsystem | [docs/duplication_architecture.md](../duplication_architecture.md) (SSOT) |
| Config format | [docs/config_format.md](../config_format.md) |
| CI wiring | [docs/ci_usage.md](../ci_usage.md) · [docs/ci_integration.md](../ci_integration.md) |
| Design rationale / roadmap | [docs/architecture-spec.md](../architecture-spec.md) · [docs/decisions/](../decisions/) |

## Exit codes (contract)

`0` OK · `1` gated violation · `2` config/parsing error · `3` internal error.
Findings print as `file:line: [rule] message` (README.md:44).

## Gate policy — who fails the build

From `src/rules/gate_policy.cpp:10-20`; everything not listed is advisory
(reported, exit `0`):

| Mode | Gating (exit `1`) |
|---|---|
| plain `check` | `SF.9`, `CASE_MISMATCH_INCLUDE`; `UNRESOLVED_LOCAL_INCLUDE` only with `--fail-on-unresolved-local` (README.md:43) |
| drift (graph baseline) | `DRIFT.1`, `DRIFT.2`, `DRIFT.4.CYCLE` |
| `--diff <revspec>` | grown cycles + new god-headers |

⚠ **Two independent gate mechanisms, by design (resolved #171).** `classifyForGate`
(`gate_policy.cpp`) drives `check`/drift modes over rule-id strings; `--diff` gates via
a structurally different path (`RegressionReport::gates()`,
`src/diff/regression_report.cpp` → `src/cli/diff_command.cpp`) over typed graph-diff
fields — merging them would mean threading rule-id strings through a struct designed
around typed fields, for no gain over the current setup. #171 kept the two paths but
pinned the contract with a cross-referenced test pair (grep `#171` in
`tests/unit/rules/gate_policy_test.cpp` and `tests/unit/diff/regression_report_test.cpp`):
Check/Drift never gate `Lakos.GodHeader`, `--diff` gates new god-headers — the one
intentional divergence (`src/cli/check_command.cpp:96`, #133). Cycles gate on both
sides. Don't change one path's gate set without updating its paired test.

## Rule / check registry

Default & drift rules register in `src/rules/rule_set.cpp`
(`makeDefaultRuleSet` / `makeDriftRuleSet`): one rule = one class = one file; adding
a rule = a new `.h`/`.cpp` pair + one `push_back` line — existing rule files stay
untouched. Scan signals run inside the `--diff` / `--history` pipelines. Threshold
SSOT: `include/archcheck/config/config.h` (`Thresholds`). Message templates quoted
verbatim from the emitting code (`<N>`/`<T>` = runtime values); rule ids inventoried
by grep over `src/` at `eca9a96`.

| Rule id | Flags | Message (template) | Threshold | Impl (`src/`) | Test (`tests/`) | Fixture |
|---|---|---|---|---|---|---|
| `SF.9` | include cycle (SCC ≥ 2; skips `foo.h↔foo.inl` split #088 and all-conditional cycles) | `cycle: A → B → … → A` | — | `rules/sf9_no_cycles.cpp` | `unit/rules/sf9_no_cycles_test.cpp` | `sf9_no_cycles/` |
| `SF.7` | `using namespace` at global scope in a header | `'using namespace' at global scope in header (SF.7)` | — | `rules/sf7_using_namespace.cpp` | `unit/rules/sf7_using_namespace_test.cpp` | `sf7_using_namespace/` |
| `SF.8` | header lacks guard/`#pragma once` (real `#ifndef`+`#define` pair; first 60 code lines; skips `.inc`, ObjC) | `header missing #pragma once or include guard (SF.8)` | — | `rules/sf8_include_guard.cpp` | `unit/rules/sf8_include_guard_test.cpp` | `sf8_include_guard/` |
| `Lakos.GodHeader` | header fan-in > threshold (PCH names excluded) | `fan-in <N> exceeds threshold <T>` | `godHeaderFanIn = 50` (`include/archcheck/config/config.h:43`) | `rules/lakos_god_headers.cpp` | `unit/rules/lakos_god_headers_test.cpp` | `lakos_god_headers/` |
| `Lakos.ChainLength` | include chain depth > threshold | `include chain depth <N> exceeds threshold <T>` | `chainLength = 10` (`include/archcheck/config/config.h:42`) | `rules/lakos_chain_length.cpp` | `unit/rules/lakos_chain_length_test.cpp` | `lakos_chain_length/` |
| `CASE_MISMATCH_INCLUDE` | local include resolves only case-insensitively (build break on Linux) | see impl | — | `scan/local_include_scan.cpp` (id emitted in `cli/check_command.cpp` `toViolation`) | `integration/scan/local_include_gate_fixtures_test.cpp` | `local_include_gate/` |
| `UNRESOLVED_LOCAL_INCLUDE` | quoted local include resolves nowhere | see impl | — | `scan/local_include_scan.cpp` (id emitted in `cli/check_command.cpp` `toViolation`) | (same) | `local_include_gate/` |
| `DRIFT.1` | new shortcut edge vs graph baseline | see impl | — | `rules/drift_no_shortcut_edge.cpp` | `unit/rules/drift_no_shortcut_edge_test.cpp` | `drift_shortcut_edge/` |
| `DRIFT.2` | cycle growth vs baseline | see impl | — | `rules/drift_no_cycle_growth.cpp` | `unit/rules/drift_no_cycle_growth_test.cpp` | `drift_cycle_growth/` |
| `DRIFT.3` | new bidirectional module coupling | `bidirectional module coupling: '…'` | — | `rules/drift_bidirectional_coupling.cpp` | `integration/rules/drift_fixtures_test.cpp` | `drift_bidirectional/` |
| `DRIFT.4.CYCLE`/`.SDP`/`.NEW` | lateral module dependency, graded (bare `DRIFT.4` never emitted) | see impl | — | `rules/drift_lateral_module_dependency.cpp` | `unit/rules/drift_lateral_module_dependency_test.cpp` | `drift_lateral/` |
| `DRIFT.NEW_CLONE` | PR introduces a code clone (`--diff`) | see impl | skip-guard `diffMaxCloneScanBytes = 40 MB` (`include/archcheck/config/config.h:55`) | `scan/new_clone_drift.cpp` | `unit/scan/new_clone_drift_test.cpp`, `integration/diff/new_clone_fixtures_test.cpp` | `diff_new_clone/` |
| `DRIFT.LOCAL_COMPLEXITY` | PR raises local complexity (`--diff`) | see impl | skip-guard `diffMaxAddedLines = 10000` (`include/archcheck/config/config.h:47`) | `scan/local_complexity_drift.cpp` (+`_metrics`) | `unit/scan/local_complexity_{metrics,drift}_test.cpp`, `integration/scan/local_complexity_fixtures_test.cpp` | `local_complexity_drift/` |
| `DRIFT.BOOL_FIELD_ACCRETION` | bool field added to a struct — neutral accretion signal, not a verdict | see impl | — | `scan/bool_field_drift.cpp` | `unit/scan/bool_field_drift_test.cpp`, `integration/scan/bool_field_drift_fixtures_test.cpp` | `bool_field_drift/` |
| `SATD.1` / `SATD.2` | new self-admitted-debt marker in the diff | truncated marker line | — | `scan/satd_scan.cpp` | `unit/scan/satd_scan_test.cpp` | `satd_delta/` |
| `TEST.1.prod_changed_tests_silent` | prod changed, tests silent | `prod +<A>/-<R> across <N> file(s), tests +<A>/-<R>` | — | `scan/test_co_evolution.cpp` | `unit/scan/test_co_evolution_test.cpp` | `test_co_evolution/` |
| `ARG.1.flag_argument_signature` / `ARG.2.boolean_literal_call` | flag-argument drift | see impl | — | `scan/flag_argument_scan.cpp` | `flag_argument_scan_test.cpp` | — |
| `SIZE.1.god_file_growth` / `HIST.1.defect_attractor` | `--history` analytics (report-only; ordering bug → task #170) | see impl | — | `scan/god_file_growth.cpp`, `scan/defect_attractor.cpp` | `unit/scan/{god_file_growth,defect_attractor}_test.cpp` | — |

`--duplication` (token clone report) is its own output format, not a `ruleId` —
see `docs/duplication_architecture.md` (SSOT); impl `src/scan/duplication/`,
tests `tests/duplication_*_test.cpp`, fixtures `duplication/`.

## Subsystem matrix

Headers: `include/archcheck/<subsystem>/`; impl: `src/<subsystem>/`; tests:
`tests/unit/<subsystem>/` + `tests/integration/<subsystem>/`. Build targets:
`archcheck_core` (static lib, `src/CMakeLists.txt:2`), `archcheck` (binary, entry
`src/main.cpp`), `archcheck_tests` (Catch2 v3, gets `ARCHCHECK_FIXTURES_DIR`).

| Subsystem | Impl highlights | Tests | Fixtures | Owns |
|---|---|---|---|---|
| **cli** | `check_command`, `diff_command`, `preview_commands`, `main.cpp` | `integration/cli/cli_smoke_e2e_test.cpp` | — | arg parsing, mode dispatch, exit codes |
| **config** | `config_loader.cpp` | `unit/config/test_loader.cpp` | `config/` | `.archcheck.yml` → `Config`; enforcement v0.2 (ADR-001) |
| **graph** | `dependency_graph`, `algorithms` (SCC, depths — `Risk: high`), `graph_builder`, `baseline`, `diff` | `unit/graph/*`, `integration/graph/*` | `graph/` | DAG, cycles, levelization, CCD/ACD/NCCD |
| **rules** | `rules/*.cpp` + factory `rule_set.cpp` + `gate_policy.cpp` | `unit/rules/*`, `integration/rules/*` | per rule (table above) | rule classes, gate vs advisory |
| **scan** | `include_scanner`, `include_resolver`, `local_include_scan`, `project_files` + signal scanners (table above) | `unit/scan/*`, `integration/scan/*` | `scan/` + per signal | preprocessor scan (no `compile_commands.json`) |
| **scan/duplication** | `duplication_scanner`, `fragmenter`, `token_normalizer`, `similarity`, `clone_index`, `clone_classifier` | `duplication_*_test.cpp` (flat) | `duplication/` | token clone detector |
| **git** | `git_exec`, `git_state`, `diff_query`, `history_query` (newest-first — see #170) | `unit/git/*` | — | fork/exec git, blobs, revspecs |
| **diff** | `regression_report` (incl. `--diff` gates), `diff_json_report`, `md_report` | `unit/diff/*`, `integration/diff/*` | `drift_real_world/` | `--diff` reporting |
| **report** | `text_reporter`, `json_reporter`, `violation_baseline` | `unit/report/*` | — | output format, `--baseline` |

## After any change

Rebuild Debug (`cmake --build build/debug`) → `./build/debug/tests/archcheck_tests`
(+ the specific test/fixture from the tables) → `lizard --CCN 15 --length 30
--arguments 5 --warnings_only src/ include/ tests/` → dogfood
`./build/debug/src/archcheck src/ include/ tests/` must report 0. Changed a fact
this page states → update the row and bump `last_checked_commit`.
