# [SCAN][DIFF] DRIFT.BOOL_FIELD_ACCRETION counts a field whose type changed to bool as a new bool field

**Created:** 2026-10-22
**Start date:** —
**Status:** new
**Module:** SCAN / DIFF
**Priority:** minor
**Difficulty:** unknown
**Blocks:** —
**Blocked by:** —
**Related:** #089 (boolean state drift), #135 (bool field accretion), #161 (manual FP audit — "rename/move artifact" class)

## Goal

A member that already existed in the baseline struct under another type (`uint8_t x;` → `bool x;`) must not count as accreted.

## Context

[src/scan/bool_field_drift.cpp](../../src/scan/bool_field_drift.cpp) collects only `bool` members per struct and emits a finding when the new struct has more of them than the baseline. A type change raises the bool count without adding state.

Found on a private project (per-commit `--diff` run, manual check of 20 random findings — all 20 were real additions of a bool member, 2 of them not new state):

- type change: struct `S`: `- uint8_t flag;` / `+ bool flag;` → reported "accreted 1 bool field(s): flag".
- move between structs: struct `Logger` gained `needDump`, previously `serviceData->needDump` in another struct, same commit. Lower priority — a move does add the field to this struct; decide whether to treat it separately.

## Execution plan

- [ ] Collect all member names of the baseline struct (not only bool ones); drop gained bool names that were already members under another type.
- [ ] Fixture: `fixtures/bool_field_drift/pass_type_change/` (`uint8_t f;` → `bool f;`) → no finding; existing fail fixtures stay red.
- [ ] Decide on cross-struct moves (same name removed from another struct in the same commit) — report or not.

## Done

- (empty)

## In progress

- (empty)

## Next steps

1. Check how members are parsed now (`boolFields`) and whether a general member-name pass is cheap.

## Key decisions

| Decision | Reason |
|---------|---------|
| ... | ... |

## Changed files

| File | Change |
|------|-----------|
| ... | ... |

## Fixtures (if a rule)

- [ ] `fixtures/bool_field_drift/pass_type_change/`
- [ ] `fixtures/bool_field_drift/fail_*/` (existing)
