# Interaction Message Popup Implementation Plan

## Document Purpose
- This document defines only the implementation plan for a simple UMG message popup feature.
- It does not perform actual C++ edits, asset generation, commandlet execution, or map placement work.
- The popup is intended to open from a world interaction and route its result back through the existing unified interaction subsystem.

## Fixed Prerequisite Rules
- The WBP workflow must follow [wbp_asset_work_guidelines_EN.md](./wbp_asset_work_guidelines_EN.md).
- Therefore, the popup UI must exist as a real `Widget Blueprint` asset rather than being assembled as a widget tree in runtime code.
- WBP creation/update logic must live in editor-module code such as `CodexUMGBootstrapEditor`, and the execution entry point is fixed to a `Commandlet`.
- If the current project's `UnrealEditor.exe` is already running and causes a commandlet conflict, stop immediately without trying a workaround and notify the user.

## Target State
1. When the player approaches a wooden sign actor, the existing interaction marker system shows the interaction label `View`.
2. When the player performs the interaction action, a simple message popup opens in a limited central screen area.
3. The popup uses a common dialog layout with a title bar, body message, and close button.
4. Inside the panel, a background blur is visible, a low-opacity sky-blue tint is layered over it, and the panel corners are rounded.
5. The popup button layout can be selected in code as either `OK` or `Yes/No`.
6. Clicking the close button closes the popup.
7. Pressing `Space` also closes the popup.
8. The `OK`, `Yes`, `No`, and `Closed` results are all delivered to `UCodexInteractionSubsystem` first.
9. The test wooden sign actor BP uses the existing mesh `/Game/Vox/Meshes/Props/SM_Vox_WoodenSignpost`.
10. Both the popup WBP and the test wooden sign BP must be generatable through the editor-module code + commandlet path.

## Current Project Touchpoints
- The interaction entry point already exists as `UCodexInteractionSubsystem` in [Source/CodexUMG/Public/Interaction/CodexInteractionSubsystem.h](../Source/CodexUMG/Public/Interaction/CodexInteractionSubsystem.h).
- World-target interaction data is already handled by `UCodexInteractionComponent` in [Source/CodexUMG/Public/Interaction/CodexInteractionComponent.h](../Source/CodexUMG/Public/Interaction/CodexInteractionComponent.h).
- The current interaction request structure is `FCodexInteractionRequest` in [Source/CodexUMG/Public/Interaction/CodexInteractionTypes.h](../Source/CodexUMG/Public/Interaction/CodexInteractionTypes.h).
- Align popup asset path constants with [Source/CodexUMG/Public/Interaction/CodexInteractionAssetPaths.h](../Source/CodexUMG/Public/Interaction/CodexInteractionAssetPaths.h).
- The base class for test interactable actors can reuse `ACodexInteractableActor` in [Source/CodexUMG/Public/Interaction/CodexInteractableActor.h](../Source/CodexUMG/Public/Interaction/CodexInteractableActor.h).
- Extend the popup widget C++ base through [Source/CodexUMG/Public/Interaction/CodexInteractionMessagePopupWidget.h](../Source/CodexUMG/Public/Interaction/CodexInteractionMessagePopupWidget.h) and [Source/CodexUMG/Private/Interaction/CodexInteractionMessagePopupWidget.cpp](../Source/CodexUMG/Private/Interaction/CodexInteractionMessagePopupWidget.cpp).
- Check the popup-capable interactable base class against [Source/CodexUMG/Public/Interaction/CodexPopupInteractableActor.h](../Source/CodexUMG/Public/Interaction/CodexPopupInteractableActor.h) and [Source/CodexUMG/Private/Interaction/CodexPopupInteractableActor.cpp](../Source/CodexUMG/Private/Interaction/CodexPopupInteractableActor.cpp).
- The existing `Widget Blueprint` generation pattern can follow the `UWidgetBlueprintFactory` flow in [Source/CodexUMGBootstrapEditor/Private/Interaction/CodexInteractionAssetBuilder.cpp](../Source/CodexUMGBootstrapEditor/Private/Interaction/CodexInteractionAssetBuilder.cpp).
- The existing input-binding extension pattern can follow [Source/CodexUMG/Private/CodexTopDownPlayerController.cpp](../Source/CodexUMG/Private/CodexTopDownPlayerController.cpp) and [Source/CodexUMG/Public/CodexTopDownInputConfigDataAsset.h](../Source/CodexUMG/Public/CodexTopDownInputConfigDataAsset.h).
- Both the runtime and editor modules already depend on `UMG`, so the first implementation pass mainly needs to add the new UI structure and interaction flow.
- Some existing interaction widget references still use `_C` class-path patterns, but this popup plan uses the WBP asset path as the source-of-truth reference according to [wbp_asset_work_guidelines_EN.md](./wbp_asset_work_guidelines_EN.md).

