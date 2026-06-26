Summarize the current session and save useful knowledge to project memory.

## What to look for in the session

Review the whole session (from the beginning, not just the last messages) and extract:

1. **Mistakes about things I should already have known.** Actions after which the user corrected me on a fact that **is already in memory** or should have been obvious from CLAUDE.md / past experience. If I knew it and forgot — then the memory isn't visible enough or is poorly worded: strengthen the wording, add a "when to apply" trigger.
2. **A mistake repeated within one session.** If I proposed the same wrong option two or three times in a row, or the user corrected a similar thing twice — that's a critical signal. Record it with an explicit **Why:** (what confused me) and **How to apply:** (how to notice the situation in the future).
3. **Learned something new.** Scripts, paths, non-trivial commands, working procedures, project quirks — things that aren't derivable from reading the code and will be lost if not written down.
4. **Confirmed moves.** A non-standard decision the user accepted without objection (also worth locking in — it's validation, not only correction).
5. **Things about the user.** Role, perspective, new preferences for working style / commits / communication.

## What NOT to record

- Code patterns, file paths, architecture — findable via grep
- Solved bugs and fix recipes — the fix is in the code, the context in the commit
- The progress of the current task, temporary state — not for future sessions
- Anything already in `CLAUDE.md` / `CLAUDE.local.md`
- Duplicates of existing memory (see the "Before proposing" section)

## Before proposing

Read `~/.claude/projects/-home-localadm-projects-cpparch/memory/MEMORY.md`. For each candidate decide:

- **new** memory — if the topic doesn't exist
- **update X** — if there's a similar memory but the new observation refines, extends, or refutes it (updating is preferable to growing one next to it)
- **strengthen X** — if the mistake happened because I didn't notice/apply an existing memory: reword it so I don't miss it in the future

## Proposal format

Show the user a compact list:

```
1. [new | update X | strengthen X] <slug> — one-line essence
2. ...
```

Without retelling the session — the user remembers it anyway. Ask for confirmation or edits.

## After "yes"

- New memory: create a file at `~/.claude/projects/-home-localadm-projects-cpparch/memory/<slug>.md` with standard frontmatter (`name`, `description`, `metadata.type` ∈ {user, feedback, project, reference}). For feedback/project, structure the body with `**Why:**` and `**How to apply:**`.
- Updating existing ones: via Edit. Don't rewrite wholesale — augment surgically.
- `MEMORY.md`: add or fix a line (one line `- [Title](file.md) — essence`, ≤150 characters).
- Link related memories via `[[slug]]`.
- At the end — a short summary: "recorded N new, updated M, strengthened K". Without retelling the contents.

## Tone

Concise. The goal is to extract *exactly what isn't obvious from the code/git/CLAUDE.md*, and save it so that in future sessions I **notice it in time**.
