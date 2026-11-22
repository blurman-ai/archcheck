# Why per-commit drift measurements look null while intuition sees drift

**Date:** 2026-11-22 · **Context:** Reconciliation of `paper/versions/paper_v2.tex`, the November 2026
re-run (`experiments/rerun_2026-11/`), earlier research documents (`docs/research/`), and codebase
audits (`backlog/new/202_*`, `backlog/new/203_*`, `backlog/DEBT.md`).

> **One-paragraph answer.** Intuition is right: architectural drift under AI-assisted development is
> real, and several parts of our own empirical record already detect it. The apparent "no per-unit
> drift" conclusion (`RQ1 = null`) is produced by the interaction of four factors: (1) we *do*
> observe drift in the November re-run and cumulative per-struct analyses, (2) the per-commit `RQ1`
> design conditions on commit size (an intermediate outcome of agent bloat), measures only
> merged/post-review commits, and suffers from severe control-arm contamination and task-mix
> confounding, (3) the corpus replay used only `archcheck --diff` (1 of 6 measurement surfaces,
> omitting the `DRIFT.1`–`DRIFT.4` architectural rules) while censoring the heaviest commits via
> `mem_cap` and timeouts and drowning `P1` in rename-induced edge noise, and (4) the corpus selection
> filter excludes born-agentic projects in favor of established C++ repositories.

---

## 1. Drift is already present in our own data

The data does not say "nothing drifts." It says drift appears as soon as we step outside the
size-matched single-commit indicator test:

1. **Post-adoption drift flow (`RQ2`, November 2026 re-run, 854 adopters / 517 placebo):**
   In [`paper_data/t4_event_study_rerun2026-11.tsv`](../../paper_data/t4_event_study_rerun2026-11.tsv),
   the weekly rate of `P1` union drift rises **1.93×** under the calendar-week fixed-effects PPML
   estimator (`M3`, 95% CI `[1.42, 2.62]`). Using the raw adopter-to-placebo ratio of post/pre rate
   ratios, `P1` rises **2.34× per week** (`[1.44, 3.95]`, $p < 0.001$) and **1.69× per commit**
   (`[1.03, 2.88]`).
2. **Cognitive complexity (`P2`) rises even per unit of change:**
   In the November re-run, `P2` rises **1.79× per week** (`[1.53, 2.09]`), **1.17× per commit** in
   `M3` (`[1.00, 1.37]`; pooled adopter/placebo ratio **1.50×** `[1.07, 2.08]`), and **1.14× per
   KLOC** (`[1.02, 1.27]`). As noted in
   [`REVIEWER_COVERAGE.md`](../../experiments/rerun_2026-11/estimates/REVIEWER_COVERAGE.md), the
   earlier claim that "per-commit and per-KLOC rates stay close to baseline" no longer holds for
   complexity.
3. **Cumulative boolean-state accretion (`per-struct drift`, 2.3×):**
   When state accretion is measured cumulatively across history (`git blame` on structs receiving
   `bool` fields across $\ge 4$ distinct commits,
   [`boolean_state_agentic_vs_not.md`](boolean_state_agentic_vs_not.md)), agentic repositories drift
   at **9.8% vs 6.0% of structs (1.6×)** and **36% vs 17% of repositories (2.1×)** at nearly equal
   commit activity (`1.08×`). After manual removal of false positives and config bags, verified drift
   is **5.6% vs 2.4% (~2.3×)**. The per-commit weekly proxy (`P3`) misses this because state
   degradation is a multi-commit accumulation process.
4. **Review capacity collapse (`RQ3`):**
   Adopters merge **1.38×** more pull requests (`[1.21, 1.58]`) and **1.47×** larger pull requests
   (median added lines 43 → 60), while human review comments per pull request fall to **0.73×**
   (`[0.52, 0.99]`) of the placebo trajectory. Even where per-line defect density is similar, more
   structural delta enters `main` per unit of human inspection.

---

## 2. Methodological traps that force `RQ1` toward the null

1. **Conditioning on a collider / mediator (commit-size matching):**
   Across the corpus, AI-attributed commits are **1.26× larger** within-repo (50 vs 21 median LOC).
   In practice, an agent often solves a 20-line problem in 50 lines by adding defensive branches,
   wrappers, or near-clones. When `RQ1` matches an agent commit of 50 lines against a human commit
   of 50 lines, it compares a bloated agent implementation of a small task against a human
   implementation of a genuinely larger task. Controlling for LOC removes the primary channel
   through which agent bloat manifests.
2. **Survivorship through the review gate (`main` and `pull/N/head`):**
   `RQ1` and `RQ2` analyze only commits that survived into the default branch. The `P5`
   "as-submitted" study ([`p5_as_submitted_drift.md`](p5_as_submitted_drift.md)) also sampled only
   **merged** pull requests and evaluated `pull/N/head`, which is the **final** branch head *after*
   review revisions (`submitted - merged == 0` on 284 of 286 PRs by construction). Rejected PRs and
   initial pre-review agent pushes (`first-push` vs `merged`) were never measured.
