Create git commit with auto-detected changes following Conventional Commits 1.0.

The canonical process is [`docs/dev/git_workflow.md`](../../docs/dev/git_workflow.md). This skill brings the commit into compliance with it.

Argument (optional): a type hint (for example `/commit fix` or `/commit test`).

## Steps

1. `git status` — look at modified/untracked files.
2. `git diff` — look at the content of the changes.
3. **Auto-format** — reformat all modified/new `.h`/`.cpp` files.

   ⚠️ The system `clang-format` on Astra is 18.1.8 and formats DIFFERENTLY than CI
   (Ubuntu, 18.1.3). Always use pinned 18.1.3, otherwise "clean" locally
   while the CI clang-format job fails. Installation is idempotent:

   ```bash
   python3 -m pip install --user --quiet 'clang-format==18.1.3'
   CF="$HOME/.local/bin/clang-format"   # 18.1.3 — matches CI

   { git diff --name-only HEAD; git ls-files --others --exclude-standard; } \
     | grep -E '\.(h|cpp)$' | xargs -r "$CF" -i
   ```

4. **Lint-gate** — run on the modified `.h`/`.cpp` files:

   ```bash
   # clang-format: the ENTIRE src include tests tree — as the format-check CI job.
   # Not just changed files: we catch formatting drift in untouched
   # files, otherwise CI fails on what we missed locally. $CF = 18.1.3.
   find src include tests \( -name '*.h' -o -name '*.cpp' \) -print \
     | xargs -r "$CF" --dry-run --Werror --style=file

   # cppcheck: always on src/ include/ (cheap, ~1 sec) — as static-analysis CI.
   # NB: the system cppcheck (Astra 1.86) is older than CI's and skips some
   # checks (e.g. performance:stlFindInsert) — "clean" on it ≠ CI "clean".
   cppcheck --enable=warning,performance,portability \
            --inline-suppr --error-exitcode=1 \
            --suppress=missingIncludeSystem --quiet \
            -I include src/ include/

   # lizard: NLOC + complexity thresholds, production only (as in CI)
   lizard --CCN 15 -T nloc=30 --arguments 5 --warnings_only src/ include/
   ```

   If at least one check failed — **stop**, print the errors and don't continue until fixed.

5. **Build + test + smoke gate** — a real build and run, as the `build` CI job.
   This is the main "don't fail on CI" gate: the coverage gate builds only
   the instrumented gcc build, whereas here we verify that the code honestly compiles,
   the tests are green, and the binary runs.

   ```bash
   # Configure + build Debug (build/debug is reused between runs).
   cmake -B build/debug -S . -G Ninja -DCMAKE_BUILD_TYPE=Debug
   cmake --build build/debug

   # Tests (creates build/debug/Testing/Temporary/LastTest.log for the block below):
   ( cd build/debug && ctest --output-on-failure )

   # Binary smoke test — exactly as on CI (don't tighten the form: CI accepts
   # both exit 2 and exit 0 — `unknown` is treated as a scan path):
   ./build/debug/src/archcheck --version
   ./build/debug/src/archcheck --help
   ./build/debug/src/archcheck unknown || test $? -eq 2
   ```

   Any step failed (didn't build / test red / smoke didn't pass) —
   **stop**, print the output and don't continue until fixed.

   > CI additionally builds on `clang-18` and `clang-18-libc++`. This is heavy
   > to run before every commit, so it's not part of the gate. If the change
   > risks portability (new `#include`s, templates, `std::` features) —
   > run it manually:
   > `CXX=clang++-18 cmake -B build/clang -S . -G Ninja -DCMAKE_BUILD_TYPE=Debug && cmake --build build/clang`.

6. **Coverage gate** — run the coverage script:

   ```bash
   bash scripts/check_coverage.sh
   ```

   The script uses a separate `build/coverage` (with `ARCHCHECK_ENABLE_COVERAGE=ON`),
   runs all tests, collects lcov with branch-coverage, and checks the thresholds:
   - lines ≥ 90%, functions ≥ 90%, branches ≥ 40% (source of truth — docs/dev/coverage_constraints.md; same thresholds in CI job `coverage`)

   If the result is **FAIL** — show the script output to the user and ask:
   continue (the user knowingly accepts low coverage) or stop.
   In the `Verified:` trailer write `build+coverage` if it passed, `build+coverage(warn)` if the user continued despite FAIL.

   Thresholds can be overridden: `MIN_LINES=50 bash scripts/check_coverage.sh`.

