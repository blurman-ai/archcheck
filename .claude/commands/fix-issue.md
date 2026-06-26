Complete a task from the backlog: verify what's done, lock it in with a commit if needed, move the file to completed/ with the summary filled in.

Argument: task ID (NNN) or a name pattern (e.g. `/fix-issue 042`, `/fix-issue sf9`).

## Steps

1. **Find the task** — in `backlog/wip/` by ID (3-digit, zero-padded) or pattern:
   ```
   Glob(pattern="{NNN}_*.md", path="backlog/wip")
   ```
   If not found in wip/ — check `backlog/new/` and `backlog/future/`. If found there — warn the user: "The task hasn't been taken into work; move it to wip and start?" (see `/issue`).

2. **Read the task file.** Print:
   - Title + path.
   - **Goal** — what should be achieved.
   - **Done** — what's already closed.
   - **Next steps** — what remains.

3. **Check git status** relative to the task:
   ```bash
   git log --oneline -10
   git status --short
   git diff --stat HEAD
   ```
   - Are there uncommitted changes related to the task?
   - Are there already commits mentioning `#NNN` or the task topic?

4. **Assess completeness:**
   - Are all items under **Next steps** closed?
   - Are there `fixtures/<rule>/pass/` and `fixtures/<rule>/fail_*/` for all affected rules? (requirement from CLAUDE.md: without fixtures a rule is not considered implemented)
   - Do the tests pass? Run:
     ```bash
     ~/projects/cpparch/build/debug/tests/archcheck_tests
     ```
     If the binary isn't built — warn, don't block.

5. **Commit, if needed:**
   If there are uncommitted changes related to the task — commit in accordance with the `/commit` skill:

   a. Run the lint-gate (as in `/commit` step 3):
      ```bash
      { git diff --name-only HEAD; git ls-files --others --exclude-standard; } \
        | grep -E '\.(h|cpp)$' | xargs -r clang-format --dry-run --Werror

      cppcheck --enable=warning,performance,portability \
               --inline-suppr --error-exitcode=1 \
               --suppress=missingIncludeSystem --quiet \
               -I include src/ include/

      lizard --CCN 15 --length 30 --arguments 5 --warnings_only src/ include/ tests/
      ```
      If at least one check failed — **stop**: print the errors and don't move on until fixed.

   b. Run the coverage gate:
      ```bash
      bash scripts/check_coverage.sh
      ```
      FAIL → show the output, ask: continue (accept warn) or stop.

   c. Build the message per Conventional Commits (type/scope per the tables in `/commit`):
      ```
      fix(<scope>): <summary> (#NNN)

      [optional body: what and why]

      AI-Assisted: Claude
      Verified: <build / build+tests / manual>
      Risk: low|med|high (<reason>)
      Co-Authored-By: Claude <noreply@anthropic.com>
      ```

   d. **Show the message to the user and wait for confirmation.** Edits requested — redo and show again.

   e. Stage only the files related to the task, and create the commit via heredoc:
      ```bash
      git add <relevant files>
      git commit -m "$(cat <<'EOF'
      <commit message>
      EOF
      )"
      ```

   f. `git push origin master`.

6. **Fill in the task file with the summary** (the sections that aren't there yet or are incomplete):

   ```markdown
   ## How it works
   <one or two sentences about the mechanics of the implementation — for a future reader>

   ## Key decisions
   - <decision 1>: <why>

   ## Changed files
   - `src/...` — <what was done> (commit <SHA>)

   ## Summary
   **Status:** completed
   **Completion date:** <YYYY-MM-DD>
   ```

7. **Move the file to `backlog/completed/`:**
   ```bash
   git mv backlog/wip/{NNN}_*.md backlog/completed/
   ```
   Commit this move as a separate `chore` commit:
   ```
   chore(tasks): close #NNN — <task title>
   ```
   Push.

8. **Report:**
   - Path to the completed file.
   - List of commits related to the task (SHA + one-liner).
   - Test status (if they were run).
