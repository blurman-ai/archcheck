# [DUP] 191: statement-run clones — a copied span that is not a braced block

**Created:** 2026-07-18
**Start date:** —
**Status:** new
**Module:** SCAN/DUPLICATION
**Priority:** major
**Difficulty:** large
**Blocks:** closing the sub-function recall gap beyond brace-delimited blocks
**Blocked by:** —
**Related:** #190 (brace-descent, done), #056 (token pass), #092 (winnowing fingerprints), #091

## Goal

Detect copy-paste whose copied region is **not a single braced block**: a bare run of
consecutive statements, a span covering several sibling blocks and the code between
them, or a chunk of one multi-line call's argument list.

## Why this exists (evidence, not speculation)

#190 made the fragmenter descend into function bodies, so a copied `if`/`for`/`while`/
`switch` body is now a fragment and gets compared. That closed the *block* half of
sub-function copy-paste — on duckdb `src/execution` measured against Duplo, coverage
went 13/24 → 17/24.

The 7 that remain were read by hand and all have the same shape: the fragmenter is
brace-based, so a copied span that is not exactly one `{…}` body can never become a
fragment, at any nesting depth. Concrete witnesses:

| Witness | Shape |
|---|---|
| duckdb `operator/helper/physical_limit.cpp:246` ↔ `physical_streaming_limit.cpp:72` | a whole `ParamsToString` body — three sibling `if` blocks with statements interleaved |
| duckdb `operator/join/physical_iejoin.cpp:2073` ↔ `2098` | a bare run of statements starting mid-function |
| monit `http/cervlet.c:1357` ↔ `1484` | 19 verbatim lines of consecutive `out_print(...)` calls |
| monit `http/cervlet.c:832` ↔ `944` | a chunk of one call's multi-line argument list |

The `physical_limit` one is the most damning: it is an entire duplicated method body,
missed only because the body is a sequence of siblings rather than one block.

## Design — this is where the #190 sliding-window sketch belongs

#190's task described a window/suffix layer. It turned out to be unnecessary for the
block class (removing one early exit in the fragmenter sufficed), but it is exactly
right for this one:

- Sliding window over the normalized token stream, stepped, hashed. The winnowing
  fingerprint machinery already exists (#092, `addFingerprintCandidates`) and does not
  need rebuilding.
- Candidate spans from equal fingerprints, extended maximally left/right, then scored
  through the existing similarity + guard pipeline.
- Emitted as `Fragment`s so scoring, classification, reporting and `DRIFT.NEW_CLONE`
  are reused unchanged.

### Open questions — carried over from #190, still open

1. **Containment.** `phaseNestedContainment` (added in #190) already drops a pair
   contained in another reported pair, so the reporting half may be free. Verify rather
   than assume — window spans are not brace-aligned, so containment will be partial
   overlap, not clean nesting.
2. **`Fragment::nested` semantics.** #190 established the rule that a fragment whose
   text is already counted elsewhere must not count as a *document* (df/IDF, the
   fingerprint frequency cap, the whole-file ratio — see #190's write-up). Window spans
   overlap heavily by construction, so this rule needs extending, not just reusing.
   **This is the main technical risk**: in #190 each of these three was found as a
   concrete regression, and one of them (the fingerprint cap) silenced a whole family of
   real clones and was invisible on a small subdirectory.
3. **Idiom flood + cost.** Unchanged from #190's sketch: windows hit far more
   boilerplate than bodies do, and all-pairs over windows is much larger. #190 already
   cost +75% wall / +47% RSS on duckdb; budget before building.

## Acceptance criteria

- [ ] The 4 witnesses above are reported.
- [ ] Duplo coverage on duckdb `src/execution` improves from the current 17/24, with the
      remainder read by hand and classified (miss vs deliberate suppression).
- [ ] duckdb full `src/`: no baseline pair lost (`lost=0`), as enforced in #190.
- [ ] Hand-triage a sample of newly-introduced pairs; report TP/FP honestly with the
      sample size, since there is still no automated `fp_corpus_r2` harness.
- [ ] Fixtures `fixtures/duplication/statement_run_clone/{pass,fail_*}/`.
- [ ] Wall-clock and RSS on duckdb stated, not absorbed silently.
- [ ] `docs/duplication_architecture.md` §3.2 and §9 updated.

## Method note

The oracle harness is built and reusable: Duplo `v1.0.1` compiled with
`g++ -std=c++2a -O2 -Isrc/include src/*.cpp`, plus the join/filter script that dedupes
blocks and drops preprocessor-dominated ones. Do not try NiCad for C++ — it has no C++
grammar and fails to parse 38–94% of files (measured, #190).
