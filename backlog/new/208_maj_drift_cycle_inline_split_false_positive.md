# [RULES][GRAPH][DIFF] DRIFT.2 and the --diff gate report a header+inline-impl pair as a new cycle

**Created:** 2026-10-22
**Start date:** —
**Status:** new
**Module:** RULES / GRAPH / DIFF
**Priority:** major
**Difficulty:** unknown
**Blocks:** —
**Blocked by:** —
**Related:** #088 (inline-split filter, SF.9 only), #126 (collapse header+impl into one component — would subsume this fix if done at graph level), #202 (GATE-1 reworks `grownSccs` matching)

## Goal

A new `foo.h` ↔ `foo.tpp` / `.inl` / `.ipp` / `.tcc` pair must not gate in drift mode or in `--diff`, the same way SF.9 already skips it.

## Context

The inline-split filter from #088 is `isInlineSplitScc`, a file-local function in
[src/rules/sf9_no_cycles.cpp:91](../../src/rules/sf9_no_cycles.cpp#L91). Only SF.9 calls it.
Both other cycle consumers go through `graph::grownSccs` ([src/graph/diff.cpp:144](../../src/graph/diff.cpp#L144)) unfiltered:

- DRIFT.2 — [src/rules/drift_no_cycle_growth.cpp:16](../../src/rules/drift_no_cycle_growth.cpp#L16), gating in `--drift-baseline`;
- `--diff` gate — [src/diff/regression_report.cpp:136](../../src/diff/regression_report.cpp#L136) (`collectEdgesAndCycles` → `grownCycles`).

Reproduced on a private Bazel project (~3300 C++ files), archcheck 0.1.8:

- plain check: `sf9_cycles: 0`, `sccs_cyclic: 1 (raw; includes conditional/inl-split)`, exit 0;
- `--drift-baseline <empty graph>`: `foo.h: [DRIFT.2] new cycle (2 members): foo.h foo.tpp`, exit 1.

`foo.h` includes `foo.tpp` at its end and the `.tpp` includes the header back — the standard template-impl split. In a per-commit `--diff` run over that project's history it fired on the commit that introduced the pair — the only gating cycle event in 3227 commits.

## Execution plan

- [ ] Move `isInlineSplitScc` (with `componentStem` / `baseName` / `dirName` it needs) out of `sf9_no_cycles.cpp` into a shared header next to `grownSccs` (graph layer); SF.9 keeps calling it.
- [ ] Apply the filter to grown SCCs: either inside `grownSccs` or in both consumers. Pick one place so the two gates cannot diverge again.
- [ ] Fixture `fixtures/drift_cycle_growth/pass_inline_split/` (baseline without the pair, current with `foo.h` ↔ `foo.tpp`) → no DRIFT.2.
- [ ] `--diff` integration test: commit that adds a `.h` + `.tpp` pair → gate ok.
- [ ] Keep `fail_new_cycle` failing; add a 3-node cycle through a `.tpp` to confirm only the strict 2-node same-stem form is skipped.

## Done

- (empty)

## In progress

- (empty)

## Next steps

1. Decide where the filter lives, coordinating with #202 GATE-1 (same function).

## Key decisions

| Decision | Reason |
|---------|---------|
| ... | ... |

## Changed files

| File | Change |
|------|-----------|
| ... | ... |

## Fixtures (if a rule)

- [ ] `fixtures/drift_cycle_growth/pass_inline_split/`
- [ ] `fixtures/drift_cycle_growth/fail_*/` (existing `fail_new_cycle` stays red)
