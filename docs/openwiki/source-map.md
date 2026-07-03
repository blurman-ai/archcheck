---
title: "Source map — subsystem → code / tests / fixtures"
type: source-map
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "src/"
  - "include/archcheck/"
  - "tests/"
  - "fixtures/"
  - "src/CMakeLists.txt"
  - "tests/CMakeLists.txt"
related_pages:
  - "index.md"
external_sources: []
---

# Source map

## Purpose

One place that answers "I want to change area X — which files, tests, and fixtures
do I read first?" Every row below is a real path at commit `241b07e`. This page is
**derived** — if it disagrees with the tree, the tree wins.

## Build targets

- `archcheck_core` — static library, all subsystems (`src/CMakeLists.txt:2`).
- `archcheck` — the CLI binary; entry point `src/main.cpp` (`src/CMakeLists.txt:78`).
- `archcheck_tests` — Catch2 v3 test binary; links `archcheck::core`, gets
  `ARCHCHECK_FIXTURES_DIR` and `ARCHCHECK_BINARY_PATH` (`tests/CMakeLists.txt`).

## Subsystem matrix

Headers live under `include/archcheck/<subsystem>/`, implementations under
`src/<subsystem>/`. Tests are under `tests/unit/<subsystem>/` (unit) and
`tests/integration/<subsystem>/` (fixture / end-to-end).

| Subsystem | `src/` implementation | Tests | Fixtures | Responsibility (verify in code) |
|---|---|---|---|---|
| **cli** | `cli/check_command`, `cli/diff_command`, `cli/preview_commands` (+ `src/main.cpp`) | `integration/cli/cli_smoke_e2e_test.cpp` | — | Arg parsing, mode dispatch (check / diff / preview), exit codes |
| **config** | `config/config_loader.cpp` (hdr `config/config.h`, `config/config_loader.h`) | `unit/config/test_loader.cpp` | `config/` | `.archcheck.yml` → `Config`; parsed + validated (enforcement v0.2 — ADR `docs/decisions/001-*`) |
| **graph** | `graph/{dependency_graph,algorithms,graph_builder,baseline,diff}.cpp` | `unit/graph/*`, `integration/graph/{end_to_end,vendor_exclude}_test.cpp` | `graph/` | Include DAG, cycles/SCC, levelization, CCD/ACD/NCCD, graph baseline + diff |
| **rules** | `rules/*.cpp` (see rule table below) + factory `rules/rule_set.cpp` | `unit/rules/*`, `integration/rules/drift_fixtures_test.cpp` | per rule (see table) | One rule = one class = one file; gate vs advisory via `rules/gate_policy.cpp` |
| **scan** | `scan/{include_scanner,include_resolver,local_include_scan,project_files,disk_file_source}.cpp` + signal scanners | `unit/scan/*`, `integration/scan/*` | `scan/`, `local_include_gate/`, and per-signal (below) | Fast preprocessor scan (no `compile_commands.json`), include resolution |
| **scan** (PR-hygiene signals) | `scan/{satd_scan,test_co_evolution,new_clone_drift,local_complexity_metrics,local_complexity_drift,bool_field_drift,flag_argument_scan,god_file_growth,defect_attractor,function_body_scan}.cpp` | `unit/scan/*`, `integration/scan/{local_complexity,bool_field_drift}_fixtures_test.cpp`, `flag_argument_scan_test.cpp` | `satd_delta/`, `test_co_evolution/`, `local_complexity_drift/`, `bool_field_drift/`, `diff_new_clone/` | Advisory drift/hygiene signals (SATD, co-evolution, complexity, flag-args, bool accretion, god-file growth, defect attractor, new-clone drift) |
| **scan/duplication** | `scan/duplication/{duplication_scanner,fragmenter,token_normalizer,similarity,clone_index,clone_classifier,fp_corpus_eval}.cpp` | `duplication_*_test.cpp` (17 flat tests) | `duplication/` | Token-based clone detector (advisory, `--duplication`). SSOT: `docs/duplication_architecture.md` |
| **git** | `git/{git_exec,git_state,git_object_file_source,diff_query,history_query}.cpp` (+ `git/git_hardening.h`) | `unit/git/*` | — | Fork/exec `git`, read blobs, diff revspec (`--diff`), history (`--history`) |
| **diff** | `diff/{regression_report,diff_json_report,md_report}.cpp` | `unit/diff/regression_report_test.cpp`, `integration/diff/*` | `drift_real_world/` | Regression / diff reporting for `--diff` (text / json / md) |
| **report** | `report/{text_reporter,json_reporter,violation_baseline}.cpp` (+ `report/json_escape.h`) | `unit/report/*` | — | Reporters (`file:line: [rule] message`), violation baseline (`--baseline` / `--save-baseline`) |

