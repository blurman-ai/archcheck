Triage this repo's open GitHub issues into backlog tasks.

No arguments. The skill pulls all **open** issues via `gh`, decides for each whether to create new tasks in `backlog/new/` or update existing ones, then comments on the issue with a list of what was created/updated and closes it.

## Invariants

- **An open issue = not yet triaged.** Closing is the only "processed" marker. Don't attach labels.
- **Folder for new tasks: `backlog/new/`.** Even if a task is obviously urgent — let the user move it to `wip/` themselves.
- **Never commit.** The skill only writes files and works with GitHub. Committing is separate, via `/commit`.
- **Ask when in doubt, not on every action.** Safe cases (a new issue with no duplicates, a clear 1-to-1 mapping, well-defined subtasks) — proceed without confirmation. Doubtful ones (updating an existing task, fuzzy scope, incomplete issue coverage by tasks) — ask.

## Steps

### 1. Get the open issues

```
gh issue list --state open --json number,title,body,author,createdAt,labels --limit 50
```

If empty — say "no open issues" and finish.

### 2. For each issue — classify

Read `title + body`. Decide:

**How many tasks does this issue spawn?**
- **N tasks**, if the body has an explicit numbered list of subtasks: markers `Issue 1 — …`, `## Issue 1`, `### 1.`, `1. **Title**` in a row, or a "findings" table. Example: #6 (Arch audit) with `Issue 1 — README documents…` × 9.
- **1 task** in all other cases, including ordinary numbered lists inside a plan (`1. step … 2. step …`) and issue-plans (#5 — it already has a phased plan laid out, that's one task).
- If in doubt — show the user the finding titles, ask "N separate / one rollup / hybrid".

**Is the issue related to an already existing task?**
- Grep the body for mentions of `#NNN` / `042_...` / file names from `backlog/`.
- If the issue mentions a draft `NNN_*.md` — that's the candidate for updating.
- Additionally: Grep by keywords from the title across `backlog/new/`, `backlog/wip/`, `backlog/future/`, `backlog/completed/`. Show the hits.

### 3. For each finding — an action

**A. Create a new task.** Conditions: no similar one in the backlog, or the user confirmed "new".

Follow the `/create-task` convention:
- ID = `max(NNN from backlog/{new,wip,future,completed}) + 1`, pad to 3 digits. For several new tasks from one issue — hand out consecutive IDs.
- Name: `NNN_<priority>_<slug>.md`, where `slug` is snake_case from the finding title.
- Priority — from the issue: look for the words `critical`, `major`, `minor`, `blocker` in the finding body. Default `minor`.
- Module — by content: `DOCS` for documentation, `RULES`/`SCAN`/`GRAPH`/`CONFIG`/`REPORT`/`CLI`/`BUILD` for code, `BACKLOG` for backlog hygiene.
- In the header set `**Related:** #<issue_number> (gh)` so there's a link back.
- In `## Context` — a short distillation of the finding body (1-3 paragraphs), don't copy-paste the whole thing.
- In `## Execution plan` — concrete steps, if they're in the issue (e.g. "acceptance" sections). If there are no steps — `- [ ] TODO: break into steps`.

The file template is the same as in `/create-task` (see `.claude/commands/create-task.md`).

**B. Update an existing task.** Conditions: the user confirmed that a backlog task covers the finding.

- Open the file, read it in full.
- Show the user: "issue #X adds such-and-such info to task #Y. What exactly to write in?" — propose a concrete patch (one or two lines in `## Context` / a new item in `## Execution plan` / an update to `**Related:**`).
- Apply via Edit.
- In `**Related:**` append `#<issue_number> (gh)` if not already there.

**C. Skip.** Conditions: the issue is a question/discussion/spam, requires no action.
- Ask the user: "Close as not-actionable, or leave open?"

### 4. Comment on and close the issue

After all the issue's findings are processed:

```bash
gh issue comment <N> --body "$(cat <<'EOF'
Triaged into backlog tasks:

- Created: #NNN [title] — `backlog/new/NNN_..._....md`
- Updated: #MMM [title] — `backlog/new/MMM_..._....md` (added: ...)

EOF
)"
```

Then:
```bash
gh issue close <N>
```

**When to ask before closing:**
- Fewer tasks found than expected from the body (some findings not covered).
- All actions are only updates to existing tasks (the issue gave no new work — maybe leave it open?).
- The issue contains a question to the user or is awaiting a reply.

In the safe case (new tasks created, clearly covering the whole issue scope) — close without confirmation, the comment already explains everything.

### 5. Summary

Briefly in chat: how many issues triaged, how many tasks created/updated, how many issues closed, how many remain open and why.

## When to stop and ask

- The issue mentions an existing task, but the content diverges from what's in the file — ask which version is the true one.
- The issue looks like a rollup of several tasks, but the boundaries are fuzzy — show the draft split.
- A numbered list looks like subtasks, but they might just be steps of one task — ask.
- The issue touches v0.2+/`future/` scope — ask whether it goes to `new/` or straight to `future/`.
- When updating an existing task the changes are large (> 10 lines of patch) — show the diff, ask.

## Don't

- Don't move tasks between `new/` / `wip/` / `future/` (that's the job of `/issue`, `/checkpoint`, `/fix-issue`).
- Don't edit `pending/`. Never. See memory `feedback_pending_folder`.
- Don't close an issue without a comment linking to the tasks.
- Don't commit the created files — the user does it via `/commit`.
- Don't attach labels on GitHub.
