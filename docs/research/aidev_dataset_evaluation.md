# AIDev dataset evaluation (task #176)

Evaluates whether the public **AIDev** dataset (Hao Li et al., arXiv:2507.15003 /
arXiv:2602.09185, hosted at [huggingface.co/datasets/hao-li/AIDev](https://huggingface.co/datasets/hao-li/AIDev))
can supplement or replace the live `gh search`-based corpus built in #173/#173a/#173b.
Steps 1-3 (download, C/C++ filter, agent×repo counts) were done by a background agent on
2026-07-06; this document covers steps 4-7 (comparison, spot-check, review-bot test, recommendation),
run against the *finished* #173b corpus (16,982 usable PRs / 1,636 repos).

## Recap: what steps 1-3 already established

- Downloaded `all_repository` (116,211 repos) and `all_pull_request` (932,791 PRs) full-scale
  tables from HuggingFace.
- Filtered to C/C++ by the repo-level `language` field: **3,802 repos** (1,357 "C" + 2,445 "C++").
- Joined to get agentic PRs in those repos: **29,781 PRs** across 5 agents (Codex 26,612,
  Copilot 1,848, Devin 735, Cursor 489, Claude Code 97).
- Files: `experiments/aidev_dataset/{all_repository,all_pull_request}.parquet`,
  `c_cpp_repos_filtered.csv`, `c_cpp_prs_filtered.csv`, `aidev_cc_summary.json`.

## Step 4 — overlap with our corpus

Compared AIDev's C/C++ subset against the full #173b corpus (seed 485 + 16,497 enriched new PRs,
1,636 distinct repos):

| | Repos | PRs |
|---|---|---|
| Ours only | 1,593 | 16,913 |
| AIDev only | 3,759 | 29,712 |
| **Overlap** | **43** | **69** |

Overlap is small relative to either side (~2.6% of our repos, ~1.1% of AIDev's C/C++ repos).
Only 2 of our top-10 repos ranked by agent-review presence (`rsyslog/rsyslog`, `netdata/netdata`)
appear in AIDev at all; the other 8 (including the #1 and #2 ranked, `deepin-community/kernel`
and `lvgl/lvgl`) are absent from AIDev entirely.

**This low overlap is not a data-quality problem — it is a methodology mismatch, and it is the
single most important finding of this evaluation:**

1. **AIDev selects by PR *author* identity** (one of 5 named coding agents). Our Channel 1/2
   (#173b) selects by *reviewer* identity (any of ~10 review bots), independent of who authored
   the PR. These are close to orthogonal populations — a human-authored PR reviewed by CodeRabbit
   (most of our corpus) will never appear in AIDev; an agent-authored PR reviewed only by a human
   (most of AIDev) will never appear in our Channel 1/2.
2. Even restricting to the *directly comparable* slice — our seed corpus (485 PRs, built the same
   way AIDev is: by author identity, specifically `app/copilot-swe-agent`) — overlap with AIDev is
   **zero**. Root cause, found by inspecting AIDev's raw `user` column for its "Copilot"-labeled
   rows: AIDev matches the literal login `Copilot` (1,834 of 1,848 C/C++ Copilot rows), not
   `app/copilot-swe-agent`. Different product/login identity, not a a bug in either dataset — a
   naming discrepancy between GitHub Copilot's coding-agent branding and the account that CLI
   `gh search prs --author` actually matches on GitHub's side.
3. **AIDev's snapshot is stale.** Every PR in the C/C++ subset has `created_at`/`merged_at` in the
   range **2024-12-25 to 2025-07-30** — confirmed against the replication package's own changelog
   ("Update Aug 10, 2025: dataset refreshed to include data up to August 1, 2025"). Today is
   2026-07-07: AIDev's crawl window ends roughly **one year before our corpus's newest data**. Any
   PR from the last ~12 months — very plausibly the fastest-growing slice of agentic-PR activity —
   is definitionally absent from AIDev, independent of login-matching.

## Step 5 — field-agreement spot-check

Sampled 20 of the 69 overlapping PRs and compared `merged_at` (our live `gh pr view` enrichment
vs. AIDev's `all_pull_request.merged_at`):

**20/20 exact agreement** (identical ISO-8601 timestamps down to the second). Where AIDev's data
does cover a PR, it is accurate — the low overlap is a coverage/scope limitation, not a
trustworthiness problem. Full detail: `experiments/agentic_weak_review/aidev_spotcheck_result.json`.

## Step 6 — review-bot detection test (AIDev-pop `pr_reviews`)

**Not run — deliberately skipped**, for a reasoned rather than a lazy reason:

- AIDev-pop's `pr_reviews`/`pr_review_comments_v2` tables (per the HuggingFace schema check in
  #176's original write-up) carry actor identity but **no explicit bot-vs-human flag** — we would
  still need our own `REVIEW_ROSTER` login-matching on top, exactly as we already do in
  `enrich_full.py`. There is no shortcut to gain here, only a second data source to reconcile.
- AIDev-pop is further restricted to repos with **>100 stars** — the opposite direction from where
  our corpus finds most of its long-tail weak-review candidates (many of our top repos, e.g.
  `ghpascon/X714`, `slafftrosheen/m1m1r.kit`, are small/low-star).
- Given steps 4-5 already show only 69 overlapping PRs total (and 0 overlap on the
  directly-comparable author-identity slice), downloading and reconciling AIDev-pop's review
  tables (adds ~600+ MB, per the HF page) for a review-bot-detection test would very likely
  reconcile with well under a hundred of our own 16,982 PRs — not enough signal to justify the
  download under the "do not download blind" instruction in #176's own contract.

## Recommendation

**Use for cross-validation only; do not adopt as a primary or standing input.**

Specifically:
- **Do** keep AIDev in mind as an independent sanity-check for the tiny slice where PRs coincide
  (69 PRs currently) — the field-agreement result (20/20) means it is safe to spot-check merge
  status/timestamps against it when both datasets happen to cover the same PR.
- **Do not** replace or feed Channels 1-3 (#173b) with AIDev — they answer a different question
  (reviewer identity vs. author identity) and AIDev's staleness would silently drop ~a year of
  the corpus's newest and presumably most relevant data.
- **Do not** invest further engineering here (e.g., downloading AIDev-pop's review tables) without
  a fresh AIDev release — the project does periodically refresh (it was refreshed once already,
  Aug 2025); if a future refresh closes the ~1-year gap, this evaluation should be re-run, not
  assumed to still hold.
- If a *much larger* author-identity expansion is ever wanted (i.e., growing the #173 seed-corpus
  step beyond `copilot-swe-agent`+3 others to include Codex/Cursor at scale), AIDev's 29,781
  C/C++ PRs are a legitimate one-time seed list to pull from — but that is a different, future
  task, not a #173b substitute, and would need its own weak-review filtering pass since AIDev
  doesn't score for that.

## Files

- `experiments/aidev_dataset/` — raw + filtered AIDev tables (from steps 1-3).
- `experiments/agentic_weak_review/aidev_comparison_result.json` — full overlap numbers (step 4).
- `experiments/agentic_weak_review/aidev_spotcheck_result.json` — 20-PR field-agreement detail (step 5).
- `backlog/new/176_maj_aidev_dataset_evaluation.md` — task contract + steps 1-3 progress notes.
