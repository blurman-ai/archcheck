# [SCAN] Window-free clone detection: remove dependence on fragment/token-window size

**Created:** 2026-07-24
**Start date:** 2026-07-24
**Status:** wip
**Module:** SCAN
**Priority:** major
**Difficulty:** large (design spike first, then core rewrite)
**Blocks:** #200 (min-size param becomes honest/monotonic only on a window-free core)
**Blocked by:** —
**Related:** #052/#056/#059/#083/#158/#190/#191/#195/#196 (all built on the fragment model), #198 (Duplo comparison = the motivating evidence), duplication_architecture.md (SSOT — must be updated)

## Problem

The token-pass clone detector fragments each function body into fixed token windows
(`minTokens`/`2×`/`4×` = 30/60/120, [fragmenter.h:38](../../include/archcheck/scan/duplication/fragmenter.h#L38))
and scores fragment-vs-fragment with weighted Jaccard. **The window size decides what
aligns**, so results depend on an internal implementation detail:

- Changing `minTokens` 30→50 is **non-monotonic**: some real clones vanish (a ~40-token
  clone falls below the floor), while *new* pairs appear (a ~55-token clone that straddled
  30-windows now fits one 50-window and aligns). Verified on our own `src/` (35→18 pairs,
  #35 `diff_query` 19-line clone lost, 2 new surfaced).
- The team already documented this boundary effect at [fragmenter.h `maxTokens` (#091)](../../include/archcheck/scan/duplication/fragmenter.h#L39):
  *"twin functions that straddle the cap fragment differently — one stays whole, the other
  subdivides — so the clone never aligns."*
- Consequence: clones look "tiny" and unstable; a user-facing "minimum clone size" knob
  can't be honest while the core re-fragments under it. This is a **root design flaw**, not
  a tuning problem — no threshold value fixes it.

The #198 comparison against Duplo (stable, line-based, monotonic in `-ml`) made this
concrete: low set-overlap and threshold-fragile output are symptoms of the window model.

## Goal

A clone-detection core whose results **do not depend on any internal window size**, so that
"minimum clone size" (#200) is a true, monotonic, understandable parameter.

## Direction (to be pinned in the spike)

Well-established window-free approaches (not our invention):

- **Maximal matching token runs** — suffix array / suffix automaton over the *normalized*
  token stream. Find the longest common token substrings directly, no windows. Identifier
  normalization → Type-2 (renamed) for free; Type-3 (gapped) by stitching adjacent exact
  runs. This is essentially how CCFinder / PMD-CPD / Duplo work; it makes min-clone-size a
  genuine post-filter with monotonic behavior.
- **AST subtree fingerprinting** (Deckard-style) — naturally window-free, but needs the
  parser → aligns with the planned v0.2 libclang backend. Consider as the semantic tier.

## The hard trade-off (the crux of the spike)

Our current differentiator — **typed STRUCTURAL / weighted-Jaccard "fuzzy similarity"** — is
exactly the part that lives on windows. A pure maximal-match core gives strong Type-1/2 but
not the fuzzy structural score in its current form. The spike MUST decide: drop STRUCTURAL,
keep a hybrid (maximal-match core + optional structural tier), or re-derive structural
similarity on a window-free basis. This changes the nature of the output and the marketing
framing — decide deliberately.

## Execution plan

- [x] **Design spike / ADR** — [ADR-004](../../docs/decisions/004-window-free-clone-core.md)
      (2026-07-24; author waived formal acceptance, directed straight to prototype).
- [x] Prototype the maximal-normalized-token-run detector; confirm monotonicity and witness
      behavior — `experiments/window_free_spike/` (gitignored), links the PRODUCT lexer.
      Results in ADR-004 §Spike evidence; summary below.
- [x] Migration map — ADR-004 §Migration map (per-layer fate table).
- [x] Product implementation: run-matching core landed behind `--window-free` next to the
      window core (seeds+extension). New module `src/scan/duplication/token_run_matcher.{h,cpp}`
      (reuses `lex()` unchanged), wired via `ScannerOptions::windowFree` → early branch in
      `scanForDuplication`; CLI `archcheck --duplication <path> --window-free`. Unit test
      `tests/duplication_token_run_matcher_test.cpp` (cross-file Type-2 witness + monotonicity
      subset property). 664 tests green, lizard clean, dogfood 0, deterministic, default path
      untouched (see note below).
- [x] `--min-lines N` output filter (mode-agnostic, monotonic; `filterByMinLines` in
      `cli/preview_commands.cpp`) — delivers #200's interim surface. P0.9/vendored need NO
      port: `collectNonVendoredSources` (#129 gate) already drops generated/vendored before
      scanning, so window-free inherits it.
- [x] Stage-3 A/B (window vs `--window-free`, both via the reporter), hand-verified per
      bucket — see "A/B evidence" below and ADR-004 §Implementation status.
- [x] Idiom-FP filter — built + witness-verified TWO candidates, chose the clean one:
      **control-flow guard** (span must branch/return) is default `--window-free`; **rarity**
      (`--rare-anchors N`) kept as an aggressive knob. Measured on src/ (ml6): rarity 125→68
      but kills the writer-TP class (`write_nodes/edges`, `writeAdded/Grown` — no rare anchor);
      control-flow 125→96, kills every idiom-FP (namespace-preamble, `check()` sig, keyword
      table) and keeps all writer/catch/dist TP. Cost of cf: drops control-flow-free clones
      (straight-line void fns, #197 member-decl runs) — recoverable via `--no-control-flow`.
- [ ] P0.2 whole-file suppress (coverage logic; 0 groups on src/, matters on corpus moves).
- [ ] **Author decision: the STRUCTURAL trade-off** (drop fuzzy band → changes product
      framing) + full corpus A/B vs #198 → then flip default + delete window layers + rewrite
      SSOT. The one genuine product call left; everything mechanical is done behind the flag.
- [ ] Corpus A/B vs the #198 baseline (same inputs, per-bucket hand-verified); quantify the
      STRUCTURAL loss bucket before flipping the default.
- [ ] Flip default, delete the window layers (#190/#191/#195 machinery), re-baseline the
      duplication fixtures consciously; rewrite `duplication_architecture.md` (SSOT) in the
      same PR. Add the monotonicity fixture.

## Spike results (2026-07-24)

- **Monotonicity:** min-tokens 30→50 on `src/` = strict subset, 0 violations (window model:
  35→18 non-nested). Holds with stitching (stitch-then-filter, threshold-independent).
- **Witnesses:** JiraClient 587↔884 cross-fn (native 115-tok run, no #195 layer needed);
  `ParamsToString() const` #196 (native 133-tok run — open window-model bug fixed for free);
  xml_utils Type-2 flagship (full pair via stitching, matched=99%); SmatchetUI adjacent twin;
  diff_query 19-line clone as one stitched span at BOTH thresholds.
- **A/B on `src/`:** 22/24 current pairs covered; both losses STRUCTURAL (hand-read: one real
  scattered-micro-edit Type-3 (`++x`/`x++` every 2-3 lines), one idiom-tail borderline).
  New true finds: identical `catch(ConfigError)` blocks `check_command↔main` (EXACT).
- **Perf:** duckdb `src/execution` 2.9 s (spike, incl. stitching) vs 33 s current.
## Product core landed (2026-07-24, uncommitted — awaiting /commit)

New files: `include/archcheck/scan/duplication/token_run_matcher.h`,
`src/scan/duplication/token_run_matcher.cpp`, `tests/duplication_token_run_matcher_test.cpp`.
Edits: `duplication_scanner.{h,cpp}` (windowFree branch), `cli/preview_commands.{h,cpp}`,
`main.cpp` (`--window-free` + help), both CMakeLists. All the spike's guards ported
(pp-mask, tandem guard, negative-gap stitch, containment dedup, EXACT/RENAMED/LITERAL/MIXED
classify). Emits `Fragment`+`Pair` so the reporter/`--diff` path consume it unchanged.

Note (self-check): default `--duplication` on `src/` went 24→25 pairs — fully explained,
NOT a default-path regression: the new module file `token_run_matcher.cpp` is now in `src/`
and (a) carries one internal STRUCTURAL self-clone (the `dedupRuns`/`dedupSpans` containment
skeletons — different predicates/types, advisory 0.66, not worth a template), (b) shifts
corpus IDF a hair; plus line-number shifts from the help/branch edits. Default path
deterministic (25/25/25), logic untouched.

## Duplo validation — 9 repos, and a REAL recall gap the aggregate hid (2026-07-25 → #201)

Against Duplo v2.3.10 (`-ml 8 -ip -json`), fair (Duplo fed EXACTLY archcheck's non-vendored
file set; phantom-guarded), **9 repos**: aggregate raw recall **1768/1848 = 96%** (`--no-cf`),
1449/1848 = 78% default (cf). Per repo: src 5/5, smatchet 2/2, duckdb-exec 23/23, 3drepo 50/51,
gladius 312/320, matrixos 197/199, falco 10/10, irreden 272/295, openvdb 897/943.

**The 96% aggregate is misleading — hand-verifying the 80 `--no-cf` misses split them:**
- *Duplo noise, correctly ignored*: `#include` blocks (preprocessor mask), license headers
  (comments), data tables (font glyphs, byte arrays, enum arrays), method signatures
  (Metal↔OpenGL `uploadSubImage2D`, decl/def), self-overlaps, generated stubs.
- *GENUINE misses — worse than Duplo*: real verbatim/near-verbatim clones in **repeated /
  self-similar regions** — `nanovdb/math/Ray.h:403-423↔435-455` (21-line ray-box block,
  region covered by ZERO window-free pairs — lost entirely), `ParmFactory.cc:1163↔1218` (real
  logic copy), `QuantizedUnitVec.cc` (5 at ~127-line stride), gladius codegen (region flagged
  but pairs dropped).

**Root cause = the same one that blows up perf** (openvdb: 117,237 spans; batch 257 min):
dedup + tandem guard + extension cap over-collapse self-similar cliques, dropping real distinct
pairs. Recall gap + O(n²) cost are one problem → **filed #201**. Corrects the earlier "82/83
not worse" note (that was 4 repos and read the aggregate; the 9-repo hand-verify found the gap).
Harness: `experiments/window_free_spike/{vs_duplo.py,batch_duplo.sh}`. Two harness self-catches:
phantom-guard window-sizing bug (fixed); fairness fileset needed (vendored bvh/ → 8% artifact).

## A/B evidence (2026-07-25, stage 3 — window core vs `--window-free`, both via reporter)

| Input (min-lines) | window | window-free | window covered | window-only dropped |
|---|--:|--:|--:|--:|
| `src/` (6) | 17 | 124 | **15/17** | 2 (both STRUCTURAL) |
| Smatchet (8) | 8 | 45 | **6/8** | 2 (both STRUCTURAL) |

- **Window-only drops are always STRUCTURAL** (scattered-micro-edit / low-overlap band),
  hand-verified: `diff_query:129↔165` (`x++`→`++x`), `include_scanner:73↔105`, Smatchet
  `JiraClient:684↔1237`, `SpreadsheetState.h` member block. Deliberate trade-off, not a bug.
- **Adds are a MIX (hand-read):** real TP the window core can't see (cross-file
  `catch(ConfigError)` EXACT `check_command`↔`main`; `write_nodes_block`↔`write_edges_block`
  parameterizable twins; copy-pasted line-distance search in `local_complexity_drift`;
  JiraClient request repeats) + newly-surfaced **idiom FP** (namespace-preamble `constexpr`
  runs; IRule `check()` interface signatures = #159 accidental class; some member-decl runs).
- **Root cause of the idiom FP:** the fragmenter was ALSO an implicit function-body-scope +
  rarity filter. **Required next filter = rarity/IDF anchor** (author-calibrated).

Next-stage staging: port the rarity/IDF filter (author calibration) → corpus A/B vs #198
baseline (quantify STRUCTURAL loss) → **author decides the STRUCTURAL trade-off** → flip
default + delete window layers + rewrite SSOT. No step flips by "the run finished".

- **Design lessons pinned in ADR-004:** preprocessor mask required (include-preamble floods);
  tandem/adjacent-twin handling must be token-based (line-based guard killed the
  copy-below-original pattern — P0.4 lesson repeated); stitch gap must admit small negative
  overlaps; same-file span must not cross its own partner (tandem chain fusion);
  volume at small min-size is filter-territory (min-lines 6: 425→122 on `src/`).

## Fixtures (if a rule)

- All existing `fixtures/dup*` / duplication fixtures must pass or be consciously re-baselined
  with the reason recorded. Add a monotonicity fixture: raising min-size yields a strict
  subset (the property the window model violates).

## Notes

- Keep the two-backend split (fast token pass vs libclang) — this rewrites the token pass core,
  not the backend architecture.
- #198's comparison numbers are a snapshot of the *current* (window) algorithm; they are the
  baseline the rewrite is measured against, not a claim about the future tool.
