#pragma once

#include "PrototypeMeshBuilderTypes.h"

namespace UE::Geometry
{
	class FDynamicMesh3;
}

class ADynamicMeshActor;
class IPrototypeMeshBridge;
class UWorld;

class FPrototypeMeshBuilderController : public TSharedFromThis<FPrototypeMeshBuilderController>
{
public:
	FPrototypeMeshBuilderController();
	~FPrototypeMeshBuilderController();

	void SetPrompt(const FString& InPrompt);
	void SetAssetName(const FString& InAssetName);
	void SetContentPath(const FString& InContentPath);
	void SetReasoningEffort(const FString& InReasoningEffort);

	const FString& GetPrompt() const;
	const FString& GetAssetName() const;
	const FString& GetContentPath() const;
	const FString& GetReasoningEffort() const;
	FText GetStatusText() const;
	bool CanSave() const;

	void Generate();
	void Save();
	void Clear();
	void CleanupPreview();

private:
	void ResetGeneratedState();
	void SetStatus(const FString& InStatus);
	bool EnsurePreviewActor(FString& OutError);
	UWorld* GetEditorWorld() const;

private:
	TUniquePtr<IPrototypeMeshBridge> Bridge;
	FString Prompt;
	FString AssetName;
	FString ContentPath;
	FString ReasoningEffort;
	FString StatusMessage;
	FPrototypeShapeDsl CurrentDsl;
	FGeneratedMeshBuffers CurrentBuffers;
	TUniquePtr<UE::Geometry::FDynamicMesh3> CurrentDynamicMesh;
	TWeakObjectPtr<ADynamicMeshActor> PreviewActor;
	bool bHasGeneratedResult = false;
};