## Rule → files (the money area)

Rules are registered in `src/rules/rule_set.cpp`
(`makeDefaultRuleSet` / `makeDriftRuleSet`). Adding a rule = a new `.h`/`.cpp` pair
+ one `push_back` line in the factory; existing rule files stay untouched (see
CLAUDE.md "One rule = one class = one file").

| Rule / check | `src/rules/…` + `include/archcheck/rules/…` | Test | Fixtures |
|---|---|---|---|
| SF.7 `using namespace` | `sf7_using_namespace.{cpp,h}` | `unit/rules/sf7_using_namespace_test.cpp` | `sf7_using_namespace/` |
| SF.8 include guard | `sf8_include_guard.{cpp,h}` | `unit/rules/sf8_include_guard_test.cpp` | `sf8_include_guard/` |
| SF.9 no cycles | `sf9_no_cycles.{cpp,h}` | `unit/rules/sf9_no_cycles_test.cpp` | `sf9_no_cycles/` |
| Lakos god-headers | `lakos_god_headers.{cpp,h}` | `unit/rules/lakos_god_headers_test.cpp` | `lakos_god_headers/` |
| Lakos chain length | `lakos_chain_length.{cpp,h}` | `unit/rules/lakos_chain_length_test.cpp` | `lakos_chain_length/` |
| Drift: no shortcut edge | `drift_no_shortcut_edge.{cpp,h}` | `unit/rules/drift_no_shortcut_edge_test.cpp` | `drift_shortcut_edge/` |
| Drift: bidirectional coupling | `drift_bidirectional_coupling.{cpp,h}` | *(via `integration/rules/drift_fixtures_test.cpp`)* | `drift_bidirectional/` |
| Drift: lateral module dependency | `drift_lateral_module_dependency.{cpp,h}` | `unit/rules/drift_lateral_module_dependency_test.cpp` | `drift_lateral/` |
| Drift: no cycle growth | `drift_no_cycle_growth.{cpp,h}` | `unit/rules/drift_no_cycle_growth_test.cpp` | `drift_cycle_growth/` |
| Gate policy (gate vs advisory) | `gate_policy.{cpp,h}` | `unit/rules/gate_policy_test.cpp` | — |
| Area-of helper (module attribution) | `area_of.h` | `unit/rules/area_of_test.cpp` | — |

> The shipped-vs-planned set and gate/advisory status are authoritative in
> [CHANGELOG.md](../../CHANGELOG.md) and the README gate table — **do not restate
> them here**, they drift. This page maps *where the code lives*, not *what ships*.

## For an agent

- Changing a rule → read its `.h` + `.cpp` + its `unit/rules/*_test.cpp` + its
  `fixtures/<rule>/`. After the change: rebuild Debug, run `archcheck_tests`, then run the standard post-change checks (see [schema.md](schema.md)).
- Adding a rule → new pair of files + one line in `src/rules/rule_set.cpp`; add
  `fixtures/<rule>/pass/` and `fixtures/<rule>/fail_*/` (a rule without fixtures does
  not exist — CLAUDE.md).
- Touching graph metrics (CCD/ACD/NCCD, cycles) → `src/graph/algorithms.cpp` +
  `unit/graph/algorithms_test.cpp`; this is `Risk: high` per the commit guide.
- Duplication work → start at `docs/duplication_architecture.md` (SSOT), not the code.

## Related pages

- [[index.md]]
