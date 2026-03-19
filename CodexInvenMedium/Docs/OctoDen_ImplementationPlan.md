# OctoDen Plugin Implementation Plan for CodexInvenMedium

## Goal

Add a new editor-only plugin named `OctoDen` to this project, with the same user-facing behavior as the `OctoDen` plugin found in the sibling project `CodexInven`.

This document is intentionally written as a clean-room implementation plan, not a copy task. The next implementation conversation should follow the behavior and constraints below without lifting code structure, identifiers, helper functions, or UI composition directly from the source plugin.

## Current Project State

Project root: `D:\github\ue5_codex\CodexInvenMedium`

Observed state at planning time:

- There is no `Plugins` directory yet.
- The project has one runtime module: `CodexInvenMedium`.
- `EnhancedInput` is already a runtime dependency of the main module.
- There are currently no project-defined `GameInstance`, `GameMode`, or `InputConfig` runtime classes.
- `DefaultEngine.ini` currently points `GameDefaultMap` to `/Engine/Maps/Templates/OpenWorld`.

Implication:

- The editor plugin can be added independently.
- Full parity of the input-linking feature requires new runtime-side support classes and properties inside `CodexInvenMedium`.
- The bootstrapper can generate `GameInstance` and `GameMode` native classes into the runtime module, but the input-linking portion still needs a project-specific data asset class and a `GameInstance` property to receive it.

## Required Outcome

After implementation, the project should contain a plugin named `OctoDen` that adds an `OctoDen` menu to the Unreal Editor main menu with two tools:

1. `Bootstrapper`
2. `Input Builder`

The resulting toolset should support this workflow:

1. Create or open a managed map under `/Game/Maps`.
2. Generate or reuse native `GameInstance` and `GameMode` classes in the `CodexInvenMedium` runtime module.
3. Create or reuse blueprint subclasses for those native classes.
4. Apply those blueprints and maps to project settings.
5. Build standard Enhanced Input assets for `Move`, `Look`, `Jump`, and `Fire`.
6. Create or reuse a runtime input config data asset.
7. Assign that input config to the project `GameInstance` blueprint defaults.

## Clean-Room Guardrails

The next implementation should preserve behavior but avoid obvious source carryover.

### Must do

- Recreate the plugin from scratch in this repository.
- Use fresh helper names, fresh local function names, and fresh UI-building structure.
- Rephrase all tooltips, dialog text, notification text, tab labels, and help text.
- Reorganize large source files if that yields clearer ownership.
- Prefer smaller units over one giant module file.

### Must not do

- Do not copy-paste any `.cpp`, `.h`, or `.uplugin` content from the sibling project.
- Do not preserve unique internal naming patterns if equivalent behavior can be expressed more cleanly.
- Do not preserve the exact order of functions, exact text literals, or exact widget layout tree.
- Do not rely on hidden assumptions from the old project without making them explicit in this project.

## Recommended High-Level Design

Implement `OctoDen` as an editor plugin with one editor module, but split the code by concern.

Recommended file layout:

```text
Plugins/
  OctoDen/
    OctoDen.uplugin
    Source/
      OctoDen/
        OctoDen.Build.cs
        Public/
          OctoDenModule.h
        Private/
          OctoDenModule.cpp
          UI/
            OctoDenMenuRegistration.cpp
            OctoDenTabs.cpp
          Bootstrapper/
            OctoDenBootstrapperSettings.h
            OctoDenBootstrapperSettings.cpp
            OctoDenBootstrapperDetails.h
            OctoDenBootstrapperDetails.cpp
            OctoDenBootstrapperService.h
            OctoDenBootstrapperService.cpp
          InputBuilder/
            OctoDenInputBuilderSettings.h
            OctoDenInputBuilderSettings.cpp
            OctoDenInputBuilderDetails.h
            OctoDenInputBuilderDetails.cpp
            OctoDenInputAssetService.h
            OctoDenInputAssetService.cpp
            OctoDenInputMappingRules.h
            OctoDenInputMappingRules.cpp
          Shared/
            OctoDenAssetNaming.h
            OctoDenAssetNaming.cpp
            OctoDenNotifications.h
            OctoDenNotifications.cpp
            OctoDenReflectionHelpers.h
            OctoDenReflectionHelpers.cpp
          Tests/
            OctoDenAutomationTests.cpp
```

