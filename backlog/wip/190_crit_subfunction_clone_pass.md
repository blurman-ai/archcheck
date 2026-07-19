# [DUP] 190: sub-function clone pass — a copied block inside a function is invisible

**Created:** 2026-07-18
**Start date:** 2026-07-18
**Status:** wip
**Module:** SCAN/DUPLICATION
**Priority:** critical
**Difficulty:** large
**Blocks:** any outward claim about `--duplication` / `DRIFT.NEW_CLONE` recall
**Blocked by:** —
**Related:** #056 (token pass), #091 (maxTokens 600), #107/#132 (NiCad cross-validation — scope defect, see below), #147, #180

## Goal

Detect copy-paste **below function granularity**: a block copied from one function into
another. Today archcheck cannot see this class at all, and it is plausibly the most common
form of copy-paste in real code.

## The defect

`src/scan/duplication/fragmenter.cpp:215`:

```cpp
if (fnBody && body >= ctx.opts.minTokens && body <= ctx.opts.maxTokens)
{
  ctx.out.push_back(makeFragment(...));
  i = j + 1;          // <-- does NOT descend
  continue;
}
```

A function body that fits in `[30, 600]` tokens is emitted **whole** and the scanner never
looks inside it. Everything nested within is therefore never compared as a unit. The whole
similarity stage then compares only whole function bodies against each other.

Consequence: a copied block is reported only when it makes up a large share of **both**
containing bodies. Surround it with enough unrelated code and it disappears.

There is no sliding-window / suffix-based pass anywhere in the pipeline. The classic clone
detectors (CCFinder, NiCad `blocks`, SourcererCC, PMD-CPD) find any repeated token run of
length >= N regardless of function boundaries. archcheck has no equivalent.

## Reproduction (verified 2026-07-18)

Identical 5-line block, the only variable is how much unrelated code surrounds it:

```
variant 1 — block inside two LARGE functions
  scanned 1 files, 5 fragments
  reported 0 pairs                                      <-- MISS

variant 2 — same block, two SMALL functions
  scanned 1 files, 5 fragments
  reported 1 pairs
    small.cpp:6-13 <-> small.cpp:17-24 (RENAMED, weighted=0.8)
```

`5 fragments` in both runs: 5 functions -> 5 fragments. The nested `if (...) { ... }` never
became a fragment even once.

### Real-world instance

duckdb `src/parallel/pipeline_executor.cpp` @ `e884eef8` — a 5-line batch-index block
duplicated verbatim in the constructor (L23-28) and in `Reset()` (L79-84). Extracted into
`InitializeLocalSinkPartitionInfo()` in `a1b3ffc9`.

**Attribution corrected 2026-07-18/19:** `a1b3ffc9` is **not** an upstream duckdb commit.
It lives only on `origin/refactor-pipeline_executor` of Mike Semikov's fork; author and
committer fields both read `Mike Semikov <mikhail.semikov@gmail.com>` (unsigned — the
fields are self-reported), subject "ParselFire + Claude Sonnet 4.6 refactoring", and he
sent it himself as the change to evaluate. Almost certainly his own commit, but outward
text should not assert authorship it cannot prove: say "the refactoring you sent", never
"your commit" as a fact claim, and never an upstream fix.

**Provenance of the clone itself (traced 2026-07-18):** introduced by
`c28ce15a654f3c7f07f7fc0d320cfb1bc8f21ffe` — Denis Hirn, "Optimize recursive CTEs by
reusing executors and working buffers" (authored 2026-03-28, landed 2026-05-13). That
commit added `PipelineExecutor::Reset()` carrying a second copy of the batch-index block;
its parent `c518cf52` has exactly one occurrence of `RegisterNewBatchIndex()` in the file,
`c28ce15a` has two. The clone therefore lived in the tree ~6 weeks (2026-05-13 →
2026-06-23) across 3678 commits before being extracted.

