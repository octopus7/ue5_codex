#pragma once

#include "PrototypeMeshBuilderTypes.h"

namespace UE::Geometry
{
	class FDynamicMesh3;
}

class UMaterialInstanceConstant;
class UMaterialInterface;
class UStaticMesh;
class UObject;

namespace PrototypeMeshBuilder
{
	bool ParseShapeDslJson(const FString& RawJson, FPrototypeShapeDsl& OutDsl, FString& OutError);
	bool ValidateShapeDsl(const FPrototypeShapeDsl& Dsl, FString& OutError);
	bool BuildMeshBuffers(const FPrototypeShapeDsl& Dsl, FGeneratedMeshBuffers& OutBuffers, FString& OutError);
	bool BuildDynamicMesh(const FGeneratedMeshBuffers& Buffers, UE::Geometry::FDynamicMesh3& OutMesh, FString& OutError);
	bool BuildStaticMeshAsset(const FString& PackagePath, const FString& AssetName, const UE::Geometry::FDynamicMesh3& DynamicMesh, UStaticMesh*& OutStaticMesh, FString& OutError);
	bool CreateVertexColorMaterialAsset(const FString& PackagePath, const FString& AssetName, UMaterialInstanceConstant*& OutMaterial, FString& OutError);
	bool ApplyStaticMeshMaterial(UStaticMesh* StaticMesh, UMaterialInterface* Material, FString& OutError);
	bool WriteAssetMetadata(UObject* Asset, const FString& MetadataJson, FString& OutError);
	FString NormalizeContentPath(const FString& InPath);
	FString SanitizeAssetName(const FString& InName);
}
