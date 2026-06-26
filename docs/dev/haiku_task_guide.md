# Guide: tasks for a Haiku executor

A document for two readers:

- the **author** (a human or a senior model) who prepares a task in `backlog/` for Haiku to execute;
- **Haiku itself**, which executes the task.

Origin: the audit of tasks #101/#102 (2026-06-11) before handing them off for execution.
Every rule here is not theory but a real problem that was found.

---

## Part 1 — for the author: checklist of a Haiku-ready task

Haiku executes literally and does not fill in missing context. A task is ready only when
all the points below are satisfied.

### 1.1. Not a single unresolved fork

Wordings like "if #X managed to do Y — reuse it, otherwise create Z" are forbidden.
The author must check the repo state themselves and write down the **resolved fact with a date**:

> Bad: "if #093 introduced `function_signature_scan`, extend it; if not — create your own".
> Good: "`function_signature_scan` does not exist (checked 2026-06-11) — we create `function_body_scan` here".

### 1.2. One formula — one source of truth

If the semantics/formula changed over the life of the task, go through **all** sections
(Goal, Detection contract, Plan, Detailed instruction, Next steps, tables) and
align them. Haiku will not pick the "fresher" section — it will take the one it reads.

Real case: in #102 the "The metric itself" section prescribed a defective v1 formula,
even though "Next steps" required v2. Either delete the outdated version, or mark it
explicitly: "implemented, judged defective, do not use".

### 1.3. Every reference to code — verified to be live

Before handing off the task, run every mentioned file/function through `ls`/`grep`.
Specify exact API names (`runGit(args, cwd)`, `collectAddedLines()`), not
"helpers from the git layer". A dead reference, for Haiku, is a reason to write a duplicate.

### 1.4. Every control case has an expected number

Not "switch must not explode", but a table: "flat 8-case switch → score **1**,
finding **NO** (v1 gave 0→19)". Without numbers the executor cannot self-check.

### 1.5. Semantics change → explicitly list which test expectations change

The most dangerous trap. If an old test encodes **defective** behavior
(in #102: `switch_case_explosion` was `must_trigger`, which under the correct semantics
is wrong), and the task says "the suite must pass", Haiku will "fix" the code back
to the defect. You must write: "change the expectation of case X from A to B, because
the old expectation was a mistake; do NOT fit the code to the old expectation".

### 1.6. Task artifacts — only in the repo

No references to `/tmp/...`, other machines, uncommitted local files.
Repro cases, manifests, expectations — copy them into `experiments/` or `fixtures/`
before handing off the task.

### 1.7. Known traps of reused tools — right in the task

If the task says "reuse function X", the author must check its boundary
behavior and write the traps down. Real case: `lex()` from token_normalizer strips out
only `#if 0` blocks, but tokenizes `#if defined(X)` — yielding a spurious keyword `if`.
Without this line in the task, Haiku would have gotten silently inflated scores.

### 1.8. Definition of done — checkable without judgment

"All cases in the table green", "13/13 synthetic", "divergence from clang-tidy ≤ 2 on
20 functions", "0 violations of own rules". Not "works correctly".

### 1.9. Mandatory things — in the imperative

"Can", "preferably", "acceptable" Haiku reads as "optional". For
mandatory things — "must", "do NOT", "only this way". Bold and CAPS for
prohibitions work.

---

## Part 2 — Haiku: rules of execution

### 2.1. Before starting — read in full

1. The task from `backlog/` — **all of it**, including "Do not do" and "Key decisions".
2. All the design docs the task references.
3. [docs/code_style.md](../code_style.md) and [docs/code_quality.md](../code_quality.md).
4. [docs/decisions/](../decisions/) — before declaring a missing feature a gap:
   it may be an accepted decision, not debt.

### 2.2. What to pay attention to

- **The detailed instruction in the task is authoritative.** Found a contradiction
  between sections of the task or between the task and the code — **stop and ask**,
  do not choose silently.
- **Expected numbers in tables are a contract.** Run every control case and
  cross-check it against the table before declaring readiness.
- **Reuse before writing.** Before creating a function/file — grep for something
  similar (the task usually names directly what to reuse). Found something similar
  but the task is silent — ask, don't duplicate.
- **code_quality limits:** functions ≤ 30 lines, classes ≤ 300, ≤ 50 new lines per
  commit, ≤ 2 new files, 0 abstractions nobody asked for. A task that requires
  more will say so explicitly.
- **Verification:** Debug build (`cmake --build build/debug`), tests
  (`ctest --output-on-failure`), lizard
  (`lizard --CCN 15 --length 30 --arguments 5 --warnings_only src/ include/ tests/`),
  dogfood (`./build/debug/src/archcheck` from the root → 0 violations on `src/`,
  `include/`, `tests/`). Paths in commands — absolute only.
- **Statuses honestly:** ran into an external constraint → status `blocked` in
  `backlog/wip/`, NOT `completed`. Completed = the task's goal is achieved and verified.

### 2.3. What to NEVER do

- **Do not commit without an explicit user command.** Finished — build, check, report, and wait.
- **Do not change a test's expectation to make it pass**, unless the task explicitly
  told you to change that particular expectation. A test fails "wrongly" → stop and report.
- **Do not touch contracts:** exit codes (0/1/2/3), CLI flags, `rules::Violation`,
  config schema — without a direct instruction in the task.
- **Do not add dependencies.** The stack is frozen: C++20, CMake, ryml, Catch2.
- **Do not refactor neighboring code** "while you're at it". Edits only in files from
  the "Planned files" section of the task; need a file beyond the list — ask.
- **Do not invent abstractions** (a "future-proof" interface, a generic engine, an extra layer) —
  this is a direct forbidden pattern from code_quality.
- **Do not touch `backlog/pending/`** — don't read it into scope, don't mention it in reports.
- **Do not mark a task done without running** build + tests + control cases.
- **Do not leave "outdated" markers** on documents: an outdated doc is either
  rewritten or deleted (on command).
- **Do not go beyond the task's "Do not do" section.** It is as much a contract as the goal.

### 2.4. Report format on completion

1. What was done — by the points of the task's plan.
2. Verification results: build/tests/lizard/dogfood — concrete numbers
   ("tests: 412/412", "13/13 synthetic", "lizard: 0 warnings").
3. The task's control cases: expectation vs fact, as a table.
4. What was NOT done and why (if anything) — as a separate list, not mixed with what's done.
5. Wait for the command to commit.
