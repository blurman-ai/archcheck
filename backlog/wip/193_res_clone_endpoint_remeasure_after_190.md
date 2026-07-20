# [RES] 193: re-measure the clone endpoint after #190 — is the old undercount uniform or size-correlated?

**Created:** 2026-07-19
**Start date:** —
**Status:** new
**Module:** SCAN/DUPLICATION + research corpus
**Priority:** critical
**Difficulty:** large
**Blocks:** any further citation of `n_newclone` in #180/#182/#184/#185 write-ups; the launch post's clone numbers
**Blocked by:** #190 must be committed and a stable binary built (currently uncommitted working tree)
**Related:** #190 (the fix), #184 (full-endpoint study), #180/#182 (event studies), #185 (review collapse), #131 (fresh corpus remeasure — precedent)

## Goal

Decide whether the published clone-endpoint numbers survive the #190 fix, by re-measuring
`n_newclone` on a **sample** of already-measured (repo, sha) pairs with the fixed binary and
comparing per-commit against the cached pre-fix values.

The question is NOT "did counts go up" (they will). It is:

> **Is the pre-#190 undercount uniform, or does it correlate with a repo/commit property —
> above all mean function size?**

A uniform shift rescales an endpoint and leaves comparisons intact. A size-correlated shift
biases every cross-repo and cross-cohort comparison, because the tool undercounted
large-function projects more (JOURNEY 2026-07-18: "чем крупнее функции в проекте, тем сильнее
недосчёт... любое сравнение «в проекте A дублирования больше, чем в B» могло быть артефактом
среднего размера функции").

## Scope: exactly one endpoint is affected

#190 changed the fragmenter only. Verified by inspection of the diff (2026-07-19): touched
`fragmenter.{h,cpp}`, `clone_index.cpp`, `duplication_scanner.cpp` — the duplication subsystem
only. Therefore:

- **Affected:** `n_newclone` (and `P1_union`, which contains it).
- **NOT affected:** `n_complexity`, `n_satd`, `n_test`, `n_flag_arg`, `n_bool_field`,
  `n_bool_struct`, `n_added_edges`, `n_removed_edges`, `n_grown_cycles`,
  `n_new_god_headers`, `chain_length_grown`, `nccd_delta`, `graph_drift`.

**Do NOT re-run the whole panel.** Re-run the clone endpoint on a sample; leave the other
endpoints' cached values untouched (re-running them would only add noise and cost).

Consistency check that this claim holds: for the sampled commits, every non-clone counter
must come back **identical** to the cached value. Any drift there means the scope claim is
wrong and the task stops for re-scoping (see Definition of done).

## Resolved facts (verified 2026-07-19 — do not re-derive)

- **The #190 fix is uncommitted** in `/home/localadm/projects/cpparch`. This task must not
  start until it is committed and a Release-or-Debug binary is built and pinned by hash.
  Record the binary's commit sha in the findings — the whole point is comparability.
- Cached pre-fix per-commit results (JSONL, one object per (repo, sha)) with the exact
  schema this task compares against:
  - `experiments/adoption_event/results_phase3.jsonl` — 14,933 rows
  - `experiments/adoption_event/results_mined.jsonl` — 9,516 rows
  - keys include `repo, sha, elapsed, exit, status, gate, n_newclone, n_complexity,
    n_satd, n_test, n_flag_arg, n_bool_field, n_bool_struct, n_added_edges,
    n_removed_edges, n_grown_cycles, n_new_god_headers, chain_length_grown, nccd_delta`
  - `experiments/adoption_event/p5/` — 3.0 GB of PR-level run state (#180 P5)
- **Reuse, do not rewrite, the runner**: `experiments/per_commit/run_worklist.py`, whose
  `categorize(doc)` maps `archcheck --diff --format=json` to those counters (`n_newclone` =
  count of `DRIFT.NEW_CLONE` in `advisory.violations`). It already handles per-commit
  timeout, root/merge commits, bad JSON, and — importantly — runs each child with
  `start_new_session=True` so a timeout kills the whole process group instead of orphaning
  git (known hazard, see Hygiene).
- `experiments/adoption_event/p5_run.py` imports the same `categorize` from
  `run_worklist` — one mapping, do not fork a second one.
- Published tables that cite the clone endpoint and would need amendment if the shift is
  size-correlated: `docs/research/all_endpoints_event_study.md` (#184; `n_newclone` DiD
  **1.37**, `n_newclone` non-zero share 24%, 17,076 units), plus #180/#182/#185 write-ups
  that reuse `P1_union`.
- Corpus (~2042 entries) is on local NVMe at `/home/localadm/oss/`; git history is a cheap
  local primitive here.
- **`MicSm_duckdb` added to the corpus 2026-07-19** (`/home/localadm/oss/MicSm_duckdb`,
  `https://github.com/MicSm/duckdb`, 78787 commits, `gc.auto=0` already set). It is Mike
  Semikov's duckdb fork and is **distinct from the existing `duckdb_duckdb`** corpus entry
  (upstream): it carries branch `origin/refactor-pipeline_executor` with `a1b3ffc9`
  ("ParselFire + Claude Sonnet 4.6 refactoring"), which upstream does not have. Verified:
  `git cat-file -t a1b3ffc97f` resolves in the fork, fails in `duckdb_duckdb`.

## Sampling plan (this is the "выборочно" part — do this before anything wider)

**Stage A — pilot, ~300 commits.** Draw from `results_phase3.jsonl` + `results_mined.jsonl`,
stratified so the size hypothesis is actually testable:

- stratify by **mean function size of the repo** (proxy, computed once per repo from the
  baseline tree — e.g. median tokens per emitted fragment, or median function NLOC via
  `lizard`); 3 strata: small / medium / large;
- within each stratum, sample both commits with cached `n_newclone == 0` and `> 0`
  (the zeros matter most: that is where new findings appear);
- record the sampling seed and the exact row selection in the findings.

**Stage A also includes the `MicSm_duckdb` known-good commits** as a positive control, since
their expected behaviour is established by hand (#190):

| commit | expectation with the fixed binary |
|---|---|
| `c28ce15a65` (clone introduced) | `n_newclone` 4 → **6**; the added pair is `pipeline_executor.cpp:72-84 ↔ 18-27` |
| `e884eef8..a1b3ffc9` (clone removed, Mike's refactor) | `n_newclone` **0** — gate silent, by design |
| `a1b3ffc9..e884eef8` (reverse) | `n_newclone` ≥ 1, includes `lines 20-29 - clone of 74-87` |

If these three do not reproduce, the harness is wrong — stop before trusting any sampled
number. This is the "validate by a positive marker" rule applied to the run as a whole.

**Stage B — widen only if Stage A shows a size-correlated shift.** If Stage A shows a
uniform shift, say so and stop; a re-run of the full panel is then not justified by the
evidence, and the write-ups need a scale note rather than new numbers.

## Method

1. Commit #190; build; pin and record binary sha. Verify on the known cases first:
   `--duplication /home/localadm/oss/MicSm_duckdb/src/parallel` at `e884eef8` must report
   `pipeline_executor.cpp:23-28 <-> 82-86`; monit sub-function recall 6/8.
2. Build the sample worklist (repo, sha) as above; store it as a TSV **in the repo**
   (`experiments/clone_remeasure_193/worklist.tsv`) so the run is reproducible.
3. Re-run with the fixed binary via `run_worklist.py` (or the thinnest wrapper around it —
   do NOT write a new runner).
4. Join old vs new per (repo, sha). Produce:
   - distribution of `Δn_newclone` overall;
   - `Δ` broken down by function-size stratum — **the central table**;
   - the non-clone-counter identity check (must be zero drift);
   - the share of commits that flipped `0 → >0` (these change the "non-zero share" and
     hence event-study power).
5. **Hand-verify at least 10 newly-appeared clone findings** across strata: read the code,
   classify TP/FP. A count that rose is worthless if the additions are noise. Report the
   TP/FP split with the sample size stated.
6. Re-estimate the #184 DiD for `n_newclone` on the sampled subset only if Stage B runs;
   for Stage A report shift statistics, not a new causal estimate.

## Hygiene (hard-won, non-negotiable)

- **Validate every run by a positive marker**, never by absence of error. A failed
  archcheck run and a clean commit both yield all-zero counters. `run_worklist.py` records
  `status`/`exit` — treat anything but the ok-status as ERROR, never as 0. (This exact trap
  produced a false "28 of 30 commits clean" table on 2026-07-18.)
- **`git config gc.auto 0` in every scanned clone.** Background auto-gc corrupts
  `git cat-file --batch` reads that `--diff --diff-mode=memory` depends on; the failure is
  silent and looks like a clean result.
- **≤ 8 workers**, and expect ~1 GB peak RSS per concurrent `--diff` (measured 2026-07-19:
  839 MB pre-#190, 1159 MB post-#190 on a 3438-node tree). Concurrency starved runs on this
  machine before: 8 of 9 failures passed when re-run alone.
- Kill the **process group** (`start_new_session=True` + `killpg`), never a bare PID —
  `pkill` is blocked; use `os.kill` by PID group.
- Suspend `baloo` for the duration. The user is at the machine: keep the box usable.
- Store the run ledger (attempted / ok / error / skipped) alongside results; append, never
  overwrite.

## Definition of done (checkable without judgment)

- [ ] Binary sha recorded; the two known-case checks (duckdb pair, monit 6/8) pass with it.
- [ ] `experiments/clone_remeasure_193/worklist.tsv` committed; seed and strata recorded.
- [ ] Stage A completed with ≥ 95% of sampled commits in ok-status; every non-ok row
      classified as ERROR (never counted as 0).
- [ ] **Non-clone counters identical** to cached values on every ok row. Any mismatch →
      status `blocked`, report, stop (the scope claim is then wrong).
- [ ] Central table produced: `Δn_newclone` by function-size stratum, with n per cell.
- [ ] ≥ 10 newly-appeared findings hand-classified TP/FP, split reported with sample size.
- [ ] Explicit verdict on the central question: **uniform** vs **size-correlated** shift,
      with the statistic that supports it.
- [ ] A written recommendation for each affected write-up (#184 / #180 / #182 / #185):
      "numbers stand", "numbers stand + add scale note", or "numbers must be re-estimated".
- [ ] Closure verdict per `docs/dev/haiku_task_guide.md` §2.5 with cited evidence.

## Do NOT

- Do NOT re-run the non-clone endpoints; do NOT overwrite the cached JSONL files (write new
  files; the old ones are the comparison baseline).
- Do NOT edit any published `docs/research/*.md` in this task — this task produces the
  recommendation; amending the write-ups is a separate, explicitly commanded step.
- Do NOT widen to the full panel before Stage A answers the uniform-vs-correlated question.
- Do NOT write a second runner or a second `categorize()`.
- Do NOT report a count shift without the hand-verified TP/FP sample beside it.
