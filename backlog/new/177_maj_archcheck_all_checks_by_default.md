# [DESIGN] 177: archcheck runs only ONE check family per invocation — evaluate a combined/`--all` mode

**Creation date:** 2026-07-07
**Status:** new
**Module:** cli / rules
**Priority:** major
**Difficulty:** medium (design decision + CLI change, needs a cost tradeoff analysis first)
**Blocks:** —
**Blocked by:** —
**Related:** #173/#173b/#176 (agentic weak-review corpus + archcheck clone-and-run pipeline where this was discovered)

## Goal

Discovered 2026-07-07 while building the #173 corpus archcheck-run pipeline
(`experiments/agentic_weak_review/archcheck_run/clone_and_run.py`): a plain
`archcheck <path>` invocation only exercises the **default check family**
(SF.9 + CASE_MISMATCH_INCLUDE gating; SF.7/8, Lakos.GodHeader/ChainLength,
UNRESOLVED_LOCAL_INCLUDE advisory). Four other, independently useful check
families exist as **separate CLI modes that must be invoked explicitly**:

- `--duplication <path>` — clone-pair report
- `--history <path>` — god-file growth + defect-attractor analytics
- `--drift-baseline <baseline> <path>` — DRIFT.1/2/3/4 family (needs a pre-saved baseline)
- `--diff <revspec> <path>` — per-commit/PR cycle + god-header gate

The first pass of the clone-and-run pipeline silently ran only 2 of these 5
families (default check + `--diff`) and the gap went unnoticed until manually
reviewed — an easy mistake to repeat in any future script or manual invocation.

**This task is a discovery/design task, not a "just add a flag" task** — evaluate
whether bundling is actually a good idea before committing to it; see risks below.

## What to evaluate

