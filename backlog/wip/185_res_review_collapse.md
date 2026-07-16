# 185 — Review Collapse: review erosion after agent adoption (Faros mechanism, C++ metrics)

**Type:** research
**Status:** wip (2026-07-12)
**Depends on:** #180 (`cohorts.tsv`, `pilot_eventstudy.py`), #180/P5 (`experiments/adoption_event/p5/`,
`docs/research/p5_as_submitted_drift.md`), #174 (`inverted_bot_drift.py` gate classifier — null
composition result; this task is the before/after time-series complement #174 did not run),
#146 (304/648 direct-push baseline)

## Motivation

Faros (22k devs, commercial telemetry): after AI adoption, median review time +441%, PRs merged
with zero review +31.3%, PR size +51-60%. Their read: reviewers don't scale with volume. We
cannot reproduce their metrics (closed data, review-time is noisy in OSS — maintainer on vacation
≠ overload). We measure the same mechanism with metrics that are objective in OSS, and add what
nobody else measures: **structural drift that reaches main unseen by a human.**

## Prior art — read before interpreting results

- **arXiv 2605.02273** "These Aren't the Reviews You're Looking For": 61.4% of agentic PRs get
  zero review; 71.6% of review comments are agent-authored; human participation is mostly *agent
  steering* ("rewrite this"), not code judgment. Direct warning: **review-presence metrics do not
  indicate human oversight.**
- **Gao et al. 2026**: 79% of human+AI PR merges happen with zero human comment.
- **arXiv 2601.18749**: more review activity raises merge odds for human PRs, **lowers** them for
  agentic PRs (a seriously-reviewed agentic PR is more likely closed than fixed).

**Consequence for framing.** "C++ reviewers are more disciplined and filter agentic code" is
**already refuted for Python/general GitHub** by the above. **None of the three have a C++ slice.**
Our contribution is testing whether C++ differs — both outcomes are informative: match → the
general law replicates on a new language (+ our R5 shows drift still gets through); diverge → a
counter-finding against three recent papers.

## Design

Same #180 estimator family: adoption event, ±12w window, pseudo-adoption placebo (never-adopters,
same machinery), owner-clustered bootstrap CI, leave-one-out, pre-trend gate, donut [-4,+4].
Reuse `cohorts.tsv`, `pilot_eventstudy.py`, `p5_fetch_meta.py` patterns (`gh api`, token +
backoff, resumable). R6 reuses the P5 frame (291 agent + 260 human eligible PRs, 18 both-arm
repos).

## Endpoints

- **R1** (main) — fraction of PRs merged without HUMAN review. Strict definition per prior art:
  `reviews` empty **OR** every reviewer is a bot (copilot, coderabbit, gemini-code-assist,
  sourcery, CI bots — an agent reviewer looks like a review in the API but is not oversight).
  Faros +31.3% analog; objective binary fact in OSS.
- **R2** — fraction of commits reaching main by direct push, no PR at all. Baseline: 304/648
  (`docs/research/bot_review_drift.md`) — turn into a before/after time series.
- **R3** — PR size (added lines, files/PR), per-PR. Faros +51.3%/+59.7% analog.
- **R4** — PR-open-to-merge time (median). Descriptive only, NOT a Faros-×5 claim — different
  quantity in OSS. State this explicitly in the write-up.
- **R5** (our unique contribution) — structural drift (P1 union) that crossed into main
  unreviewed, before/after adoption. R1/R2 ∩ drift events. Framing: "how many architectural
  regressions reached main unseen by any human, before vs after agent adoption." Everyone else
  measures *how* agentic PRs get reviewed; nobody measures *what gets through* structurally.