## Core Principles
- The popup is viewport UMG added with `AddToViewport`, not a `UWidgetComponent` attached to the world.
- The popup widget tree must be stored inside the real `WBP_InteractionMessagePopup` asset.
- Code is responsible only for data injection, show/hide behavior, button-layout switching, and result routing.
- Every `OK` / `Yes` / `No` / `Closed` result must be received by `UCodexInteractionSubsystem` first and then routed to the target actor or another follow-up handler.
- In the first implementation pass, allow only one popup at a time. Queues, stacking, and multiple modals are out of scope.
- While a popup is open, new interaction requests should either be ignored or held until the current popup closes.
- In the first implementation pass, the wooden sign can keep `Use` as the interaction type enum. The user-visible interaction name is solved with `PromptText = "View"`.
- Because the popup has clickable buttons, opening it requires the player controller to manage a `Game and UI`-style input mode and mouse cursor visibility.

## Deliverables

### Runtime C++ deliverables
- `ECodexPopupButtonLayout`
  - `Ok`
  - `YesNo`
- `ECodexPopupResult`
  - `Ok`
  - `Yes`
  - `No`
  - `Closed`
- `FCodexInteractionPopupRequest`
  - bundles title, message, button layout, requester, target actor, and source interaction component
- `FCodexInteractionPopupResponse`
  - bundles request ID, result enum, requester, and target actor
- `UCodexInteractionMessagePopupWidget`
  - C++ base class of the real `WBP_InteractionMessagePopup`
- `UCodexInteractionSubsystem` extension
  - opening popups, tracking the active popup, popup close requests, result intake, and result routing
- popup-capable interaction actor extension
  - recommended: `ACodexPopupInteractableActor : public ACodexInteractableActor`
  - the wooden sign BP uses this class as its parent
- `UCodexTopDownInputConfigDataAsset` extension
  - add `PopupCloseAction`
- `ACodexTopDownPlayerController` extension
  - bind `PopupCloseAction` and forward active-popup close requests

### Editor / commandlet deliverables
- popup WBP creation/update code inside `CodexUMGBootstrapEditor`
- wooden sign BP creation/update code inside `CodexUMGBootstrapEditor`
- extension of the existing [Source/CodexUMGBootstrapEditor/Private/Commandlets/CodexInteractionAssetBuildCommandlet.cpp](../Source/CodexUMGBootstrapEditor/Private/Commandlets/CodexInteractionAssetBuildCommandlet.cpp)
  - for the first pass, extending the existing interaction asset build commandlet is preferred over adding a separate new commandlet

### Real content deliverables
- `/Game/UI/Interaction/WBP_InteractionMessagePopup`
- `/Game/Blueprints/Interaction/BP_Interactable_WoodenSignPopup`
- `/Game/Input/Actions/IA_PopupClose`
- update `/Game/Input/Contexts/IMC_TopDown`
- update `/Game/Data/Input/DA_TopDownInputConfig`
- update test wooden sign placement in `/Game/Maps/BasicMap` if needed

## Recommended Data Structures

### 1. `ECodexPopupButtonLayout`
- `Ok`
  - show only the `OK` button in the bottom button row
- `YesNo`
  - show `Yes` and `No` in the bottom button row

### 2. `ECodexPopupResult`
- `Ok`
  - `OK` button click
- `Yes`
  - `Yes` button click
- `No`
  - `No` button click
- `Closed`
  - title-bar close button or `Space` close

### 3. `FCodexInteractionPopupRequest`
- recommended fields
  - `FGuid RequestId`
  - `FText Title`
  - `FText Message`
  - `ECodexPopupButtonLayout ButtonLayout`
  - `TWeakObjectPtr<APlayerController> RequestingController`
  - `TWeakObjectPtr<AActor> SourceActor`
  - `TWeakObjectPtr<UCodexInteractionComponent> SourceInteractionComponent`
- purpose
  - deliver all popup display and result-routing context in one object

### 4. `FCodexInteractionPopupResponse`
- recommended fields
  - `FGuid RequestId`
  - `ECodexPopupResult Result`
  - `TWeakObjectPtr<APlayerController> RequestingController`
  - `TWeakObjectPtr<AActor> SourceActor`
  - `TWeakObjectPtr<UCodexInteractionComponent> SourceInteractionComponent`
- purpose
  - let `UCodexInteractionSubsystem` interpret the result and route follow-up handling

## Recommended Runtime Structure

