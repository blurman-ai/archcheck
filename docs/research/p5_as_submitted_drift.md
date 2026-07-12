# P5 — as-submitted PR drift (task #180, endpoint P5)

**Status:** run 2026-07-12. **Bottom line up front:** within the same repository, agent-authored
PRs are **not** architecturally dirtier *as submitted* than human-authored PRs — the
pre-registered within-repo contrast lands slightly on the "agent cleaner" side with the CI
touching zero, so H-P5 ("agent PRs as submitted carry more drift") is **not supported**. The raw
pooled numbers look agent-dirtier, but that is a repo-composition artifact (agent PRs concentrate
in a few high-drift firmware/library repos) that reverses once you condition on repo — the same
thing that killed the #115 velocity signal under repo fixed effects. The registered **review-filter
gap** (submitted − merged) came out null **by construction** and does not estimate what it was
meant to; the sharp version of that test remains unrun.

## What P5 is and why it matters

Every other estimate in #180 — including the central null — is measured on *merged* history, i.e.
only commits that survived review. If reviewers caught agent drift before merge, agents would look
clean in merged history *because a human gate stood in front of them*, which would undercut the
"don't read the diff, install deterministic gates" reading. P5 measures drift on the state the
agent **submitted** (`pull/N/head`), and compares agent vs human PRs **within the same repos**, so
the contrast is independent of any single repo's base drift rate.

## Method

- **Frame.** 649 agent PRs (485 copilot-swe-agent seed + 164 `agent_authored=True` from the #173
  enriched corpus, deduped) across 49 repos. Human control arm freshly enumerated per agent-repo
  from GitHub (merged, non-bot author, capped ≤3×/40 per repo): 639 PRs. base/head/merge SHAs
  recovered via `gh api` (the assets carry none).
- **As-submitted state.** For each PR: `git fetch origin pull/N/head`; `mb = git merge-base
  base head`; `archcheck --diff --diff-mode=memory --format=json mb..head` (two-dot — archcheck
  rejects three-dot). Outcome vector = **the same `categorize()` the adoption panel uses**
  (`run_worklist.py`), so the P5 drift union (`complexity+bool_field+flag_arg+newclone+satd+
  added_edges`) is directly comparable to P1–P4.
- **Eligibility.** C++-touching (`git diff mb..head` hits a C/C++ file) and non-bulk
  (added ≤ 10 000; the bulk gate skips advisories). 291 agent + 260 human eligible PRs across
  **18 repos with both arms** (17 owner clusters).
- **Estimator.** Load-bearing = **within-repo** contrast (agent − human), aggregated as the
  **median of per-repo contrasts**, owner-clustered bootstrap CI + leave-one-repo-out. Pooled is
  shown only to expose composition. Primary outcome = **fraction-with-any-drift** (robust to the
  many zero-drift PRs and to size); mean union reported with an outlier caveat.

## Amendments logged (prereg left these open for P5)

