# WBP Class And Asset Workflow Guidelines

## Document Purpose
- This document defines only the fixed rules for how future `Widget Blueprint` assets must be created, updated, and referenced.
- It does not perform actual WBP creation, asset editing, or commandlet execution work.
- Even if another WBP-related planning document is ambiguous, this document takes priority for WBP asset references and generation/execution rules.

## Fixed Rules

### 1. Do not generate the widget tree in runtime code
- The `WidgetTree` root and all child widget structure must physically exist inside the real `WBP_*` asset.
- The final UI used by other systems must always be a real `Widget Blueprint` that can be inspected in the UMG designer.
- The C++ base class may handle state reflection, data binding, animation triggers, and show/hide control.
- However, C++ must not assemble the full widget hierarchy through paths such as `NativeConstruct`, `RebuildWidget`, or similar runtime tree-building logic.

### 2. Other systems must reference the WBP asset, not the generated `_C` class path
- When another system stores a path, document value, or automation setting for a WBP, the source-of-truth reference must be the real `Widget Blueprint` asset path, not the generated `_C` class path.
- Example:
  - allowed: `/Game/UI/Interaction/WBP_InteractionIndicator.WBP_InteractionIndicator`
  - forbidden: `/Game/UI/Interaction/WBP_InteractionIndicator.WBP_InteractionIndicator_C`
- Even if a UE API needs a `UClass` at the final call site, the stored and shared reference value must remain the WBP asset.
- In short, class resolution may happen only at the last usage point. The general system must not treat `_C` paths as the primary reference.

### 3. WBP creation and updates must be implemented only in editor-module code
- Real WBP asset creation and update logic must live in editor-module code such as `CodexUMGBootstrapEditor`.
- Manual editor clicking is not the baseline production workflow.
- Runtime modules must not own the responsibility of generating WBP structure.

### 4. The execution entry point is fixed to a commandlet
- WBP asset creation and update work must be executed only through a `Commandlet`.
- In other words, the implementation lives in the editor module, and the actual execution entry point is the commandlet.
- The repeatable automation path should be designed around `UnrealEditor-Cmd.exe ... -run=<Commandlet>`.

### 5. If editor execution conflicts occur, stop immediately without a workaround
- If commandlet execution conflicts with the current project's `UnrealEditor.exe` process and causes locking, load failure, duplicated session issues, or similar errors, stop the work immediately.
- In that situation, do not try any workaround path.
- Forbidden examples:
  - keep going with the already opened editor session attached
  - redo the same work manually through editor clicks
  - rerun through a separate temporary script path
  - force only part of the asset generation and patch the rest later
- In this situation, report the failure and cause to the user as-is, and tell the user to close the editor before rerunning.

## Recommended Workflow
1. Implement WBP creation/update logic as editor-module functions inside `CodexUMGBootstrapEditor`.
2. Add a dedicated `Commandlet` entry point that calls that logic.
3. When the commandlet runs, it creates or updates the target WBP asset.
4. The result must remain as a real `WBP_*` asset under `Content`.
5. Other systems must reference that result as a WBP asset, not as a generated class path.
6. If an editor/session conflict is detected during execution, fail immediately and notify the user.

## Prohibited Summary
- Building the widget tree entirely in code while leaving the WBP asset structurally empty
- Using `_C` class paths as the primary WBP reference in other systems
- Letting runtime code, rather than an editor module, own WBP asset creation or editing
- Treating manual editor work as the official generation path instead of a commandlet
- Continuing through editor-execution conflicts by trying workaround steps

## Completion Criteria
- The target UI exists inside project `Content` as a real `WBP_*` asset.
- The widget hierarchy is inspectable inside that WBP asset's `WidgetTree`.
- Other usage points are connected through the WBP asset reference, not through a WBP-generated class path.
- Creation/update logic lives in editor-module code, and execution happens only through a commandlet.
- If an editor execution conflict occurs, the work stops immediately and user notification takes priority.
