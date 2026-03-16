// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenInventoryIconSubsystem.h"

#include "Engine/Texture2D.h"
#include "UObject/UObjectGlobals.h"

namespace
{
	constexpr int32 InventoryIconSize = 64;
	constexpr int32 InventoryIconPadding = 6;
	constexpr float InventoryIconOutlineThickness = 1.0f;

	void SetPixel(TArray64<uint8>& InOutPixels, const int32 InX, const int32 InY, const FColor& InColor)
	{
		const int64 PixelIndex = ((static_cast<int64>(InY) * InventoryIconSize) + InX) * 4;
		InOutPixels[PixelIndex + 0] = InColor.B;
		InOutPixels[PixelIndex + 1] = InColor.G;
		InOutPixels[PixelIndex + 2] = InColor.R;
		InOutPixels[PixelIndex + 3] = InColor.A;
	}
}

void UCodexInvenInventoryIconSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	InventoryIcons.Reset();
	for (const ECodexInvenPickupType PickupType : CodexInvenPickupData::GetAllPickupTypes())
	{
		InventoryIcons.Add(PickupType, CreateInventoryIconTexture(CodexInvenPickupData::GetPickupDefinitionChecked(PickupType)));
	}
}

UTexture2D* UCodexInvenInventoryIconSubsystem::GetInventoryIcon(const ECodexInvenPickupType InPickupType) const
{
	if (const TObjectPtr<UTexture2D>* FoundTexture = InventoryIcons.Find(InPickupType))
	{
		return FoundTexture->Get();
	}

	return nullptr;
}

UTexture2D* UCodexInvenInventoryIconSubsystem::CreateInventoryIconTexture(const FCodexInvenPickupDefinition& InDefinition) const
{
	TArray64<uint8> PixelData;
	BuildInventoryIconPixels(InDefinition, PixelData);

	const FName TextureName = MakeUniqueObjectName(
		GetTransientPackage(),
		UTexture2D::StaticClass(),
		FName(*FString::Printf(TEXT("InventoryIcon_%s"), *InDefinition.DisplayName.Replace(TEXT(" "), TEXT("")))));

	UTexture2D* Texture = UTexture2D::CreateTransient(InventoryIconSize, InventoryIconSize, PF_B8G8R8A8, TextureName, PixelData);
	if (Texture == nullptr)
	{
		return nullptr;
	}

	Texture->LODGroup = TEXTUREGROUP_UI;
	Texture->MipGenSettings = TMGS_NoMipmaps;
	Texture->NeverStream = true;
	Texture->SRGB = true;
	Texture->Filter = TF_Nearest;
	Texture->UpdateResource();
	return Texture;
}

void UCodexInvenInventoryIconSubsystem::BuildInventoryIconPixels(const FCodexInvenPickupDefinition& InDefinition, TArray64<uint8>& OutPixels)
{
	OutPixels.Init(0, static_cast<int64>(InventoryIconSize) * InventoryIconSize * 4);

	FLinearColor OutlineLinearColor = InDefinition.TintColor * 0.35f;
	OutlineLinearColor.A = 1.0f;

	const FColor FillColor = InDefinition.TintColor.GetClamped().ToFColorSRGB();
	const FColor OutlineColor = OutlineLinearColor.GetClamped().ToFColorSRGB();

	const float Center = (static_cast<float>(InventoryIconSize) - 1.0f) * 0.5f;
	const float OuterRadius = Center - InventoryIconPadding;
	const float InnerRadius = FMath::Max(0.0f, OuterRadius - InventoryIconOutlineThickness);
	const float OuterRadiusSquared = OuterRadius * OuterRadius;
	const float InnerRadiusSquared = InnerRadius * InnerRadius;

	for (int32 Y = 0; Y < InventoryIconSize; ++Y)
	{
		for (int32 X = 0; X < InventoryIconSize; ++X)
		{
			bool bInsideOuterShape = false;
			bool bInsideInnerShape = false;

			if (InDefinition.MeshKind == ECodexInvenPickupMeshKind::Cylinder)
			{
				const float DeltaX = static_cast<float>(X) - Center;
				const float DeltaY = static_cast<float>(Y) - Center;
				const float DistanceSquared = (DeltaX * DeltaX) + (DeltaY * DeltaY);

				bInsideOuterShape = DistanceSquared <= OuterRadiusSquared;
				bInsideInnerShape = DistanceSquared <= InnerRadiusSquared;
			}
			else
			{
				const int32 MinOuter = InventoryIconPadding;
				const int32 MaxOuter = InventoryIconSize - InventoryIconPadding - 1;
				const int32 MinInner = MinOuter + 1;
				const int32 MaxInner = MaxOuter - 1;

				bInsideOuterShape = X >= MinOuter && X <= MaxOuter && Y >= MinOuter && Y <= MaxOuter;
				bInsideInnerShape = X >= MinInner && X <= MaxInner && Y >= MinInner && Y <= MaxInner;
			}

			if (!bInsideOuterShape)
			{
				continue;
			}

			SetPixel(OutPixels, X, Y, bInsideInnerShape ? FillColor : OutlineColor);
		}
	}
}
