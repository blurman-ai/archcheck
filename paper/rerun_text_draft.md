# Re-run text draft for paper_v2.tex

For: the author, before any edit to `paper/versions/paper_v2.tex`. Each block names the place in v2
(line numbers of v2 as of 2026-11-07) and gives the replacement LaTeX. Sources of every number are listed
at the end. The submitted text is backed up in `paper/versions/paper_submitted_backup_2026-11-07.tex`.

Decisions already taken by the author: main estimator = PPML with repository and calendar-week fixed
effects (M3); R1 counts any recorded human review; review volume and depth enter RQ3 as exploratory.

## 1. Abstract, paragraph 3 (v2 lines 46-52)

```latex
In a preregistered adoption event study covering 854 adopting repositories and
517 placebo controls, agent adoption is associated with 1.93\x{} more drift
events per week than in placebo repositories observed in the same calendar
weeks (95\% CI $[1.42, 2.62]$). Per commit the increase is 1.24\x{}
($[0.97, 1.58]$) and per KLOC 1.14\x{} ($[0.96, 1.35]$); cognitive-complexity
growth is the one endpoint that also rises per unit of change (1.17\x{} per
commit, 1.14\x{} per KLOC). Merged pull requests rise 1.38\x{} relative to
placebo, while human review comments per pull request fall to 0.73\x{}.
```

The closing paragraph of the abstract (v2, after line 52) changes to:

```latex
Most of the post-adoption increase in drift comes from more changes arriving,
with a smaller per-change rise in complexity; the number of human review
comments does not grow with the number of pull requests.
```

## 2. Introduction (v2 lines ~105-112 and the third contribution)

Replace "After adoption, however, drift events accumulate faster per week, and the share of pull requests
merged without recorded human review increases." with:

```latex
After adoption, however, drift events accumulate faster per week, mostly
because more changes arrive, and the number of human review comments does not
grow with the number of pull requests.
```

Third contribution: replace "showing that the share of changes merged without recorded human review rises
after adoption" with:

```latex
showing that merged pull requests increase after adoption while human review
comments per pull request decrease.
```

## 3. RQ2 Design: panel size (v2 line 363)

```latex
The confirmatory panel contains 854 adopter repositories and 517 never-adopter
placebo controls, selected by the registered rules from the corpus as of
22~August~2026.
```

## 4. RQ2 Deviations from the registration (v2 lines 386-399, whole paragraph)

```latex
\paragraph{Deviations from the registration.} The registration specified a
Poisson pseudo-maximum-likelihood (PPML) estimator with repository and
date-group$\times$event-week fixed effects (M2). Drift per commit has a
calendar-time shape in both arms: it rises through 2025, peaks in
January--March 2026, and falls afterwards. Placebo windows straddle that peak,
so the placebo post/pre ratio is below one (P1: 0.72) and an event-time
comparison of the two arms mixes adoption with calendar time. We therefore
report as the main estimate a PPML with repository and calendar-week fixed
effects (M3), which compares adopters with placebo repositories in the same
calendar week; the registered M2 estimate is given for P1 in the text. The
estimates submitted for review used pooled post/pre rate ratios on a smaller
panel (361 adopters, 265 placebo) and an earlier analyzer version; the panel
was rebuilt with the registered rules and replayed with archcheck~0.1.8.

Within each repository-week at most ten commits, chosen by hash, were
analyzed, and each is weighted by the number of commits in that week divided
by the number analyzed (210{,}594 analyzed commits). Excluded by outcome-blind
rules: 20{,}480 commits whose analysis exceeded the memory cap (9.7\%;
adopters 14.5\% before and 13.6\% after the event, placebo 9.9\% and 12.0\%),
1{,}656 with unreadable Git objects, 428 bulk imports, 75 pure file moves
($\ge$20 renamed C/C++ files and at least half of the changed C/C++ files),
and 360 for other technical reasons. A pure file move otherwise reports every
include edge of the moved files as new. KLOC counts added C/C++ lines.

The registered clone endpoint P4 is clone \emph{stock}; the panel measures
clone \emph{introductions}, and stock is measured only in the replication of
Section~\ref{sec:agarwal-repl}. TOST equivalence tests at $[0.80, 1.25]$ are
inconclusive for every per-commit and per-KLOC ratio. Not run: the
year-earlier and Dockerfile-adoption placebo controls and the pure-agent
stratum S1.
```

