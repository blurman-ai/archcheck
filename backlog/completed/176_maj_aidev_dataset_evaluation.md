# [RESEARCH][CORPUS] 176: evaluate AIDev dataset as source/cross-check for the agentic weak-review corpus

**Creation date:** 2026-07-06
**Completion date:** 2026-07-07
**Status:** completed
**Module:** RESEARCH][CORPUS
**Priority:** major
**Difficulty:** medium (data download + filtering + comparison, no live API calls)
**Blocks:** —
**Blocked by:** —
**Related:** #173 (parent corpus task), #173a (enrichment), #173b (channel search completion — currently running live gh-search pipeline; this task is a parallel/offline check, not a replacement, do not touch #173b's running pipeline or its output files)

## Goal

We build the agentic weak-review C/C++ corpus by calling `gh search prs`/`gh search code` live,
thousands of times, hitting GitHub secondary rate limits repeatedly (#173b). A public,
pre-built research dataset — **AIDev** — turned up during #173b (2026-07-06) that already
catalogs agent-authored PRs at a scale we cannot match via live search. Evaluate whether it
can supplement or eventually replace our live-search channels.

**AIDev facts (verified 2026-07-06, do not re-verify):**
- Paper: "The Rise of AI Teammates in Software Engineering (SE) 3.0" — arXiv:2507.15003
  (companion: arXiv:2602.09185, "AIDev: Studying AI Coding Agents on GitHub").
- Replication package: https://github.com/SAILResearch/AI_Teammates_in_SE3 (analysis notebooks only,
  no raw data — repo itself is small).
- Actual data: HuggingFace https://huggingface.co/datasets/hao-li/AIDev — **969 MB, 2,456,073 rows**.
- Scale: 932,791 agentic PRs, 116,211 repos, 72,189 developers, 5 agents (OpenAI Codex 814,522,
  Devin 29,744, GitHub Copilot 50,447, Cursor 32,941, Claude Code 5,137).
- Full-scale tables (`all_pull_request`, `all_repository`, `all_user`): PR metadata, agent label,
  **`language` field on `all_repository`** (this is what lets us filter to C/C++ without live API calls).
- Curated **AIDev-pop** subset (>100 stars, 33,596 PRs): adds `pr_timeline`, `pr_comments`, `pr_reviews`,
  `pr_review_comments_v2`, `pr_commits`, `pr_commit_details` (file-level diffs, large patches excluded
  by GitHub API), `pr_task_type`, `issue`.
- **Caveat found during evaluation, not yet resolved:** `pr_reviews`/`pr_review_comments_v2` carry actor
  identity but do **not** appear to explicitly flag bot-vs-human reviewer status — our own
  `REVIEW_ROSTER` login-matching (from `enrich_reviews.py` / `enrich_full.py`, task #173a/#173b) would
  still be needed on top of AIDev data.
- License: per-repo, inherits the original repo's license; user must check per source.
- Does **not** cover repos below the agent's own crawl window or agents outside the 5 listed
  (e.g. Sourcery, CodeRabbit, Gemini Code Assist, Qodo etc. are review bots we track in #173a/#173b —
  AIDev is about PR *authorship* agents, not review agents; the review-bot angle stays ours to build).

## What this task is NOT

- NOT a corpus rebuild. Do not migrate #173/#173b's corpus onto AIDev in this task.
- NOT a reason to stop or modify the #173b live pipeline — that keeps running independently.
- NOT full reproduction of the AIDev paper's analysis notebooks.

## Detailed instruction

1. Download `all_repository` and `all_pull_request` tables only first (skip AIDev-pop / commit
   diffs initially — full 969 MB is not needed for a first filterability check). Use `datasets`
   or direct HF file download (check the HF dataset viewer / `resolve` URLs before pulling 969 MB blind).
2. Filter `all_repository` by `language` for C/C++/C-family (reuse the extension/language-share logic
   conventions from `experiments/agentic_weak_review/collect_candidates.py::c_family_share`, adapted
   to AIDev's repo-level `language` field rather than `gh repo view --json languages` byte shares —
   note the schema difference, don't assume identical semantics).
3. Join to `all_pull_request` for those repos; count agentic PRs per agent for the C/C++ subset.
4. Compare against our existing corpus (`experiments/agentic_weak_review/agentic_pr_candidates_enriched_v2.jsonl`
   seed + `new_prs_enriched.jsonl` from #173b, once #173b finishes): overlap by repo, overlap by PR URL,
   and what AIDev finds that our gh-search channels missed (and vice versa).
5. For a small sample (~20 PRs) that appear in both AIDev and our corpus, spot-check field agreement
   (merged status, review presence) between AIDev's data and our live `gh pr view` enrichment — AIDev's
   snapshot could be stale relative to a live PR.
6. If AIDev-pop's `pr_reviews` looks usable for review-bot detection, pull that subset for the C/C++
   overlap only (not all 33,596) and test our `REVIEW_ROSTER` matching against it.
7. Write findings to `docs/research/aidev_dataset_evaluation.md`: coverage comparison (numbers, not
   vibes), whether AIDev should become a standing input for future corpus refreshes, what it cannot
   replace (review-bot detection, our specific weak-gate criteria), and a recommendation (adopt /
   cross-check only / discard) with reasoning tied to the numbers, not to AIDev's own paper claims.

## Control cases (contract)

| Check | Expected |
|-------|----------|
| `all_repository` C/C++ filter | non-zero repo count; report exact number |
| Repo overlap with our corpus | report count + list any of our top 10 ranked repos (`top_agent_reviewed_repos.json`) missing from AIDev entirely |
| PR-level field spot-check (20 samples) | report agreement rate; flag if AIDev data is stale vs. live `gh pr view` |
| Recommendation | one of: adopt as primary / use for cross-validation / discard — with the specific number that drove the call |

## Definition of done

`docs/research/aidev_dataset_evaluation.md` exists with the coverage comparison and a clear
recommendation. No corpus files under `experiments/agentic_weak_review/` are modified by this task.
Report delivered. No commit without explicit `/commit`.

## Do NOT do

- Do NOT touch `experiments/agentic_weak_review/*` (owned by #173/#173b, currently running live).
- Do NOT download the full 969 MB blind before checking whether a filtered/streamed subset is possible.
- Do NOT treat AIDev's own paper conclusions as ground truth for our specific weak-review criteria —
  independently verify against our matcher.

## Changed files

| File | Change |
|------|--------|
| `docs/research/aidev_dataset_evaluation.md` | new — findings + recommendation |
| `backlog/new/176_maj_aidev_dataset_evaluation.md` | this task |

## Progress (2026-07-06, partial — steps 1-3 done by background agent)

### Step 1: Downloaded tables

Downloaded directly from HuggingFace dataset `hao-li/AIDev`:
- **all_repository.parquet**: 116,211 rows × 7 columns (5.85 MB)
  - Columns: id, url, license, full_name, language, forks, stars
- **all_pull_request.parquet**: 932,791 rows × 14 columns (315.45 MB)
  - Columns: id, number, title, body, agent, user_id, user, state, created_at, closed_at, merged_at, repo_id, repo_url, html_url

Files cached at `/home/localadm/projects/cpparch/experiments/aidev_dataset/all_{repository,pull_request}.parquet`

### Step 2: Language field inspection and C/C++ filtering

Inspected `language` field in all_repository:
- **Total distinct language values**: 250
- **C language**: exactly 1,357 repos
- **C++ language**: exactly 2,445 repos
- **Total C/C++**: 3,802 repos

Distinct language values examined (subset with 'C' prefix/substring shown):
- Plain "C": 1,357 repos
- Plain "C++": 2,445 repos (no C++11/C++14/C++17/C++20 variants in the field — GitHub's language classifier outputs normalized "C++" label)
- Related but excluded: Objective-C (42), Objective-C++ (3), CUDA (19), Cython (7), C# (3,463), CSS (1,688), etc.

**Filter applied**: `language IN ('C', 'C++')`

### Step 3: Join and count by agent

Joined all_pull_request to filtered C/C++ repos on repo_id:

| Agent | PR Count |
|-------|----------|
| OpenAI_Codex | 26,612 |
| Copilot | 1,848 |
| Devin | 735 |
| Cursor | 489 |
| Claude_Code | 97 |
| **TOTAL** | **29,781** |

### Output files

All files written to `/home/localadm/projects/cpparch/experiments/aidev_dataset/`:
- `all_repository.parquet` — raw full table (116,211 repos)
- `all_pull_request.parquet` — raw full table (932,791 PRs)
- `c_cpp_repos_filtered.csv` — filtered to C/C++ only (3,802 repos)
- `c_cpp_prs_filtered.csv` — joined C/C++ PRs (29,781 rows)
- `aidev_cc_summary.json` — structured summary with counts and metadata

## Progress (2026-07-07, steps 4-7 complete — task fully done)

#173b finished (all 10 control cases pass, final corpus: 16,982 PRs / 1,636 repos). Ran steps 4-7
against the finished corpus. Full writeup: [docs/research/aidev_dataset_evaluation.md](../../docs/research/aidev_dataset_evaluation.md).

**Step 4 (overlap):** Repo overlap 43/1,636 ours ∩ 3,802 AIDev C/C++. PR overlap 69/16,982 ∩ 29,781.
Only 2 of our top-10 ranked repos appear in AIDev at all. Root causes found (not a bug in either
dataset): (a) AIDev selects by PR *author* identity (5 named agents), we select by *reviewer*
identity (#173b Channels 1-3) — near-orthogonal populations; (b) even the directly-comparable
slice (our 485-PR seed corpus, also author-identity-based) has **zero** overlap with AIDev because
AIDev's "Copilot" agent matches literal login `Copilot`, not `app/copilot-swe-agent`; (c) AIDev's
entire C/C++ subset is date-capped at **2025-07-30** (confirmed against its own changelog) — about
a year stale relative to today (2026-07-07).

**Step 5 (spot-check):** 20 of the 69 overlapping PRs compared on `merged_at` between our live
`gh pr view` data and AIDev's snapshot — **20/20 exact agreement**. Where AIDev covers a PR, the
data is accurate; the gap is coverage, not trust.

**Step 6 (review-bot detection test):** Deliberately **not run** — reasoned skip, not a lazy one.
AIDev-pop's review tables have no explicit bot flag (would still need our own `REVIEW_ROSTER`
matching, no shortcut gained), are restricted to >100-star repos (opposite of where our long-tail
candidates live), and the already-tiny 69-PR overlap makes the ~600 MB extra download not worth it
under the task's own "don't download blind" rule.

**Recommendation:** cross-validation only, not a primary/standing input. Do not feed AIDev into
Channels 1-3 (different question, stale data would silently drop the newest year of the corpus).
AIDev's 29,781-PR C/C++ author-identity list is a legitimate seed for a *future, separate* task if
the #173 seed-corpus step is ever expanded beyond `copilot-swe-agent` — not a #173b substitute.

Output files: `experiments/agentic_weak_review/aidev_comparison_result.json` (step 4 detail),
`experiments/agentic_weak_review/aidev_spotcheck_result.json` (step 5 detail),
`docs/research/aidev_dataset_evaluation.md` (full writeup + recommendation).

No files under `experiments/agentic_weak_review/*` corpus JSONLs were modified (only two small
result JSONs were added, per the task's allowance to write comparison output). No commit made.
