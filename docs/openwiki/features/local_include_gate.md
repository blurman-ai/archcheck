---
title: "Local include-resolution gate — CASE_MISMATCH_INCLUDE / UNRESOLVED_LOCAL_INCLUDE"
type: feature
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "src/scan/local_include_scan.cpp"
  - "include/archcheck/scan/local_include_scan.h"
  - "src/rules/gate_policy.cpp"
  - "include/archcheck/rules/gate_policy.h"
  - "src/cli/check_command.cpp"
  - "src/cli/check_command.h"
  - "src/main.cpp"
  - "tests/integration/scan/local_include_gate_fixtures_test.cpp"
  - "fixtures/local_include_gate/"
related_pages:
  - "../index.md"
  - "../source-map.md"
  - "../cli-contract.md"
external_sources: []
---

# Local include-resolution gate — CASE_MISMATCH_INCLUDE / UNRESOLVED_LOCAL_INCLUDE

## Purpose

In plain check mode, scans every quoted `#include` that the graph could not
resolve with exact filesystem case and splits it into two disjoint findings
(#167/#168): a real build break on case-sensitive filesystems
(`CASE_MISMATCH_INCLUDE`, **gates**), or a quoted local include that resolves
nowhere at all (`UNRESOLVED_LOCAL_INCLUDE`, advisory by default, promotable to
gating). Angle-bracket / system includes are never scanned.

## Verified facts

- **Detection**: `scanLocalIncludeIssues(snapshot, universe = nullptr)`
  (`include/archcheck/scan/local_include_scan.h:48-49`, impl
  `src/scan/local_include_scan.cpp:120-147`). Only `IncludeKind::Quote`
  directives are considered (`classify`, `:103-104`); a directive the graph
  already resolves (`Resolution::Unresolved` check at `:105-106`) is skipped.
- **`LocalIncludeIssue::Kind`** (`local_include_scan.h:17-21`): `CaseMismatch`
  ("resolves only when case is ignored — a build break on case-sensitive
  filesystems") vs. `Unresolved` ("no match at all — may be generated or
  depend on a custom include root").
- **Exact-case escape hatch** (`exact_case_exists`, `local_include_scan.cpp:81-94`):
  a token is *not* a case problem if it exists exact-case in the scan
  subtree's index, in the enclosing repo's universe index, or as a
  rebased-relative path under the universe (an include root elsewhere in the
  repo). Comment: "Without this guard every self-include is a phantom
  CASE_MISMATCH against itself (#168)."
- **`RepoUniverse`** (`local_include_scan.h:37-41`): carries the enclosing
  repo's full file list plus the scan root's repo-relative prefix, for when the
  scan root is a subtree of a larger repo (`archcheck src` reaching
  `include/`). Built by `repoUniverseFor` (`src/cli/check_command.cpp:119-134`),
  which walks parent directories looking for `.git` and returns `nullopt` if
  the scan root already IS a repo root.
- **Case-insensitive match** (`ci_match`, `:66-72`): lexicographically smallest
  match from a lowercased `'/'`-suffix index (`build_ci_suffix_index`, `:38-52`)
  built over the subtree and, if present, the universe.
- **Classification result** (`classify`, `:100-116`): exact-case exists →
  `nullopt` (silent); else a case-insensitive match (subtree or universe) →
  `Kind::CaseMismatch` with `resolvedCaseInsensitive` set; else →
  `Kind::Unresolved`, `resolvedCaseInsensitive` empty.
- **Violation construction** (`toViolation`, `src/cli/check_command.cpp:139-147`):
  - `CaseMismatch` → `{"CASE_MISMATCH_INCLUDE", issue.file, issue.line,
    "quoted include \"" + issue.include + "\" resolves only case-insensitively
    to " + issue.resolvedCaseInsensitive + " (breaks on case-sensitive
    filesystems)"}` (`:141-144`).
  - `Unresolved` → `{"UNRESOLVED_LOCAL_INCLUDE", issue.file, issue.line,
    "quoted local include \"" + issue.include + "\" does not resolve under
    project roots"}` (`:145-146`).
- **Gate policy** (`classifyForGate`, `src/rules/gate_policy.cpp:8-21`): in
  `GateMode::Check`, `SF.9` and `CASE_MISMATCH_INCLUDE` are always
  `FindingDisposition::Gating`; `UNRESOLVED_LOCAL_INCLUDE` is `Gating` only if
  `failOnUnresolvedLocal` is true, else `Advisory`; every other rule id is
  `Advisory`. In `GateMode::Drift`, only `DRIFT.1`/`DRIFT.2`/`DRIFT.4.CYCLE` gate.
- **`--fail-on-unresolved-local` flag**: `src/main.cpp:279` dispatches to
  `dispatch_fail_unresolved_local` (`:213-219`), which calls `cli::runCheck(root,
  OutputFormat::Text, {}, std::nullopt, /*failOnUnresolvedLocal=*/true)`
  (`:218`). Help text: `"  archcheck --fail-on-unresolved-local [path]  (also
  gate on quoted local includes that do not resolve)"` (`main.cpp:24`).
- **Reported findings text** (`reportCheckGate`, `src/cli/check_command.cpp:101-105`):
  when advisories exist, prints `"note: " + advisory + " advisory finding(s)
  reported, not gated (chain-length / god-header / SF.7-8 are physical-design
  advisories); the gate fails on dependency cycles (SF.9) and include case
  mismatches (CASE_MISMATCH_INCLUDE). Use --baseline to track existing debt.\n"`.
- **Wiring into check mode** (`checkViolations`, `src/cli/check_command.cpp:228-249`):
  after running the registered rules, builds the `RepoUniverse` via
  `repoUniverseFor(root)` and appends `toViolation(issue)` for every
  `scanLocalIncludeIssues(snapshot, universe ? &*universe : nullptr)` result
  (`:245-247`) — this runs for every `archcheck <path>` check-mode invocation,
  not gated behind a flag itself.
- **Gate status**: per README, `CASE_MISMATCH_INCLUDE` is listed under
  **gating**, `UNRESOLVED_LOCAL_INCLUDE` under **advisory** (promotable via
  `--fail-on-unresolved-local`) (`README.md:43`); the signal table's "Core
  gate" row lists "SF.9 cycles, CASE_MISMATCH_INCLUDE, DRIFT.1/2/4.CYCLE, `--diff`
  new/grown cycles and new god-headers | exit `1`" (`README.md:52`). CHANGELOG
  0.1.6 confirms: "`CASE_MISMATCH_INCLUDE`… gates by default… `UNRESOLVED_LOCAL_INCLUDE`,
  advisory by default; `--fail-on-unresolved-local` promotes it to a gating
  finding." (`CHANGELOG.md:19-31`, #168).
- **Self-include regression guard**: `pass_self_include` fixture models the
  libjxl `enc_fast_lossless.cc` pattern (a file `#include`-ing itself for macro
  re-expansion); confirmed silent by
  `local_include_gate_fixtures_test.cpp:110-116`.
- **Attribution**: no Core Guidelines/Lakos/Martin citation in code, README, or
  CHANGELOG for this gate — it is framed as "a real build break on
  case-sensitive Linux" (`README.md:43`), not an authority citation in the
  Core-Guidelines/Lakos/Martin sense the default rule set uses.

## For an agent

- Change detection/classification → `src/scan/local_include_scan.{cpp,h}`.
- Change gate vs. advisory disposition → `src/rules/gate_policy.{cpp,h}`
  (`classifyForGate`) — this is the one place `Check`/`Drift` mode and
  `failOnUnresolvedLocal` are consulted; don't duplicate the dispatch elsewhere.
- Change the emitted rule id / message text → `toViolation` in
  `src/cli/check_command.cpp:139-147`.
- After changing: rebuild Debug, run
  `integration/scan/local_include_gate_fixtures_test.cpp` (9 cases: pass, case
  mismatch, unresolved, subtree with/without universe, cross-boundary case
  mismatch, self-include), exercise
  `fixtures/local_include_gate/{pass,fail_case_mismatch,fail_unresolved,
  pass_self_include,subtree}`, then run the standard post-change checks (see [schema.md](../schema.md)). (must stay 0).

## Evidence

- `src/scan/local_include_scan.cpp:81-94,100-116` — exact-case escape hatch + classification.
- `src/cli/check_command.cpp:139-147` — rule ids + exact message strings.
- `src/rules/gate_policy.cpp:8-21` — gate disposition per rule id / mode / flag.
- `src/main.cpp:213-219,279` — `--fail-on-unresolved-local` wiring.
- `README.md:43,52` and `CHANGELOG.md:19-31` — gate classification (#168).

## Related pages

- [[../index.md]] · [[../source-map.md]] · [[../cli-contract.md]]
