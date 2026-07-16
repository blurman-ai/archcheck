# Full-endpoint adoption event study (task #184)

**Status:** one run over every endpoint archcheck computes, on the #180 sample. No
pre-aggregation; unions decomposed; low-n endpoints kept and flagged. Post-selection for the
launch write-up happens from the table, not in the script.

**Reproduces #180 on the shared endpoints** (identical estimator, weeks = ISO-week panel rows):
P1_union/wk 1.32, complexity/wk 1.71, newclone/wk 1.37, flag/wk 1.26 — all matching the
pre-registered numbers to two decimals. What changed from `build_panel.py`: this run dedups
(repo,sha) across sources (the old panel double-counted a few overlapping commits, <0.1% of
cells). Everything else is the same machinery.

## Provenance (reproducible)

- Sample: `cohorts.tsv` — **361 adopters / 265 pseudo-adoption placebo**, event = first agent
  marker, same `event_ts`.
- Estimator: pooled post/pre rate ratio, owner-clustered bootstrap (2000 reps) CI + two-sided
  p, pre-trend OLS on [-12,-1], placebo through identical machinery. Window ±12; ±26 only for
  the time series.
- Seed 1234567; archcheck `bdeed63` (v0.1.6-37); corpus `/home/localadm/oss`.
- Script `experiments/adoption_event/full_endpoint_study.py`; chart `plot_study184.py`.
- Outputs (gitignored): `study184_endpoints.csv` (endpoint × normalizer), `study184_timeseries.csv`
  (week −26..+26 × cohort × endpoint × normalizer → median/p25/p75), `study184_chart.png`.

## The chart

![production vs drift](../../experiments/adoption_event/study184_chart.png)

Pooled per-offset rates, each series indexed to its pre-period ([-12,-1]) level. Flat pre-trend,
a step at week 0, then a sustained plateau: **commits/week and drift-events/week rise together;
drift *per commit* stays flat.** Placebo wobbles around 1.0 with no step at 0. (The x-axis is
capped at ±13 — the range where n stays ≥290; beyond ±12 the post-history thins to ~90 repos and
a few high-drift repos produce spurious tail spikes.)

## Every endpoint (weeks channel = the finding; per-commit = per-unit mechanism)

RR = adopter post/pre; placebo = same on never-adopters. Full CIs/p in the CSV.

### Volume channel, differential vs placebo (adopter clears placebo)

| endpoint | /week | placebo/wk | /commit | note |
|---|---|---|---|---|
| n_complexity | **1.71** | 1.16 | **1.42** | only endpoint whose per-unit lift survives |
| n_bool_field | **1.55** | 0.77 | 1.28 (ns) | per-week up, per-unit not significant |
| n_removed_edges | 1.34 | 1.18 | 1.11 | |
| n_newclone | **1.37** | 1.06 | 1.13 (ns) | |
| n_test | **1.28** | 1.14 | 1.06 | bookkeeping category |
| n_added_edges | 1.28 (p.06) | 1.09 | 1.06 | |
| graph_drift (cycles∪cross∪edges) | 1.28 (p.08) | 1.10 | 1.05 | union |
| **P1_union** | **1.32** | 1.09 | 1.09 (ns) | headline union |

### Per-week rise NOT differential (placebo ≈ or > adopter → calendar/noise, not adoption)

| endpoint | /week | placebo/wk | read |
|---|---|---|---|
| n_new_god_headers | 1.36 | 1.16 | weak, ns |
| n_flag_arg | 1.13 | 1.35 | placebo higher — no adoption effect |
| n_new_cross_area | 0.97 | 1.72 | placebo higher — no adoption effect |
| n_chain_grown | 1.07 | 1.53 | placebo higher |
| n_bool_struct | 1.36 | 2.64 | placebo higher; noisy (n=61) |
| changed_files | 1.14 (ns) | 1.19 | placebo higher (partial coverage, 512/626) |
| median_commit_size | **1.20** | **1.20** | identical → calendar, not adoption |

### Negative / null

