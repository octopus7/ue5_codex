#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PSVGameOverWidget.generated.h"

class UButton;

/**
 * 게임 오버 화면에서 OK 버튼으로 일시정지를 해제하는 위젯
 */
UCLASS()
class PINKSURVIVOR_API UPSVGameOverWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
    TObjectPtr<UButton> OkButton = nullptr;

private:
    UFUNCTION()
    void HandleOkClicked();

    void ResumeGame();
};
