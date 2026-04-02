# UE5 Project Bootstrap Request

This file is a reusable request template for bootstrapping a brand new Unreal Engine 5 project without launching Unreal Editor.

Use this when you want the assistant to set up:
- a UE5 `.gitignore`
- a `BasicMap` level copied from the engine default template only when that copy is safe
- `GameDefaultMap` and `EditorStartupMap`

## How To Use

Copy the request block below, replace `<ABSOLUTE_PROJECT_PATH>`, and send it as-is.

## Request Template

```md
Read this file first:
<ABSOLUTE_PATH_TO_THIS_MD_FILE>

Then bootstrap the following UE5 project without launching Unreal Editor.

Project path:
<ABSOLUTE_PROJECT_PATH>

Important workflow:
1. Read the request file completely.
2. Before making any file changes, tell me exactly what you are going to do.
3. Summarize:
   - whether you found an existing `.gitignore`
   - whether you found an installed UE engine matching `EngineAssociation`
   - whether `Template_Default.umap` looks safe to copy by file operation alone
   - which files you plan to create or modify
4. After that summary, stop and ask whether you should continue.
5. Only proceed if I explicitly confirm.

Work scope after confirmation:
1. Create or update the project-root `.gitignore` using the UE5 ignore template from this file.
2. Read the project's `.uproject` file and resolve the installed engine path from `EngineAssociation`.
3. Inspect `/Engine/Content/Maps/Templates/Template_Default.umap`.
4. Create `Content/Maps/BasicMap.umap` from that template only if plain file-copy is safe.
5. Ensure `Config/DefaultEngine.ini` contains:
   - `GameDefaultMap=/Game/Maps/BasicMap`
   - `EditorStartupMap=/Game/Maps/BasicMap`
6. Create missing folders if needed.

Constraints:
- Do not launch Unreal Editor.
- Prefer filesystem operations and config edits only.
- Do not revert unrelated user changes.
- The git repository root may be above the project directory.

Safety rules for map creation:
- Only use plain file copy when the template map is effectively self-contained for this operation.
- Do not use plain file copy if related data is required, including:
  - `ExternalActors`
  - `ExternalObjects`
  - `*_BuiltData.uasset`
- If copy-only is not safe, stop and explain why.

Rules for `.gitignore`:
- If `.gitignore` does not exist, create it.
- If `.gitignore` already exists, merge carefully and do not remove user-defined rules unless they directly conflict with the required UE5 rules.
- Ensure at least the UE5 ignore entries listed in this file are present.

Verification after changes:
- Report every created or modified file path.
- Confirm whether `Content/Maps/BasicMap.umap` exists.
- Show the final `GameDefaultMap` and `EditorStartupMap` values found in `Config/DefaultEngine.ini`.
- State that Unreal Editor was not launched.
- Briefly explain why the template map was safe to copy, or why the process was stopped.
```

## UE5 .gitignore Template

Use this content as the baseline when creating or merging the project `.gitignore`.

```gitignore
# Unreal Engine 5
Binaries/
DerivedDataCache/
Intermediate/
Saved/

# Plugin-generated artifacts
Plugins/*/Binaries/
Plugins/*/Intermediate/

# Visual Studio / Rider / VS Code
.vs/
.idea/
.vscode/

# Generated project files
*.sln
*.VC.db
*.VC.opendb
*.opensdf
*.sdf
*.suo
*.user
*.userprefs
*.vcxproj
*.vcxproj.filters
*.vcxproj.user

# Unreal generated files
*.code-workspace
*.xcodeproj
*.xcworkspace

# Crash dumps and logs
*.log
*.dmp
*.tmp

# Per-user editor content
Collections/
Developers/
```

## Notes

- This workflow is intended for a brand new UE5 project where the initial setup is repetitive and should be doable without launching the editor.
- The confirmation step is intentional. The assistant should explain the planned actions first, then wait for approval before modifying files.
- The map-copy step is only valid when the selected template map does not require companion data outside the copied `.umap`.
