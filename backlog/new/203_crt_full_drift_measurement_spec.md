# 203 — Full-drift measurement spec: what the corpus re-run must capture

**Type:** research / infrastructure
**Status:** new
**Depends on:** #180, #182, #184, #185; recovered harness in `experiments/recovered_2026-08-22/`
**Blocks:** any corpus re-run

## Why this task exists

The corpus has been re-run roughly ten times, and each pass discovered something that had not
been measured the previous time. The cause is not carelessness about individual signals — it is
structural: **the whole per-commit corpus (484,500 commits) was scanned with one mode out of six.**

Counted from the recovered harness (`grep` over `scripts_recovered_all/`):

| invocation | occurrences |
|---|---|
| `--diff` | 40 |
| `--duplication` | 6 |
| `--history` | 3 |
| `--drift-baseline` | 2 |
| `--save-graph-baseline` | 1 |

`--diff` carries the per-commit panel. Everything else was a side probe on a handful of repos.
Whole rule families therefore never entered any panel, and their absence is invisible in the
output — there is no empty column to notice, the signal simply does not appear.

The same failure has a second form inside `--diff`: `run_worklist.categorize()` routes any
unrecognised rule ID into `n_other`. A new rule is counted but not identified, and nothing warns.

This task fixes the *inventory*, not the pipeline. No run starts until the list below is complete
and the pre-flight gates pass.

## Ground truth: what the tool actually emits

Established from shipped behaviour (`--help` of the built binary plus an empirical probe), **not**
from design docs — per the project's own rule that capability claims are checked against the
binary. Binary used: `build/debug/src/archcheck`, version `0.1.8`.

### Probe

`--diff --diff-mode=memory --format=json` over 120 non-merge commits of this repo. Rule IDs that
actually fired:

| rule ID | fires |
|---|---|
| `DRIFT.NEW_CLONE` | 232 |
| `DRIFT.BOOL_FIELD_ACCRETION` | 8 |
| `DRIFT.LOCAL_COMPLEXITY` | 4 |
| `TEST.1.prod_changed_tests_silent` | 2 |
| `ARG.2.boolean_literal_call` | 2 |
| `SATD.1` | 2 |

Structural JSON fields, commits where non-empty: `added_edges` 8/120, `nccd_delta` 5/120,
`removed_edges` 1/120; `grown_cycles`, `new_god_headers`, `new_cross_area_dependencies`,
`chain_length_grown`, `complexity_skipped_added_lines`, `rename_suppressed_cycles` all 0/120.

**Scope caveat:** one repository. Other repos will emit `SATD.2`, `ARG.1`, and the `DRIFT.4.*`
family. The prefix matching (`SATD*`, `TEST*`, `ARG*`) covers those; the point of the probe was to
confirm the mapping has no hole on the signals that dominate, and it does not.

### The six measurement surfaces

| # | invocation | emits | in the old panel? |
|---|---|---|---|
| 1 | `archcheck [path]` | `SF.9`, `CASE_MISMATCH_INCLUDE` (gating); `SF.7`, `SF.8`, `Lakos.GodHeader`, `Lakos.ChainLength`, `UNRESOLVED_LOCAL_INCLUDE`, `FUNC.COGNITIVE_COMPLEXITY` (advisory) | **no** |
| 2 | `--baseline` / `--save-baseline` | new-violations-vs-baseline delta | no |
| 3 | `--drift-baseline` | `DRIFT.1`, `DRIFT.2`, `DRIFT.4.CYCLE` (gating); `DRIFT.3`, `DRIFT.4.NEW`, `DRIFT.4.SDP` (advisory) | **no** |
| 4 | `--duplication [--window-free] [--min-lines N]` | clone **stock** | edges of the Agarwal window only |
| 5 | `--history` | `SIZE.1.god_file_growth`, `HIST.1.defect_attractor` | **no** |
| 6 | `--diff [--diff-mode] [--format]` | the six rule IDs above + the structural fields | **yes — this was the whole panel** |

Surface 3 is the load-bearing omission. `DRIFT.1` (shortcut edge), `DRIFT.2` (bidirectional
coupling), `DRIFT.3` (lateral module dependency) and `DRIFT.4.SDP` are the Lakos / Martin
architectural-drift layer — the rules that most directly express the thesis the project set out to
test — and they appear in **no** published endpoint.

## What the re-run must capture

### A. Per-commit (`--diff`) — keep everything the old panel had

`gate`; `gating.grown_cycles`, `gating.new_god_headers`; `advisory.added_edges`,
`removed_edges`, `new_cross_area_dependencies`, `chain_length_grown`, `nccd_delta`; and from
`advisory.violations`: `n_newclone`, `n_complexity`, `n_satd`, `n_test`, `n_flag_arg`,
`n_bool_field`, `n_bool_struct`.

**Mandatory per the author (2026-08-22):** clones, boolean-state drift, function complexity. All
three are in this set and confirmed firing.

### B. Per-commit — two fields the old panel never took

- `advisory.complexity_skipped_added_lines`
- `advisory.rename_suppressed_cycles`

