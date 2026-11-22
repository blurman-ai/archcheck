# [SCAN] 207: a run shared in the INTERIOR of two different functions is not detected

**Created:** 2026-08-25
**Start date:** —
**Status:** new
**Module:** SCAN/DUPLICATION
**Priority:** major
**Difficulty:** unknown
**Blocks:** —
**Blocked by:** —
**Related:** #195 (cross-function partial clone — boundary runs, completed), #191 (statement-run
pass, completed), #190 (subfunction clone pass, wip), #194 (compare clone detectors)

## Goal

Detect a contiguous block copied between two otherwise-different functions when the block sits
**away from both function boundaries**. #195 closed the prologue/suffix shape with bounded
prefix/suffix windows; an interior run matches no window and is missed.

## Context

Found while cross-validating against Duplo on 20 pinned repos (#194). Witness:

`fmidev/smartmet-plugin-wms` — `wms/Layer.cpp:869-877` ↔ `wms/TimeLayer.cpp:164-172`

```cpp
std::shared_ptr<QueryServer::Query> originalGridQuery(new QueryServer::Query());
QueryServer::QueryConfigurator queryConfigurator;
T::AttributeList attributeList;

std::string producerName = gridEngine->getProducerName(*paraminfo.producer);

auto valid_time = getValidTime();
```

Nine lines, identical. The enclosing functions are `Layer::getGridValue` and
`TimeLayer::generate` — different everywhere else, so the whole-fragment comparison scores them
below threshold. The same block recurs at `IsobandLayer.cpp:430-446` ↔ `RasterLayer.cpp:535-545`,
a third file pair, so this is a repeated copy site and not a one-off.

**Why #195's layer does not catch it.** That task deliberately chose "a bounded boundary-run
layer, not the general #191 statement-run pass", and builds prefix/suffix spans of
`min_tokens`, `2*min_tokens`, `4*min_tokens` from each function body's edges. Here the block
starts ~6 lines into `getGridValue` and ~15 lines into `generate` — behind a timer setup, two
comment blocks and a `throw` guard. No prefix window on either side aligns with it.

**The detector is otherwise working on this repo**, so this is a shape gap, not a threshold
problem: `archcheck --duplication --min-lines 6` reports **607 pairs** there, including EXACT
ones in both files. `wms/TimeLayer.cpp:179-193` — the region immediately after the missed block
— IS reported, paired with three other files. Only the interior run itself is absent.

Duplo (line-based, no notion of function boundaries) reports it, which is the whole reason it
surfaced.

## Not in scope

Lowering thresholds. #195's closure records the risk plainly: sub-fragment matching explodes the
candidate-pair count, and #194 measured PMD's identifier-blind mode flooding output ~4x. The fix
must be a shape the index can carry, with a min-block guard, not a looser score.

Also not this task: copy-paste nested **within one** function body (#190), and runs of
declarations in a class body (#197, #208).

## Execution plan

- [ ] Decide the layer: sliding interior windows over emitted function bodies vs extending #191's
      statement-run pass to cross-function pairs. Record the A/B, the way #195 did.
- [ ] Fixture: two functions differing at both ends, sharing an interior run; plus a pass/ tree
      that must stay clean.
- [ ] Implement with a min-block guard; keep boundary fragments `nested` as #195 does so df/IDF,
      fingerprint caps and whole-file ratios are unaffected.
- [ ] A/B on a mid-size repo (duckdb `src/execution` was #195's bench): pairs added, pairs lost,
      wall-clock. The layer must be shown load-bearing, not assumed.
- [ ] Verify the witness above is reported, and that the third file pair is too.
- [ ] Update [docs/duplication_architecture.md](../../docs/duplication_architecture.md) with the
      layer's semantics and FP boundaries.

## Acceptance criteria

- [ ] `Layer.cpp:869-877 ↔ TimeLayer.cpp:164-172` reported, with a span covering the matched
      region (see #204 — the reported range must describe what actually matched)
- [ ] No more than a modest pair-count increase on the A/B repo; hand-check a sample of the new
      pairs before accepting them
- [ ] Fixtures pass; archcheck still clean on its own `src/`, `include/`, `tests/`

## Evidence

`experiments/clone_compare/` — `compare_duplo.py` (the run), `show_clone_deltas.py duplo_only`
(prints each finding with both code fragments), `repos.tsv` (the 20 pinned inputs).
Report: `archcheck-journal/reports/duplo_vs_archcheck.md`.