### 1. `UCodexInteractionSubsystem`
- responsibilities
  - receive existing interaction requests
  - determine whether the current focused target is a popup interaction target
  - build popup requests
  - create and display the active popup instance
  - process `Space`-based close requests
  - receive button-click results
  - route the result to the target actor or another follow-up handler
- recommended new functions
  - `bool OpenInteractionPopup(const FCodexInteractionPopupRequest& Request)`
  - `void SubmitInteractionPopupResult(const FCodexInteractionPopupResponse& Response)`
  - `void RequestCloseActivePopup(APlayerController* RequestingController)`
  - `bool HasActivePopup() const`
- state-management rules
  - keep only one active popup
  - if a popup is already active, `RequestInteraction()` does not open another one
  - when the popup closes, clean up internal state and restore the prior input mode

### 2. `UCodexInteractionMessagePopupWidget`
- role
  - C++ base class of `WBP_InteractionMessagePopup`
  - handles widget state reflection, not widget generation
- responsibilities
  - apply title/body text
  - switch button layout
  - forward close, `OK`, `Yes`, and `No` button clicks to the subsystem
  - optionally handle a simple fade-in when first shown
- non-responsibilities
  - widget tree generation
  - interaction target selection
  - directly consuming the result
- implementation rules
  - all button variants must physically exist in the WBP, and code only switches `Visibility`
  - the source-of-truth reference is the WBP asset path
  - `UClass` resolution happens only right before `CreateWidget`

### 3. `ACodexPopupInteractableActor`
- purpose
  - common base for interaction targets that open popup messages
- recommended owned data
  - `PopupTitle`
  - `PopupMessage`
  - `ButtonLayout`
- recommended behavior
  - do not immediately destroy or consume inside `HandleInteractionRequested`
  - instead, provide the request context used to build `FCodexInteractionPopupRequest`
  - receive the popup result through a `BlueprintImplementableEvent` or `BlueprintNativeEvent` and react afterward

### 4. Wooden sign BP
- recommended name
  - `BP_Interactable_WoodenSignPopup`
- recommended parent class
  - `ACodexPopupInteractableActor`
- default values
  - Static Mesh: `/Game/Vox/Meshes/Props/SM_Vox_WoodenSignpost`
  - InteractionType: `Use`
  - PromptText: `View`
  - PopupTitle: e.g. `Notice`
  - PopupMessage: e.g. a test sign message
  - ButtonLayout: `Ok` or `YesNo`

## WBP Structure Specification

### Real asset name
- `/Game/UI/Interaction/WBP_InteractionMessagePopup`

### Structure rules
- The widget tree below must physically exist inside the WBP asset.
- Runtime C++ does not create this tree. It only binds and controls existing widgets.

### Recommended widget tree
- `RootCanvas`
  - full-screen root
- `Overlay_ScreenRoot`
  - central alignment layer
- `SizeBox_PopupFrame`
  - popup max-width limiter
- `Overlay_PanelRoot`
  - visual panel and content overlap
- `BackgroundBlur_Panel`
  - blur only within the panel area
- `Border_SkyTintPanel`
  - low-opacity sky-blue tint layer
- `VerticalBox_Content`
  - title bar / body / button row layout
- `HorizontalBox_TitleBar`
  - `TXT_Title`
  - `BTN_Close`
- `TXT_Message`
  - message body
- `HorizontalBox_ButtonRow`
  - `BTN_Ok`
  - `BTN_Yes`
  - `BTN_No`

### Recommended BindWidget targets
- `TXT_Title`
- `TXT_Message`
- `BTN_Close`
- `BTN_Ok`
- `BTN_Yes`
- `BTN_No`
- `HorizontalBox_ButtonRow`

## Visual Design Specification

### Layout
- The popup stays fixed in the screen center.
- Do not use a full-screen black modal backdrop.
- Use a structure where only the central panel pulls focus.

### Panel size
- target width `480 ~ 560`
- target height `220 ~ 320`
- overly wide full-screen boxes are not allowed

### Panel style
- Behind the panel, the background must appear blurred through `BackgroundBlur`.
- The blur should be limited to the central panel area rather than the entire screen.
- A low-opacity sky-blue tint is layered over the blur.
  - for example: sky-blue RGB + `Alpha 0.12 ~ 0.20`
- The corners must look clearly rounded.
  - recommended radius `16 ~ 24`
- The title bar and body should feel visually separated, but without excessive ornament.

### Button style
- `OK`, `Yes`, `No`, and close buttons should follow the same design family.
- In the first implementation pass, hover/pressed states can stay simple, but there should be at least a visible distinction between normal and hover.
- In `YesNo` layout, hide `OK`.
- In `Ok` layout, hide `Yes` and `No`.

