# MANIFEST — provenance for every number in `paper_data/`

Built 2026-08-22. Each table below lists where every value came from, how to get it back,
and the SHA-256 of the file it was read out of.

## Provenance classes

| class | meaning |
|---|---|
| `raw` | read out of the original run's tool output, recovered verbatim from the pre-reinstall session transcripts (see §Recovery) |
| `doc` | read verbatim from a research document in `docs/research/`; the primary data file no longer exists |
| `NOT_REPRODUCED` | the value could be neither recomputed nor located in any document; the cell is left `NA` |

**No value in these tables was estimated, interpolated, or inferred.**

## Recovery note (read this first)

The primary data files named in the research docs — `experiments/per_commit/results_full.jsonl`
(520,177 records), `cohorts.tsv`, `unified_metrics.tsv`, `panel_weekly.tsv` — **no longer exist.**
`experiments/` was covered by `.gitignore` (root `.gitignore:89`), so it was in neither git repo, and
the machine was reinstalled (Astra Linux → Ubuntu) on 2026-07-21/22. The reinstall backup
(`backup_reinstall_2026/projects_rest.tar.zst`) deliberately excluded `cpparch`, on the assumption
that git covered it. It did not cover the gitignored subtree.

What **was** recovered, on 2026-08-22, from `dotfiles_auth.tar.zst` (the pre-reinstall `~/.claude/`
snapshot, taken 2026-07-21) and now stored under `experiments/recovered_2026-08-22/`:

- **89 session transcripts** covering 2026-06-23 … 2026-07-21 (`transcripts_pre_reinstall/`),
  including the verbatim tool output of the #180, #182, #184 and #185 runs.
- **90 analysis scripts** replayed out of the transcripts' `Write`/`Edit` payloads
  (`scripts_recovered_all/`) — every `adoption_event/` and `review_collapse/` script the docs cite.
  All of them pass `python3 -m py_compile`, i.e. they are complete files, not truncated excerpts.
- **4 raw-output extracts** (`evidence/`) — the exact console output the published tables were
  written from.

Datasets (the `.jsonl`/`.tsv` inputs themselves) are **not** recoverable; only the code and the
printed results are. Regenerating them requires re-running the pipeline over the corpus.

## Source files and hashes

| file | sha256 |
|---|---|
| `docs/research/central_finding.md` | `5b3560e5122d1a3b834fa0a935010067471f43fd3b3affccfd0d8513597e2573` |
| `docs/research/agent_drift_within_repo.md` | `7ef78d1fa81fdf1b24d3a1b46fb7a69f6989397b298e0ba7dc47ea0759e9ae16` |
| `docs/research/adoption_event_study.md` | `d447b027b7859690b6724a98fd7fe4fbe3a5213bc587d4e0d1824972cf0ff2b7` |
| `docs/research/adoption_event_study_prereg.md` | `9ab403927e73c2c171ff33b906b5daa864fc221b4638c61b230d81333a4b8fef` |
| `docs/research/review_collapse.md` | `d66d44ddf7d6343455e4ab507e593d1f32f09a168641f524f34c66925d5a121c` |
| `docs/research/p5_as_submitted_drift.md` | `44b61355c0a0f7a95fe92b591a0561148dd3337863f4a7d8a99545845d0ffa2e` |
| `docs/research/agarwal_replication.md` | `fc30e0de34566b75520b152443edfda3441976662ef0b3fbbb3aea7c8bd4197f` |
| `docs/research/all_endpoints_event_study.md` | `33783ce216ed1f6dfe56aa34cdf04a326063d335a123eae4719c9ab4f6f440b2` |
| `evidence/p1_p4_event_study_raw.txt` | `c9b10650a31dc86f6cd0b9fec5006f14d8562a359683a05c95238d569d4dcf28` |
| `evidence/r1_r5_review_collapse_raw.txt` | `4ef9b3fe7b8fe53b0b5ba7d15556dd8340c13a0c77e099b4b402626e169ad470` |
| `evidence/agarwal_replication_raw.txt` | `0259a2c2c6228573efef9b4cfaf559c5c23fef8985700175900e3789d57f288d` |
| `evidence/p5_as_submitted_raw.txt` | `fb07d6f0d870ae803752e56d95e07eaebe8ac28449623976851e5d1d427b1d20` |

`evidence/` paths are relative to `experiments/recovered_2026-08-22/`.

## Tool version

The runs behind T4/T7 were made with archcheck `bdeed63` = `v0.1.6-37-gbdeed63`
(stated in `all_endpoints_event_study.md` §Provenance; the commit is present in this repo).
Estimator seed `1234567`. Corpus root at run time was `/home/localadm/oss`, which is
`/home/localadm/oss_corpus/oss` after the Ubuntu migration.

