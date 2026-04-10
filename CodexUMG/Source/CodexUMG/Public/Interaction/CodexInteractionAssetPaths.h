// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

namespace CodexInteractionAssetPaths
{
	static const TCHAR* const UIPath = TEXT("/Game/UI/Interaction");
	static const TCHAR* const InputActionsPath = TEXT("/Game/Input/Actions");
	static const TCHAR* const InputContextsPath = TEXT("/Game/Input/Contexts");
	static const TCHAR* const InputDataPath = TEXT("/Game/Data/Input");
	static const TCHAR* const BlueprintsPath = TEXT("/Game/Blueprints/Interaction");

	static const TCHAR* const FilledCircleTextureName = TEXT("T_InteractionFilledCircle");
	static const TCHAR* const OuterRingTextureName = TEXT("T_InteractionOuterRing");
	static const TCHAR* const IndicatorWidgetName = TEXT("WBP_InteractionIndicator");
	static const TCHAR* const InteractActionName = TEXT("IA_Interact");
	static const TCHAR* const MappingContextName = TEXT("IMC_TopDown");
	static const TCHAR* const InputConfigName = TEXT("DA_TopDownInputConfig");
	static const TCHAR* const InteractableAppleName = TEXT("BP_Interactable_Apple");
	static const TCHAR* const InteractableStrawberryName = TEXT("BP_Interactable_Strawberry");

	static const TCHAR* const FilledCircleTextureObjectPath = TEXT("/Game/UI/Interaction/T_InteractionFilledCircle.T_InteractionFilledCircle");
	static const TCHAR* const OuterRingTextureObjectPath = TEXT("/Game/UI/Interaction/T_InteractionOuterRing.T_InteractionOuterRing");
	static const TCHAR* const IndicatorWidgetObjectPath = TEXT("/Game/UI/Interaction/WBP_InteractionIndicator.WBP_InteractionIndicator");
	static const TCHAR* const IndicatorWidgetClassObjectPath = TEXT("/Game/UI/Interaction/WBP_InteractionIndicator.WBP_InteractionIndicator_C");
	static const TCHAR* const InteractActionObjectPath = TEXT("/Game/Input/Actions/IA_Interact.IA_Interact");
	static const TCHAR* const MappingContextObjectPath = TEXT("/Game/Input/Contexts/IMC_TopDown.IMC_TopDown");
	static const TCHAR* const InputConfigObjectPath = TEXT("/Game/Data/Input/DA_TopDownInputConfig.DA_TopDownInputConfig");
	static const TCHAR* const AppleMeshObjectPath = TEXT("/Game/Vox/Meshes/Food/SM_Vox_Apple.SM_Vox_Apple");
	static const TCHAR* const StrawberryMeshObjectPath = TEXT("/Game/Vox/Meshes/Food/SM_Vox_Strawberry.SM_Vox_Strawberry");
}