- **A1** outcome = the P1 union via JSON `categorize()` (prereg named no columns for P5).
- **A2** base = `merge-base(base, head)`, diff `mb..head`.
- **A3** matching = within-repo, author-type; per-PR is the finding, per-KLOC is mechanism
  (framing rule #2). Human PRs enumerated live (only 10 agent-repos had a harvested human C++ PR).
- **A4** estimator = repo-median / owner-clustered, pooled shown only for domination.
- **A5** gap = submitted − merged on agent merged PRs — **see the null-by-construction note below.**

## Results

**G1 gate: 1110/1112 head-refs fetched = 99.8% (≥80% → PASS).** The `pull/N/head` mechanism works;
almost nothing is lost to purged refs.

### Primary — within-repo, is the agent PR dirtier as submitted?

| outcome | pooled (agent / human) | within-repo MEDIAN contrast (agent − human) | owner-clustered 95% CI | LOO range |
|---|---|---|---|---|
| fraction-with-any-drift | 0.553 / 0.454 | **−0.210** | [−0.365, **+0.000**] | [−0.235, −0.185] |
| mean drift-union / PR | 8.01 / 4.99 | −0.775 | [−1.833, +0.000] | [−0.882, −0.667] |

Median union per PR (pooled): agent 1, human 0. Hard-gate fires (new/grown cycle or new god-header,
the only blocking events) are rare in both arms: **agent 4/291 = 1.4%, human 2/260 = 0.8%.**

Reading: **pooled, agent PRs look dirtier; within-repo they do not.** The pooled gap is composition —
agent PRs concentrate in a few high-drift repos (firmware `FULL-FIRMWARE-Coche-Marcos` 85 agent PRs,
`hgopalan/ERF` 65, `mla-c/mla-core` 17), while human PRs spread across lower-base-rate repos.
Condition on repo and the excess disappears and slightly reverses, with the CI upper bound sitting
exactly at 0. So **H-P5 is not supported**: agent PRs as submitted are not more architecturally
drift-laden than human PRs in the same repo.

### Why "agents cleaner" is NOT the claim either — the sign is not robust

Size-stratified (pooled) fraction-with-any-drift tells a different story in the middle band:

| added lines | agent frac (n) | human frac (n) |
|---|---|---|
| [0, 50) | 0.26 (94) | 0.23 (94) |
| [50, 200) | **0.74 (73)** | 0.51 (77) |
| [200, 1000) | **0.70 (89)** | 0.54 (59) |
| [1000, ∞) | 0.60 (35) | 0.83 (30) |

Within 50–1000-line PRs, agent is *higher*. Well-size-matched repos split too:
`safijari/openpuck` (agent 147 vs human 138 median lines) and `mla-c/mla-core` show agent ≥ human;
`Puma`/`Agentino` show agent cleaner despite larger PRs; `RingBufferCpp` is a size confound (agent
18 vs human 851 median lines). Per-repo agent samples are small (median ~5–6 PRs). **Net: no robust
difference in either direction** — whatever sign appears flips under repo vs size controls. This is
the central null holding at the as-submitted, pre-merge state.

### Secondary — the review-filter gap is null BY CONSTRUCTION (does not estimate the filter)

For agent merged PRs the gap submitted − merged is 0 for 284/286 PRs (and for 155/157 of the
*nonzero-drift* ones — even the 488-edge PRs have `merged_union == submitted_union` exactly). This
is **mechanical, not a finding**: `pull/N/head` is the PR's **final** head, and the merge lands that
final head unchanged (`merge^1..merge` ≈ `mb..head` because the fork point equals the merge's base
parent). It measures "did the merge alter the final head" (≈ never), **not** "did review change the
agent's *original* submission before merge." The intended test — first-pushed head vs merged —
needs PR push-event history and is **unrun**. Do not cite the gap as evidence that review stripped
(or failed to strip) drift.

## What this means for the two-doors framing

P5 lands on **door 2** (no evidence review was catching agent-specific architectural drift): agent
PRs at merge-decision time are not architecturally dirtier than human PRs in the same repo, so
eyes-on-diff for *architecture* are not justified by an agent-specific drift signal — consistent
with "design what to observe, gate it deterministically." But this is the **weak** form of door 2:
"as submitted" here is the final PR head, which may already reflect any pre-merge revision, and the
gap that would have closed the "review filtered it earlier" loophole is null-by-construction. P5
does not overturn the central null and gives no support to "agents submit dirtier architecture";
the strongest review-filter test remains open.

## Caveats (stated, not buried)

- **Agent arm is ~75% Copilot** (485/649) — a single agent tool; generalizability caveat.
- **"As-submitted" = final PR head**, not first push. Pre-merge review-response commits (rare for
  these weak-human-gate PRs, but unmeasured) would already have shaped it, biasing the raw agent
  drift *down*. The first-push comparison is the unrun sharp test.
- **Two giant monorepos dropped** (`tenstorrent/tt-metal`, `xadupre/onnx-light`): 172
  `skipped_slow_repo` — their full-tree scan exceeds the per-PR budget even at 600 s. The surviving
  18 repos skew toward small/mid size; the largest recovered both-arm repo is `makr-code/ThemisDB`.
- **Small per-repo n** (median ~5–6 agent PRs/repo) → each repo contrast is noisy; the aggregate is
  a median of noisy estimates.
- **Mean union is outlier-driven** (mla-core has three ~488-added-edge feature PRs); frac-with-any
  and median are the load-bearing statistics.

## Assets

- Harness (gitignored): `experiments/adoption_event/p5_fetch_meta.py` (frame + SHAs),
  `p5_run.py` (fetch → merge-base → archcheck, killpg/session hygiene, slow-repo guard, resumable),
  `p5_estimate.py` (fixed-seed estimator). Data under `experiments/adoption_event/p5/`.
- Reuses `run_worklist.categorize()` (identical outcome mapping to the panel) and
  `pilot_eventstudy.py` bootstrap/LOO hygiene.
- Related: `adoption_event_study.md` (P1–P4), `agarwal_replication.md`, `adoption_event_study_prereg.md`.
