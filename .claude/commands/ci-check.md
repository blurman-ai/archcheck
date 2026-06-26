Check whether the latest CI run on GitHub is green for the current branch.

Argument (optional): a branch name or `<run-id>`. Without an argument — the current branch (`git branch --show-current`).

The skill looks at GitHub Actions via `gh`, rather than building locally. The goal is to give a "green / red" answer in one call and, if red, name the failed jobs with their cause.

## Invariants

- **CI reflects the pushed HEAD, not the local one.** If there are unpushed commits — the status on GitHub refers to an older state. This must be spelled out to the user, not silently reported as "green".
- **Read-only.** The skill fixes nothing and commits nothing. Found red — it reports; fixing is separate, on command.
- **Don't run a build locally.** There's a dedicated path for local gates (see the last step) — but that's not the main mode.

## Steps

1. **Determine the branch and reconcile with remote:**
   ```bash
   BR=$(git branch --show-current)
   git fetch -q origin "$BR" 2>/dev/null
   git log --oneline "origin/$BR..HEAD"   # unpushed commits
   ```
   If the list is non-empty — remember: CI only knows about `origin/$BR`, the local HEAD is N commits ahead.

2. **Find the latest run:**
   ```bash
   gh run list --branch "$BR" --limit 1
   ```
   Take its run-id and status (`completed`/`in_progress`, `success`/`failure`/`cancelled`).
   If a `<run-id>` was passed as an argument — use it.

3. **If the run is still going** (`in_progress`/`queued`) — say so and offer to wait (you can poll with `gh run watch <id> --exit-status`, but don't block for long without asking).

4. **If `success`** — report "🟢 CI green on `<sha>` (`<commit subj>`)". If HEAD is ahead of remote — add the warning from step 1.

5. **If `failure`/`cancelled`** — break it down by job:
   ```bash
   gh run view <id>              # job tree: shows which steps X
   gh run view <id> --log-failed # logs of failed steps only
   ```
   For each failed job give a short cause (1–2 lines): which step failed and the key error line. Don't dump the whole log.

6. **Output** — a "job → status → cause" table and an explicit verdict. If HEAD is ahead of origin — remind that some of the problems may already have been fixed locally, and it's worth pushing/rechecking.

## Local gates (on request, if CI is red and you want to check before pushing)

The same checks that `.github/workflows/ci.yml` runs, in "fast first" order.

⚠️ Tool versions on Astra ≠ CI, "clean" locally may not mean "green CI":
- **clang-format**: the system 18.1.8 formats differently than CI (18.1.3). Install the pin:
  `python3 -m pip install --user --quiet 'clang-format==18.1.3'` → `$HOME/.local/bin/clang-format`.
- **cppcheck**: the system 1.86 is older than CI's, it skips some checks.
- **build/debug**: if configured without `-Werror` — it will skip warnings that CI fails on. CI runs Debug with the default `ARCHCHECK_ENABLE_WARNINGS=ON`.

```bash
CF="$HOME/.local/bin/clang-format"   # 18.1.3, as on CI

# 1. clang-format (as in CI)
find src include tests -name '*.h' -o -name '*.cpp' \
  | xargs "$CF" --dry-run --Werror --style=file

# 2. cppcheck (gate)
cppcheck --enable=warning,performance,portability --inline-suppr \
  --error-exitcode=1 --suppress=missingIncludeSystem --quiet -I include src/ include/

# 3. lizard (gate: NLOC≤30, CCN≤15, args≤5, production only)
lizard --CCN 15 -T nloc=30 --arguments 5 --warnings_only src/ include/

# 4. build + test (Debug, with strict warnings as on CI)
cmake -B build/debug -S . -G Ninja -DCMAKE_BUILD_TYPE=Debug -DARCHCHECK_ENABLE_WARNINGS=ON
cmake --build build/debug
( cd build/debug && ctest --output-on-failure )
```