---

## t1_corpus.tsv

| column | value | class | source |
|---|---|---|---|
| `repos` | 1188 | doc | `agent_drift_within_repo.md:4` header, `central_finding.md:23` |
| `commits_total` | 484500 | doc | same; 520,177 commits attempted, 484,500 ok |
| `commits_ai` | 72778 | doc | `agent_drift_within_repo.md:§3`, `central_finding.md:24` |
| `commits_human` | 408716 | doc | same |
| `commits_automation` | 3006 | doc | same ("non-AI automation") |
| `median_loc_ai` | 50 | doc | `agent_drift_within_repo.md:§3` "corpus median 50 vs 21 LOC" |
| `median_loc_human` | 21 | doc | same |
| `size_ratio_within_repo` | 1.26 | doc | same, "within-repo median ratio 1.26×, 140 vs 96 repos, p = 0.005" |

Checks run: `72778 + 408716 + 3006 = 484500` ✓ ; `72778 / 484500 = 15.02%`, matching the
documented "15.0% of commits AI-attributed" ✓.

Recompute (needs the lost dataset):
```
wc -l experiments/per_commit/results_full.jsonl          # 520177 attempted
# ok-commit count and the agent/human/automation split come from authorship_join.py
```

## t2_rigor_ladder.tsv

All three rows: class `doc`, from `agent_drift_within_repo.md` §3 "The complexity claim, in three
levels of rigor" (table), cross-stated in `central_finding.md` §2.

| level | numbers taken | source line |
|---|---|---|
| 1 pooled | 27.0% vs 15.4%, ratio 1.76 | `agent_drift_within_repo.md` §3 level-1 row |
| 2 within-repo raw | Δ +1.55 pp, 135/102 repos, p=0.037 | §3 level-2 row |
| 3 within-repo + size-matched | Δ +0.06 pp, 172/157 repos, p=0.44 | §3 level-3 row |

`ratio` for level 1 is quoted as printed (1.76). Recomputing it from the two rounded percentages
gives 27.0/15.4 = 1.753 — see `DISCREPANCIES.md` §4.

`agent_rate_pct`/`human_rate_pct` are `NA` on levels 2–3 because the source reports those levels as
a paired within-repo delta, not as two rates.

## t3_seven_categories.tsv

Class `doc` for every cell — `agent_drift_within_repo.md` §3 "All categories, within-repo +
size-matched (344 repos)", restated identically in `central_finding.md` §3.

Column mapping: the source column "repos A>H / A<H" is split into `repos_agent_worse` (A>H) and
`repos_agent_better` (A<H).

Check run: every row sums `worse + better + ties = 344` ✓ (all seven categories).

The raw sign-test output for this run is **not** recoverable — the run is dated 2026-06-18 and the
recovered transcripts begin 2026-06-23. The document is the primary record.

## t4_event_study.tsv

Class `raw` for every cell. Source: `evidence/p1_p4_event_study_raw.txt`, the tool output of the
confirmatory run, timestamped **2026-07-11T21:02:13Z**, transcript
`973c1cf3-9e78-4701-8cc4-54ef9e0a95f5.jsonl`. Values agree digit-for-digit with
`adoption_event_study.md` §"Confirmatory results".

Columns beyond the requested set — `donut_week`, `pretrend_slope`, `pretrend_se` — are present in
the raw output and are kept because the pre-trend gate is otherwise unauditable from the table alone.

`n_adopters=361`, `n_placebo=265` from the same output's header line
(`adopters=361  controls(placebo)=265`).

Recompute (needs the lost dataset):
```
python3 scripts_recovered_all/adoption_event/pilot_eventstudy.py    # on panel_weekly.tsv + cohorts.tsv
```

## t5_review_collapse.tsv

`pre`/`post` columns: class `doc` (`review_collapse.md` §R1/§R3/§R5 tables) — the estimator printed
counts and ratios, not the two period rates.
All other columns: class `raw`, from `evidence/r1_r5_review_collapse_raw.txt`, run timestamped
**2026-07-12T18:15:28Z**, transcript `1b35c653-79a6-485e-ac5f-a686509182f6.jsonl`
(header `panel rows: 3034  drift rows: 2726`).

`rr_drop_week0` / `rr_drop_weeks0to2` and their p-values: class `doc`, `review_collapse.md`
§Robustness donut table. Placebo arms were not donut-tested, hence `NA`.

