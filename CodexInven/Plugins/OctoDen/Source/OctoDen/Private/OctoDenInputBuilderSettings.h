#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "OctoDenInputBuilderSettings.generated.h"

class UInputMappingContext;

UENUM()
enum class EOctoDenStandardInputAction : uint8
{
	Move UMETA(DisplayName = "Move"),
	Look UMETA(DisplayName = "Look"),
	Jump UMETA(DisplayName = "Jump"),
	Fire UMETA(DisplayName = "Fire")
};

USTRUCT()
struct FOctoDenInputBindingDraft
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Bindings")
	FKey PrimaryKey;

	UPROPERTY(EditAnywhere, Category = "Bindings")
	FKey SecondaryKey;

	UPROPERTY(EditAnywhere, Category = "Bindings")
	FKey GamepadKey;

	bool HasAnyValidKey() const;
	TArray<FKey> GetUniqueValidKeys() const;
};

struct FOctoDenManagedInputActionState
{
	EOctoDenStandardInputAction Action = EOctoDenStandardInputAction::Move;
	int32 ValidMappingCount = 0;

	bool IsAdded() const
	{
		return ValidMappingCount > 0;
	}
};

struct FOctoDenManagedInputAnalysis
{
	bool bHasSelectedInputMappingContext = false;
	int32 NullActionMappingCount = 0;
	TArray<FOctoDenManagedInputActionState> ActionStates;

	TArray<EOctoDenStandardInputAction> GetAvailableActions() const;
	const FOctoDenManagedInputActionState* FindActionState(EOctoDenStandardInputAction InAction) const;
	bool HasAvailableActions() const;
};

UCLASS(Transient)
class UOctoDenInputBuilderSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Input Builder|IMC")
	TObjectPtr<UInputMappingContext> SelectedInputMappingContext = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input Builder|IA")
	FString InputActionPrefix = TEXT("IA_");

	UPROPERTY(EditAnywhere, Category = "Input Builder|IA", meta = (ToolTip = "Content browser folder path. Example: /Game/Input/Actions"))
	FString InputActionFolder = TEXT("/Game/Input/Actions");

	UPROPERTY(Transient)
	EOctoDenStandardInputAction SelectedAction = EOctoDenStandardInputAction::Move;

	UPROPERTY(EditAnywhere, Transient, Category = "Input Builder|Draft")
	FOctoDenInputBindingDraft JumpBindings;

	UPROPERTY(EditAnywhere, Transient, Category = "Input Builder|Draft")
	FOctoDenInputBindingDraft FireBindings;

	static TArray<EOctoDenStandardInputAction> GetAllStandardActions();
	static FText GetStandardActionDisplayText(EOctoDenStandardInputAction InAction);
	static FString GetStandardActionStem(EOctoDenStandardInputAction InAction);
	static EInputActionValueType GetStandardActionValueType(EOctoDenStandardInputAction InAction);
	static bool UsesPresetBindings(EOctoDenStandardInputAction InAction);
	static FText GetPresetBindingSummary(EOctoDenStandardInputAction InAction);

	FOctoDenManagedInputAnalysis AnalyzeSelectedInputMappingContext() const;
	bool ResolveSelectedAction(EOctoDenStandardInputAction& OutAction) const;
	bool HasSelectedInputMappingContext() const;
	bool CanAddSelectedAction(FText* OutFailReason = nullptr) const;
	bool SelectedActionUsesPresetBindings() const;

	FString GetCanonicalInputActionName(EOctoDenStandardInputAction InAction) const;
	FString GetCanonicalInputActionPackagePath(EOctoDenStandardInputAction InAction) const;
	FString GetCanonicalInputActionObjectPath(EOctoDenStandardInputAction InAction) const;

	const FOctoDenInputBindingDraft& GetBindingDraft(EOctoDenStandardInputAction InAction) const;
	FOctoDenInputBindingDraft& GetBindingDraft(EOctoDenStandardInputAction InAction);
};
