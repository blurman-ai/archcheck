# Review Collapse: review erosion after agent adoption (task #185)

**Build date:** 2026-07-12 · **Task:** #185 · **Genre:** evidentiary line for the article, Faros
mechanism replication attempt on C++ OSS metrics.

> **Bottom line.** Faros (22k devs, commercial telemetry) found that after AI adoption, PRs merged
> with zero review rose +31.3% and PR size rose +51-60%; their read is "reviewers don't scale with
> volume." We cannot reproduce their numbers (closed data, review-*time* is noisy in OSS), but the
> same mechanism replicates on objective OSS facts: after the adoption event, the share of PRs
> merged with **no human review** rises **+27%** for adopters (45.8% → 58.1%) while never-adopters
> stay flat (49.2% → 48.0%) — clean placebo contrast, p<0.001. PR size rises **+62%** (median 37→60
> added lines), also flat on placebo. And the thing nobody else measures: the share of **structural
> drift that reaches main with no human review at all** rises too (67.3% → 74.1%, p=0.001, placebo
> flat) — though this one is front-loaded and less robust than R1 (see Robustness §; weakens to
> p=0.059 if the first three post-weeks are dropped). Review-effort on agent-authored PRs (P5 frame,
> within-repo) shows **no significant
> difference** from human PRs on any axis (reviews, comments, CHANGES_REQUESTED) — consistent with
> recent prior art that agentic PRs are not reviewed *more*, and directly relevant to the "C++
> reviewers are more disciplined" hypothesis: **not supported** here either — see R7.

---

## Prior art (read before interpreting the results below)

- **arXiv 2605.02273** "These Aren't the Reviews You're Looking For": 61.4% of agentic PRs get zero
  review; 71.6% of review comments are themselves agent-authored; human participation is mostly
  *agent steering* ("rewrite this"), not code judgment. Direct warning: **review-presence metrics
  do not indicate human oversight.**
- **Gao et al. 2026**: 79% of human+AI PR merges happen with zero human comment.
- **arXiv 2601.18749**: more review activity raises merge odds for human PRs, **lowers** them for
  agentic PRs.

**Consequence.** "C++ reviewers are more disciplined and filter agentic code" is already refuted
for Python/general GitHub by the above. Neither paper has a C++ slice — that's this task's actual
contribution: does C++ differ? Both outcomes are informative; see §6 (R7).

## Method

Same estimator family as #180/P5: pooled post/pre rate ratio (RR), owner-clustered bootstrap 95%
CI (2000 reps), two-sided bootstrap p, leave-one-owner-out (LOO) range, donut [-4,+4] weeks,
pre-trend OLS slope on the pre-period only. `cohorts.tsv` (361 adopters / 265 never-adopter
placebo), ±12-week event window, event = first agent-adoption marker.

