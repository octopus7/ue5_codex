#pragma once

#include "CoreMinimal.h"
#include "EditorReimportHandler.h"
#include "Factories/Factory.h"
#include "VoxStaticMeshFactory.generated.h"

class UStaticMesh;

UCLASS()
class VOXIMPORTEREDITOR_API UVoxStaticMeshFactory : public UFactory, public FReimportHandler
{
	GENERATED_BODY()

public:
	UVoxStaticMeshFactory(const FObjectInitializer& ObjectInitializer);

	virtual bool FactoryCanImport(const FString& Filename) override;
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;

	virtual bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
	virtual void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;
	virtual EReimportResult::Type Reimport(UObject* Obj) override;

private:
	UObject* ImportFromFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, FFeedbackContext* Warn, bool& bOutOperationCanceled, UStaticMesh* ExistingMesh = nullptr);
};
