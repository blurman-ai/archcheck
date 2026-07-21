# [DUP] 191: statement-run clones — a copied span that is not a braced block

**Created:** 2026-07-18
**Start date:** 2026-07-20
**Status:** wip
**Module:** SCAN/DUPLICATION
**Priority:** major
**Difficulty:** large
**Blocks:** closing the sub-function recall gap beyond brace-delimited blocks
**Blocked by:** —
**Related:** #190 (brace-descent, done), #195 (boundary prefix/suffix runs — **complementary, NOT subsumed**; A/B 2026-07-21 below), #196 (trailing-qualifier bodies, split out), #056 (token pass), #092 (winnowing fingerprints), #091

## Goal

Detect copy-paste whose copied region is **not a single braced block**: a bare run of
consecutive statements, a span covering several sibling blocks and the code between
them, or a chunk of one multi-line call's argument list. This is the general case; #190
covers one `{…}` body, #195 covers a run anchored to a function's start or end. A run
that sits **in the middle** of two otherwise-different functions is reached by neither.

## Why this exists (evidence, not speculation)

#190 made the fragmenter descend into function bodies, so a copied `if`/`for`/`while`/
`switch` body is now a fragment. #195 added bounded prefix/suffix windows for shared
prologues/epilogues. Both are anchored: #190 to a brace pair, #195 to `bodyLo`/`bodyHi`.
A copied run that is neither a single block nor flush against a function boundary is
still invisible.

### Primary witness — JiraClient (offset middle run, the case that reopened #195)

`Smatchet@f8368a23 Source_Core/src/JiraClient.cpp:587↔884`
(`FetchUsers` / `FetchFieldCatalog`). Checkouts on disk:
- `/tmp/claude-1000/-home-localadm-projects-cpparch/8a69622c-2ca3-4219-9803-757598851218/scratchpad/inputs/Smatchet/Source_Core/src/JiraClient.cpp` (exact line numbers below match this copy)
- `/home/localadm/oss/alexandrosk0_Smatchet/Source/Core/src/Tracker/JiraClient.cpp` (fresh OSS clone)

Shape: **14 lines byte-identical**, `JiraClient.cpp:590-603 == 887-900` (verified by
`diff`): the `if (cfg.ApiToken.empty() || cfg.Domain.empty()) {…}` guard, three
`const std::string base/basicCred/authHeader = …`, the `cpr::Header headers{…}`
initializer, and `cpr::Redirect redirect(true, true);`.

Why every existing layer misses it:
- It is an **offset middle run**: preceded by *different* `outX.clear()` lines
  (`outUsers` vs `outFields`+`outComponents`) and followed by *different* request code,
  so it is flush with neither `bodyLo` nor `bodyHi` → #195 boundary windows never align.
- It is **not one braced block** (it spans an `if{}` + loose statements + a `{}`
  initializer) → #190 brace-descent never emits it as a unit.
- The two functions are **large** (`FetchUsers` 584-878, `FetchFieldCatalog` 879-1151,
  ~290 lines each) and share only ~14 → whole-body similarity is far below threshold.

**Decisive isolation experiment (run 2026-07-20).** Extracted just the two functions
into separate files (there the guard repeats only 2×, so no file-wide boilerplate /
frequency suppression can be blamed) and ran `archcheck --duplication` →
**0 pairs reported**. This rules out the frequency guard and proves the miss is a
**fragmentation** gap: there is no statement-run granularity, so the copied span never
becomes a comparable `Fragment`. (Full-file run reports 5 other pairs, none at 587/884.)

### Earlier witnesses (duckdb / monit, from #190 hand-read)

| Witness | Shape |
|---|---|
| duckdb `operator/helper/physical_limit.cpp:246` ↔ `physical_streaming_limit.cpp:72` | a whole `ParamsToString` body — three sibling `if` blocks with statements interleaved |
| duckdb `operator/join/physical_iejoin.cpp:2073` ↔ `2098` | a bare run of statements starting mid-function |
| monit `http/cervlet.c:1357` ↔ `1484` | 19 verbatim lines of consecutive `out_print(...)` calls |
| monit `http/cervlet.c:832` ↔ `944` | a chunk of one call's multi-line argument list |

The `physical_limit` one is the most damning of the old set: an entire duplicated method
body, missed only because the body is a sequence of siblings rather than one block.

## Design