- **R1–R4** (`experiments/review_collapse/r1345_fetch_panel.py`): for each cohort repo, up to 3
  mainline (first-parent HEAD) commits sampled per period (pre/post), classified via
  `gh api commits/{sha}/pulls` → reviews (bucket definitions reused verbatim from
  `inverted_bot_drift.py`, #146/#174's classifier). PR size/timestamps fetched once per unique PR
  (cached). 3,034 commits classified, 244 (adopter) / 263 (placebo) distinct owners.
- **R5** (`r5_fetch_drift_review.py`): drift events = commits in the existing per-commit corpus
  (`experiments/per_commit/results_full.jsonl` + `dates*.tsv`) with any P1-union counter nonzero
  (complexity/bool_field/flag_arg/newclone/satd/added_edges), restricted to the ±12w window and to
  cohort repos covered by that corpus (465/626 = 74% by basename match — not re-scanned). Up to 3
  drift commits sampled per period per repo from that population (28,376 raw candidates — too
  large to classify in full within API budget), same classifier. 2,726 drift commits classified.
- **R6** (`r6_fetch_effort.py`): reuses the P5 frame verbatim (291 agent + 260 human eligible PRs,
  18 both-arm repos, `experiments/adoption_event/p5/`). Per PR: submitted-review count,
  CHANGES_REQUESTED count, review-comment count, commits after the first review. Estimator =
  median of per-repo (agent − human) contrasts, repo-clustered bootstrap CI, LOO — identical shape
  to P5's own estimator.
- **R7**: no new data — R1's C++ level compared, indicatively only, against the prior-art numbers
  above.

**Definitions.** R1 = among commits that went through a PR (excludes direct-push), fraction merged
with reviews empty or every reviewer a bot (`coderabbitai`, `copilot-*-reviewer`,
`gemini-code-assist`, `sourcery-ai`, generic `[bot]` CI accounts, …) — an agent/bot review counts
as *not* human oversight, per the prior-art warning above. R2 = fraction of mainline commits with
no associated PR at all (direct push). R5 = R1 ∪ R2 computed on the drift-event population only
("did this structural regression reach main with zero human eyes on it, via either path").

**Self-check.** 5 rows hand-verified against raw `gh api` output across R1/R2/R3/R4/R5/R6 (bucket,
review list, PR size, comment/commit counts) — all matched exactly, including one cross-check of
the underlying drift signal (`n_added_edges=3, n_newclone=1`) for an R5 row. Owner concentration
checked for the R1 adopter sample: top 5 of 244 owners = 7.2% of rows — no concentration issue.
Median-based RRs (R3/R4) cross-checked by hand against raw pre/post medians — exact match.

## Results

### R1 — PRs merged with no human review (main endpoint)

| | pre | post | RR | 95% CI | p | LOO | donut | pretrend |
|---|---|---|---|---|---|---|---|---|
| **adopter** | 45.8% (n=450) | 58.1% (n=499) | **1.270** | [1.134, 1.447] | **<0.001** | [1.241,1.288] | 1.343 | 0.012/wk |
| placebo | 49.2% (n=309) | 48.0% (n=306) | 0.977 | [0.876, 1.093] | 0.689 | [0.963,0.990] | 0.949 | 0.000/wk |

Flat pre-trend, clean placebo (no step at the pseudo-event), a real +27% rise for adopters only.
Faros' analogous number is +31.3% — same order of magnitude, different metric/environment, not a
reproduction claim.

### R2 — direct push, no PR at all

| | pre | post | RR | 95% CI | p |
|---|---|---|---|---|---|
| adopter | 38.8% (n=735) | 34.8% (n=765) | 0.897 | [0.791, 1.005] | 0.062 (marginal) |
| placebo | 59.1% (n=756) | 60.3% (n=771) | 1.020 | [0.981, 1.064] | 0.352 |

**Direction is the opposite of R1** — direct-push share goes *down*, not up, after adoption.
Reading: agent-assisted workflows (Copilot, Claude Code PR flows) route through PRs by
construction; adopting them shifts volume *into* the PR channel rather than around it. The erosion
is not "agents bypass PRs" — it's "the PRs agents create/touch don't get looked at." R2 and R1
together are why R5 (below) matters more than either alone.

### R3 — PR size (added lines, median)

| | pre | post | RR | 95% CI | p |
|---|---|---|---|---|---|
| adopter | 37 | 60 | **1.622** | [1.125, 2.557] | **0.011** |
| placebo | 34 | 37.5 | 1.103 | [0.618, 2.059] | 0.715 |

Faros analog +51-60%; here +62%, placebo flat. Pooled means are far higher (1886/964) and heavily
outlier-driven (a handful of vendored-code-dump PRs) — the median is the load-bearing number;
pretrend slope (raw OLS, outlier-sensitive) is not informative for this endpoint and is not used
as a gate here.

### R4 — PR open-to-merge latency, median minutes [descriptive only]

| | pre | post | RR | 95% CI | p |
|---|---|---|---|---|---|
| adopter | 1216 | 987 | 0.812 | [0.506, 1.155] | 0.231 |
| placebo | 1283 | 1512 | 1.179 | [0.745, 2.102] | 0.480 |

Noisy on both arms, CI crosses 1 throughout, no placebo contrast. **Not a Faros-×5 analog** — OSS
merge latency is dominated by maintainer availability, not reviewer load, exactly as flagged in the
task design. Reported for completeness, not as a finding.

### R5 — structural drift reaching main with NO human review (R1 ∪ R2, drift-conditioned)

| | pre | post | RR | 95% CI | p |
|---|---|---|---|---|---|
| **adopter** | 67.3% (n=620) | 74.1% (n=645) | **1.102** | [1.035, 1.177] | **0.001** | | |
| placebo | 77.0% (n=712) | 78.3% (n=743) | 1.018 | [0.982, 1.053] | 0.325 |

**This is the endpoint nobody else measures.** Two in three drift-carrying commits already reach
main unreviewed before adoption (base rate is high everywhere — see caveats); after adoption that
climbs another 6.8 points for adopters specifically, with a flat placebo. Framing: *the share of
architectural regressions that cross into main with zero human eyes on them keeps rising after
agent adoption, on top of an already-high base rate.* **Caveat (see Robustness §): this effect is
front-loaded in the first post-adoption weeks and weakens to marginal (p=0.059) once weeks +0..+2
are excluded — it is real but smaller and less robust than R1.**

### R6 — review effort, agent vs human PR, within-repo (P5 frame, 18 repos)

| metric | contrast (median, agent−human) | 95% CI | p | pooled agent / human mean |
|---|---|---|---|---|
| submitted reviews | −0.111 | [−1.456, 0.500] | 0.838 | 2.53 / 2.17 |
| human reviews | +0.083 | [0.000, 0.513] | 0.857 | 0.79 / 0.83 |
| bot reviews | −0.361 | [−1.456, 0.000] | 0.236 | 1.74 / 1.34 |
| CHANGES_REQUESTED | 0.000 | [0.000, 0.000] | 1.000 | 0.02 / 0.01 |
| review comments | −1.389 | [−3.167, 0.000] | 0.101 | 4.14 / 3.66 |
| commits after 1st review | −0.491 | [−1.767, 0.000] | 0.219 | 0.84 / 1.72 |

## Robustness — is the post-adoption rise an artifact of the event definition?

The adoption event is a **marker commit**, so an adopter's week 0 contains a commit *by
construction*; the placebo pseudo-date is a random calendar date (from the adopter date
distribution, `build_cohorts.py`), **not** anchored to a commit. This asymmetry produces a real
week-0 spike in the **count/volume channel** (see the companion #184 study): adopter empty-week
fraction is ~25% in the pre-period but collapses to **1.7% at week 0**, with mean commits jumping
8.6 → 18.87; the placebo stays flat (~32% empty, ~6.5 commits at every offset, including week 0).
The marker commit itself is normal-sized (median 25 added lines vs the repo's 18-line median
commit; ratio 0.90), so the week-0 count spike is a **density-anchoring** artifact, not a fat-commit
one. And weeks 0..+2 drift-events are **52% concentrated in the top-5% of repos** (18 of 361).

**Does this contaminate #185's per-commit rates?** #185 samples commits *across the full ±12-week
window* (not per-week bins), so the single anchored week-0 contributes ~2% of the sample. Two donut
tests:

| endpoint | baseline (post +0..+12) | drop week 0 only (pure artifact) | drop burst +0..+2 |
|---|---|---|---|
| R1 | 1.270 (p<0.001) | **1.269 (p<0.001)** | 1.201 (p=0.007) |
| R5 | 1.102 (p=0.001) | **1.104 (p=0.001)** | 1.071 (p=0.059) |
| R3 | 1.622 (p=0.017) | 1.541 (p=0.028) | 1.486 (p=0.052) |

- **The pure event-definition artifact (week-0 anchoring) does not bias any endpoint** — dropping
  week 0 leaves R1 and R5 essentially unchanged.
- **R1 is fully robust**: it survives even dropping the entire +0..+2 burst (1.201, p=0.007,
  placebo flat). The review-erosion signal is spread across the whole post window, not front-loaded.
- **R5 and R3 are partly carried by the early post-weeks.** R5's post rate by offset band:
  pre 0.673 → +1..+2 **0.844** → +3..+5 0.757 → +6..+12 0.722. The +1..+2 elevation is the strongest
  part; removing it drops R5 to marginal (1.071, p=0.059). Weeks +1..+2 are **not** the anchoring
  artifact (that's week 0 alone) — they are plausibly a real early-adoption ramp — but they overlap
  the repo-concentration zone (§Q4), so R5's headline 1.10 should be read as **"a real but
  front-loaded effect; the sustained tail (+6..+12, 0.722 vs 0.673 pre) is smaller and its CI would
  touch 1."** R5 is the least robust of the significant endpoints; R1 is the one to lean on.

**No significant difference on any axis.** All CIs include or touch 0. Directionally, agent PRs get
slightly *fewer* human reviews, *fewer* comments, and markedly fewer follow-up commits after the
first review (pooled ratio 0.49) — none of it significant at n=18 repos, but every arrow points
toward *less* engagement, not more, which sides with the prior-art reading over the "reviewers work
harder on agent code" hypothesis (see task's own two-branch framing). **This does not mean agent
PRs are dirtier** — P5 already showed no excess drift as-submitted; R6 only says reviewers do not
spend measurably more (or less) effort catching it.

## R7 — is C++ more disciplined? (indicative only, no statistical comparison)

Our adopters' **pre-adoption** R1 level (45.8%) sits below the general-GitHub agentic-PR figure
from arXiv 2605.02273 (61.4% no-review) — consistent with "C++ repos start out more disciplined."
But the **post-adoption** level (58.1%) closes almost the entire gap. Read plainly: whatever
review-discipline head start C++ repos have erodes after adopting agents, converging toward the
same weak-review norm the general-GitHub literature already documents. This is **not** a
statistical test (no non-C++ cohort exists in this corpus to contrast against) — it is one
corpus's before/after trajectory landing near a number from a different corpus and methodology.
Stated as an observation, not a finding.

## Caveats (stated, not buried)

- **Sample size was cut mid-run** (k=8 → k=3 commits/period/repo) to fit an interactive-session API
  budget; final panel is 3,034 commits / 244-263 owners, drift panel 2,726 / 209-256 owners. Power
  is adequate for the headline endpoints (R1, R3, R5 all clear p<0.05 with tight CIs) but R2/R4's
  marginal/null results could sharpen with a fuller re-run — flagged, not filled.
- **R5's drift population covers 465/626 (74%) of cohort repos** — repos outside the existing
  per-commit corpus (`results_full.jsonl`) are silently absent, not re-scanned. Not believed to be
  systematically biased (coverage gaps are largely clone/mining-pipeline artifacts, not adoption-
  correlated) but unverified.
- **"Human review" = ≥1 non-bot login on the PR's review list.** Per the prior-art warning, this
  cannot distinguish real judgment from a rubber-stamp comment, and cannot see offline/pair review
  that never touches the GitHub API. Both R1 and R5 numbers are upper bounds on real oversight, in
  both periods equally (so the pre/post *contrast* is more trustworthy than either level alone).
- **R2's direction (down, not up) is a genuine finding, not noise-cleanup** — stated plainly in
  §R2 rather than smoothed into "no change."
- **R3/R4 pretrend slopes are raw-value OLS**, dominated by outlier PR sizes/latencies; not used to
  gate R3/R4's read. The RR itself (median-based) is the load-bearing number and was hand-verified.
- **R6 inherits every P5 caveat**: ~75% Copilot agent arm, median ~5-6 PRs/repo, 18-repo frame.
  Small n means the directional pattern (less engagement, not more) is suggestive, not confirmed.
- **Two rate-limit backoffs occurred** during the R1345/R5 runs (`openwrt/mt76`, `zlib-ng/zlib-ng`)
  — both scripts backed off and resumed automatically (resumable JSONL, verified 0 data loss), but
  those two repos' samples may be slightly time-shifted relative to others.
- **Not claimed:** Faros ×5 reproduction (different metric/environment — R4 explicitly descriptive
  only); "C++ review is better/worse than Python" as a statistical claim (R7 is indicative only);
  R6 null ⇒ "reviewers catch agent drift" (P5 already answered that question; R6 measures effort,
  not catch rate).

## Assets

- Fetchers (gitignored data, scripts tracked): `experiments/review_collapse/r1345_fetch_panel.py`,
  `r5_fetch_drift_review.py`, `r6_fetch_effort.py`.
- Shared estimator: `experiments/review_collapse/estimator.py` (owner-clustered bootstrap RR, LOO,
  pretrend, donut — same shape as `pilot_eventstudy.py`); driver `run_estimates.py`; R6-specific
  `r6_estimate.py`.
- Reuses `classify()`/`slug_of()` from `experiments/trending_run/inverted_bot_drift.py` (#146/#174)
  and the P5 frame (`experiments/adoption_event/p5/`, `docs/research/p5_as_submitted_drift.md`).
- Related: `docs/research/adoption_event_study.md` (#180), `docs/research/gate_moderator_interaction.md`
  (#174, cross-sectional composition null — this task is the before/after complement),
  `docs/research/bot_review_drift.md` (#146, the 304/648 baseline this extends into a time series).
