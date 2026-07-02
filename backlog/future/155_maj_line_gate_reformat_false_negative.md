# [SCAN][DUPLICATION] line-overlap gate is fooled by reformatting → drops real reformat-clones (FN)

**Created:** 2026-06-27
**Status:** future — known limitation; needs corpus check before any further work
**Module:** SCAN/DUPLICATION
**Priority:** major
**Difficulty:** M
**Related:** #083 (precision/FP — this is the *recall* counterpart), #070 (FP fix proposals), #107 (external-oracle recall), #052/#056 (token clone detector)

## Result 2026-06-27 — the lcs-rescue does NOT work (corpus-refuted)

Implemented an `lcs`-rescue for the joint floor — `weighted≥0.75 AND (line≥0.50 OR lcs≥τ)`
— and ran baseline-vs-rescue over the **103 FP-corpus repos** present locally (the same
`fp_corpus_r2.tsv` set the line floor was calibrated on). The rescue is **monotonic** (only
adds pairs), so the delta is exactly the newly-admitted pairs. τ-sweep from a single
`rescue=0` probe run (29 146 candidate pairs with `line<0.5`, baseline = 27 527 pairs):

| τ (lcs) | new pairs admitted | total vs baseline |
|---------|--------------------|-------------------|
| 0.80    | 27 240             | **+99.0 %** |
| 0.90    | 20 016             | +72.7 % |
| 0.95    | 13 969             | +50.7 % |
| 0.99    |  8 936             | +32.5 % |
| 1.00    |  8 452             | **+30.7 %** |

Even at τ=1.00 (token sequences *identical* after `id`-normalization) the rescue inflates
output by **+31 %**. No threshold recovers the target reformat-clone without a large FP flood.

**Root cause:** the line floor's FP-suppression value comes substantially from sensitivity to
per-line *literal identifiers*. Two idiom instances (same skeleton, different names) differ
line-by-line but share the normalized token sequence, so normalized LCS scores them ~1.0 and
re-admits exactly the idiom/boilerplate FP class the floor exists to kill. `lcs` (on
normalized `id` tokens) discards the very signal doing the work.

**Decision:** reverted. The FN below is real but stays **unsolved** — do not ship a
normalized-LCS rescue. Untested future angle: a *raw-token* (identifier-sensitive) LCS
rescue, which would pass pure reflows but not renamed idioms — but the target pair is itself
partly renamed (`f`/`face`, a factored local), so raw-LCS may not recover it either. Needs its
own corpus check before any work.

**Disposition (2026-06-28):** moved to `future/` as a known limitation. The reformat-FN class
is narrow and not a v0.1 blocker. Likely requires the v0.2 semantic layer to solve cleanly.
Do not revisit without: (a) a corpus check confirming raw-token LCS actually recovers the
target pair, and (b) verifying it does not re-open the idiom-FP class.

---

## Original write-up (the FN that motivated this)

### The concrete case (verified 2026-06-27 on the leadline repo)

```
gallery_export.cpp  pushFace(dst, mesh, f)        <->  viewer_data.cpp  addFeatureFace(dst, mesh, face)
   weighted = 0.868   line = 0.364   lcs = 0.866
```

Same function — "take a `Stage4Mesh` + face index, push 3 world-space `Vec3` for the triangle
vertices". The only difference: `viewer_data` binds a local `const Point &p` and does
`push_back` on one line; `gallery_export` inlines `v[...]` and spreads the same `push_back`
across three lines. The token metrics see through it (`weighted=0.868`); the **line metric**
(`lineOverlap` = Jaccard over whitespace-collapsed source lines, `similarity.cpp:67`) sees
"1-line vs 3-line `push_back`" as disjoint lines → `line=0.364`, below the 0.50 floor.

The joint floor `phase8JointTokenOrderFloor` (`duplication_scanner.cpp`) is an **AND**:
`weighted≥0.75 AND line≥0.50`. So the line gate hides a pair the token metric already flagged.
A false negative on a trivial reformat (clang-format reflow at the 120-col boundary).

### Why the line gate exists (don't just delete it)

Its documented purpose is to reject **bag-of-words collisions** — and, as the corpus run
above showed, also identifier-distinct **idiom** repeats. Dropping it re-opens that FP class.

### How mature detectors handle reformatting (for reference)

- **Token-stream tools** (PMD CPD, SonarQube CPD, CCFinder, SourcererCC) compare the token
  *sequence* and ignore line boundaries; a Type-1 clone is defined as identical "except for
  whitespace and layout", so reformatting is normalized away by construction.
- **NiCad** pretty-prints to a canonical layout (one statement per canonical line) *before*
  the line-wise compare — the user's "join the wrapped statement back" intuition, generalized.

The honest takeaway from this task: our token metrics already are reformat-immune; the FN
only survives because the line floor is an additional, identifier-sensitive filter — and that
sensitivity is load-bearing for precision, so we can't cheaply route around it. Surfacing this
FN class likely requires the v0.2 semantic layer, not a metric tweak.

## Sources

- NiCad — flexible pretty-printing + code normalization:
  https://www.cs.usask.ca/~croy/papers/2009/Roy_ICSM2009_Clone.pdf
- SonarQube CPD (token-based, Type-1 ignores whitespace/layout):
  https://www.sonarsource.com/blog/manage-duplicated-code-with-sonar/
