# 183 maj — Agent-operable cells: pilot on archcheck's own dev workflow

Status: wip
Created: 2026-07-11
Start date: 2026-07-11
Executor: **Sonnet** (design + implementation). Escalate to the author per the closure
protocol this task itself introduces (section D below) — that is not circular: until D lands,
escalate = stop and ask in chat.

## Background (read first)

The author's working hypothesis "agent-operable architecture" (source article, Russian:
`/home/localadm/projects/linked_in/posts/agent_operable_architecture_ru.md` — read at least
§§2–6, 13, 18, 23): a system should offer an agent bounded **cells** where it can close a
full engineering cycle without reading the whole system. A cell = **hand** (bounded action
surface) + **eye** (compact, reproducible, before/after-comparable consequences) + **laws**
(machine-executable invariants, not advice) + **closure protocol** (5 verdicts, not "done") +
**evidence artifact** with a mandatory `Unverified` section.

archcheck's dev infrastructure is already ~70% of a cell system, just unnamed: "one rule =
one class = one file + one factory line" is a textbook action surface; fixtures are the
mandatory reproduction; the /commit skill chains format→lint→build→tests→coverage→self-check;
openwiki is the per-cell context map; JOURNEY/completed/ is the §14 external memory. This
task NAMES the cells, closes the 3 real gaps (consolidated eye, explicit passports, closure
protocol), and pilots the loop on one real task.