These are **censoring counters**: added lines the complexity check declined to score, and cycle
events suppressed as renames. Both were zero on this repo's history, but on the corpus — vendored
dumps, mass renames — they will not be. If either correlates with commit size or author class, it
silently biases exactly the two endpoints the paper leads with (complexity, cycles). They must be
recorded per commit and reported as a coverage table, adopters vs controls, agent vs human.

### C. Rule-ID completeness invariant

`n_other` must be **0** across the whole run. Any non-zero value means a rule ID the mapping does
not know about, and the run stops until the mapping is extended. Additionally: dump the observed
set of distinct rule IDs at the end of the run and diff it against `--help`'s advertised list.

### D. Surfaces 1, 3, 4, 5 — decide and record, per surface

For each: either measure it on the same cohorts, or write down why not. "Nobody thought of it" is
what this task exists to prevent; "measured and null" and "deliberately out of scope" are both
acceptable outcomes, silence is not.

Minimum proposal, to be confirmed before the run:

- **surface 3 (`--drift-baseline`)** — per-repo, baseline at the pre-window edge, evaluated at the
  post-window edge. This is the highest-value gap.
- **surface 4 (`--duplication`)** — clone **stock** at both window edges for *all* cohort repos,
  not just the Agarwal 21. `--diff` measures clone *introductions*; stock is a different quantity
  and the two disagreed in the published results.
- **surface 5 (`--history`)** — once per repo per window edge.
- **surface 1** — snapshot at both window edges; cheap, and gives `FUNC.COGNITIVE_COMPLEXITY` and
  the include-resolution counters a before/after reading.

## Cohorts: dates are re-derivable, and this is verified

`cohorts.tsv` is lost. It is reconstructible, and the reconstruction was validated:

- adoption dates are mined by `mine_markers.scan_repo()` from `git log --all` on **local clones —
  no network**; event = earliest of four markers (`CLAUDE.md`, bot author, codex merge subject,
  `Co-Authored-By` trailer);
- placebo pseudo-dates are **deterministic, not random**: `build_cohorts.py` assigns
  `cand[(i * 7919) % len(cand)]` over the sorted adopter dates. There is no RNG in the file;
- the corpus clones survive (`~/oss_corpus/`, and `backup_reinstall_2026/oss*.tar.zst`), frozen at
  the 2026-07-21 backup state.

**Validation performed 2026-08-22.** Four rows of the original `cohorts.tsv` survive in a session
transcript. Re-deriving those four repos' `event_ts` with the recovered code reproduced all four
**exactly, to the second**:

| repo | expected | got |
|---|---|---|
| `78_xiaozhi-esp32` | 1762782119 | 1762782119 |
| `AMDResearch_omniprobe` | 1772459445 | 1772459445 |
| `AcademySoftwareFoundation_OpenImageIO` | 1757458457 | 1757458457 |
| `AntaresSimulatorTeam_antares-xpansion` | 1758094520 | 1758094520 |

## Pre-flight gates (all must pass before the run starts)

1. **Shallow-clone audit.** `mine_markers` reads `git log --all`; a shallow clone truncates history
   and silently moves the "earliest marker" forward. The Agarwal notes already flag 7 shallow
   repos. Check every cohort clone for `.git/shallow` / `core.repositoryformatversion` grafts, and
   ledger the shallow ones — they are either unshallowed or excluded, never silently included.
2. **Rule-ID diff.** Observed IDs ⊆ mapping. `n_other == 0`.
3. **Mode coverage.** Each of the six surfaces is either scheduled or has a written reason.
4. **Censoring counters recorded**, with a per-arm coverage table planned.
5. **Version pinned and stated.** The published results used `bdeed63` (`v0.1.6-37`); the current
   binary is `0.1.8`, which changed `TEST.1` (whitespace-insensitive production churn) and the
   generated-file classifier. **Results will not be identical to the published ones** — that is
   expected, and the delta must be reported, not quietly absorbed.

## Acceptance criteria

- [ ] Inventory table (surfaces 1–6) reviewed by the author; each marked *measure* or *skip + reason*
- [ ] Harness records every field in A and B
- [ ] `n_other == 0` over the full run; distinct-rule-ID dump attached
- [ ] Shallow audit ledgered; shallow repos unshallowed or excluded
- [ ] Cohorts regenerated; the four validation rows still reproduce exactly
- [ ] Coverage table for the censoring counters, adopters vs controls and agent vs human
- [ ] Version delta vs `v0.1.6-37` measured on a sample and reported

## Assets

- Recovered harness: `experiments/recovered_2026-08-22/scripts_recovered_all/`
  (`adoption_event/`, `review_collapse/`, `external_lists/` — all `py_compile`-clean)
- Outcome mapping fragment: `scripts_recovered_all/per_commit_fragments/run_worklist_categorize.py.fragment`
  (the full `per_commit/run_worklist.py` is lost)
- Raw output of the original runs: `experiments/recovered_2026-08-22/evidence/`
- Data-loss record and archive scan: `experiments/recovered_2026-08-22/README.md`,
  `paper_data/DISCREPANCIES.md`
- Corpus: `~/oss_corpus/{oss,oss_adoption,oss_agentic,oss_agarwal21}`
