#include "CodexInvenAttendanceDayEntryWidget.h"

#include "Blueprint/WidgetTree.h"
#include "CodexInvenInventoryIconSubsystem.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/GameInstance.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateColor.h"

namespace
{
	const TCHAR* ClaimedText = TEXT("\uC218\uB839 \uC644\uB8CC");
	const TCHAR* ClaimableText = TEXT("\uC624\uB298 \uC218\uB839");
	const TCHAR* LockedText = TEXT("\uC7A0\uAE08");
	const TCHAR* AttendanceDayCardLockedTexturePath = TEXT("/Game/UI/Attendance/T_AttendanceDayCardLocked.T_AttendanceDayCardLocked");
	const TCHAR* AttendanceDayCardClaimableTexturePath = TEXT("/Game/UI/Attendance/T_AttendanceDayCardClaimable.T_AttendanceDayCardClaimable");
	const TCHAR* AttendanceDayCardClaimedTexturePath = TEXT("/Game/UI/Attendance/T_AttendanceDayCardClaimed.T_AttendanceDayCardClaimed");

	UTexture2D* GetEntryBackgroundTexture(const ECodexInvenAttendanceDayState InState)
	{
		static TWeakObjectPtr<UTexture2D> LockedTexture;
		static TWeakObjectPtr<UTexture2D> ClaimableTexture;
		static TWeakObjectPtr<UTexture2D> ClaimedTexture;

		TWeakObjectPtr<UTexture2D>* TextureSlot = nullptr;
		const TCHAR* TexturePath = nullptr;

		switch (InState)
		{
		case ECodexInvenAttendanceDayState::Claimed:
			TextureSlot = &ClaimedTexture;
			TexturePath = AttendanceDayCardClaimedTexturePath;
			break;

		case ECodexInvenAttendanceDayState::Claimable:
			TextureSlot = &ClaimableTexture;
			TexturePath = AttendanceDayCardClaimableTexturePath;
			break;

		case ECodexInvenAttendanceDayState::Locked:
		default:
			TextureSlot = &LockedTexture;
			TexturePath = AttendanceDayCardLockedTexturePath;
			break;
		}

		if (TextureSlot != nullptr && !TextureSlot->IsValid())
		{
			*TextureSlot = LoadObject<UTexture2D>(nullptr, TexturePath);
		}

		return TextureSlot != nullptr ? TextureSlot->Get() : nullptr;
	}

	FLinearColor GetCardFallbackColor(const ECodexInvenAttendanceDayState InState)
	{
		switch (InState)
		{
		case ECodexInvenAttendanceDayState::Claimed:
			return FLinearColor(0.82f, 0.94f, 0.90f, 1.0f);

		case ECodexInvenAttendanceDayState::Claimable:
			return FLinearColor(0.97f, 0.91f, 0.72f, 1.0f);

		case ECodexInvenAttendanceDayState::Locked:
		default:
			return FLinearColor(0.88f, 0.92f, 0.98f, 1.0f);
		}
	}

	FLinearColor GetTitleColor(const ECodexInvenAttendanceDayState InState)
	{
		switch (InState)
		{
		case ECodexInvenAttendanceDayState::Claimed:
			return FLinearColor(0.08f, 0.56f, 0.46f, 1.0f);

		case ECodexInvenAttendanceDayState::Claimable:
			return FLinearColor(0.72f, 0.48f, 0.10f, 1.0f);

		case ECodexInvenAttendanceDayState::Locked:
		default:
			return FLinearColor(0.18f, 0.22f, 0.30f, 1.0f);
		}
	}

	FLinearColor GetRewardPanelColor(const ECodexInvenAttendanceDayState InState)
	{
		switch (InState)
		{
		case ECodexInvenAttendanceDayState::Claimed:
			return FLinearColor(0.96f, 1.0f, 0.98f, 0.78f);

		case ECodexInvenAttendanceDayState::Claimable:
			return FLinearColor(1.0f, 0.98f, 0.94f, 0.80f);

		case ECodexInvenAttendanceDayState::Locked:
		default:
			return FLinearColor(0.96f, 0.97f, 1.0f, 0.76f);
		}
	}