Why (three payoffs): (1) better closure quality from agent executors on this repo;
(2) archcheck becomes the second measured case study for the author's article (after
leadline); (3) a dogfooding story for launch ("we develop an architecture gate inside
agent cells gated by itself").

## Already machine-enforced — do NOT redo (dedup)

- **Exit-code contract**: `tests/integration/cli/cli_smoke_e2e_test.cpp` asserts 0/1/2
  including advisory-vs-gate semantics (#133). Reference it in the laws table; write no new test.
- **Thresholds**: coverage 90/90/40 (`scripts/check_coverage.sh` + CI), lizard CCN15/NLOC30/
  args5, clang-format 18.1.3 (pinned pip), cppcheck — all already run in /commit steps 3–6.
- Self-check dogfooding (`./build/debug/src/archcheck` on src/ include/ tests/ → 0 violations)
  — exists as a manual step; the evidence script makes it non-skippable.

## Deliverables

### D1. `scripts/agent_evidence.sh` — the consolidated eye

One bash script that RUNS THE EXISTING gates (no new checks, consolidation only — reuse the
exact commands from `.claude/commands/commit.md` steps 3–6 and the smoke block) and prints
ONE compact block, ≤25 lines (article §18 step 4):

```
=== archcheck evidence [<label>] @ <HEAD short-sha>[+dirty] ===
build (Debug):    OK | FAIL
tests:            <passed>/<total> PASSED | FAILED: <names>
self-check:       <N> violations (src include tests)
format (18.1.3):  clean | <N> files dirty
cppcheck:         clean | <N> warnings
lizard:           clean | <N> warnings
coverage:         lines <X>% funcs <Y>% branches <Z>%  (gates 90/90/40)
smoke:            --version OK, --help OK, unknown arg -> exit 2
Unverified:       clang-18/libc++ build (CI-only); Windows/macOS binaries; runtime perf
```

Usage: `scripts/agent_evidence.sh [label]`. Before/after = run once on the base state, once
after the change, diff the two blocks. Coverage is the slowest gate — support
`EVIDENCE_FAST=1` to skip it (block must then print `coverage: SKIPPED (fast mode)` so the
omission is visible, never silent). Exit non-zero if any gate failed.

### D2. `docs/dev/agent_cells.md` — cell passports + global laws

English, ≤150 lines. Content:

1. **Four cell passports** (table per cell: allowed paths / forbidden / eye command /
   escalation triggers):
   - **new-default-rule**: allowed `src/rules/<rule>.{h,cpp}` (new pair), ONE line in
     `src/rules/rule_set.cpp`, `tests/unit/rules/`, `fixtures/<rule>/pass|fail_*/`;
     forbidden: `graph/`, `scan/`, `report/`, existing rule files, public headers, exit
     codes; escalate: rule needs a graph/scanner API change, or no fixture can reproduce it.
   - **test-fixture-only**: allowed `tests/`, `fixtures/`; forbidden `src/`; escalate: the
     test exposes a product bug (the fix is a different cell, different task).
   - **reporter-change**: allowed `src/report/`, reporter tests; forbidden rule/graph logic
     and exit-code semantics.
   - **research-script**: allowed `experiments/` (gitignored) and journal-repo `analysis/`;
     forbidden `src/` entirely; eye = the self-checking rules of CLAUDE.md (spot-check 2–3
     cases by hand before any conclusion).
2. **Global laws** (each row: law + WHERE it is enforced): exit-code contract (test above),
   coverage/lizard/format/cppcheck gates, self-check 0 violations, code_quality budgets
   (≤50 new lines/commit, ≤2 new files, 0 unrequested abstractions).
3. **The Goodhart law (non-negotiable, from the author):** an agent must never weaken a
   fixture, delete/relax a test assertion, regenerate a baseline, or lower a threshold in
   order to make a gate pass. Any change to the cell's own laws (this doc, `scripts/`,
   thresholds, fixtures of OTHER rules, baselines) = **ESCALATE**. A cell may not modify
   its own laws.

### D3. Closure protocol → `docs/dev/haiku_task_guide.md`

Add a short section (applies to ALL executor models, not only Haiku): every task/cell run
ends with exactly one verdict, citing evidence lines:

- `ACCEPT` — target scenario fixed, all laws green, evidence attached, Unverified listed;
- `REJECT` — change rolled back, reason recorded;
- `MIXED` — improves one case, degrades another; needs author's scope call;
- `INSUFFICIENT_EVIDENCE` — maps to the existing CLAUDE.md rule "doubts remain → task stays
  open, doubts go to the user";
- `ESCALATE` — maps to existing rules "blocked → wip, not completed" and "gap → surface,
  don't work around".

Cross-link agent_cells.md ↔ haiku_task_guide.md. Do not contradict existing rules — extend.

### D4. Pilot: one real task through the cell, measured

Run ONE small real backlog task end-to-end under the new process. Default candidate:
`backlog/new/144_min_diff_unresolved_baseline_exit2.md` (small, bounded, matches the
test-fixture/rule cells); if on reading it turns out non-trivial, ask the author to pick
another. Record in THIS task's progress log (§15 metrics of the article):

- evidence block BEFORE and AFTER (verbatim);
- verdict + which evidence lines justify it;
- first-prompt closure: yes/no; number of human interventions; number of escalations;
- wall time; anything the passports got wrong (feed back into D2).

## Phases and gates

| Phase | What | Gate |
|---|---|---|
| A | Read article §§2–6/13/18/23, code_quality.md, haiku_task_guide.md, commit.md skill; draft the evidence block format | show the draft block to the author (one message, cheap) |
| B | Implement D1; run on clean tree AND on a deliberately broken state (e.g. one failing test) | both sample outputs recorded here; broken state clearly shows FAIL; fast mode prints SKIPPED |
| C | Write D2 passports + laws | every passport's eye command actually runs; forbidden lists checked against real dir layout |
| D | Add D3 verdicts to the guide | no contradiction with CLAUDE.md self-checking section or task-workflow rules |
| E | Pilot D4 | before/after evidence + verdict + metrics logged; escalations quoted verbatim |

## Out of scope (do not build)

- `.archcheck.yml` layer rules on archcheck's own repo — needs v0.2 config enforcement;
  add one line to `backlog/DEBT.md` instead.
- Hidden/rotating scenarios against gate-gaming (conflicts with determinism; open question
  §17.3 of the article — note, don't solve).
- CI integration of `agent_evidence.sh` (candidate follow-up, separate task).
- Any framework/abstraction layer. Plain bash + two markdown files is the whole build.

## Constraints

- `docs/code_style.md` + `docs/code_quality.md` budgets apply (split commits: script / docs /
  guide edit / pilot are separate commits, each ≤50 new lines where feasible; the script may
  exceed slightly — it is glue, keep it as small as honesty allows).
- All artifacts in English. Commits via /commit with the author's confirmation, as always.
- Reuse commands verbatim from `.claude/commands/commit.md` — do not invent new gate
  invocations (version-pinned clang-format 18.1.3 via `~/.local/bin/clang-format`!).

## Assets (exact paths)

- Article: `/home/localadm/projects/linked_in/posts/agent_operable_architecture_ru.md`
- Gate chain source of truth: `.claude/commands/commit.md` (steps 3–6), `scripts/check_coverage.sh`
- Exit-code contract test: `tests/integration/cli/cli_smoke_e2e_test.cpp`
- Executor contract to extend: `docs/dev/haiku_task_guide.md`
- Context map to reference: `docs/openwiki/index.md`
- Pilot candidate: `backlog/new/144_min_diff_unresolved_baseline_exit2.md`

## Acceptance criteria

- [x] `scripts/agent_evidence.sh`: one ≤25-line block, all existing gates, FAIL visible on a
      broken tree, fast mode explicit, non-zero exit on any failure.
- [x] `docs/dev/agent_cells.md`: 4 passports + global laws + the Goodhart law; every eye
      command verified runnable.
- [x] Closure protocol (5 verdicts) in `docs/dev/haiku_task_guide.md`, cross-linked, no
      contradictions with existing rules.
- [x] Pilot task executed through the cell: before/after evidence, verdict with line
      references, §15 metrics recorded in this file.
- [x] `backlog/DEBT.md` line about self-`.archcheck.yml` (post-v0.2).
- [x] Passport corrections discovered during the pilot fed back into D2 (or recorded as
      open items if they need the author's call).

## Progress log

### Phase A — draft evidence block, author sign-off

Read the full article (all sections, not just §§2–6/13/18/23), `code_quality.md`,
`haiku_task_guide.md`, `.claude/commands/commit.md`, `scripts/check_coverage.sh`. Confirmed
the exact self-check invocation (`archcheck src include tests` → `--format json`
`summary.total`). Showed the draft D1 block (task's own template, filled with real numbers:
build OK, 644/644 tests, 0 violations) to the author. Author approved with 3 amendments:

1. Build-failure short-circuit also applies to `coverage` (own build dir, but skip rather
   than burn 5 min on a known-broken tree); skipped lines don't need separate exit-code
   accounting — the build FAIL already makes exit non-zero.
2. `EVIDENCE_FAST=1` blocks must never justify `ACCEPT` — only a full run closes a cell.
   Closes a Goodhart loophole ("only run fast, coverage never drops"). Implemented as a
   visible `[FAST — not ACCEPT-eligible]` tag in the block header (machine-visible, not just
   documented) plus a stated rule in D2/D3.
3. Missing tool ≠ clean. A missing pinned tool (clang-format 18.1.3, cppcheck, lizard) prints
   `ERROR (tool missing: ...)` and fails the exit code — never a silent `clean`.

### Phase B — `scripts/agent_evidence.sh`, tested clean + broken

Implemented with all 3 amendments. Verified by hand (not just written and trusted):
clang-format's `--dry-run --Werror` per-file error format, cppcheck's `[file:line]: (severity)`
format, lizard's `: warning:` line format — each confirmed against real tool output on
scratch files before wiring the parser, not guessed.

Clean-tree full run (`baseline-full`): build OK, 644/644 tests, 0 self-check violations,
format/cppcheck/lizard clean, coverage 91.9%/96.0%/57.3% (gates 90/90/40), smoke OK. 1m38s
wall time.

Broken-state runs (each introduced, verified FAIL, then reverted and re-verified clean):
- One flipped `REQUIRE` assertion in `include_resolver_test.cpp` → `tests: 643/644 PASSED |
  FAILED: resolve_include quote: directory-relative hit -> Project`, exit 1.
- One `#error` in `main.cpp` (compile failure) → `build (Debug): FAIL`, and `tests`/
  `self-check`/`smoke`/`coverage` all correctly print `SKIPPED (build failed)` while
  `format`/`cppcheck`/`lizard` (static, build-independent) still ran clean. Exit 1.

`EVIDENCE_FAST=1` confirmed prints `coverage: SKIPPED (fast mode)` and tags the header
`[FAST — not ACCEPT-eligible]`.

### Phase C — `docs/dev/agent_cells.md`

Verified every passport's paths against the real tree before writing them down (not
assumed from the task description): `src/rules/*.{h,cpp}` pairs, `rule_set.cpp`'s
`makeDefaultRuleSet`/`makeDriftRuleSet` factories, `fixtures/<rule>/pass|fail_*/` naming,
`src/report/` + `include/archcheck/report/` + `tests/unit/report/`, `experiments/`
(gitignored) and the companion-repo `analysis/`. Ran the exact eye command
(`scripts/agent_evidence.sh <cell-name>`) for all 3 code-touching cells with their literal
passport labels — all ran clean.

### Phase D — closure protocol in `haiku_task_guide.md`

Added §2.5 (5 verdicts), extended §2.4's report format to point at it, cross-referenced
§2.2 "Statuses honestly" (ESCALATE) and the root CLAUDE.md self-checking section
(INSUFFICIENT_EVIDENCE) — verified both referenced sections say what I claimed they say
before citing them.

### D4 pilot — #153 item 1 (`fixtures/vendor_exclusion/`), cell: `test-fixture-only`

**Candidate swap.** The task's default candidate, #144, turned out to be already shipped —
`git log` showed it committed and on `master` since 2026-06-27 (`e147132`), 12 commits back;
the `backlog/new/144_*.md` file was stale (never moved to `completed/`). Surfaced this to the
author instead of silently picking a replacement (per the task's own instruction: "if it
turns out non-trivial, ask the author to pick another" — here inverted: trivial because
already done). Screened `backlog/new/` (20 tasks) against the four passports; most touch
`src/graph/`/`src/scan/` (forbidden outside `research-script`) or are too large/process-y.
Landed on **#153 item 1** — the mandatory `fixtures/vendor_exclusion/` fixtures split out of
completed #127 — verified against the real code (not just the task text) that the exclusion
logic (`AuthoredScope`, `file_classification.h`) is shipped+unit-tested but has no fixture
dir yet. Author confirmed with "да".

**What was built** (all inside `tests/**` + `fixtures/**`, confirmed via `git diff --stat --
src/ include/` = empty):
- `fixtures/vendor_exclusion/{pass_nested_license,fail_own_code,pass_generated_marker,
  pass_minified_data}/` + `README.md` — each file's content engineered against the actual
  trigger conditions read from `file_classification.h` (exact license-marker strings,
  `@generated` offset rule, `hasMinifiedContent`'s 4096-byte/110-avg-line-length thresholds),
  not guessed.
- `tests/integration/scan/vendor_exclusion_fixtures_test.cpp` (new) — 4 test cases, reading
  the fixture trees from disk and calling `AuthoredScope::fromFiles`/`.excluded()` directly
  (matching the existing `authored_scope_test.cpp` API), plus one full-binary run
  (`runArchcheck`) proving the `fail_own_code` SF.7 violation actually reaches the report,
  not just survives the `AuthoredScope` check.
- `tests/CMakeLists.txt` — one line registering the new test file (explicit list, not glob).

**Evidence BEFORE** (`pilot-153-before`):
```
=== archcheck evidence [pilot-153-before] @ 7330323+dirty ===
build (Debug):    OK
tests:            644/644 PASSED
self-check:       0 violations (src include tests)
format (18.1.3):  clean
cppcheck:         clean
lizard:           clean
coverage:         lines 91.9% funcs 96.0% branches 57.3%  (gates 90/90/40)
smoke:            --version OK, --help OK, unknown arg -> exit 2 OK
Unverified:       clang-18/libc++ build (CI-only); Windows/macOS binaries; runtime perf
```

**Evidence AFTER, first full run** (`pilot-153-after`) — the eye caught a real defect:
```
=== archcheck evidence [pilot-153-after] @ 7330323+dirty ===
build (Debug):    OK
tests:            648/648 PASSED
self-check:       0 violations (src include tests)
format (18.1.3):  1 files dirty
cppcheck:         clean
lizard:           clean
coverage:         lines 92.0% funcs 96.0% branches 57.4%  (gates 90/90/40)
smoke:            --version OK, --help OK, unknown arg -> exit 2 OK
Unverified:       clang-18/libc++ build (CI-only); Windows/macOS binaries; runtime perf
```
(exit 1 — the new test file needed a blank line between include groups per `.clang-format`.
Fixed with the pinned formatter, no manual style guessing.)

**Evidence AFTER, fixed** (`pilot-153-after-fixed`) — this is the block the verdict below
cites, a full run (no `EVIDENCE_FAST`), eligible per the Goodhart law:
```
=== archcheck evidence [pilot-153-after-fixed] @ 7330323+dirty ===
build (Debug):    OK
tests:            648/648 PASSED
self-check:       0 violations (src include tests)
format (18.1.3):  clean
cppcheck:         clean
lizard:           clean
coverage:         lines 92.0% funcs 96.0% branches 57.4%  (gates 90/90/40)
smoke:            --version OK, --help OK, unknown arg -> exit 2 OK
Unverified:       clang-18/libc++ build (CI-only); Windows/macOS binaries; runtime perf
```

**Verdict: ACCEPT.**
- Target scenario fixed: `fixtures/vendor_exclusion/` (#153's unmet acceptance bar, item 1)
  now exists — 4 fixture dirs + `fail_own_code` anti-over-exclude regression, all passing.
- All laws green: `tests: 648/648 PASSED` (+4 net vs. before, 0 regressions), `self-check: 0`,
  `format: clean`, `cppcheck: clean`, `lizard: clean`, `coverage` still above all 3 gates
  (91.9→92.0 / 96.0→96.0 / 57.3→57.4, no drop).
- Cell boundary respected: `git diff --stat -- src/ include/` empty for the whole pilot.
- `Unverified` section carried through unchanged (nothing about this pilot touches
  clang-18/libc++, non-Linux binaries, or runtime perf).
- Dogfood acceptance sub-criterion from #153 ("archcheck src/ include/ tests/ stays 0")
  satisfied by the `self-check:` line.

**§15 metrics:**
- First-prompt closure: **no**, at the #183-task level (3 amendment rounds in Phase A, one
  candidate-swap round after #144 turned out stale) — but **yes** for the D4 pilot itself
  once #153 item 1 was confirmed: one evidence-driven fix cycle (the format-gate catch above)
  with zero further human input needed to reach `ACCEPT`.
- Human interventions across #183: 4 — the initial wip-start confirmation, the Phase A
  3-point amendment, the pilot-candidate screening-approach choice, the final "да"
  confirmation of #153 item 1.
- Escalations: 0 formal `ESCALATE` verdicts. One stop-and-ask moment outside the formal
  protocol (the #144-is-stale finding) — consistent with this task's own preamble ("until D
  lands, escalate = stop and ask in chat").
- Wall time: not instrumented precisely (see passport correction below); the two measured
  full `agent_evidence.sh` runs (before/after) took **1m38s each**, concrete data points.
- Passport corrections fed back into D2 (see the two edits already in
  `docs/dev/agent_cells.md`):
  1. Fixed a stale cross-link (D2 pointed at "Part 2A", D3 landed as "§2.5").
  2. Clarified that `test-fixture-only`'s allowed surface includes `tests/CMakeLists.txt`
     (needed to register a new test file — the target list is explicit, not globbed).
  3. Added a call-out that "bounded action surface" is not "bounded reading surface":
     writing 4 *correct* fixtures required reading `file_classification.h`,
     `authored_scope.h`, and `source_snapshot.h` in full — a guessed fixture would have
     tested nothing.
  4. **Resolved after author review:** author's call was "cheap and harmless, add it" —
     `scripts/agent_evidence.sh` now prints a trailing `wall time:` line (bash `$SECONDS`,
     zero setup cost). This covers the gate-chain's own run time, not the whole cell
     session (reading, writing fixtures, etc.) — that still needs the agent to self-report
     start/end, out of scope for a bash script. Final full run after the change:
     `wall time:        53s`, all gates otherwise unchanged (648/648, 0 violations,
     coverage 92.0/96.0/57.4, clean format/cppcheck/lizard).
