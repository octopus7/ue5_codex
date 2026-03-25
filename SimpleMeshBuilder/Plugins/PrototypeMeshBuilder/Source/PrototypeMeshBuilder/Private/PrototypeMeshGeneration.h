#pragma once

#include "PrototypeMeshBuilderTypes.h"

namespace UE::Geometry
{
	class FDynamicMesh3;
}

class UMaterialInterface;
class UStaticMesh;
class UObject;

namespace PrototypeMeshBuilder
{
	bool ParseMeshPayloadJson(const FString& RawJson, FPrototypeMeshPayload& OutPayload, FString& OutError);
	bool ParseShapeDslJson(const FString& RawJson, FPrototypeShapeDsl& OutDsl, FString& OutError);
	bool ValidateShapeDsl(const FPrototypeShapeDsl& Dsl, FString& OutError);
	bool BuildMeshBuffers(const FPrototypeShapeDsl& Dsl, FGeneratedMeshBuffers& OutBuffers, FString& OutError);
	bool BuildMeshBuffers(const FPrototypeMeshPayload& Payload, FGeneratedMeshBuffers& OutBuffers, FString& OutError);
	bool ParseVoxelGridJson(const FString& RawJson, FPrototypeVoxelGrid& OutGrid, FString& OutError);
	bool ValidateVoxelGrid(const FPrototypeVoxelGrid& Grid, FString& OutError);
	bool BuildVoxelMeshBuffers(const FPrototypeVoxelGrid& Grid, FGeneratedMeshBuffers& OutBuffers, FString& OutError);
	bool CountOccupiedVoxels(const FPrototypeVoxelGrid& Grid, int32& OutOccupiedCount, FString& OutError);
	bool BuildDynamicMesh(const FGeneratedMeshBuffers& Buffers, UE::Geometry::FDynamicMesh3& OutMesh, FString& OutError);
	bool BuildStaticMeshAsset(const FString& PackagePath, const FString& AssetName, const UE::Geometry::FDynamicMesh3& DynamicMesh, UStaticMesh*& OutStaticMesh, FString& OutError);
	bool WriteMeshBuffersObjFile(const FString& FilePath, const FGeneratedMeshBuffers& Buffers, FString& OutError);
	bool WriteTextFileUtf8(const FString& FilePath, const FString& Contents, FString& OutError);
	bool CreateVertexColorLitMaterialAsset(const FString& PackagePath, const FString& AssetName, UMaterialInterface*& OutMaterial, FString& OutError);
	bool ApplyStaticMeshMaterial(UStaticMesh* StaticMesh, UMaterialInterface* Material, FString& OutError);
	bool WriteAssetMetadata(UObject* Asset, const FString& MetadataJson, FString& OutError);
	FString NormalizeContentPath(const FString& InPath);
	FString SanitizeAssetName(const FString& InName);
}
