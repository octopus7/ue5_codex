# Clock Widget Task Template

Use this template in another conversation when you want the same kind of non-MVVM clock widget implemented again.

## Variables

- `<EnginePath>`: Unreal Engine installation path
- `<ProjectRoot>`: project root path
- `<ProjectName>`: project name
  - Example: `CodexInven`
- `<ProjectEditorModuleName>`: the editor module name for the project
  - Usually `<ProjectName>Editor`
  - Example: `CodexInvenEditor`
- `<ProjectUprojectPath>`: full `.uproject` path
- `<CommandletName>`: commandlet name for one-off asset generation

## Request Template

```text
This is an Unreal Engine 5.7.4 project. The engine path is <EnginePath>, and all build/run steps must use that exact engine path.
First verify the engine version again from AGENTS.md, <ProjectName>.uproject, and Source/*.Target.cs.

In this document:
- <ProjectName> is the project name. Example: CodexInven
- <ProjectEditorModuleName> is the editor module name for that project. It is usually <ProjectName>Editor. Example: CodexInvenEditor

Do not migrate an existing widget. Implement a brand new regular clock widget.

Requirements:
- Create one runtime C++ widget class derived from UUserWidget
- Do not put widget tree/layout construction code inside the runtime class
- The runtime class must use BindWidget only for the required widgets, and only update the time text plus hour/minute/second hand angles
- Show both a digital clock and an analog hand clock
- The analog clock must use a circular clock face
- There must be 12 tick marks, and the 3/6/9/12 directions should be visually stronger
- Place the widget at the top center of the screen
- Use a warm color for the rectangular container background
- Create a new WBP that inherits from the C++ parent widget class
- Build the WBP widget tree with editor-only one-off code
- Put the shared WBP tree inspection utility in permanent/common code
- Put WBP creation/update code in a transient folder so it can be removed later
- If drawing a circular clock face directly is inconvenient, generating a texture and using it in an Image widget is acceptable
- Load the WBP from the player controller and add it to the viewport
- Do not use Python Remote Execution, external Python scripts, or DefaultEngine.ini changes

Transient code execution:
- Implement one-off editor code as a commandlet or other editor-only scaffold
- If you use a commandlet, include the actual command line used to run it
- Command format:
  "<EnginePath>\\Engine\\Binaries\\Win64\\UnrealEditor-Cmd.exe" "<ProjectUprojectPath>" -run=<CommandletName> -unattended -nop4 -nosplash -NoSound
- Example:
  "<EnginePath>\\Engine\\Binaries\\Win64\\UnrealEditor-Cmd.exe" "<ProjectUprojectPath>" -run=<ProjectName>ClockWidgetScaffold -unattended -nop4 -nosplash -NoSound

Deliverables:
- runtime widget class
- player controller hookup
- shared WBP tree inspection utility
- one-off WBP/texture generation commandlet or editor scaffold
- /Game/UI/WBP_CodexClock
- /Game/UI/T_ClockFace_Analog if needed

Verification:
- build <ProjectEditorModuleName>
- run the commandlet above to generate the WBP
- confirm 0 error(s), 0 warning(s)
- if the editor is not running after the build, open the project editor
```

## Current Project Example

- `<EnginePath>`: your local Unreal Engine `5.7` installation path
- `<ProjectRoot>`: `D:\github\ue5_codex\CodexInven`
- `<ProjectName>`: `CodexInven`
- `<ProjectEditorModuleName>`: `CodexInvenEditor`
- `<ProjectUprojectPath>`: `D:\github\ue5_codex\CodexInven\CodexInven.uproject`
- `<CommandletName>`: `CodexInvenClockWidgetScaffold`

Example run:

```powershell
& "<EnginePath>\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" `
  "D:\github\ue5_codex\CodexInven\CodexInven.uproject" `
  -run=CodexInvenClockWidgetScaffold `
  -unattended -nop4 -nosplash -NoSound
```

## Reference Locations

- runtime widget: `Source/CodexInven/CodexInvenClockWidget.h`, `Source/CodexInven/CodexInvenClockWidget.cpp`
- player controller hookup: `Source/CodexInven/CodexInvenTopDownPlayerController.h`, `Source/CodexInven/CodexInvenTopDownPlayerController.cpp`
- shared tree utility: `Source/CodexInvenEditor/Public/WidgetBlueprint/CodexInvenWidgetBlueprintTreeUtils.h`, `Source/CodexInvenEditor/Private/WidgetBlueprint/CodexInvenWidgetBlueprintTreeUtils.cpp`
- transient scaffold: `Source/CodexInvenEditor/Private/Transient/ClockWidgetScaffolding/CodexInvenClockWidgetScaffoldCommandlet.h`, `Source/CodexInvenEditor/Private/Transient/ClockWidgetScaffolding/CodexInvenClockWidgetScaffoldCommandlet.cpp`
