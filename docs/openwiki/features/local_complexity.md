---
title: "Local complexity drift — per-function cognitive complexity growth (`DRIFT.LOCAL_COMPLEXITY`)"
type: feature
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "src/scan/local_complexity_metrics.cpp"
  - "include/archcheck/scan/local_complexity_metrics.h"
  - "src/scan/local_complexity_drift.cpp"
  - "include/archcheck/scan/local_complexity_drift.h"
  - "src/cli/diff_command.cpp"
  - "include/archcheck/config/config.h"
  - "tests/unit/scan/local_complexity_metrics_test.cpp"
  - "tests/unit/scan/local_complexity_drift_test.cpp"
  - "tests/integration/scan/local_complexity_fixtures_test.cpp"
  - "fixtures/local_complexity_drift/"
related_pages:
  - "../index.md"
  - "../source-map.md"
  - "../cli-contract.md"
external_sources: []
---

# Local complexity drift — per-function cognitive complexity growth (`DRIFT.LOCAL_COMPLEXITY`)

## Purpose

In `--diff`, computes a per-function Sonar-style Cognitive Complexity score
(Campbell 2018) for each changed file's old and new version, matches functions
across the two versions, and reports functions whose complexity **grew**
past corpus-calibrated floors, or brand-new functions that start above the
threshold (#101/#102). Advisory only.

## Verified facts

- **Rule id**: `DRIFT.LOCAL_COMPLEXITY` (`kRuleId`, `src/scan/local_complexity_drift.cpp:19`).
- **Metric** (`computeFileComplexity`, `local_complexity_metrics.cpp:375-382`,
  declared `local_complexity_metrics.h:33`): lexes the source, strips
  preprocessor-directive tokens (`stripDirectiveTokens`, keeping the largest
  branch of an `#if`/`#elif`/`#else` chain — `:285-330`), discovers function
  spans (`discoverFunctions`, from `function_body_scan.h`) and scores each with
  a token-level state machine (`scoreSpan`/`ScoreState`, `:24-281`) — the header
  calls it a "token-level port of the corpus-validated #102 prototype scorer"
  (`local_complexity_metrics.h:31-32`).
- **`FunctionComplexity` fields** (`local_complexity_metrics.h:13-24`):
  `qualifiedName`, `paramFingerprint`, `paramArity`, `startLine`, `endLine`,
  `meaningfulLoc`, `score` (comment: "Sonar Cognitive Complexity (Campbell 2018),
  token-level"), `structuralCount`, `maxNesting` — the header notes `score` is
  "the only field findings are based on; the rest is report diagnostics" (`:11-12`).
- **Thresholds** (`local_complexity_drift.cpp:19-27`):
  - `kCognitiveThreshold = 25` — comment: "Sonar C-family / clang-tidy default
    threshold."
  - `kDeltaK = 5` — soft-warning floor (referred to as `LCX.3` in code comments
    and `docs/research/local_complexity_drift_corpus_report.md:22`).
  - `kAboveFloor = 3` — floor for growth on an already-above-threshold function
    (`LCX.2`); comment cites "the Δ1–2 tail on already-huge functions was 72/210
    corpus findings and not actionable (#102 verdict)."
  - `LCX.1`/`LCX.2`/`LCX.3` are **doc/comment labels for the signal hierarchy**,
    not separate `ruleId` strings in code — the only emitted `ruleId` is
    `DRIFT.LOCAL_COMPLEXITY` for every finding kind.
- **Decision hierarchy** (`reasonFor`, `:154-164`, applied when an old match
  exists): let `delta = newScore - oldScore`;
  - `oldScore < 25 && newScore >= 25` (confident match) → suffix `", crossed 25"` (LCX.1)
  - `oldScore >= 25 && delta >= 3` (confident match) → suffix `", already above 25"` (LCX.2)
  - else `delta >= 5` → empty suffix (LCX.3)
  - else → `nullopt`, no finding.
- **Growth message** (`growthMessage`, `:166-170`): `"function '" +
  qualifiedName + "' grew local complexity from " + oldScore + " to " +
  newScore + " (+" + delta + reasonSuffix + ")"`.
- **New-function finding** (`addNewFunctionFinding`, `:174-181`): only if
  `fn.score >= 25`; message `"new function '" + qualifiedName + "' has local
  complexity " + score + " (threshold 25)"`.
- **Exclusions / cross-matching guards**:
  - Test-macro bodies (`TEST`, `TEST_F`, `TEST_P`, `TYPED_TEST`, `BENCHMARK`,
    `MOCK_METHOD`) are dropped (`isTestMacroSymbol`, `:37-42`).
  - A file with an unclosed brace (broken commit) is skipped entirely on either
    side (`hasUnclosedBrace`, `:139-150`).
  - Matching key is `(qualifiedName, paramArity)`, preferring an exact
    `paramFingerprint`; ambiguous multi-candidate matches degrade to
    nearest-start-line **low confidence**, which only ever qualifies for the
    soft LCX.3 signal (`findOldMatch`, `:57-84`, used in `reasonFor`'s
    `!lowConfidence` guards).
  - Arity-changed match (`findArityChangedMatch`, `:88-103`) and namespace/class
    "rescoped" match (`findRescopedMatch`, `:110-133`) both mark `lowConfidence
    = true`.
  - Cross-file move detection: functions that disappeared from an old file
    (score `>= kDeltaK`) are pooled (`MovedFunction`, `addDisappearedFunctions`,
    `:187-214`); a "new" function elsewhere whose short name/arity/score (within
    tolerance) matches a pooled entry is treated as relocated, not grown
    (`consumeMovedMatch`, `:217-230`, used in `compareFunctions` at `:248-249`).
  - Authored-file classification comes from the whole current tree via
    `SourceSnapshot` (vendored/test/generated dropped) — `collectFilePairs`,
    `:282-316`.
- **Two entry points**: `compareLocalComplexity(oldSource, newSource, file)` —
  single-file compare (`local_complexity_drift.h:27-28`, impl `:265-271`); and
  `detectLocalComplexityDrift(oldSnapshot, newSnapshot, changedFiles)` — whole
  diff, shares one `movedPool` across files (`.h:34-36`, impl `:320-335`).
- **`ComplexityDriftResult`** (`local_complexity_drift.h:16-21`): `violations`
  plus `positiveDelta`/`negativeDelta` (PR-level aggregate; comment "Advisory
  only: never gates the exit code (#101)").
- **Config guard**: `diffMaxAddedLines`, default `10000`
  (`include/archcheck/config/config.h:47`), YAML key `diff_max_added_lines`
  (`src/config/config_loader.cpp:325`). If the diff's total added lines (from
  `git::collectNumstat`) exceed it, `collectDiffAdvisories` returns early
  (`src/cli/diff_command.cpp:244-249`) **before** calling
  `collectSnapshotAdvisories` — so this single guard also skips new-clone drift,
  flag-argument drift, and bool-field drift for that commit, not just complexity.
  Skip message: `"\nlocal complexity drift (advisory): skipped — diff adds " +
  totalAdded + " lines (bulk import; thresholds.diff_max_added_lines)\n"`
  (`diff_command.cpp:278-279`).
- **Reporting** (`printComplexityResult`, `diff_command.cpp:87-98`): header
  `"\nlocal complexity drift (advisory):\n"`, per-violation line `"  " << file
  << ":" << line << ": " << ruleId << " — " << message`, then `"  net
  complexity delta: +" << positiveDelta`, plus `" (improvements: " <<
  negativeDelta << ")"` when `negativeDelta < 0`.
- **Gate status**: advisory — README: "…local complexity, flag-argument drift,
  and new clone drift are advisory" (`README.md:46`); signal table "PR hygiene
  advisories | …, local complexity, … | reported, exit `0`" (`README.md:54`).
- **Attribution**: cites Sonar/clang-tidy's Cognitive Complexity default
  (Campbell 2018) for the metric itself; no Core Guidelines/Lakos/Martin
  citation for the *drift* signal — `unverified` whether a formal attribution
  beyond the metric's own citation exists.

## For an agent

- Change the scorer → `src/scan/local_complexity_metrics.cpp` (+ header); it is
  a hand-ported state machine — read the `ScoreState` fields and
  `docs/research/local_complexity_drift_scorer_review.md` before editing.
- Change the drift comparison / matching heuristics →
  `src/scan/local_complexity_drift.cpp`; every heuristic there cites a corpus
  false-positive class in its comment (`#109 <repo>`) — read the comment before
  loosening a guard.
- Change either default (`kCognitiveThreshold`, `kDeltaK`, `kAboveFloor`) → these
  are file-local `constexpr`, not in `config.h`; changing `diffMaxAddedLines` IS
  in `config.h` (SSOT).
- Called only from `--diff` (`src/cli/diff_command.cpp:224,282`); no check-mode
  invocation exists.
- After changing: rebuild Debug, run `unit/scan/local_complexity_metrics_test.cpp`
  (20 cases, scorer semantics) + `unit/scan/local_complexity_drift_test.cpp` (18
  cases, matching/move heuristics) + `integration/scan/local_complexity_fixtures_test.cpp`
  (6 cases against `fixtures/local_complexity_drift/{pass,fail_growth,
  fail_new_complex}`), then run the standard post-change checks (see [schema.md](../schema.md)).

## Evidence

- `src/scan/local_complexity_drift.cpp:19-27` — rule id + threshold constants.
- `src/scan/local_complexity_drift.cpp:154-181` — decision hierarchy + messages.
- `include/archcheck/scan/local_complexity_metrics.h:11-24` — `FunctionComplexity` fields.
- `include/archcheck/config/config.h:47` — `diffMaxAddedLines` default.
- `src/cli/diff_command.cpp:87-98,244-249,278-279` — reporting + bulk-import guard.
- `README.md:46,54` — advisory gate classification.

## Related pages

- [[../index.md]] · [[../source-map.md]] · [[../cli-contract.md]]
