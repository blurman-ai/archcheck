# archcheck vs free C++ clone detectors (Duplo, PMD CPD)

**Task:** #194 · **Date:** 2026-07-20 · **Status:** v1 (proof-of-method, 3 inputs)

How archcheck's `--duplication` pass compares against the two viable free,
C++-capable copy-paste detectors that build/run in our environment. Not a corpus
claim — a small, hand-verified comparison to locate where we win and where we lose.

## Setup

| Tool | Kind | Invocation | License |
|------|------|-----------|---------|
| archcheck | fragment (function/sub-function), typed + scored | `archcheck --duplication <path>` (current, incl. #190) | — |
| Duplo v0.4.0 | line-based (Type-1/2, near-identical) | `duplo -ml 4 -ip <filelist>` | GPL-2.0 |
| PMD CPD 7.7.0 | token-based (sliding window) | `pmd cpd -l cpp --minimum-tokens 50` | BSD |

Notes on reproducibility:
- The current `dlidstrom/Duplo` HEAD needs `<format>`/`<ranges>` + `nlohmann_json`
  and will not build on Astra's GCC 8.3. Tag **v0.4.0** is plain C++; it builds
  after a 2-line portability patch (wrap `std::isalpha` in a `[](unsigned char)`
  lambda for `find_if`; add `#include <cstring>` for `std::strlen`). No behavior change.
- PMD needs a JVM; we ran it under a portable Temurin JRE 17 (extracted, no system install).
- Inputs: archcheck's own `src/`; **Smatchet**@`f8368a23`; **duckdb_webbed**@`10bf9649`.

Raw pair/line counts are **not** comparable across tools — each has a different
threshold and granularity. The real signals are (a) line-coverage overlap and
(b) hand-verified witnesses. Metric: two clone instances match if both code
regions overlap (any overlap, either orientation).

## Numbers

| Input | archcheck dup-lines | Duplo | PMD | archcheck pairs corroborated by Duplo / PMD |
|-------|--------------------:|------:|----:|:--------------------------------------------|
| archcheck `src/` | 329 | 276 | 285 | 11/13 · 6/13 |
| Smatchet | 122 | 235 | 155 | 6/11 · 5/11 |
| duckdb_webbed | 2241 | 1059 | 1784 | 42/66 · 54/66 |

Counts diverge mostly from threshold: Duplo@`-ml 4` is noisy (many tiny blocks),
PMD@50 is mid, archcheck reports fewer, larger, typed clones.

## Where archcheck is better (verified by reading the code)

**1. Type-2 / renamed clones — the core edge.**
Two whole-function copy-renames that **both** Duplo and PMD (at default settings) miss:
- `Smatchet/Source_Core/src/SmatchetUI.cpp:532↔541` — `ResolveOptionId` vs
  `ResolveOptionLabel`: identical loop, one token differs (`option.Id`→`option.Value`).
- `duckdb_webbed/src/xml_utils.cpp:348↔383` — a 32-line function copied and
  renamed (`comments`→`cdata_sections`, `XML_COMMENT_NODE`→`XML_CDATA_SECTION_NODE`,
  `traverse_comments`→`traverse_cdata`).

Duplo is line-based (every renamed line reads as changed); PMD CPD is
identifier-sensitive by default (renamed tokens break the match). archcheck
normalizes and labels these RENAMED/STRUCTURAL. This is why archcheck's
line-coverage on duckdb is the highest of the three — real Type-2 signal, not
false positives.

**Independent validation.** Re-run PMD in Type-2 mode (`--ignore-identifiers
--ignore-literals`) and its agreement with archcheck jumps — corroboration of
archcheck's pairs rises 6→11/13 (`src`), 5→7/11 (Smatchet), 54→58/66 (duckdb).
So archcheck's extra finds are genuine clones a Type-2-configured tool confirms.
But PMD pays for it: pair count explodes ~4× (113→484 on duckdb), i.e. archcheck
reaches comparable Type-2 recall **at usable precision, zero-config**, while
identifier-blind PMD floods output.

**2. Vendored / third-party exclusion by design.**
`archcheck --duplication ThirdParty/imgui` reports "scanned 0 files" —
`isVendoredFile`/`pathHasVendoredDir` skip vendored trees. 11 of Duplo's 24 extra
Smatchet pairs were clones inside Dear ImGui. Neither Duplo nor PMD filters
vendored code, so they report dependency-internal clones as if they were yours —
noise for an architecture tool.

**3. Typed, scored output.** EXACT / RENAMED / STRUCTURAL + weighted & line
scores. Duplo and PMD emit neither — just locations.

**4. Short exact clones below PMD's token floor.**
`JiraClient.cpp:112-115↔181-184` — a byte-identical 4-line block. archcheck
catches it; PMD@50 tokens filters it (~20 tokens).

## Where archcheck is worse (verified)

**1. Cross-function partial clones / shared prologues.**
`JiraClient.cpp:587↔884` — a real 17-line block that is the shared prologue of
two otherwise-different functions (`FetchUsers` / `FetchFieldCatalog`, differing
only in `outUsers`→`outComponents`). **PMD caught it; archcheck missed it.**
archcheck compares whole function-fragments, so two mostly-different functions
score below its similarity threshold. #190 detects copy-paste nested **within
one** function body, not a fragment shared **across two** functions — a genuine
remaining gap (kin of #191).

**2. Raw recall on short first-party blocks.** Duplo@`-ml 4` surfaces more 4–6
line first-party matches; some real, most low-value. A threshold/granularity
tradeoff, not a defect — but worth stating plainly.

**3. No structured output for `--duplication`.** `--format json` is ignored (text
only). Duplo emits XML/CSV; PMD emits CSV/XML/JSON. An operational gap for anyone
piping results into other tooling.

## Takeaways

- archcheck's differentiator is **typed Type-2 detection at usable precision,
  zero-config**, plus vendored-code exclusion — the things that make clone output
  actionable for architecture review rather than a flood.
- The clear loss is **cross-function partial clones** (shared prologue/suffix
  across two functions), which token-window tools catch and our fragment model
  does not. Tracked as a follow-up near #191.
- `--duplication` needs a machine-readable format to be integrable.

## Repro

Scripts and raw output live in the task's scratchpad (`compare.py`,
`witness.py`, `raw/*.{archcheck.txt,duplo.txt,pmd.csv}`). See #194 for the full
run log and the detector-gate table.
