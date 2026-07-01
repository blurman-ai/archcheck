# [SCAN] Clone-detector FP reduction — classification gaps + cosmetic data-table guard

**Created:** 2026-06-29
**Status:** wip — A+C DONE (`5277cd2`), D measured+closed (`b91fe32`), harness fixed +
statement-floor DONE; 2026-06-30 uncommitted follow-up added Bison/Flex generated filtering,
CLASSIC switch-skeleton filtering, and deleted-line move suppression.
**Module:** SCAN / duplication (#056/#070), classification (#129)

> **RESUME HERE — checkpoint 2026-06-30 (read this first).** Jump to the section
> "## CHECKPOINT 2026-06-30" at the bottom: it lists exactly what is done, what is
> uncommitted, how it was measured, where the trustworthy numbers are, and what is left.

## Done 2026-06-29 — Parts A + C (commit `5277cd2`)

Drafted by Haiku, reviewed + fixed by Opus. Verified: 610 tests, dogfood 0, lizard clean,
corpus acceptance (chrxh_alien `*Tests.cpp` pairs 7→0; djeada data-table pairs 518→477).

- **Part A** shipped: `isTestBasename`/`isTestDirName` now match CamelCase `XxxTests`/`XxxTest`
  (capital-T + lowercase-before guard) and the `.test.`/`.spec.` infix.
- **Part C** shipped: `phase10DataTableClassifier` is a real DROP (both diversity<0.30 AND
  cloneType∈{LITERAL,MIXED}), togglable via `enableDataTableDrop`.

**Review catches (the value — see JOURNEY 2026-06-29):**
1. The CamelCase size guards were off by one (`>=5`/`>=4` then read index `size-6`/`size-5`)
   → out-of-bounds read on a bare `Tests`/`Test` segment. Fixed to `>=6`/`>=5`.
2. The data-table guard fires ONLY on a pair that ALREADY clears the joint floor with both
   diversity<0.30 AND LITERAL/MIXED. An all-different-literals palette (every line differs)
   is killed UPSTREAM by the floor (low line overlap), never reaching the guard — the draft's
   fixture proved nothing. The real target is the djeada signature: a table that is byte-
   identical except one line (`default:`) → high line overlap, LITERAL, low diversity.
3. Measurement trap recorded: a stale (un-relinked) binary made the guard look like a no-op
   (518==518). The lib-level probe is authoritative (1266→1223). Always re-link before A/B.

---

**Origin:** eyeball triage of the CURRENT detector's actual fires (not corpus labels —
those proved unreliable: several round-2 "FP" are real copy-paste). 82 fires across 11 repos
read by hand; raw dumps were in scratch (`fphunt.py` triage). Sample precision ≈ 75-80 %;
the FP mass clusters in the classes below.
**Related:** #070 (FP guards), #129 (AuthoredScope test/vendor/generated gate), #131 (Group 3
recall fix P0.6b — added the run-path that widened the coincidental class in Part D),
[docs/dev/duplication_decision_path.md](../../docs/dev/duplication_decision_path.md) (read FIRST).

---

## Goal

Cut the duplication detector's false positives at their real sources, found by reading the
detector's own output. Four root causes, two of them **mechanical and Haiku-ready** (Parts A, C),
two that **need measurement/design first — NOT for Haiku** (Parts B, D).

## Evidence — the FP classes (with verified causes)