- **R6** — review effort: agent-authored vs human-authored PR, within-repo, P5 frame. Per-PR:
  submitted-review count, review-comment count, CHANGES_REQUESTED count, commits after first
  review. Estimator = median of per-repo contrasts, owner-clustered CI, LOO.
  Both branches are informative: agent PRs get MORE iterations/comments but P5 already showed no
  excess drift as-submitted → reviewers spend more effort on agentic code and still don't catch
  structure (archcheck's human-hours pitch). Agent PRs get the SAME or LESS → matches prior art
  (agentic PRs reviewed less), filter-hypothesis dead either way.
- **R7** — indicative only: level/trend of R1 on this C++ cohort vs prior-art numbers (61.4%
  no-review, 79% no-comment) and Faros (+31.3%). No non-C++ cohort here — comparison is
  indicative, not statistical. State this explicitly.

## What NOT to claim

- Not "we reproduced Faros ×5" — different metric, different environment.
- Not "C++ review is better/worse than Python" — no non-C++ cohort. R7 is indicative only:
  "in our C++ repos, unreviewed-PR share is X, trend is Y; for comparison, general GitHub is 61.4%."
- **R6 positive ≠ "reviewers catch agent drift."** P5 already showed no excess drift as-submitted.
  R6 measures effort spent, not drift caught — do not conflate.
- Review-presence ≠ human oversight (prior art). Bot review does not count as review.

## Method notes

R1/R2/R3/R4/R5 use `cohorts.tsv` (361 adopters / 265 placebo) with commit-level sampling per
period + `gh api commits/{sha}/pulls` → reviews (reusing `classify()` from
`experiments/trending_run/inverted_bot_drift.py`, extended to fetch PR size/timestamps once per
unique PR, cached). R5 drift signal from the existing per-commit corpus
(`experiments/per_commit/results_full.jsonl` + `dates.tsv`) — covers 470/626 cohort repos (75%,
by basename match); gap stated, not filled by re-scanning. R6 reuses the P5 frame verbatim
(`p5/pr_meta.tsv` + `p5/results.jsonl` for eligibility), fetches reviews/comments/commits per PR.

## Deliverable

`docs/research/review_collapse.md`, format of `p5_as_submitted_drift.md`: bottom line up front,
method, table with CI/placebo/LOO, honest caveats, what was NOT run.

## Progress

**2026-07-12** — task scoped (3 revision rounds from the author), prior-art check folded in.
Coverage check done: per-commit drift corpus covers 470/626 cohort repos (75%). Implemented and
ran all three fetchers (reusing `inverted_bot_drift.classify()` from #146/#174), fixed 2 bugs
caught before showing results (R1 wrongly conflated with R2 via `no_pr` bucket; a bootstrap
p-value formula bug that misread a degenerate all-zero distribution as p=0.000 "significant"
instead of p=1.000 "no evidence" — fixed with inclusive `<=`/`>=` comparisons in both
`estimator.py` and `r6_estimate.py`). 5 rows hand-verified against raw `gh api` output across
every endpoint, all matched exactly. K reduced from 8 to 3 commits/period/repo mid-run (ETA was
~9h at k=8 with two concurrent fetchers; deterministic seeded shuffle meant the k=3 sample was a
strict prefix of already-fetched k=8 data, so no work was wasted).

**Result — DONE, deliverable written.** `docs/research/review_collapse.md`. Faros mechanism
replicates on C++ OSS with clean placebo contrasts: R1 (PRs merged without human review) +27%
for adopters (45.8%→58.1%, p<0.001), placebo flat. R3 (PR size) +62% (p=0.011), placebo flat.
R5 (structural drift reaching main unreviewed, the task's unique contribution) +6.8pp
(67.3%→74.1%, p=0.001), placebo flat. R2 (direct push) goes DOWN not up (0.897 RR, marginal) —
a real, counter-intuitive finding, explained in the write-up (agent workflows route through PRs
by construction). R4 (merge latency) correctly noisy/null as predicted, reported descriptive
only. R6 (review effort, agent vs human, within-repo P5 frame) shows no significant difference
on any axis — directionally agent PRs get less engagement, not more, siding with prior art over
the "reviewers compensate with effort" hypothesis. R7 (indicative): C++ cohort starts more
disciplined than the general-GitHub prior-art baseline (45.8% vs 61.4%) but the post-adoption
level (58.1%) nearly closes the gap.

JOURNEY appended+pushed to the companion repo.

**2026-07-12 (skeptic-pass, author's 4 questions on the weeks-0..+2 peak).** Verified the
event-definition artifact: adopter week 0 is nonzero by construction (empty-week fraction
25%→1.7% at week 0; mean commits 8.6→18.9), placebo pseudo-date is random/non-anchored
(`build_cohorts.py`) so its week 0 stays flat (~32% empty). Marker commit is normal-sized
(ratio 0.90 to repo median) — the spike is density-anchoring, not a fat commit. Weeks-0..+2
drift is 52% concentrated in the top-5% of repos. **Impact on #185:** the count/volume channel
(#184) is contaminated at week 0, but #185's per-commit rates are NOT — dropping week 0 leaves
R1 1.270→1.269 and R5 1.102→1.104 unchanged (per-commit sampling across the full window makes
week 0 ~2% of the sample). R1 survives even dropping the whole +0..+2 burst (1.201, p=0.007);
R5 and R3 are front-loaded and weaken to marginal (R5 1.071 p=0.059) when the burst is dropped.
Added a Robustness § to `review_collapse.md` + hedged the R5 bottom-line. Artifact-check script:
`experiments/review_collapse/artifact_check.py`.

**Status: analysis complete, robustness-checked; awaiting user command to move to completed/.**
