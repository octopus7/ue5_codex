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
	static const TCHAR* const SmileIconTextureName = TEXT("T_InteractionSmileYellow");
	static const TCHAR* const TileRoundedGradientTextureName = TEXT("T_InteractionTileRoundedVerticalGradient");
	static const TCHAR* const IndicatorWidgetName = TEXT("WBP_InteractionIndicator");
	static const TCHAR* const MessagePopupWidgetName = TEXT("WBP_InteractionMessagePopup");
	static const TCHAR* const ScrollMessagePopupWidgetName = TEXT("WBP_InteractionScrollMessagePopup");
	static const TCHAR* const DualTileTransferPopupWidgetName = TEXT("WBP_InteractionDualTileTransferPopup");
	static const TCHAR* const DualTileTransferTileEntryWidgetName = TEXT("WBP_InteractionDualTileTransferTileEntry");
	static const TCHAR* const GeminiFlashSimplePopupWidgetName = TEXT("WBP_SimplePopup");
	static const TCHAR* const InteractActionName = TEXT("IA_Interact");
	static const TCHAR* const PopupCloseActionName = TEXT("IA_PopupClose");
	static const TCHAR* const MappingContextName = TEXT("IMC_TopDown");
	static const TCHAR* const InputConfigName = TEXT("DA_TopDownInputConfig");
	static const TCHAR* const InteractableAppleName = TEXT("BP_Interactable_Apple");
	static const TCHAR* const InteractableStrawberryName = TEXT("BP_Interactable_Strawberry");
	static const TCHAR* const InteractableWoodenSignPopupName = TEXT("BP_Interactable_WoodenSignPopup");
	static const TCHAR* const InteractableWoodenSignScrollPopupName = TEXT("BP_Interactable_WoodenSignScrollPopup");
	static const TCHAR* const InteractableWoodenSignDualTileTransferPopupName = TEXT("BP_Interactable_WoodenSignDualTileTransferPopup");

	static const TCHAR* const FilledCircleTextureObjectPath = TEXT("/Game/UI/Interaction/T_InteractionFilledCircle.T_InteractionFilledCircle");
	static const TCHAR* const OuterRingTextureObjectPath = TEXT("/Game/UI/Interaction/T_InteractionOuterRing.T_InteractionOuterRing");
	static const TCHAR* const SmileIconTextureObjectPath = TEXT("/Game/UI/Interaction/T_InteractionSmileYellow.T_InteractionSmileYellow");
	static const TCHAR* const TileRoundedGradientTextureObjectPath = TEXT("/Game/UI/Interaction/T_InteractionTileRoundedVerticalGradient.T_InteractionTileRoundedVerticalGradient");
	static const TCHAR* const IndicatorWidgetObjectPath = TEXT("/Game/UI/Interaction/WBP_InteractionIndicator.WBP_InteractionIndicator");
	static const TCHAR* const MessagePopupWidgetObjectPath = TEXT("/Game/UI/Interaction/WBP_InteractionMessagePopup.WBP_InteractionMessagePopup");
	static const TCHAR* const ScrollMessagePopupWidgetObjectPath = TEXT("/Game/UI/Interaction/WBP_InteractionScrollMessagePopup.WBP_InteractionScrollMessagePopup");
	static const TCHAR* const DualTileTransferPopupWidgetObjectPath = TEXT("/Game/UI/Interaction/WBP_InteractionDualTileTransferPopup.WBP_InteractionDualTileTransferPopup");
	static const TCHAR* const DualTileTransferTileEntryWidgetObjectPath = TEXT("/Game/UI/Interaction/WBP_InteractionDualTileTransferTileEntry.WBP_InteractionDualTileTransferTileEntry");
	static const TCHAR* const GeminiFlashSimplePopupWidgetObjectPath = TEXT("/Game/UI/GeminiFlash/WBP_SimplePopup.WBP_SimplePopup");
	static const TCHAR* const InteractActionObjectPath = TEXT("/Game/Input/Actions/IA_Interact.IA_Interact");
	static const TCHAR* const PopupCloseActionObjectPath = TEXT("/Game/Input/Actions/IA_PopupClose.IA_PopupClose");
	static const TCHAR* const MappingContextObjectPath = TEXT("/Game/Input/Contexts/IMC_TopDown.IMC_TopDown");
	static const TCHAR* const InputConfigObjectPath = TEXT("/Game/Data/Input/DA_TopDownInputConfig.DA_TopDownInputConfig");
	static const TCHAR* const AppleMeshObjectPath = TEXT("/Game/Vox/Meshes/Food/SM_Vox_Apple.SM_Vox_Apple");
	static const TCHAR* const StrawberryMeshObjectPath = TEXT("/Game/Vox/Meshes/Food/SM_Vox_Strawberry.SM_Vox_Strawberry");
	static const TCHAR* const WoodenSignMeshObjectPath = TEXT("/Game/Vox/Meshes/Props/SM_Vox_WoodenSignpost.SM_Vox_WoodenSignpost");

	inline FString MakeGeneratedClassObjectPath(const TCHAR* AssetObjectPath)
	{
		return FString(AssetObjectPath) + TEXT("_C");
	}
}
