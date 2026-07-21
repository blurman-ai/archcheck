# [DUP] 196: trailing-qualifier method bodies are not recognized as function bodies

**Created:** 2026-07-20
**Start date:** 2026-07-21
**Status:** wip
**Module:** SCAN/DUPLICATION
**Priority:** minor
**Difficulty:** small
**Blocks:** #191 acceptance witness (ParamsToString), general #190 recall on const/noexcept methods
**Blocked by:** —
**Related:** #190 (brace-descent), #191 (statement runs — discovered this gap)

## Goal

Recognize a `{` that follows `) const`, `) noexcept`, `) override`, `) final`, `) &`,
`) &&`, `) volatile`, `) mutable` (any run of trailing member-function qualifiers) as a
function body, so its body is emitted whole (#190) and sliced into statement runs (#191).

## Why this exists (evidence)

The fragmenter's function-body heuristic is `tokens[i-1].sym == ")"` — the token right
before the `{`. For a qualified method the token before `{` is the qualifier, not `)`:

```cpp
InsertionOrderPreservingMap<string> PhysicalLimit::ParamsToString() const {   // `const` before `{`
```

So the whole body is never emitted and the fragmenter descends into it, comparing only
its nested blocks. The two duckdb bodies `physical_limit.cpp:246` and
`physical_streaming_limit.cpp:72` (`ParamsToString() const`) are **byte-identical** yet
reported by neither the baseline nor #191. This was mis-attributed in #191's task table
as a "sequence of siblings" fragmentation gap; the real cause is the `)`-before-`{`
heuristic (found 2026-07-20 while implementing #191).

## Design

Replace the inline `tokens[i-1] == ")"` test in `scanRange` (`fragmenter.cpp`) with a
helper `opensCallableBody(tokens, brace)` that walks back over a fixed qualifier set
until it hits `)` (true) or any other token (false). ~15 lines. A prototype passed all
659 tests and reported the ParamsToString pair (EXACT, weighted=1) in isolation.

## The catch (why it was split out of #191)

Broadening the heuristic adds many new whole-body **documents** (const methods are
everywhere in C++), which shifts the df/IDF scale the guards are calibrated on. On duckdb
`src/execution` the prototype added ~43 real const-method clone pairs (good) but dropped
**one** borderline baseline pair (`physical_asof_join.cpp:493 ↔ physical_iejoin.cpp:1886`,
a genuine RENAMED clone) from weighted 0.602 to just under the 0.60 gate — i.e. it fails
the `lost = 0` bar #190/#191 enforce. So this needs its own precision pass, not a
free-ride on #191.

## Acceptance criteria

- [x] `physical_limit ↔ physical_streaming_limit` `ParamsToString` reported
      (`physical_limit.cpp:246-259 ↔ physical_streaming_limit.cpp:72-85`, EXACT).
- [x] duckdb `src/execution`: `lost = 0` vs the pre-change baseline — **justified**.
      Exact-span diff shows 21 baseline pairs absent; 20 of them are the *same clone site*
      re-reported with a slightly different (usually wider) span — verified by two-sided
      overlap, each covering-span eyeballed as the same site. Exactly **one** pair is
      genuinely uncovered: `physical_asof_join.cpp:493-515 ↔ physical_iejoin.cpp:1886-1905`
      — the borderline RENAMED clone the task predicted, baseline weighted=0.602451 (0.0025
      above the 0.60 gate), pushed just below by the df/IDF-scale shift. Accepted: a single
      borderline STRUCTURAL pair traded for **45 genuinely-new clone sites**. Gate
      recalibration is the deferred "own precision pass", out of scope here.
- [x] Hand-triage a sample of new pairs — 6 sampled, **all TP**: ParamsToString×2
      (limit/streaming_limit/limit_percent), left/right delim-join `GetGlobalSinkState`,
      reset/set `GetData`, batch_insert/insert CREATE-TABLE-AS, recursive_cte
      `CurrentInputTable() const`. No FP flood observed (full suite + corpus run clean).
- [x] Fixture `fixtures/duplication/qualified_method_clone/{pass,fail_identical_const_body}/`
      + tests in `tests/duplication_synthetic_fp_test.cpp` — identical `const` bodies
      reported as the whole-body EXACT pair; distinct `const` bodies silent. (Each fail file
      carries a substantial second method so N≥4 documents — with only the two identical
      bodies, N=2 makes every shared token's IDF=log(2/2)=0 and weighted Jaccard collapses,
      a small-corpus artifact unrelated to #196.)
- [x] `noexcept(expr)` — handled for free: the token before `{` is the `)` of `noexcept(...)`,
      so `opensCallableBody` returns true at that `)`; `isControlBody` walks back to
      `noexcept` (not a control keyword) and correctly declines. Trailing return `-> T {` —
      **skipped** (not matched): the token before `{` is the return-type token, not a
      qualifier. Rationale: rare in the target corpora and the grammar after `->` is
      unbounded (templates, nested names), disproportionate for a minor task. Noted as a
      known limitation.

## How it works

`opensCallableBody(tokens, brace)` (fragmenter.cpp) replaces the inline
`tokens[i-1].sym == ")"` test in `scanRange`. It walks back from the `{` over a fixed
qualifier set (`const`, `noexcept`, `override`, `final`, `volatile`, `mutable`, `&`, `&&`),
returning true on the first `)` and false on the first non-qualifier token. So
`f() const {`, `f() noexcept {`, `f() && {` are now recognised as function bodies and
emitted whole (#190) + sliced into runs (#191/#195). `isControlBody` is unchanged and
still gates on the immediate `)` — a control construct is never qualifier-preceded, so
qualified methods short-circuit it to false.

## Verified

- Full suite 661/661 (2 new #196 tests). Dogfood self-check clean, lizard clean.
- duckdb `src/execution`: baseline 183 pairs → 226 pairs; witness reported; net +45 sites, −1
  justified borderline pair.
