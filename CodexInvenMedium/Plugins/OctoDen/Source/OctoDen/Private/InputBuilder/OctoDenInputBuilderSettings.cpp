#include "InputBuilder/OctoDenInputBuilderSettings.h"

#include "InputBuilder/InputBuilderService.h"
#include "Shared/OctoDenAssetNaming.h"

TArray<FKey> FOctoDenKeyDraft::GetUniqueValidKeys() const
{
	TArray<FKey> Result;
	if (PrimaryKey.IsValid())
	{
		Result.Add(PrimaryKey);
	}
	if (SecondaryKey.IsValid() && SecondaryKey != PrimaryKey)
	{
		Result.Add(SecondaryKey);
	}
	return Result;
}

bool FOctoDenKeyDraft::HasAnyValidKey() const
{
	return PrimaryKey.IsValid() || SecondaryKey.IsValid();
}

void UOctoDenInputBuilderSettings::ResetToDefaults()
{
	SelectedInputMappingContext = nullptr;
	InputActionPrefix = TEXT("IA_");
	InputActionFolder = TEXT("/Game/Input/Actions");
	InputConfigFolder = TEXT("/Game/Input/Configs");
	InputConfigAssetName = TEXT("DA_DefaultInputConfig");
	ManagedAction = EOctoDenManagedInputAction::Move;
	JumpDraft = MakeDefaultDraft(EOctoDenManagedInputAction::Jump);
	FireDraft = MakeDefaultDraft(EOctoDenManagedInputAction::Fire);
	LastStatus = FText::GetEmpty();
}

EInputActionValueType UOctoDenInputBuilderSettings::GetManagedValueType(const EOctoDenManagedInputAction ManagedAction)
{
	switch (ManagedAction)
	{
	case EOctoDenManagedInputAction::Move:
	case EOctoDenManagedInputAction::Look:
		return EInputActionValueType::Axis2D;
	case EOctoDenManagedInputAction::Jump:
	case EOctoDenManagedInputAction::Fire:
	default:
		return EInputActionValueType::Boolean;
	}
}

FText UOctoDenInputBuilderSettings::GetManagedActionLabel(const EOctoDenManagedInputAction ManagedAction)
{
	switch (ManagedAction)
	{
	case EOctoDenManagedInputAction::Move:
		return NSLOCTEXT("OctoDenInputBuilder", "MoveLabel", "Move");
	case EOctoDenManagedInputAction::Look:
		return NSLOCTEXT("OctoDenInputBuilder", "LookLabel", "Look");
	case EOctoDenManagedInputAction::Jump:
		return NSLOCTEXT("OctoDenInputBuilder", "JumpLabel", "Jump");
	case EOctoDenManagedInputAction::Fire:
	default:
		return NSLOCTEXT("OctoDenInputBuilder", "FireLabel", "Fire");
	}
}

bool UOctoDenInputBuilderSettings::UsesPresetMappings(const EOctoDenManagedInputAction ManagedAction)
{
	return ManagedAction == EOctoDenManagedInputAction::Move || ManagedAction == EOctoDenManagedInputAction::Look;
}

FOctoDenKeyDraft UOctoDenInputBuilderSettings::MakeDefaultDraft(const EOctoDenManagedInputAction ManagedAction)
{
	FOctoDenKeyDraft Draft;
	if (ManagedAction == EOctoDenManagedInputAction::Jump)
	{
		Draft.PrimaryKey = EKeys::SpaceBar;
		Draft.SecondaryKey = EKeys::Gamepad_FaceButton_Bottom;
	}
	else if (ManagedAction == EOctoDenManagedInputAction::Fire)
	{
		Draft.PrimaryKey = EKeys::LeftMouseButton;
		Draft.SecondaryKey = EKeys::Gamepad_RightTrigger;
	}
	return Draft;
}

FString UOctoDenInputBuilderSettings::GetCanonicalActionAssetName(const EOctoDenManagedInputAction InManagedAction) const
{
	const FString Prefix = OctoDenAssetNaming::SanitizeAssetName(InputActionPrefix, TEXT("IA_"));
	return Prefix + UOctoDenInputBuilderSettings::GetManagedActionLabel(InManagedAction).ToString();
}
