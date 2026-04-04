#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CMWTopDownPlayerController.generated.h"

class UUserWidget;

UCLASS()
class CODEXMAZEWALKER_API ACMWTopDownPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACMWTopDownPlayerController();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnPossess(APawn* InPawn) override;

	bool GetAimWorldLocation(FVector& OutAimWorldLocation) const;

protected:
	void InitializeInputMappingContext() const;
	void EnsureMinimapWidget();
	bool ResolveCursorAimLocation(FVector& OutAimWorldLocation) const;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> MinimapWidget;

	UPROPERTY(Transient)
	FVector CachedAimWorldLocation = FVector::ZeroVector;

	UPROPERTY(Transient)
	bool bHasAimWorldLocation = false;
};
