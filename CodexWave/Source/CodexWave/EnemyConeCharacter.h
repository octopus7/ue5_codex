#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyConeCharacter.generated.h"

class UStaticMeshComponent;
class UTextRenderComponent;

// 간이 적 캐릭터: 뒤집힌 콘 모양의 시각화만 제공
UCLASS(meta=(DisplayName="EnemyConeCharacter_임시"))
class CODEXWAVE_API AEnemyConeCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyConeCharacter();

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void UnPossessed() override;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UStaticMeshComponent* VisualMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UTextRenderComponent* HPText;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UTextRenderComponent* LifeText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
    int32 HitsToDie = 3;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Health")
    int32 CurrentHits = 0;

    // 자동 사망 시간(초). 0이면 자동 사망하지 않음. 기본 0초.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lifetime", meta=(ClampMin="0.0"))
    float AutoDeathTime = 0.0f;

    void HandleDeath();
    void UpdateHPText();
    void UpdateLifetimeText();

    UPROPERTY(Transient)
    class AEnemyAIController* CachedAIController = nullptr;
};