This exact tree is not mandatory, but the implementation should separate:

- menu and tab registration
- bootstrapper behavior
- input builder behavior
- generic asset/path/reflection helpers
- tests

## Plugin Descriptor and Build Setup

### Add plugin descriptor

Create `Plugins/OctoDen/OctoDen.uplugin`.

Requirements:

- Friendly name: `OctoDen`
- Category: `Editor`
- Type: editor-only module
- Enabled by default: true
- Beta flag is acceptable
- Add plugin dependency on `EnhancedInput`

### Add module rules

Create `Plugins/OctoDen/Source/OctoDen/OctoDen.Build.cs`.

Expected dependencies include at least:

- `Core`
- `CoreUObject`
- `Engine`
- `Slate`
- `SlateCore`
- `InputCore`
- `EnhancedInput`
- `UnrealEd`
- `ToolMenus`
- `PropertyEditor`
- `Projects`
- `AssetTools`
- `AssetRegistry`
- `Kismet`
- `KismetCompiler`
- `BlueprintGraph`
- `EditorSubsystem`
- `EditorFramework`
- `InputEditor`

Trim the final list if some are not actually needed.

## Project-Side Runtime Support Required

The old plugin assumes runtime classes exist. This project does not currently have them, so parity requires explicit support.

### Add a runtime input config data asset class

Add a new runtime class in the main module:

- `UCodexInvenMediumInputConfigDataAsset : UDataAsset`

Required UPROPERTY fields:

- `UInputMappingContext* InputMappingContext`
- `UInputAction* MoveAction`
- `UInputAction* LookAction`
- `UInputAction* JumpAction`
- `UInputAction* FireAction`

Preferred location:

```text
Source/CodexInvenMedium/Input/CodexInvenMediumInputConfigDataAsset.h
Source/CodexInvenMedium/Input/CodexInvenMediumInputConfigDataAsset.cpp
```

### Add a runtime game instance base that can receive the data asset

Add a runtime class in the main module:

- `UCodexInvenMediumGameInstance : UGameInstance`

Required UPROPERTY field:

- `UCodexInvenMediumInputConfigDataAsset* DefaultInputConfig`

Preferred location:

```text
Source/CodexInvenMedium/Core/CodexInvenMediumGameInstance.h
Source/CodexInvenMedium/Core/CodexInvenMediumGameInstance.cpp
```

### Build.cs update for runtime module

Ensure `CodexInvenMedium.Build.cs` has dependencies needed by the new runtime types:

- `Core`
- `CoreUObject`
- `Engine`
- `InputCore`
- `EnhancedInput`

It already includes these, so likely no change is required.

## Tool 1: Bootstrapper Specification

## User intent

The Bootstrapper tab should help an editor user quickly establish a project baseline around a chosen map.

## Editor UI fields

Provide editable fields roughly equivalent to:

- runtime module name
- managed map name
- managed map template choice
- target map reference
- editor startup map reference
- game default map reference
- whether to make generated game mode the global default
- native `GameInstance` class name
- blueprint folder and blueprint name for `GameInstance`
- native `GameMode` class name
- blueprint folder and blueprint name for `GameMode`

Do not replicate the exact category names or labels from the old plugin; equivalent wording is enough.

## Supported actions

The tab should expose these operations:

1. Create managed map
2. Open managed map
3. Generate native code
4. Create blueprints and apply project settings
5. Reset form state

Optional:

- a collapsible help section
- result/status text

## Managed map rules

- Managed maps live under `/Game/Maps`.
- The tool should sanitize the user-entered map name into a valid asset name.
- The tool should reject invalid package paths.
- If the map already exists, `Create` should fail with a clear message.
- If the map does not exist, `Open` should fail with a clear message.
- Before opening or creating a map, save dirty maps with editor prompts.
- Support at least these templates:
  - Basic
  - Blank
  - Time Of Day

## Suggested defaults

When the tab opens, prepopulate fields based on:

- project name: `CodexInvenMedium`
- current editor world if one exists
- default managed map path if there is no valid current world

Suggested naming behavior:

- derive a project-specific `GameMode` stem from the project name and map name
- default `GameInstance` class name to a project-specific class
- default blueprint folders to reasonable content paths under `/Game/Blueprints`

The naming strategy may differ from the source plugin, but the generated names must be deterministic and valid.

## Native code generation behavior

When the user clicks the equivalent of `Generate Code`:

