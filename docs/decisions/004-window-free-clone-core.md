# ADR-004: Window-free clone detection core (maximal matching token runs)

**Status:** accepted (2026-07-24, task #199 — author waived formal review, directed straight to prototype)
**Origin:** [backlog/wip/199_maj_window_free_clone_detection.md](../../backlog/wip/199_maj_window_free_clone_detection.md),
Duplo comparison #194/#198, [docs/duplication_architecture.md](../duplication_architecture.md) §3

## Context

The token-pass clone detector fragments each function body into fixed token windows
(`minTokens`/`2×`/`4×`, [fragmenter.h](../../include/archcheck/scan/duplication/fragmenter.h))
and scores fragment-vs-fragment with weighted Jaccard. The window size decides what
aligns, so results depend on an internal implementation detail:

- **Non-monotonic in min size.** Raising `minTokens` 30→50 on our own `src/` gives
  35→18 pairs, but the sets are not nested: a 19-line real clone (`diff_query`) vanishes
  while 2 new pairs appear (clones that straddled 30-windows fit one 50-window). A
  user-facing "minimum clone size" (#200) cannot be honest while the core re-fragments
  under it.
- **A stack of layers exists only to compensate windowing.** #190 (nested fragments),
  #195 (boundary runs), #191 (statement runs), the #091 `maxTokens` 400→600 bump — each
  patches a class of clones the window boundaries hid. Two classes are still open:
  #196 (`ParamsToString() const` — the `)`-before-`{` body heuristic misses
  trailing-qualifier methods entirely) and #197 (class-body member-decl runs — outside
  any function body, never fragmented).
