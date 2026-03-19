// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/SoftObjectPath.h"
#include "UObject/SoftObjectPtr.h"
#include "CodexInvenPickupData.generated.h"

class UStaticMesh;
class UTexture2D;
struct FCodexInvenPickupDefinition;

UENUM()
enum class ECodexInvenPickupType : uint8
{
	CubeRed,
	CubeGreen,
	CubeBlue,
	CubeGold,
	CylinderRed,
	CylinderGreen,
	CylinderBlue,
	CylinderGold
};

UENUM()
enum class ECodexInvenPickupMeshKind : uint8
{
	Cube,
	Cylinder
};

namespace CodexInvenPickupData
{
	CODEXINVEN_API FString GetPickupAssetSlug(ECodexInvenPickupType InPickupType);
	CODEXINVEN_API FString GetPickupMeshPackagePath(ECodexInvenPickupType InPickupType);
	CODEXINVEN_API FString GetPickupIconPackagePath(ECodexInvenPickupType InPickupType);
	CODEXINVEN_API FSoftObjectPath GetPickupMeshAssetPath(ECodexInvenPickupType InPickupType);
	CODEXINVEN_API FSoftObjectPath GetPickupIconAssetPath(ECodexInvenPickupType InPickupType);
	CODEXINVEN_API TConstArrayView<ECodexInvenPickupType> GetAllPickupTypes();
	CODEXINVEN_API const FCodexInvenPickupDefinition* FindPickupDefinition(ECodexInvenPickupType InPickupType);
	CODEXINVEN_API const FCodexInvenPickupDefinition& GetPickupDefinitionChecked(ECodexInvenPickupType InPickupType);
}

UENUM()
enum class ECodexInvenPickupRarity : uint8
{
	Common,
	Gold
};

USTRUCT()
struct FCodexInvenPickupDefinition
{
	GENERATED_BODY()

	FCodexInvenPickupDefinition() = default;

	FCodexInvenPickupDefinition(
		const ECodexInvenPickupType InType,
		const TCHAR* InDisplayName,
		const bool bInStackable,
		const ECodexInvenPickupMeshKind InMeshKind,
		const FLinearColor& InTintColor,
		const ECodexInvenPickupRarity InRarity,
		const bool bInUseMetallicMaterial = false)
		: Type(InType)
		, DisplayName(InDisplayName)
		, bStackable(bInStackable)
		, MeshKind(InMeshKind)
		, TintColor(InTintColor)
		, Rarity(InRarity)
		, bUseMetallicMaterial(bInUseMetallicMaterial)
		, WorldMesh(CodexInvenPickupData::GetPickupMeshAssetPath(InType))
		, InventoryIcon(CodexInvenPickupData::GetPickupIconAssetPath(InType))
	{
	}

	UPROPERTY()
	ECodexInvenPickupType Type = ECodexInvenPickupType::CubeRed;

	UPROPERTY()
	FString DisplayName;

	UPROPERTY()
	bool bStackable = false;

	UPROPERTY()
	ECodexInvenPickupMeshKind MeshKind = ECodexInvenPickupMeshKind::Cube;

	UPROPERTY()
	FLinearColor TintColor = FLinearColor::Red;

	UPROPERTY()
	ECodexInvenPickupRarity Rarity = ECodexInvenPickupRarity::Common;

	UPROPERTY()
	bool bUseMetallicMaterial = false;

	UPROPERTY()
	TSoftObjectPtr<UStaticMesh> WorldMesh;

	UPROPERTY()
	TSoftObjectPtr<UTexture2D> InventoryIcon;
};
