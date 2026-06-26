# C++ drift under agentic development — report

**Date:** 2026-06-06. **Method:** controlled event-study, no hypothesis-pandering.

## Question
Does switching to agentic (AI) development change commit density and structural debt
(cycles, copy-paste) in C++ OSS repositories — and more so than in ordinary (human) repos.

## Method
- **Agentic:** 100 C++ repos with AI trailers in ≥20 of 100 commits (`Co-authored-by: Claude/Copilot/...`),
  >300 commits since May 2025, created before 2024-06 (there is a "before"), not size giants.
- **Control:** 50 non-agentic C++ repos (human message length, no trailers), same era.
- Per repo: clone (shallow since 2024, with blobs), `git log` → adoption date = first AI trailer;
  commits/lines/files in the window **90 days BEFORE vs 90 days AFTER** adoption (for control — placebo date 2025-09-01);
  archcheck (cycles, copy-paste) on the snapshot **before adoption** and **now**.
- **Honesty rule:** into the statistics only `commits_before ≥ 10` (there is a real "before");
  born-agentic (no "before") — as a separate counter, NOT in the conclusions.
- Tools: `experiments/ai_repo_run/unified_metrics.py` (collection), `analyze_unified.py` (analysis).
  Raw data line by line: `experiments/ai_repo_run/unified_metrics.tsv` (150 repos, all metrics).

## Funnel
150 repos → 44 filtered out by size (asset giants) → 13 born-agentic (no "before") →
**61 agentic + 31 control** reliable.

## Results (difference-in-differences)

| metric | agentic (n=61) | control (n=31) | conclusion |
|---|---|---|---|
| commits after/before | **1.61×** (sharp ≥2×: 37%, no speedup <1.3×: 40%) | 1.00× (12% / 67%) | AI sharply drives activity |
| lines per commit | 190 → **306** (median ratio 1.44) | 132 → 130 (0.93) | commits are larger with AI |
| files per commit | 4.84 → 4.2 | 3.53 → 3.58 | no difference |
| share of repos where **cycles grew** | 3% (2/60) | 6% (2/31) | AI does NOT breed cycles |
| share of repos where **copy-paste grew** | 38% (22/57), Δmed 0 | 32% (10/31), Δmed 0 | almost no difference |

## Conclusions (objectively)
1. **Activity soars with AI.** +61% commits and +44% lines per commit after adoption — against
   a FLAT control (1.00×). Clean diff-in-diff: an AI effect, not a general trend.
2. **Structural debt does NOT grow faster than for humans.** Cycles flat (3% vs 6%), copy-paste
   barely differs (38% vs 32%, median delta 0). In C++, AI pours in more and larger
   code, but cycles/dupes do not flare up relative to the control.

## What this means for the "C++ is more disciplined" hypothesis
The data **rather supports** it on the short horizon: there is volume growth, but no structural breakdown.
**But one cannot claim "C++ is immune to AI drift":**
- The "after" window is often short (repos adapted in 2025-26), and structural debt accumulates slowly —
  "did not flare up" may mean "still too early".
- archcheck thresholds are conservative (v0.1); some subtle drift may go uncaught.
- The sample is 61 reliable agentic repos; the long tail needs a re-run in 6-12 months.

## Related findings
- Cross-language (8 languages, 2300 repos): density flat in the population, growth is the agentic TAIL
  (see `experiments/ai_repo_run/crosslang/`).
- Literature (METR, DORA 2025, GitClear, "Debt Behind the AI Boom"): per-repo velocity ≠
  adoption; the velocity surge is transient, debt accumulates — consistent with points 1-2 above.
