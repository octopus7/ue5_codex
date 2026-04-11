// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interaction/CodexDualTileTransferPopupInteractableActor.h"

ACodexDualTileTransferPopupInteractableActor::ACodexDualTileTransferPopupInteractableActor()
{
	SetPopupTitle(FText::FromString(TEXT("번호 이동")));
	SetPopupMessage(FText::FromString(TEXT("좌우 패널 사이에서 숫자를 이동합니다.")));
	SetPopupButtonLayout(ECodexPopupButtonLayout::Ok);
}

ECodexInteractionPopupStyle ACodexDualTileTransferPopupInteractableActor::GetPopupStyle() const
{
	return ECodexInteractionPopupStyle::DualTileTransfer;
}

bool ACodexDualTileTransferPopupInteractableActor::AllowsPopupControllerClose() const
{
	return false;
}

void ACodexDualTileTransferPopupInteractableActor::PopulatePopupRequest(FCodexInteractionPopupRequest& PopupRequest) const
{
	Super::PopulatePopupRequest(PopupRequest);
	PopupRequest.LeftNumbers = LeftNumbers;
	PopupRequest.RightNumbers = RightNumbers;
	PopupRequest.bAllowDuplicateNumbers = bAllowDuplicateNumbers;
}

void ACodexDualTileTransferPopupInteractableActor::SetLeftNumbers(const TArray<int32>& NewLeftNumbers)
{
	LeftNumbers = NewLeftNumbers;
}

void ACodexDualTileTransferPopupInteractableActor::SetRightNumbers(const TArray<int32>& NewRightNumbers)
{
	RightNumbers = NewRightNumbers;
}

void ACodexDualTileTransferPopupInteractableActor::SetAllowDuplicateNumbers(const bool bInAllowDuplicateNumbers)
{
	bAllowDuplicateNumbers = bInAllowDuplicateNumbers;
}
