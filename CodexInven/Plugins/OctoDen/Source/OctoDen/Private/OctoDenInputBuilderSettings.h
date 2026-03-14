#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "OctoDenInputBuilderSettings.generated.h"

USTRUCT()
struct FOctoDenInputActionRow
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Action")
	FString ActionName;

	UPROPERTY(EditAnywhere, Category = "Action")
	EInputActionValueType ValueType = EInputActionValueType::Boolean;

	UPROPERTY(EditAnywhere, Category = "Bindings")
	FKey PrimaryKey;

	UPROPERTY(EditAnywhere, Category = "Bindings")
	FKey SecondaryKey;

	UPROPERTY(EditAnywhere, Category = "Bindings")
	FKey GamepadKey;
};

UCLASS(Transient)
class UOctoDenInputBuilderSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Input Builder|IMC")
	FString InputMappingContextPrefix = TEXT("IMC_");

	UPROPERTY(EditAnywhere, Category = "Input Builder|IMC")
	FString InputMappingContextName = TEXT("Default");

	UPROPERTY(EditAnywhere, Category = "Input Builder|IMC", meta = (ToolTip = "Content browser folder path. Example: /Game/Input/Contexts"))
	FString InputMappingContextFolder = TEXT("/Game/Input/Contexts");

	UPROPERTY(EditAnywhere, Category = "Input Builder|IA")
	FString InputActionPrefix = TEXT("IA_");

	UPROPERTY(EditAnywhere, Category = "Input Builder|IA", meta = (ToolTip = "Content browser folder path. Example: /Game/Input/Actions"))
	FString InputActionFolder = TEXT("/Game/Input/Actions");

	UPROPERTY(EditAnywhere, Category = "Input Builder|Rows")
	TArray<FOctoDenInputActionRow> Actions;
};
