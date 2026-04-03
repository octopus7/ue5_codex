#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CodexHarnessInputConfigDataAsset.generated.h"

class UInputAction;
class UInputMappingContext;

UENUM(BlueprintType)
enum class ECodexHarnessConfiguredInputAction : uint8
{
	Move,
	Look,
	Fire,
	Restart
};

UCLASS(BlueprintType)
class CODEXHARNESS_API UCodexHarnessInputConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> FireAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> RestartAction = nullptr;

	UFUNCTION(BlueprintPure, Category = "Input")
	const UInputAction* GetInputAction(ECodexHarnessConfiguredInputAction InAction) const;

	UFUNCTION(BlueprintPure, Category = "Input")
	bool HasRequiredBindings() const;
};