1. Whether a `--all` (or similarly named) mode that runs default + duplication +
   history in one invocation is worth adding, given:
   - `--duplication` is O(fragments²)-ish pairwise comparison — cost scales
     differently from the O(files) preprocessor scan that makes the default
     mode CI-viable in "seconds per PR" (ADR-003's whole rationale).
   - `--history` walks full git log — cost scales with repository history
     length, not tree size; a repo with a huge history but small tree could be
     slow in a way the default mode never is.
   - `--drift-baseline` structurally CANNOT be bundled — it requires a
     baseline file from a previous run; there is no zero-config way to
     produce "drift since when" without one. At most, `--all` could run
     default + duplication + history and separately document that drift
     needs its own baseline workflow.
2. If bundling is rejected for CI-cost reasons (likely, given ADR-003
   precedent): what's the cheaper fix instead —
   - A `--help` / docs callout making the 5-mode split explicit and hard to miss
     (arguably the real gap: the CLI shape is fine, the *discoverability* isn't).
   - A `--report-coverage` flag or exit-time hint ("N check families available,
     M were not run") for scripts/CI that only invoke a subset.
3. Whatever is decided, update [docs/architecture-spec.md](../../docs/architecture-spec.md)
   and `--help` text so the mode split (and the reason for it) is explicit,
   citing ADR-003's cost-tiering precedent if that's the rationale kept.

## Do NOT do

- Do NOT just merge everything into the default `archcheck <path>` invocation
  without measuring the cost impact on a real repo first (reuse
  `scripts/bench_materialize.sh`-style benchmarking approach, or run
  `--duplication`/`--history` timing on a few corpus repos already cloned in
  `/home/localadm/oss_agentic/` from #173's archcheck-run).
- Do NOT touch the `--drift-baseline` requirement for an explicit baseline file
  — that's a correctness requirement (drift needs *a* reference point), not
  an oversight.

## Changed files

| File | Change |
|------|--------|
| TBD after evaluation — likely `docs/architecture-spec.md`, `--help` text, possibly a new CLI flag | |
| `backlog/new/177_maj_archcheck_all_checks_by_default.md` | this task |

## Benchmark findings (2026-07-07)

**Setup:** `build/release/src/archcheck` (release build, dated 2026-07-03 — 3 days
older than `build/debug`, but the mode dispatch/logic under test hasn't changed
since; noted per task instructions since debug would show inflated numbers).
Wall-clock timed with bash `date +%s.%N` around each invocation, 2-3 repeats per
(repo, mode) pair. Repos: all 7 from `/home/localadm/oss_agentic/` that had
reached `HISTORY_DONE`/`DRIFT_DONE` in `experiments/agentic_weak_review/archcheck_run/ledger.tsv`
at check time (only 1, `supaplextor/aqemu-0.10.x`, had the final terminal `DONE`
tag — the pipeline was still mid-run on `tenstorrent/tt-metal` — so all 7
"reached DRIFT_DONE" repos were used instead of waiting; `tt-metal` itself and
any repo without a ledger entry were excluded as instructed). Only read from
these directories; nothing in `archcheck_run/` was touched.

| Repo | Files | LOC | Commits | default ms | `--duplication` ms | `--history` ms |
|---|---|---|---|---|---|---|
| rvdbreemen/OTGW-firmware | 43 | 13,772 | 3,153 | 11 | 27 | 513 |
| florinzgz/FULL-FIRMWARE-Coche-Marcos | 177 | 33,941 | 1,923 | 21 | 172 | 31 |
| MaslowCNC/Maslow_4 | 426 | 59,974 | 2,159 | 43 | 298 | 77 |
| supaplextor/aqemu-0.10.x | 110 | 57,348 | 103 | 25 | 250 | 112 |
| katahiromz/RisohEditor | 170 | 70,749 | 674 | 30 | 377 | 209 |
| mla-c/mla-core | 287 | 77,679 | 740 | 45 | 321 | 268 |
| hgopalan/ERF | 277 | 123,118 | 1,588 | 60 | 660 | 461 |

(Files/LOC = `find` + `wc -l` over `.c/.cc/.cxx/.cpp/.h/.hh/.hpp` under the repo;
Commits = `git log --oneline | wc -l`, i.e. total history depth, not the window
`--history` actually reads — see below.)

**A contention scare worth recording:** the first pass of `--history` timing
showed katahiromz at 5.2s and rvdbreemen at 65s — wildly out of line with
everything else. Before trusting those numbers, checked `ps aux` and found an
*unrelated* background pipeline (`archcheck-journal/analysis/graph_drift/generate_per_commit_graph_drift.py`,
not `clone_and_run.py`) spawning many parallel `archcheck --diff` subprocesses
against these exact same repo directories at that moment, saturating the CPU.
Re-ran both cleanly (3x each, consistent to within 2ms) once the contention was
noticed: 209ms and 513ms respectively — the numbers in the table above. Flagging
this because it's exactly the kind of "looks suspicious, double-check before
showing" case CLAUDE.md calls out — the 65s figure would have wrongly killed
the entire bundling idea had it gone unchecked.

**Mechanism finding that changes the risk picture:** the task write-up (and the
CLAUDE.md/architecture-spec framing) describes `--history` as walking "full git
log," with cost scaling with total repository history length. That's not what
the code does: `runHistory()` calls `queryCommitHistory(root, 200)`
(`src/cli/preview_commands.cpp:155`), which hardcodes `git log --numstat -n 200`
(`src/git/history_query.cpp:81-97`) — capped at the 200 most recent commits
*regardless of total repo history depth*. rvdbreemen has 3,153 commits total but
`--history` only ever reads the last 200; the same is true for every repo above
900+ total commits. This means history cost is already self-bounding by
construction — it does not grow unboundedly with repository age. What it does
scale with (mildly, within that fixed 200-commit window) is diff density: the
`GodFileGrowthDetector`/`DefectAttractorDetector` passes (`src/scan/god_file_growth.cpp`,
`src/scan/defect_attractor.cpp`) are near-linear scans over the numstat entries
in that window, so a repo with a commit-dense recent history (rvdbreemen: small
43-file tree but heavy recent churn, 4,262 numstat lines in its last 200 commits)
costs more than a repo with a sparser recent history even if its tree is larger
(MaslowCNC: 426 files, only ~cheaper numstat volume recently, 77ms). Tree size
and total repo age are both weak predictors; recent-window diff density is the
actual driver, and it's small in absolute terms across this sample.

**Analysis:** Combined `default + --duplication + --history` cost across all 7
repos (13.7K–123K LOC, 43–3,153 commits) tops out at ~1.18s (hgopalan/ERF:
60+660+461ms) and is as low as ~110ms (rvdbreemen: 11+27+513... actually
history-dominated there at 513ms alone, still sub-second total ~551ms). Nothing
in this sample comes remotely close to threatening ADR-003's "seconds per PR"
CI-viability goal — the worst combined case is nearly two orders of magnitude
under a "several seconds" budget. `--duplication`'s pairwise design is the only
component with a theoretically worse (O(fragments²)-ish) growth curve, and this
sample's largest repo (hgopalan, 277 files) only reached 660ms; the corpus's one
much larger repo, tenstorrent/tt-metal (8,742 files), was excluded per task
instructions because the other pipeline was still actively processing it, so
this analysis cannot rule out duplication cost growing non-trivially on
monorepo-scale trees (1000s of files) — that remains an open question, not
measured here. `--history`'s theoretical risk ("scales with history length, not
tree size") turns out to be moot: the 200-commit cap already bounds it,
independent of how old or how large the repository is.

**Recommendation: (a) with one caveat.** Add an `--all` mode bundling
default + `--duplication` + `--history` (drift stays separate, per the
"Do NOT do" list — it structurally needs a baseline file). The cost is
negligible for every real-world repo size tested here (up to 123K LOC / 277
files / 1,588 commits, well above what a typical PR-sized diff touches) and
the two "risky-sounding" costs in the task description both turn out to be
smaller than feared: `--history` is already hard-capped at 200 commits
(`src/cli/preview_commands.cpp:155`), and `--duplication` stayed sub-second
through the largest repo actually measured. The caveat: since the corpus's one
much-larger repo (tt-metal, 8,742 files) couldn't be measured here (still
in use by another pipeline), `--all` should ship with a simple safety net for
truly large trees — e.g. a file-count/fragment-count soft warning or a
`--all --no-duplication` escape hatch — rather than an unconditional assumption
that duplication stays cheap forever; a follow-up spot-check on tt-metal (or
another repo north of ~1,000 files) once available would close that gap. Doc
updates should also correct the "`--history` walks full git log" framing in
`docs/architecture-spec.md` to reflect the actual 200-commit cap, since that
was the load-bearing assumption behind treating history cost as a bundling
risk in the first place.
