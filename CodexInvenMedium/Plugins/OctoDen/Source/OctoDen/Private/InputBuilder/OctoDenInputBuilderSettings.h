#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
#include "UObject/Object.h"
#include "OctoDenInputBuilderSettings.generated.h"

class UInputMappingContext;

UENUM()
enum class EOctoDenManagedInputAction : uint8
{
	Move,
	Look,
	Jump,
	Fire
};

USTRUCT()
struct FOctoDenKeyDraft
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Bindings")
	FKey PrimaryKey;

	UPROPERTY(EditAnywhere, Category = "Bindings")
	FKey SecondaryKey;

	TArray<FKey> GetUniqueValidKeys() const;
	bool HasAnyValidKey() const;
};

UCLASS(Transient)
class UOctoDenInputBuilderSettings : public UObject
{
	GENERATED_BODY()

public:
	void ResetToDefaults();

	static EInputActionValueType GetManagedValueType(EOctoDenManagedInputAction ManagedAction);
	static FText GetManagedActionLabel(EOctoDenManagedInputAction ManagedAction);
	static bool UsesPresetMappings(EOctoDenManagedInputAction ManagedAction);
	static FOctoDenKeyDraft MakeDefaultDraft(EOctoDenManagedInputAction ManagedAction);
	FString GetCanonicalActionAssetName(EOctoDenManagedInputAction ManagedAction) const;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> SelectedInputMappingContext = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	FString InputActionPrefix = TEXT("IA_");

	UPROPERTY(EditAnywhere, Category = "Input")
	FString InputActionFolder = TEXT("/Game/Input/Actions");

	UPROPERTY(EditAnywhere, Category = "Input")
	FString InputConfigFolder = TEXT("/Game/Input/Configs");

	UPROPERTY(EditAnywhere, Category = "Input")
	FString InputConfigAssetName = TEXT("DA_DefaultInputConfig");

	UPROPERTY(EditAnywhere, Category = "Input")
	EOctoDenManagedInputAction ManagedAction = EOctoDenManagedInputAction::Move;

	UPROPERTY(EditAnywhere, Category = "Bindings")
	FOctoDenKeyDraft JumpDraft;

	UPROPERTY(EditAnywhere, Category = "Bindings")
	FOctoDenKeyDraft FireDraft;

	UPROPERTY(VisibleAnywhere, Category = "Status")
	FText LastStatus;
};

USTRUCT()
struct FOctoDenInputContextAnalysis
{
	GENERATED_BODY()

	bool bHasSelection = false;
	int32 NullActionMappingCount = 0;
	TArray<EOctoDenManagedInputAction> PresentActions;
	TArray<EOctoDenManagedInputAction> AvailableActions;
};

USTRUCT()
struct FOctoDenManagedActionBuildResult
{
	GENERATED_BODY()

	int32 RemovedNullMappings = 0;
	int32 RemovedExistingMappings = 0;
	int32 AddedMappings = 0;
	bool bCreatedInputAction = false;
};
