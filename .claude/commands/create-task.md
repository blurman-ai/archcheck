Create a new task document in `backlog/new/`.

Argument: short task name in snake_case (e.g. `/create-task sf9_cycle_detection`).

## File name

```
backlog/new/NNN_<priority>_<argument>.md
```

- **NNN** — 3-digit ID, `max(existing) + 1` across all subdirectories (`new/`, `wip/`, `future/`, `completed/`).
- **priority** — 3-letter code: `blk` / `crt` / `maj` / `min`.
- **Folder**: `new/` if the task is for the current release (v0.1 / v0.2), `future/` if explicitly deferred to v0.3+ (requires `**Target release:**` in the header).

Example: `006_maj_spec_refactor.md`.

## Steps

1. **Find the next ID:**
   ```
   Glob(pattern="???_*.md", path="backlog/new")
   Glob(pattern="???_*.md", path="backlog/wip")
   Glob(pattern="???_*.md", path="backlog/future")
   Glob(pattern="???_*.md", path="backlog/completed")
   ```
   From all collected names extract the first 3 digits, find max, add 1. Pad to 3 digits (`007`, not `7`).

2. **Check for duplicates** — find similar names:
   ```
   Glob(pattern="*{argument}*.md", path="backlog/new")
   Glob(pattern="*{argument}*.md", path="backlog/wip")
   Glob(pattern="*{argument}*.md", path="backlog/future")
   Glob(pattern="*{argument}*.md", path="backlog/completed")
   ```
   If found — show the paths, ask: continue with the existing one or create a new one.

3. **Find related tasks** — Grep by keywords from the name across `backlog/new/`, `backlog/wip/`, `backlog/future/`, `backlog/completed/`. If there are related ones — propose noting the ID as `Related: #NNN` in the header.

4. **Ask the user**, if not obvious from the name:
   - **Module / tag**: `CONFIG` / `GRAPH` / `SCAN` / `RULES` / `REPORT` / `CLI` / `FIXTURES` / `BUILD` / `DOCS`. Can be combined (`RULES][SF`).
   - **Priority**: `blocker` / `critical` / `major` / `minor`. If the user already specified — don't re-ask. Without any specification — default `minor`.
     - Mapping to code: `blocker → blk`, `critical → crt`, `major → maj`, `minor → min`.
   - **Target release**: if the task is obviously for v0.3+ (design / contract / future feature) — ask whether to put it straight into `backlog/future/`. Default — `new/`.
   - **Goal** — one sentence.

5. **Create the file**:
   - In `backlog/new/NNN_<code>_{argument}.md` (default).
   - In `backlog/future/NNN_<code>_{argument}.md`, if the user confirmed a v0.3+ target release. Add a line `**Target release:** v0.X+` to the template header right after `**Difficulty:**`.

   File template:
   ```markdown
   # [MODULE] Task title

   **Created:** {today YYYY-MM-DD}
   **Start date:** —
   **Status:** new
   **Module:** {module}
   **Priority:** {priority}
   **Difficulty:** unknown
   **Blocks:** —
   **Blocked by:** —
   **Related:** —

   ## Goal

   {one sentence goal}

   ## Context

   {description or "TODO: describe context"}

   ## Execution plan

   - [ ] Step 1
   - [ ] Step 2

   ## Done

   - (empty)

   ## In progress

   - (empty)

   ## Next steps

   1. ...

   ## Key decisions

   | Decision | Reason |
   |---------|---------|
   | ... | ... |

   ## Changed files

   | File | Change |
   |------|-----------|
   | ... | ... |

   ## Fixtures (if a rule)

   - [ ] `fixtures/{rule}/pass/`
   - [ ] `fixtures/{rule}/fail_*/`
   ```

6. Report the path of the created file and the assigned ID.

## When you start work — choosing the format

The canonical process is [`docs/dev/git_workflow.md`](../../docs/dev/git_workflow.md).

- **Direct push to master** — for routine tasks (small fixes, single commits, documentation). The repo admin is in the bypass list, no PR needed.
- **Feature branch** `<type>/<NNN>-<slug>` — for significant work (new module, refactor, multi-stage feature). Type from Conventional Commits (`feat / fix / docs / refactor / chore / test / build / perf / ci`). Merge — via PR in the UI or a direct merge.

Example: task `#015` with priority `crt`, module `RULES/SF`, type `feat` → branch `feat/015-sf9-cycle-detection`.

## Moving between states

- **new → wip:** `git mv backlog/new/NNN_... backlog/wip/NNN_...`, set `**Start date:** YYYY-MM-DD`, status → `wip`.
- **wip → completed:** `git mv backlog/wip/NNN_... backlog/completed/NNN_...`, status → `done`, append at the end:
  - **Completion date:** YYYY-MM-DD
  - **How it works** (principle / algorithm / data flow)
  - **What controls it** (config, flags, env vars)
  - **What it relates to** (dependencies, modules)
  - **Diagnostics** (logs, metrics, key debugging spots)

When the priority changes — rename the file (new 3-letter code), the ID doesn't change.

## Links between tasks

In `**Blocks:**`, `**Blocked by:**`, `**Related:**` use the format:
```
**Blocks:** #004 (project_skeleton), #002 (github_actions_ci)
```

The name in parentheses is for readability, the ID is the stable anchor.
