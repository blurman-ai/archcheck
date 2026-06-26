Update the checkpoint file for the current task.

Find the current task file:
```
Glob(pattern="*.md", path="backlog")
```
If there are several — ask the user which one is currently in progress (or take the most recently modified).

Read the file and update the sections per current progress:
- Move completed items into **Done**.
- Update **In progress**.
- Add new decisions to **Key decisions**.
- Update **Changed files**.
- Add new commits, if any (SHA + short description).

Keep it concise. Don't duplicate information between sections.
