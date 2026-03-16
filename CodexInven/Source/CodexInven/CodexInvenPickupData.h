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
	CylinderRed,
	CylinderGreen,
	CylinderBlue
};

UENUM()
enum class ECodexInvenPickupMeshKind : uint8
{
	Cube,
	Cylinder
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
		const FLinearColor& InTintColor)
		: Type(InType)
		, DisplayName(InDisplayName)
		, bStackable(bInStackable)
		, MeshKind(InMeshKind)
		, TintColor(InTintColor)
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
};

namespace CodexInvenPickupData
{
	CODEXINVEN_API TConstArrayView<ECodexInvenPickupType> GetAllPickupTypes();
	CODEXINVEN_API const FCodexInvenPickupDefinition* FindPickupDefinition(ECodexInvenPickupType InPickupType);
	CODEXINVEN_API const FCodexInvenPickupDefinition& GetPickupDefinitionChecked(ECodexInvenPickupType InPickupType);
}
