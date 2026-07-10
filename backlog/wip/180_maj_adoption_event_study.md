# 180 maj — Adoption event study: do agents drift MORE OFTEN? (complexity + flag programming first)

Status: wip
Created: 2026-07-10
Start date: 2026-07-10
Executor: intended for a strong model (Opus). Delegate mechanical steps (joins, sanity
counts, per-repo audits) to Haiku subagents per docs/dev/haiku_task_guide.md — token economy
is an explicit requirement from the author.

## Goal (author's framing, and the honest version of it)

The author needs the claim: **"agents produce architectural drift more often than humans,
therefore archcheck is more relevant than ever."**

The defensible version of that claim, given everything this project has already measured, is
a RATE claim, not a per-unit-quality claim: **after agent adoption, a repository accumulates
drift events per unit time ×1.5–1.7 faster** (volume channel: more and larger commits at
equal-or-worse per-unit rates ⇒ strictly more drift per week). On top of that, two metric
families have real prior signal of a per-unit effect and are the PRIORITY outcomes here
(explicit author instruction: "not only the graph — mainly complexity and flag programming"):

1. **Complexity** (`DRIFT.LOCAL_COMPLEXITY`, `n_complexity`): the only raw within-repo signal
   against agents (Δ +1.55 pp, p = 0.037 before size-matching; #119 hint p = 0.032). It died
   under per-commit size-matching — but for the "more often" claim size is NOT a confounder,
   it is part of the mechanism: agents make ~1.3× larger commits, and commit size is the
   universal drift driver. Per-week complexity-ratchet rate is expected to rise.
2. **Flag programming** (`DRIFT.BOOL_FIELD_ACCRETION` → `n_bool_field`/`n_bool_struct`;
   ARG* flag-argument rules → `n_flag_arg`, task #093): the ONE surviving "agents worse"
   signal in the whole program — per-struct boolean-state drift agentic 9.8% vs 6.0%
   (eye-checked corrected 5.6% vs 2.4% ≈ **2.3×**), and the bool_field×complexity coupling
   survives size+repo FE (#119, confirmed case-by-case as "flag → branching in same module").
   This family is the best shot at a per-unit effect that survives controls.

Graph metrics (added_edges, cross_area, cycles, god headers) stay in as secondary/volume
outcomes; cycles are exploratory only (base rate 0.11% — no power, established).

Do NOT re-run a fourth per-commit "are agent commits worse, size-matched" test as the
headline — that null is established three ways (central_finding.md; #115 repo-FE death;
#173: 0 cycles on 104 agent-reviewed PRs). The design below measures something none of the
prior runs measured: the repo-level drift-rate TRAJECTORY through the adoption event.

## Why this design (external anchors)

- **NBER WP 35275** (Demirer, Musolff, Yang, May 2026, "Writing Code vs. Shipping Code",
  https://www.nber.org/papers/w35275 — PDF verified read): 100k+ GitHub devs, matched event
  study around tool adoption. Adoption identified from PUBLIC data for three tools —
  Claude Code = user's first commit touching `CLAUDE.md`; GitHub Async Agent = PRs authored
  by `copilot-swe-agent[bot]` (human attributed via co-author trailers / requested reviewers
  / assignees); Codex = branch prefix `codex/`. Control = a developer with the same activity
  profile ONE YEAR EARLIER (anti-contamination). Placebo tools: GitHub Pro (→ ~0 long-run),
  Docker (+23% vs +109% sync agents). Effects: commits +36% autocomplete / +109% sync
  (Claude Code +199%) / +34% agent-authored async; attenuation LoC +741% → releases +20%.
  Claude Code signs >5% of ALL public commits by early 2026. They measured QUANTITY
  attenuation; nobody measured STRUCTURE through the adoption event. That is our gap.
- **Cui, Demirer et al., Management Science 2025** (3 RCTs, N=4,867, +26.08% tasks/week):
  experimental assignment, methodologically irrelevant to us, but the external anchor for the
  velocity story (already reconciled in central_finding.md §6, commit fc01ab4).
- **Nearest competitor:** Larsen & Moghaddam, arXiv 2606.13298 (June 2026) — causal study of
  architectural quality under agentic adoption, JAVA, ARCAN smells, propensity matching.
  READ IT in Phase 0 and differentiate: our niche (C++ physical design, include graph,
  complexity, flag accretion; commit-level attribution) is empty. Also cite Zhu/Tsantalis/
  Rigby arXiv 2605.02741 ("Volume-Quality Inverse Law": code volume is a near-perfect
  predictor of structural degradation — directly supports the volume-channel framing).
- **AIDev is a confirmed dead end** for us (#176, docs/research/aidev_dataset_evaluation.md):
  author-identity selection, ~1 year stale, 2.6% repo overlap. Cross-validation only.

## What changes vs our previous runs

### Changes in SEARCH (corpus construction)

1. **Unit of discovery = adoption event, not "agentic repo".** For each candidate repo, mine
   the EARLIEST of four public markers from FULL git history (the screen_ledger agentic flag
   was computed on ≤100 sampled commits — good for screening, unusable for dating):
   - first commit touching `CLAUDE.md` (NBER's Claude Code signal);
   - first PR / merged commit authored by `copilot-swe-agent[bot]` (also
     `devin-ai-integration[bot]`, `google-labs-jules[bot]` for tagging, though NBER dropped
     them as <10% of async activity);
   - first merge of a `codex/`-prefixed branch (recoverable locally from merge-commit
     subjects "Merge pull request … from …/codex/…"; else via gh api);
   - first AI trailer (`Co-Authored-By: Claude <noreply@anthropic.com>`, `Generated with
     Claude Code`, Cursor/Codex/Devin trailers) — reuse the exact regexes from
     `screen_corpus.py` / `agent_author_scan.py`.
   Note: trailers/markers live in commit message BODIES → local `git log` only (GH Archive
   does not carry commit messages; verified externally). This is cheap on clones.
2. **Adopter pool:** the 7,510 `agentic=1` rows of
   `experiments/ai_repo_run/screen_ledger.tsv` (29,106 screened, #122 funnel). Eligibility:
   ≥12 weeks AND ≥50 commits BEFORE the adoption event, ≥12 weeks after, not a fork,
   owner-deduplicated. The pre-history requirement kills the born-agentic/greenfield confound
   BY DESIGN (the #115/#174 dead ends: ThemisDB-type repos have no baseline; pure-agent
   repos live 40 days median). Target: ≥250 eligible adopters; confirmatory stratum = repos
   with ≥20% AI-attributed commit share post-adoption (dose from the authorship pipeline).
3. **Controls — triangulation, not a single alignment** (repo architecture is non-stationary:
   measured secular within-repo drift with NO adoption: complexity rates +13% first→second
   half, SATD −23%; a lone year-shifted control would eat that as fake treatment):
   - (a) matched controls from the 21,596 `agentic=0` pool, outcomes taken ONE YEAR EARLIER
     (NBER anti-contamination trick);
   - (b) contemporaneous never-adopters (contaminated by hidden AI → bias-to-null; clean on
     calendar). If (a) and (b) agree → effect is real; divergence = the calendar artifact,
     publish it explicitly;
   - (c) **pseudo-adoption placebo**: fake adoption dates (drawn from the real adopters' date
     distribution, they cluster H2-2025) assigned to never-adopters, run through the
     IDENTICAL pipeline. Must be ≈0. This is the strongest referee defense and costs nothing.
   - (d) Dockerfile-adoption placebo (NBER-style non-AI tool) from the same pool.
   Matching covariates: pre-12-week weekly commit activity, repo age, size (KLOC), owner
   dedup. Cluster SEs by GitHub owner, not repo.
4. **Clone hygiene** (all lessons already paid for, central_finding.md §12): clone with
   `--shallow-since = (event − 16 weeks)`; `GIT_LFS_SKIP_SMUDGE=1`; exclude parentless /
   shallow-boundary commits AT SOURCE (they fake "whole tree added"); verify each repo's
   pre-window is strictly inside clone depth. New clones go to a NEW directory
   (`~/oss_adoption/` suggested), never mixed into `~/oss/` or `~/oss_agentic/`. Append-only
   ledgers like grow_corpus_ledger.tsv (reuse `grow_corpus.py` funnel where possible).
   Corpus-run hygiene per project memory: ≤8 workers, suspend baloo, kill process GROUPS.

### Changes in CALCULATION (estimation)

1. **Unit of analysis = repo-week panel**, not a pooled commit set. Outcomes from
   `archcheck --diff --diff-mode=memory --format=json` per commit, aggregated to weeks
   (STDOUT parsing is insufficient — clone/complexity/nccd/bool columns only exist in the
   JSON path; see `experiments/per_commit/run_worklist.py::categorize()`).
2. **Estimator: PPML (Poisson pseudo-ML) with repo and matched-pair×event-time fixed effects
   and log-exposure offsets** — NOT the NBER "normalize by pre-period mean" (breaks on our
   outcomes: ~12% of repos have a zero pre-period SATD mean, ~95% zero cycles; ratio
   normalization drops clean repos = selection on the outcome). Three exposure channels:
   weeks (volume), commits (per-unit), KLOC-added (per-unit, size-robust — squash-merge
   policy shifts make per-commit deceptive; agents ~1.3× larger commits).
3. **Pre-registered endpoints** (commit the analysis plan to the repo BEFORE mining;
   everything else secondary under Romano–Wolf stepdown):
   - **P1 (volume / "more often"):** weekly count of drift events, union of
     complexity + bool/flag + newclone + satd + added_edges; exposure = weeks.
     Expected ×1.5–1.7 (consistent with fc01ab4: commits ×1.67, lines/commit ×1.40).
   - **P2 (complexity):** `n_complexity` events — per week AND per KLOC. Per-week expected
     up; per-KLOC is the honest test of a per-unit effect.
   - **P3 (flag programming):** `n_bool_field` + `n_flag_arg` accretion rate per week and
     per KLOC; PLUS the cumulative per-struct boolean-state analysis (the method that
     produced the surviving 2.3×: git-blame per-struct, ≥4 commits into struct) run
     before-vs-after adoption within adopters and against controls. Prior: 2.3× — this is
     the endpoint most likely to show agents worse per unit.
   - **P4 (clone STOCK trajectory):** motivated by a real observation the per-commit design
     cannot see — the author's near-100%-agentic project accumulated 19 blatant clones that
     archcheck caught immediately, while merged-history per-commit counts say agents are
     clean. Measure the observable fact: whole-repo duplication level (archcheck duplication
     mode) on snapshots — monthly over [−6, +12] months around adoption — adopters vs
     controls; prediction: adopter stock grows faster. **Author's framing rule (2026-07-10):**
     drift the agent introduced COUNTS regardless of whether a human later cleaned it up —
     later cleanup is review labor (a cost imposed by agent drift), NOT a mitigation. Do not
     use introduction-vs-removal decomposition as a defense of agents; removal rates may be
     reported as an optional diagnostic only. In the ≥20%-dose stratum expect stock effects
     several times the median — the author's project is the limiting case (≈100% dose, no
     human gate).
   - **P5 (pre-gate drift — the agent's unfiltered output):** every merged-history design,
     including our central null, measures only commits that SURVIVED review. If reviewers
     catch agent drift pre-merge, agents look clean in merged history precisely because a
     human gate stood in front of them ("agent submitted a PR with a clone — what happens
     next?": either it merges and our counters see it, or review forces a rework and the
     produced drift becomes invisible to every merged-history counter). Measure drift in
     agent PRs AS SUBMITTED: `git fetch origin pull/N/head` (GitHub keeps these refs after
     merge and branch deletion), replay `archcheck --diff base..head` on the submitted
     state, and compare with matched human-authored PRs in the SAME repos. Sample: 300–500
     agent PRs + matched human PRs drawn from the #173 corpus assets (16,982 enriched PRs,
     `experiments/agentic_weak_review/`). This is the most direct test of "agents PRODUCE
     drift more often", independent of gate filtering; the as-submitted vs as-merged gap is
     the review-filtering estimate — and the exact work archcheck automates.
   - Other categories (SATD, test co-evo, graph gates): secondary; per-unit nulls
     reported as TOST equivalence with pre-declared bounds (not "p>0.05"). Cycles:
     exploratory only.
4. **Dose-response co-primary:** effect vs post-adoption AI-commit share (attribution by
   metadata only — bot author / trailer / AI committer, NEVER message prose; pipeline
   exists: `experiments/per_commit/authorship.tsv`, coverage 93.4%). Monotone dose-response
   is what turns "correlated" into "caused by agents" for a referee.
5. **Event-study plot with 12 pre-adoption weekly coefficients is a GATE, not a figure:**
   flat pre-trends in commits AND drift rates required, else the volume channel is reported
   descriptive, not causal (reverse Ashenfelter dip: maintainers adopt agents when planning
   a push). Donut specification dropping weeks [−4, +4] (CLAUDE.md is a formalization
   event; audit marker-vs-first-use lag manually on 30 adopters, widen donut if median lag
   >2 weeks).
6. **Robustness (non-negotiable, each burned us before):** leave-one-repo-out on every
   confirmatory estimate (ThemisDB lesson: one repo was 60% of agent events); bulk_skip
   (>10k-line commits, 0.46% of commits but agent-skewed) as separate outcome + sensitivity
   with capped inclusion; merge-commit share as covariate; winsorize 1/99.

### Descriptive stratum S1: pure-agent repos (separate table, author's explicit request)

The #174 "unanalyzable" verdict applies to CAUSAL within-repo designs only (no pre-agent
baseline, no human commits to contrast against, and youth alone produces drift). As a
DESCRIPTIVE profile the stratum is cheap, explicitly requested by the author ("the agent
threw a project together in a month and it died or froze — fine, just measure the drift in
its commits; a separate table is fine"), and has never actually been measured: the previous
attempt (ai-377 stratum, #146) was crippled by the #149 clone-scan timeout — 256/372 repos
blacklisted (69%), 71,839/111,224 commits skipped (65%). Coverage failure, not evidence.

- Sample: bot-author-share ≥90% / born-agentic repos (`agent_author_ranked.tsv` + screen
  ledger; the retired ai-377 list is the starting point). Dead/frozen repos are IN scope.
- Measure with the FIXED scanner: full drift profile per commit and per KLOC (all categories
  incl. complexity, bool/flag; clone stock per KLOC at HEAD). Coverage gate: ≥90% of commits
  scanned per repo, else exclude AND count the exclusion in the table.
- Context column (context, not causality): age/size-matched young greenfield repos from the
  agentic=0 pool; table caption names both caveats (greenfield selection; hidden-AI
  contamination of 2025+ "human" greenfield → bias-to-null).
- Never pooled into P1–P5 estimates (the ThemisDB lesson is precisely this stratum leaking
  into pooled numbers).

### Power (measured, not guessed — half-split σ on results_full.jsonl: edges .62, complexity .54, newclone .68, satd .84)

- Pilot (~46–60 usable pairs from existing data; unified_metrics.tsv has 100 agentic repos
  but only 46 with ≥50 pre-commits): detects only ≥×1.3–1.7 → **pilot = pipeline validation,
  σ estimation, pre-trend check. Pre-commit to that framing; a null pilot proves nothing.**
- Full (n≈400 pairs): volume ×1.2–1.3 detectable (expected ×1.5–1.7 → comfortable);
  per-unit equivalence bounds ±15–18%; flag-family per-unit prior 2.3× → detectable even at
  pilot scale if real.
- Cost measured: archcheck replay 1.61 s/commit → ~250–350k new commits ≈ 1.5–2.5 days on
  8 workers; ~600–900 new repos ≈ 40–60 GB (NVMe had ~79 GB free — plan cleanup first).

## Phases and gates

| Phase | What | Gate |
|---|---|---|
| 0 | Pre-register analysis plan (commit BEFORE mining). Read Larsen&Moghaddam 2606.13298. Pilot on EXISTING data: date-join results_full.jsonl (NO timestamps in it!) × authorship.tsv × git-log dates from local clones; event-study on ~46–60 adopters vs both control types; pseudo-adoption placebo | pre-trends flat; σ ≤ half-split estimates; placebo ≈ 0 (else year-shift design dead → fall back to contemporaneous controls + bias-to-null argument) |
| 1 | Mine adoption events across 7,510 agentic=1 repos (full-history marker scan; Haiku subagents fine for the mechanical scan/verify loop); manual lag audit on 30 | ≥250 eligible; median marker lag ≤2 wk; ≥20%-dose stratum ≥150 repos |
| 2 | Control pool: matching from 21,596 agentic=0; both alignments; Dockerfile + pseudo-adoption placebo assignments | — |
| 3 | Clone + replay (~600–900 repos, ledgered, hygiene checks) + P4 duplication-mode snapshots (monthly, [−6,+12] months around event, adopters AND controls; vendor-filter mandatory before counting) + P5 as-submitted replay (fetch pull/N/head for 300–500 agent PRs + matched human PRs from #173 assets) + S1 pure-agent stratum remeasure (fixed scanner) | parentless rate sane; per-repo failure <5%; bulk_skip rate sane; snapshot clone counts eye-checked on 10 repos before aggregating; S1 coverage ≥90% commits/repo; P5 head-refs fetchable for ≥80% of sample |
| 4 | PPML DiD + event-study + dose-response + placebos + LOO + Romano–Wolf + TOST | placebos ≈ 0; LOO-stable; else demote to exploratory |
| 5 | Write-up: docs/research/adoption_event_study.md + JOURNEY milestone + 3–5 case studies (one repo, before/after figure — reviewers love these); robustness annexes | — |

~5–7 weeks total. Phase 0 requires zero new clones.

## Assets to reuse (exact paths)

- `experiments/per_commit/results_full.jsonl` — 517k per-commit outcomes (columns incl.
  n_complexity, n_bool_field, n_flag_arg, n_newclone, n_satd, nccd_delta, added_total,
  bulk_skip). No dates — needs the join.
- `experiments/per_commit/authorship.tsv` — per-commit AI attribution (dose).
- `experiments/per_commit/run_worklist.py` — the JSON replay runner (the load-bearing path).
- `experiments/ai_repo_run/screen_ledger.tsv` (29,106 pool), `grow_corpus.py`,
  `screen_corpus.py`, `commit_count.py`, `agent_author_scan.py` (marker regexes),
  `unified_metrics.tsv` (pilot adopters + dates).
- `docs/research/central_finding.md` — §13 = the bar this study must clear; §12 = hygiene
  traps; §6 = velocity reconciliation the volume channel must reproduce (sanity anchor:
  commits-after/before ≈ ×1.67 — a mismatch is a pipeline bug, not a finding).
- `docs/research/boolean_state_agentic_vs_not.md`, `boolean_state_drift_eyecheck.md` — the
  per-struct 2.3× method to extend (P3).
- `docs/research/ai_code_detection_landscape.md` — detection-method reference.
- `analysis/CORPUS_CRITERIA.md` (journal repo) — gates for any NEW repo admitted.

## Top publishability risks (pre-empted in-design)

1. "Your treatment isn't treatment" (dilution + fuzzy dating) → dose-response co-primary,
   ≥20% stratum, donut, audited marker lag.
2. "Year-earlier controls measure the calendar, not agents" (secular drift +13%/−23%
   measured!) → dual control alignments bracket the truth + pseudo-adoption placebo
   published explicitly.
3. "Forking paths on a partly-null result" → pre-registration committed before mining,
   5 primary endpoints (volume / complexity / flags / clone stock / pre-gate), TOST
   bounds declared in advance, Romano–Wolf for all secondaries, pilot pre-framed as
   validation.

## Progress log

**2026-07-10 — Phase 0 executed on existing data (zero new clones).** Assets in
`experiments/adoption_event/`:
- `build_dates.py` → `dates.tsv`: dated all 520,177 shas of `results_full.jsonl` via
  `git log --all` (0 repo errors). Verified `results_full` has FULL-history coverage per repo
  (not truncated) — e.g. MatrixOS 1,106 ok commits spanning 2024-07..2026-04 = all-refs range.
- `mine_markers.py` → `adoption_markers.tsv`: 4 markers on all 1,200 clones →
  **828 adopters, 372 marker-free**, span 2025-02..2026-06, median 2026-02.
  *Correctness fix:* the reused screen_corpus BOT/TRAILER regexes false-positive on human
  names ("Co-authored-by: Devin Gibson") and faked 2024 adoptions — tightened to unambiguous
  bot/email forms (precision > recall for DATING; a false-early date poisons the pre-period).
  Marker↔unified dating lag |median| = **1 day** (methods agree).
- `build_panel.py` → `panel_weekly.tsv`: 49,998 repo-week cells; `n_bool_field` joined from
  the separate `bool_field_added.jsonl` (task text wrongly claimed it's a `results_full` column).
- `build_cohorts.py` → `cohorts.tsv`: **212 eligible adopters, 260 pseudo-adoption controls**
  (owner-deduped, ≥50 pre-commits, ≥12wk each side). Far above the plan's expected ~46–60 —
  full-history marker dating on the whole 1,200-repo corpus, not just unified's 100.
- `pilot_eventstudy.py`: pooled post/pre rate ratios (±12wk), owner-clustered bootstrap CIs,
  pseudo-placebo, LOO, donut, pre-trend. **Results table in
  `docs/research/adoption_event_study_prereg.md` §Phase-0 pilot.**

**Pilot verdict (validation, not a hypothesis test — power only ≥×1.3–1.7):**
- Volume channel reproduced: **P1 per-week 1.34 [1.04,1.67], per-commit 0.99** → more drift
  per week via more commits, ~flat per commit (the established null, now as a rate effect).
- **Placebo ≈ 1** (0.94–1.11) across endpoints → window machinery clean, year-shift fallback
  not forced.
- **Pre-trend gate already bites P2 complexity** (largest raw jump: 1.76/wk, 1.30/commit, but
  NON-flat pre-trend) → descriptive-only until donut/FE clean it; likely why the raw complexity
  signal died under matching before.
- **P3 flag:** 1.50/wk [1.05,2.09], flat pre-trend, placebo ~1 → cleanest causal candidate;
  per-commit modest (1.10).
- **P4 introduction proxy flat per commit (0.97)** → supports the removal-collapse hypothesis;
  stock/consolidation needs Phase-3 duplication snapshots (not computable from per-commit data).
- Volume anchor reconciled: ×1.67 (central_finding §6) is total-history-halves on `unified`'s
  lighter commit set; full-coverage per-week surge is median 1.35 / mean 2.12 / 45% ≥1.5×.
  Same direction & order — the ×1.67 is a guide, not a hard gate on a different commit set.

**Open before Phase 1:** (1) pre-registration doc must be COMMITTED before mining starts
(user's call — never commit without command); (2) the 11 unmapped unified adopters (z3, CCF,
daScript, erhe…) have clones but are absent from `results_full` — decide whether to windowed-
replay them (Phase-0.5) or defer to Phase 1's full mine; (3) P4 stock is a Phase-3 new-run
item (duplication-mode monthly snapshots, [−6,+12] months); (4) **2026-07-10 author revision:
endpoints revised — P4 reframed (stock as fact, cleanup ≠ mitigation, consolidation demoted
to optional diagnostic), P5 pre-gate drift added, S1 pure-agent descriptive stratum added.
SYNC `docs/research/adoption_event_study_prereg.md` with these BEFORE committing it** — the
prereg must register the endpoints as revised, or the revision looks post-hoc.

**2026-07-10 (cont.) — prereg SYNCED with the revision.** `adoption_event_study_prereg.md` now
registers 5 primary endpoints: P4 reframed to clone-STOCK-as-fact (cleanup = imposed review
labor, not mitigation; consolidation demoted to optional diagnostic; no intro-minus-removal
ledger); P5 pre-gate drift added (per-PR, agent-as-submitted vs matched human, `pull/N/head`
replay, Phase 3); S1 pure-agent descriptive stratum added (never pooled). Verified P5 assets
exist (`experiments/agentic_weak_review/`: 485 categorized agent-PR candidates + 16,664
enriched) but they carry `pr_url`/author, NOT base/head SHAs → as-submitted replay needs
network `pull/N/head` fetches = Phase 3, no Phase-0 slice. S1 asset `agent_author_ranked.tsv`
present. Prereg is ready to commit; nothing else in Phase 0 is computable from existing data
(P4 stock, P5 pre-gate, S1 remeasure are all new-run items). **Doc still awaits the commit
command — must land BEFORE any Phase-1 mining.**

## Acceptance criteria

- [ ] Pre-registration doc committed before Phase 1 mining starts.
      *(drafted + SYNCED with the 2026-07-10 endpoint revision, P1–P5 + S1 registered;
      `docs/research/adoption_event_study_prereg.md` — awaiting commit command)*
- [x] Phase-0 pilot report: pre-trend, σ (half-split 1.18/1.47), pseudo-adoption placebo (≈1).
- [ ] ≥250 dated adoption events with ledgered provenance; 30-repo manual lag audit.
- [ ] Confirmatory estimates for P1/P2/P3/P4/P5 with LOO, both control alignments, both
  placebos.
- [ ] S1 descriptive table: pure-agent stratum drift profile, ≥90% per-repo commit coverage
  (redoing the #149-crippled ai-377 run), matched young-greenfield context column, caveats
  in caption, kept out of all pooled estimates.
- [ ] The headline sentence is supportable verbatim: "after agent adoption, repos accumulate
  architectural drift events ×K faster (K with CI); flag-programming accretion rises ×M even
  per unit of code; clone stock grows ×S faster; agent PRs as submitted carry ×G more drift
  than matched human PRs" — or the honest negative is recorded in JOURNEY and the task
  closes as refuted (author decides framing; per project rules, blocked/refuted ≠ completed).
- [ ] Case-study candidates for Phase 5 include the author's own near-100%-agentic project
  ("leadline", the 19-clones observation) as the limiting-case before/after example —
  confirm with the author before publishing anything from it.
