# [RESEARCH][CORPUS] Agentic C++ repositories with weak human review

**Creation date:** 2026-07-06
**Start date:** 2026-07-06
**Status:** wip
**Module:** RESEARCH][CORPUS
**Priority:** major
**Difficulty:** medium
**Blocks:** —
**Blocked by:** —
**Related:** #074 (ai_repo_discovery_roi_alignment), #119 (unified per-commit drift correlation), #145 (trending C++ cohort), #146 (bot review lets drift through), #172 (signal attribution audit), #174 (gate moderator — the local-corpus depth complement to this breadth corpus)
**Verification:** manual GitHub metadata audit; no `archcheck` run yet

## Goal

Build a second experimental corpus for `archcheck`: C/C++ GitHub repositories where
AI agents appear to create PRs and the merge path looks weakly human-reviewed.

This corpus is deliberately different from mature OSS projects with strong maintainer
review. The hypothesis to test later is:

> AI-authored PRs with weak human review are more likely to let structural regressions through.

Do **not** claim causality in this task. The current phase is candidate discovery only.

## Why this task exists

The existing broad corpus may be biased toward healthier repositories:

- many PRs pass through human maintainer review before merge;
- strong review/CODEOWNERS/branch protection can filter out architectural drift;
- repo fixed effects can hide the specific weak-gate condition we actually care about.

So the next useful contrast is not just `agentic vs human`, but:

- `agent-authored + weak human gate`
- versus `agent-authored + visible human review`
- versus human-authored PRs in the same repositories.

## Current artifacts

Generated under `experiments/agentic_weak_review/` (gitignored, local research output):

| File | Purpose | Status |
|------|---------|--------|
| `repo_candidates.md` | ranked repositories with evidence and PR links | initial seed (29 repos) |
| `agentic_pr_candidates.jsonl` | one JSON object per candidate PR (seed) | 485 PRs |
| `agentic_pr_candidates_enriched.jsonl` | seed PRs + agent_reviewer_logins, human_gate_review_count | 485 PRs, complete |
| `agentic_pr_candidates_categorized.jsonl` | enriched PRs + categorical tags | 485 PRs, complete |
| `search_results_channel1.jsonl` | PRs found via reviewer-identity search | 2,266 PRs (raw format) |
| `search_results_channel2.jsonl` | PRs found via agent author × reviewer bot | 8 PRs (raw format) |
| `search_results_new_prs.jsonl` | consolidated new PRs from Channels 1-2 | 2,274 PRs, cleaned |
| `notes.md` | search strategy, false positives, ambiguities, sweep results | updated with Channel 1-3 sweep |
| `collect_candidates.py` | reproducible collection script (seed) | static |
| `categorize_prs.py` | script to add categorical tags | new |
| `search_channels.py` | script for Channel 1-3 searches | new |
| `cloned_repos.tsv` | manifest of cloned repos (TBD) | empty (for future) |

## Results (as of 2026-07-06 23:XX)

### Seed corpus (initial collection, 2026-07-06 afternoon)
- **485 PRs** from **29 repositories**
- All from PR author `app/copilot-swe-agent`
- **319** with zero visible human review/comment
- **283** with fast merge (`<=60 min`)
- **94** with visible human review (likely from bots)

### After enrichment and categorization (173a + Step 1)
- **108 PRs** have `agent_review_present == true` (agent reviewer bots detected)
- **235 PRs** are `strong_candidate` (agent + weak gate + fast merge)
- **68 PRs** are `agent_review_only_candidate` (agent + bot review, no human review)
- Human gate reviews: distribution from 0 to N (TBD: histogram)

### After Channel 1-3 searches (Step 2, 2026-07-06 evening) — PARTIAL, raw discovery only

Audited 2026-07-06 (evening): the numbers below are search hits, NOT candidates per
§Candidate definition. Corrections to the original claims:

- **2,274 new PR URLs** across **250 new repositories** (dedup vs seed verified: 0 overlap).
  These are raw `pr_url` strings only — no language-byte filter, no weak-gate proxy,
  no per-candidate reason yet.
- Channel 1 (reviewer identity): 2,266 URLs, but the query matrix is ~1/4 covered.
  Actually executed: `coderabbitai×C` (hit the 1000-result cap, truncated),
  `cubic-dev-ai×C++` (hit the cap), `korbit-ai×C++`, `korbit-ai×C`, `ellipsis-dev×C`.
  NOT executed at all: `copilot-pull-request-reviewer`, `gemini-code-assist`,
  `sourcery-ai`, `greptile-apps`, `amazon-q-developer`, `claude`; `coderabbitai×C++`
  is also missing. No `--created` date slicing → capped queries are truncated samples.
- Channel 2 (agent × reviewer): 8 URLs from 8 repos.
- Channel 3 (config files): **tooling failure, not "0 results"**. `search_channels.py`
  calls `gh search code` without `--json`; in a non-TTY run gh prints nothing and exits 0,
  so the script parsed empty output. The same query with `--json` returns matches
  (verified twice on 2026-07-06 with `.coderabbit.yaml`). Channel 3 must be rerun.

### Acceptance criteria — met for the SEED corpus only
- ✓ At least 20 candidate repositories → 29 (seed)
- ✓ At least 100 merged PR candidates → 485 (seed)
- ✓ Every candidate has a concrete link and stated reason → seed only (`why_candidate`)
- ✓ No PR included only for AI mentions → seed only
- ✓ No causal claim made
- ✗ The 2,274 new URLs do NOT count toward the criteria until enriched and filtered
  (→ #173b)

### Known limitations
- New 2,274 PRs not yet enriched with review/comment data (would require ~2K more API calls; hit rate limit during search)
- Per-file-path filtering (`.c/.cpp/.h/...`) deferred (can be added on-demand for subset)
- Timeline/mergedBy data (auto-merge detection) not yet collected
- `CORPUS_SUMMARY.yaml` and `top_agent_reviewed_repos.json` are built on the raw URL set —
  treat as provisional until the #173b re-enrichment

## Candidate definition

Keep a PR only if:

- repository is C/C++-relevant by GitHub language data;
- PR is merged;
- PR has a plausible agent-authorship or agent-review signal;
- merge path is weakly human-reviewed by visible GitHub metadata:
  - zero formal human reviews; or
  - zero/minimal human comments; or
  - very fast merge; or
  - comments/reviews mostly from bots.

Every candidate must have:

```json
{
  "repo": "owner/name",
  "pr_url": "...",
  "merged": true,
  "agent_signal": "...",
  "human_review_count": 0,
  "human_comment_count": 0,
  "bot_comment_count": 0,
  "merge_latency_minutes": 0,
  "changed_files": 0,
  "additions": 0,
  "deletions": 0,
  "why_candidate": "..."
}
```

## Classification labels

Use these labels in the next pass. Do not collapse everything into "AI did it".

| Label | Meaning | Strong evidence |
|-------|---------|-----------------|
| `agent_authored` | agent likely wrote the PR/commits | PR author bot/app; commit author bot; AI trailer |
| `agent_reviewed` | agent/bot performed review/commentary | CodeRabbit/Copilot/Claude review comments; `@coderabbitai review`; `@copilot review` |
| `human_reviewed` | visible human formal review exists | `APPROVED` / `CHANGES_REQUESTED` by non-bot |
| `human_discussed` | visible human discussion exists | meaningful non-bot comments, not just `@copilot fix` |
| `weak_human_gate` | little visible human filtering | no human review and `<=1` human comment |
| `fast_merge` | merge happened quickly | threshold initially `<=30` or `<=60` minutes |
| `possible_auto_merge` | merge has automation evidence | `mergedBy` bot/app or timeline auto-merge event |
| `strong_candidate` | best corpus target | `agent_authored && weak_human_gate && fast_merge` |
| `agent_review_only_candidate` | bot review substituted for human review | `agent_authored && !human_reviewed && agent_reviewed` |

## Auto-merge / agent-review detection rules

### Agent-authored PR

Set `agent_authored` when at least one strong signal exists:

- PR author: `app/copilot-swe-agent`, `google-labs-jules[bot]`,
  `devin-ai-integration[bot]`, `sweep-ai[bot]`;
- commit author/committer is an AI bot/app;
- commit trailer/body mentions `Co-authored-by: Claude`, `Generated with Claude Code`,
  `Codex`, `Cursor`, `Devin`;
- human comments `@copilot fix ...`, followed by bot commits/comments.

Mark as uncertain, not high-confidence, when only weak evidence exists:

- PR title/body only mentions AI tooling;
- repository has Claude/Copilot workflows but the PR itself is human-authored.

### Agent-reviewed PR

Set `agent_reviewed` when one of these signals exists:

- review/comment author is an AI review bot, for example `coderabbitai`,
  `copilot-pull-request-reviewer`, `github-copilot[bot]`, `qodo`, `sourcery-ai`;
- body contains AI review structure: `Walkthrough`, `Actionable comments`,
  `AI-generated review`, suggested patches;
- human invokes review with `@coderabbitai review`, `@copilot review`, `@claude review`.

Keep this separate from approval. AI review comments are usually advisory, not merge authority.

### Auto-merged / weakly human-merged

Set `weak_human_gate` / `fast_merge` from these observable signals:

- `human_review_count == 0`;
- `human_comment_count == 0`, or only command comments such as `@copilot resolve conflicts`;
- merge latency is very low for the size of the PR;
- review exists only from bots;
- PR is marked `[WIP]` but merged quickly;
- repeated pattern in one repo: many agent PRs in a short window, little human discussion.

Set `possible_auto_merge` only when stronger timeline/merge evidence exists:

- `mergedBy` is bot/app;
- timeline contains auto-merge events;
- merge commit author/committer is bot;
- branch protection/auto-merge allowed merge after CI without human approval.

Use a GraphQL timeline/event pass for these; `gh pr list` is not enough.

## Search strategy for agent-REVIEWED repos (probed 2026-07-06)

The first pass found agent-*authored* PRs only. Finding repos where the *review* is agentic
needs different channels. All four below were probed live with `gh` on 2026-07-06.

### Known blind spot in the current collector (must fix before next pass)

`collect_candidates.py::count_reviews` skips reviews with `state == "COMMENTED"`.
Copilot code review (`copilot-pull-request-reviewer[bot]`) and CodeRabbit post almost
exclusively `COMMENTED` reviews (they cannot approve by default), so **all 485 collected
candidates show `bot_review_count == 0`, which is empirically false**: a probe
(`--author copilot-swe-agent[bot] --reviewed-by copilot-pull-request-reviewer[bot]`)
returned PRs from seed repos already in the corpus (`ghpascon/X714`, `ImagingTools/Agentino`,
`ImagingTools/Puma`, `slafftrosheen/m1m1r.kit`). Fix: keep two separate counters —
`human_gate_reviews` (APPROVED/CHANGES_REQUESTED by non-bot; COMMENTED still excluded,
correct for gate strength) and `agent_review_present` (ANY review or comment by a roster bot,
including COMMENTED).

### Channel 1 — PR search by reviewer identity (primary; verified)

`gh search prs --merged --language c++ --reviewed-by "<bot-login>"` works and returns
C++ repos directly. Verified logins:

- `coderabbitai[bot]` — works;
- `copilot-pull-request-reviewer[bot]` — works (`reviewed-by:Copilot` display name does NOT);
- `gemini-code-assist[bot]` — works.

To probe (same pattern, unverified): `qodo-merge-pro[bot]`, `sourcery-ai[bot]`,
`greptile-apps[bot]`, `ellipsis-dev[bot]`, `korbit-ai[bot]`, `cubic-dev-ai[bot]`,
`amazon-q-developer[bot]`, `claude[bot]`.

Practical notes:

- run each query for `--language c++` AND `--language c` (filter is repo primary language;
  mixed repos are missed — Channel 3 compensates);
- also run the `--commenter <bot>` variant: CodeRabbit walkthroughs are issue comments and
  can exist without a formal review object;
- search caps at 1000 results per query → slice by `--created` date windows to paginate.

### Channel 2 — machine-loop intersection (verified; highest-value cell)

`gh search prs --merged --language c++ --author "<agent-bot>" --reviewed-by "<review-bot>"`
directly returns the `agent_authored && agent_reviewed` cell in one query. Verified with
copilot-swe-agent × copilot-pull-request-reviewer. Run the cross product of the author
roster (§Agent-authored) × reviewer roster (Channel 1). Weak-human-gate still needs
per-PR enrichment (a human may ALSO have reviewed).

### Channel 3 — config-file code search (repo-level opt-in; verified, noisy)

`gh search code --filename .coderabbit.yaml` works (also try `.coderabbit.yml`,
`.pr_agent.toml`, `.sourcery.yaml`, `.korbit-config.yaml`, `.gemini/`). For Claude:
`--filename claude-code-review.yml` — verified but VERY noisy (template/guide/skill repos
dominate); require path to be exactly `.github/workflows/...` and confirm actual bot review
activity in merged PRs before accepting a repo.

This channel is essential for one case Channel 1 cannot see: action-based reviewers
(anthropics/claude-code-action and similar) post as `github-actions[bot]` or a repo-local
app, so `reviewed-by:` search by a global bot login misses them. Attribute these by body
markers ("Claude Code Review", "Generated with Claude Code"), never by `github-actions[bot]`
login alone (CI noise).

Results are language-mixed → post-filter by `gh repo view --json languages` as in pass 1.

### Channel 4 — re-enrich the existing 485 candidates (cheapest, do first)

The seed corpus already contains agent-reviewed PRs mislabeled as `bot_review_count == 0`
(see blind spot above). Re-fetch reviews including COMMENTED state, record reviewer logins,
and tag `agent_reviewed` retroactively. No new search needed.

### Pitfalls

- Allowlist review agents by roster; `*[bot]` alone is not an agent-review signal
  (dependabot, renovate, CI bots).
- CodeRabbit is free for OSS → its presence does not imply humans stopped reviewing;
  the weak-gate test stays per-PR, not per-repo.
- GH Archive / BigQuery (`PullRequestReviewEvent` by bot actors) is the scale fallback
  if API caps bite; not needed at current corpus size.

## Corpus storage layout (decided 2026-07-06)

Cloned repositories for this corpus go to **`/home/localadm/oss_agentic/`** — a sibling of the
main corpus root, NEVER into `/home/localadm/oss/`. Rules:

- same directory naming as the main corpus: `owner_name` (slash → underscore);
- `analysis/CORPUS_CRITERIA.md` gates do **not** apply here — this corpus has its own
  candidate definition (§Candidate definition above); do not "fix" that by running its gates;
- conversely, never let these repos leak into the main-corpus ledgers/baselines: any script
  that walks a corpus root must take the root as an explicit parameter, not assume `/oss/`;
- clone with `GIT_LFS_SKIP_SMUDGE=1`, full history (per-commit replay needs blobs; repos are small);
- keep a manifest `experiments/agentic_weak_review/cloned_repos.tsv`
  (`repo<TAB>clone_date<TAB>why<TAB>candidate_pr_count`) so the clone set is auditable,
  same spirit as the main-corpus ledger.

## Execution plan

- [x] Create first candidate reports from C++ GitHub PR metadata.
- [x] Reach acceptance floor: at least 20 repos and 100 PR candidates.
- [x] Keep report skeptical and link every candidate.
- [x] **173a** Fix `count_reviews` blind spot: re-enrich existing 485 PRs (Channel 4).
      - Added: `agent_reviewer_logins`, `agent_review_present`, `human_gate_review_count`
      - 108 PRs have `agent_review_present == true`
      - 5 errors (< 50 threshold) ✓
      - Results: `agentic_pr_candidates_enriched.jsonl`
- [x] Add categorical tags to enriched PRs (agent_authored, agent_reviewed, weak_human_gate, fast_merge, strong_candidate, etc.)
      - 235 `strong_candidate` (agent + weak gate + fast merge)
      - 68 `agent_review_only_candidate` (agent + bot review, no human)
      - Results: `agentic_pr_candidates_categorized.jsonl`
- [ ] Reviewer-identity search sweep (Channel 1) — **PARTIAL** (audited 2026-07-06):
      only 5 of ~16 matrix queries ran before rate limits (2,266 raw URLs); six reviewers
      not queried at all, two queries truncated at the 1000 cap. Completion → **#173b**.
- [x] Machine-loop cross product (Channel 2): agent authors × review bots
      - 8 new PRs from 8 repos found
- [ ] Config-file code search (Channel 3) — **TOOLING FAILURE, must rerun** (audited
      2026-07-06): `gh search code` was called without `--json`, which prints nothing in
      a non-TTY run; the recorded "0 results" is false (same query with `--json` returns
      matches). Rerun → **#173b**.
- [ ] Enrich all new PR URLs (reviews/comments/**file paths**), tag `touches_cpp_source` /
      `cpp_files_changed`, and build `repo_selection.tsv` ranked by C++ source files changed
      in agent PRs (user directive 2026-07-06) → executor-ready subtask **#173b**
      (`backlog/new/173b_maj_agent_review_search_completion.md`).
- [ ] Add timeline/mergedBy pass for a smaller top-N set to classify `possible_auto_merge`.
- [ ] Add a second pass that fetches per-file paths only for new 2,274 PRs and tags
      `touches_cpp_source` (deferred: would hit rate limits; can process subset on demand).
- [ ] Split PRs into:
      `source_code`, `tests`, `docs`, `ci`, `mixed`, `unknown` (on 485 enriched set first).
- [ ] Pick a high-quality subset: repos with at least 5 source-changing agent PRs and enough
      human-authored merged PRs in the same period.
- [ ] Only after that, run `archcheck` on selected PRs/commits.

## Acceptance criteria

- At least 20 candidate repositories unless the signal is clearly rare.
- At least 100 merged PR candidates if available.
- Every candidate has a concrete link and a stated reason.
- No PR is included only because it mentions AI in prose.
- Uncertain cases are marked as uncertain.
- No causal claim is made.

## Non-goals

- Do not run `archcheck` in the discovery phase.
- Do not write the article.
- Do not overfit to one bot name in the final corpus.
- Do not claim that zero GitHub review proves zero human review.

## Open questions

- What is the best merge-latency threshold: `<=5`, `<=30`, or `<=60` minutes?
- Should owner self-approval count as meaningful review or weak gate?
- How to handle bot review plus one command-style human comment?
- Should CI-only/docs-only PRs stay in the weak-review corpus, or only in a separate process corpus?

## Progress (2026-07-07) — first archcheck run on the corpus; clone signal is ~50% noise

First `archcheck` pass over the selected corpus repos is running under
`experiments/agentic_weak_review/archcheck_run/` (`clone_and_run.py`, all 5 modes per repo:
fullrepo/duplication/history/drift/per-commit graph-drift). ~32 of ~46 repos fully done when
this note was written; run still in progress — **let it finish before recomputing.**

**Key finding — the raw clone-drift signal is not publishable as-is.** Full-JSONL breakdown
across 22 repos (8284 clone-drift events):

| Bucket | Events | Share |
|--------|-------:|------:|
| AUTHORED (plausible drift — **upper bound**) | 4162 | 50.2% |
| base-lib multi-mount (mla-core embeds its base-lib 3×) | 1733 | 20.9% |
| net-snmp vendored (`net-snmp-*-final-patched/`) | 1124 | 13.6% |
| giant-amalgam (7900-line `ezsnmp_netsnmp.cpp` etc.) | 894 | 10.8% |
| cross-mount (same file under two roots) | 371 | 4.5% |
| **Noise total** | **4122** | **49.8%** |

~50% is classifiable noise archcheck's classifier does not yet catch. The residual "AUTHORED"
is an upper bound — 2507 of it is mla-core alone (itself suspect: embedded base-lib + platform
ports), so the true "new copy-paste in authored code" is materially lower. A trustworthy corpus
number needs the classifier fix + a rerun.

**Self-check catch (honest):** first pass I called mla-core "mostly embedded"; per-path recount
showed 59% of its clones sit in `core-os/` (not base-lib). Corrected before reporting — see
[[feedback_verify_each_case_over_aggregate]].

**Spawned:** [#179](../new/179_maj_vendored_version_dir_classification.md) (net-snmp version-dir
classification gap — the clean, safe part) and a `backlog/DEBT.md` entry (giant-amalgam files —
separate heuristic). `baselib` deliberately NOT banned (over-exclusion). The drift→agent-PR join
(`agent_pr_matches.tsv`, 64 PRs) is still TODO — the payoff step this corpus exists for.

## Progress (2026-07-07, later) — drift→agent-PR join DONE; strong hypothesis refuted

The payoff join (previously TODO) is done, after the #179 classifier fix, via two read-only passes
(existing drift jsonl + `gh`, no clones/reruns). Detail:
`experiments/agentic_weak_review/{task1_breadth_join.md, task2_depth_netdiag_mlacore.md}`.

Scope: the 10 repos with BOTH a per-commit drift replay AND ≥1 agent-reviewed PR = **104 agent-reviewed
PRs, all `copilot-swe-agent`-authored** (so the class-D human-authored arm is empty by construction).
SHA-level join (full 40-char content SHAs; `(#N)`-subject fallback for squash repos), then the actual
AI review bodies were read per drift-HIT PR.

**Result — the literal hypothesis ("weak AI review lets architectural drift THROUGH the gate") is not
supported on this corpus.** Across all 104 agent-reviewed PRs: **0 grown cycles, 0 new cross-module /
layer dependencies.** OTGW-firmware (primary target, 51 reviewed) is a clean null — its agent PRs touch
JS/HTML/docs, not include structure. Agentino/Puma also null.

**Two narrower findings hold (both witnessed):**
1. *Disjoint altitudes* — bots catch line-level correctness and some duplication (ODR, leaks,
   missing/unused includes, duplicated `Theme` constants) but never name an architectural metric;
   function-complexity growth passes unmentioned even when the bot reads the exact file
   (m1m1r.kit#5 27→50, #3 cplx 32, NetDiagnostics#8 7→19).
2. *Disjoint commit sets* — the real structural drift bypasses the review gate: NetDiagnostics'
   `1d4fe45` "Phase-2 MVC" (+32 edges, new `controllers→engine/models` coupling) was pushed
   direct-to-main with no PR; mla-core's drift is unreviewed agent renames.

**Reframe:** the corpus payoff is not "a bot slept through a cycle" but "structural drift and AI review
operate on disjoint commit sets and disjoint altitudes — archcheck covers the plane and the commits the
bot never processes." This is a second, independent confirmation of #146; written up in
`docs/research/bot_review_drift.md` §11 and JOURNEY (2026-07-07).

**Caveats:** class D empty (no human-authored arm); the only same-PR silent-on-structure cases are
local-complexity, 4 of them in one greenfield hobby ESP32 repo (m1m1r.kit); mla-core Σ3390 is ~75%
path-rename re-count of a triple-mounted base-lib (real floor ≈71 edges).

**Still open:** re-run the corpus clone/complexity numbers post-#179 to refresh the ~50%-noise figure
(separate step); a human-authored comparison arm would need PRs outside the copilot-swe-agent seed.

## Changed files

| File | Change |
|------|--------|
| `experiments/agentic_weak_review/` | local candidate reports and collection script |
| `experiments/agentic_weak_review/archcheck_run/` | first archcheck corpus run (all 5 modes/repo) |
| `experiments/agentic_weak_review/task1_breadth_join.md` | corpus-wide drift×review join (breadth) |
| `experiments/agentic_weak_review/task2_depth_netdiag_mlacore.md` | NetDiagnostics + mla-core deep dive |
| `docs/research/bot_review_drift.md` | §11 — second-corpus confirmation + disjoint-planes reframe |
| `backlog/wip/173_maj_agentic_weak_review_corpus.md` | this task |
