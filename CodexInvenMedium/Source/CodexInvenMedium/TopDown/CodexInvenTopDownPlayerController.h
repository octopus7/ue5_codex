#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CodexInvenTopDownPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class CODEXINVENMEDIUM_API ACodexInvenTopDownPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACodexInvenTopDownPlayerController();

	virtual void BeginPlay() override;

	UInputMappingContext* GetDefaultInputMappingContext() const;

private:
	void ApplyDefaultInputMapping();
};
