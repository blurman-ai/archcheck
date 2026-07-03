---
title: "History analytics — god-file growth + defect attractors (`--history`)"
type: feature
status: verified
last_checked_commit: "241b07e"
source_paths:
  - "src/scan/god_file_growth.cpp"
  - "include/archcheck/scan/god_file_growth.h"
  - "src/scan/defect_attractor.cpp"
  - "include/archcheck/scan/defect_attractor.h"
  - "src/git/history_query.cpp"
  - "include/archcheck/git/history_query.h"
  - "src/cli/preview_commands.cpp"
  - "src/main.cpp"
  - "tests/unit/scan/god_file_growth_test.cpp"
  - "tests/unit/scan/defect_attractor_test.cpp"
related_pages:
  - "../index.md"
  - "../source-map.md"
  - "../cli-contract.md"
external_sources: []
---

# History analytics — god-file growth + defect attractors (`--history`)

## Purpose

`archcheck --history <path>` mines `git log --numstat` to report two
report-only signals: files that are large **and** consistently growing
("god-file growth"), and files disproportionately represented in fix-like
commits ("defect attractors"). Exit is always `0` — this mode never gates CI.

## Verified facts

- **Mode**: `archcheck --history <path>` dispatches to `cli::runHistory`
  (`src/main.cpp:234-236`, `isPathPreviewMode` at `:257`). Help text:
  `"  archcheck --history <path>                   (history analytics: god-file
  growth; advisory, does not gate CI)"` (`main.cpp:37`) — the help string names
  only god-file growth; the defect-attractor signal also runs (see below) but is
  not mentioned in that line.
- **`runHistory`** (`src/cli/preview_commands.cpp:152-177`):
  1. `buildLocMap(root)` — current LOC per non-vendored file, counted as
     `'\n'` occurrences (`preview_commands.cpp:68-77`).
  2. `archcheck::git::queryCommitHistory(root, 200)` — up to 200 commits
     (`:155`).
  3. `GodFileGrowthDetector(locMap, history).detect()`; always prints
     `"queried " << history.size() << " commits, found " <<
     godFileViolations.size() << " god-file growth candidate(s)\n"` then, per
     violation, `file << ": [" << ruleId << "] " << message` (`:159-164`).
  4. `DefectAttractorDetector(history).detect()`; if non-empty, prints
     `"\ndefect attractors (advisory): " << defectViolations.size() << "
     file(s)\n"` then the same per-violation line format (`:168-175`).
  5. `return 0;` unconditionally (`:176`) — report-only, matches README:
     "History analytics | `--history` god-file growth and defect-attractor
     signals | report-only, exit `0`" (`README.md:55`).
- **`queryCommitHistory`** (`src/git/history_query.cpp:81-96`): runs `git log
  --numstat --format=%H%x1f%s -n <limit>` (no `--reverse`), i.e. plain git-log
  order. Header doc: "Returns commits in git-log order (newest first) —
  detectors rely on this." (`include/archcheck/git/history_query.h:28`).
  `parseHistoryOutput` (`:54-79`) splits on the `\x1f` (ASCII 31) separator and
  attaches numstat lines to the current commit; rename entries (`" => "`) are
  skipped (`:38-39`).
- **God-file growth** — `SIZE.1.god_file_growth`
  (`src/scan/god_file_growth.cpp:182`), `line = 0`. All four conditions must
  hold (`isGodFileCandidate`, `:131-162`; constants `:18-21`):
  1. `currentLoc >= P75` — P75 of current LOC over non-`AuthoredScope::pathExcluded`
     files (`calculateP75`, `:31-53`).
  2. net growth (`Σadded − Σdeleted` over the whole history window for that
     path, `computeNetGrowth`, `:55-71`) `>= 30%` of current LOC **or** `>= 300`
     lines absolute (`kMinGrowthPercent=30`, `kMinGrowthAbsolute=300`).
  3. `>= 5` consecutive commits (from `history_.rbegin()` forward, stopping at
     the first touching-but-non-growth commit) with `added > deleted`
     (`countConsecutiveGrowthTouches`, `:73-105`, `kMinConsecutiveGrowth=5`).
  4. no commit anywhere in the window has `deleted - added >= 50`
     (`hasMeaningfulShrink`, `:107-120`, `kMeaningfulShrinkThreshold=50`).
  - Message (`formatMessage`, `:122-129`): `"current " + currentLoc + " LOC, net
    +" + netGrowth + " lines, " + growthTouches + " consecutive growth
    commits"`.
