# 187 — Calibrate the FUNC.COGNITIVE_COMPLEXITY default threshold on a stable corpus snapshot

**Type:** research
**Status:** new
**Related:** #101 (drift local-complexity), #102 (drift corpus prototype), #184 (adoption cohort)

## Why

The absolute per-function cognitive-complexity advisory `FUNC.COGNITIVE_COMPLEXITY`
shipped (commits `b2f1ff1`/`698f3c1`/`03f5e29`) with a **corpus-calibrated default of 40**.
The calibration was a one-off session run and is not yet a reproducible artifact. This task
formalizes it: a committed script + a research write-up, so the threshold choice is defensible
and re-runnable when the corpus grows.

## What was already measured (2026-07-14, informal)

Ran the shipped binary at `function_cognitive_complexity: 1` over the #184 cohort
(`experiments/adoption_event/cohorts.tsv`) — HEAD scan only, no `--diff`, so no git orphans.
510 C++ repos actually scanned (626 cohort paths, 512 on disk, 2 non-C++), 1,141,580 functions.

Per-function score: mean 8.5, median 3, p90 18, p95 30, p99 84, max 7987.

| threshold | % of all funcs | % repos w/ ≥1 | median/repo |
|---|---|---|---|
| 25 (clang-tidy) | 6.70% | 98.6% | 73 |
| 30 (p95) | 5.21% | 98.2% | 59 |
| **40 (shipped)** | **3.40%** | **95.9%** | **37** |
| 50 | 2.41% | 92.7% | 27 |

Verdict: as a **gate** this is catastrophic (median dozens/repo) → advisory-only vindicated.
Tail is real (hand-verified: FlashCpp `try_instantiate_class_template` = 7705, a multi-thousand
line function), not a scorer artifact. Determinism confirmed (re-scan reproduced counts).

## To do

- [ ] Commit a reproducible harness under `experiments/` (HEAD scan per repo, parse
      `FUNC.COGNITIVE_COMPLEXITY` scores, aggregate the distribution). NOTE: `--config` and
      `--format json` cannot be combined (see `backlog/DEBT.md` 2026-07-14) — either fix that
      first or parse text output.
- [ ] Re-run on a **frozen** corpus list (the shared `/home/localadm/oss` is mutated by parallel
      sessions — 37 repos vanished mid-run this time; snapshot the repo list + commit SHAs first).
- [ ] Confirm or revise the default 40; write up in `docs/research/`.
- [ ] Decide whether to filter `TEST_F`/`BENCHMARK` bodies when scanning a `tests/` tree.

## Acceptance

A committed script reproduces the distribution table above (±noise) from a pinned repo list, and
a short research doc records the threshold rationale.
