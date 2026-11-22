# DISCREPANCIES

Every value in `paper_data/` was checked against the assertion list supplied with the task and,
where the original run output survived, against that output. This file records what did not line up.

## Assertions that all passed

| assertion | result |
|---|---|
| corpus 1188 repos, 484,500 commits, 72,778 AI / 408,716 human / 3,006 automation | ✓ (and the three-way split sums exactly to 484,500) |
| RQ1 344 repos | ✓ (all seven categories sum worse+better+ties = 344) |
| complexity +0.06 pp, p=0.44, 172/157/15 | ✓ |
| satd −0.52 pp, p=1.6e-10 | ✓ |
| newclone −0.23 pp, p=0.0073 | ✓ |
| event study n=361 adopters / 265 placebo | ✓ (raw run output) |
| P1 1.32 [1.08, 1.61], placebo 1.09 | ✓ (raw) |
| P2 1.71 [1.34, 2.16] | ✓ (raw) |
| P3 1.26 [0.89, 1.81] | ✓ (raw) |
| P4 1.37 [1.11, 1.70] | ✓ (raw) |
| review R1 45.8→58.1 adopter, 49.2→48.0 placebo | ✓ (and both reproduce the printed RR) |
| PR size 37→60 | ✓ |
| R5 67.3→74.1, p=0.001 | ✓ (raw) |
| R5 weakens to p=0.059 without the first three post-weeks | ✓ |

## 1. `review_collapse.md` states two different p-values for R3

The document's own §R3 table reports **p = 0.011**. Its §Robustness donut table reports the same
baseline estimate as **1.622 (p=0.017)**.

The original estimator output (`evidence/r1_r5_review_collapse_raw.txt`, 2026-07-12T18:15:28Z) reads:

```
ADOPTER  RR=1.622 CI=[1.125,2.557] p=0.011 n_pre= 449 n_post= 499 ...
```

`t5_review_collapse.tsv` carries **0.011**. The `0.017` appears to be from a separate bootstrap
draw (the donut runs re-bootstrap), and R3's p sits close enough to 0.01 that resampling noise
moves it. **Action for the paper: quote 0.011 and drop the 0.017, or state the bootstrap
seed-sensitivity explicitly.** This is a live inconsistency inside a published doc, not a
transcription error in these tables.

## 2. T7 per-repo table is a mid-run snapshot; its median disagrees with the published aggregate

Published median complexity/week over Agarwal's repos: **1.49**.
Recomputed from `t7_agarwal_replication.tsv`: **1.48** (n=14 repos with a complexity value).

Cause: the per-repo table was printed at 2026-07-11T15:25:06Z while the replay was at ~6,304 of
6,700 commits; the published aggregate comes from the completed replay at 20:34:10Z. No per-repo
table was ever printed from the final run, so the two cannot be reconciled without re-running the
replay — and the input dataset (`results_agarwal.jsonl`) is lost.

Corroborating evidence that this is a snapshot effect, not a transcription error: an aggregate
printed one minute *after* the per-repo table (15:26:24Z, still mid-run) reports median
complexity/week **1.42**, pooled 1.37 — different again from both. The other four T7 statistics
(commits/wk 1.56, kloc/wk 2.03, complexity/commit 0.905≈0.91, dup median 0.0%, and the 9/10
small/large split) reproduce the final aggregate exactly.

**Action for the paper: cite the aggregate row from the doc; if per-repo rows are shown, label them
as a 94%-complete replay snapshot.**

## 3. T7 duplication-stock range differs in the last digit

Doc and final aggregate: range **−12.0 … +159.0 %**.
Recomputed from the per-repo rows in `t7`: **−11 … +159 %**.

The minimum is `chrxh/alien`, printed per-repo as `171.0 -> 151.2 (-11%)`. The unrounded value is
151.2/171.0 − 1 = **−11.58 %**, which the aggregate rounds to −12 and the per-repo line truncates to
−11. Same underlying number, two rounding conventions. Benign; noted so a referee recomputing the
range from the per-repo table is not surprised.

## 4. Pooled complexity ratio 1.76 does not follow from the printed percentages

`t2_rigor_ladder.tsv` level 1 quotes 27.0% vs 15.4%, ratio 1.76 — as published. Recomputing from
the two displayed percentages gives **27.0 / 15.4 = 1.753**, which rounds to 1.75.

The ratio was evidently computed on unrounded rates (e.g. 27.04/15.36 = 1.760) and the percentages
rounded for display independently. Benign, but a referee *will* divide those two numbers.
**Action for the paper: either report the ratio to two significant figures as 1.8×, or publish the
percentages to one more decimal.**

## 5. R5's confidence interval was printed twice with different bounds

Two runs on 2026-07-12 printed R5 as `CI=[1.035,1.177]` and `CI=[1.033,1.174]`. The document and
`t5_review_collapse.tsv` use `[1.035, 1.177]`. Both intervals exclude 1 and the point estimate is
identical (1.102); the difference is bootstrap resampling noise. No action beyond fixing the
estimator seed if the CI is quoted to three decimals.

## Not a discrepancy, but a gap the paper must disclose

**The primary datasets are gone.** `results_full.jsonl`, `cohorts.tsv`, `unified_metrics.tsv`,
`panel_weekly.tsv` and `results_agarwal.jsonl` did not survive the 2026-07-21 machine reinstall
(`experiments/` was gitignored and the backup excluded this repo on the assumption git covered it).
Analysis code and printed results were recovered from session transcripts; the inputs were not.

This was verified exhaustively, not inferred from the backup's own documentation: all 14 reinstall
archives (137 GB compressed) were listed in full and searched for
`results_full|cohorts.tsv|unified_metrics|panel_weekly|results_agarwal|cpparch`. Two archives
returned non-zero counts, both explained:
`dotfiles_auth.tar.zst` matches only `.claude/projects/…-cpparch/` transcript paths (752), and
`oss.tar.zst` matches exactly two files — `unified_metrics.h` and `unified_metrics.c` inside the
corpus repository `SpharxTeam/AgentOS`, a substring collision with no relation to
`unified_metrics.tsv`. **No archive contains any of the lost data files.**

Consequences for the submission:

- Every table here is reproducible *as a number*, and the code that produced it is available, but
  **no table can currently be recomputed end-to-end** without re-running the pipeline over the corpus.
- The corpus clones themselves **do** survive (`~/oss_corpus/`, plus
  `backup_reinstall_2026/oss*.tar.zst`), so a full re-run is possible in principle; it is not a
  small job.
- T3's raw run output is additionally unavailable (that run predates the surviving transcripts by
  five days), so `t3_seven_categories.tsv` rests on the document alone.
