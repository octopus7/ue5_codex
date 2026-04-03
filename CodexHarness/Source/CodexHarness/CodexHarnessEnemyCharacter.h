#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CodexHarnessEnemyCharacter.generated.h"

class UCodexHarnessHealthComponent;
class UStaticMesh;
class UStaticMeshComponent;
struct FPropertyChangedEvent;

UCLASS()
class CODEXHARNESS_API ACodexHarnessEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACodexHarnessEnemyCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostLoad() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	void RefreshVisualMeshDefaults();

	UFUNCTION(BlueprintPure, Category = "Visual")
	UStaticMeshComponent* GetVisualMeshComponent() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	UCodexHarnessHealthComponent* GetHealthComponent() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsAlive() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<UStaticMesh> DefaultVisualMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	FVector DefaultVisualMeshScale = FVector(6.0f, 6.0f, 6.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	FVector DefaultVisualMeshOffset = FVector(0.0f, 0.0f, -36.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float FollowRange = 3000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float AttackRange = 140.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float AttackDamage = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float AttackInterval = 1.0f;

private:
	void TickChaseAndAttack();
	void HandleDeath();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> VisualMeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCodexHarnessHealthComponent> HealthComponent = nullptr;

	float NextAttackTime = 0.0f;
};
