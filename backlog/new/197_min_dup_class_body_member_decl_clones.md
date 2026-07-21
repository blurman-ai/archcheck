# [DUP] 197: clones of member-function runs in a class body are not detected

**Created:** 2026-07-21
**Start date:** —
**Status:** new
**Module:** SCAN/DUPLICATION
**Priority:** minor
**Difficulty:** unknown
**Blocks:** —
**Blocked by:** —
**Related:** #190 (brace-descent), #191 (statement-run), #195 (boundary-run), #196 (trailing-qualifier bodies)

## Goal

Detect copy-paste that is a run of **member-function declarations/definitions inside a
class body** — i.e. several small sibling methods copied between two classes — which the
current fragmenter misses because it operates on *function bodies*, not on runs of
members inside a `class { … }` scope.

## Evidence (hand-read, 2026-07-21)

Surfaced classifying the 6 Duplo-uncovered blocks on duckdb `src/execution` (#191
acceptance). One residual real miss:

`operator/join/physical_nested_loop_join.cpp:223-236` ↔
`operator/join/physical_piecewise_merge_join.cpp:129-142`

A local-state class copied between the two join operators: `ResetState(...)` +
`SupportsReuse()` + the head of `Reset(...)`, byte-identical except the `Cast<…>` type
(`NestedLoopJoinGlobalState` vs `MergeJoinGlobalState`). It is a real, refactorable clone.

Why it is missed: these are member declarations/definitions sitting directly in a
`class { … }` body. #190 descends into function *bodies*; #191 statement-runs fire inside a
body; #195 boundary-runs anchor to a body's edges. None emits a fragment for a run of
*members* at class scope.

## Not in scope / cautions

- This is a **minor** recall edge, not a correctness bug. Do not build it if it inflates
  candidate pairs or FPs — class bodies are full of near-identical tiny getters/overrides
  (`SupportsReuse() { return true; }` appears everywhere), exactly the idiom-flood risk.
- Measure precision on the corpus (baseline diff, `lost=0`) before keeping, same gate as
  #191. A cheap first cut: only emit a class-scope run when it spans ≥ `min_tokens` and
  contains at least one non-trivial method body, not just declarations.

## Acceptance criteria

- [ ] The witness above is reported.
- [ ] duckdb `src/execution`: `lost = 0`; Duplo coverage does not regress.
- [ ] Hand-triage new pairs; report TP/FP with sample size (watch the getter/override idiom flood).
- [ ] Fixtures `fixtures/duplication/class_body_member_clone/{pass,fail_*}/`.
- [ ] `docs/duplication_architecture.md` updated.
