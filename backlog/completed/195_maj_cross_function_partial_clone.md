# [SCAN] Detect cross-function partial clones (shared prologue/suffix)

**Created:** 2026-07-20
**Start date:** 2026-07-20
**Status:** completed (2026-07-21) — witness now reported; boundary-run layer confirmed load-bearing (see Closure)
**Module:** SCAN
**Priority:** major
**Difficulty:** unknown
**Blocks:** —
**Blocked by:** —
**Related:** #190 (subfunction_clone_pass), #191 (statement_run_clone_pass), #194 (compare_clone_detectors)

## Goal

Detect copy-paste that is a contiguous block **shared across two otherwise-different functions** (e.g. an identical prologue or suffix), which the current fragment-level comparison misses because the two whole functions score below the similarity threshold.

## Context

Found while comparing archcheck against PMD CPD (#194). Concrete witness:
`Smatchet@f8368a23 Source_Core/src/JiraClient.cpp:587↔884` — a real 17-line block
that is the shared prologue of two different functions (`FetchUsers` /
`FetchFieldCatalog`, differing only in `outUsers`→`outComponents`). **PMD's
sliding token window catches it; archcheck misses it.**

Root cause: archcheck compares whole function-fragments. Two functions that share
a 17-line head but diverge afterward have low overall similarity → the pair falls
below threshold and is never reported. #190 detects copy-paste nested **within
one** function body; it does not cover a block **shared across two** functions.
#191 (statement-run pass) is the closest kin — decide whether this is a #191
extension or its own pass before implementing.

This is a Type-1/Type-2 recall gap, not a precision issue. The risk in fixing it
is the usual one: sub-fragment / sliding-window matching tends to explode the
candidate-pair count and surface low-value tiny matches. Any fix must hold the
line on precision (min block size, weighting) — see the #194 write-up on how
PMD's identifier-blind mode floods output ~4×.

See [docs/research/clone_detector_comparison.md](../../docs/research/clone_detector_comparison.md)
and [docs/duplication_architecture.md](../../docs/duplication_architecture.md).

## Execution plan

- [x] Decide: implement a bounded boundary-run layer, not the general #191 statement-run pass.
- [x] Add a fixture reproducing the shared-prologue case (fail_*) + a pass/ that must stay clean.
- [x] Implement prefix/suffix matching across otherwise-different function bodies with a min-block guard.
- [x] Verify against the real witness (Smatchet@f8368a23 was cloned in #194's scratchpad) — **FAILS, see below**.
- [x] Update duplication_architecture.md with the new layer's semantics and FP boundaries.

## Done

- Added bounded boundary fragments for emitted function bodies:
  `min_tokens`, `2*min_tokens`, `4*min_tokens` prefix/suffix spans.
- Boundary fragments are marked `nested`, so they do not affect df/IDF,
  fingerprint frequency caps, or whole-file clone ratios.
- Added a boundary-only fallback from zero weighted score to plain Jaccard. This
  handles the two-document prologue case where every copied token has IDF 0.
- Added overlap deduplication after containment so shifted boundary windows report
  one strongest pair per copy site.
- Added fixtures:
  `fixtures/duplication/cross_function_partial_clone/fail_shared_prologue/`
  and `fixtures/duplication/cross_function_partial_clone/pass/`.

## Verification (2026-07-20, via #194 — reopens this task)

The motivating witness **was** available (Smatchet@f8368a23 cloned in #194's
scratchpad). Ran the current #195 binary against it:

- **`JiraClient.cpp:587↔884` (FetchUsers/FetchFieldCatalog shared prologue) is STILL NOT reported.** The exact case this task was filed for is not caught.
- The **synthetic fixture passes** (`fields.cpp:3-12 ↔ users.cpp:3-12`, EXACT w=1) and `[duplication]` tests are green — but the fixture is **degenerate**: the shared block *is* the whole 10-line function, so it never exercises the real condition (a short prologue as a small **prefix of a large, otherwise-different, partly-renamed** function inside a real multi-file corpus). Classic "fixture passes ≠ feature works".
- **Dogfood is positive though:** on archcheck's own `src/` the boundary-run layer adds **19 real cross-function/prologue clones** (13→32 pairs), hand-checked as true (e.g. `sf7_using_namespace.cpp:89-91 ↔ sf8_include_guard.cpp:157-159` identical loop preamble; `duplication_scanner.cpp:118-122 ↔ 139-143`). No new FP violations (self-check gate stays 0). So the layer is *useful*, it just doesn't reach the target case.

**Likely cause (hypothesis for the fix):** in the real witness the 17-line prologue is a small fraction of a ~100-line function, the first line is renamed (`outUsers`→`outComponents`), and with ~92k candidate pairs the boundary fragment pair scores below threshold. The `min/2×min/4×min` prefix windows may not align to a 17-line span, or IDF weighting on common C++ prologue tokens sinks the score. Needs: (a) a **realistic fixture** (short prologue inside a large divergent function), (b) recheck window sizing / the boundary score fallback for the non-IDF-0 real-corpus case.

## In progress

- Reopened. The layer works generally but misses the filed witness — needs a realistic fixture + score/window fix.

## Next steps

1. If an external checkout for the JiraClient witness is available, verify that
   `Smatchet@f8368a23 Source_Core/src/JiraClient.cpp:587↔884` now reports.

## Key decisions

| Decision | Reason |
|---------|---------|
| Boundary-run layer, not #191 | #195 is only shared function prologue/suffix; arbitrary middle statement-runs remain #191 and need separate cost/FP calibration. |
| Boundary fragments are `nested` | Their text is already counted by the enclosing body; keeping them out of document statistics preserves #190's df/IDF invariant. |
| Zero weighted score falls back to plain only for boundary pairs | Shared prologues can have IDF 0 in small corpora; broad fallback would weaken calibrated weighted scoring. |

## Changed files

| File | Change |
|------|-----------|
| `include/archcheck/scan/duplication/fragmenter.h` | Added `Fragment::boundary` and `FragmentOptions::boundaryRuns`. |
| `src/scan/duplication/fragmenter.cpp` | Emits bounded prefix/suffix boundary fragments for function bodies. |
| `src/scan/duplication/duplication_scanner.cpp` | Enables boundary runs, boundary score fallback, span-balance and overlap dedup. |
| `tests/duplication_synthetic_fp_test.cpp` | Added #195 fail/pass fixture tests. |
| `fixtures/duplication/cross_function_partial_clone/` | Added shared-prologue and short-common-prefix fixtures. |
| `docs/duplication_architecture.md` | Documented boundary-run semantics and boundaries. |
| `docs/openwiki/index.md` | Updated duplication subsystem fixture/test pointer. |

## Fixtures (if a rule)

- [x] `fixtures/duplication/` shared-prologue fail case
- [x] pass/ case that must stay clean (two functions with a coincidental short common head)

## How it works

The fragmenter still emits normal function/body fragments first. In the duplication
scanner path only, it additionally emits bounded prefix/suffix fragments for each
emitted function body when the body has at least another `min_tokens` outside the
boundary span. These synthetic spans flow through the existing index, scoring,
classification and reporting pipeline.

## What controls it

`FragmentOptions::boundaryRuns` controls emission and is enabled by
`scanForDuplication`. The size guard is derived from `FragmentOptions::minTokens`,
so lowering the global duplication floor also lowers the boundary-run floor.

## What it relates to

#190 covers brace-delimited nested blocks. #195 covers function prefix/suffix spans.
#191 remains the broader arbitrary statement-run/sliding-window task.

## Diagnostics

- `cmake --build build/debug` — passed.
- `./build/debug/tests/archcheck_tests "[duplication][fixtures]"` — passed.
- `./build/debug/tests/archcheck_tests "[duplication]"` — passed.
- `./build/debug/tests/archcheck_tests` — passed, 657 test cases / 4789 assertions.
- `./build/debug/src/archcheck src include tests` — passed, no violations.
- `./build/debug/src/archcheck --duplication fixtures/duplication/cross_function_partial_clone/fail_shared_prologue`
  — reports one pair, `fields.cpp:3-12 <-> users.cpp:3-12`.
- `./build/debug/src/archcheck --duplication fixtures/duplication/cross_function_partial_clone/pass`
  — reports zero pairs.
- `lizard --CCN 15 --length 30 --arguments 5 --warnings_only ...` on changed
  files still reports pre-existing long functions/tests (`phase13FileLocalIdfDownweight`
  and long duplication tests); new/modified product logic no longer appears in
  the warning list.

## Closure (2026-07-21)

The acceptance witness that reopened this task — `Smatchet@f8368a23
JiraClient.cpp:587↔884` — **is now reported** (`587-605 ↔ 883-900`, STRUCTURAL
weighted=0.732), verified against the pinned OSS-git blob. It is caught by *this*
task's boundary prefix window; the fix that pushed its score over threshold came
with #191's index/scoring changes. So #195 + #191 together close the case.

**This layer is NOT subsumed by #191.** An A/B (disable `enableBoundaryRuns`,
statement-runs on) on duckdb `src/execution` dropped pairs **183 → 123** and lost
the witness — boundary-runs carry ~52 real pairs statement-runs do not. So
`enableBoundaryRuns` stays; the two layers are complementary, not overlapping.
(The "#191 supersedes #195" phrasing in commit `104cde9` and an earlier #191 draft
was wrong and is corrected in #191's write-up.) Full A/B numbers: #191 task,
§"A/B — #195 boundary-runs are load-bearing".
