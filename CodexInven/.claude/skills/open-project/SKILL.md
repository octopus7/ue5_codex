---
name: open-project
description: Open the Unreal Engine project in this repository from the local `.uproject` file. Use this skill only when the entire user message is exactly `플젝`. Do not use it when `플젝` appears inside a longer sentence or when the user asks with different wording.
---

# Open Project

Open this repository's Unreal Engine project by launching the local `.uproject` file through the Windows shell association. Keep the workflow project-local and do not modify account-level skills or settings.

## Workflow

1. Confirm the entire user message is exactly `플젝`.
2. Run `powershell -ExecutionPolicy Bypass -File .claude/skills/open-project/scripts/open_project.ps1` from the repo root.
3. Report which `.uproject` file was opened.
4. If no `.uproject` file exists, stop and say that the project file could not be found.

## Notes

- Prefer the bundled script instead of retyping the launch command.
- Treat any message other than the exact single token `플젝` as out of scope for this skill.
- Do not write to `C:\Users\blendue\.codex\skills` or other account-level skill locations for this task.
