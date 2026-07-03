# [SCAN] Fix commit-order contract between history_query and god_file_growth

**Created:** 2026-07-03
**Start date:** —
**Status:** new
**Module:** SCAN
**Priority:** maj
**Difficulty:** unknown
**Blocks:** —
**Blocked by:** —
**Related:** #169 (openwiki — surfaced this while writing features/history_analytics.md)

## Goal

Resolve the contradictory commit-order contract so `--history` god-file-growth counts
consecutive *recent* growth from the correct end of the window.

## Context

Surfaced while documenting `--history` for the openwiki (#169). Two headers disagree
about what `queryCommitHistory` returns:

- `include/archcheck/git/history_query.h:28` — "Returns commits in git-log order
  (**newest first**) — detectors rely on this."
- `include/archcheck/scan/god_file_growth.h:25` — "commits in chronological order
  (**oldest first**, as returned by queryCommitHistory)."

The runtime truth: `queryCommitHistory` runs `git log --numstat ...` with **no
`--reverse`** (`src/git/history_query.cpp:89`) → **newest-first**. So `history_query.h`
is right and the `god_file_growth` comment is wrong.

`GodFileGrowthDetector::countConsecutiveGrowthTouches`
(`src/scan/god_file_growth.cpp:73-78`) is written on the false "oldest-first"
premise: its comment says "count from the most recent commits backwards… commits are
stored oldest-first, so iterate backwards" and it iterates `history_.rbegin()`. With
the actual newest-first order, `rbegin()` is the **oldest** commit, so the
"consecutive recent growth" count walks from the wrong end. Real `--history` runs feed
this path: `src/cli/preview_commands.cpp:155-157` pipes `queryCommitHistory(root,200)`
straight into `GodFileGrowthDetector`.

Impact: report-only analytics (`--history`, exit 0) — not a gate — but a wrong
number undermines trust in the signal. Also check `defect_attractor`
(`include/archcheck/scan/defect_attractor.h:21` also states an order assumption).

## Execution plan

- [ ] Add a test that pins the real order of `queryCommitHistory` on a fixture repo
      (assert commit[0] is the newest / HEAD).
- [ ] Fix the wrong side: correct the `god_file_growth.h`/`.cpp` comments and the
      iteration direction in `countConsecutiveGrowthTouches` so "consecutive recent
      growth" starts at the newest commit.
- [ ] Audit `defect_attractor` for the same order assumption; fix if affected.
- [ ] Add/extend a fixture proving the corrected consecutive-growth semantics.
- [ ] Update `docs/openwiki/features/history_analytics.md` (remove the flagged
      contradiction once fixed; bump `last_checked_commit`).

## Done
- (empty)

## In progress
- (empty)

## Next steps
1. Write the order-pinning test, confirm newest-first, then fix the detector side.

## Key decisions

| Decision | Reason |
|---------|---------|
| Fix the detector, not `queryCommitHistory` | `history_query.h:28` + `git log` (no `--reverse`) are the established contract "detectors rely on"; the `god_file_growth` comment is the wrong one. |

## Changed files

| File | Change |
|------|-----------|
| ... | ... |

## Fixtures (if a rule)

- [ ] history fixture proving consecutive-recent-growth counts from the newest commit
