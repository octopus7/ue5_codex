Korean: [README_KO.md](./README_KO.md)

# Project Bootstrapper

Project Bootstrapper is an Unreal Engine editor plugin that helps bootstrap a project with project-native startup classes and map defaults.

It provides a single workflow to:

- create or reopen a managed map under `/Game/Maps`
- generate native `GameInstance` and `GameMode` classes in the target runtime module
- create matching Blueprint assets from those native classes
- assign the selected map, editor startup map, game default map, and optional global default game mode

This repository currently hosts the plugin as a project plugin under `Plugins/ProjectBootstrapper`.

## Plugin Type

- Category: `Editor`
- Module type: `Editor`
- Current version: `1.0.0`

## Where To Find It In The Editor

After the plugin is enabled, open:

- `Tools > Custom Tools > Project Bootstrapper`

## Main Workflow

1. Enter a managed map name and create or open the map.
2. Confirm the runtime module and the native class names.
3. Click `Generate Code`.
4. Wait until Unreal finishes compiling or reloading the module if needed.
5. Click `Create Blueprints & Apply`.

## Key Inputs

- `TargetRuntimeModule`: runtime module that will receive generated native classes
- `ManagedMapName`: name used for `/Game/Maps/<Name>`
- `ManagedMapTemplate`: `Basic`, `Blank`, or `TimeOfDay`
- `TargetMap`: map that receives the generated `GameMode`
- `EditorStartupMap`: editor startup map
- `GameDefaultMap`: game default map
- `bSetAsGlobalDefaultGameMode`: optionally sets the generated `GameMode` as the global default
- `GameInstanceBlueprintFolder` and `GameInstanceBlueprintName`
- `GameModeBlueprintFolder` and `GameModeBlueprintName`

## Repository Layout

- `Plugins/ProjectBootstrapper`: plugin root
- `Plugins/ProjectBootstrapper/Source`: C++ source
- `Plugins/ProjectBootstrapper/Resources`: icons and help text files

## Binary Distribution

This plugin is intended to be distributed as a binary plugin package.

See [DEPLOYMENT.md](./DEPLOYMENT.md) for the packaging, validation, and release steps.
For the Korean version, see [DEPLOYMENT_KO.md](./DEPLOYMENT_KO.md).