The fix is **statement-run fragments**: emit, in addition to bodies (#190) and boundary
windows (#195), fragments for contiguous runs of top-level statements starting at *every*
statement boundary, not just the two ends. This is the generalization of #195's
`emitBoundaryRuns` to interior offsets.

### Recommended path (consistent with #190/#195 — reuse the fragment pipeline)

Pin points in the current code (`src/scan/duplication/fragmenter.cpp`):

1. Add `emitStatementRuns(ctx, bodyLo, bodyHi)` next to `emitBoundaryRuns` (called from
   `emitBodyAndDescend`). Slide the window **start** across top-level statement
   boundaries in `[bodyLo, bodyHi)`; at each start emit windows of ~`minTokens`,
   `2*minTokens`, `4*minTokens` tokens, snapping both edges to statement boundaries.
2. Statement boundaries = top-level `;` (paren depth 0) and matched-brace edges. Reuse
   the paren/`;` logic already in `trackParenAndStatements` and the `ctx.match` array
   from `braceMatch` so a window never cuts mid-expression or mid-block.
3. Mark every emitted run `nested = true; boundary = true` (reuse the #195 invariant so
   these synthetic spans do **not** change df/IDF, the fingerprint frequency cap, or
   whole-file clone ratios — Open Question 2 below).
4. Gate behind `FragmentOptions::statementRuns` + `ScannerOptions::enableStatementRuns`
   (mirror `boundaryRuns`/`enableBoundaryRuns`), enabled by `scanForDuplication`.
5. Dedup: reuse `phaseOverlapDedup` and `phaseNestedContainment` (both extended in #195)
   so shifted/overlapping windows collapse to one strongest pair per copy site. Verify
   `phaseSpanBalance` (#195, drops pairs whose spans differ >2×) does not kill legit
   statement-run pairs.
6. Scoring: runs flow through `phase3ScoreCandidates` → `fillPairMetrics` →
   `boundaryAwareWeighted`. **Recheck the IDF-0 fallback**: #195's fallback fires only at
   `weighted == 0.0`, but a real-corpus statement run of common C++ tokens scores
   *small-but-nonzero*, so the fallback never triggers on JiraClient. Extend the plain-
   Jaccard fallback (or lower the floor) for `boundary && nested` runs — this is likely
   the concrete reason the JiraClient pair, even once emitted, would still miss.

Alternative (from the original #191 sketch, keep as fallback if fragment count explodes):
a winnowing-fingerprint sliding window (#092 `addFingerprintCandidates`) over the
normalized token stream, candidate spans from equal fingerprints extended maximally,
scored through the same pipeline. Heavier to wire, but decouples candidate generation
from the fragment count.

### Open questions — carried over, still live

1. **Containment / overlap.** `phaseNestedContainment` + `phaseOverlapDedup` already drop
   contained/overlapping pairs (#195). Statement-run spans are not brace-aligned, so this
   is partial overlap, not clean nesting — verify one copy site yields exactly one pair
   across the body/boundary/statement-run layers, not three.
2. **`Fragment::nested` semantics — main technical risk.** #190 established that a
   fragment whose text is already counted elsewhere must not count as a *document*
   (df/IDF, the fingerprint frequency cap, the whole-file ratio). Statement-run windows
   overlap heavily by construction; in #190 each of these three was a concrete regression,
   and one (the fingerprint cap) silenced a whole family of real clones and was invisible
   on a small subdirectory. Marking runs `nested=boundary=true` reuses the guard, but
   re-verify on a full-corpus run, not a subdir.
3. **Idiom flood + cost.** Windows hit far more boilerplate than bodies do, and all-pairs
   over windows is much larger. #190 already cost +75% wall / +47% RSS on duckdb; budget
   before building and state the delta.

## Acceptance criteria

- [x] **JiraClient `587↔884` is reported** (primary acceptance — the case that reopened #195).
      Full file: `JiraClient.cpp:587-605 ↔ 883-900` (STRUCTURAL, weighted=0.732, line=0.778),
      containing the `590-603 ↔ 887-900` run. Also surfaces the same auth prologue copied
      into `UpdateIssueFields` (667) and `FetchIssues` (1237) — all real.
- [~] The 4 duckdb/monit witnesses: **iejoin `2073↔2098` reported** ✓. **ParamsToString**
      (`physical_limit ↔ physical_streaming_limit`) is NOT a statement-run case — the task
      table mis-diagnosed it; the real cause is the `) const`-before-`{` heuristic gap →
      split to **#196** (prototype reports it, but ripples the IDF baseline, so it needs its
      own precision pass). monit `cervlet.c` is a C file not in the corpus — not re-measured.
- [x] Duplo coverage on duckdb `src/execution` **17/24 → 18/24** (oracle re-run 2026-07-21).
      Duplo `v1.0.1` (`g++ -std=c++2a -O2 -Isrc/include src/*.cpp`), `-ml 8 -ip`, on current
      duckdb HEAD `fd552a8c8` → 24 blocks. Paired design on the **same** checkout: pre-#191
      binary (committed HEAD, 154 pairs) = **17/24** (reproduces the #190 baseline exactly);
      post-#191 (183 pairs, +29) = **18/24**. Clean **+1, zero coverage regression** — the one
      flip is block #3 `join_hashtable dict_state` (MISS→COVERED via the statement-run pair
      `1269-1284 ↔ 1340-1356`, STRUCTURAL weighted=0.484, absent pre-#191). The 6 residual
      misses are all explained and none is a #191 defect (each read by eye, both sides,
      2026-07-21): **4 are `ParamsToString() const`** (`) const`-before-`{` trailing-qualifier
      gap → **#196**: hash/perfecthash/partitioned/streaming_window/streaming_limit — real
      byte-identical loops we currently fail to *parse* as bodies, not idioms we suppress);
      **1** is a real clone of a local-state class body (`NestedLoopJoin` ↔ `PiecewiseMergeJoin`
      `ResetState`/`SupportsReuse`/`Reset` declarations, outside #191's function-body scope);
      **1 (`comparison_join:141 ↔ hash_join:2552`) is a Duplo FALSE POSITIVE** — the matched
      `FlatVector::SetSize(mark_vector…)` text does not exist anywhere in `hash_join.cpp`
      (grep confirms), Duplo's `EndLineNumber="1"` EOF bug produced a bogus pairing. So real
      archcheck misses = **5** (4×#196 + 1 class-body); the 6th is oracle noise → effective
      recall **18/23**, not 18/24. #191's primary evidence remains the JiraClient witness,
      not the duckdb count.
- [x] duckdb `src/execution`: **`lost = 0`** (154 baseline pairs, all present by span-overlap;
      verified with a site-overlap comparator). Required a fix so a synthetic run pair never
      suppresses a real body pair by containment (`shouldDropContainedPair`).
- [x] Hand-triage of the 29 new pairs: 25 statement-run (boundary) + 4 non-boundary.
      TP examples verified by eye — JiraClient-class offset runs; `aggregate_hashtable ↔
      join_hashtable` `dict_state` setup (RENAMED, real); `csv_state_machine_cache`
      transition-table blocks (identical modulo the `CSVState` enum — real, refactorable
      copy-paste, reported as a dense clique). The one real FP class — `switch` type-dispatch
      cases (`Select<int8_t>`, `<int16_t>`, …) — is suppressed by the control-body skip
      (execute_comparison flood ~40 → 0 statement-run pairs).
- [x] Fixtures `fixtures/duplication/statement_run_clone/{pass,fail_offset_middle_run}/`.
      `fail_*` mirrors JiraClient (a shared session-setup run offset in the MIDDLE of two
      ~50-line divergent partly-renamed bodies). Test drives it with `maxTokens=120` (scale
      proxy for a >600-token body) and includes a **negative control**: with
      `enableStatementRuns=false` the pair vanishes — proving #191 is what reports it.
- [x] Wall / RSS on duckdb `src/execution` (205 files): **+85% wall (18s→33s), +40% RSS
      (215→300 MB)**, comparable to #190's own +75%/+47%.
- [x] Dedup verified: on JiraClient each function-pair sharing the prologue yields exactly
      one pair (3 functions → C(3,2)=3 distinct site-pairs, no body/boundary/run duplicates
      of the same pair).
- [x] `docs/duplication_architecture.md` §3.2 + §9 updated. **`enableBoundaryRuns` kept**
      (not folded): #195's fixed anchors fire for bodies ≤ maxTokens where #191 is gated OFF,
      so they are complementary, not redundant.

## Method note

The oracle harness is built and reusable: Duplo `v1.0.1` compiled with
`g++ -std=c++2a -O2 -Isrc/include src/*.cpp`, plus the join/filter script that dedupes
blocks and drops preprocessor-dominated ones. Do **not** try NiCad for C++ — it has no
C++ grammar and fails to parse 38–94% of files (measured, #190). For the JiraClient
witness verify directly: `archcheck --duplication <dir-with-JiraClient.cpp>` and grep for
`587` / `884` (or the `590-603 ↔ 887-900` span the run actually occupies).

## Key decisions

| Decision | Reason |
|---|---|
| Statement-run fragments, generalizing #195's `emitBoundaryRuns` to interior offsets | The JiraClient witness is an offset middle run; anchoring windows to `bodyLo`/`bodyHi` structurally cannot reach it. |
| Reuse the fragment pipeline, not a new detector | Scoring, classification, reporting, `DRIFT.NEW_CLONE`, baseline all reused unchanged; consistent with #190/#195. |
| Runs are `nested + boundary` | Keeps df/IDF / freq-cap / whole-file-ratio invariant (the #190/#195 rule). |
| **Emit only for function bodies `> max_tokens`**, from `scanRange` (not `emitBodyAndDescend`) | The JiraClient/iejoin functions exceed `max_tokens`, so `emitBodyAndDescend` never fires for them — the sketch's hook point would miss the primary witness. Bodies within `[min,max]` are already covered by #190 body + #195 boundary; interior runs there are pure idiom-flood/cost for no unique recall. |
| **Skip control bodies** (`if`/`for`/`while`/`switch`/`catch`) | Slicing a `switch`'s near-identical type-dispatch cases is a measured FP flood (duckdb `execute_comparison`, ~40 pairwise RENAMED clones). All witnesses are function-body-level runs. |
| **Drop same-file overlapping/adjacent pairs at candidate generation** (`clone_index`), not just in `phase7` | Overlapping windows within one function form an O(k²) same-function clique; dropping at candidacy is a pure cost win (identical result) and made the run tractable (duckdb went from >2 min timeout to 33 s). |
| **A synthetic run pair never suppresses a real body pair by containment** (`shouldDropContainedPair`) | A run window can be larger than a small body; without the guard the real pair was dropped as "contained in" a run pair that `phaseOverlapDedup` then discarded too → 3 lost baseline pairs. Restores `lost=0`. |
| `df.find` (not `df.at`) in the rare-token index | A token can live only in run fragments of an oversized body that was never emitted as a document → absent from `df`; absent = zero documents = maximally rare. `df.at` threw. |
| Extend the IDF-0 plain-Jaccard fallback | #195's `weighted==0.0` fallback never fires on real corpora (small-nonzero IDF); reused as-is via `boundary` flag — sufficient for the witnesses at the default gate, no lowering needed. |
| Precision gate = corpus baseline diff, not fixtures | Statement-run granularity is the classic FP-explosion source (PMD identifier-blind floods ~4×, #194); fixtures cannot catch that. |
| Trailing-qualifier method detection split to **#196** | It unblocks the ParamsToString witness but broadens #190 corpus-wide and drops one borderline baseline pair (fails `lost=0`); it is a `)`-before-`{` heuristic fix, orthogonal to statement runs. |

## How it works (as built)

Files touched (`src/scan/duplication/`):
- `fragmenter.{h,cpp}` — `FragmentOptions::statementRuns`; `statementStarts` (top-level
  boundaries, skipping matched-brace groups as units), `snapEnd`, `emitStatementRuns`
  (slide start × 3 token tiers, `nested+boundary`), `isControlBody` (skip if/for/while/
  switch/catch). Called from `scanRange` for `fnBody && body > maxTokens && !isControlBody`.
- `clone_index.cpp` — `sameFileOverlapOrAdjacent` guard in both candidate generators
  (`findCandidatePairs`, `pairFingerprintPostings`); `df.find` fallback.
- `duplication_scanner.{h,cpp}` — `ScannerOptions::enableStatementRuns` (default true,
  wired into `phase1`); `shouldDropContainedPair` synthetic-vs-real containment guard.

Reproduce the primary witness: `archcheck --duplication <dir-with-JiraClient.cpp>` →
grep `587` / `883`.

## A/B — #195 boundary-runs are load-bearing (do NOT fold)

Tested 2026-07-21 whether statement-runs (#191) make `enableBoundaryRuns` (#195)
redundant. Flipped `enableBoundaryRuns=false` (statement-runs on), re-ran on duckdb
`src/execution`:

- pairs **183 → 123** (net −52; 62 lost, 10 shifted-in);
- **the JiraClient witness is LOST** (`587-605 ↔ 883-900` gone) — it is carried by the
  boundary prefix window, not by an interior statement-run;
- lost pairs include many EXACT/RENAMED weighted-1 real clones.

Conclusion: boundary-runs (#195) are a **complementary, load-bearing layer**, NOT subsumed
by #191. The earlier "#191 supersedes #195" claim (incl. commit `104cde9`'s message) was
wrong; corrected here. `enableBoundaryRuns` stays on.
