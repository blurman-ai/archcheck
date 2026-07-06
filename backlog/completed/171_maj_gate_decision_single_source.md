# [RULES] Consolidate the two gate-decision paths (check vs --diff)

**Created:** 2026-07-03
**Start date:** —
**Status:** new
**Module:** RULES
**Priority:** maj
**Difficulty:** unknown
**Blocks:** —
**Blocked by:** —
**Related:** #169 (openwiki — surfaced in rules/gate_policy.md), #133

## Goal

Give "what gates the build" a single source of truth, or an explicitly documented and
tested contract for the intended check-vs-`--diff` divergence, so the two paths can't
silently drift apart.

## Context

Surfaced while documenting `gate_policy.md` for the openwiki (#169). There are **two
independent gate-decision mechanisms**, not one:

- `src/rules/gate_policy.cpp` — `classifyForGate` / `isGating` / `countGating`, driven
  by `GateMode::Check` / `GateMode::Drift`. Used by plain `check` and
  `check --graph-baseline` (`src/cli/check_command.cpp`).
- `RegressionReport::gates()` — `include/archcheck/diff/regression_report.h:80`,
  computed in `src/diff/regression_report.cpp`, gating on `grownCycles` /
  `newGodHeaders`. Used only by `--diff <revspec>`
  (`src/cli/diff_command.cpp:354,362,478`).

They **disagree on god-headers**: `GateMode::Drift` never gates on god-headers (there
is no such rule id in `makeDriftRuleSet`), while `--diff` gates on *new* god-headers.
This divergence is currently **intentional and known** — the in-code comment at
`src/cli/check_command.cpp:96` says: "This mirrors the narrow-gate trust model, but not
the exact --diff gate: diff also gates new god-headers. #133."

The risk is not a bug today, it is **drift between two hand-kept gate definitions**:
a future change to one path (add/remove a gating rule) can silently desync the other,
and there is no single place or test that pins the intended contract.

## Execution plan

- [ ] Decide the target: (a) one gate-decision function both paths call, parameterized
      by mode; or (b) keep two paths but add a single documented contract + a test that
      pins exactly which findings gate in `Check` / `Drift` / `--diff`.
- [ ] If (a): route `RegressionReport::gates()` and `countGating` through one source of
      truth; keep the deliberate `--diff` new-god-header gate as an explicit mode flag.
- [ ] Add a test that fails if the two paths' gating sets diverge in any way other than
      the documented `--diff` god-header rule.
- [ ] Update `docs/openwiki/rules/gate_policy.md` to state the resolved contract; bump
      `last_checked_commit`.

## Done
- (empty)

## In progress
- (empty)

## Next steps
1. Read both paths end-to-end, write down the exact current gating set per mode, then
   choose (a) unify or (b) contract+test.

## Key decisions

| Decision | Reason |
|---------|---------|
| Treat as design consolidation, not a bug | The check-vs-`--diff` god-header divergence is deliberate (#133); the problem is two un-pinned definitions that can drift. |

## Changed files

| File | Change |
|------|-----------|
| ... | ... |

## Fixtures (if a rule)

- [ ] test pinning the gating set for Check / Drift / --diff modes
