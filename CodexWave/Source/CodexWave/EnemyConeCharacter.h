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
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UStaticMeshComponent* VisualMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UTextRenderComponent* HPText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
    int32 HitsToDie = 3;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Health")
    int32 CurrentHits = 0;

    void HandleDeath();
    void UpdateHPText();
};
