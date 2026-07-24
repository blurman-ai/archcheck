# [SCAN] Multi-repo set-overlap: archcheck vs latest Duplo clone sets

**Created:** 2026-07-24
**Start date:** 2026-07-24
**Status:** wip
**Module:** SCAN
**Priority:** major
**Difficulty:** unknown
**Blocks:** —
**Blocked by:** —
**Related:** #194 (compare_clone_detectors, proof-of-method 3 repos), #195/#191/#196 (cross-function/statement-run/const-method gaps), #158 (fp_reduction), #083 (precision_gate)

## Goal

Recompute the archcheck-vs-Duplo comparison with the **latest Duplo (v2.3.10, JSON
output)** — and not on one repo but across a **sample of corpus C++ repos** — then
compare the **sets** each tool found: intersection (both), archcheck-only,
Duplo-only, and Jaccard. #194 stopped at pair-corroboration counts on 3 repos; this
task broadens to full set-overlap over N repos and locks the deltas with
hand-verified witnesses per bucket.

## Why now

Ubuntu migration unblocked building Duplo HEAD (needs `<format>`/`<ranges>` +
nlohmann_json — impossible on Astra GCC8). Latest Duplo builds clean on GCC 13 and
emits JSON with real block text (`Lines[]`), so normalization no longer parses text.
The v1.0.1 structural phantom (`EndLineNumber=1`) is gone; residual content-phantom
rate measured at 1/2036 on duckdb core (block #102, hash-collision). See JOURNEY
2026-07-24 and #194.

## Method

- **Inputs:** pin a reproducible sample of C++ corpus repos (start: archcheck `src/`,
  Smatchet@`f8368a23`, duckdb-core@`f707ac5f`, plus 4-6 more corpus repos at pinned
  HEAD). Record commit per repo.
- **Tools:** archcheck `--duplication <path>` (current) · Duplo v2.3.10
  `-ml <N> -ip -json`. Same input tree through both.
- **Normalize** each tool's output to a common span schema:
  `(fileA, a1,a2, fileB, b1,b2)`. Duplo = JSON; archcheck = parse text
  (`--format json` NOT wired for `--duplication` — text only).
- **Fairness:** apply archcheck's vendored/third-party exclusion to Duplo's output
  too (Duplo has no such filter → reports ImGui/dep clones). Report both raw and
  vendored-filtered.
- **Set metrics, two granularities:**
  1. **Line-coverage set** = set of `(file,line)` flagged. Jaccard + |A∩B|, |A\B|,
     |B\A| per repo and aggregated.
  2. **Pair-corroboration** (v1 continuity): a pair from A corroborates a pair from B
     if both spans overlap (either orientation).
- **Phantom guard:** before counting a Duplo block as "found", verify its reported
  `Lines[]` text is present at BOTH endpoints (drop content-phantoms like #102).
- **Hand-verify** a sample of each bucket (archcheck-only, Duplo-only, both) per the
  self-check rule — witness-per-case before any aggregate claim.

## Execution plan

- [ ] Build the normalization + set-overlap harness (`compare_sets.py`): archcheck-text
      + duplo-json → span sets → line/pair overlap, per-repo + aggregate.
- [ ] Select + pin the repo sample; record commits.
- [ ] Run both tools on every repo; capture raw output (archcheck text, duplo json).
- [ ] Compute set overlap (line Jaccard + pair corroboration), raw and vendored-filtered.
- [ ] Phantom-filter Duplo blocks (text-at-both-endpoints); log dropped.
- [ ] Hand-verify N witnesses per bucket per repo.
- [ ] Write up: update `docs/research/clone_detector_comparison.md` (v2, multi-repo),
      append JOURNEY, close/advance #194.

## Notes / findings

- Duplo v2.3.10 binary prints `VERSION v1.0.1` (stale hardcode) — it IS the new code
  (has `-json`/JsonExporter, `-j`, `-d`). Don't identify by the version string.
- Build: `cmake -B build -S . -DCMAKE_BUILD_TYPE=Release` in the Duplo checkout, GCC 13,
  no patches. Binary in scratchpad.
- duckdb "input" = the DuckDB **core** submodule tree (`f707ac5f`), scanned directly —
  archcheck's vendored exclusion means v1 pointed at the core, not the webbed wrapper.

## Fixtures (if a rule)

- n/a (comparison / benchmark task).
