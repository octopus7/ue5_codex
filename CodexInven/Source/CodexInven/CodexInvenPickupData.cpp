// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenPickupData.h"

namespace
{
	const FCodexInvenPickupDefinition* FindPickupDefinitionInternal(const ECodexInvenPickupType InPickupType)
	{
		static const FCodexInvenPickupDefinition Definitions[] =
		{
			FCodexInvenPickupDefinition(ECodexInvenPickupType::CubeRed, TEXT("Cube Red"), false, ECodexInvenPickupMeshKind::Cube, FLinearColor(1.0f, 0.15f, 0.15f, 1.0f), ECodexInvenPickupRarity::Common),
			FCodexInvenPickupDefinition(ECodexInvenPickupType::CubeGreen, TEXT("Cube Green"), false, ECodexInvenPickupMeshKind::Cube, FLinearColor(0.15f, 1.0f, 0.15f, 1.0f), ECodexInvenPickupRarity::Common),
			FCodexInvenPickupDefinition(ECodexInvenPickupType::CubeBlue, TEXT("Cube Blue"), false, ECodexInvenPickupMeshKind::Cube, FLinearColor(0.15f, 0.45f, 1.0f, 1.0f), ECodexInvenPickupRarity::Common),
			FCodexInvenPickupDefinition(ECodexInvenPickupType::CubeGold, TEXT("Cube Gold"), false, ECodexInvenPickupMeshKind::Cube, FLinearColor(0.98f, 0.80f, 0.22f, 1.0f), ECodexInvenPickupRarity::Gold, true),
			FCodexInvenPickupDefinition(ECodexInvenPickupType::CylinderRed, TEXT("Cylinder Red"), true, ECodexInvenPickupMeshKind::Cylinder, FLinearColor(1.0f, 0.15f, 0.15f, 1.0f), ECodexInvenPickupRarity::Common),
			FCodexInvenPickupDefinition(ECodexInvenPickupType::CylinderGreen, TEXT("Cylinder Green"), true, ECodexInvenPickupMeshKind::Cylinder, FLinearColor(0.15f, 1.0f, 0.15f, 1.0f), ECodexInvenPickupRarity::Common),
			FCodexInvenPickupDefinition(ECodexInvenPickupType::CylinderBlue, TEXT("Cylinder Blue"), true, ECodexInvenPickupMeshKind::Cylinder, FLinearColor(0.15f, 0.45f, 1.0f, 1.0f), ECodexInvenPickupRarity::Common),
			FCodexInvenPickupDefinition(ECodexInvenPickupType::CylinderGold, TEXT("Cylinder Gold"), true, ECodexInvenPickupMeshKind::Cylinder, FLinearColor(0.98f, 0.80f, 0.22f, 1.0f), ECodexInvenPickupRarity::Gold, true)
		};

		for (const FCodexInvenPickupDefinition& Definition : Definitions)
		{
			if (Definition.Type == InPickupType)
			{
				return &Definition;
			}
		}

		return nullptr;
	}
}

TConstArrayView<ECodexInvenPickupType> CodexInvenPickupData::GetAllPickupTypes()
{
	static const ECodexInvenPickupType PickupTypes[] =
	{
		ECodexInvenPickupType::CubeRed,
		ECodexInvenPickupType::CubeGreen,
		ECodexInvenPickupType::CubeBlue,
		ECodexInvenPickupType::CubeGold,
		ECodexInvenPickupType::CylinderRed,
		ECodexInvenPickupType::CylinderGreen,
		ECodexInvenPickupType::CylinderBlue,
		ECodexInvenPickupType::CylinderGold
	};

	return TConstArrayView<ECodexInvenPickupType>(PickupTypes);
}

const FCodexInvenPickupDefinition* CodexInvenPickupData::FindPickupDefinition(const ECodexInvenPickupType InPickupType)
{
	return FindPickupDefinitionInternal(InPickupType);
}

const FCodexInvenPickupDefinition& CodexInvenPickupData::GetPickupDefinitionChecked(const ECodexInvenPickupType InPickupType)
{
	const FCodexInvenPickupDefinition* Definition = FindPickupDefinitionInternal(InPickupType);
	check(Definition != nullptr);
	return *Definition;
}