Open question for the author: the dose-response analysis was computed in the re-run (P2 per week 1.35 /
1.43 / 2.71 for zero / low / high post-adoption AI-commit share; per KLOC 1.21 / 1.08 / 1.22). Add one
sentence with it, or leave it in the artifact.

## 5. RQ2 Table 3 (v2 lines 401-422)

```latex
\begin{table*}[t]
\caption{Adoption event study (854 adopters, 517 placebo). Main estimate:
PPML incidence-rate ratio of the adopter post period, repository and
calendar-week fixed effects, owner-clustered 95\% CI. Adopter and placebo
columns: post/pre rate ratio per week of each arm. Pre-trend: slope of the
adopter pre period (SE) and $p$ of the joint pre-period test of the
registered event-study PPML.}
\label{tab:event}
\small
\begin{tabular}{@{}lllllll@{}}
\toprule
Endpoint & /week [95\% CI] & /commit [95\% CI] & /KLOC [95\% CI] & Adopter & Placebo & Pre-trend \\
\midrule
P1 union      & \textbf{1.93} [1.42, 2.62] & 1.24 [0.97, 1.58] & 1.14 [0.96, 1.35] & 1.67 & 0.72 & $+0.033$ (0.018), $p=0.87$ \\
P2 complexity & \textbf{1.79} [1.53, 2.09] & \textbf{1.17} [1.00, 1.37] & \textbf{1.14} [1.02, 1.27] & 1.79 & 0.86 & $+0.0035$ (0.0011), $p=0.06$ \\
P3 flag       & \textbf{1.41} [1.07, 1.85] & 0.93 [0.73, 1.19] & 0.92 [0.75, 1.14] & 1.32 & 0.73 & $+0.0015$ (0.0013), $p=0.004$ \\
P4 new clone  & \textbf{1.75} [1.37, 2.22] & 1.09 [0.89, 1.33] & 1.12 [0.92, 1.37] & 1.54 & 0.62 & $+0.017$ (0.004), $p=0.64$ \\
\bottomrule
\end{tabular}
\end{table*}
```

The threeparttable footnote goes away: the pre-trend is now a column.

## 6. RQ2 Results (v2 lines 426-439, both paragraphs)

```latex
Table~\ref{tab:event} summarizes the confirmatory results. After adoption the
weekly rate of P1 drift events is 1.93\x{} that of placebo repositories in the
same calendar weeks (95\% CI $[1.42, 2.62]$); the registered M2 estimator
gives 1.84\x{} ($[1.08, 3.13]$). Commit volume rises in step: census commits
in the window rise 1.39\x{} in adopters and fall to 0.94\x{} in placebo
repositories. Per commit, P1 rises 1.24\x{} ($[0.97, 1.58]$) and per KLOC
1.14\x{} ($[0.96, 1.35]$). P1 is heavy-tailed: added include edges and new
clones are most of its events, and single commits dominate some repositories;
winsorized at the 99th percentile of repository-week counts, the adopter/placebo
ratio of per-week rate ratios is 1.59.

Complexity (P2) is the least heavy-tailed endpoint and the one where the
per-unit rate also rises: 1.79\x{} per week, 1.17\x{} per commit
($[1.00, 1.37]$), 1.14\x{} per KLOC ($[1.02, 1.27]$). Its adopter pre-period
has a positive slope ($+0.0035$, SE $0.0011$), and the joint pre-period test
gives $p=0.06$, so part of the P2 increase may continue a trend that began
before adoption. Clone introductions (P4) and flag-argument growth (P3) rise
per week (1.75\x{}, 1.41\x{}) but not per commit (1.09\x{}, 0.93\x{}). The
pre-period of P3 fails the joint test ($p=0.004$), and the adopter pre-period
of P4 has a positive slope ($+0.017$, SE $0.004$) although its joint test
passes ($p=0.64$).
```

