// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CodexInvenPickupData.generated.h"

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
};

namespace CodexInvenPickupData
{
	CODEXINVEN_API TConstArrayView<ECodexInvenPickupType> GetAllPickupTypes();
	CODEXINVEN_API const FCodexInvenPickupDefinition* FindPickupDefinition(ECodexInvenPickupType InPickupType);
	CODEXINVEN_API const FCodexInvenPickupDefinition& GetPickupDefinitionChecked(ECodexInvenPickupType InPickupType);
}
