# Adoption event study — pre-registration (task #180)

**Status:** pre-registration draft, committed BEFORE Phase-1 mining. Analysis plan is frozen
here; deviations are logged as amendments with dates. Pilot (Phase 0) results are reported at
the end as pipeline validation only — they do not test the hypotheses (power detects only
≥×1.3–1.7).

## Question and hypotheses

Does a repository accumulate architectural drift *more often per unit time* after it adopts
agentic AI coding tools? The defensible, pre-committed claim is a **rate** claim (volume
channel), plus two per-unit endpoints that carry real prior signal, a clone-**stock**
trajectory endpoint, and a pre-merge-gate endpoint that measures the agent's unfiltered
output directly. Five primary endpoints, frozen here.

- **H-P1 (volume / "more often"):** post-adoption weekly count of drift events rises ×1.5–1.7
  (exposure = weeks). Per-commit rate expected ≈ flat (the established null); the surge is
  driven by more and larger commits.
- **H-P2 (complexity):** `n_complexity` ratchet events rise per week; per-KLOC is the honest
  per-unit test. Prior: the only raw within-repo anti-agent signal (Δ+1.55 pp, p=0.037 before
  size-matching) that died under per-commit matching.
- **H-P3 (flag programming):** `n_bool_field` + `n_flag_arg` accretion rises per week and per
  KLOC. Prior: per-struct boolean-state drift ×2.3 (#119), the one signal surviving size+repo
  FE. Best shot at a per-unit effect.
- **H-P4 (clone STOCK trajectory):** post-adoption whole-repo duplication **stock** grows
  faster in adopters than controls. Motivated by a near-100%-agentic project that accumulated
  19 blatant clones archcheck caught immediately, while merged-history per-commit counts call
  agents clean. **Framing rule (author, 2026-07-10):** drift the agent introduced *counts*
  regardless of whether a human later removed it — later cleanup is review labor, a cost the
  agent drift *imposed*, NOT a mitigation that exonerates the agent. So the endpoint is the
  observable stock level over time, not an introduction-minus-removal ledger. Removal /
  consolidation rate may be reported as an *optional diagnostic* only, never as a defense of
  agents. Measured on duplication-mode snapshots (monthly, [−6,+12] months, vendor-filtered) —
  **Phase 3**, not computable from the per-commit corpus. Phase-0 shows only the introduction
  rate `n_newclone` through the event, reported as a plain fact (not a mechanism claim).
- **H-P5 (pre-gate drift — the agent's unfiltered output):** agent PRs *as submitted* carry
  more drift than matched human PRs in the same repos. Every merged-history design (including
  our own central null) sees only commits that SURVIVED review; if reviewers catch agent drift
  pre-merge, agents look clean in merged history *because a human gate stood in front of them*.
  Measure drift on the submitted state: `git fetch origin pull/N/head` (GitHub retains these
  refs post-merge/branch-deletion), replay `archcheck --diff base..head`, compare agent vs
  matched human PRs. This is the most direct test of "agents PRODUCE drift more often,"
  independent of gate filtering; the as-submitted − as-merged gap estimates the review filter —
  the exact labor archcheck automates. **Phase 3** (needs PR head-ref fetches). Sample:
  300–500 agent PRs + matched human PRs from the #173 corpus
  (`experiments/agentic_weak_review/`, 485 categorized agent-PR candidates + 16,664 enriched).

Plus a **descriptive stratum S1** (never pooled into P1–P5): pure-agent repos (bot-author-share
≥90%), full drift profile per commit and per KLOC with the FIXED scanner, ≥90% per-repo commit
coverage (redoing the #149-crippled ai-377 run), against an age/size-matched young-greenfield
context column. Descriptive only — no causal claim (no pre-agent baseline; youth alone drifts).

## Design

- **Unit of analysis:** repo-week panel for P1–P4. Outcomes from `archcheck --diff
  --diff-mode=memory --format=json` per commit (JSON path only — clone/complexity/nccd/bool
  columns are absent from STDOUT), aggregated to ISO weeks. `n_bool_field` comes from the
  separate `bool_field_added.jsonl` replay, joined on (repo, sha). **P5 is per-PR** (unit =
  pull request, compared within repo across author type), **S1 is per-repo descriptive** —
  neither is pooled into the panel estimates.
- **Adoption event = earliest of four public markers** mined from full local git history:
  first `CLAUDE.md` touch; first commit authored by an agent bot (`copilot-swe-agent`,
  `devin-ai-integration`, `google-labs-jules`, `cursoragent`, `*[bot]` AI forms); first
  `codex/` merge subject; first AI trailer. **Dating precision is prioritized over recall:**
  the loose screen_corpus BOT/TRAILER regexes false-positive on human author names
  ("Co-authored-by: Devin Gibson"), faking 2024 adoptions — the marker miner requires
  unambiguous bot/email forms. Marker↔hand-date lag audited (pilot: |lag| median = 1 day).
- **Estimator:** PPML (Poisson pseudo-ML) with repo and matched-pair×event-time fixed effects
  and log-exposure offsets. Three exposure channels: weeks (volume), commits (per-unit),
  KLOC-added (per-unit, size-robust). NOT NBER-style pre-period-mean normalization (drops
  clean repos with zero pre-period counts = selection on the outcome). Cluster SEs by owner.
- **Eligibility:** ≥50 commits and ≥12 weeks before the event, ≥12 weeks after, owner-dedup,
  not a fork. The pre-history requirement kills the born-agentic/greenfield confound by design.
- **Controls — triangulation:** (a) year-earlier matched controls (NBER anti-contamination);
  (b) contemporaneous never-adopters (marker-free; bias-to-null on hidden AI, clean on
  calendar); (c) **pseudo-adoption placebo** (fake dates from the adopter date distribution
  assigned to never-adopters — must be ≈1); (d) Dockerfile-adoption placebo. Agreement of
  (a) and (b) ⇒ effect real; divergence ⇒ calendar artifact, published explicitly.
- **Pre-trend gate:** 12 pre-adoption weekly coefficients must be jointly flat, else that
  endpoint is reported descriptive, not causal. Donut drops weeks [−4,+4].
- **Dose-response co-primary:** effect vs post-adoption AI-commit share (metadata attribution
  only — bot/trailer/committer, never prose). Monotone dose-response is what turns "correlated"
  into "caused by agents."
- **Robustness (each burned us before):** leave-one-repo-out on every confirmatory estimate;
  bulk_skip (>10k-line commits) as separate outcome + capped-inclusion sensitivity;
  merge-commit share covariate; winsorize 1/99; Romano–Wolf stepdown for secondaries; per-unit
  nulls reported as TOST equivalence with pre-declared bounds, not "p>0.05".

## Endpoints (frozen)

| ID | Family | Metric | Primary exposure | Phase | Prior |
|----|--------|--------|------------------|-------|-------|
| P1 | volume | union: complexity+bool_field+flag_arg+newclone+satd+added_edges | weeks | 0/4 | ×1.5–1.7 |
| P2 | complexity | n_complexity | weeks + KLOC | 0/4 | +1.55 pp raw, died under matching |
| P3 | flag | n_bool_field + n_flag_arg | weeks + KLOC | 0/4 | ×2.3 per-struct |
| P4 | clone stock | duplication-mode STOCK level, snapshots | months | 3 | field obs (19 clones) |
| P5 | pre-gate | drift in agent PRs as-submitted vs matched human PRs | per-PR | 3 | review-filter hypothesis |

Descriptive (never pooled): **S1** pure-agent stratum — full drift profile per commit & per
KLOC, ≥90% coverage gate, matched young-greenfield context column (Phase 3).
Secondary (Romano–Wolf): SATD, test co-evolution, added_edges, cross_area, god headers.
Exploratory only: cycles (base rate 0.11%, no power).

## Related work / niche

- **NBER WP 35275** (Demirer et al., May 2026): 100k+ devs, matched event study, measured
  QUANTITY attenuation (commits +109% sync / Claude Code +199%; LoC +741% → releases +20%).
  Nobody measured STRUCTURE through the event — our gap. We reuse their public-marker dating
  and year-earlier-control trick.
- **Larsen & Moghaddam, arXiv 2606.13298** (SEAA 2026): Java/Arcan, 151 repos, staggered DiD.
  Headline "Architectural Smell Density −6.7%" is a **denominator effect**: raw smell counts
  flat (+1.1%, p=0.82) while LOC grew +12.8%. Their per-smell robust finding is Hub-Like
  Dependency −5.0%. They explicitly do NOT cover: C++/include graphs, cyclomatic-complexity
  ratchets, boolean-flag accretion, commit-level AI attribution/dose. Their "raw counts flat
  while LOC grows" is *consistent with our volume-channel thesis* and their gaps are exactly
  our niche. Adoption is a single binary event for them; no dose-response (they report p=0.716
  and caveat the tool-split as post-hoc).
- **Zhu/Tsantalis/Rigby, arXiv 2605.02741** ("Volume-Quality Inverse Law"): code volume
  near-perfectly predicts structural degradation — supports the volume channel.

## Phase-0 pilot (validation only — NOT a hypothesis test)

Existing data, zero new clones. 1,200-repo per-commit corpus (`results_full.jsonl`, full-
history coverage verified) → dated via `git log --all` → weekly panel (49,998 repo-weeks) →
markers mined on all 1,200 → **212 eligible adopters (owner-deduped), 260 pseudo-adoption
placebo controls**. Windows ±12 weeks.

**Volume-anchor reconciliation.** central_finding §6 reports commits-after/before ×1.67 — that
is the *median of total-history halves* on the lighter `unified_metrics` commit set. On the
full-coverage `results_full` panel the comparable per-week commit-rate surge is **median 1.35,
mean 2.12, 45% of repos ≥1.5×**. The ×1.67 is not reproduced as an identity because it is a
different statistic (unbounded halves, sensitive to where a late-2025 event sits on the
timeline) on a different commit set (`unified` undercounts — e.g. MatrixOS 277 commits vs
1,106 in `results_full`). Direction and order-of-magnitude match; the ×1.67 is a guide, not a
hard gate. Marker↔`unified` dating lag: |lag| median = 1 day (the two dating methods agree).

**Pilot estimates (post/pre pooled rate ratios, owner-weighted; ±12wk):**

| Endpoint | per week [95% CI] | per commit | per KLOC | placebo (weeks) | LOO (weeks) | donut | pre-trend |
|----------|-------------------|-----------|----------|-----------------|-------------|-------|-----------|
| P1 union | 1.34 [1.04, 1.67] | 0.99 | 1.46 | 1.06 | [1.26, 1.38] | 1.38 | flat ✓ |
| P2 complexity | 1.76 [1.28, 2.32] | 1.30 | 1.92 | 0.94 | [1.58, 1.89] | 1.90 | **NOT flat** |
| P3 flag | 1.50 [1.05, 2.09] | 1.10 | 1.64 | 1.11 | [1.29, 1.57] | 1.37 | flat ✓ |
| P4i newclone (intro) | 1.32 [0.98, 1.75] | 0.97 | 1.44 | 1.11 | [1.23, 1.39] | 1.41 | flat ✓ |

Raw pooled event totals (pre→post, 212 adopters): P1 60,449→87,406; P2 11,162→20,568;
P3 2,440→3,830; P4i 18,972→26,270. Commits 18,316→26,038.

**What the pilot establishes (validation goals):**

1. **Pipeline reproduces the volume channel.** P1 per-week 1.34 with per-commit ≈1.0 (0.99):
   more drift per week because more commits, ~no change per commit — the established per-commit
   null, now shown as a *rate* effect. This is the headline mechanism, confirmed operationally.
2. **Pseudo-adoption placebo ≈ 1** across all endpoints (0.94–1.11) → the effect is not a
   mechanical artifact of the window machinery, and the year-shift control design is **not**
   forced (fallback to contemporaneous + bias-to-null not needed at pilot scale).
3. **Pre-trend gate works and already bites P2.** Complexity has the largest raw jump but a
   non-flat pre-adoption trend (slope +0.0073/wk, ~2.2σ) → it must be reported descriptive
   until the donut/matched-pair FE clean it. This likely explains why the raw complexity
   signal died under per-commit matching in prior runs.
4. **P4 introduction rate is flat per commit (0.97), up ×1.32 per week.** Reported as a plain
   fact, not a mechanism: clone introductions rise with commit volume, not per unit of work.
   Whether clone **stock** grows (the actual P4 endpoint, and the 19-clones field observation)
   is a Phase-3 duplication-snapshot measurement. Per the author's framing rule, any later
   human consolidation is review labor imposed by the drift, not an exoneration — so Phase 3
   measures the stock level, and does not net introductions against removals.
5. **Stability:** LOO ranges are tight (no single owner flips a sign); P1 half-split 1.18/1.47.

**Caveats carried into the full study:** the pilot uses pooled rate ratios, not PPML with FE
(no statsmodels in the pilot env) — the full study uses the frozen PPML estimator. Complexity
concentration: top-3 owners hold ~29% of post-adoption complexity events (FlashCpp ~14%),
LOO-robust but flagged. KLOC channel is noisier than commits (bulk_skip zeros, squash merges).

## Amendments

_(log design deviations here with dates)_

**2026-07-11 (before Phase 3) — reporting hierarchy for exposure channels.** The harm claim
of this study is the ABSOLUTE per-week accumulation of drift events (P1–P3 primary exposure
stays weeks, unchanged). Per-commit / per-KLOC rates are reclassified from "the honest
per-unit test" to **mechanism attribution**: they show through which channel the accumulation
flows (volume), and are reported alongside the per-week rate but never as a mitigation of it.
Rationale (author's rule, mirroring the P4 cleanup rule): maintainer and reviewer capacity is
measured in person-weeks, not KLOC — dividing complexity by code volume does not reduce the
burden the team carries, it answers a different question ("is agent code worse line-by-line?")
which this study does not claim. Volume denominators can hide degradation: Larsen & Moghaddam's
headline −6.7% smell *density* decomposes into flat raw counts over +12.8% LOC; conversely
Agarwal/He/Vasilescu (arXiv:2601.13597) headline an un-normalized +35% total cognitive
complexity on +77% lines — the same volume channel our P1 measures, presented without the
decomposition. We report both numerator and denominator explicitly, always. Consequence: the
per-KLOC puzzle from the pilot (1.44–1.92 vs per-commit ≈1.0) is demoted from a citation
blocker to a Phase-4 diagnostic; no per-KLOC result gates the per-week headline. Endpoint
definitions, exposures, and TOST bounds in the frozen table are unchanged.

**2026-07-12 (P5 run) — P5 operational choices (prereg named no drift columns / matching for P5).**
(A1) Outcome = the P1 drift union computed by `archcheck --diff --format=json` via the panel's own
`run_worklist.categorize()`, so P5 is comparable to P1–P4. (A2) As-submitted base = `merge-base(base,
head)`; diff `mb..head` two-dot (archcheck rejects three-dot). (A3) Human arm freshly enumerated per
agent-repo from GitHub (merged, non-bot, C++-touching, capped ≤3×/40) — only 10 agent-repos had a
harvested human C++ PR, so within-repo matching required live enumeration; per-PR is the finding,
per-KLOC mechanism. (A4) Load-bearing estimator = within-repo median of per-repo (agent − human)
contrasts, owner-clustered bootstrap + LOO; pooled shown only for composition. (A5) Review-filter
gap = submitted − merged. **Correction recorded honestly:** the gap came out null *by construction*
— `pull/N/head` is the PR's final head and the merge lands it unchanged, so `merge^1..merge` ≈
`mb..head` (0 for 284/286 PRs, 155/157 nonzero-drift ones). It measures "did merge alter the final
head" (≈never), NOT "did review change the agent's original submission." The intended
first-pushed-head-vs-merged test needs PR push-event history and is **not run**. Result:
`p5_as_submitted_drift.md` — H-P5 (agent dirtier as submitted) **not supported** (within-repo
median −0.21, CI to 0); raw pooled agent-excess is repo composition; sign not robust to size, so no
robust difference either way. G1 head-ref fetch 99.8% (PASS). Two giant monorepos (tt-metal,
onnx-light) dropped to per-PR scan timeout.
