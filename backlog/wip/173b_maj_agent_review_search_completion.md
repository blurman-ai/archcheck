# [RESEARCH][CORPUS] 173b: complete channel searches, enrich new PRs, C++ file-level repo selection

**Creation date:** 2026-07-06
**Status:** wip
**Start date:** 2026-07-06
**Module:** RESEARCH][CORPUS
**Priority:** major
**Difficulty:** medium (mechanical but long-running; all decisions pre-made)
**Blocks:** #173 repo selection / clone phase
**Blocked by:** —
**Related:** #173 (parent), #173a (completed pattern to follow)
**Executor:** junior model (Haiku/Sonnet). Read [docs/dev/haiku_task_guide.md](../../docs/dev/haiku_task_guide.md) Part 2 before starting.

## Goal

Three things, in order:

1. Complete the Channel 1 search matrix and rerun the broken Channel 3 (config-file) search.
2. Enrich every discovered PR (2,274 existing + new ones) with reviews/comments/**file paths**,
   and add file paths to the 485 seed PRs.
3. Build a per-repo selection table ranked by **C++ source files changed in agent PRs** —
   repos where agents touch only CMake/docs/CI must sink (user directive 2026-07-06).

## Resolved facts (checked 2026-07-06 — do not re-verify, do not second-guess)

- Inputs in `experiments/agentic_weak_review/` (gitignored, intentional):
  `agentic_pr_candidates_enriched.jsonl` (485 lines, has `agent_review_present` etc.),
  `search_results_new_prs.jsonl` (2,274 lines, each `{"pr_url": ...}` only),
  `search_channels.py`, `enrich_reviews.py` (working roster logic — reuse it).
  Any input missing → STOP and report.
- Channel 1 queries actually executed so far: `coderabbitai×C` (hit the 1000 cap, truncated),
  `cubic-dev-ai×C++` (hit the cap), `korbit-ai×C++`, `korbit-ai×C`, `ellipsis-dev×C`. Nothing else.
- `gh search prs --reviewed-by` works for: `coderabbitai[bot]`, `copilot-pull-request-reviewer[bot]`,
  `gemini-code-assist[bot]`, `sourcery-ai[bot]`, `greptile-apps[bot]`, `ellipsis-dev[bot]`,
  `korbit-ai[bot]`, `cubic-dev-ai[bot]`, `amazon-q-developer[bot]`, `claude[bot]`.
  `qodo-merge-pro[bot]` does NOT exist — do not query it.
- `gh search code --filename .coderabbit.yaml --limit 5 --json repository` returns 5 results
  (verified twice on 2026-07-06).
- Control PR file data (verified via `gh pr view --json files,changedFiles`):
  - `ghpascon/X714/pull/3` → 3 files, **3** C++-source;
  - `ImagingTools/Puma/pull/37` → 9 files, **0** C++-source (all CMakeLists.txt).
- Seed∩new dedup verified: 0 overlapping `pr_url`.

## Traps (all real, all hit before)

1. **`gh search code` WITHOUT `--json` prints NOTHING in a non-TTY run and exits 0.**
   This produced a false "Channel 3: 0 results" on 2026-07-06. Always pass
   `--json repository,path` and parse JSON.
2. **`gh pr view` review/comment author logins come without the `[bot]` suffix**
   (`copilot-pull-request-reviewer`, not `...[bot]`). Match the roster by stripping an
   optional `[bot]` and lowercasing — reuse the matching from `enrich_reviews.py`,
   do NOT use `is_bot_author()` from `collect_candidates.py`.
3. **Search caps at 1000 results per query.** Any query returning exactly 1000 is truncated:
   re-run it sliced by `--created` windows: `<2024-01-01`, `2024-01-01..2024-12-31`,
   `2025-01-01..2025-06-30`, `2025-07-01..2025-12-31`, `2026-01-01..2026-12-31`.
   A slice that still returns 1000 → halve the window and note it; do not silently accept truncation.
4. **Rate limits.** ~3,000 `gh pr view` calls total. Sleep ≥ 0.7 s between PR views,
   ≥ 2 s between searches. Write output incrementally after each item; on restart skip
   `pr_url`s already present in the output file. If HTTP 403/429 persists → sleep 300 s and
   continue; after 3 such stalls STOP and report progress.
5. **C++-source extension list** (reuse `SOURCE_EXTS` from `collect_candidates.py`):
   `.c .cc .cpp .cxx .h .hh .hpp .hxx .ino`, lowercase match. CMakeLists.txt, `.cmake`,
   `.md`, `.yml` are NOT source.

## Detailed instruction

### Step 1 — fix `search_channels.py`

- Channel 3: add `--json repository,path` and JSON parsing (trap 1).
- Add `--created` slicing for capped queries (trap 3).
- Make all channels append-and-resume: never overwrite existing `search_results_*.jsonl`;
  skip queries whose results are already recorded.

### Step 2 — complete the Channel 1 matrix

Run the missing queries
(`gh search prs --merged --language <lang> --reviewed-by "<login>" --limit 1000 --json url`):

`coderabbitai×C++`; `copilot-pull-request-reviewer×{C++,C}`; `gemini-code-assist×{C++,C}`;
`sourcery-ai×{C++,C}`; `greptile-apps×{C++,C}`; `ellipsis-dev×C++`;
`amazon-q-developer×{C++,C}`; `claude×{C++,C}`; `cubic-dev-ai×C`.
Also re-run the two known-truncated queries (`coderabbitai×C`, `cubic-dev-ai×C++`) with
date slicing. Append per-query result rows (one JSON per URL, fields
`pr_url, reviewer, language, created_window`) to `search_results_channel1.jsonl`.

### Step 3 — rerun Channel 3

Filenames: `.coderabbit.yaml`, `.coderabbit.yml`, `.pr_agent.toml`, `.sourcery.yaml`,
`.korbit-config.yaml`. For each found repo run `gh repo view --json languages` (cache per repo)
and keep only repos with ≥ 5% C-family bytes (reuse `c_family_share()` from
`collect_candidates.py`). Write kept repos to `search_results_channel3.jsonl`
(`repo, config_file`). Expect a small set; 0 total is NOT acceptable (see control table).

### Step 4 — consolidate

Rebuild `search_results_new_prs.jsonl`: all Channel 1+2 URLs, dedup by `pr_url`, minus the
485 seed URLs. Keep the one-object-per-line format but include `reviewer`/`language` where known.

### Step 5 — enrich

Create `enrich_full.py` (extend the pattern of `enrich_reviews.py`; functions ≤ 30 lines):

- For every consolidated new PR: one call
  `gh pr view <url> --json author,title,createdAt,mergedAt,additions,deletions,changedFiles,reviews,comments,files`.
  Compute: `agent_reviewer_logins`, `agent_review_present`, `human_gate_review_count`,
  `human_comment_count`, `bot_comment_count`, `merge_latency_minutes`,
  `cpp_files_changed` (count of file paths ending in SOURCE_EXTS),
  `touches_cpp_source` (bool), `agent_authored` (author matches the agent-author roster from
  #173 §Agent-authored), `enrich_error` on failure. Drop PRs that are not merged.
  Per-repo language filter ≥ 5% C-family (cache; drop the PR otherwise, count drops).
  Output: `new_prs_enriched.jsonl`, incremental, resume-safe.
- For the 485 seed PRs: fetch only `files`, add `cpp_files_changed` + `touches_cpp_source`
  to each record from `agentic_pr_candidates_enriched.jsonl`, write
  `agentic_pr_candidates_enriched_v2.jsonl` (485 lines; do not modify inputs).

### Step 6 — per-repo selection table

Create `build_repo_selection.py` → `repo_selection.tsv`, one row per repo, columns:

`repo, agent_pr_count, agent_prs_touching_cpp, total_cpp_files_changed_in_agent_prs,
median_cpp_files_per_agent_pr, weak_gate_pr_count, agent_review_only_pr_count,
human_reviewed_pr_count, c_family_share`

computed over seed v2 + new enriched, `agent_authored` PRs only for the `agent_*` columns.
Sort by `agent_prs_touching_cpp` desc, then `total_cpp_files_changed_in_agent_prs` desc.
This is the input for the clone phase (#173 §Corpus storage layout) — do NOT clone anything here.

### Step 7 — rebuild summaries and report

Regenerate `CORPUS_SUMMARY.yaml` and `top_agent_reviewed_repos.json` from enriched data only
(they are currently built on raw URLs — state this in the report). Update `notes.md` counts.
Report per haiku_task_guide §2.4 with the control table below. Then WAIT — do not commit.

## Control cases (contract)

| Check | Expected |
|-------|----------|
| Channel 3 rerun, `.coderabbit.yaml` raw hits | ≥ 5 repos before language filter |
| Channel 3 total after language filter | > 0 OR every dropped repo listed with its language share |
| `coderabbitai×C++` query | > 0 results (currently absent from data) |
| Seed v2 line count | 485 |
| `ghpascon/X714/pull/3` in seed v2 | `cpp_files_changed == 3`, `touches_cpp_source == true` |
| `ImagingTools/Puma/pull/37` in seed v2 | `cpp_files_changed == 0`, `touches_cpp_source == false` |
| Consolidated new PRs | no duplicate `pr_url`; 0 overlap with seed |
| `enrich_error` rate | ≤ 10% of attempted PRs |
| `repo_selection.tsv` | `ghpascon/X714` present, `agent_prs_touching_cpp ≥ 1` |
| `repo_selection.tsv` sanity | a repo whose agent PRs are CMake/docs-only ranks below any repo with ≥ 1 cpp-touching agent PR |

## Definition of done

All control rows match. `repo_selection.tsv` exists and is sorted as specified.
Summaries regenerated from enriched data. Report delivered with the control table
(expectation vs fact). No commit, no clones, no archcheck runs.

## Do NOT do

- Do NOT run `archcheck`; do NOT clone repositories.
- Do NOT modify input files in place (`agentic_pr_candidates*.jsonl`, `search_results_new_prs.jsonl` may be rebuilt only as described in Step 4).
- Do NOT extend the reviewer/author rosters.
- Do NOT query `qodo-merge-pro[bot]`.
- Do NOT touch anything outside `experiments/agentic_weak_review/` and this task file.
- Do NOT commit.

## Changed files

| File | Change |
|------|--------|
| `experiments/agentic_weak_review/search_channels.py` | fix channel 3 --json, add slicing/resume |
| `experiments/agentic_weak_review/enrich_full.py` | new enrichment script |
| `experiments/agentic_weak_review/build_repo_selection.py` | new aggregation script |
| `experiments/agentic_weak_review/new_prs_enriched.jsonl` | new output |
| `experiments/agentic_weak_review/agentic_pr_candidates_enriched_v2.jsonl` | seed + file data |
| `experiments/agentic_weak_review/repo_selection.tsv` | new selection table |
| `backlog/new/173b_maj_agent_review_search_completion.md` | this task |

## Report (2026-07-07, complete)

All 7 steps done. Full detail and final numbers: `experiments/agentic_weak_review/CORPUS_SUMMARY.yaml`
(rewritten from enriched data, not raw URLs). Two real bugs found and fixed mid-run (both fixed
immediately, pipeline resumed with zero data loss — see `notes.md` "Channel 1-3 completion + full
enrichment" section for detail):

1. `search_channels.py` v1 stored `gh search prs --json url` output (a single-line JSON array) as
   one string inside `pr_url` instead of parsing it — silently corrupted all Channel 1/2 data.
2. `enrich_full.py` crashed on PRs where `gh pr view --json files` (or `reviews`/`comments`)
   returns an explicit JSON `null` instead of an empty array — `dict.get(key, [])` doesn't catch
   that. Fixed with `data.get(key) or []` everywhere. Crashed once at ~5850/10063 new PRs
   (2026-07-06 21:28), fixed, resumed clean.

### Control cases: expectation vs fact

| Check | Expected | Fact | Result |
|-------|----------|------|--------|
| Channel 3 rerun, `.coderabbit.yaml` raw hits | ≥ 5 repos before language filter | **99** repos | PASS |
| Channel 3 total after language filter | > 0 OR every dropped repo listed with share | **24** kept (1+6+9+8+0 across 5 patterns) | PASS |
| `coderabbitai×C++` query | > 0 results | **1000** (capped — see caveat below) | PASS |
| Seed v2 line count | 485 | **485** | PASS |
| `ghpascon/X714/pull/3` in seed v2 | `cpp_files_changed==3`, `touches_cpp_source==true` | **3 / true** | PASS |
| `ImagingTools/Puma/pull/37` in seed v2 | `cpp_files_changed==0`, `touches_cpp_source==false` | **0 / false** | PASS |
| Consolidated new PRs | no duplicate `pr_url`; 0 overlap with seed | **0 duplicates, 0 overlap** (16,664 unique) | PASS |
| `enrich_error` rate | ≤ 10% of attempted PRs | **15/16,664 = 0.09%** | PASS |
| `repo_selection.tsv` | `ghpascon/X714` present, `agent_prs_touching_cpp ≥ 1` | present, **3** | PASS |
| `repo_selection.tsv` sanity | CMake/docs-only agent repos rank below any cpp-touching repo | all 13 zero-cpp repos sit strictly below all cpp-touching repos | PASS |

**All 10 control cases pass.**

### Final scale

- Seed: 485 PRs / 29 repos (100% `app/copilot-swe-agent`-authored).
- New PRs found (Channel 1+2, dedup'd, 0 overlap with seed): **16,664** (up from 2,274 in the
  pre-173b broken run — the parsing-bug fix alone recovered ~7x more data).
- Total usable enriched PRs: **16,982** across **1,636** distinct repos.
- `repo_selection.tsv`: 49 repos with agent-authored PRs, ranked by real C++ file touch.
- `top_agent_reviewed_repos.json`: 1,620 repos have at least one agent-reviewed PR (any author);
  top 5 are `deepin-community/kernel` (747), `lvgl/lvgl` (657),
  `Oichkatzelesfrettschen/mesa-26-gororoba` (494), `rsyslog/rsyslog` (461),
  `skyrim-multiplayer/skymp` (398).

### Known limitations (documented, not blocking)

- 3 reviewer×language combos (`coderabbitai×c++`, `greptile-apps×c++`, `claude[bot]×c++`) never
  got their `--created`-window slices to succeed — every window hit a persistent secondary rate
  limit during this run. Their counts are undercounts. Re-running just these 3 combos later would
  recover more data; does not affect the acceptance bar.
- `gh pr view` with the heavy field set measured at ~2.3-3.7s/call (TLS handshake per process, not
  rate-limit throttling — GraphQL quota stayed under 15% used the whole run). Budget ~20-25
  PRs/min for any future full re-enrichment of a corpus this size.
- `search_results_new_prs.jsonl` rows are reviewer-identity matches, not necessarily
  agent-authored — `agent_authored` field on each `new_prs_enriched.jsonl` row disambiguates.
  `repo_selection.tsv` (agent-authored only) is the correct input for the clone/archcheck phase;
  `top_agent_reviewed_repos.json` (any author) answers a different question (review-bot presence).

No commit made. No clones, no archcheck runs, per Do NOT do.
