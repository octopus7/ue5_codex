#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyConeCharacter.generated.h"

class UStaticMeshComponent;

// 간이 적 캐릭터: 뒤집힌 콘 모양의 시각화만 제공
UCLASS(meta=(DisplayName="EnemyConeCharacter_임시"))
class CODEXWAVE_API AEnemyConeCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyConeCharacter();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UStaticMeshComponent* VisualMesh;
};