| Class | Example (repo : file) | Verified cause |
|---|---|---|
| **Unit-test / fuzzer boilerplate** | chrxh_alien `source/EngineTests/ObjectConnectionTests.cpp` (all 7 sampled fires were test bodies); AztecProtocol `alu_trace.test.cpp`, `bigfield.fuzzer.hpp` | test files NOT excluded — see Part A. `--duplication` filters via `AuthoredScope::fromFiles` (`project_files.cpp:160`, #129), which calls `isTestBasename`/`isTestDirName`, but those miss camelCase + infix naming. |
| **Generated code** | cschladetsch_CppKAI `SimpleCalculator.proxy.h` / `.agent.h` RPC stubs (Add/Subtract/Multiply near-identical) | `.proxy.h`/`.agent.h` not in `isGeneratedPath` (`file_classification.h`). Part B (design). |
| **Data-table / palette** | djeada `skeleton_archer.cpp:17` ↔ `swordsman.cpp:18` (`case N: return {r,g,b};` color tables, ×3); `scatter_rules.h` (enum→multiplier switch) | `phase10DataTableClassifier` only **down-weights** (×0.85) and runs AFTER the joint floor → no suppression. Part C. |
| **Vendored single-header** | NamecoinGithub_LLL-TAO `json.h` (nlohmann, char-dispatch state machine) | a vendored single header not under a vendored DIR isn't excluded (same gap as #131 Group 1). Part B (design). |
| **Coincidental / shared-skeleton** | graphillion_kyotodd BDD iterators (`enum Phase{...}; struct Frame{...}; stack.reserve(64)` shared across ~7 iterators, line-overlap 0.20-0.35); fuddlesworth gap setters; djeada `terrain.cpp` river-vs-bridge | low line-overlap pairs sharing a scaffold but with divergent bodies. **Admitted by the #131 P0.6b run-path** (≥6 shared lines + diversity≥0.30 + weighted≥0.60). Part D (measurement). |

---

# PART A — test-file recognition gap (HAIKU-READY)

**Resolved facts (checked 2026-06-29):**
- `isTestBasename` (`include/archcheck/scan/file_classification.h:441`) matches only: stem starts
  with `test_`/`test-`, OR stem ends with one of `_test _tests _spec -test -tests -spec`
  (separator required). It is **case-insensitive** (operates on `toLowerAscii(filename)`).
- `kTestDirNames` (`file_classification.h:424`) = `{test, tests, tst, testutil, testutils,
  unittest, unittests}`; `isTestDirName` normalizes the segment via `normalizeDirSegment` (lowercases,
  strips separators) then exact-matches the set.
- **Gaps that leak (confirmed by the triage):**
  - `ObjectConnectionTests.cpp` → stem `objectconnectiontests` ends with `tests` but **no separator** → not matched.
  - `alu_trace.test.cpp` → stem `alu_trace.test` ends with `.test` not `_test` → not matched.
  - dir `EngineTests/` → normalizes to `enginetests` ≠ any `kTestDirNames` entry → not matched.

**The fix — extend recognition, with a hard guard against false matches.**

### A.1 `isTestBasename` — add two patterns

Add to `isTestBasename`, AFTER the existing suffix loop, BEFORE `return false`:

1. **CamelCase suffix** — match when the **original** (not lowercased) `filename` stem ends with
   `Test` or `Tests` AND the character before that suffix is a lowercase ascii letter (a CamelCase
   boundary). This requires checking the raw `filename` (the function currently only keeps the
   lowercased `name`); add a parallel raw-stem check. Capital `T` is the discriminator.
2. **Dotted infix** — match when the lowercased `filename` contains `.test.` or `.spec.`
   (e.g. `alu_trace.test.cpp`, `foo.spec.cc`).

**CRITICAL negative cases (must NOT be classified as test — guard 1.5 of the Haiku guide):**
`latest.cpp`, `contest.cpp`, `fastest.cpp`, `attestation.cpp`, `greatest.cpp`, `protest.cpp`.
All contain the substring `test` but: lowercase `t` (not CamelCase `Test`) and no `.test.` infix.
The capital-`T` requirement is what excludes them — do NOT lowercase before the CamelCase check.

### A.2 `isTestDirName` — add CamelCase dir suffix

A dir segment whose **raw** form ends with `Test` or `Tests` preceded by a lowercase letter
(`EngineTests`, `UnitTests`, `EngineTest`) is a test dir. Same capital-`T` + lowercase-before guard.
`normalizeDirSegment` lowercases, so this check needs the raw segment — add it in `isTestDirName`
before normalization, mirroring A.1's logic. Negative: `manifest/`, `greatest/` must NOT match.

### A.3 Fixtures (mandatory — acceptance bar)

- `fixtures/file_classification/` (or extend the existing classification test
  `tests/unit/scan/` — grep `isTestBasename` in tests/ for the live test file, checked 2026-06-29):
  assert TRUE for `ObjectConnectionTests.cpp`, `FooTest.cpp`, `alu_trace.test.cpp`, `bar.spec.cc`;
  assert FALSE for every negative case in A.1; assert dir TRUE for `EngineTests`, FALSE for `manifest`.
- One **duplication** fixture: `fixtures/duplication/test_boilerplate/` — two files
  `WidgetTests.cpp` + `GadgetTests.cpp` each with an identical arrange-act-assert block; assert
  `scanForDuplication` over the directory (through `collectNonVendoredSources`) reports **0 pairs**
  between them (they are excluded as tests). Mirror the existing duplication fixture style.

### A.4 Acceptance (numbers)

- All A.3 unit assertions pass.
- `./build/debug/src/archcheck --duplication ~/oss/chrxh_alien/source` no longer reports
  any pair whose BOTH sides are `*Tests.cpp` (before this task: ≥7 such pairs in the sample).
- Existing suite stays green; **dogfood**: `./build/debug/src/archcheck src include tests` = 0 violations
  (archcheck's own `tests/` are already excluded by `tests/` dir; this change must not regress that).

### A.5 No expectation flips

This change only EXCLUDES more files. No existing test encodes "a `*Tests.cpp` clone must be
reported", so no expectation needs inverting (guard 1.5). If grep finds one, STOP and report it —
do not delete it silently.

---

# PART C — make the data-table guard actually suppress (HAIKU-READY, tight spec)

**Resolved fact (checked 2026-06-29):** `phase10DataTableClassifier`
(`src/scan/duplication/duplication_scanner.cpp:186`) multiplies `p.weighted *= 0.85` when both
fragments have `diversity < 0.30`, and runs in `applyCandidateFilters` AFTER
`phase8JointTokenOrderFloor` with **no subsequent threshold re-check** — so it changes only the
reported weight, never membership. The palette/enum-table FPs survive.

**The fix:** turn it into a real DROP. A pair is a data table when **both** fragments are
`diversity < 0.30` **AND** the clone type is `LITERAL` or `MIXED` (set by `phaseClassifyCloneType`
— but that runs last; instead test it inline here via `cloneType(frags[p.a], frags[p.b])`, or move
the data-table phase to run after classification). Drop such pairs.

- **Why both conditions:** `diversity<0.30` alone could catch a real low-diversity logic copy;
  requiring LITERAL/MIXED ties it to "the difference between the two is only literals" — the
  signature of a `case N: return {lit,...}` palette or an enum→constant map.
- **Parameters:** `diversity < 0.30` (existing), type ∈ {LITERAL, MIXED}. Add a `ScannerOptions`
  bool `enableDataTableDrop = true` so it is togglable (mirror `enableP1Guards`).

**Control cases with expected outcomes (guard 1.4):**
- djeada `skeleton_archer.cpp:17` ↔ `swordsman.cpp:18` (LITERAL, both low-diversity) → **dropped (NO finding)**.
- djeada `scatter_rules.h` enum→multiplier switch (MIXED, low-diversity) → **dropped**.
- A real STRUCTURAL copy with diversity≥0.30 (e.g. ThemisDB `mysql_importer`↔`sqlite_importer`,
  LITERAL but diversity high) → **still reported** (verify diversity ≥ 0.30 by hand first; if it is
  LITERAL+low-diversity, it is correctly a borderline drop — record the number, do not assume).

**Fixtures:** `fixtures/duplication/data_table/pass/` (a real logic copy that must still fire) and
`fixtures/duplication/data_table/fail_palette/` (two `case N: return {lit,lit,lit};` switches that
must NOT fire). Acceptance: pass-fixture reports the pair, fail-fixture reports 0.

**Expectation flips:** check `duplication_fp_guards_test.cpp` / `duplication_synthetic_fp_test.cpp`
for any existing data-table case asserting a down-weighted-but-present pair; if found, its
expectation changes from "present with reduced weight" to "absent" — update it and note WHY in the
commit (the down-weight was a no-op; suppression is the intended behavior).

---

# PART B — generated / vendored single-header (NOT HAIKU — needs design)

Do **not** hand to Haiku. These need a human/senior decision because naming-based exclusion is
risky (a `.proxy.h` could be hand-written; a vendored single-header has no path signal):
- **Generated RPC stubs** (`.agent.h`, `.proxy.h`, `.fuzzer.hpp`): decide between a naming list
  vs a content-banner check (preferred — `isGeneratedPath` already reads banners for some types).
- **Vendored single-header** (nlohmann `json.h`, etc.): same open gap as #131 Group 1
  (whole-repo-vendored / bundled headers). Solve together, not piecemeal.

# PART D — coincidental / shared-skeleton (NOT HAIKU — needs measurement)

The #131 P0.6b run-path (`phase8JointTokenOrderFloor`, run path: `sharedLines ≥ 6 &&
diversity ≥ 0.30 && weighted ≥ jointRunWeightedThreshold`) admits pairs that share a scaffold
(graphillion `Frame`/`stack` iterator skeleton; setters; declaration runs) but have line-overlap
0.20-0.35 and divergent bodies. Candidate levers (each needs a corpus recall↔precision measurement
via `experiments/corpus_remeasure_131/group3_*.py` — that harness reports BOTH metrics):
1. require the shared line run to be **contiguous** (a real block), not scattered — `sharedLines`
   is currently an order-LCS length over `normLineSeq`; add a longest-contiguous-run measure and
   gate on it.
2. raise `jointRunWeightedThreshold` (0.60 → 0.65/0.70) — trades int64/uint64-class recall.
3. require a rare anchor on the run path (the #131 `jointMinSharedRare` knob, currently 0) with a
   calibrated `jointAnchorDfCap` — #131 showed df≤4 too strict, df≤12 leaked; needs a sweep.

Decide D only with the curve in hand. Do not tune blind.

## Part D — MEASURED 2026-06-29 (the curve is in hand)

All levers swept on the Group-3 labelled corpus from the committed high-recall config
(runW=0.60, rare=0 → recall 45.7% / precision 68.8% / suppression 82.5%). Curve in
`experiments/corpus_remeasure_131/{partD_curve.txt,partD_contiguous.txt}` (gitignored).

| lever | recall | precision | TP cost | verdict |
|---|---|---|---|---|
| rare-anchor=2 (df≤12) | 43.6% | 70.9% | −3 TP | ~1:1 trade |
| weighted floor 0.70 | 42.9% | 69.8% | −4 TP | worse trade |
| weighted 0.65 + rare2 | 40.7% | 70.4% | −7 TP | worst recall |
| contiguous-run ≥3/4/5 | 45.0/42.1/40.7% | 69.2/70.2/69.5% | −1/−5/−7 TP | ~1:1, no separation |
| **switch-skeleton stop** | **45.7%** | **69.6%** | **0 TP** | **clean — SHIPPED `b91fe32`** |
| rare-shared-LINES ≥2/3 (dfCap3) | 44.3/42.1% | 69.7/68.6% | −2/−5 TP | WORSE than 1:1 — reverted |

**Finding.** The three numeric knobs (rare-anchor, weighted floor) and the contiguous-run
measure all trade ~1 real catch per FP removed — they cannot separate a coincidental scaffold
from a real edited copy, because the scaffolds ARE long, contiguous, high-weight runs. The
discriminator is not the run's length/shape but its **content**: skeleton vs substance.
Excluding switch-skeleton lines (`case X:`/`break;`/`default:`/`switch(...)`) from the run
measure is the only lever that removed FP at **zero recall cost** — shipped.

**Sequential-scaffold idioms (SQL `ensureTables`, TFT slider draws, struct-assign blocks,
graphillion BDD iterators) — line-rarity tried and REVERTED.** Hypothesis: a real copy shares
project-RARE lines; a scaffold shares only common ones. Implemented per-line document frequency
(CloneIndex.lineDf) + a run-path gate requiring ≥K shared lines with lineDf≤3. Measured: a WORSE
than 1:1 trade (≥2 → −2 TP / −1 FP). Row-level: it killed 1 real idiom FP (tasksmack Gpu/Network
panels) but also 2 labelled TP — bylins/mud SQL data-source copy AND graphillion zdd iterator
(corpus-labelled TP, though arguably the recursion→iteration recipe). **Why it fails:** rarity is
repo-relative, same wall as the rare-TOKEN anchor (#131 df≤12 leak). A real copy's surviving
shared lines are often themselves ceremony (the specifics got edited, or live in a family of
similar code where they aren't rare). Rarity — token or line — does NOT separate; the only clean
lever was switch-skeleton (UNIVERSAL, content-free keywords).

**Conclusion for D.** Clean precision wins come from universal non-specificity (the `switch`
skeleton), not from rarity (always repo-relative) or run geometry (length/contiguity — scaffolds
are long contiguous blocks). Remaining idiom classes need a different idea (a curated stop-list of
library-ceremony lines is brittle/library-specific; an AST/semantic signal is out of v0.1 scope).
Numeric knobs documented as not worth their recall cost. D is effectively closed: one clean lever
shipped, the rest are dominated.

---

## 2026-06-30 — HARNESS FIX: the Group-3 matcher was over-crediting (we were measuring in a vacuum)

Triggered by the user reviewing fired cases by eye and catching #7 (ibex `join.cpp`).
Root cause found in the harness, not the detector:

- The harness shipped **two divergent matchers**. `group3_precision.py:row_fires`
  required BOTH endpoints; `group3_summary.py:row_fires` — **the one that produced
  every reported recall/precision number** — fired a row if a **single** endpoint of
  **any** finding touched **any** labelled file (`endpoint_hits(...) or endpoint_hits(...)`).
  Its header even bragged it was loosened ("v1 understated recall"). For a whole-file
  label like `join.cpp:1-856`, ANY finding touching join.cpp fired the row.
- **My #7 explanation to the user was itself wrong.** I had run an isolated single-file
  probe that only reproduced within-file join pairs and concluded "matcher artifact,
  join↔reshape not join↔interpreter." The **cached harness finding** (`group3_findings_skeleton.jsonl`)
  shows 14 genuine `join.cpp ↔ interpreter.cpp` cross-file pairs. #7 is a **real fire**
  on a monolith-split code **MOVE** (lines relocated, not copy-pasted) — a legitimate FP
  class the detector can't distinguish from a copy without diff-of-diffs. Not an artifact.

**The corrected matcher** (`experiments/corpus_remeasure_131/match.py`, used by
`rescore.py` and now by `group3_summary.py`): a finding credits a row iff (a) ≥1 endpoint
lands in a labelled span (basename + range overlap) AND (b) the finding's topology matches
the row's declared topology, read from the **klass/fp_class column** — `within-file-chunk`/
`idiom`→within, `cross-file-chunk`/`whole-file`→cross, `partial-edited`/`data-table`/etc.→either.
The topology column is essential: the corpus pins only ONE representative span for most
cross-file copies (`commands_cgroup.cpp:29-53` for a cgroup↔network copy), so the added_loc
text alone can't tell within from cross. Every fired verdict now returns a **witness finding**.

**Building this matcher took two wrong drafts**, each caught by hand-checking witnesses
(NOT by trusting the aggregate):
1. parsed `added_loc`/`base_loc` separately → dropped elided `:104-127` continuations
   (DataDog refCountGuard falsely missed).
2. demanded both endpoints inside one span → killed single-span within-file families
   (bylins, FlashCpp falsely missed). The "precision 78%" it produced was as fake as 69.6%.

**Corrected Group-3 numbers (shipped `skeleton` config), every verdict witness-backed:**

| matcher | recall | precision | suppression |
|---|---|---|---|
| legacy OR (what Part D was measured with) | 45.7% | 69.6% | 83.1% |
| **CORRECTED (topology + witness)** | **43.6%** | **70.1%** | **84.3%** |

**Part D conclusions SURVIVE re-scoring** (the OR inflation was ~uniform, so relative
comparisons held). Switch-skeleton lever, corrected: `w60_r0` 61/140 → `skeleton` 61/140
= **0 TP cost** (confirmed under both matchers), precision 69.3→70.1. Reverted levers stay
dominated: rareline2 −2 TP, contig3 −1 TP, RAREANCHOR −10 TP, w70_r0 −5 TP. The Part D
table was right for the right reasons; only the absolute numbers move ~2-3 pp.

## 2026-06-30 — #12 recorded: switch-skeleton stop only covers the RUN path, not CLASSIC

The user asked why the switch-case filter (`b91fe32`) didn't suppress a near-verbatim
copied switch (CppKAI visitor dispatch). Cause, confirmed in `fragmenter.cpp:collectNormLines`:
`isSwitchSkeletonLine` is applied ONLY to `f.normLineSeq.push_back` (the RUN-path measure
→ `sharedLines`). The line right after, `f.normLines.insert`, keeps ALL lines including
`case:`/`break;`/`switch`. The CLASSIC path (`weighted≥0.75 AND line≥0.50`) scores over
`normLines` → a near-verbatim switch copy fires CLASSIC, which the filter never touches.

**Candidate (NOT yet done — needs the same Part D measurement, now on the fixed harness):**
also exclude switch-skeleton lines from `normLines` (classic `lineOverlap`). RISK: the
classic path is the conservative high-precision path; narrowing it can drop real near-verbatim
copies. Measure recall↔precision via `rescore.py` before shipping. Do NOT tune blind (Part D rule).

## 2026-06-30 — composition-vs-copypaste idea (future signal, from the user)

User's idea on case #20: a run that **calls the same functions with different parameters**
is *composition/idiom*, not copy-paste. Neither rarity nor run-geometry catches it (it's about
*what* the lines are, like the switch lever). Candidate future signal; not scoped here.

## Working order

1. **Part A first** (highest FP mass, lowest risk, fully mechanical).
2. **Part C** (tight spec, fixtures gate it).
3. Re-run the eyeball triage (`fphunt.py` style) on 4-5 repos to confirm the test/data-table mass
   dropped and nothing real regressed.
4. Parts B, D — separate tasks once a human picks the approach / a sweep is run.

## Self-check (mandatory)

- Every fixture has BOTH a pass and a fail case (project rule).
- The CamelCase test rule's negative cases (`latest`, `attestation`, …) are unit-asserted — this is
  the one trap in Part A.
- After Parts A+C, dogfood `archcheck src include tests` = 0; full suite green; lizard clean
  (`lizard --CCN 15 --length 30 --arguments 5 --warnings_only` on touched files).

---

# CHECKPOINT 2026-06-30 — resume point (read first)

A long session that (1) fixed the measurement harness, (2) corrected my own wrong #7
explanation, (3) hand-triaged all 26 fired FP, (4) shipped a new clean lever (statement
floor). Everything below the line "Session work" is **uncommitted**.

## DONE this session

### 1. Harness fix — the numbers were measured with a broken matcher (COMMITTABLE, low risk)
- **Bug:** the Group-3 harness had TWO matchers; the one that produced every Part D
  recall/precision number (`group3_summary.py:row_fires`) credited a row on a SINGLE
  endpoint touching ANY labelled file via OR. Whole-file labels (`join.cpp:1-856`) then
  matched any finding touching the file → mis-attribution.
- **Fix:** `experiments/corpus_remeasure_131/match.py` — one topology-aware matcher.
  A finding credits a row iff (a) ≥1 endpoint lands in a labelled span (basename+range) AND
  (b) the finding's topology matches the row's `klass`/`fp_class` column (within-file-chunk/
  idiom→within, cross-file-chunk/whole-file→cross, partial-edited/data-table/etc→either).
  Returns a WITNESS finding for every fired verdict. `rescore.py` re-scores any cached
  findings jsonl and prints the legacy-OR delta; `group3_summary.py` now delegates to it.
- **Validated by hand** (NOT by aggregate): building it took two wrong drafts, each caught by
  witness inspection (separate-field parse dropped elided `:104-127`; "both endpoints in one
  span" killed single-span within-file families). See JOURNEY 2026-06-30.
- **#7 self-correction:** I had told the user ibex `join.cpp` was a "matcher artifact
  (join↔reshape)". WRONG — the cached findings show 14 genuine `join↔interpreter` cross-file
  pairs; it is a real fire on a monolith-split code MOVE. Lesson saved to memory
  (`feedback_provenance_before_showing`): read the artifact, never a side-probe, before showing.

### 2. Statement floor — NEW clean lever (UNCOMMITTED, code change)
- **Problem the user pushed on:** the detector reported 1-line "clones". The min size is
  `minTokens=30` (tokens, NOT lines); a dense one-liner packs 31 tokens onto one line
  (`for(int i=0;i<4;i++) buf[i]=(v>>(8*i))&0xFF;`), and the classic ratio path
  (weighted≥0.75 AND lineOverlap≥0.50) had NO absolute substance floor → it passed at
  lineOverlap=1.0.
- **Tried first (REVERTED): a line floor.** Style-dependent — it would drop a real copy of 7
  statements written on 2 lines (broke `kAdjacentCopyPasteSource` test). User chose
  "statements, not lines".
- **Shipped (in tree, uncommitted): a STATEMENT floor.** `Fragment::statementCount` = top-level
  `;` at paren-depth 0 (for-headers excluded); classic path requires
  `min(stmtA,stmtB) >= jointMinClassicStatements` (=2). Style-robust.
- **Floor is 2, not higher — KNOWN LIMIT:** a real 3-statement function (`bracketGap`: loop +
  two guarded returns) is indistinguishable by ANY size axis (lines/statements/tokens) from a
  3-statement near-idiom (NAAb VM `av=;bv=;push(op)`). Floor 2 kills only 1-statement dense
  one-liners (zero collateral); floor ≥3 takes collateral. Same wall as Part D.
- **Measured (corrected harness, real binary):** recall 43.6%→43.6% (**0 TP cost**),
  precision 70.1%→70.9%, suppression 84.3%→84.9% (+1 FP row: FreshVoxel), finding-level noise
  1891→1845 (**−46 one-statement clones**). First zero-recall-cost lever since switch-skeleton.
- **Files:** `include/.../fragmenter.h` (+statementCount), `src/.../fragmenter.cpp` (count loop),
  `include/.../duplication_scanner.h` (+jointMinClassicStatements=2), `src/.../duplication_scanner.cpp`
  (classic gate), `tests/duplication_fp_guards_test.cpp` (+kDenseOneLinerSource test).
- **Gates:** 611 tests green, dogfood `archcheck src include tests`=0, lizard clean on the two
  touched functions (2 pre-existing warnings in `phase13`/`wholeFileClonePairs` are NOT mine).

### 3. Hand-triage of all 26 fired FP (reference, in chat history; verdicts below)
Real precision is higher than the labelled number: **~14/26 fired "FP" are actually real
copy-paste the corpus under-labelled** (AetherSDR biquad, djeada horse_archer carthage↔roman,
NexusMiner 40-line block, Ibex variant-ladder, viperx1/Smatchet extractable, alien generator-UI,
etc.). Genuine "detector wrong" ≈ 8-10, in clean classes:
- API ceremony (ggml/Qt/SIMD) — no cheap universal signal (Part D).
- generated (#9 bison) — Part B.
- too-small (#15/#25) — FIXED by statement floor.
- code-move (#22/#24/#25, and #7) — needs diff-of-diffs (finding also present in deleted lines).
- required mirror (#18 CPU/GPU PCG32) — needs opt-in annotation, out of v0.1.
- composition (#21 tasksmack ImPlot) — user's idea: same funcs, different params (see below).

## REMAINING / NOT DONE
- **Commit the session work** (harness files are gitignored under experiments/; the code change
  + task + JOURNEY are not). No commit was made — waiting for the explicit command.
- **Part B** (generated `.proxy.h`/`.agent.h`/bison + vendored single-header) — still open, needs
  design (naming-list vs content-banner). #9, #8 land here.
- **code-move signal (diff-of-diffs)** — the biggest remaining genuine-FP class (#7/#22/#24).
  Idea: a cross-file clone whose source side also appears in the commit's DELETED lines is a
  move, not a copy. New task, not scoped yet.
- **composition vs copy-paste (#20)** — user's idea: a run that calls the same functions with
  different parameters is composition, not copy-paste. Candidate future signal (content-based,
  like switch). Not scoped.
- The 3-statement wall (NAAb-class) is documented as a hard limit, not a TODO.

## HOW TO MEASURE (so the next session reproduces, not re-derives)
- Corpus gate run: `cd experiments/corpus_remeasure_131 && python3 group3_precision.py`
  (writes `group3_findings.jsonl`; ~80s with the warmed corpus, 8 workers, memory-mode = no git
  orphans). Then `python3 rescore.py group3_findings.jsonl` for recall/precision/suppression
  with witnesses + the legacy-OR delta.
- **Trust `match.py` (topology + witness), NOT the legacy OR.** Quote a number only after
  hand-checking a sample of its witnesses (the matcher was wrong twice this session).
- Cached configs (gitignored): `group3_findings_skeleton.jsonl` = shipped-before-this-session;
  `group3_findings_STMTFLOOR.jsonl` = with the statement floor; `_PREMINLINE`/`_PRE158` etc.
- Spot-check a single repo: `archcheck --diff --diff-mode=memory <sha>~1..<sha> <repo>` then read
  the `DRIFT.NEW_CLONE` lines; pull both code sides with `git -C <repo> show <sha>:<path>`.

## WHERE THE TRUSTWORTHY RESULTS ARE
- Part D lever table (above) — re-validated under the corrected matcher (conclusions held,
  absolutes shifted ~2-3pp).
- Statement-floor numbers — this checkpoint, §2.
- The 26-FP triage verdicts — chat 2026-06-30 (not all transcribed here; §3 is the summary).

## KNOWN PROBLEMS / TRAPS
- The corpus `klass`/`fp_class` column is the topology source of truth; `added_loc` is often
  ONE-SIDED for cross-file copies — never infer topology from the label text alone.
- Some corpus TP labels credit 2-3 line fragments as copy-paste (NAAb, NexusMiner); treat
  sub-4-statement "recall" with suspicion (the user's stance: that is not copy-paste).
- Re-link the binary before any A/B (a stale binary gave a 518==518 no-op illusion, #158 A+C).

## 2026-06-30 — composition/API-choreography probe + Part B generated fix

User pushed back: do not split the composition idea into a new task while #158 is still open.
The temporary #159 write-up was deleted; the evidence lives here.

### Composition probe — real class, no generic fix yet

Re-ran the current Group-3 gate:

```text
cd experiments/corpus_remeasure_131
python3 group3_precision.py
python3 rescore.py group3_findings.jsonl
```

Corrected matcher result reproduced:

```text
recall      61/140 = 43.6%
suppression 141/166 = 84.9%
precision   61/86 = 70.9%
```

Manual witness checks confirmed the user's idea as a real FP explanation:

- `mgradwohl/tasksmack@05a40b4bd3`: ImPlot/ImGui chart blocks share `BeginPlot`,
  `SetupAxes`, `SetupAxisFormat`, `SetupAxisLimits`, `plotLineWithFill`, hover tooltip;
  the domain data differs (disk read/write vs threads/page-faults, labels, colors,
  formatters, y-axis format).
- `florinzgz/FULL-FIRMWARE-Coche-Marcos@838e686d30`: TFT slider drawing repeats the same
  API choreography with different value source, label, y-coordinate and color.

But the offline probe (`experiments/corpus_remeasure_131/composition_probe.py`,
local ignored experiment) showed simple thresholds are dominated:

```text
family-based:  FP 1 / TP collateral 1
family-free:   FP 2-4 / TP collateral 4-5
call-density:  FP 1 / TP collateral 1-2, then FP 0 when tightened
```

Conclusion: `same calls with different parameters` is real, but `same calls` /
callee-LCS / argument-overlap / call-density must NOT become a generic guard.
If revisited, it needs a narrower API-choreography model (UI/plot/drawing DSL) with
fixtures and a TP-collateral table.

Rechecked on the current post-inline-case corpus state (`group3_findings_INLINECASE.jsonl`):

```text
minCalls=4 calleeLcs>=0.80 argOverlap<=0.35 family>=0.60: FP 1 / TP 1
minCalls=5 calleeLcs>=0.85 argOverlap<=0.65 family>=0.60: FP 2 / TP 2
minCalls=5 calleeLcs>=0.85 argOverlap<=0.75 family>=0.60: FP 4 / TP 5
minCalls=6 calleeLcs>=0.85 argOverlap<=0.75 family>=0.70: FP 3 / TP 3
```

The strict hit is not the desired UI-composition class; it is `NexusMiner`, which the user
classified as real copy-paste. `Lightpad` itself illustrates why a generic "different args"
metric is not enough: the repeated Qt stylesheet builders have high callee-LCS, but the
argument-token overlap remains medium/high because both sides share `theme`, `c`, `inputBg`,
`.name()`, `qMax`, `radius`, etc. This points to a narrower future signal: API-DSL/builder
composition, probably based on repeated `QString`/`.arg` or UI drawing call chains, not on
generic call-sequence similarity.

User clarified a narrower idea: not nested calls in arbitrary expressions, but top-level
call-statement choreography:

```cpp
helper1(1, 2, 3);
helper2(4, 5);
helper3(7, 8);
```

versus the same helper sequence with different payload arguments. Offline probe over current
findings counted only standalone call lines (`callee(...);` / `obj.callee(...);`), then compared
callee-sequence LCS and argument overlap:

```text
strict: minCalls=3 calleeLcs>=0.95 argOverlap<=0.35 diffCallRatio>=0.80 -> FP 0 / TP 0
base:   minCalls=3 calleeLcs>=0.85 argOverlap<=0.55 diffCallRatio>=0.75 -> FP 2 / TP 0
loose:  minCalls=3 calleeLcs>=0.80 argOverlap<=0.70 diffCallRatio>=0.60 -> FP 3 / TP 3
many:   minCalls=5 calleeLcs>=0.85 argOverlap<=0.65 diffCallRatio>=0.75 -> FP 2 / TP 0
```

The base/many hits were `Usagi-dono` (`LOG`/`q.exec` table-setup calls) and `xsscx/research`
(`printf`/`SignatureToFourCC`/diagnostic calls). The user classified both as copy-paste/useful
findings, not suppressible composition. The loose config also catches `FULL-FIRMWARE` TFT drawing,
but the user classified it as copy-paste and it brings 3 TP collateral rows. Conclusion: this
top-level-call version is a good future research direction, but the current corpus does not
contain a clean suppressible example. Do not ship a guard until a real positive example set exists.

### Part B generated — Bison/Flex ordinary-name output

Concrete fix started: generated parser/scanner files under ordinary names (`parser.cpp`,
`parser.h`, `scanner.cpp`) escaped `isGeneratedPath` because they are not `.tab.*` / `lex.yy`.
The real corpus witness is `esrrhs/fakelua`: Bison/Flex output under `src/compile/bison/`
and `src/compile/flex/`.

Fix direction: extend `hasGeneratedHeader` with narrow, top-of-file Bison/Flex skeleton
markers instead of a broad prose marker:

- `A Bison parser, made by GNU Bison`
- `Skeleton implementation/interface for Bison`
- `A lexical scanner generated by flex`
- `#define FLEX_SCANNER` together with `YY_FLEX_MAJOR_VERSION`

Tests added in `file_classification_test.cpp` and `authored_scope_test.cpp`. This should
remove the generated FP class before duplication scanning through the shared
`AuthoredScope` gate.

## 2026-06-30 — follow-up fix pack measured on Group-3

Implemented in-tree, not committed:

- **Generated ordinary-name Bison/Flex output:** `hasGeneratedHeader` now recognizes narrow
  Bison/Flex skeleton banners and the Flex macro/version signature. Covered by file
  classification and `AuthoredScope` tests.
- **Switch-case support for CLASSIC too:** switch skeleton lines (`switch`, bare `case`,
  `default`, `break`) and inline switch-table rows (`case N: action; break;`) are excluded
  from both `normLineSeq` and `normLines`, so the earlier Part-D lever now applies to
  run-path and classic line-overlap scoring.
- **Move detector for `--diff` new-clone drift:** `collectDeletedLines` records deleted-side
  old-line numbers; a new clone touching added lines is suppressed when the same normalized
  fragment content touched deleted parent lines. Covered by unit and e2e diff tests.
- **Composition/API-choreography:** measured again, deliberately **not shipped** as a generic
  guard. The simple models remove FP only with equal/worse TP collateral.

### Corpus result, corrected matcher

Re-run:

```text
cd experiments/corpus_remeasure_131
python3 group3_precision.py
python3 rescore.py group3_findings.jsonl
python3 group3_summary.py
```

Current measured result after the inline switch-table extension:

```text
measured rows: 306
recall      58/140 = 41.4%
suppression 148/166 = 89.2%
precision   58/76  = 76.3%
```

Delta vs `group3_findings_STMTFLOOR.jsonl` using the same corrected `match.py`:

```text
FP removed: 7
TP collateral: 3
```

Removed FP rows:

- `cschladetsch/CppKAI@a25e5d1f` — generated proxy move/coincidental row.
- `esrrhs/fakelua@ace8cfd8` — Bison/Flex generated `parser.cpp/parser.h/scanner.cpp`.
- `fuddlesworth/PlasmaZones@78ee9b04` — whole-file EditorController split/move.
- `graphillion/kyotodd@dbf24972` — whole-file `mtbdd_*` header split.
- `graphillion/kyotodd@9b365935` — whole-file `zdd_adv_*` split.
- `mgradwohl/tasksmack@05a40b4b` — UI composition/API choreography.
- `cometdom/DirettaRendererUPnP@603a2273` — inline switch-table rows (`case N: ...; break;`).

TP collateral rows:

- `danielraffel/pulp@2b9d6f5b` — WASAPI switch-like within-file chunk.
- `deltahdl/deltahdl@30f61166` — elaborator switch/dispatch-like within-file chunk.
- `xsscx/research@34fb052b` — small signature-shift heuristic blocks.

### Remaining fired FP set

18 measured FP rows remain:

- data-table: 5
- idiom/API ceremony: 7
- other: 3
- whole-file/move/split: 2
- generated/SIMD-family: 1

Rows:

- `CrispStrobe/CrispASR@272f7ee0` — loader/weight-table style block.
- `HendrikGC02/Astroray@5d290120` — wavefront RNG header pair.
- `NamecoinGithub/NexusMiner@34f6aa4a` — repeated protocol request block.
- `aethersdr/AetherSDR@081cb1b2` — EQ coefficient/data-table block.
- `alexandrosk0/Smatchet@b6f35116` — consent gate idiom.
- `bobjansen/Ibex@030a73b5` — interpreter variant-ladder idiom.
- `bobjansen/Ibex@a238ed63` — join/interpreter-derived whole-file split still fires via sibling pairs.
- `chrxh/alien@a2e905cf` — Inspector/Inspection rename plus remaining GUI sibling pair.
- `cometdom/DirettaRendererUPnP@635159b3` — SIMD memcpy family.
- `djeada/Lightpad@0e316a95` — UI style table/function family.
- `djeada/Standard-of-Iron@003a0dc5` — renderer family table.
- `esrrhs/fakelua@5a4e6fcf` — compiler extraction/split within new file.
- `esrrhs/fakelua@243bc399` — reverse compiler extraction/split.
- `etkecc/komai@88cd9985` — slash-command data table.
- `florinzgz/FULL-FIRMWARE-Coche-Marcos@5c9a89e8` — RenderContext overload idiom.
- `florinzgz/FULL-FIRMWARE-Coche-Marcos@838e686d` — TFT menu/API choreography.
- `viperx1/Usagi-dono@238159cc` — manager boilerplate/idiom.
- `xsscx/research@26b3110d` — heuristic-library repeated blocks.

### Verification

```text
cmake --build build/debug
ctest --output-on-failure                         # 615/615 passed
./build/debug/src/archcheck src include tests     # No violations found
lizard --CCN 15 --length 30 --arguments 5 --warnings_only src/ include/ tests/
```

`lizard` still returns non-zero because of pre-existing warnings elsewhere; the new
warnings introduced during this fix pack were removed before finishing.

### 2026-06-30 — residual FP review + min-line floor probe

User re-reviewed the 18 remaining labelled FP by looking at real code snippets, not labels.
Result: almost all are legitimate copy-paste or at least useful advisory repeats. The corpus
labels substantially overstate residual noise.

Human verdict from the review:

- copy-paste / useful advisory repeat: `CrispASR`, `Astroray`, `NexusMiner`, `AetherSDR`,
  `Ibex` variant ladder, `Ibex` join/reshape sibling, `alien` GUI pair,
  `DirettaRendererUPnP` `FastMemcpy_Audio*`, `Standard-of-Iron`, `fakelua c_gen`,
  `fakelua` arithmetic/codegen blocks, `komai`, `FULL-FIRMWARE` gauges,
  `FULL-FIRMWARE menu_led_control`, `Usagi-dono`, `xsscx/research`.
- composition / future-signal candidate: `Lightpad` Qt stylesheet builders.
- minor/acceptable noisy edge: `Smatchet` guard boilerplate (extractable common helper,
  but not enough mass to justify a risky generic suppressor now).

Follow-up investigation:

- `fakelua c_gen` is not a random mismatch. The full witness is a large repeated compiler
  code-generation block: both sides extract `SyntaxTreeFunction` / `SyntaxTreeLocalFunction`,
  build parameter lists, iterate math specializations, and emit C signatures. The cropped
  4-line chat snippet made it look unrelated.
- `FULL-FIRMWARE menu_led_control` is also a real repeated UI slider choreography: get config,
  set TFT datum/color, draw label, draw slider background/fill/border, format value, draw value.
- `DirettaRendererUPnP` `FastMemcpy_Audio*` is copy-paste/performance-variant code, not
  generated output. The commit says "SIMD and PCM optimizations"; `docs/SIMD_OPTIMIZATION_CHANGES.md`
  describes AVX2/AVX-512 `memcpy_audio` with prefetch, and the repository includes the upstream
  `FastMemcpy_Avx.h` banner (`FastMemcpy.c - skywind3000@163.com, 2015`). The remaining witness
  is the local AVX-512/audio specialization: aligned vs unaligned 1024-byte loops with identical
  prefetch offsets (`2048`, `2112`, `2176`, `2240`) and fallback chunk thresholds (`512`, `256`,
  `128`). Those are magic performance constants, but the duplication finding is still useful.

Min-line floor idea measured offline over `group3_findings_INLINECASE.jsonl`: filter emitted
findings whose **minimum substantive line count** is below N, then rescore with `match.py`.

```text
minLines=2: recall 58/140=41.4%  FP fired 18/166  precision 76.3%
minLines=3: recall 57/140=40.7%  FP fired 18/166  precision 76.0%
minLines=4: recall 54/140=38.6%  FP fired 18/166  precision 75.0%
minLines=5: recall 53/140=37.9%  FP fired 18/166  precision 74.6%
minLines=6: recall 50/140=35.7%  FP fired 18/166  precision 73.5%
minLines=7: recall 48/140=34.3%  FP fired 17/166  precision 73.8%
minLines=10: recall 43/140=30.7% FP fired 15/166  precision 74.1%
```

Conclusion: default `minLines=5` is dominated. It removes **zero** of the remaining
labelled FP but loses 5 TP rows (`NexusMiner`, `NAAb` x2, `CppKAI`, `kyotodd`). The chat
examples were cropped to 3-4 lines for readability; the actual remaining witnesses are all
larger (minimum substantive line counts: 6, 8, 9, 11, 16, 21, 35, 52, ...). Do not ship a
default line floor of 5. A user-tunable option can still be considered later, but not as
the default precision fix.
