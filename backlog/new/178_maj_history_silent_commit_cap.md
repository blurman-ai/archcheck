# [BUG] 178: `--history` silently caps at 200 commits, output message misrepresents it as the total

**Creation date:** 2026-07-07
**Status:** new
**Module:** src/cli/preview_commands.cpp, src/git/history_query.cpp
**Priority:** major
**Difficulty:** small (message fix is trivial; whether to raise/configure the cap needs a decision)
**Blocks:** —
**Blocked by:** —
**Related:** #177 (discovered while benchmarking `--history` for that task's cost analysis)

## Goal

`archcheck --history <path>` hardcodes a **200-commit window**
(`runHistory()` → `queryCommitHistory(root, 200)` at
`src/cli/preview_commands.cpp:155`; the limit is passed straight into
`git log --numstat -n 200` at `src/git/history_query.cpp:81-97`) — regardless
of how much history the repository actually has.

**The bug is not the cap itself** (a bounded window is a reasonable, even
necessary, design choice for a "does not gate CI" advisory mode) — **it's that
the tool's own output actively misrepresents a capped window as a complete
count.** Verified 2026-07-07 on `rvdbreemen/OTGW-firmware` (3,153 total
commits via `git log --oneline | wc -l`):

```
$ archcheck --history /home/localadm/oss_agentic/rvdbreemen_OTGW-firmware
queried 200 commits, found 0 god-file growth candidate(s)
```

A user reading "queried 200 commits" has no way to distinguish "this repo has
exactly 200 commits" from "this repo has 3,153 commits and 2,953 of them
(93%) were never examined." Neither `--help`, nor
[docs/architecture-spec.md](../../docs/architecture-spec.md), nor the
CHANGELOG entry that introduced the mode (`--history <path>` advisory mode,
"repository-history analytics over one `git log --numstat` pass") mentions a
200-commit limit anywhere. It is undocumented everywhere it could plausibly
be documented, and the runtime message reads as if the number reported *is*
the total.

## Concrete failure mode this causes

`SIZE.1.god_file_growth` and `HIST.1.defect_attractor` both operate only on
the last 200 commits. A file that grew into a god-file gradually over commit
1-500 of a 3,000-commit repo, then stayed flat for the last 200, is
**invisible** to `--history` — and the tool reports "queried 200 commits,
found 0 candidates" with no hint that 2,800 commits of history were never
looked at. This is exactly the kind of result that looks clean and isn't —
the same failure class CLAUDE.md's "Self-checking conclusions" section warns
about, except here it's the *product* producing the misleading signal, not
the operator.

## Fix

1. **Minimum, uncontroversial part:** change the output message to state the
   window honestly when it's truncated, e.g. `queried the most recent 200 of
   3153 commits` (or `queried all 200 commits` when total ≤ 200). Needs one
   more `git rev-list --count` (or equivalent) call, or deriving the total
   from the same `git log` invocation if cheaply available.
2. **Needs a decision, not just a fix:** should the cap be raised, made
   configurable (`--history-limit N`), or left at 200 with the honest message
   from (1) as the complete fix? Per #177's benchmark
   (`backlog/new/177_maj_archcheck_all_checks_by_default.md`), reading 200
   commits cost 209-513ms on repos with 674-3,153 total commits — cost is not
   the reason to keep it capped low if it currently is one; re-benchmark
   before deciding a new number, don't guess.
3. Update `docs/architecture-spec.md` and the CHANGELOG's `--history` entry
   to state the window explicitly once the number (and whether it's
   configurable) is decided.

## Do NOT do

- Do NOT silently raise the number without re-benchmarking cost (see #177 for
  the existing benchmark harness / already-cloned repos to reuse).
- Do NOT change `SIZE.1.god_file_growth`/`HIST.1.defect_attractor` detection
  logic itself — this task is about window truncation and its visibility,
  not the detectors' algorithms.

## Changed files

| File | Change |
|------|--------|
| `src/cli/preview_commands.cpp` | honest "N of M commits" message; possibly a configurable limit |
| `src/git/history_query.cpp` | may need a total-commit-count query alongside the existing `-n` fetch |
| `docs/architecture-spec.md` | document the window explicitly |
| `CHANGELOG.md` | correct/extend the `--history` entry |
| `backlog/new/178_maj_history_silent_commit_cap.md` | this task |

## Finding (2026-07-07): 200 commits is not always cheap — direct evidence for point 2

While running `--history` across 36 real OSS repos (#173b corpus pipeline), 16 of 34
successfully-cloned repos hit a 300s timeout on this single check. Investigated 2 concretely:

- **`katahiromz/RisohEditor`** (150 files) and **`rvdbreemen/OTGW-firmware`** (38 files) —
  both timed out at 300s inside the 8-worker parallel pipeline, but completed in **0.2-0.3s**
  when re-run standalone, no contention. These are pipeline-concurrency artifacts, not
  `--history` being slow — most of the 16 timeouts are likely this class (several are tiny
  repos), though not all 16 were individually re-verified.
- **`netdata/netdata`** (4,509 commits total, capped to 200) — genuinely does not finish
  `--history` even standalone/isolated: still running past 60s wall-clock, only ~0.7-1.2s
  actual CPU time consumed (i.e., not CPU-bound — waiting on I/O). Ran the underlying
  `git log --numstat -n 200` directly (bypassing archcheck) with the same hardening flags:
  confirms the slowness is in `git` itself against this repo's on-disk state (24 separate
  pack files, an artifact of the corpus pipeline's `--filter=blob:none` partial clone), not
  in `history_query.cpp`'s C++ parsing. Explicitly ruled out `git gc --auto` background
  packing as the cause: re-ran with `-c gc.auto=0` and it was still slow (a background-gc
  message from `git help gc` DID appear in one run's stderr, but disabling it didn't fix
  the timing) — so the true bottleneck is `--numstat`'s diff computation across many small
  packs on a large, actively-developed repo, not a single identifiable git side-effect.

**Implication for point 2 of this task:** "just raise the cap" needs to budget for the fact
that even the CURRENT 200-commit window can take 60s+ (not the ~200-500ms measured in #177's
benchmark on smaller repos) on a large, high-churn C project. Any re-benchmark before raising
the cap must include at least one repo in netdata's size/activity class, not just the smaller
fixtures #177 used — otherwise the new number will look cheap in testing and be expensive in
practice on exactly the kind of large legacy codebase this tool is meant to help.
