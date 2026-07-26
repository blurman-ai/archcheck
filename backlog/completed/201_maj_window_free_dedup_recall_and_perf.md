# [SCAN] Window-free core: recall gap on repeated code + O(n²) perf blow-up (one root)

**Created:** 2026-07-25
**Start date:** 2026-07-25
**Completed:** 2026-07-26
**Status:** done

## Resolution (2026-07-26)

The premise (dedup/tandem/clique over-collapse) was mostly wrong. The repeated-block recall gap
was **two lexer correctness bugs**; the perf blow-up was **O(n²) in three stages**, unrelated to
clique count. Author accepted the self-similar-codegen/table clique tail as the same noise band
as the data tables (region flagged is enough; not every pairwise instance chased).

**Recall** (openvdb #198 harness, min-lines 8, no-cf): **95% → 99%**; gladius 97%. Clean-code
witnesses fixed (Ray.h, ParmFactory). Residual = noise band (data tables, generated stubs, dead
`#if 0`, tandem twins, self-similar codegen).

**Perf** (full openvdb, both modes): **1h58m → 2m05s**, output identical.

### How it works
- **Lexer (`token_normalizer.cpp`):** `skipDeadIfBlock` stops the dead-branch skip at a
  top-level `#else`/`#elif` (live alternative is lexed). `tryConsumeQuoted` counts a
  backslash-newline continuation so later token lines don't drift.
- **Perf (`token_run_matcher.cpp`), all result-preserving:** `dedupRuns` uses a per-diagonal
  interval map (predecessor containment test); `stitchSpans` searches only the compatible
  diagonal band (`findStitchHost`); `dedupSpans` buckets by (fileA,fileB) and runs after the
  idiom/structureless drops (a rare-poor / cf-free span can't contain a rare-rich / cf one).

### Changed files
- `src/scan/duplication/token_normalizer.cpp` — two lexer fixes + `isElseOrElif`.
- `src/scan/duplication/token_run_matcher.cpp` — three O(n²)→O(n log n) index fixes; filter reorder.
- `tests/duplication_token_normalizer_test.cpp` — `#if 0/#else` + string-continuation regressions.
- `tests/duplication_token_run_matcher_test.cpp` — 3× repeated-block clique coverage test.

### Left open (deliberately, author call)
Bounded clique canonicalization for the self-similar codegen tail (gladius `ToCommandStreamVisitor`
648-680 covered by zero pairs) — declined as the same noise band as periodic data tables.
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

## Progress (2026-07-25)

**The premise was two problems, not one. The flagship "ZERO pairs" witness (Ray.h) is NOT the
dedup/clique hypothesis — it is a lexer bug.** Instrumented the run core on Ray.h: the region
403-423↔435-455 had **zero tokens in the stream**, so no run was ever generated (candidate
gen, not dedup). Root: `skipDeadIfBlock` in `token_normalizer.cpp` tracked only `#if`/`#endif`
depth and **ignored `#else`/`#elif`**, so `#if 0 … #else … #endif` swallowed the *live* else
branch. Ray.h's two `intersects()` loops live in exactly such an `#else` (`#if 0`@334 /
`#else`@390 / `#endif`@457). Duplo does not preprocess, so it saw them → counted as archcheck
misses.

- **FIXED** (`skipDeadIfBlock` now stops the dead-branch skip at a top-level `#else`/`#elif`;
  nested `#if…#endif` in the dead branch still fully skipped). Ray.h now reports
  `400-429 ↔ 432-468 (MIXED 98.8%)`, covering the witness. Unit test added
  (`duplication_token_normalizer_test.cpp`, "#if 0 skips dead branch but keeps the #else").
  All 666 ctest pass; dogfood 0 violations.
- Corpus-wide correctness bug (any `#if 0/#else`), not clone-specific — also fixes IO.h's
  active side (its other endpoint is legit dead `#if 0` code → remains a correct Duplo-noise
  drop).

**Second lexer bug found — the ParmFactory witness was ALSO not the clique hypothesis.**
Traced `ParmFactory.cc:1163-1183 ↔ 1218-1238` (a clean EXACT copy): the clone WAS being
detected, but reported at *drifted* line numbers, so vs_duplo (line-based) couldn't match it
to Duplo → false "miss". Root: `tryConsumeQuoted` skipped a backslash-newline line
continuation inside a string (`"…\n\`↵) with `i += 2` **without incrementing `line`**.
ParmFactory's ~18-line continued help string dropped the counter ~47 lines by mid-file
(`mTable` at true src 1265 was labelled L1218). `file:line` is the product contract.

- **FIXED** (`token_normalizer.cpp`: count the escaped newline). ParmFactory now reports
  `1163-1183 ↔ 1218-1238` EXACT — the exact Duplo witness. Unit test added. 668 ctest pass,
  dogfood 0.
- Corpus-wide `file:line` correctness bug for any string with backslash-line-continuation
  (help text, SQL, embedded shaders) — affects reported clone lines everywhere, not just #201.

**So BOTH flagship "repeated-code recall" witnesses (Ray.h, ParmFactory) were lexer
correctness bugs, NOT the dedup/tandem/clique over-collapse the task hypothesized.** Re-running
the #198 openvdb harness with both fixes to see what (if any) genuine clique recall gap remains.

**Half B — perf blow-up — still to verify/bound.** Confirmed distinct cause:
- `QuantizedUnitVec.cc` (no `#if 0` anywhere) is a **periodic float lookup table** — all `lit`
  tokens → huge self-similar run clique. Single-file it is fast + 0 pairs (data-table pairs
  fall under `--min-lines`); the **117k-span / perf blow-up is a whole-repo phenomenon**, not
  single-file.
- `ParmFactory.cc`, `PoissonSolver.h`, gladius codegen, SOP_* houdini repeats: repeated-block
  cliques away from any `#if 0` → genuinely the dedup/tandem/extension-cap over-collapse.
- Direction (bounded clique canonicalization + candidate-gen cost bound) unchanged from the
  task; needs pinning (esp. whether periodic *data tables* are a clique to canonicalize or
  Duplo noise the cf-default already drops — matters for the acceptance list).

**Recall — essentially closed by the two lexer fixes.** openvdb #198 harness (min-lines 8,
no-cf): **897/943 (95%) → 934/943 (99%)**. The 9 residual misses: 5 = QuantizedUnitVec float
data table (noise, not chased per author), 1 = IO.h copy in a dead `#if 0` branch (correct
drop), 3 = small 8–10-line blocks at the min-lines boundary (Ray.h 480-487, PoissonSolver
795-802, SOP_* cross-file). The hypothesized dedup/tandem/clique over-collapse was NOT the
recall cause.

**Perf — the real remaining problem, and it is O(n²) in two stages, not clique count.**
Profiled openvdb/openvdb/math (27 files, 59k runs / 22k spans): `dedupRuns` 5.3s + `stitchSpans`
6.6s dominated (~13s total). Both only ever match on the same/near diagonal (`a-b`), so indexed
by diagonal:
- `dedupRuns`: linear scan → predecessor lookup in a per-diagonal interval map. **5271ms → ~80ms**, result identical.
- `stitchSpans`: full reverse scan → diagonal-band search (`canStitch` forces host diagonal within `gap+k`). **6624ms → ~720ms**, result identical (still picks highest-index stitchable span; 3310 pairs unchanged).
- math subdir: **13s → 2.5s**. `dedupSpans` (910ms, also O(n²)) now the largest stage — measuring full openvdb to decide if it needs the same treatment.

**Perf — DONE, result-preserving.** Full openvdb both modes: **1h58m → 2m05s**. Three O(n²)
stages diagonally/file-pair indexed, each keeping the identical output:
- `dedupRuns`: per-diagonal interval map (predecessor test). 5271ms → ~80ms.
- `stitchSpans`: diagonal-band host search (`findStitchHost`). 6624ms → ~720ms.
- `dedupSpans`: file-pair bucketing + moved AFTER the idiom/structureless drops (a rare-poor /
  cf-free span can't contain a rare-rich / cf span, so the final set is identical, and default
  mode sheds the data-table flood before dedup). 444824ms → ~8-10s.
Pair counts identical before/after (openvdb 117357 no-cf / 87488 cf; math subdir 3310) — the
refactor changed cost, not results. 669 ctest pass, dogfood 0, lizard clean, clang-format clean.

**Acceptance status:**
- Ray.h ✓ (lexer #if0/#else), ParmFactory ✓ (lexer string-continuation) — the clean-code witnesses.
- QuantizedUnitVec — periodic float table, author-classed noise (not chased).
- gladius `ToCommandStreamVisitor` — 120 near-identical `out << "..."` codegen blocks; the region
  IS flagged (`775-1010↔1300-1541` MIXED 0.97), but earliest instance **648-680 is covered by
  ZERO pairs** → the residual self-similar-clique gap (the task's original hypothesis, now
  isolated to genuinely self-similar codegen/tables). The other ~8 residual misses per repo:
  generated Stub headers (interface-signature idiom), dead `#if 0` copies, tandem twins.
- Fixture: inline clique test `duplication_token_run_matcher_test.cpp` (3× block, each instance
  covered, ≤3 pairs) — matches the window-free test style (fixture-equivalent).

**OPEN (author decision):** whether to build bounded clique canonicalization for the
self-similar codegen/clique tail (the gladius 648-680 class), or accept "region flagged, not
every pairwise instance covered" — it is the same noise band as the data tables. Recall on
CLEAN code is fully restored (lexer fixes); perf is fixed. This is the one genuine product call
left. Then: strip profiling done, final commit (holds all fixes per author).

## Notes

- This is the gate on #199's default flip: the window-free core is otherwise stronger than the
  window core (Type-2/cross-function/monotonic min-size) and covers unique clones ≥ Duplo, but
  the repeated-block gap must close first.
- Do NOT "fix" it by removing the dedup/tandem guards wholesale — they exist for real reasons
  (tandem twins, self-overlap). The fix is a bounded clique canonicalization, not their removal.
