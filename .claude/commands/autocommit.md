Create git commit WITHOUT asking for confirmation — fire-and-forget variant of `/commit`.

The canonical process is [`docs/dev/git_workflow.md`](../../docs/dev/git_workflow.md). This skill brings the commit into compliance with it.

**Purpose.** The user runs the command and walks away. The skill builds the message, stages, commits, and pushes by itself — **without showing it and without waiting for confirmation**. The difference from `/commit` is exactly this: the "show and wait" step is removed.

Argument (optional): a type hint (for example `/autocommit fix` or `/autocommit test`).

## Hard autonomy rule

You won't be around to answer a question. Therefore:

- **A failed gate — FIX it first.** lint failed, the build didn't compile, a test is red, smoke returned the wrong exit, or coverage FAILed → try to fix the cause (reformat, address the cppcheck/lizard finding, fix the compilation/test, top up coverage with a test). Once it's fixed and the gate passes — **commit**. After the fix, run the gate again.
- **Doesn't fix autonomously — STOP.** If the fix requires a human decision, is risky, or is unclear — don't commit, leave the error output and describe what you tried. Better no commit than garbage in history.
- **No interactive questions.** A fork that requires a human decision (what to include, secrets in the diff, ambiguous scope) — stop and describe the problem, don't guess.

## Steps

1. `git status` — modified/untracked files.
2. `git diff` — the content of the changes.
3. **Auto-format** — reformat all modified/new `.h`/`.cpp` files:

   ```bash
   { git diff --name-only HEAD; git ls-files --others --exclude-standard; } \
     | grep -E '\.(h|cpp)$' | xargs -r clang-format -i
   ```

4. **Lint-gate** — same as the format-check + static-analysis CI jobs:

   ```bash
   # clang-format: the ENTIRE src include tests tree (not just changed files) —
   # we catch formatting drift, the way CI does.
   find src include tests \( -name '*.h' -o -name '*.cpp' \) -print \
     | xargs -r clang-format --dry-run --Werror --style=file

   cppcheck --enable=warning,performance,portability \
            --inline-suppr --error-exitcode=1 \
            --suppress=missingIncludeSystem --quiet \
            -I include src/ include/

   lizard --CCN 15 --length 30 --arguments 5 --warnings_only src/ include/ tests/
   ```

   If any failed — **STOP**, print the errors, do not continue.

5. **Build + test + smoke gate** — same as the `build` CI job. This is the main gate
   against "green locally, red on CI":

   ```bash
   cmake -B build/debug -S . -G Ninja -DCMAKE_BUILD_TYPE=Debug
   cmake --build build/debug
   ( cd build/debug && ctest --output-on-failure )
   ./build/debug/src/archcheck --version
   ./build/debug/src/archcheck --help
   ./build/debug/src/archcheck unknown || test $? -eq 2   # as on CI, don't tighten the form
   ```

   Didn't build / test red / smoke failed → FIX it first
   (see the autonomy rule). Doesn't fix autonomously — **STOP**.
   (CI also runs clang-18 / libc++ — they're not part of the local gate as heavy.)

6. **Coverage gate**:

   ```bash
   bash scripts/check_coverage.sh
   ```

   Thresholds: lines ≥ 90%, functions ≥ 90%, branches ≥ 40% (same as in CI job `coverage`; source — docs/dev/coverage_constraints.md).
   Result **FAIL** — **STOP**, show the script output, do not commit.
   (In autonomous mode we don't continue "knowingly accepting low coverage" — there's no one to accept it.)
   On success — `Verified: build+coverage`.

7. The test log was already created by step 5 (`build/debug/Testing/Temporary/LastTest.log`) — read it, extract the suite, tests, PASSED/FAILED.
8. Build the message per Conventional Commits (schema, types, scope, subject, body, trailers — **identical to `/commit`**, see [`commit.md`](commit.md)).
9. **WITHOUT showing or confirming.** Immediately and carefully stage only the relevant files:
   - No `.env`, keys, secrets. Noticed a secret in the diff — **STOP**.
   - Binaries — don't stage (in autonomous mode there's no one to ask).
   - Related `.h` and `.cpp` — together.
10. Create the commit via heredoc.
11. Push: `git push origin master` (admin direct push); on a feature branch — `git push -u origin <branch>`.
12. `git status` afterward — confirm it went through. Report briefly: hash, branch, push result.

## Commit message

Entirely as in [`commit.md`](commit.md): Conventional Commits 1.0, type/scope tables, subject ≤ 72, body "what and why", a `Tests-Run`/`Tests-Result` block when a log is present, and trailers:

```
AI-Assisted: Claude
Verified: <build+coverage / autotest / manual / build>
Risk: low|med|high (reason)
Co-Authored-By: Claude <noreply@anthropic.com>
```

## IMPORTANT

- Heredoc for formatting.
- Only actually modified files.
- Force-push to master is blocked — don't bypass.
- Unclear what to include / secret in the diff / ambiguous scope → **STOP and describe**, don't guess.
