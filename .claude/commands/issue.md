Continue working on a task from the local backlog.

Argument: task ID (NNN) or filename pattern (e.g. `/issue 008`, `/issue 8`, `/issue dependency_graph`).

Steps:

1. Parse argument:
   - If matches `^\d+[a-z]?$` (digits, optionally followed by a single letter like `008a`): task ID.
   - Otherwise: filename substring pattern (snake_case).

2. Find matching file in **all** backlog dirs — `new/`, `wip/`, `future/`, `completed/`:
   ```
   Glob(pattern="{NNN}_*.md", path="backlog/new")
   Glob(pattern="{NNN}_*.md", path="backlog/wip")
   Glob(pattern="{NNN}_*.md", path="backlog/future")
   Glob(pattern="{NNN}_*.md", path="backlog/completed")
   ```
   For ID: `{NNN}` is the argument zero-padded to 3 digits (e.g. `8` → `008`). If no match, retry without padding (e.g. `8_*.md`) — some legacy IDs may exist.

   For filename pattern: `*{pattern}*.md` in the same four dirs.

   IMPORTANT: Use `path` parameter for directory, `pattern` for filename only. Never put `backlog/` into `pattern`.

3. If multiple matches across dirs: show the list (path + first line) and ask the user to clarify.

4. Read the matched file.

5. **If file was found in `backlog/completed/`**:
   - Treat as read-only documentation. Tell the user: "Task is completed (`backlog/completed/`), showing it for reference".
   - Summarize sections **Goal**, **How it works**, **Key decisions**, **Changed files** and stop. Do not propose continuation unless the user asks.
   - Exception: if the user explicitly says "reopen" / "return to wip", move the file to `backlog/wip/`, set `**Status:** wip` and `**Start date:** <today>`.

6. **If file is in `backlog/new/`**:
   - Immediately `git mv` to `backlog/wip/`, add `**Start date:** <today>`, set status to `wip`, and start work. Do **not** ask for confirmation — invoking `/issue <id>` on a `new/` task is itself the go-ahead.

   **If file is in `backlog/future/`**:
   - Ask the user: "Move to wip and start?" — if yes, `git mv` to `backlog/wip/` and add `**Start date:** <today>`, set status to `wip`.
   - If no, treat as read-only briefing.

7. **Optionally** try GitHub issue with the same number (only if argument is a numeric ID and the repo has a GH remote):
   ```bash
   gh issue view {N} --comments 2>/dev/null
   ```
   - If the issue exists and has comments not reflected in the local file under `## Comments from GitHub` — append them in format `**[YYYY-MM-DD] @user:** text`, skip empty/bot noise, tell the user how many were added.
   - If `gh` fails (no remote, no issue, gh not installed) — silently skip. GitHub integration is best-effort; the backlog file is the source of truth.

8. Extract and display key context from the local file:
   - Title (first line `# ...`) + path to the file.
   - **Goal** (why the task exists).
   - **Done** (current status).
   - **In progress** / **Next steps**.
   - **Key decisions**.
   - **Changed files** (with commit SHAs, if any).
   - **Blocks / Blocked by / Related** — if present, resolve the IDs into the names of neighboring tasks.

9. Resume work based on **In progress** / **Next steps**.

10. Update progress via `/checkpoint` as you go.

Example:
```
/issue 8
→ Found: backlog/wip/008_blk_dependency_graph_foundation.md
→ Status: wip (started 2026-05-12)
→ Goal: build an in-memory component graph from the include-scanner output
→ Done:
  - 008a: API skeleton (commit f2182bd)
  - 008b: naive extraction (commit b5f9a1b)
→ Next steps:
  1. Wire 008c (skip comments) into the scan pipeline
  2. Cover with a fixture fixtures/include_scanner/comments/
→ Continuing from step 1…
```
