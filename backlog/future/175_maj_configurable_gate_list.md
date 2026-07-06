# [RULES][CONFIG] Configurable gate list: let a project promote advisory checks to gating

**Created:** 2026-07-06
**Start date:** —
**Status:** new
**Module:** RULES][CONFIG
**Priority:** maj
**Difficulty:** unknown
**Target release:** v0.2+
**Blocks:** —
**Blocked by:** — (#171 done 2026-07-06, resolved as pinned contract + test, not unification — see below)
**Related:** #168 (unresolved_local_include_gate — the existing single-rule precedent, `--fail-on-unresolved-local`), #005 (sarif_reporter_spec)

## Goal

Let a project opt specific advisory findings (`DRIFT.NEW_CLONE`, `DRIFT.LOCAL_COMPLEXITY`,
`SF.7`, `SF.8`, `Lakos.GodHeader`, `Lakos.ChainLength`, ...) into gating, via `.archcheck.yml`
or a CLI flag, instead of the fixed default gate set (cycles + god-headers in `--diff`;
`SF.9` + `CASE_MISMATCH_INCLUDE` in `check`).

## Context

`archcheck` already has one instance of this pattern: `--fail-on-unresolved-local` (#168)
promotes `UNRESOLVED_LOCAL_INCLUDE` from advisory to gating. That flag is a one-off,
hand-written for a single rule. There is no general mechanism.

Motivating case (manual spot-check, 2026-07-06, `ImagingTools/Agentino`,
see `backlog/wip/173_maj_agentic_weak_review_corpus.md` and the archcheck-journal entry
for that date): `archcheck --diff` correctly found real copy-paste (`DRIFT.NEW_CLONE`) and a
complexity spike (`DRIFT.LOCAL_COMPLEXITY`, 4→30) in an agent-authored commit
(`3594827`, "Fix macOS service stop handling"). None of it gated the build — `gate: ok`
across all 438 commits in that repo's history — because clone/complexity checks are
advisory-only with no opt-in path. A project that has already paid down its clone/complexity
debt and wants to hold the line has no way to do that today short of a custom CI script that
parses archcheck's text/JSON output itself.

**#171 is done (2026-07-06).** It kept the two gate-decision mechanisms as-is
(`gate_policy.cpp::classifyForGate` for `check`, `RegressionReport::gates()` for `--diff`
— unifying them would mean threading rule-id strings through a typed graph-diff struct
for no real gain) but pinned the one intentional divergence (god-headers) with a
cross-referenced test pair (`tests/unit/rules/gate_policy_test.cpp` ×
`tests/unit/diff/regression_report_test.cpp`, grep `#171`). This task can now build on
top of that pinned contract instead of two silently-drifting internals.

## Execution plan

- [x] #171 landed as a pinned documented contract + test (not unification — see above).
- [ ] Decide the config shape: likely `.archcheck.yml` `gate: [SF.9, DRIFT.NEW_CLONE, ...]`
      (explicit allowlist of rule ids to gate, on top of the always-gating core) — check
      `docs/config_format.md` for the established `rules:`/`thresholds:` style and follow it,
      not a new ad hoc shape.
- [ ] Decide whether `--fail-on-unresolved-local` becomes sugar for
      `gate: [UNRESOLVED_LOCAL_INCLUDE]` (backward compatible) or is deprecated in favor of
      the general mechanism — avoid maintaining both long-term.
- [ ] Decide default behavior when a promoted-to-gating advisory check has known FP risk
      (clone detection especially, per `docs/duplication_architecture.md`) — likely requires
      `--baseline`/`--drift-baseline` to already be in use, so a team doesn't gate on
      pre-existing debt from day one.
- [ ] Update `docs/config_format.md` (SSOT) and `--help` text.
- [ ] Fixtures: a project config with an extended gate list + a fixture PR/diff that only
      trips the newly-gated rule, verifying non-zero exit.

## Done

- (empty)

## In progress

- (empty)

## Next steps

1. Wait for #171.

## Key decisions

| Decision | Reason |
|---------|---------|
| `future/`, target v0.2+ | Depends on #171 (unscheduled); general config-driven rule enforcement is already phase-2 territory per `docs/config_format.md` ("Enforcement status"). |
| Priority `maj`, not `min` | Ties directly to the project's core value proposition (CI enforcement) and is motivated by a concrete finding, not speculative — but not urgent enough to jump ahead of #171. |

## Changed files

| File | Change |
|------|-----------|
| ... | ... |

## Fixtures (if a rule)

- [ ] `fixtures/gate_config/pass/` — extended gate list, no violation of the promoted rule
- [ ] `fixtures/gate_config/fail_promoted_clone/` — promoted `DRIFT.NEW_CLONE` trips the gate