- **Defect attractor** — `HIST.1.defect_attractor`
  (`src/scan/defect_attractor.cpp:199`), `line = 0`. Pipeline
  (`detect`, `:179-207`):
  - `isFixLikeCommit(subject)` matches `kFixPattern = R"(\b(fix(es|ed)?|bug|
    crash|regress(ion|ed)?|hotfix|segfault|fault|oops|leak)\b)"`, case-insensitive
    (`:24-25,56-61`).
  - `shouldSkipCommit` drops merge commits (subject starts `"Merge"`), commits
    with an empty file list, and "mechanical" fix-like commits touching `> 30`
    files (`:63-85`).
  - `aggregateFixTouches` counts, per production file (not
    `AuthoredScope::pathExcluded`, and matching `kProjectExtensions` from
    `file_classification.h:49`), how many surviving fix-like commits touch it
    (`:87-116,27-47`).
  - `kMinFixTouches = 5` absolute floor (`:20`); the reporting threshold is the
    90th-percentile touch count (`calculateTopDecileThreshold`, `:118-150`) —
    if fewer than 10 files have any fix touches, the threshold is the max
    instead of a percentile (`:141-144`).
  - A file qualifies if `count >= kMinFixTouches && count >= threshold`
    (`filterCandidates`, `:162-175`), sorted by count descending.
  - Message (`formatMessage`, `:152-157`): `fixTouches + " fix-like touches in "
    + history_.size() + " commits"` — the denominator is the **total** commits
    queried (up to 200), not just fix-like ones touching that file.
- **Internal ordering inconsistency (verified, unresolved in code)**: two
  headers disagree about the order `queryCommitHistory` returns.
  `history_query.h:28` states newest-first, and
  `defect_attractor.h:21` agrees ("Constructs detector from git history (newest
  commits first)."). But `god_file_growth.h:25` documents the opposite:
  `"history: commits in chronological order (oldest first, as returned by
  queryCommitHistory)"`, and `god_file_growth.cpp:75-76` implements
  `countConsecutiveGrowthTouches` on that assumption: `"Commits are stored
  oldest-first, so iterate backwards."`, iterating `history_.rbegin()` →
  `history_.rend()` expecting `rbegin()` to be the newest commit. Since
  `queryCommitHistory` actually returns newest-first (confirmed: plain `git log`
  with no `--reverse`, `history_query.cpp:89`) and `preview_commands.cpp:155-157`
  passes that result into `GodFileGrowthDetector` unmodified, `rbegin()` in
  production is the **oldest** commit in the (≤200-commit) window, not the
  newest — so the "5 consecutive recent growth commits" walk in
  `countConsecutiveGrowthTouches` actually starts from the oldest end of the
  window and moves toward the newest, the reverse of what its own comment
  claims. The unit tests in `tests/unit/scan/god_file_growth_test.cpp` do not
  disambiguate this: every multi-commit test uses either a uniform sequence of
  identical growth commits (order-invariant) or appends a distinguishing commit
  last, which — regardless of which end is "newest" — is still the first commit
  `rbegin()` visits, so passing tests do not prove which semantic ordering is
  correct in production. This is flagged here, not fixed — treat any change to
  `countConsecutiveGrowthTouches` or its callers as `Risk: medium`, verify the
  actual order end-to-end before relying on "recent."

## For an agent

- Change god-file growth → `src/scan/god_file_growth.{cpp,h}`; read the ordering
  caveat above first if touching `countConsecutiveGrowthTouches`.
- Change defect attractor → `src/scan/defect_attractor.{cpp,h}`; the fix-pattern
  regex and skip rules are the tunable surface.
- Change history collection → `src/git/history_query.{cpp,h}`; `limit`
  (default 200) is a `--numstat`-record cap, not wall-clock (`history_query.h:31`).
- Wired only from `--history` (`src/cli/preview_commands.cpp:152-177`,
  `src/main.cpp:234-236`) — there is no `--diff` or check-mode invocation of
  either detector.
- No `fixtures/` directory backs this feature (not listed as a rule with
  `fixtures/<rule>/pass|fail_*` in `docs/source-map.md`'s rule table) — coverage
  is unit-test only (`god_file_growth_test.cpp` 6 cases,
  `defect_attractor_test.cpp` 11 cases).
- After changing: rebuild Debug, run both unit test files, then run the standard post-change checks (see [schema.md](../schema.md)).

## Evidence

- `src/scan/god_file_growth.cpp:17-21,131-191` — the four conditions + rule id + message.
- `src/scan/defect_attractor.cpp:20-25,118-150,179-207` — pattern, threshold, rule id + message.
- `src/git/history_query.cpp:81-96` — `queryCommitHistory` git-log invocation (no `--reverse`).
- `include/archcheck/git/history_query.h:28` vs. `include/archcheck/scan/god_file_growth.h:25`
  and `src/scan/god_file_growth.cpp:75-76` — the ordering-contract mismatch.
- `src/cli/preview_commands.cpp:152-177` — `runHistory` wiring, always `return 0`.
- `README.md:55` — report-only, exit 0 classification.

## Related pages

- [[../index.md]] · [[../source-map.md]] · [[../cli-contract.md]]