- `--duplication` on baseline: **not reported**.
- `--diff e884eef8..a1b3ffc9`: **completely silent** — the clone removal did not register,
  because the clone was never seen.
- The one pair archcheck *did* report in that file (`GetData`/`Sink` debug scaffolding under
  `#ifdef`) is a should-not-collapse symmetric pair.

Score on that file: the tool found what must not be touched and missed what the commit
actually fixed.

## Why the NiCad cross-check did not catch this

`reports/nicad_vs_archcheck.md` (#132) ran:

```bash
./nicad3 functions c examples/monit-4.2
```

`functions` granularity — the same granularity archcheck already has. NiCad's `blocks`
granularity, which covers exactly this class, was never run. The report's headline
"Real recall bugs found in archcheck: 0" is true *within function granularity* and reads as
a general clean bill of health. This is a scope defect in the validation, not a wrong number.

**First step of this task is to rerun that cross-check with `./nicad3 blocks c` and get a
quantitative size for the gap.** That number is the acceptance baseline for everything below.

### DONE 2026-07-18 — the baseline number

`./nicad3 blocks c examples/monit-4.2` (minsize=10, threshold=0.3, blind rename):
1506 blocks, 35 clone pairs, 20 classes — against 437 functions / 27 pairs at
`functions` granularity.

Isolating the sub-function class (a `blocks` class with no `functions` counterpart):

| genuinely sub-function classes | archcheck finds | **archcheck misses** |
|---:|---:|---:|
| 8 | 1 | **7** |

Full table and the two hand-verified witnesses are in
[reports/nicad_vs_archcheck.md](../../reports/nicad_vs_archcheck.md) §Sub-function
granularity. The cleanest witness: a byte-identical 17-line block copied between
`do_service_directory` and `do_service_file` in `http/cervlet.c` — archcheck silent.

**7 is a floor, not the gap.** NiCad's `minsize=10` hides everything shorter,
including the 5-line duckdb case that motivated this task.

### Dead end: NiCad cannot be the C++ oracle

A June 2026 corpus run (`/home/localadm/nicad_work`, 8 C++ repos, both granularities)
already exists. Its C++ numbers are unusable: NiCad has no C++ grammar, and the
present-as-C workaround failed to parse **38–94%** of files (Kartend 94%,
IrredenEngine 90%, BambuStudio 87%, LibreSprite 86%, Catch2 83%); the irrlicht
`blocks` run died with `rc=99`. Its "only-NiCad authored ≈ 0" result is therefore
**not** evidence that the sub-function gap is small in C++ — NiCad simply never read
the code. Do not cite that run as recall validation.

Consequence: any C++ recall number for this class needs a different oracle
(PMD-CPD and SourcererCC both handle C++ and are token-based; both are plausible).

## Design sketch

Second layer beside the current one; the existing fragmentation stays as-is.

- Sliding window over the normalized token stream, window >= N tokens (N ~ 40-50 as a
  starting point), stepped, hashed (rolling hash / winnowing fingerprints).
- Candidate pairs from equal fingerprints, then extend maximally left/right, then run the
  existing similarity + guard pipeline on the extended spans.
- Emit spans as `Fragment`s so the existing scoring, classification, reporting and
  `DRIFT.NEW_CLONE` path are reused unchanged.

### Open design questions

1. **Overlap with the function-level layer.** A whole-function copy will now be found twice.
   Need a containment rule (prefer the maximal span; drop a sub-span fully contained in a
   reported pair with the same file-pair).
2. **Idiom flood.** Sub-function windows will hit far more boilerplate than function bodies
   do (error-check ladders, lock/unlock, switch arms). The existing guards were calibrated on
   function-scale fragments and will need recalibration against `fp_corpus_r2.tsv`; expect the
   joint floor and the diversity gate to need separate thresholds for this layer.
3. **Cost.** All-pairs over windows is much larger than over function bodies. Fingerprint
   bucketing must carry the recall; measure wall-clock on duckdb (2904 files, currently 12s).

## Acceptance criteria

- [x] NiCad `blocks` cross-check rerun; gap quantified and written up (2026-07-18:
      7 missed sub-function classes on monit-4.2; C++ oracle unavailable — see above).
- [x] The 7 monit misses are reported — **6 of 7**; the 2 remaining (of 8 classes) are
      low-similarity cross-file cases at 70–76% NiCad similarity.
- [x] The duckdb `pipeline_executor.cpp` batch-index pair is reported on `e884eef8` — but
      **its form depends on scan scope**, which is worth knowing before quoting it:
      - `--duplication src/parallel` (20 files): the tight block,
        `pipeline_executor.cpp:23-28 <-> 82-86` (EXACT, weighted=0.8).
      - `--duplication src` (2904 files): only the wider enclosing span,
        `20-29 <-> 74-87` (STRUCTURAL, weighted=0.51 after P1 down-weighting), ranked
        **1154th of 1208** reported pairs.
      Checked with guards disabled: at full-tree scale the tight pair is not a scored
      candidate at all — its tokens (`partition_info`, `batch_index`,
      `RegisterNewBatchIndex`) are common across duckdb, so IDF dilutes the weight below
      the 0.60 gate. This is the corpus-scale rare-df dilution already documented in
      §3.3 of the architecture doc, not something #190 introduced; #190 is what makes
      the site visible at all. Practical consequence: the finding is real but low-
      salience in a whole-repo scan — it is the `--diff` path (which scopes to changed
      files) where it actually surfaces to a user.
- [~] `--diff` on that commit range: **archcheck has no "clone removed" report at all** —
      `--diff` only reports *new* clones (`DRIFT.NEW_CLONE`), so this criterion cannot be
      met as written. Verified the substantive half instead: on the reverse range the
      block now fires `DRIFT.NEW_CLONE ... lines 20-29 - clone of ...:74-87`, where before
      it was silent. Reframed 2026-07-19: this is by design, not a gap — the gate catches
      drift, it does not applaud improvements; silence on a clone-removing commit is the
      correct signal. Only a research caveat (history trends undercount repayment) is
      recorded in `backlog/DEBT.md`.
- [x] The variant-1 repro reports the pair. Caveat: in a *single-file* corpus it still
      cannot — with 2 documents every IDF weight is 0 by construction, which is
      pre-existing behaviour, not #190. In any realistic corpus it fires (verified).
- [x] Fixtures: `fixtures/duplication/subfunction_clone/{pass,fail_block_into_function}/`
      plus two Catch2 cases in `tests/duplication_synthetic_fp_test.cpp`.
- [~] FP rate on `fp_corpus_r2.tsv`: **no automated harness exists** —
      `evaluateAgainstCorpus` is a documented placeholder and the corpus rows are external
      repo+SHA references. Substituted a before/after differential on duckdb `src/`
      (2904 files): **905 baseline pairs all preserved, lost=0**, +303 new. Hand-triaged a
      spread of 8 new pairs across EXACT/STRUCTURAL/cross-file — **8/8 true positives**,
      0 FPs. Sample is 2.6% of the new pairs; not a precision measurement.
- [x] Cost, measured on both paths (the `--diff` path was previously unmeasured, and it
      is the one that actually runs in CI):

      | path | wall | peak RSS |
      |---|---|---|
      | `--duplication` duckdb `src/` (2904 files) | 12.1s → **21.1s** (+75%) | 365MB → **538MB** (+47%) |
      | `--diff c518cf52..c28ce15a` (23 files changed) | 20.8s → **23.9s** (+15%) | 839MB → **1159MB** (+38%) |

      The `--diff` path pays much less proportionally because #180 already scopes scoring
      to pairs incident to changed files, so the 144k → 352k candidate growth barely
      applies there; the cost is the doubled fragment population itself. Note the
      **839MB pre-existing** baseline on `--diff` — most of that 1.16GB is not #190, it
      is the mode holding graph + fragments for two revisions at once. Worth its own
      task if CI memory ever bites.

      On that commit #190 bought exactly the intended finding: NEW_CLONE went 4 → 6, all
      four prior findings preserved verbatim, the two added being
      `pipeline_executor.cpp:72-84 ↔ 18-27` (the batch-index clone this task exists for)
      and `physical_recursive_cte.cpp:328-336 ↔ pipeline.cpp:41-50`.
- [x] `docs/duplication_architecture.md` updated — §3.2 documents the nesting rule and
      the three places `Fragment::nested` is load-bearing; §9 Boundaries rewritten.

## Immediate doc corrections — do these FIRST, independently of the code

The tool is **published**. Shipped behavior does not match the outward copy. These are
minutes of work and should not wait for the code:

1. **`README.md:11`** — "*A PR introduces a copied block — `archcheck --diff` names the clone
   and its source*". A block copied *into* an existing function does not fire. Reword to what
   ships, or verify the demo PRs are whole-function copies and say so.
2. **`reports/nicad_vs_archcheck.md`** — add the granularity caveat: the run was
   `functions`, the sub-function class was not covered, "0 recall bugs" is scoped to that.
3. **`docs/duplication_architecture.md` §9 Boundaries** — current wording is
   "*a class/namespace in `[min,max]` is emitted whole instead of its methods*". This
   understates it: **any** function in `[min,max]` is emitted whole and nothing nested inside
   it is ever compared. Restate plainly and list sub-function copy-paste as a known miss.

## How it works (implemented 2026-07-18)

The design sketch above proposed a **second layer**: a sliding window over the token
stream with rolling-hash fingerprints, extended maximally, emitted as `Fragment`s.
That was not built. The fingerprint machinery it describes already exists (#092
k-gram winnowing, `addFingerprintCandidates`), so the whole class turned out to be
reachable by removing one early-exit instead of adding a parallel pipeline.

**The change is four lines of behaviour and three consistency fixes.**

1. `fragmenter.cpp` — an emitted function body is now also descended into, instead of
   `i = j + 1` skipping past it. Nested blocks become fragments and flow through the
   existing scoring, classification, reporting and `DRIFT.NEW_CLONE` path unchanged.
   Fragments emitted inside an already-emitted body carry `Fragment::nested`.

2. `Fragment::nested` then has to be honoured everywhere a count of *documents* is
   meant, because a nested fragment's text is already counted in its enclosing body.
   Each of these was found as a concrete regression, not by inspection:

   - **df / IDF** (`clone_index.cpp`). Double-counting drove IDF toward zero — an
     identical nested block scored ~0 and never became a candidate. Excluding nested
     fragments keeps the df/IDF scale *identical* to pre-#190, which is what keeps the
     `fp_corpus_r2.tsv`-calibrated guard thresholds valid.
   - **Fingerprint frequency cap** (`fpMaxPostings`). A run is fingerprinted in both the
     body and the nested block, so ordinary runs crossed the cap and were dismissed as
     boilerplate. This one silenced duckdb's entire `duckdb_tables`/`views`/`indexes`/
     `triggers`/`sequences` clone family — 5 real pairs at weighted 0.83–0.91 vanished.
     Caught only by diffing the full before/after pair sets; it does not reproduce on a
     small subdirectory, because the cap is an absolute count.
   - **Whole-file clone ratio.** Nested pairs inflate numerator and denominator
     unequally, pushing sibling files over the 80% bar. Two byte-identical single-
     function test fixtures started being suppressed as whole-file twins.

3. `phaseNestedContainment` (`duplication_scanner.cpp`) — drops a pair whose both sides
   sit inside the corresponding sides of another surviving pair, so one copy site
   produces one finding at its maximal span. On monit this cut 82 reported pairs to 48
   without losing any of the newly-found sub-function classes.

**Result.** monit sub-function recall 1/8 → 6/8. duckdb: all 905 baseline pairs
preserved, +303 new, 8/8 hand-checked true positives. 655/655 tests, dogfooding clean,
lizard clean.

### Key decisions

- **No second pipeline.** A sliding-window layer would have needed its own guard
  calibration (the sketch's open question 2) and its own cost model. Descending in the
  existing fragmenter reuses every calibrated guard as-is.
- **Nested fragments are fragments, but not documents.** This single rule resolves the
  sketch's open question 1 (overlap) at the *statistics* level, and the containment
  filter resolves it at the *reporting* level. They are different problems and both
  needed solving; conflating them is what made the first attempt lose real clones.
- **min_tokens was left at 30.** Lowering it is the obvious next lever for the short-block
  class (the 5-line duckdb block is only caught because it is 30+ tokens dense), but that
  reopens the idiom-flood question and belongs in its own measured task.

### Changed files

- `src/scan/duplication/fragmenter.cpp` — descend into emitted bodies; `Range` carries `inside`
- `include/archcheck/scan/duplication/fragmenter.h` — `Fragment::nested`
- `src/scan/duplication/clone_index.cpp` — df/IDF and fingerprint cap count documents
- `src/scan/duplication/duplication_scanner.cpp` — `phaseNestedContainment`; whole-file ratio
- `tests/duplication_synthetic_fp_test.cpp` — two fixture-driven cases
- `tests/duplication_fragmenter_test.cpp`, `tests/duplication_fp_guards_test.cpp` — fixture
  expectations that encoded the old one-body-one-fragment nesting
- `fixtures/duplication/subfunction_clone/{pass,fail_block_into_function}/`
- `docs/duplication_architecture.md`, `reports/nicad_vs_archcheck.md`, `README.md`, `backlog/DEBT.md`

### Open / not done

- **The 2 remaining monit misses are deliberate, not debt.** Both are switch-dispatch
  shapes (`util.c:653 ↔ cervlet.c:1447`, `util.c:1084 ↔ 1166`). Neither reaches
  candidacy even with every guard disabled, because `isSwitchSkeletonLine` strips
  `switch (…)`/`case X:`/`break;`/`default:` from the line signal by design — and in
  these two the switch skeleton *is* the shared substance. Read both by hand: the
  second is not copy-paste at all (two unrelated one-line bodies dispatched on the same
  enum — NiCad's blind rename over-reports it), the first is a real adapted copy whose
  distinctive content (`printf` + plain text vs `out_print` + HTML) genuinely differs.
  Chasing 8/8 would mean weakening a calibrated FP guard to gain one true positive.
- Sub-`min_tokens` copy-paste — still structurally invisible.
- ~~No C++ recall number~~ — obtained via Duplo, see the section above: 13/24 → 17/24 on
  duckdb `src/execution`.
- **Statement-run clones** (a copied span that is not one braced block) — the residual
  class, characterised with four hand-read witnesses. Split out as **#191**.
- Cost: +75% wall / +47% RSS on duckdb. Acceptable for now; if it bites, the lever is
  candidate generation (352k pairs), not the fragmenter.

## C++ recall number — obtained 2026-07-18 (Duplo, not NiCad)

NiCad is unusable on C++ (no grammar; the present-as-C workaround fails on 38–94% of
files). PMD-CPD and SourcererCC both need a JVM, and no JRE is installed here.
Used **Duplo** instead — native C++, line-based, and crucially *function-boundary
agnostic*, which is exactly the property this class needs. Built from tag `v1.0.1`
(later tags need `<format>` → GCC 13; local GCC is 8.3).

```bash
duplo -ml 8 <filelist> out.txt      # min block 8 lines
```

Corpus: duckdb `src/execution`, 205 files. Duplo blocks were deduped and blocks that
are majority-preprocessor or brace scaffolding were dropped (a line-based tool reports
include ladders, which are not the class under test), leaving 24 substantive blocks.

| | before #190 | after #190 |
|---|---:|---:|
| Duplo blocks covered by archcheck | **13 / 24** | **17 / 24** |
| archcheck pairs on that subtree | 77 | 99 |

On monit the same harness gives 33/67 covered; most of the uncovered there are
deliberate suppressions rather than misses — e.g. `protocols/ldap2.c ↔ ldap3.c` is a
byte table of LDAP protocol constants, which the P1.1 data-table guard drops on
purpose. Verified by reading them.

### The residual class is now characterised (this is the real finding)

The 7 remaining C++ misses are not a threshold problem. They share one shape: **the
copied region is not a single braced block.** The fragmenter emits `{…}` bodies, so it
can represent a copied `if`/`for`/`while`/`switch` body, but not:

- a bare run of consecutive statements (`physical_iejoin.cpp:2073 ↔ 2098`),
- a span covering several *sibling* blocks plus the code between them
  (`physical_limit.cpp:246 ↔ physical_streaming_limit.cpp:72` — a whole
  `ParamsToString` body: three sibling `if`s with statements interleaved),
- a chunk of one multi-line call's argument list (monit `cervlet.c:832 ↔ 944`).

This is precisely the class the original sliding-window sketch targets, and it is the
honest scope of what #190 did **not** close. Brace-descent got the blocks; a
window/suffix pass is what gets the runs. Split out as its own task rather than
smuggled in here, because it reopens the guard-calibration question the sketch flagged.

## Per-commit validation on duckdb `pipeline_executor` history (2026-07-19)

30 commits touching `src/parallel/pipeline_executor.{cpp,hpp}` since 2025-07, each run as
`--diff <parent>..<commit>`. All 30 produced valid output (verified by the presence of the
`baseline_ref:` header — see the methodology warning below).

Three are merges (539 / 378 / 92-file diffs) and are excluded: a merge's first-parent diff
is a whole branch's work, so its counts describe the branch, not one author's editing.

Of the **27 regular commits, 21 are completely silent**. The six that fire:

| commit | date | author | clones | bools | complexity | subject |
|---|---|---|---:|---:|---:|---|
| `c28ce15a65` | 2026-03-28 | Denis Hirn | **6** | 2 | 3 | Optimize recursive CTEs by reusing executors and working buffers |
| `9b007a5e4b` | 2026-02-24 | dentiny | 4 | 0 | 0 | [DRAFT] Initial PR for thread annotation |
| `496de75687` | 2026-04-24 | Denis Hirn | 4 | 0 | 0 | Move Sink and Source state resets to the state as well |
| `dac8f19e73` | 2026-05-20 | Mytherin | 2 | 1 | 0 | Fix profiling tests |
| `5728627930` | 2026-02-02 | Etgar Shmueli | 1 | 0 | 1 | Refactor `DataChunk::Verify` … |
| `2c6d7698c0` | 2026-02-04 | Etgar Shmueli | 1 | 0 | 0 | Update `DataChunk::Verify` … |

The two heaviest are both Denis Hirn's executor-reuse series — the same series that created
the batch-index clone (`c28ce15a65`), which then survived ~6 weeks until it was extracted.

### Methodology warning — a failed run is indistinguishable from a clean commit

The first pass of this table was **wrong and nearly went out**: 24 of 30 runs had produced no
archcheck output at all (git's background auto-gc broke the `git cat-file --batch` reads that
`--diff` memory mode depends on), and every counter for those commits was recorded as 0. The
summary therefore read "28 of 30 commits clean, the tool is admirably quiet" — a conclusion
built entirely on failures. File size was no defense: the failed outputs were ~600 bytes of
git's gc notice.

Two rules for any future per-commit harness:

1. **Validate each run** by a positive marker in its output (`baseline_ref:`); record anything
   else as ERROR, never as 0.
2. **Disable auto-gc** in the scanned clone (`git config gc.auto 0`) — and note `--diff` peaks
   near 1GB, so concurrent runs starve each other (8 of 9 retried failures passed when run
   alone).

This is the same failure shape as the NiCad C++ dead end recorded above: a tool that could not
read the input reported nothing, and nothing looked like a clean result.