	FLinearColor GetRewardTextColor(const ECodexInvenAttendanceDayState InState)
	{
		static_cast<void>(InState);
		return FLinearColor(0.09f, 0.11f, 0.16f, 0.98f);
	}

	FLinearColor GetStateBadgeColor(const ECodexInvenAttendanceDayState InState)
	{
		switch (InState)
		{
		case ECodexInvenAttendanceDayState::Claimed:
			return FLinearColor(0.10f, 0.48f, 0.40f, 0.96f);

		case ECodexInvenAttendanceDayState::Claimable:
			return FLinearColor(0.78f, 0.60f, 0.16f, 0.98f);

		case ECodexInvenAttendanceDayState::Locked:
		default:
			return FLinearColor(0.18f, 0.22f, 0.30f, 0.96f);
		}
	}

	FLinearColor GetStateTextColor(const ECodexInvenAttendanceDayState InState)
	{
		switch (InState)
		{
		case ECodexInvenAttendanceDayState::Claimable:
			return FLinearColor(0.99f, 0.99f, 0.97f, 1.0f);

		case ECodexInvenAttendanceDayState::Claimed:
		case ECodexInvenAttendanceDayState::Locked:
		default:
			return FLinearColor(0.96f, 0.98f, 1.0f, 1.0f);
		}
	}

	FText GetStateText(const ECodexInvenAttendanceDayState InState)
	{
		switch (InState)
		{
		case ECodexInvenAttendanceDayState::Claimed:
			return FText::FromString(FString(ClaimedText));

		case ECodexInvenAttendanceDayState::Claimable:
			return FText::FromString(FString(ClaimableText));

		case ECodexInvenAttendanceDayState::Locked:
		default:
			return FText::FromString(FString(LockedText));
		}
	}
}

void UCodexInvenAttendanceDayEntryWidget::SetPresentationData(const FCodexInvenAttendanceDayPresentationData& InPresentationData)
{
	PresentationData = InPresentationData;
	bHasPresentationData = true;
	BuildWidgetTreeIfNeeded();
	RefreshVisualState();
}

void UCodexInvenAttendanceDayEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SetIsFocusable(false);
	BuildWidgetTreeIfNeeded();
	RefreshVisualState();
}

void UCodexInvenAttendanceDayEntryWidget::BuildWidgetTreeIfNeeded()
{
	if (WidgetTree == nullptr)
	{
		WidgetTree = NewObject<UWidgetTree>(this, TEXT("AttendanceDayEntryWidgetTree"));
	}

	if (RootBorder != nullptr)
	{
		return;
	}

	RootBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("RootBorder"));
	RootBorder->SetPadding(FMargin(10.0f, 10.0f, 10.0f, 8.0f));
	WidgetTree->RootWidget = RootBorder;

	UVerticalBox* const ContentBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ContentBox"));
	RootBorder->SetContent(ContentBox);

	DayNumberTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DayNumberTextBlock"));
	FSlateFontInfo DayNumberFont = DayNumberTextBlock->GetFont();
	DayNumberFont.Size = 22;
	DayNumberTextBlock->SetFont(DayNumberFont);
	DayNumberTextBlock->SetJustification(ETextJustify::Right);
	if (UVerticalBoxSlot* const DaySlot = ContentBox->AddChildToVerticalBox(DayNumberTextBlock))
	{
		DaySlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 8.0f));
		DaySlot->SetHorizontalAlignment(HAlign_Right);
	}

	USizeBox* const RewardIconSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("RewardIconSizeBox"));
	RewardIconSizeBox->SetWidthOverride(48.0f);
	RewardIconSizeBox->SetHeightOverride(48.0f);
	RewardIconImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("RewardIconImage"));
	RewardIconSizeBox->SetContent(RewardIconImage);
	if (UVerticalBoxSlot* const IconSlot = ContentBox->AddChildToVerticalBox(RewardIconSizeBox))
	{
		IconSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 10.0f));
		IconSlot->SetHorizontalAlignment(HAlign_Center);
	}

	RewardPanelBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("RewardPanelBorder"));
	RewardPanelBorder->SetPadding(FMargin(8.0f, 8.0f, 8.0f, 10.0f));
	if (UVerticalBoxSlot* const RewardPanelSlot = ContentBox->AddChildToVerticalBox(RewardPanelBorder))
	{
		RewardPanelSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		RewardPanelSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 10.0f));
	}

	RewardTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("RewardTextBlock"));
	RewardTextBlock->SetAutoWrapText(true);
	RewardTextBlock->SetJustification(ETextJustify::Center);
	RewardTextBlock->SetShadowColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.06f));
	RewardTextBlock->SetShadowOffset(FVector2D(0.0f, 1.0f));
	RewardPanelBorder->SetContent(RewardTextBlock);

	StateBadgeBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("StateBadgeBorder"));
	StateBadgeBorder->SetPadding(FMargin(10.0f, 4.0f));
	if (UVerticalBoxSlot* const BadgeSlot = ContentBox->AddChildToVerticalBox(StateBadgeBorder))
	{
		BadgeSlot->SetHorizontalAlignment(HAlign_Center);
	}

	StateTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("StateTextBlock"));
	FSlateFontInfo StateFont = StateTextBlock->GetFont();
	StateFont.Size = 11;
	StateTextBlock->SetFont(StateFont);
	StateTextBlock->SetJustification(ETextJustify::Center);
	StateBadgeBorder->SetContent(StateTextBlock);
}

