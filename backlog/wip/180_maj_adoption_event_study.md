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
     **Author's framing rule #2 (2026-07-11, amendment before Phase 3):** the harm claim is
     the ABSOLUTE per-week accumulation, not the per-unit rate. Maintainers and reviewers
     carry the total complexity of the repo; their budget is measured in person-weeks, not
     KLOC — dividing by code volume does not reduce the burden, it answers a different
     question ("is agent code worse line-by-line?") that we do not claim. Volume denominators
     can HIDE degradation (Larsen & Moghaddam's −6.7% smell density = flat raw counts / LOC
     +12.8%; Agarwal et al. 2601.13597 conversely headline un-normalized +35% complexity on
     +77% lines). Reporting hierarchy: per-week rate is the finding; per-commit/per-KLOC is
     mechanism attribution (shows the channel is volume), reported alongside but NEVER as a
     mitigation of the per-week rise. Same logic as the P4 cleanup rule. This also demotes
     the per-KLOC puzzle (pilot 1.44–1.92) from blocker to diagnostic — resolve it in Phase 4
     before citing any per-KLOC number, but no per-KLOC result gates the headline.
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
| 1b | Join the 3 external repo lists (Zenodo 19375881 / agentic-coding-impact / tech-debt-ai-coding — see "External literature & new assets") with a C++ language filter; dedupe vs our funnel; same eligibility gates; optionally widen marker set (census + Robbes regexes) | yield per source ledgered; any NEW marker passes the 30-hit lag audit before its adopters enter the panel |
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

## External literature & new assets (added 2026-07-11 — actionable)

Full annotated survey: `docs/research/agentic_drift_literature.md` (4 parallel web sweeps,
~50 searches, key URLs verified live 2026-07-11). READ IT FIRST. This section extracts only
what changes the execution of THIS task.

### New repo sources — mine these for additional C++ adopters (Phase 1b, cheap)

Three published repo lists now exist. None has a C++ breakdown — the language join is ours
to do. For each list: download → extract repo full names → language-filter (gh api
`repos/{owner}/{repo}/languages`, keep C++/C majority; Haiku subagents fine for the
mechanical loop) → dedupe against `screen_ledger.tsv` + `phase1_eligible.tsv` → apply OUR
eligibility gates unchanged (≥50 commits & ≥12 wk pre-event, ≥12 wk post, owner-dedup, not
a fork, CORPUS_CRITERIA.md). Record yield per source in the ledger (expect low single-digit
% C++ — a small n gain, but these repos are externally validated agent adopters, which
blunts the "your detector is homemade" review).

1. **Zenodo 19375881** (AIware 2026 config dataset, arXiv:2605.08435): 4,741 repos with
   agent configs (Claude Code, Gemini CLI, Codex CLI, Copilot CLI, Cursor), cut 2026-04,
   CC-BY, 1.4 GB. `repos.csv` has full names. Config presence = adoption signal we already
   use; here pre-harvested and fresher than our funnel.
2. **github.com/shyamagarwal13/agentic-coding-impact** (Agarwal/He/Vasilescu, MSR 2026,
   arXiv:2601.13597): 401 agent-first + 606 control + 117 IDE-first repo lists. Their
   adoption dates may be reusable — VERIFY their date definition against ours on 10 repos
   before trusting (our gate: marker lag ≤2 wk).
3. **github.com/yueyueL/tech-debt-ai-coding** (Liu/Lo "Debt Behind the AI Boom",
   arXiv:2603.28592): 6,299 repos, 302.6k AI-attributed commits (99% validated precision).
   Corpus is Python/JS/TS-focused but their case list includes librealsense (C++) — scan
   the full repo list for C++ before dismissing.

### Marker-set widening (optional Phase-1 re-mine — decide by cost)

Our 4 markers (CLAUDE.md / copilot-swe-agent / codex branches / Co-Authored-By) miss agents
the census catches. Sources for additional validated regexes:

- **arXiv:2606.24429** (Khosravani & Mockus, 180M-repo census): Type B message signatures
  (`Generated by Replit`, `Replit-Commit-Author:`, `Generated by Codex`, `codex-cli`;
  90.5% precision), Type C author-name suffix `(aider)` (86.8%), Type D config files
  `.windsurfrules`, `copilot-instructions.md`, `.replit`, `.aider.conf.yml`, `.claudeignore`,
  `.cursor/` (92%). Regexes are in the arXiv HTML; Zenodo replication package promised but
  not yet posted. Killer stat for the write-up: bot-account lookup alone undercounts Claude
  Code ~30× — cite it to justify our multi-signal design.