## 7. Replication paragraph (v2 lines 450-452)

Replace "Weekly complexity events rise (median 1.49\x) while per-commit complexity is flat-to-down (median
0.91\x), again the volume channel, not a per-unit penalty." with:

```latex
Weekly complexity events rise (median 1.49\x); per-commit complexity does not
(median 0.91\x), unlike the 1.17\x{} of the panel.
```

## 8. RQ2 answer (v2 lines 461-464)

```latex
\noindent\textbf{RQ2 answer.} Agent adoption is associated with a higher rate
of architectural drift per week, mostly through more commits. Per commit and
per KLOC the union endpoint rises less, with intervals that include one;
complexity rises per unit as well (1.17\x{} per commit), with a pre-period
trend that weakens this reading. The per-week direction is consistent on an
independent list of 19 repositories.
```

## 9. RQ3 (v2 lines 469-524, whole section body)

```latex
A throughput-driven increase matters only if quality-control mechanisms scale
with the new volume. We measured human review before and after adoption with
the same adopter/placebo framing (owner-clustered bootstrap, LOO, donut,
pre-trends). Up to eight mainline commits per repository and period were
sampled and classified through their pull requests (19{,}978 commits, 844
adopter and 500 placebo owners). Merged pull requests per repository were
counted for the 12 weeks on each side of the event (1{,}372 repositories), and
for the 9{,}050 merged pull requests of the sample we collected every review,
inline comment, and conversation comment, excluding bots and the pull-request
author.

\begin{table*}[t]
\caption{Review capacity before/after adoption. Ratio: adopter post/pre over
placebo post/pre, owner-clustered bootstrap 95\% CI. Volume and comment rows
are exploratory (not registered).}
\label{tab:review}
\small
\begin{tabular}{@{}lllll@{}}
\toprule
Endpoint & Adopter pre $\to$ post & Placebo pre $\to$ post & Ratio [95\% CI] & $p$ \\
\midrule
R1: PRs w/o recorded human review (\%) & 38.7 $\to$ 42.5 & 43.1 $\to$ 44.6 & 1.06 [0.96, 1.18] & 0.29 \\
R2: direct pushes (\%)                  & 46.3 $\to$ 43.4 & 63.3 $\to$ 62.5 & 0.95 & --- \\
R3: PR size (median added lines)        & 43 $\to$ 60     & 41 $\to$ 39     & 1.47 [1.02, 2.14] & 0.04 \\
Merged PRs, 12 weeks                    & 60{,}231 $\to$ 78{,}170 & 17{,}950 $\to$ 16{,}885 & 1.38 [1.21, 1.58] & $<$0.001 \\
Human comments per PR                   & 2.79 $\to$ 2.62 & 2.36 $\to$ 3.05 & 0.73 [0.52, 0.99] & 0.05 \\
\bottomrule
\end{tabular}
\end{table*}

After adoption, adopters merge 1.30\x{} more pull requests while placebo
repositories merge 0.94\x{}, a ratio of 1.38 ($[1.21, 1.58]$). Pull requests
also become larger (median added lines 43 to 60; ratio 1.47). The number of
human review comments per pull request falls to 0.73\x{} of the placebo
trajectory ($[0.52, 0.99]$): the total number of human comments in adopting
repositories stays at the level of placebo repositories (ratio 1.00,
$[0.71, 1.44]$) while the number of pull requests rises. The share of pull
requests merged with no recorded human review rises from 38.7\% to 42.5\%;
against the placebo trajectory this is 1.06\x{} ($[0.96, 1.18]$), not
significant. Fewer changes bypass pull requests after adoption (direct pushes
46.3\% to 43.4\%), so the change happens inside the pull-request workflow.

The registered R5 endpoint, the share of drift-carrying commits merged without
human review, was not reproduced on the re-run panel (ratio 1.02,
$[0.91, 1.14]$); the submitted value (1.10) came from drift commits sampled
directly, and the re-run joins the review sample to the replay sample.
```

