# [SCAN][DIFF] DRIFT.LOCAL_COMPLEXITY reports a renamed function as a new function with its full complexity

**Created:** 2026-10-22
**Start date:** —
**Status:** new
**Module:** SCAN / DIFF
**Priority:** minor
**Difficulty:** unknown
**Blocks:** —
**Blocked by:** —
**Related:** #102 (local complexity drift), #109 (rescoped-match fix for namespace moves)

## Goal

A function renamed in a commit (same file, same arity, old name gone) must be matched to its old version, so `--diff` reports only the complexity delta instead of "new function ... has local complexity N".

## Context

Matching in [src/scan/local_complexity_drift.cpp](../../src/scan/local_complexity_drift.cpp) is by `(qualifiedName, paramArity)`; `findRescopedMatch` (line ~110) covers namespace/class moves but requires the same short name. A rename changes the short name, so the old function is treated as deleted and the renamed one as new.

Found on a private project (per-commit `--diff` run, manual check of 15 random findings — 14 correct, this one wrong):

- before: `void computeA(in, shift, n, m, out)` in `x.cpp`;
- after: `void applyB(in, shift, n, m, out)` in the same file (same parameters, `computeA` gone), plus new tail-processing helpers;
- reported: `new function 'applyB' has local complexity 65 (threshold 25)`. The real growth is the tail handling, not 65.

On that run 47 of 297 production complexity findings are "new function"; how many are renames is not measured.

## Execution plan

- [ ] Add a rename match: old function absent on the new side, same file, same `paramArity`, no other candidate — choose by body similarity (token overlap) or nearest start line; report as growth from the old score.
- [ ] Keep genuinely new functions reportable: require the old one to have disappeared (same rule as `findRescopedMatch`).
- [ ] Fixture: `fixtures/local_complexity_drift/pass_rename/` — rename without new branches → no finding; `fail_rename_grew/` — rename + added branches → "grew from X to Y".
- [ ] Re-measure on a per-commit corpus run: how many "new function" findings turn into "grew from X to Y".

## Done

- (empty)

## In progress

- (empty)

## Next steps

1. Decide the similarity criterion (arity + same file only, or body tokens too).

## Key decisions

| Decision | Reason |
|---------|---------|
| ... | ... |

## Changed files

| File | Change |
|------|-----------|
| ... | ... |

## Fixtures (if a rule)

- [ ] `fixtures/local_complexity_drift/pass_rename/`
- [ ] `fixtures/local_complexity_drift/fail_rename_grew/`