- **arXiv:2601.18341 + 2601.18345** (Robbes et al., MSR 2026 Distinguished Paper): 150
  heuristics for ~50 agents (86 file, 20 branch, 44 label). **Read the "Perils" paper
  (2601.18345, 8 documented pitfalls) BEFORE adding any heuristic** — it is the field's
  errata list for exactly this kind of mining.
- Gate unchanged: every NEW marker must pass the 30-hit manual lag audit (median ≤2 wk)
  before its adopters enter the panel; markers that fail stay in a diagnostic column.

### External anchors & must-engage papers (Phase 4 interpretation + Phase 5 write-up)

- **P1 volume anchors:** NBER 35275 (+36%/+109% commits); Agarwal 2601.13597 (+36% commits,
  +77% lines). Their headline "+35% cognitive complexity" is UN-normalized repo-month totals
  on +77% lines — i.e. our volume channel presented without decomposition; their only
  normalized quality metric (duplication density) is +7.9% n.s. = our per-commit clone null;
  their complexity pre-trends are non-flat and acknowledged. Use as external replication of
  the volume story, per framing rule #2 (report numerator AND denominator, headline = per-week).
- **P4 stock anchors:** GitClear×GitKraken 2026 (623M lines: duplication +81%, moved lines
  −70%, functional connectivity −35%); Horikawa arXiv:2511.04824 (agent refactoring = renames,
  rarely duplication/dependency work — the mechanism behind stock growth); HackerNoon 49
  vibe-coded repos (jscpd 7.98% avg vs 3–5% benchmark, raw data published).
- **Must engage honestly (they cut against us):** Borg et al. arXiv:2507.00788 (EMSE 2026,
  controlled 151-dev experiment, NO downstream maintainability penalty — the strongest null);
  Mao arXiv:2603.27130 (AI code has LOWER cross-file duplication 17.2% vs 24.5%; only paper
  with C++ content, 7.8% of files). Both agree with our own per-commit nulls — position them
  as consistent with, not contradicting, the volume+stock+pre-gate story (P1/P4/P5), which
  none of them measures.
- **SmellBench arXiv:2605.07001:** 63.1% of architecture-smell detector output is FP — use
  when contrasting archcheck's `file:line` evidence-based checks with ARCAN-style detectors.

### Confirmed-empty niches (the moat — claim explicitly in the write-up)

Verified empty across academic + grey sweeps (2026-07-11): (1) C++ × agents × architecture —
nothing; no study touches include graphs, physical design, levelization, cycles; (2)
boolean-flag / flag-argument proliferation — unmeasured anywhere; P3 is a novel claim, cite
the emptiness; (3) dependency cycles in AI-adoption designs — nobody measures them. Closest
competitor remains Larsen & Moghaddam (Java/ARCAN, 0 citations yet).

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

## Current status (2026-07-11)

**LATEST — Phase-1 mining DONE + archcheck cost-wall fixed.**
- After the expansion cost wall (below), the author chose "fix archcheck first" → **`#181`
  shipped** (`fe3cdb4`): `--diff` clone scan focuses on changed files + dedupes the parent scan,
  CUBRID 84 s→~42 s (2×), identical clone set, all gates green. (See `backlog/wip/181`.)