## Input And Interaction Flow
1. When the player approaches the wooden sign, the existing interaction marker shows `View`.
2. When the player presses the interaction input, `ACodexTopDownPlayerController` still only calls `UCodexInteractionSubsystem::RequestInteraction()`.
3. `UCodexInteractionSubsystem` determines whether the currently focused interaction target is a popup-capable actor.
4. If it is, it builds `FCodexInteractionPopupRequest` and opens the popup.
5. The subsystem loads the popup asset through the WBP asset path, resolves the widget class only at the final usage point, and then calls `CreateWidget` and `AddToViewport`.
6. When the popup opens, the player controller shows the mouse cursor and switches input mode so popup clicking is possible.
7. If the user presses `BTN_Close` or `Space`, a `Closed` result is generated.
8. If the user presses `OK`, `Yes`, or `No`, the corresponding result enum is generated.
9. The result always enters `UCodexInteractionSubsystem::SubmitInteractionPopupResult()` first.
10. The subsystem optionally forwards the result to the target actor, closes the popup, and restores the input mode and cursor state.

## Input Asset Plan

### 1. New input action
- `/Game/Input/Actions/IA_PopupClose`
- value type: `Boolean`
- default key: `Space`

### 2. Input mapping
- add `IA_PopupClose -> Space` to the existing `/Game/Input/Contexts/IMC_TopDown`
- add a `PopupCloseAction` reference to `/Game/Data/Input/DA_TopDownInputConfig`

### 3. Player controller binding
- add `BindTopDownPopupCloseAction()` to [Source/CodexUMG/Private/CodexTopDownPlayerController.cpp](../Source/CodexUMG/Private/CodexTopDownPlayerController.cpp) following the existing binding pattern
- `HandlePopupCloseInput()` calls `UCodexInteractionSubsystem::RequestCloseActivePopup(this)` only when an active popup exists
- if no popup is active, it does nothing

## Editor Module / Commandlet Plan

### 1. WBP generation rules
- `WBP_InteractionMessagePopup` is created/updated as a real asset by editor-module code.
- Its `WidgetTree` must physically contain the root, blur, title, message, and button widgets defined above.
- Do not create a fallback runtime path that builds the widget tree.

### 2. BP generation rules
- `BP_Interactable_WoodenSignPopup` is created/updated by editor-module code.
- Its parent class is `ACodexPopupInteractableActor`.
- Its mesh is `/Game/Vox/Meshes/Props/SM_Vox_WoodenSignpost`.
- `PromptText` defaults to `View`.

### 3. Commandlet entry
- In the first implementation pass, the recommended approach is to extend the existing `UCodexInteractionAssetBuildCommandlet` with these steps:
  1. create/update the popup WBP
  2. create/update `IA_PopupClose`
  3. update `IMC_TopDown` and `DA_TopDownInputConfig`
  4. create/update the wooden sign BP
  5. update test placement in `BasicMap` if needed

### 4. Editor conflict handling
- If the current project editor is already running during commandlet execution, fail immediately.
- Do not try workaround paths after the failure.
- Only tell the user that the editor must be closed before rerunning.

## Completion Criteria
- `WBP_InteractionMessagePopup` exists as a real asset.
- The widget tree is visible inside that WBP asset.
- The wooden sign BP exists as a real asset and uses `SM_Vox_WoodenSignpost`.
- The wooden sign interaction prompt is `View`.
- Interacting opens a central popup.
- The popup can be closed by both the close button and `Space`.
- The button layout can be selected in code as `Ok` or `YesNo`.
- `OK`, `Yes`, `No`, and `Closed` are all received by `UCodexInteractionSubsystem` first.
- Creation/update happens only through the editor-module + commandlet path.

## Verification Checklist
1. Opening `WBP_InteractionMessagePopup` in the UMG designer shows the real title bar, message, close button, button row, and blur-panel structure.
2. The source-of-truth WBP reference does not store an `_C` class path.
3. `BP_Interactable_WoodenSignPopup` uses `SM_Vox_WoodenSignpost` as its mesh.
4. Near the wooden sign, the existing interaction prompt text appears as `View`.
5. On interaction input, the popup occupies only a limited central area.
6. The background behind the panel appears blurred.
7. A low-opacity sky-blue tint is visibly layered over the panel.
8. The panel corners appear rounded.
9. In `Ok` layout, only `OK` is visible.
10. In `YesNo` layout, only `Yes` and `No` are visible.
11. Pressing `Space` closes the popup and sends `Closed`.
12. Button-click results reach `UCodexInteractionSubsystem` before they reach the actor.
13. If a commandlet/editor conflict occurs, the process stops immediately with no workaround attempt.
