#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CodexHarnessPlayerController.generated.h"

class UCodexHarnessInputConfigDataAsset;
struct FInputActionValue;

UCLASS()
class CODEXHARNESS_API ACodexHarnessPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACodexHarnessPlayerController();

	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;

protected:
	UFUNCTION(BlueprintPure, Category = "Input")
	const UCodexHarnessInputConfigDataAsset* GetInputConfig() const;

	void ApplyInputMappingContext();
	void HandleMove(const FInputActionValue& InputValue);
	void HandleFireStarted();
	void HandleRestartStarted();
	bool TryGetCursorGroundPoint(FVector& OutWorldPoint) const;
	void UpdateAimFromCursor() const;

	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	int32 InputMappingPriority = 0;
};