3. **Task-mix confounding and control-arm contamination:**
   In `RQ1`, "human" means "no explicit metadata marker." A 50-repository audit
   ([`camera_ready_changes.md` A6](../../paper/camera_ready_changes.md)) showed that **79%** of
   post-adoption unmarked commits are authored by developers who use AI markers on other commits in
   the same repository, and in 43/50 repositories AI markers appear only as trailers on human
   commits. Manual inspection showed developers leave trailers on substantive C++ feature commits
   and omit them on build, config, and chore commits (or use unmarked IDE completion). With
   plausible misclassification ($a = 0.20, h = 0.30$), a true $2.0\times$ agent penalty attenuates
   to an observed $1.38\times$.
4. **Binary per-commit quantization ($D(c,k) \in \{0,1\}$):**
   Scoring whether a commit introduces $\ge 1$ finding equates a commit adding 1 finding with a
   commit adding 25 findings, and hides slow sub-threshold ratcheting (e.g., fan-in growing from 30
   to 49 over twenty commits).

---

## 3. Blind spots and bugs in `archcheck` and the replay pipeline

1. **Five of six measurement surfaces were never run on the panel ([#203](../../backlog/new/203_crt_full_drift_measurement_spec.md)):**
   The 484,500-commit census and the adoption panel ran only `archcheck --diff`. Surface 3
   (`--drift-baseline`), which evaluates **`DRIFT.1` (shortcut edge across layers), `DRIFT.2`
   (bidirectional module coupling), `DRIFT.3` (lateral module dependency), and `DRIFT.4.SDP`**, was
   never executed on the `RQ1`/`RQ2` cohorts. On an earlier 33-PR sample, `DRIFT.1` fired on **21%
   of AI PRs** (7/33) with zero false alarms on 26 clean PRs.
2. **Selective censoring of the most complex commits (`mem_cap` and timeouts):**
   In the November re-run, **20,480 sampled commits (9.7% overall; 14.5% pre / 13.6% post in
   adopters)** were dropped because `archcheck` exceeded the per-worker memory cap, and in `RQ1`
   **34,372 commits** were skipped due to slow-repo budgets or per-commit timeouts (plus giant
   monorepos `tt-metal` and `onnx-light` in `P5`). Memory and time blowups occur precisely on dense
   include graphs and large clone-scan trees — censoring the right tail where structural drift is
   heaviest.
3. **Confirmed analyzer bugs that mute or drown the drift signal
   ([#202](../../backlog/new/202_crt_architecture_integrity_hardening.md),
   [`DEBT.md`](../../backlog/DEBT.md)):**
   - **`GATE-1` (equal-size cycle replacement missed):** `grownSccs` in `src/graph/diff.cpp` matches
     SCCs by maximum node overlap and reports only when `baselineSize < currentSize`. If cycle
     `{a.h, b.h}` becomes `{a.h, c.h}`, size stays 2 and no cycle drift is reported.
   - **`GATE-2` (bulk commits skip graph gating):** When added lines exceed 10,000,
     `collectDiffAdvisories` returns early without building `SourceSnapshot`, causing
     `runDiffFullPath` to skip the include-graph build and return `gate: ok` even if a new cycle or
     god-header was created.
   - **Pure file moves flood `P1` with thousands of fake `added_edges` (`DEBT.md` 2026-11-06):**
     `addedEdges` compares raw file paths without mapping through `collectRenamedPaths`. Just 75
     pure-rename commits generated **19,500 spurious `added_edges`** (and `added_edges` accounts for
     53% of all `P1` events), swamping genuine architectural coupling with directory-move noise.
   - **Failed `git diff` silently outputs zero findings (`DEBT.md` 2026-11-03):**
     `collectAddedLines` / `collectNumstat` in `src/git/diff_query.cpp` return `{}` when `git diff`
     exits non-zero (e.g., a missing non-C++ blob in a partial clone), silently turning drift
     commits into clean `0`-violation records (36/1,087 commits in `pilot2`).

---

## 4. Corpus selection biases (C++ OSS)

- **Exclusion of born-agentic repositories:** Requiring $\ge 300$ commits since May 2025, $\ge 50$
  pre-adoption commits, and $\ge 12$ weeks of pre-adoption history filters out greenfield
  "vibe-coded" / born-agentic projects where intuition and external benchmarks (SlopCodeBench,
  Constraint Decay) see the sharpest structural collapse.
- **High threshold inertia in mature C++:** In established C++ codebases, crossing a fan-in of 50
  (`god_headers`) or closing a new file-level include cycle happens in $0.05\%–0.21\%$ of commits.
  In `RQ1`, 263 to 306 of 344 repositories are tied at zero on these categories, giving the sign
  test power to detect an effect only if agents are worse in $\ge 70\%–78\%$ of non-tied
  repositories.

---

## 5. High-leverage next steps

Tracked in backlog task [`#211`](../../backlog/new/211_maj_high_leverage_drift_signal_recovery.md):

1. **Run `--drift-baseline` (`DRIFT.1`–`DRIFT.4`) and cumulative `per-struct boolean drift`** across
   the adoption cohorts (`pre` vs `post` window edges, $\pm 12$ weeks).
2. **Test the unattenuated agent signal (`S1` pure-agent stratum + un-size-matched within-author
    contrast):** evaluate repositories with $\ge 90\%$ AI-attributed commits and report the
   per-task/per-commit effect without conditioning away LOC bloat.
3. **Fix `added_edges` rename mapping and `GATE-1` SCC identity in `archcheck`** so graph drift
   measures real structural new dependencies rather than directory reorganizations.