Checks run (pre/post against the independently-printed RR):

| endpoint | post/pre | printed RR | agrees |
|---|---|---|---|
| R1 adopter | 58.1/45.8 = 1.2686 | 1.270 | ✓ |
| R1 placebo | 48.0/49.2 = 0.9756 | 0.977 | ✓ |
| R3 adopter | 60/37 = 1.6216 | 1.622 | ✓ |
| R5 adopter | 74.1/67.3 = 1.1010 | 1.102 | ✓ |
| R5 placebo | 78.3/77.0 = 1.0169 | 1.018 | ✓ |

R3's p-value is `0.011` here, taken from the raw estimator output; the document states `0.011` in
its own §R3 table but `0.017` in the Robustness table — see `DISCREPANCIES.md` §1.

Recompute (needs the lost dataset):
```
python3 scripts_recovered_all/review_collapse/run_estimates.py     # drives estimator.py
```

## t6_p5_as_submitted.tsv

Class `raw` for every cell. Source: `evidence/p5_as_submitted_raw.txt`, transcript
`973c1cf3-9e78-4701-8cc4-54ef9e0a95f5.jsonl`, run 2026-07-12. Header line:
`ok PRs: 1100 | eligible (C++-touch, non-bulk): 551 (agent=291, human=260)`;
`G1 head-ref fetched: 1110/1112 = 99.8% (gate >=80%: PASS)`.

`n_owner_clusters = 17` is class `doc` (`p5_as_submitted_drift.md` §Method — the raw output prints
repos, not owner clusters).

The raw output also carries a per-repo agent/human breakdown, but it prints only the top 12 of the
18 both-arm repos, so no per-repo P5 table is emitted here.

Recompute (needs the lost dataset):
```
python3 scripts_recovered_all/adoption_event/p5_estimate.py
```

## t7_agarwal_replication.tsv

Per-repo rows: class `raw`, from `evidence/agarwal_replication_raw.txt` — the
`=== per-repo table (sorted by complexity/wk ratio) ===` block and the
`=== dup density change pre->post ===` block, transcript
`5b0cdc73-d3ba-444a-815d-650cedac41de.jsonl`.

**Snapshot caveat — read before citing a per-repo number.** The per-repo table was printed at
**2026-07-11T15:25:06Z**, while the commit replay was still running (`6304/6700` commits at
15:26). The aggregate that the paper cites comes from the *completed* replay at
**2026-07-11T20:34:10Z**. The two are close but not identical — see `DISCREPANCIES.md` §2. No
per-repo table was ever printed from the final run, so this is the only per-repo evidence that
exists.

`size_class` is derived (`large` iff `pre_commits >= 100`), reproducing the raw output's own split
(`small<100 pre-commits: 9, large>=100: 10`) exactly ✓.
`direction_complexity_wk` / `direction_dup_stock` are derived: `up` if ratio > 1 / change > 0,
`down` if < 1 / < 0, `flat` if change = 0, `NA` if the source cell is `None`.

`NA` cells, and why:

| repo | missing | reason |
|---|---|---|
| `commaai/panda`, `zeroc-ice/ice-demos` | all ratio columns | degenerate pre/post window — excluded from the n=19 split by the source run (`agarwal_replication.md` §Caveats) |
| `microsoft/onnxruntime`, `microsoft/react-native-windows` | dup-density columns | absent from the dup-stock block in the raw output; **NOT_REPRODUCED** |
| 5 repos (`TermGraph`, `Helios`, `Effekseer`, `NepTrainKit`, `sailor`) | complexity columns | printed as `None` by the source run (no complexity events in one period) |

Checks run against the *final* published aggregate:

| statistic | recomputed from t7 | published | agrees |
|---|---|---|---|
| median commits/wk | 1.56 (n=19) | 1.56 | ✓ |
| median kloc/wk | 2.03 (n=19) | 2.03 | ✓ |
| median complexity/commit | 0.905 (n=14) | 0.91 | ✓ |
| median dup-density %change | 0.0 (n=19) | 0.0 | ✓ |
| small/large split | 9 / 10 | 9 / 10 | ✓ |
| median complexity/wk | **1.48** (n=14) | **1.49** | ✗ — §2 |
| dup %change range | **−11 … +159** | **−12 … +159** | ✗ — §3 |

Recompute (needs the lost dataset):
```
# full pipeline recovered under scripts_recovered_all/external_lists/:
#   build_worklist_agarwal.py -> clone_agarwal.py -> dup_snapshots_agarwal.py
#   -> build_agarwal_report.py   (results_agarwal.jsonl itself is lost)
```