7. The test log was already created by step 5 (`build/debug/Testing/Temporary/LastTest.log`) — read it, extract the suite name, the test list, PASSED/FAILED.
8. Analyze the changes and build the message per the Conventional Commits schema:

   ```
   <type>(<scope>): <subject>

   [optional body]

   [optional trailers]
   ```

9. **Show the message to the user and WAIT for confirmation.** Edits requested — rewrite and show again.
10. Carefully stage only the relevant files:
   - No `.env`, keys, secrets.
   - Binaries — only if the user explicitly asked.
   - Related `.h` and `.cpp` — together.
11. Create the commit via heredoc.
12. `git push origin master` (direct push is allowed for admins; if working on a feature branch — `git push -u origin <branch>`).
13. `git status` afterward — confirm it went through.

## Type — what to choose

| Type | When |
|---|---|
| `feat` | new product functionality |
| `fix` | bugfix in product code |
| `docs` | documentation only (README, docs/, spec) |
| `refactor` | code change without behavior change |
| `test` | tests or fixtures |
| `build` | build system (CMake, dependencies) |
| `ci` | CI configs (GitHub Actions) |
| `perf` | optimization |
| `chore` | routine: repo infrastructure, skills, renames, configs |

## Scope — where it lives

| Scope | Subsystem |
|---|---|
| `config` | YAML loader, Config struct |
| `graph` | component graph, cycles, metrics |
| `scan` | include / clang scanners |
| `rules/sf` | Core Guidelines SF.* rules |
| `rules/lakos` | cycles, god-headers, CCD/ACD/NCCD |
| `rules/martin` | I/A/D metrics |
| `rules/custom` | user pattern rules |
| `report` | text / json / sarif reporters |
| `cli` | main, arguments, exit codes |
| `fixtures` | `fixtures/`, test corpora |
| `build` | CMake, packaging |
| `docs` | general documents (README, docs/) |
| `spec` | the architecture specification specifically |
| `claude` | `.claude/` (settings, skills) |
| `tasks` | `backlog/` (task management) |
| `process` | git workflow, CHANGELOG, release process |

Several scopes in one commit — pick the most representative one or omit the scope. None fits — invent a short one.

## Subject (first line)

- ≤ 72 characters.
- Lowercase, no trailing period.
- Imperative: `add`, not `added`/`adds`.
- May reference a task: `(#NNN)`.

## Body

After a blank line. What and why, not how (the how is visible in the diff). Optional.

When tests are present — a separate block:

```
Tests-Run: test1, test2, test3
Tests-Result: X/Y PASSED (Z%)
```

## Trailers (on top of Conventional Commits, for AI auditing)

```
AI-Assisted: Claude
Verified: <how you verified — autotest / manual / build / nothing>
Risk: low|med|high (reason)
Co-Authored-By: Claude <noreply@anthropic.com>
```

| Trailer | When required |
|---|---|
| `AI-Assisted:` | Any AI involvement in the code |
| `Verified:` | Always with `AI-Assisted` |
| `Risk:` | Changing rules, graph, graph metrics; deleting code |
| `Co-Authored-By:` | If AI wrote a significant part |

**Risk:**
- `low` — documentation, comments, small edits.
- `med` — new code, refactoring, new rules without semantic changes.
- `high` — changes to graph logic, default rules, exit codes, baseline format, config format.

## Splitting commits

- Code separate from fixtures (different review audience).
- Rules separate from infrastructure.
- Refactoring separate from features.
- Documentation-only — a separate commit.

## Before deleting legacy

1. Tag or baseline-commit.
2. Tests covering the functionality must exist.
3. In the commit — a reference to the tag for rollback.

## Example

```
fix(graph): correct cycle detection on self-loops (#015)

Self-loop A → A was not visible in DFS due to an early return on a visited node.
Now the self-loop is detected by a separate check before DFS.

Tests-Run: graph_self_loop, graph_two_node_cycle, graph_dag
Tests-Result: 3/3 PASSED (100%)

AI-Assisted: Claude
Verified: autotest
Risk: med (graph logic behavior)

Co-Authored-By: Claude <noreply@anthropic.com>
```

## IMPORTANT

- Heredoc to preserve formatting.
- Match existing code style.
- Only actually modified files.
- If it's unclear what to include — ask.
- Force-push to master is blocked — don't try to bypass it.
