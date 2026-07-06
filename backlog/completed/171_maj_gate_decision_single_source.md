# [RULES] Consolidate the two gate-decision paths (check vs --diff)

**Created:** 2026-07-03
**Start date:** 2026-07-06
**Status:** done
**Module:** RULES
**Priority:** maj
**Difficulty:** unknown
**Blocks:** #175 (configurable_gate_list)
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

- [x] Decide the target: (a) one gate-decision function both paths call, parameterized
      by mode; or (b) keep two paths but add a single documented contract + a test that
      pins exactly which findings gate in `Check` / `Drift` / `--diff`.
- [x] ~~If (a)~~ — not applicable, chose (b).
- [x] Add a test that fails if the two paths' gating sets diverge in any way other than
      the documented `--diff` god-header rule.
- [x] Update `docs/openwiki/index.md` (the actual location of the gate-policy section;
      "rules/gate_policy.md" in the original plan doesn't exist — the wiki is one page,
      see #169) to state the resolved contract; bumped `last_checked_commit`.

## Done
- Chose option (b): keep the two mechanisms, pin the contract with a cross-referenced
  test pair instead of merging them.
- `tests/unit/rules/gate_policy_test.cpp`: new test case pinning that Check and Drift
  mode never gate `Lakos.GodHeader` (the one side of the documented divergence
  expressible via rule-id strings).
- `tests/unit/diff/regression_report_test.cpp`: one-line comment on the existing
  `REQUIRE(r.gates())` in the new-god-header test, cross-referencing the paired
  assertion above (grep `#171` to find both halves).
- `docs/openwiki/index.md`: gate-policy warning box rewritten to state the divergence
  is resolved (pinned by tests, not eliminated), points at both test files;
  `last_checked_commit` bumped to `850911f`.
- Full build + `ctest` (634/634 passed) + self-dogfood (`archcheck src/ include/
  tests/` → 0 violations) + `lizard` (clean) + `check_openwiki.py` (0 errors/warnings).

## In progress
- (empty)

## Next steps
- (empty — see #175, blocked by this task, for the follow-on user-facing feature)

## Key decisions

| Decision | Reason |
|---------|---------|
| Treat as design consolidation, not a bug | The check-vs-`--diff` god-header divergence is deliberate (#133); the problem is two un-pinned definitions that can drift. |
| Chose (b): two paths + cross-referenced pinning tests, not (a) unification | `RegressionReport` is a typed graph-diff struct by design (no rule-id strings); threading rule ids through it to share `classifyForGate` would mean a moderate refactor across `diff_command.cpp`/reporters for no behavioral gain over a test that already pins the one real divergence. Every `RegressionReport::gates()` field contribution (grownCycles, newGodHeaders, addedEdges, chainLengthGrown, newCrossAreaDependencies, nccdDelta) was already individually pinned in `regression_report_test.cpp` before this task — the only genuinely missing pin was the Check/Drift side of the god-header contrast, which is what the new test adds. |

## Changed files

| File | Change |
|------|-----------|
| `tests/unit/rules/gate_policy_test.cpp` | new pinning test case (+11 lines incl. comment) |
| `tests/unit/diff/regression_report_test.cpp` | 3-line cross-reference comment on existing `REQUIRE(r.gates())` |
| `docs/openwiki/index.md` | gate-policy section rewritten; `last_checked_commit` bumped |
| `backlog/new/171_maj_gate_decision_single_source.md` → `backlog/completed/` | this task |
| `backlog/new/171...` → `backlog/wip/` → `backlog/completed/` | status transitions |

## Fixtures (if a rule)

- Not a rule — no `fixtures/` directory needed; the pinning is unit-test-level
  (`tests/unit/rules/gate_policy_test.cpp`, `tests/unit/diff/regression_report_test.cpp`).
