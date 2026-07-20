# [SCAN] Compare archcheck duplication against free C++ clone detectors

**Created:** 2026-07-20
**Start date:** 2026-07-20
**Status:** wip
**Module:** SCAN
**Priority:** major
**Difficulty:** unknown
**Blocks:** —
**Blocked by:** —
**Related:** #052 (cross_tu_duplication_detector), #056 (fast_backend_token_pass), #059 (coincidental_clone_filtering), #083 (duplication_precision_gate), #158 (clone_detector_fp_reduction), #190 (subfunction_clone_pass)

## Goal

Run a set of free, C++-capable copy-paste / clone detectors on the same inputs as archcheck's duplication pass and produce a side-by-side comparison of what each finds (recall, precision, granularity, FP classes).

## Context

archcheck ships a token-based clone detector (advisory) built up across #052/#056/#059/#083/#158/#190. We have never benchmarked it against the established free tools. We need to know, concretely: what do the standard tools catch that we miss, what do we catch that they miss, and where our FP/FN classes differ. This grounds the duplication subsystem's positioning and feeds the precision work.

Candidate free detectors that handle C++ (verify licenses / build before committing to each):

- **Duplo** — open-source C/C++/Java line-based clone detector ("дупло").
- **PMD CPD** — Copy/Paste Detector, has a C++ (CPPTokenizer) frontend; token-based; free/BSD.
- **Simian** — free for open-source use (license caveat — check before including).
- **CCFinderX / NiCad / Deckard** — research clone detectors (Type-1/2/3); heavier to build, include only if feasible.
- **clang-based / cpp-clone-detector style tools** — scan for any maintained OSS option.

The comparison must run on a shared, reproducible input set — reuse a few showcase repos and/or corpus repos so the same files go through every tool. Normalize each tool's output to a common shape (file:line span pairs + size) so overlap can be computed rather than eyeballed.

See [docs/duplication_architecture.md](../../docs/duplication_architecture.md) — the SSOT for our layers and FP classes; frame the comparison in its terms.

## Execution plan

- [ ] Enumerate free C++ clone detectors, verify each is (a) buildable/installable here, (b) license-clear to run for a comparison. Drop the ones that fail either gate — log why.
- [ ] Pick a shared, reproducible input set (a handful of showcase/corpus repos; pin commits).
- [ ] Run each external tool + archcheck's duplication pass on the same input; capture raw output.
- [ ] Normalize outputs to a common span-pair schema so overlap is computable.
- [ ] Compute overlap: what all agree on, what only archcheck finds, what only tool X finds; classify the deltas (granularity, Type-1/2/3, FP class).
- [ ] Hand-verify a sample of each delta bucket (witness-per-case, per the self-check rule) before any aggregate claim.
- [ ] Write up the comparison (where we win/lose, and any concrete precision/recall follow-ups for our detector).

## Done