void UCodexInvenAttendanceDayEntryWidget::RefreshVisualState()
{
	if (!bHasPresentationData || RootBorder == nullptr || RewardPanelBorder == nullptr || StateBadgeBorder == nullptr ||
		DayNumberTextBlock == nullptr || RewardTextBlock == nullptr || StateTextBlock == nullptr || RewardIconImage == nullptr)
	{
		return;
	}

	if (UTexture2D* const BackgroundTexture = GetEntryBackgroundTexture(PresentationData.State))
	{
		RootBorder->SetBrushFromTexture(BackgroundTexture);
		RootBorder->SetBrushColor(FLinearColor::White);
	}
	else
	{
		RootBorder->SetBrushFromTexture(nullptr);
		RootBorder->SetBrushColor(GetCardFallbackColor(PresentationData.State));
	}

	RewardPanelBorder->SetBrushColor(GetRewardPanelColor(PresentationData.State));
	StateBadgeBorder->SetBrushColor(GetStateBadgeColor(PresentationData.State));

	DayNumberTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%02d"), PresentationData.DayNumber)));
	DayNumberTextBlock->SetColorAndOpacity(FSlateColor(GetTitleColor(PresentationData.State)));

	if (UTexture2D* const RewardIconTexture = GetRewardIconTexture())
	{
		RewardIconImage->SetBrushFromTexture(RewardIconTexture, true);
		RewardIconImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		RewardIconImage->SetVisibility(ESlateVisibility::Collapsed);
	}

	RewardTextBlock->SetText(PresentationData.RewardText);
	RewardTextBlock->SetColorAndOpacity(FSlateColor(GetRewardTextColor(PresentationData.State)));
	StateTextBlock->SetText(GetStateText(PresentationData.State));
	StateTextBlock->SetColorAndOpacity(FSlateColor(GetStateTextColor(PresentationData.State)));
}

UTexture2D* UCodexInvenAttendanceDayEntryWidget::GetRewardIconTexture() const
{
	const UGameInstance* const GameInstance = GetGameInstance();
	if (GameInstance == nullptr)
	{
		return nullptr;
	}

	const UCodexInvenInventoryIconSubsystem* const IconSubsystem =
		GameInstance->GetSubsystem<UCodexInvenInventoryIconSubsystem>();
	return IconSubsystem != nullptr
		? IconSubsystem->GetInventoryIcon(PresentationData.Reward.PickupType)
		: nullptr;
}