- **n_grown_cycles** DOWN 0.63/wk (ns, n=18) — cycles do not rise with adoption; matches the
  launch positioning (old cycles are the detector's proof, not the disease).
- **n_satd** flat 0.88/wk, and DOWN per-commit (0.73, p.03) — fewer TODO/FIXME per commit after
  adoption.
- **nccd_delta** 0.92 (mean, ns) — no adoption effect on the NCCD component metric.
- **P3_flag** 1.26/wk but CI crosses 1 (p.20); per-commit 1.04 — the flag claim does not carry
  in the adoption design (as in #180).

## Reading

The volume channel is broad and robust: nearly every architectural-drift endpoint accumulates
**faster per week** after adoption, and the differential (adopter minus placebo) is real for the
core set (complexity, bool_field, newclone, added_edges, the P1 union). The **per-commit / per-unit
channel is flat almost everywhere** — the single exception is local cognitive complexity
(1.42/commit, p.001), which is worth stating honestly rather than rounding to "per-unit flat."
Several apparent per-week rises are shared with placebo (median commit size 1.20=1.20; god-headers;
cross-area and flag-arg where placebo is *higher*) and are calendar, not adoption.

This is the #180 story across the full endpoint set: **agents don't write worse code per commit,
they generate more of it faster than review scales** — with local complexity as the one per-unit
smell that persists. The selection of endpoints for the launch post is the author's to make from
`study184_endpoints.csv`.

## Addenda (follow-up questions)

### `pooled_rate` column in the time series

`study184_timeseries.csv` now carries `pooled_rate` alongside `median/p25/p75`:

    pooled_rate(week) = Σ events over cohort repos at that offset / Σ exposure (repo-weeks)

absolute, **not** indexed, **not** per-KLOC (repo-weeks = repos whose history span covers the
week). Added because for sparse endpoints (complexity, newclone, graph_drift) the per-repo
median is 0 in every week — no line can be drawn from it — while the pooled rate is the same
absolute curve the chart already used. Populated on `per_week` rows only, for the endpoints
`n_complexity, n_newclone, n_satd, n_bool_field, n_added_edges, graph_drift, commits, added_loc`,
both cohorts, weeks −26..+26.

### What is actually in `P1_union` (docs vs code)

`n_events_pre = 72212` because `P1_union` in **code** is the six-component sum, not "grown cycle
∪ new cross-area edge" as some positioning notes said. Adopter pre-window breakdown:

| component | pre-window sum | share |
|---|---|---|
| n_added_edges | 38239 | 53% |
| n_newclone | 17076 | 24% |
| n_complexity | 8166 | 11% |
| n_satd | 5757 | 8% |
| n_flag_arg | 2041 | 3% |
| n_bool_field | 933 | 1% |
| **sum** | **72212** | |

`n_grown_cycles` (52) and `n_new_cross_area` (235) are **not** in `P1_union` — that is why the
union dwarfs them. The endpoint that matches the "cycles/cross-area" wording is the separate
`graph_drift` = grown_cycles ∪ new_cross_area ∪ added_edges (also decomposed in the CSV). **Code
is authoritative; the doc wording was wrong** — `run_worklist.categorize` + the `UNIONS` table in
`full_endpoint_study.py` define the truth.

### Q1 — naming the culprits: top-20 adopter commits by added_loc

`git diff --numstat` per commit, top-3 extensions and top-3 top-level dirs by added lines.
Confirms the hypothesis — the tail is data/vendoring, not code:

| added | date | repo | top-3 ext | top-3 dir |
|---|---|---|---|---|
| 3,574,948 | 2024-12-18 | aatxe_lute | (binary-only diff) | |
| 3,149,935 | 2025-05-16 | JamesZwq_nclique | `.sorted` 2.10M | (root) 3.15M, `src` |
| 2,946,117 | 2026-02-13 | opensta | (binary-only diff) | |
| 2,625,085 | 2026-01-24 | numaes_protoCore | `.xml` 2.31M, `.cc` 0.11M | `xml/` 2.31M, `build/` 0.16M |
| 2,540,749 | 2025-10-20 | christoph2_pyA2L | `.a2l`/`.A2L` 2.40M+ | `examples/`, `pya2l/`, docs |
| 2,443,752 | 2025-03-01 | JamesZwq_nclique | `.txt` 2.03M, `.edges` | `results/`, `graphs/` |
| 1,807,120 | 2026-01-23 | numaes_protoCore | `.xml` 1.80M | `docs/`, `test/`, `core/` |
| 1,775,491 | 2025-12-22 | AntaresSimulatorTeam_antares | `.mps` 1.78M, `.cpp` 0.16M | `data_test/`, `src/` |
| 1,156,570 | 2025-10-15 | AntaresSimulatorTeam_antares | `.txt` 1.08M, `.csv` 0.70M | `data_test/` |
| 1,049,892 | 2025-03-19 | JamesZwq_nclique | `.edges` 1.05M | `graphs/` |

(full 20 in the run transcript.) Exactly the expected pattern: solver benchmark data (`.mps`,
`.a2l`, `.edges`, `.sorted`), fixture/results dumps, and a couple of binary-diff commits (large
files git treats as opaque — `numstat` shows `-`, contributing 0 to `added_total` via text lines
but the repo-diff itself is dominated by binary blobs). No `third_party/`/`vendor/` hits in the
top-20 specifically — this corpus's tail is **benchmark/fixture data checked into the main tree**,
not vendored source trees. (Vendor dirs matter lower in the tail — see Q3.)

### Q2 — what `numstat_added` actually counts

From `experiments/per_commit/run_worklist.py::numstat_added`: `git diff --numstat <sha>^..<sha>`,
summing the added-lines column for **every line where that column is a digit**. That means:
- **all text files count** — json, txt, fixture data, generated code, vendored source; nothing
  is filtered by extension or path.
- **binary files are excluded** (git prints `-` for them, which fails `.isdigit()`).
- **merge commits are excluded upstream**, not by this function: the main corpus's commit list
  comes from `gen_full.py` (`git rev-list --no-merges --min-parents=1 -- <C/C++ globs>`), so the
  commit *set* is non-merge and touches ≥1 C/C++ file — but once a commit is selected,
  `numstat_added` sums lines across **all** files it touched, C/C++ or not.

**So yes — `added_total` / `added_loc` measures text-volume, not code-volume.** This is exactly
why `median_commit_size` (median = robust to the tail) reads 1.20 in both cohorts while the pooled
mean swings by cohort — see below.

### Q3 — `added_loc_trimmed`: does the tail go away?

Filter: keep only C/C++ extensions (`file_classification.h::kSourceExtensions`), drop
`third_party/ vendor/ external/ deps/ third-party/` paths and `*.generated.*` / `*.pb.cc` /
`*.pb.h`, then drop whole commits above the cohort's 99th-percentile `added_total`
(cut: adopter 4544 LOC, placebo 2474 LOC). Computed via streaming `git log --numstat` per cohort
repo (script: `build_trimmed.py`; output: `study184_added_loc_trimmed.csv`, pooled per_week,
weeks −26..+26, endpoints `added_loc_raw` / `added_loc_trimmed` / `commits`).

**Pre-period spikiness (max/median), measured on the estimator's actual pre-window [-12,-1]**
(restricting to this window matters — week −26 has only 108 active repos vs ~345 near the event,
so a wider window pulls in a thin-sample tail artifact that inflates the ratio independent of any
real trimming):

| cohort | commits (baseline) | added_loc raw | added_loc trimmed |
|---|---|---|---|
| adopter | 1.22× | 2.60× | **1.23×** |
| placebo | 1.46× | 3.72× | **1.16×** |

**Criterion met for both cohorts** — trimmed drops to commit-level smoothness (1.2–1.5× target).
The tail is vendoring/generated/data, as hypothesized; no further culprit search needed.
(First pass of this diagnostic used the full [-26,-1] range and wrongly showed adopter at 1.81× —
an artifact of including the thin-tail edge weeks; caught before reporting, corrected to the
[-12,-1] window used everywhere else in this study.)

### `median_commit_size` 1.20 vs added-LOC-per-commit ≈ 1.0 — median vs a heavy tail

These measure different things and the commit-size distribution is extremely tail-heavy (the
**top 1% of commits carry 74–86% of all added LOC** — vendored trees, generated files, bulk
imports):

| cohort | median commit size (typical commit) | pooled mean added/commit (tail-dominated) |
|---|---|---|
| adopter | 20 → 24  (**×1.20**) | 877 → 894  (**×1.02, flat**) |
| placebo | 15 → 18  (**×1.20**) | 319 → 383  (×1.20) |

So the *typical* commit grew ~20% in both cohorts (calendar, not adoption — placebo matches). The
*mean* added-per-commit is driven by a handful of monster commits and is unstable: for adopters
it is flat (×1.02 ≈ 1.0). Whichever number reads as "≈1.0" is a mean/tail artifact, and the
robust reading is the median. Note: I could not reproduce a **placebo** added-LOC-per-commit of
exactly 0.99 from any estimator here — pooled placebo is 1.20, per-repo median-of-ratios 1.11; the
only 0.99 in the outputs is `n_satd per_commit` placebo (0.9882). If the 0.99 came from a specific
cell, point me at it and I'll trace it exactly.
