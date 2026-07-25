# [SCAN] Window-free core: recall gap on repeated code + O(n²) perf blow-up (one root)

**Created:** 2026-07-25
**Status:** new
**Module:** SCAN
**Priority:** major
**Difficulty:** medium (dedup/candidate-generation rework + a bound on self-similar cliques)
**Blocks:** #199 stage-4 (default flip) — cannot flip until repeated-block recall matches Duplo
**Blocked by:** #199 (the window-free core this fixes)
**Related:** #199 (window-free core / ADR-004), #198 (Duplo comparison), #191 (the same
switch/self-similar clique flood that #191 guarded against in the window core)

## Problem

The window-free core (`token_run_matcher.cpp`, behind `--duplication --window-free`) has a
**real recall gap on repeated code**, found by the 9-repo Duplo comparison (#198 method):
aggregate coverage of Duplo was 96% (1768/1848), but that average hid the gap. Hand-verified
misses split into Duplo noise (includes, license headers, data tables, signatures — correctly
ignored) and **genuine misses of real verbatim/near-verbatim clones**, all in repeated /
self-similar regions:

- `nanovdb/math/Ray.h:403-423 ↔ 435-455` — a verbatim 21-line ray-box block with control flow.
  Decisive check: the region is covered by **ZERO** window-free pairs — lost entirely, not
  merely paired differently.
- `openvdb_houdini/ParmFactory.cc:1163-1183 ↔ 1218-1238` — a real 21-line logic copy.
- `QuantizedUnitVec.cc` — 5 misses at a regular ~127-line stride (a periodic block).
- gladius `ToCommandStreamVisitor.cpp` code-generator — the region IS flagged (an 846-token
  clone) but specific pairwise instances are dropped.

**Root cause (shared with the perf blow-up).** In a self-similar region (a block copied 3+
times, an unrolled emitter, a periodic table), the maximal-run set is huge and highly
overlapping. `dedupRuns` (same-diagonal containment), `dedupSpans` (both-sided containment),
the tandem guard (`canStitch`: a same-file span must not cross its own partner's start), and
the forward extension cap (`a+len <= b`) together **over-collapse the clique and drop real
distinct clone pairs** that line-based Duplo reports. The same self-similarity explodes span
count — openvdb produced **117,237 spans**; the 9-repo × 2-mode batch took **257 minutes**,
dominated by these regions.

So recall and perf are one problem: the core does not bound or canonicalize self-similar
cliques; it both drowns in them (cost) and mis-dedups them (recall).

## Goal

On repeated/self-similar code, window-free must not be **worse than Duplo**: every real
duplicated block Duplo reports must be covered by at least one window-free pair — while
keeping the span count (and wall-clock) bounded on the same regions.

## Direction (to be pinned)

- **Canonicalize cliques, don't drop them.** When N copies of a block exist, report a bounded,
  representative set (e.g. a clique anchored on the longest span + each instance covered once)
  instead of either O(N²) pairs (Duplo) or an over-collapsed subset (current). "Every instance
  covered by ≥1 pair" is the recall contract; O(N²) is not required.
- **Fix the drop, not just the count.** Audit `dedupRuns`/`dedupSpans`/tandem/extension-cap for
  the case where a real pair is discarded as "contained" on a *different* instance/diagonal.
  A run that covers a source region no other kept pair covers must survive.
- **Bound cost at candidate generation**, like #191 did for statement runs: cap per-region
  fan-out, collapse near-identical overlapping runs *before* scoring — but log what was
  collapsed (no silent truncation).

## Acceptance / fixtures

- Re-run the #198 Duplo harness (`experiments/window_free_spike/vs_duplo.py`, fair fileset +
  phantom guard) on the 9 repos; **every real-code miss above must become covered** (Ray.h,
  ParmFactory, QuantizedUnitVec, gladius), verified by the "region covered by ≥1 pair" check —
  not just an aggregate bump.
- Perf: openvdb span count and wall-clock brought to a sane bound (target: no single repo >
  a few minutes); record before/after.
- Fixture: `fixtures/dup_repeated_block/` — a block copied 3× in one file; assert each of the
  3 instances is covered by ≥1 reported pair, and the span count is bounded (not O(k²)).

## Notes

- This is the gate on #199's default flip: the window-free core is otherwise stronger than the
  window core (Type-2/cross-function/monotonic min-size) and covers unique clones ≥ Duplo, but
  the repeated-block gap must close first.
- Do NOT "fix" it by removing the dedup/tandem guards wholesale — they exist for real reasons
  (tandem twins, self-overlap). The fix is a bounded clique canonicalization, not their removal.
