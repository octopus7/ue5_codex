#pragma once

#include "Misc/Optional.h"
#include "PrototypeMeshBuilderTypes.h"

namespace UE::Geometry
{
	class FDynamicMesh3;
}

class AActor;
class ADynamicMeshActor;
class IPrototypeMeshBridge;
class UDynamicMeshComponent;

class FPrototypeMeshBuilderController : public TSharedFromThis<FPrototypeMeshBuilderController>
{
public:
	FPrototypeMeshBuilderController();
	~FPrototypeMeshBuilderController();

	void SetPrompt(const FString& InPrompt);
	void SetAssetName(const FString& InAssetName);
	void SetContentPath(const FString& InContentPath);
	void SetReasoningEffort(const FString& InReasoningEffort);
	void SetGenerationMode(EPrototypeGenerationMode InGenerationMode);
	void SetVoxelResolution(int32 InVoxelResolution);
	void SetUseSharedMaterial(bool bInUseSharedMaterial);

	const FString& GetPrompt() const;
	const FString& GetAssetName() const;
	const FString& GetContentPath() const;
	const FString& GetReasoningEffort() const;
	EPrototypeGenerationMode GetGenerationMode() const;
	int32 GetVoxelResolution() const;
	bool GetUseSharedMaterial() const;
	FText GetStatusText() const;
	FText GetSelectedActorText() const;
	TArray<TSharedPtr<FString>>& GetJobDisplayItems();
	const TArray<TSharedPtr<FString>>& GetJobDisplayItems() const;
	bool CanSave() const;
	bool CanDeleteSelectedPreview() const;

	void Generate();
	void Save();
	void DeleteSelectedPreview();
	void Clear();
	void CleanupPreview();

private:
	struct FQueuedGenerateRequest
	{
		FPrototypeMeshRequest Request;
		double EnqueuedAtSeconds = 0.0;
	};

	struct FActiveGenerateJob
	{
		FPrototypeBridgeJobHandle Handle;
		FPrototypeMeshRequest Request;
		double EnqueuedAtSeconds = 0.0;
		double StartedAtSeconds = 0.0;
	};

	struct FPreviewRecord
	{
		TWeakObjectPtr<ADynamicMeshActor> PreviewActor;
		FGuid JobId;
		EPrototypeGenerationMode GenerationMode = EPrototypeGenerationMode::Primitive;
		FString MeshName;
		FString SourcePayloadJson;
		FString Prompt;
		FString ReasoningEffort;
		FString RequestedAssetName;
		FString RequestedContentPath;
		FString Diagnostics;
		double EnqueuedAtSeconds = 0.0;
		double StartedAtSeconds = 0.0;
		double CompletedAtSeconds = 0.0;
		int32 TriangleCount = 0;
		int32 PrimitiveCount = 0;
		FIntVector VoxelResolution = FIntVector::ZeroValue;
		int32 OccupiedVoxelCount = 0;
		int32 PreviewOffsetIndex = 0;
	};

	struct FGeneratedPreviewData
	{
		EPrototypeGenerationMode GenerationMode = EPrototypeGenerationMode::Primitive;
		FString MeshName;
		FString SourcePayloadJson;
		int32 PrimitiveCount = 0;
		FIntVector VoxelResolution = FIntVector::ZeroValue;
		int32 OccupiedVoxelCount = 0;
	};

private:
	bool Tick(float DeltaTime);
	void TryStartNextQueuedJob();
	void PollActiveJob();
	void FinalizeCompletedJob(const FActiveGenerateJob& CompletedJob, const FPrototypeBridgeResult& BridgeResult);
	void RebuildJobDisplayItems();
	void SetStatus(const FString& InStatus);
	FString FormatElapsed(double ElapsedSeconds) const;
	FString BuildJobSummary(const FString& Prefix, const FPrototypeMeshRequest& Request, double ElapsedSeconds) const;
	bool ExportDebugArtifacts(const FPrototypeMeshRequest& Request, const FPrototypeBridgeResult& BridgeResult, const FGeneratedPreviewData* PreviewData, const FGeneratedMeshBuffers* Buffers, FString& OutDirectory, FString& OutError) const;
	FVector GetNextPreviewLocation() const;
	FVector GetPreviewLocationForIndex(int32 PreviewIndex) const;
	void DestroyAllPreviewActors();
	void RefreshPreviewActorOffsets();
	AActor* GetSingleSelectedActor(FString& OutError) const;
	UDynamicMeshComponent* GetSavableDynamicMeshComponent(AActor* Actor) const;
	FPreviewRecord* FindPreviewRecord(const AActor* Actor);
	const FPreviewRecord* FindPreviewRecord(const AActor* Actor) const;
	bool SpawnPreviewActor(const FActiveGenerateJob& CompletedJob, const FGeneratedPreviewData& PreviewData, const FGeneratedMeshBuffers& Buffers, const UE::Geometry::FDynamicMesh3& DynamicMesh, const FPrototypeBridgeResult& BridgeResult, FString& OutError);
	bool BuildSelectedActorMetadataJson(const AActor& SelectedActor, const FPreviewRecord* PreviewRecord, const FString& SavedMeshPath, const FString& SavedMaterialPath, bool bUsingSharedMaterial, FString& OutJson) const;
	bool BuildMaterialMetadataJson(const FString& SavedMaterialPath, const FString& SavedMeshPath, bool bUsingSharedMaterial, FString& OutJson) const;

private:
	TUniquePtr<IPrototypeMeshBridge> Bridge;
	FString Prompt;
	FString AssetName;
	FString ContentPath;
	FString ReasoningEffort;
	EPrototypeGenerationMode GenerationMode = EPrototypeGenerationMode::Primitive;
	int32 VoxelResolution = 32;
	bool bUseSharedMaterial = true;
	FString StatusMessage;
	TArray<FQueuedGenerateRequest> PendingJobs;
	TOptional<FActiveGenerateJob> ActiveJob;
	TArray<FPreviewRecord> PreviewRecords;
	TArray<TSharedPtr<FString>> JobDisplayItems;
	FTSTicker::FDelegateHandle TickHandle;
};
