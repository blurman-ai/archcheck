---
title: "Test co-evolution — production churn without matching test churn"
type: feature
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "src/scan/test_co_evolution.cpp"
  - "include/archcheck/scan/test_co_evolution.h"
  - "src/cli/diff_command.cpp"
  - "tests/unit/scan/test_co_evolution_test.cpp"
  - "fixtures/test_co_evolution/"
related_pages:
  - "../index.md"
  - "../source-map.md"
  - "../cli-contract.md"
external_sources: []
---

# Test co-evolution — production churn without matching test churn

## Purpose

Flags a `--diff` where production code churns significantly while tests stay
(almost) silent — production and tests are expected to co-evolve; large
one-sided churn is a signal the change shipped without test coverage
(`test_co_evolution.h:11-16`).

## Verified facts

- **Entry point**: `detectTestCoEvolution(const std::vector<archcheck::git::NumStat> &numstat)`
  (`test_co_evolution.h:16`, impl `test_co_evolution.cpp:96-119`). Input is `git diff
  --numstat`-shaped data (`archcheck::git::NumStat`), not per-line content.
- **Path classification** (`classifyPath`, `:25-46`) buckets each numstat entry into
  `Production` / `Test` / `Other`:
  - a rename entry (`" => "` present) is classified by its **destination** path
    (`:28-29`);
  - must have a "project extension" (`hasProjectExtension`, `:33`) or it is `Other`;
  - vendored files (`isVendoredFile` / `pathHasVendoredDir`, `:38`) are `Other`, not
    `Production`;
  - test dir/basename (`pathHasTestDir` / `isTestBasename`, `:42`) → `Test`;
    everything else with a project extension → `Production`.
- **Churn aggregation** (`aggregateChurn`, `:58-82`): rename-only entries
  (`added == 0 && removed == 0`) are skipped entirely (`:66-67`, confirmed by
  `test_co_evolution_test.cpp:76-81`, "rename-only → ignored for churn").
- **Trigger thresholds** (`shouldReportCoEvolution`, `:85-92`, literal conditions):
  - `prodChurn >= 80 && testChurn == 0` (`:87`) — "strict": any production churn
    ≥ 80 lines (added+removed) with zero test churn.
  - `prodChurn >= 200 && testChurn > 0 && testChurn < (prodChurn / 20)` (`:89`) —
    "soft": production churn ≥ 200 with test churn present but under 5% of it.
    Confirmed boundary-exact by `test_co_evolution_test.cpp:110-118` ("soft
    threshold boundary": prod=200, test=10 → exactly 5% → does **not** fire, since
    the code uses strict `<`).
- **At most one finding per diff**, id `TEST.1.prod_changed_tests_silent`
  (`test_co_evolution.cpp:112`), file `"<diff>"` (`:113`), `line = 0` (`:114`) — a
  whole-diff-level signal, not tied to one file/line (confirmed
  `test_co_evolution_test.cpp:52-53`).
- **Message format** (`:107-109`): built with
  `msg << "prod +" << stats.prodAdded << "/-" << stats.prodRemoved << " across "
  << stats.prodFileCount << " file(s), tests +" << stats.testAdded << "/-" <<
  stats.testRemoved;` — e.g. for prod +70/-30 across 2 files, tests +0/-0, the
  message contains `"+70/-30"`, `"2 file(s)"`, `"+0/-0"` (confirmed
  `test_co_evolution_test.cpp:55-57`).
- **Wiring**: called once per `--diff` run, fed by a full numstat pass —
  `result.testCoEvolution = archcheck::scan::detectTestCoEvolution(numstatEntries);`
  (`src/cli/diff_command.cpp:241`, `numstatEntries` from `archcheck::git::collectNumstat`
  at `:240`).
- **Report header**: printed under the literal header `"test co-evolution
  (advisory):"` with each finding as `"  " << ruleId << ": " << message` (no
  file/line — `diff_command.cpp:270-275`), a different format from the shared
  `printViolationList` used by SATD/flag-argument/bool-field, because this finding
  has no per-line location.
- **Gate status**: **advisory only** (README gate table: "PR hygiene advisories |
  SATD, test co-evolution, local complexity, flag arguments, new clones | reported,
  exit `0`", `README.md:54`). CHANGELOG: "Advisory-only." (CHANGELOG.md, #097).
- **Attribution**: no Core Guidelines/Lakos/Martin citation found in code or
  CHANGELOG — `unverified` whether this carries formal authority attribution;
  CHANGELOG cites only issue #097.

## For an agent

- Change detection → `src/scan/test_co_evolution.cpp` (+ header). Threshold
  literals (`80`, `200`, `/20`) are inline in `shouldReportCoEvolution`
  (`:85-92`), not sourced from `Thresholds` (`include/archcheck/config/config.h`) —
  verify before assuming they are configurable.
- Called only from the `--diff` path (`src/cli/diff_command.cpp:241`); there is no
  check-mode invocation.
- After changing: rebuild Debug, run `unit/scan/test_co_evolution_test.cpp`,
  exercise `fixtures/test_co_evolution/{pass,fail}/*.numstat`, then run the standard post-change checks (see [schema.md](../schema.md)).

## Evidence

- `src/scan/test_co_evolution.cpp:85-92` — the two literal trigger conditions.
- `src/scan/test_co_evolution.cpp:96-119` — `detectTestCoEvolution`, message format,
  rule id `TEST.1.prod_changed_tests_silent`.
- `src/cli/diff_command.cpp:241,270-275` — wiring + report header string/format.
- `README.md:54` — advisory gate classification.

## Related pages

- [[../index.md]] · [[../source-map.md]] · [[../cli-contract.md]]
