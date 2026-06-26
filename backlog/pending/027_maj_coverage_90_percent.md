# [BUILD/TESTS] Raise coverage-gate thresholds to 90% across all metrics

**Created:** 2026-05-28
**Started:** 2026-05-28
**Status:** blocked (toolchain constraint — waiting for an environment change)
**Module:** BUILD / TESTS
**Priority:** major
**Blocks:** —
**Blocked by:** lcov 1.13 + GCC 8.3 (Astra Linux 1.7) — see `docs/dev/coverage_constraints.md`
**Related:** —

## Goal

Raise the thresholds in `scripts/check_coverage.sh` from `70/60/40` to `90/90/90`
and make the gate pass reliably at those thresholds.

## Current state (2026-05-28)

| Metric    | Was (task start) | Now    | Goal | Gap |
|-----------|------------------|--------|------|-----|
| Lines     | 70%              | ~96%   | 90%  | ✓ (threshold raised to 90%) |
| Functions | 60%              | ~95%   | 90%  | ✓ (threshold raised to 90%) |
| Branches  | 40%              | ~63%   | 90%  | **−27 pp — blocked** |

Lines and Functions: **partially done** — thresholds raised to 90%, actual values are higher.
Branches: **blocked** — 90% is unreachable on the current toolchain; the threshold is left at 40%.

## Why branches is blocked

GCC 8.x + lcov 1.13 (Astra Linux 1.7): every call to a potentially-throwing function
produces a throw arc — the "returned normally" branch plus the "threw an exception" branch.
Throw arcs are physically uncoverable without simulating OOM.

**lcov 1.13** — no `--exclude-unreachable-branches` (added in 1.15).
**lcov 2.4** with `--filter exception` — tested, **no effect** on GCC 8.x data:
GCC 8 does not annotate throw-arcs in a format lcov 2.4 recognizes.
`libcapture-tiny-perl` and `libdatetime-perl` are installed — the Perl dependencies work,
but the filter itself does not fire on GCC 8 gcov data.

Details: `docs/dev/coverage_constraints.md`.

## Exact per-file breakdown (lcov --list, 2026-05-28)

| File | Branches% | Total branches | Uncovered |
|------|-----------|----------------|-----------|
| `regression_report.h` | 25.0% | 24 | 18 |
| `scan/disk_file_source.cpp` | 50.0% | 8 | 4 |
| `report/violation_baseline.cpp` | 53.3% | 240 | 112 |
| `graph/graph_builder.cpp` | 54.1% | 37 | 17 |
| `rules/sf9_no_cycles.cpp` | 55.9% | 68 | 30 |
| `git/git_object_file_source.cpp` | 57.2% | 138 | 59 |
| `rules/lakos_chain_length.cpp` | 57.7% | 26 | 11 |
| `rules/lakos_god_headers.cpp` | 57.7% | 26 | 11 |
| `git/git_state.cpp` | 58.6% | 232 | 96 |
| `graph/baseline.cpp` | 59.6% | 280 | 113 |
| `graph/diff.cpp` | 69.3% | 140 | 43 |
| `graph/algorithms.cpp` | 70.1% | 154 | 46 |
| `scan/include_scanner.cpp` | 76.1% | 184 | 44 |

## Done

- `MIN_LINES=90`, `MIN_FUNCTIONS=90` raised in `scripts/check_coverage.sh` — passing
- `MIN_BRANCHES=40` kept — cannot go higher without a toolchain change
- `LCOV_EXCL_START/STOP` around post-fork child-process code:
  - `src/git/git_state.cpp` — execChild + LCOV_EXCL_BR_LINE on drainFd/collectChild
  - `src/git/git_object_file_source.cpp` — execGitChild, execCatFileBatch, runGitOneShot, spawnCatFileBatch
- `LCOV_EXCL_BR_LINE` on ostream calls in `src/diff/regression_report.cpp`
- New tests `tests/unit/diff/regression_report_test.cpp` — +5: writeAdded, writeGrown, writeEmpty, metric regressions
- New tests `tests/unit/graph/baseline_test.cpp` — +6: MalformedSchema errors, empty graph
- Bug fix in `src/graph/baseline.cpp`: empty graph → `nodes: []` instead of YAML null
- `docs/dev/coverage_constraints.md` — new file, documents the problem and what was tried
- `CLAUDE.md` — link to coverage_constraints.md

## Unblocked when

- Moving to Ubuntu 22.04+ / Debian Bookworm → lcov 1.16+: add `--exclude-unreachable-branches`
- Or `pip3 install gcovr` + rewrite `check_coverage.sh` for gcovr (`--exclude-throw-branches`)

## Key decisions

| Decision | Reason |
|----------|--------|
| LCOV_EXCL_START/STOP only for the fork-child | The only legitimate place: gcov can't see the child process |
| LCOV_EXCL_BR_LINE only for explicit throw-arc lines | Don't touch lines with logical branches |
| Branches threshold NOT raised | Toolchain limitation; can't be worked around without changing gcc/lcov |
| lcov 2.4 experiment failed | `--filter exception` doesn't work on GCC 8 gcov data |

## Changed files

| File | Change |
|------|--------|
| `scripts/check_coverage.sh` | MIN_LINES/FUNCTIONS → 90 (branches kept at 40) |
| `src/git/git_state.cpp` | LCOV_EXCL_START/STOP + LCOV_EXCL_BR_LINE |
| `src/git/git_object_file_source.cpp` | LCOV_EXCL_START/STOP + LCOV_EXCL_BR_LINE |
| `src/diff/regression_report.cpp` | LCOV_EXCL_BR_LINE on STL calls |
| `src/graph/baseline.cpp` | fix write_nodes_block/write_edges_block for the empty graph |
| `tests/unit/diff/regression_report_test.cpp` | +5 new tests |
| `tests/unit/graph/baseline_test.cpp` | +6 new tests |
| `docs/dev/coverage_constraints.md` | new file — explains the branches limitation |
| `CLAUDE.md` | link to coverage_constraints.md |