- **Detector gate (step 1) — environment probe.** No external detector and no JVM installed on this host; `g++ 8.3` (Astra, GCC8), `clang++-11`, `cmake 3.18`, network up.
  - **Duplo** ("дупло", the named tool): the current `dlidstrom/Duplo` HEAD needs `<format>`/`<ranges>` + `nlohmann_json` (won't build on GCC8/libstdc++8). Tag **v0.4.0** is plain C++ with no external deps → **builds and runs** here after a 2-line portability patch (documented below). GPL-2.0. Line-based Type-1/2 detector. Binary at `scratchpad/duplo`.
    - Patch (so results are reproducible): `src/FileTypeBase.cpp` — wrap `std::isalpha` in a `[](unsigned char)` lambda for `find_if`; add `#include <cstring>` to `FileType_{CS,VB,S}.cpp` for `std::strlen`. No behavioral change.
  - Smoke run on archcheck's own `src/` (58 files, `-ml 4 -ip`) produced block-pair output; **early overlap with archcheck confirmed** — both independently flag `git/diff_query.cpp:24↔44` and the `new_clone_drift.cpp` self-clones.
- **archcheck output shape captured:** span-pair lines `fileA:a-b <-> fileB:c-d (TYPE, weighted=, line=)`; `--format json` is NOT wired for `--duplication` (still text). Normalization must parse text.
- **Duplo output shape:** `file(line)` / `file(line)` header pair + the duplicated block text; block length in lines. No similarity score (Type-1/2 exact-ish).

## Results — v1 comparison (3 inputs, 2026-07-20)

Tools: **archcheck** `--duplication` (current, incl. #190) · **Duplo v0.4.0** `-ml 4 -ip` (line-based) · **PMD CPD 7.7.0** `-l cpp --minimum-tokens 50` (token-based, default id-sensitive). Portable Temurin JRE 17 in scratchpad (no system install). Inputs: archcheck `src/`, Smatchet@f8368a23, duckdb_webbed@10bf9649.

Metric = line-coverage (set of (file,line) flagged) + pair corroboration (any-overlap, either orientation). Raw counts are NOT comparable (different thresholds/granularity) — coverage + hand-verified witnesses are the real signal.

| Input | archcheck dup-lines | Duplo | PMD | archcheck pairs corrob. by Duplo / PMD |
|-------|--------------------:|------:|----:|:--------------------------------------|
| archcheck_src | 329 | 276 | 285 | 11/13 · 6/13 |
| smatchet | 122 | 235 | 155 | 6/11 · 5/11 |
| duckdb_webbed | 2241 | 1059 | 1784 | 42/66 · 54/66 |

### Where archcheck is BETTER (verified by reading code)
- **Type-2 / renamed clones** — its core edge. CASE 1 `SmatchetUI.cpp:532↔541` (ResolveOptionId/Label, one token differs) and CASE 4 `xml_utils.cpp:348↔383` (32-line function copy-renamed comments→cdata) are genuine copy-rename clones **both Duplo and PMD miss** (Duplo=line-based; PMD id-sensitive by default). This is why archcheck's line-coverage on duckdb is highest — real signal, not FP.
- **Vendored/third-party exclusion by design** — `isVendoredFile`/`pathHasVendoredDir`; `--duplication` on `ThirdParty/imgui` = "scanned 0 files". 11 of Duplo's 24 extra smatchet pairs were imgui noise. Duplo/PMD have no such filter → they report vendored-dep clones as if they were your code.
- **Typed, scored output** (EXACT/RENAMED/STRUCTURAL + weighted/line score). Duplo & PMD give neither.
- **Short exact clones** below PMD's token floor (CASE 2, 4-line exact: archcheck catches, PMD@50 filters).

### Where archcheck is WORSE (verified)
- **Cross-function partial clones / shared prologues** — CASE 3 `JiraClient.cpp:587↔884`, a real 17-line block that is the shared prologue of two otherwise-different functions (FetchUsers/FetchFieldCatalog). **PMD caught it; archcheck missed it** — it compares whole function-fragments, so two mostly-different functions score below threshold. #190 handles copy-paste nested **within one** function body, NOT a fragment shared **across two** functions → genuine remaining gap (kin of #191).
- **Raw recall on short first-party blocks** — Duplo@-ml4 surfaces more 4–6 line first-party matches; some real, mostly low-value. Threshold/granularity tradeoff, not a bug.
- **No structured output for `--duplication`** — `--format json` is ignored (text only). Duplo emits XML/CSV, PMD CSV/XML/JSON → easier to pipe. Operational gap.

### Method caveats
- Raw pair/line counts diverge mostly from threshold choice; don't read them as recall. Duplo@-ml4 is noisy (many tiny blocks), PMD@50 mid, archcheck fewer+larger+typed.
- Duplo build required a 2-line GCC8 portability patch (documented above). PMD run at default (id-sensitive); a `--ignore-identifiers` (Type-2) run is a natural follow-up to test whether PMD-with-renaming closes archcheck's Type-2 edge.
- Only 3 repos — proof-of-method, not a corpus claim.

## In progress

- Reporting to author + open questions (write-up scope, corpus expansion, PMD Type-2 mode, filing the CASE-3 gap).

## Open questions (for the author)

1. **PMD CPD** (token-based, C++ frontend, BSD): needs Java, not installed. Plan is a **portable OpenJDK tarball** extracted into scratchpad (no system install, no sudo). OK to proceed, or keep the comparison to Duplo + archcheck for v1?
2. **Input set:** `experiments/showcase/` repos, a few corpus repos, or archcheck's own `src/` as a start? Need pinned commits for reproducibility.
3. **Simian** (Java, license-restricted for non-OSS) and heavier research tools (NiCad/Deckard/CCFinderX) — include or drop? Leaning drop for v1 (license / build cost).

## Detector shortlist (gate result)

| Tool | C++? | License | Buildable here | Verdict |
|------|------|---------|----------------|---------|
| Duplo v0.4.0 | yes (line-based) | GPL-2.0 | yes (2-line patch) | **IN** |
| PMD CPD | yes (token-based) | BSD | needs portable JRE | pending author OK |
| Simian | yes | free-for-OSS only | needs JRE | lean DROP (license) |
| NiCad / Deckard / CCFinderX | yes (Type-3/AST) | mixed | heavy build | DEFER |

## Next steps

1. Get author's call on PMD (portable JRE) + input set.
2. Freeze the shared input set with pinned commits.
3. Run Duplo (+ PMD if in) + archcheck on it; capture raw output.
4. Normalize all outputs to a common span-pair schema; compute overlap + classify deltas; hand-verify a sample per bucket; write up.

## Key decisions

| Decision | Reason |
|---------|---------|
| Only free + C++-capable tools | Matches the request; keeps the comparison reproducible for any user |
| Shared pinned input set | Same files through every tool → overlap is computable, not eyeballed |
| Normalize to common span schema | Overlap must be measured, not visually compared |

## Changed files

| File | Change |
|------|-----------|
| ... | ... |

## Fixtures (if a rule)

- n/a (comparison / benchmark task, not a new rule)
