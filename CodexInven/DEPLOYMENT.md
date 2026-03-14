Korean: [DEPLOYMENT_KO.md](./DEPLOYMENT_KO.md)

# Binary Plugin Deployment

This document describes how to package and distribute `ProjectBootstrapper` as a binary Unreal Engine plugin.

For the plugin overview and editor workflow, see [README.md](./README.md).
For the Korean overview, see [README_KO.md](./README_KO.md).

## Goal

Create a packaged plugin that another Unreal Engine project can install without copying this full repository.

## Recommended Distribution Format

Ship the packaged output as a versioned archive such as:

- `ProjectBootstrapper-1.0.0-UE5.x-Win64.zip`

Include:

- `.uplugin`
- `Binaries/`
- `Resources/`
- `Content/` if the plugin adds content in the future
- `Config/` if the plugin adds plugin-level config in the future

Do not include:

- `Intermediate/`
- `.sln`
- project-level `Binaries/`
- project `Saved/`

## Packaging Methods

### Method 1: Unreal Editor

Use the plugin packaging action from the Unreal Editor when available:

1. Open the host project in Unreal Editor.
2. Open the Plugins window.
3. Locate `Project Bootstrapper`.
4. Run `Package`.
5. Choose an output directory for the packaged plugin.

This is the simplest manual path for local release builds.

### Method 2: RunUAT BuildPlugin

Use `RunUAT BuildPlugin` for repeatable packaging.

Example:

```powershell
Engine\Build\BatchFiles\RunUAT.bat BuildPlugin `
  -Plugin="D:\github\ue5_codex\CodexInven\Plugins\ProjectBootstrapper\ProjectBootstrapper.uplugin" `
  -Package="D:\Builds\ProjectBootstrapper" `
  -TargetPlatforms=Win64
```

Notes:

- Replace `Engine\Build\BatchFiles\RunUAT.bat` with the actual Unreal Engine installation path.
- Keep the package output outside the project directory when possible.
- Add more target platforms only if the plugin is tested on them.

## Release Checklist

Before publishing a package:

1. Confirm the plugin version in `ProjectBootstrapper.uplugin`.
2. Package the plugin for the intended engine version and platform.
3. Install the packaged output into a clean Unreal project under `Plugins/ProjectBootstrapper`.
4. Launch the editor and verify the plugin loads without recompiling.
5. Verify the menu entry appears at `Tools > Custom Tools > Project Bootstrapper`.
6. Run the full bootstrap flow: create or open a managed map, generate code, create blueprints, and apply project defaults.
7. Archive the packaged folder as a release artifact.

## Installation In Another Project

1. Unzip the packaged plugin.
2. Copy the `ProjectBootstrapper` folder into the target project's `Plugins/` directory.
3. Open the target project in Unreal Editor.
4. Enable the plugin if Unreal does not enable it automatically.
5. Restart the editor if prompted.

Expected result:

- the plugin loads from its packaged binaries
- the menu entry is available in the editor

## Versioning Guidance

Keep the binary package aligned with:

- Unreal Engine major and minor version
- target platform
- plugin `Version` and `VersionName`

Binary plugin compatibility is not guaranteed across different engine versions, so package and label releases per supported UE version.

## Suggested Release Artifact Layout

```text
ProjectBootstrapper-1.0.0-UE5.x-Win64.zip
  ProjectBootstrapper/
    Binaries/
    Resources/
    ProjectBootstrapper.uplugin
```

## Current Plugin Source

Source plugin path in this repository:

- `Plugins/ProjectBootstrapper`