Keep the existing paragraph about agent vs human PR review effort (v2 lines 513-518) after this, starting
"Nor is it explained by reviewers spending their effort selectively on agent code"; reword its first words to
"The decrease is not explained by reviewers spending their effort selectively on agent code:".

```latex
\medskip
\noindent\textbf{RQ3 answer.} Human review does not scale with the
post-adoption increase in change volume: adopters merge 1.38\x{} more pull
requests relative to placebo, and human review comments per pull request fall
to 0.73\x{}. The share of pull requests with no recorded human review rises
slightly, without a significant difference from placebo.
```

## 10. Discussion list (v2 lines 536-541)

```latex
\item quality per comparable change, by author: approximately flat (RQ1);
\item change volume per unit time: higher;
\item architectural-drift events per unit time: higher, with a smaller
  per-change rise in complexity;
\item human review comments per pull request: lower.
```

## 11. Threats: Review observability (v2, paragraph after "Repository selection")

Replace the last clause "which makes our R1 estimate, if anything, conservative as a measure of actual
inspection." with:

```latex
Comment counts measure written review, not reading; a reviewer may approve
after reading without comment. The comment and pull-request volume analyses
were added after the registration.
```

## 12. Conclusion (v2 lines ~700-712)

```latex
Across 1{,}188 C++ repositories and 484{,}500 analyzed commits, we find no
evidence that AI-attributed commits are more architecturally damaging than
human commits of comparable size within the same repository. The strongest
observable author difference is that AI-attributed commits are larger. After
agent adoption, repositories accumulate architectural-drift events 1.93\x{}
faster per week than placebo repositories in the same calendar weeks, mostly
through more commits; complexity also rises per commit (1.17\x{}). Adopters
merge 1.38\x{} more pull requests relative to placebo, while human review
comments per pull request fall to 0.73\x{}.

Coding agents increase the rate of change, structural regressions arrive
faster, and the amount of human review stays where it was. For teams adopting
coding agents, deterministic, author-agnostic structural checks in CI scale
verification with generation volume.
```

## 13. Data availability (v2 line 724)

```latex
The re-run per-commit records of the event study (210{,}594 commits), the
cohort definitions, the review panel, and the pull-request counts are part of
the deposit.
```

Question for the author: confirm that these files go into the next Zenodo version.

## Sources

| Number | File |
|---|---|
| Table 3, M2, winsorized, pre-trends, adopter/placebo RR | `paper_data/t4_event_study_rerun2026-11.tsv` |
| M3 per week / commit / KLOC | `experiments/rerun_2026-11/estimates/ppml_calendar.out` |
| R1-R5 per arm | `paper_data/t5_review_collapse_rerun2026-11.tsv` (R1 = row `R1_no_human_reviewer_at_all`) |
| R1, R3, R5 ratios | `experiments/rerun_2026-11/estimates/review_ratio.json` |
| Merged PRs, total comments ratio | `experiments/rerun_2026-11/estimates/pr_volume.json` |
| Comments per PR | `experiments/rerun_2026-11/estimates/review_depth.json` |
| Exclusions | `experiments/rerun_2026-11/estimates/exclusions.tsv`, `REVIEWER_COVERAGE.md` |
| Census commits 1.39 / 0.94 | `panel.tsv`, column `n_commits_census` |
| Dose-response | `REVIEWER_COVERAGE.md` (computed before the rename exclusion; recompute before use) |
