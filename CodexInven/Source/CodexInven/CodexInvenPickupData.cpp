// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenPickupData.h"

#include "Misc/PackageName.h"

namespace
{
	FString MakeObjectPath(const FString& InPackagePath)
	{
		return InPackagePath + TEXT(".") + FPackageName::GetLongPackageAssetName(InPackagePath);
	}

	const FCodexInvenPickupDefinition* FindPickupDefinitionInternal(const ECodexInvenPickupType InPickupType)
	{
		static const FCodexInvenPickupDefinition Definitions[] =
		{
			FCodexInvenPickupDefinition(ECodexInvenPickupType::CubeRed, TEXT("Field Helmet"), false, ECodexInvenPickupMeshKind::Cube, FLinearColor(0.88f, 0.25f, 0.20f, 1.0f), ECodexInvenPickupRarity::Common),
			FCodexInvenPickupDefinition(ECodexInvenPickupType::CubeGreen, TEXT("Armor Vest"), false, ECodexInvenPickupMeshKind::Cube, FLinearColor(0.22f, 0.84f, 0.34f, 1.0f), ECodexInvenPickupRarity::Common),
			FCodexInvenPickupDefinition(ECodexInvenPickupType::CubeBlue, TEXT("Utility Backpack"), false, ECodexInvenPickupMeshKind::Cube, FLinearColor(0.24f, 0.48f, 0.96f, 1.0f), ECodexInvenPickupRarity::Common),
			FCodexInvenPickupDefinition(ECodexInvenPickupType::CubeGold, TEXT("Tool Case"), false, ECodexInvenPickupMeshKind::Cube, FLinearColor(0.98f, 0.80f, 0.22f, 1.0f), ECodexInvenPickupRarity::Gold, true),
			FCodexInvenPickupDefinition(ECodexInvenPickupType::CylinderRed, TEXT("Medkit Pouch"), true, ECodexInvenPickupMeshKind::Cylinder, FLinearColor(0.88f, 0.25f, 0.20f, 1.0f), ECodexInvenPickupRarity::Common),
			FCodexInvenPickupDefinition(ECodexInvenPickupType::CylinderGreen, TEXT("Energy Drink Can"), true, ECodexInvenPickupMeshKind::Cylinder, FLinearColor(0.22f, 0.84f, 0.34f, 1.0f), ECodexInvenPickupRarity::Common),
			FCodexInvenPickupDefinition(ECodexInvenPickupType::CylinderBlue, TEXT("Ration Pack"), true, ECodexInvenPickupMeshKind::Cylinder, FLinearColor(0.24f, 0.48f, 0.96f, 1.0f), ECodexInvenPickupRarity::Common),
			FCodexInvenPickupDefinition(ECodexInvenPickupType::CylinderGold, TEXT("Pill Bottle"), true, ECodexInvenPickupMeshKind::Cylinder, FLinearColor(0.98f, 0.80f, 0.22f, 1.0f), ECodexInvenPickupRarity::Gold, true)
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

FString CodexInvenPickupData::GetPickupAssetSlug(const ECodexInvenPickupType InPickupType)
{
	switch (InPickupType)
	{
	case ECodexInvenPickupType::CubeRed:
		return TEXT("FieldHelmet");

	case ECodexInvenPickupType::CubeGreen:
		return TEXT("ArmorVest");

	case ECodexInvenPickupType::CubeBlue:
		return TEXT("UtilityBackpack");

	case ECodexInvenPickupType::CubeGold:
		return TEXT("ToolCase");

	case ECodexInvenPickupType::CylinderRed:
		return TEXT("MedkitPouch");

	case ECodexInvenPickupType::CylinderGreen:
		return TEXT("EnergyDrinkCan");

	case ECodexInvenPickupType::CylinderBlue:
		return TEXT("RationPack");

	case ECodexInvenPickupType::CylinderGold:
		return TEXT("PillBottle");
	}

	return TEXT("UnknownPickup");
}

FString CodexInvenPickupData::GetPickupMeshPackagePath(const ECodexInvenPickupType InPickupType)
{
	return FString::Printf(TEXT("/Game/Art/Pickups/Meshes/SM_Pickup_%s"), *GetPickupAssetSlug(InPickupType));
}

FString CodexInvenPickupData::GetPickupIconPackagePath(const ECodexInvenPickupType InPickupType)
{
	return FString::Printf(TEXT("/Game/Art/Pickups/Icons/T_PickupIcon_%s"), *GetPickupAssetSlug(InPickupType));
}

FSoftObjectPath CodexInvenPickupData::GetPickupMeshAssetPath(const ECodexInvenPickupType InPickupType)
{
	return FSoftObjectPath(MakeObjectPath(GetPickupMeshPackagePath(InPickupType)));
}

FSoftObjectPath CodexInvenPickupData::GetPickupIconAssetPath(const ECodexInvenPickupType InPickupType)
{
	return FSoftObjectPath(MakeObjectPath(GetPickupIconPackagePath(InPickupType)));
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