- Then the author chose "майнинг" → **Phase-1 adoption mining executed**:
  - **376 owner-deduped eligible adopters** (403 pre-dedup) with ledgered provenance
    (`phase1_eligible.tsv`): corpus 209 (replayed, in panel) + local-agentic 62 + newly-mined 105.
    **Phase-1 gate ≥250 exceeded; ≈ the n≈400 full-study target.** Span 2025-02..2026-05,
    median 2025-12, clean (no pre-2025 outliers).
  - Method: eligibility (marker + **≥50 TOTAL pre-commits** + ≥12 wk each side) is git-log-only,
    so new repos are **bare `--filter=blob:none`** clones (MB not GB), mined, then **deleted**
    (`clone_and_mine.py`, delete-after → disk stays flat; eligible repos get full-cloned in
    Phase 3). Yield **12.7%** eligible/processed on a 1,000-repo batch of the 6,368 uncloned
    agentic repos; ~5,368 remain if more power is wanted.
  - **Two correctness fixes caught by self-check mid-run** (both re-run clean): (1) eligibility
    used a 12-week *window* pre-commit count, not TOTAL pre-history — caught by re-cloning
    `mcci-catena/bootloader` (261 commits, dormant right before adoption); (2) a bare `🤖` in the
    trailer regex faked a 2022 adoption (`microsoft/mu_plus`) — dependabot/CI emoji noise;
    tightened to `🤖 generated` (Claude Code's real trailer still caught by "generated…claude").
    Re-mined all sources; corpus 214→212, mined 127→126.
  - **Disk economy (author ask):** blobless + delete-after + freed **3.37 GB** of stale build
    dirs (kept `build/debug`, `_deps`).
  - Dose stratum (Phase-1 gate ≥150 at ≥20% AI-share): corpus has **57 ≥0.20** (of 191
    computable; median 0.06, p75 0.27). Local+mined dose not yet computed (they're from the
    agentic pool → likely higher) — **follow-up** (needs authorship on those repos).

**NEXT:** (a) compute dose for local+mined → confirm ≥150-dose stratum; (b) Phase-3 full-clone +
replay the 167 not-yet-replayed eligible (local 62 + mined 105) — now cheaper post-`#181`;
(c) `#181` index-level focus for a further replay speedup; (d) more mining batches only if n>400
power is wanted.

---

**DONE (earlier)**
- Phase 0 fully executed and **committed+pushed** (`06dfc25`): pre-registration doc
  (`docs/research/adoption_event_study_prereg.md`, 5 endpoints P1–P5 + S1 stratum, synced with
  the author revision) + pilot validation (212 adopters, 260 placebo controls). Gate satisfied:
  pre-trends checked, placebo ≈1, σ half-split 1.18/1.47, volume channel reproduced.
- Pilot verdict: P1 volume ×1.34/wk at ×0.99 per-commit; P3 flag ×1.50/wk (clean pre-trend,
  best causal candidate); P2 complexity biggest raw jump but pre-trend NOT flat → descriptive;
  P4-intro flat per commit (×0.97). Details: prereg §Phase-0 pilot.
- Phase-1 expansion prep: inventoried the 7,510-repo agentic pool → found **408 agentic repos
  already cloned but not replayed**; marker-mined them → **138 eligible-if-replayed**; built
  windowed worklist (106 repos ≤1200 win-commits, 57,281 commits, window [event−16wk,+12wk]);
  generalized panel+cohort scripts to two data sources. All scripts in
  `experiments/adoption_event/` (gitignored per project convention).

**STOPPED — expansion hit a real COST WALL (honest null on the shortcut).** The 106-repo
replay-only expansion was meant to grow 212→~300 cheaply. It doesn't: the expansion repos are
large C++ projects (median 3,581 tracked files) whose per-commit `--diff` replay costs 15–85 s
(clone/duplication scan on <40 MB trees; include-graph rebuild on >10 k-file trees) — the full
57 k-commit run projected to ~20 h. Two mitigation attempts failed cleanly:
- timeout 180→60 s: rate stayed ~0.65/s (medium repos scan 30–60 s without timing out).
- disable clone scan via a parent `~/oss/.archcheck.yml`: **the strict config validator
  (`version`+non-empty `modules`+`rules` required) rejected every minimal config → exit 2**, and
  I initially MISREAD the exit-2 fast-exit ("0.00 s") as a real 84 s→0 s speedup. Caught on the
  honest re-check (exit code, not just wall-time). Config route abandoned (also biases n_newclone).
- Partial results merged: only **+2 eligible adopters landed (214 total)** — CUBRID (84 s/commit),
  lightning, Alchemy, Antares burned run-time and failed windowed eligibility. Orphaned archcheck
  process groups from the killed runs were cleaned (killpg), baloo resumed.

**Conclusion:** growing n is NOT cheap from already-cloned repos — per-commit clone-scan replay of
large C++ repos is the bottleneck. Phase 0 stands validated at n≈212. This is a resource/scope
decision for the author, not an autonomous grind.

**FORK (needs author steer)**
1. **Full Phase-1 mine** — clone + windowed-replay the 6,369 not-yet-cloned agentic repos toward
   n≈400. Cost: 40–60 GB disk (only 79 GB free — cleanup first), ~1.5–2.5 days on 8 workers.
   Same clone-scan cost wall applies → likely need a per-commit clone-scan cap or a smaller
   fast-repo stratum. Big, deliberate run.
2. **Skip the n-growth**, treat Phase 0 (n≈212) as the validated pilot, and move to the Phase-3
   NEW-endpoint runs that are the actual scientific contribution: P4 duplication-stock snapshots,
   P5 as-submitted PR replay (`pull/N/head`), S1 pure-agent stratum. These also need runs but are
   where the novel signal is.
3. **Product angle:** the cost wall itself is a finding — per-commit whole-repo clone scan is too
   slow for large C++ repos at corpus scale; relevant to archcheck's own `--diff` performance on
   big repos (a real user would hit the same 84 s). Worth a DEBT note.

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

**2026-07-10 (cont.) — Phase-1 expansion IN FLIGHT (no new clones).** Inventory of the 7,510
agentic=1 pool: 1,141 already cloned in `~/oss`, of which 733 are in the panel and **408 are
cloned-but-unreplayed**. Marker-mined those 408 (`mine_local_agentic.py` →
`local_agentic_markers.tsv`): 241 have a marker, **138 eligible-if-replayed** (≥50 pre-commits,
≥12wk each side on full history). Capped at ≤1,200 windowed commits (drop 32 mega-repos —
#149 domination trap, logged) → **106 repos, 57,281 commits** windowed to [event−16wk,+12wk]
(`build_worklist_expansion.py` → `worklist_expansion.tsv`; window = `git log HEAD`, matching
the original all-commits-on-branch traversal, verified rf⊆HEAD not first-parent). Replaying via
the load-bearing `run_worklist.py` → `results_expansion.jsonl` (8 workers, baloo suspended,
`n_bool_field` inline post-#090). Downstream generalized to 2 sources: `build_panel.py`
(reads `results_expansion.jsonl` + `dates_expansion.tsv`), `build_cohorts.py` (merges
`local_agentic_markers.tsv`). **Caveat found mid-run:** a few large-TREE C++ repos (Alchemy,
Antares, 3drepobouncer) hit the 180s duplication-scan timeout → blacklisted → their commits
lost; the cap was on commit-count, the timeout is LOC-bound. Acceptable (gate has margin:
even −20 repos → ~297 ≥ 250); dropped repos will be logged, timeout handling revisited in
Phase 3. Expansion target after merge: 212 → ~300 eligible adopters, still zero new clones.

## Acceptance criteria

- [x] Pre-registration doc committed before Phase 1 mining (`06dfc25`, P1–P5 + S1 registered).
- [x] Phase-0 pilot report: pre-trend, σ (half-split 1.18/1.47), pseudo-adoption placebo (≈1).
- [x] ≥250 dated adoption events with ledgered provenance — **376 owner-deduped**
      (`experiments/adoption_event/phase1_eligible.tsv`); marker↔hand-date |lag| median 1 day
      (30-repo audit on corpus). Dose stratum ≥150 still to confirm (local+mined dose pending).
- [~] Confirmatory estimates **P1/P2/P3/P4 DONE on the expanded panel (n=361/265), LOO +
  placebo + donut + pre-trend** — write-up `docs/research/adoption_event_study.md` (2026-07-12).
  P3 per-struct re-run on cohorts still outstanding.
- [x] **P5 as-submitted PR drift DONE (2026-07-12)** — `docs/research/p5_as_submitted_drift.md`.
  291 agent + 260 human eligible PRs across 18 both-arm repos; G1 head-ref fetch 99.8% (PASS,
  ≥80%). Within-repo median contrast (agent − human) fraction-with-any-drift **−0.21
  [−0.365, +0.000]**, LOO [−0.235, −0.185]: **H-P5 not supported** — agent PRs not architecturally
  dirtier as submitted than human PRs in the same repo; raw pooled agent-excess is repo
  composition (Simpson, as #115 under repo FE), sign not robust to size → no robust difference.
  **Review-filter gap null by construction** (`pull/N/head` = final head ≈ merged; first-push-vs-
  merged test unrun). Giant monorepos tt-metal/onnx-light dropped to per-PR scan timeout.
- [ ] S1 descriptive table: pure-agent stratum drift profile, ≥90% per-repo commit coverage
  (redoing the #149-crippled ai-377 run), matched young-greenfield context column, caveats
  in caption, kept out of all pooled estimates. **NOT run.**
- [x] Headline settled as the **honest volume-channel** version: "after agent adoption, repos
  accumulate architectural drift events ×1.32/wk [1.08,1.61], driven by volume (per-commit ≈1);
  per-unit quality flat-to-contested (P2 per-KLOC contested; P3 flag CI crosses 1; P4 clone
  **stock** null in the Agarwal replication)." The per-unit "agents worse" claims did NOT
  survive expansion/replication — recorded in JOURNEY + write-up. Volume claim supports the
  archcheck pitch; per-unit claims are honest negatives, consistent with prior nulls.
- [ ] Case-study candidates for Phase 5 include the author's own near-100%-agentic project
  ("leadline", the 19-clones observation) as the limiting-case before/after example —
  confirm with the author before publishing anything from it.