- **External evidence (#194/#198).** Duplo and PMD-CPD, both window-free
  (line/token maximal matching), are stable and monotonic in their min-size knobs;
  the archcheck-vs-Duplo set overlap is threshold-fragile exactly where windowing
  decides alignment.

This is a root design flaw, not a tuning problem: no window size fixes alignment that
depends on the window.

## Decision

Replace the fragment-vs-fragment core with **maximal matching token runs** over the
*normalized* token stream (the established CCFinder / PMD-CPD / Duplo model):

1. **Keep the lexer + selective normalization unchanged** (`token_normalizer.cpp`,
   keep-calls + case labels — the precision asset carries over verbatim).
2. **Match maximal exact runs** of normalized tokens across the whole stream (suffix
   array / hashed k-gram seeds + maximal extension — the spike used seeds; the product
   choice is an implementation detail, the *output* is the set of maximal runs, which
   no threshold influences).
3. **Type-3 via gap-stitching:** merge co-linear runs separated by small edits into one
   reported span (the spike shows real clones arriving as 2 adjacent runs with a tiny
   gap — `diff_query` 20-40↔42-60, `xml_utils` 347-367+368-375).
4. **`--min-lines`/min-tokens becomes a pure post-filter** over the run/span set —
   monotonic by construction (#200 becomes honest end-to-end).
5. **Classifier re-derived on aligned runs:** positions in an exact normalized run align
   1:1 (no LCS needed) — raw lexemes equal → EXACT, id-substitutions → RENAMED,
   lit-substitutions → LITERAL; **stitched spans with edits → MIXED (Type-3 diverged
   copies — the core product, kept via stitching)**. Only *loose no-substantial-run*
   matches (shared shape, not an actual copy) fall away, and that boundary is a tunable
   recall dial (see trade-off), not an abandonment of diverged-copy detection.
6. **Precision filters port as span post-filters** (see migration map).

The two-backend split (ADR-003) is untouched: this rewrites the token-pass core, not
the backend architecture.

## Spike evidence (2026-07-24, `experiments/window_free_spike/`, gitignored sandbox)

A ~250-line prototype linking the **product lexer** (identical normalization), k-gram
seeds + maximal extension + containment dedup:

| Check | Window model | Window-free spike |
|---|---|---|
| min-size 30→50 on `src/` | non-monotonic (35→18, losses + new pairs) | strict subset, 0 violations (522→236 runs) |
| `diff_query` 19-line clone | lost at 50 | present at 30 and 50 (2 adjacent runs, stitch candidate) |
| JiraClient 587↔884 cross-fn prologue (#195's witness) | needed the boundary-runs layer | native 115-token run |
| SmatchetUI 532↔541 adjacent twin | caught | caught (tandem-aware same-file guard, see risks) |
| duckdb `ParamsToString() const` (#196, open bug) | **missed** | native 133-token run 242-262↔68-88 |
| xml_utils 348↔383 Type-2 renamed (#194 flagship) | caught | caught; with stitching = the full function pair 347-375↔382-410 (MIXED, matched=99%) |
| member-decl runs (#197, open) | missed (fragmenter scope) | in-stream by construction (structural property, not yet witnessed) |
| duckdb `src/execution` wall (205 files) | 33 s | 2.9 s core incl. stitching (no filter phases yet — bound, not claim) |
| A/B on `src/` vs the current binary (24 pairs) | — | 22/24 covered; both losses are STRUCTURAL (see trade-off); new true finds incl. the `check_command↔main` identical `catch(ConfigError)` blocks |

Gap-stitching verified end-to-end: the `diff_query` witness reports as one span
20-40↔42-60 (MIXED, runs=2, matched=96%) — the full 19-line clone the window model
loses at `minTokens` 50. Three stitching lessons the product core must inherit:
(a) maximal runs on both sides of a point edit *overlap* by a few tokens (both
overrun the edit), so the stitch gap must admit small negatives; (b) tandem twin
families (A~B~C adjacent in one file) chain-stitch into a self-overlapping span
unless a same-file span is forbidden from crossing its own partner's start;
(c) hand-verified loss class: scattered micro-edits every 2-3 lines
(`currentLineNo++` → `++currentLineNo`, diff_query 129↔165) fragment runs below
seed length — irrecoverable by stitching, this is the STRUCTURAL cost made concrete.

## The trade-off — NOT "drop diverged copies"

**First, what is NOT on the table.** The detector's entire job is Type-3 / diverged
copy-paste — *edited copies* (SSOT `duplication_architecture.md` §2: the sole retained
detector; §12: "the core"). Keeping copy-paste that was then edited a little is the
product, not an option. **Window-free keeps it, and it is the majority of the output**:
on `src/` the 96 window-free spans are 5 EXACT (verbatim) + 32 RENAMED (Type-2) + 12
LITERAL (a constant changed) + **47 MIXED (logic edited / lines inserted — Type-3)**.
A copy with a renamed variable, a changed constant, a flipped operator, or an inserted
line is caught: the identical stretches on each side of the edit are maximal runs, and
**stitching** merges them into one span with the edits inside (verified: `diff_query`
20-40↔42-60 MIXED matched 96%, `xml_utils` 347-375↔382-410 MIXED 99%). No diverged copy
is abandoned.

**What is actually at stake is narrower.** The old "STRUCTURAL" label (weighted ≈
0.5–0.8) covered two very different things under one band:
1. *real edited copies* — window-free keeps these as MIXED/LITERAL (above);
2. *loose "same shape, not a copy" matches* — two blocks sharing vocabulary/idioms with
   no substantial identical stretch (e.g. `diff_query` 129↔165: the `+`-line vs `-`-line
   diff handlers, a shared tail but different heads, old weighted 0.49). This is the
   genuine fuzzy end — half real, half idiom-FP (#159 accidental clones, the weakest
   #198 corroboration).

Only class (2) is shed by the exact-run core, and even that is **a tunable recall dial,
not a product sacrifice**: how short a matching stretch to trust (the seed length) and
how far to stitch across edits. Lower the dial → catch denser-edited copies, at the cost
of more loose-shape noise; higher → cleaner. This is set by measurement against
copy-paste witnesses, the same way the idiom filter was — it is engineering, not a
choice to hand the user.

The tuning + idiom-filter effect must be *quantified against copy-paste witnesses before
the default flips* (#198 multi-repo, hand-verified): which class-(2) matches are real
edited copies we still want vs idiom-FP we do not.

## Migration map

| Existing layer | Fate under the window-free core |
|---|---|
| lexer + selective normalization (§6 SSOT) | **unchanged** — carried over verbatim |
| fragmenter (#190 nested, #195 boundary, #191 statement runs) | **retired** — sub-function, prologue/epilogue, and mid-function runs are native maximal runs; no fragmentation, no compensating layers |
| `)`-before-`{` heuristic (+ #196 gap) | **retired** — no function-boundary heuristic at all; #196/#197 fixed as a side effect |
| inverted index, rare-df, winnowing fingerprints (#092) | **replaced** by the run-matching structure (seeds/suffix array) |
| bag overlap + weighted Jaccard + token-LCS confirm | **replaced**: exact runs need no confirm; stitched spans reuse the diff/LCS machinery for classification only |
| classifier EXACT/RENAMED/LITERAL/MIXED | **re-derived** on aligned runs (simpler: 1:1 positions) |
| P0.2 whole-file suppress, P0.9 generated, vendored excludes | **ported** as span/file post-filters (unchanged semantics) |
| P0.5 canon, P0.6 joint floor, phase7 same-function | P0.5 ported; P0.6 obsolete (no scores to floor — min-size replaces it); same-file handling becomes token-based (see risks) |
| P1.1 data-table, P1.2 boilerplate, P1.4 file-local IDF, diversity guards | **ported** as span post-filters — they carry the precision load at small sizes |
| `--diff` / new-clone drift, baseline | consumer-side: spans instead of fragment pairs; parent-guard logic unchanged |
| #200 `--min-lines` | becomes intrinsic + monotonic (the point of this ADR) |

## Consequences and risks

- Five compensating mechanisms retire; two open bug classes (#196, #197) close as side
  effects. `duplication_architecture.md` (SSOT) gets rewritten in the same PR that
  lands the core; all `fixtures/dup*` re-baselined consciously, plus a monotonicity
  fixture (min-size N ⊂ N−k).
- **New guards the spike proved necessary:** (a) preprocessor-directive mask — the
  fragmenter was an implicit scope filter; without one, `#include` preambles flood the
  output as shifted RENAMED ladders; (b) tandem/adjacent-twin handling — same-file
  guards must be token-offset-based, extension capped at the pair distance; a
  line-based guard silently killed the copied-right-below-the-original pattern (the
  P0.4 lesson repeated in one spike); (c) idiom-clique management at report level
  (dense clique collapse) — the raw run floor at 30 tokens is high (7 757 runs on
  duckdb execution vs 183 shipped pairs; 322 at 80 tokens); the min-size default and
  ported P1 filters, not the core, set the output volume. On `src/`, `--min-lines 6`
  alone cuts 425 spans to 122 (monotonically); the P1 filters are not in the spike
  and cut further.
- #198's comparison numbers are the fixed baseline the rewrite is measured against
  (same inputs, same witnesses, per-bucket hand-verification) — lost-TP must be
  enumerated, not aggregated.
- Staging: (1) core lands behind a flag next to the window core; (2) corpus A/B vs the
  #198 baseline; (3) filters ported; (4) default flips and window layers are deleted.
  No step flips by "the run finished" — each has witness-level acceptance.

## Implementation status (2026-07-24/25, behind `--duplication --window-free`)

Stages 1–3 landed (uncommitted). Stage 4 (default flip + layer deletion) is deliberately
NOT done — it hinges on the STRUCTURAL trade-off and idiom-filter calibration, both author
calls.

**Stage 1 — core.** `src/scan/duplication/token_run_matcher.{h,cpp}` (reuses `lex()`),
wired via `ScannerOptions::windowFree`. Emits `Fragment`+`Pair`; reporter/`--diff`
unchanged. All spike guards ported (pp-mask, tandem guard, negative-gap stitch,
containment dedup, EXACT/RENAMED/LITERAL/MIXED classify). Unit test + monotonicity test.

**Stage 2 — min-size + inherited excludes.** `--min-lines N` is a mode-agnostic output
filter (`filterByMinLines`, monotonic by construction) — delivers #200's interim surface
too. Vendored/**generated** exclusion needs NO port: `collectNonVendoredSources` (#129
gate) already drops them before scanning, so window-free inherits it.

**Stage 3 — A/B evidence (window core vs `--window-free`, both via the shipped reporter):**

| Input (min-lines) | window core | window-free | window covered by w-free | window-only (dropped) |
|---|--:|--:|--:|--:|
| `src/` (6) | 17 | 124 | **15/17** | 2 |
| Smatchet (8) | 8 | 45 | **6/8** | 2 |

- **Every window-only drop is STRUCTURAL** — the low-overlap / scattered-micro-edit band
  the core sheds by design. Hand-verified: `diff_query.cpp:129↔165` (`x++`→`++x` every
  2–3 lines), `include_scanner.cpp:73↔105`, Smatchet `JiraClient` 684↔1237,
  `SpreadsheetState.h` member block. This is the ADR trade-off made concrete, not a bug.
- **The adds are a genuine MIX (hand-read, per bucket):**
  - *Real TP the window core structurally cannot see* — cross-file duplicated
    `catch(ConfigError)` block (`check_command`↔`main`, EXACT), `write_nodes_block`↔
    `write_edges_block` parameterizable twins (`graph/baseline.cpp`), a copy-pasted
    nearest-by-line search (`local_complexity_drift`), JiraClient request-handling
    repeats. This is the core's value proposition, delivered.
  - *Newly-surfaced idiom FP* — the fragmenter was ALSO an implicit **function-body-scope
    + rarity** filter; removing it surfaces (i) namespace-preamble declaration runs
    (`namespace X { namespace { constexpr ... }` — different constants, same skeleton),
    (ii) interface-signature boilerplate (`ViolationList X::check(const DependencyGraph&,
    ...) const {` repeated across every rule — the #159 accidental-clone class), (iii)
    some member-declaration runs (#197 — sometimes TP, sometimes idiom).
- **Idiom-FP filter — two candidates built and measured against witnesses (src/, min-lines 6):**

  | guard | pairs | idiom-FP killed | TP survived |
  |---|--:|---|---|
  | none | 125 | — | all |
  | **rarity** (M≥1 rare anchor, df ≤ 12% files) | 68 | namespace-preamble ✓, `check()` sig ✓, keyword-table ✓ | catch ✓, dist-search ✓ — **but write_nodes/edges & writeAdded/writeGrown DROPPED** ✗ |
  | **control-flow** (span must branch/return) | 96 | namespace-preamble ✓, `check()` sig ✓, keyword-table ✓ | **all writer/catch/dist TP survive** ✓ |

  Rarity is the SSOT-endorsed lever (§3.3/P1.4) but it is **blunt**: the "parameterized
  writer/helper" TP class (`write_nodes_block`↔`write_edges_block`, `writeAdded`↔`writeGrown`)
  shares no rare token, so rarity kills it — the exact TP/FP overlap SSOT §5/§Level 3 warns of,
  and killing it kills the tool's flagship "missing reuse edge". The **control-flow guard is
  the clean discriminator**: the idiom-FP classes are all *non-executable* (declarations, data
  tables, signatures); a real copied body branches or returns. It removes the idiom-FP AND
  spares the writer TPs.

- **Decision (shipped default of `--window-free`):** `requireControlFlow = true`,
  `minRareAnchors = 0`. Rarity stays available (`--rare-anchors N`) as an aggressive knob.
  **Cost of control-flow (measured, honest):** it drops control-flow-free real clones —
  straight-line void functions and pure member/enum declaration runs (the #197 class the raw
  core surfaces). On src/ that cost is 0 window-covered pairs; on Smatchet it is 1 (6/8→5/8).
  Recoverable via `--no-control-flow`. This is a frequency trade: idiom-flood (every file/rule)
  ≫ control-flow-free clones (rare), so cf-on is the better default. The residual idiom floor
  and the #197-vs-idiom ambiguity are the LLM-confirm tier's job (SSOT §Level 4), not a snapshot
  filter's.

**Stage 3 filters — DONE (not deferred).** Built + witness-verified both the rarity guard
(`--rare-anchors`) and the control-flow guard; chose control-flow as the clean default (table
above). The calibration is empirical, not guessed: each candidate was measured against the
named TP/FP witnesses, and the one that killed a flagship TP class was rejected.

## Duplo validation — "not worse than Duplo" (2026-07-25)

Checked window-free against the external oracle **Duplo v2.3.10** (`-ml N -ip -json`,
line-based) on 4 diverse repos. Fairness (#198): Duplo blocks restricted to files archcheck
actually scans (dumped `collectNonVendoredSources`), and a phantom guard drops Duplo blocks
whose text is not present at both endpoints.

| repo | Duplo (fair) | WF default (cf) | WF `--no-control-flow` |
|---|--:|--:|--:|
| archcheck `src/` | 7 | 7/7 | 7/7 |
| Smatchet | 7 | 6/7 | 6/7 |
| duckdb `src/execution` | 23 | 23/23 (1 Duplo phantom caught) | 23/23 |
| 3drepo bouncer | 46 | 35/46 | 46/46 |
| **total** | **83** | **71/83 (86%)** | **82/83 (99%)** |

**Verdict (revised after 9 repos, hand-verified): NOT uniformly not-worse — a real recall gap
exists on repeated code.** Aggregate raw recall is 1768/1848 = 96%, but the average hides the
gap. The 80 `--no-cf` misses split into Duplo noise (includes, license headers, data tables,
signatures, stubs — correctly ignored) and **genuine misses of real verbatim clones in
repeated/self-similar regions**: `nanovdb/math/Ray.h:403-423↔435-455` (region covered by ZERO
window-free pairs), `ParmFactory.cc:1163↔1218`, `QuantizedUnitVec.cc` (×5, periodic), gladius
codegen. On UNIQUE clones window-free is ≥ Duplo and finds far more (Type-2/cross-function); on
REPEATED blocks it under-reports. Root cause = dedup/tandem/extension over-collapsing
self-similar cliques — the same thing that blew span count to 117,237 on openvdb (257-min
batch). Recall gap + perf are one problem → **#201**, and the gate on flipping the default.
(The earlier "82/83 not worse" was 4 repos read off the aggregate; superseded.)

**The default cf-guard's 12-block gap is characterized (hand-read):** all non-executable —
(a) JSON state-transition/data tables (rapidjson), (b) decl/def signature duplication
(`.cpp` param list == `.h` param list — normal C++, a Duplo FP), and (c) **real #197
parallel-class member-declaration clones** (`data_processor_dgn.h`↔`dwg.h`, an identical
`convertTo3DRepoMaterial(...)` decl — a genuine extract-a-base-class edge). So cf-on is not
free: it sheds Duplo noise (a,b) but also a real actionable class (c), recoverable via
`--no-control-flow`. This is the same trade the STRUCTURAL/#197 decision turns on.

**Two self-check catches on the harness (recorded so the number is trustworthy):** (1) the
phantom guard first sized its search window by `len(Lines)` not the endpoint span, so it
wrongly dropped 2 real *gapped* Duplo blocks — fixed to use the actual span; (2) without the
fairness fileset, 3drepo read as 8% because Duplo scanned the vendored `bvh/` library (MIT
header) archcheck excludes — the dump-based fileset is the correct control.

## Still not done, and why

**P0.2 whole-file suppress** (needs coverage logic, 0 groups on
`src/`, matters on corpus moves); default flip + window-layer deletion + SSOT rewrite — this
one **hinges on the STRUCTURAL trade-off** (dropping the fuzzy band changes the product/
marketing framing) and on a full corpus A/B vs #198. That is the one genuine product decision
left; everything mechanical is done and behind `--window-free`.