- validate runtime module name
- resolve the actual runtime module context for the project
- normalize requested class names to valid Unreal native class names
- create missing classes using Unreal project code generation APIs
- reuse existing classes when they already exist
- verify parent classes:
  - game instance class must derive from `UGameInstance`
  - game mode class must derive from `AGameModeBase`
- tell the user to wait if code compilation or hot reload is not finished yet

Important:

- This tool should not try to invent a second custom `GameInstance` base if `UCodexInvenMediumGameInstance` already exists and can be reused.
- Decide one of these implementation modes and document it in code comments:
  - Mode A: Bootstrapper generates arbitrary project classes from user input.
  - Mode B: Bootstrapper prefers the project's fixed runtime base classes and only generates map-specific `GameMode`.

Recommended for this project:

- Use Mode A for parity.
- However, default the `GameInstance` class field to `CodexInvenMediumGameInstance` so the input-linking feature has a stable receiver property.

## Blueprint generation and apply behavior

When the user clicks the equivalent of `Create Blueprints & Apply`:

- resolve the native classes and confirm they are loaded
- create or reuse blueprint assets for `GameInstance` and `GameMode`
- verify the reused blueprints inherit from the expected parent classes
- load the selected target map
- set the world settings default game mode for that map
- update `UGameMapsSettings`:
  - `GameInstanceClass`
  - `EditorStartupMap`
  - `GameDefaultMap`
  - optionally global default game mode
- save the changed map and generated blueprint packages

## Tool 2: Input Builder Specification

## User intent

The Input Builder tab should assist the user in building a conventional Enhanced Input setup with fixed standard actions.

## Managed standard actions

Implement exactly four managed actions:

1. `Move`
2. `Look`
3. `Jump`
4. `Fire`

These are the only actions the tool manages.

## Editor UI fields

Provide fields roughly equivalent to:

- selected input mapping context
- input action prefix
- input action folder
- input config folder
- input config asset name
- currently selected managed action
- binding drafts for actions that use simple key input

Result/status text should be shown after actions complete or fail.

## Default values

Use sane defaults similar in intent to:

- input action prefix: `IA_`
- input action folder: `/Game/Input/Actions`
- input config folder: `/Game/Input/Configs`
- input config asset name: `DA_DefaultInputConfig`

## Action value types

Use these required value types:

- `Move`: `Axis2D`
- `Look`: `Axis2D`
- `Jump`: `Boolean`
- `Fire`: `Boolean`

## Binding rules

### Move

Apply a preset mapping set:

- `D`
- `A` with X negation
- `W` with axis swizzle so it contributes on Y
- `S` with axis swizzle and Y negation
- `Gamepad_Left2D`

### Look

Apply a preset mapping set:

- `Mouse2D`
- `Gamepad_Right2D`

### Jump

Apply a default boolean binding draft:

- `SpaceBar`
- `Gamepad_FaceButton_Bottom`

### Fire

Apply a default boolean binding draft:

- `LeftMouseButton`
- `Gamepad_RightTrigger`

Implementation note:

- For strict parity with old behavior, `Jump` and `Fire` should always fall back to their default drafts when applying.
- If that behavior is undesirable, document any deliberate change because it alters parity.

## IMC analysis behavior

The tab should analyze the selected `UInputMappingContext` and answer:

- whether an IMC is selected
- how many null-action mappings are present
- which managed actions are already represented by valid mappings
- which managed actions are still available to add

Matching strategy:

- consider a managed action present when an IMC mapping references the canonical input action asset path for that action and the key is valid
- ignore mappings whose action is null, but count them for diagnostics

## Add/build action behavior

The main build action should:

1. validate that an IMC is selected
2. validate input action folder path
3. resolve which managed action should be added
4. create or load the canonical `UInputAction` asset for that action
5. force its value type to the managed standard
6. clean null mappings from the IMC
7. remove previous mappings for that action
8. apply the standard preset mappings
9. save the `UInputAction` package and the IMC package
10. show a summary message

## Runtime input config linking behavior

The link action should only be enabled when:

- an IMC is selected
- all four managed actions exist in the IMC
- input config folder and asset name resolve to a valid package path

When linking:

1. resolve project runtime class `UCodexInvenMediumInputConfigDataAsset`
2. create or load the configured input config data asset
3. load the canonical managed `InputAction` assets for all four actions
4. write the IMC and action references into the data asset properties
5. resolve the current project `GameInstance` blueprint from `UGameMapsSettings`
6. obtain its class default object
7. assign the input config asset into `DefaultInputConfig`
8. mark assets dirty and save them
9. show success notification and summary text

## Reflection and compatibility rules

To keep the tool robust, implement explicit reflection-based validation with actionable error messages.

### Data asset validation

Before writing into the input config asset, verify the class contains these object properties:

- `InputMappingContext`
- `MoveAction`
- `LookAction`
- `JumpAction`
- `FireAction`

Expected types:

- `UInputMappingContext*` for `InputMappingContext`
- `UInputAction*` for the rest

### Game instance validation

Before assigning runtime input config, verify:

- the configured game instance blueprint exists
- its generated class exists
- its class default object is valid
- it exposes `DefaultInputConfig`
- `DefaultInputConfig` accepts a type compatible with `UDataAsset` or the project-specific subclass

## Menu and Tab Registration

The plugin should add a new top-level submenu under the Level Editor main menu.

Minimum commands:

- open Bootstrapper tab
- open Input Builder tab

Recommended tab IDs:

- `OctoDen.Bootstrapper`
- `OctoDen.InputBuilder`

These IDs can be changed, but should remain stable once chosen.

## Settings Objects

Use transient UObject settings containers for each tab so that details panels can edit them directly.

Recommended rules:

- each tab owns one transient settings instance
- when the tab closes, release the settings object and details view references
- provide reset methods for both tabs

## Error Handling Requirements

Every user-triggered action must fail loudly and clearly when prerequisites are missing.

Required characteristics:

- use modal dialogs for blocking failures
- use notifications for successful operations
- update visible status text inside the tab after each operation
- never silently skip missing assets or invalid property names

## Asset and Naming Rules

Implement utility functions for:

- sanitizing asset names
- normalizing package folder paths
- building package paths and object paths
- optionally converting loose user strings into PascalCase native class names

Behavior requirements:

- remove or replace invalid package characters
- avoid empty names by using deterministic fallbacks
- prevent native class names from starting with digits
- normalize prefixes for generated input action names

## Automation Test Coverage

Add editor automation tests for the recreated plugin.

Minimum expected coverage:

1. menu registration test
2. input builder default settings test
3. IMC analysis test
4. jump repair or overwrite test
5. move preset mapping test
6. remove-and-readd action test

Recommended extra coverage:

7. bootstrapper default derivation test
8. asset path sanitization test
9. reflection validation failure test for missing `DefaultInputConfig`
10. reflection validation failure test for malformed input config asset class

## Implementation Phases

## Phase 1: Scaffolding

1. Create `Plugins/OctoDen` and the plugin descriptor.
2. Add the editor module and build rules.
3. Register the module in the project by regenerating project files if needed.
4. Make the plugin compile with an empty menu entry.

Definition of done:

- Unreal can discover the plugin.
- The editor target compiles.

## Phase 2: Runtime support

1. Add `UCodexInvenMediumInputConfigDataAsset`.
2. Add `UCodexInvenMediumGameInstance`.
3. Ensure both classes compile and are visible to reflection.

Definition of done:

- runtime classes can be loaded by path
- UHT runs cleanly

## Phase 3: Menu and tabs

1. Add top-level `OctoDen` menu.
2. Add Bootstrapper and Input Builder tab spawners.
3. Add details views and reset behavior.

Definition of done:

- both tabs open from the menu

## Phase 4: Bootstrapper service

1. Add settings object and default derivation logic.
2. Add managed map create/open flow.
3. Add native code generation flow.
4. Add blueprint generation and project-setting apply flow.

Definition of done:

- the tool can create a managed map
- the tool can create or reuse project classes and blueprints
- the selected map and project settings update correctly

## Phase 5: Input builder service

1. Add settings object and IMC analysis logic.
2. Add input action asset creation/loading.
3. Add mapping preset application.
4. Add runtime input config asset creation/loading.
5. Add assignment into `GameInstance` defaults.

Definition of done:

- the tool can fully build and link a standard four-action input setup

## Phase 6: Tests and polish

1. Add automation tests.
2. Refine status messaging.
3. Verify save prompts and package saving behavior.
4. Verify behavior against a fresh project state and an already-partially-configured state.

Definition of done:

- automation tests pass
- tool behavior is repeatable and idempotent where expected

## Concrete Files Expected to Be Added

At minimum, expect these new files:

```text
Plugins/OctoDen/OctoDen.uplugin
Plugins/OctoDen/Source/OctoDen/OctoDen.Build.cs
Plugins/OctoDen/Source/OctoDen/Public/OctoDenModule.h
Plugins/OctoDen/Source/OctoDen/Private/OctoDenModule.cpp
Plugins/OctoDen/Source/OctoDen/Private/... additional implementation files
Source/CodexInvenMedium/Input/CodexInvenMediumInputConfigDataAsset.h
Source/CodexInvenMedium/Input/CodexInvenMediumInputConfigDataAsset.cpp
Source/CodexInvenMedium/Core/CodexInvenMediumGameInstance.h
Source/CodexInvenMedium/Core/CodexInvenMediumGameInstance.cpp
```

Likely modified files:

```text
CodexInvenMedium.uproject
Source/CodexInvenMedium/CodexInvenMedium.Build.cs
Config/DefaultEngine.ini
```

`DefaultEngine.ini` may be modified by the tool during use rather than by initial implementation, but the implementation should be prepared for that.

## Acceptance Criteria

Implementation is complete only if all of the following are true.

### Discovery and build

- The plugin is visible in the Unreal Editor.
- The editor target builds successfully.
- The plugin loads without startup errors.

### Menu and tabs

- The Level Editor main menu contains `OctoDen`.
- Both tool tabs open and close without crashes.

### Bootstrapper

- A managed map can be created under `/Game/Maps`.
- An existing managed map can be reopened.
- Native runtime classes can be created or reused.
- Blueprint subclasses can be created or reused.
- Project map settings and game instance settings are applied correctly.

### Input Builder

- The tool can create or reuse `IA_Move`, `IA_Look`, `IA_Jump`, `IA_Fire`-style assets with the configured prefix.
- The selected IMC receives the expected mappings.
- Null mappings are cleaned.
- Reapplying an action replaces stale mappings.
- The runtime input config asset is created or reused and populated.
- The current game instance blueprint receives the `DefaultInputConfig` reference.

### Tests

- Automation tests compile.
- At least the minimum coverage set passes in editor automation.

## Known Project-Specific Risks

### Risk 1: Input-linking feature depends on a stable game instance base

If the bootstrapper generates an arbitrary custom `GameInstance` class name and the user does not use `CodexInvenMediumGameInstance` or another compatible subclass exposing `DefaultInputConfig`, the runtime input-link step will fail.

Mitigation:

- default Bootstrapper `GameInstance` class name to `CodexInvenMediumGameInstance`
- validate the `DefaultInputConfig` property before linking
- explain the failure in the UI

### Risk 2: Editor code generation may require compile/reload delay

After native class generation, Unreal may not have loaded the generated classes immediately.

Mitigation:

- disable the apply step until the generated classes can actually be loaded
- tell the user to wait for compilation/reload to finish

### Risk 3: Existing project settings may conflict

Users may already have maps, game modes, or game instance blueprints assigned.

Mitigation:

- make the apply step explicit
- show what will be changed
- only overwrite settings through the apply action

### Risk 4: Reflection property names are a hard contract

The input config linker depends on exact property names in the runtime classes.

Mitigation:

- define and keep stable the property names listed in this plan
- cover them with tests

## Suggested Implementation Prompt for the Next Conversation

Use this only as a starting brief for the next implementation turn:

> Add a new editor plugin named `OctoDen` to `CodexInvenMedium` following `Docs/OctoDen_ImplementationPlan.md`. Implement the plugin from scratch without copying code from the sibling project. Include the required runtime support classes `UCodexInvenMediumInputConfigDataAsset` and `UCodexInvenMediumGameInstance`, then build the Bootstrapper and Input Builder tabs, wire menu registration, and add automation tests.

## Final Notes

This project does not currently have the runtime types needed for full behavioral parity. The next implementation conversation should treat those runtime classes as part of the scope, not as optional future work.

If implementation time must be reduced, do not cut the runtime support classes. Cut optional help text, extra polish, or some automation breadth first. The minimum viable parity requires:

- plugin descriptor and editor module
- bootstrapper tab
- input builder tab
- runtime input config data asset class
- runtime game instance base with `DefaultInputConfig`
- project setting application
- core automation tests
