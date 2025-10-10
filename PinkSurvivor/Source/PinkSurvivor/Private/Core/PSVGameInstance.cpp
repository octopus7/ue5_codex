#include "Core/PSVGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Save/PSVSaveGame.h"

UPSVGameInstance::UPSVGameInstance()
{
    SaveSlotName = TEXT("PlayerProgress");
    SaveUserIndex = 0;
}

void UPSVGameInstance::Init()
{
    Super::Init();

    LoadOrCreateSaveGame();
}

int32 UPSVGameInstance::GetPersistentGold() const
{
    return ActiveSave ? ActiveSave->PersistentGold : 0;
}

void UPSVGameInstance::AddPersistentGold(int32 Amount)
{
    if (Amount <= 0)
    {
        return;
    }

    if (!ActiveSave)
    {
        LoadOrCreateSaveGame();
    }

    if (!ActiveSave)
    {
        return;
    }

    ActiveSave->PersistentGold = FMath::Max(0, ActiveSave->PersistentGold + Amount);
    InternalSaveGame();
}

void UPSVGameInstance::SavePersistentData()
{
    InternalSaveGame();
}

void UPSVGameInstance::LoadOrCreateSaveGame()
{
    if (ActiveSave)
    {
        return;
    }

    if (USaveGame* LoadedSave = UGameplayStatics::LoadGameFromSlot(SaveSlotName.ToString(), SaveUserIndex))
    {
        ActiveSave = Cast<UPSVSaveGame>(LoadedSave);
    }

    if (!ActiveSave)
    {
        ActiveSave = Cast<UPSVSaveGame>(UGameplayStatics::CreateSaveGameObject(UPSVSaveGame::StaticClass()));
        InternalSaveGame();
    }
}

void UPSVGameInstance::InternalSaveGame()
{
    if (!ActiveSave)
    {
        return;
    }

    UGameplayStatics::SaveGameToSlot(ActiveSave, SaveSlotName.ToString(), SaveUserIndex);
}
