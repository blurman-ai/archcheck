# [RESEARCH][CORPUS] 173a: tag agent-reviewed PRs in the weak-review corpus

**Creation date:** 2026-07-06
**Status:** new
**Module:** RESEARCH][CORPUS
**Priority:** major
**Difficulty:** low (mechanical; all decisions pre-made)
**Blocks:** #173 next passes
**Blocked by:** —
**Related:** #173 (parent task; read its §"Search strategy for agent-REVIEWED repos" for context)
**Executor:** junior model (Haiku/Sonnet). Read [docs/dev/haiku_task_guide.md](../../docs/dev/haiku_task_guide.md) Part 2 before starting.

## Goal

Add `agent_review_present` tagging to the 485 already-collected candidate PRs in
`experiments/agentic_weak_review/agentic_pr_candidates.jsonl`, and probe which AI-review-bot
logins are searchable via `gh search prs --reviewed-by`. No new corpus collection. No archcheck runs.

## Resolved facts (all checked 2026-07-06 — do not re-verify, do not second-guess)

- `experiments/agentic_weak_review/agentic_pr_candidates.jsonl` exists locally, **485 lines**,
  one JSON object per line. The directory is gitignored local research output — that is intentional.
  If the file is missing: **STOP and report**, do not regenerate.
- Every line has fields `pr_url`, `bot_review_count`, `human_review_count`, `human_comment_count`
  (full field list: run `head -1 ... | python3 -m json.tool`).
- **All 485 lines have `bot_review_count == 0`, and this is wrong.** Cause:
  `count_reviews()` in `experiments/agentic_weak_review/collect_candidates.py` skips reviews with
  `state == "COMMENTED"`, and AI review bots post almost exclusively COMMENTED reviews.
- Verified example: `gh pr view https://github.com/ghpascon/X714/pull/3 --json reviews` returns
  exactly one review: author login `copilot-pull-request-reviewer`, state `COMMENTED`.
- `gh` CLI is installed and authenticated; `gh search prs --reviewed-by "coderabbitai[bot]"` works.
- `python3` is available.

## Traps (read before writing any code)

1. **Do NOT reuse `is_bot_author()` from collect_candidates.py to detect review bots.**
   In `gh pr view --json reviews` output the reviewer login comes **without** the `[bot]` suffix
   (verified: `copilot-pull-request-reviewer`, not `copilot-pull-request-reviewer[bot]`), so
   `is_bot_author()` classifies it as human. Match against the explicit roster below instead.
2. **Do NOT skip COMMENTED reviews** when detecting agent review presence. COMMENTED is the
   normal state for AI review bots.
3. **Rate limits.** The first collection pass hit GitHub secondary rate limits. Therefore:
   sleep ≥ 0.7 s between `gh pr view` calls, write output incrementally after each PR, and on
   restart skip PRs whose `pr_url` is already in the output file. 485 calls ≈ 6–8 minutes; that is expected.
4. In `gh search prs`, `--reviewed-by "Copilot"` (display name) returns an error. Only
   `[bot]`-suffixed logins work in search (this is the opposite of trap 1 — search wants the
   suffix, `pr view` output omits it). Match roster logins case-insensitively and ignore an
   optional `[bot]` suffix.

## Review-bot roster (the allowlist; exact, do not extend without asking)

`copilot-pull-request-reviewer`, `coderabbitai`, `gemini-code-assist`, `qodo-merge-pro`,
`sourcery-ai`, `greptile-apps`, `ellipsis-dev`, `korbit-ai`, `cubic-dev-ai`,
`amazon-q-developer`, `sweep-ai`, `claude`.

A login matches the roster if, after stripping an optional trailing `[bot]` and lowercasing,
it equals a roster entry. `github-actions` is deliberately NOT in the roster (CI noise).

## Detailed instruction

### Step 1 — enrichment script

Create `experiments/agentic_weak_review/enrich_reviews.py` (new file; the experiments dir is
not subject to src/ code-quality file limits, but keep functions ≤ 30 lines). The script must:

1. Read `agentic_pr_candidates.jsonl` (485 lines).
2. For each PR: run `gh pr view <pr_url> --json reviews,comments`.
3. Compute and add these fields to the record (keep ALL existing fields unchanged):
   - `agent_reviewer_logins`: sorted unique roster-matching logins found among review authors
     AND comment authors (list of strings, may be empty);
   - `agent_review_present`: `true` iff `agent_reviewer_logins` is non-empty;
   - `human_gate_review_count`: count of reviews with state `APPROVED` or `CHANGES_REQUESTED`
     whose author does NOT match the roster and does not end with `[bot]` and does not start
     with `app/`;
   - `enrich_error`: string, only present if the `gh pr view` call failed for this PR
     (record the error, keep the line, continue).
4. Append each enriched record to `agentic_pr_candidates_enriched.jsonl` immediately
   (incremental write, resume-safe per trap 3). Do NOT modify or overwrite the input file.
5. Print a final summary line: total, `agent_review_present` count, error count.

### Step 2 — run the enrichment

Run the script to completion. If more than 50 PRs end with `enrich_error`, STOP and report
(likely rate limiting; do not improvise workarounds).

### Step 3 — reviewer-login search sweep

For each of these 8 unverified logins, run
`gh search prs --merged --language c++ --reviewed-by "<login>" --limit 5 --json url` :

`qodo-merge-pro[bot]`, `sourcery-ai[bot]`, `greptile-apps[bot]`, `ellipsis-dev[bot]`,
`korbit-ai[bot]`, `cubic-dev-ai[bot]`, `amazon-q-developer[bot]`, `claude[bot]`.

Record per login: `works` / `error <message>` and the number of results (0–5). Sleep ≥ 2 s
between queries. Append the results as a new section `## Reviewer-login sweep (173a)` in
`experiments/agentic_weak_review/notes.md`, one line per login.

### Step 4 — report

Follow the report format from haiku_task_guide.md §2.4, including the control-case table below
with expectation vs fact. Then WAIT — do not commit.

## Control cases (contract — check every row before declaring readiness)

| Check | Expected |
|-------|----------|
| Input line count | 485 |
| Output line count (excluding pure duplicates on resume) | 485 |
| Input lines with `bot_review_count > 0` | 0 |
| `ghpascon/X714/pull/3` | `agent_review_present == true`, logins contain `copilot-pull-request-reviewer` |
| `ghpascon/X714/pull/2` | `agent_review_present == true` |
| `ImagingTools/Agentino/pull/48` | `agent_review_present == true` |
| `ImagingTools/Puma/pull/37` | `agent_review_present == true` |
| `slafftrosheen/m1m1r.kit/pull/5` | `agent_review_present == true` |
| Total `agent_review_present == true` | ≥ 5 (record the exact number in the report) |
| `enrich_error` count | ≤ 50 |
| Sweep: `coderabbitai[bot]` control query | works, ≥ 1 result (already verified; rerun as sweep sanity check) |

## Definition of done

All control-case rows match. `agentic_pr_candidates_enriched.jsonl` has 485 valid JSON lines.
`notes.md` has the sweep section with 9 lines (8 unverified + 1 control). Report delivered. No commit.

## Do NOT do

- Do NOT re-run `collect_candidates.py` or any new corpus collection.
- Do NOT modify `agentic_pr_candidates.jsonl` or `collect_candidates.py`.
- Do NOT run `archcheck` on anything.
- Do NOT extend the roster or add search queries beyond the 9 listed.
- Do NOT touch anything outside `experiments/agentic_weak_review/` and this task file.
- Do NOT commit.

## Changed files

| File | Change |
|------|--------|
| `experiments/agentic_weak_review/enrich_reviews.py` | new script (gitignored) |
| `experiments/agentic_weak_review/agentic_pr_candidates_enriched.jsonl` | new output (gitignored) |
| `experiments/agentic_weak_review/notes.md` | sweep section appended (gitignored) |
| `backlog/new/173a_maj_agent_review_enrichment.md` | this task |
