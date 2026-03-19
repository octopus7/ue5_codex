// Fill out your copyright notice in the Description page of Project Settings.

#include "CodexInvenInventoryIconSubsystem.h"

#include "Engine/Texture2D.h"
#if WITH_EDITOR
#include "TextureCompiler.h"
#endif
#include "UObject/UObjectGlobals.h"

namespace
{
	constexpr int32 InventoryIconSize = 64;

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
		InventoryIcons.Add(PickupType, LoadInventoryIconTexture(CodexInvenPickupData::GetPickupDefinitionChecked(PickupType)));
	}

	InventorySlotBackgrounds.Reset();
	InventorySlotBackgrounds.Add(ECodexInvenPickupRarity::Gold, CreateInventorySlotBackgroundTexture(ECodexInvenPickupRarity::Gold));
}

UTexture2D* UCodexInvenInventoryIconSubsystem::GetInventoryIcon(const ECodexInvenPickupType InPickupType) const
{
	if (const TObjectPtr<UTexture2D>* FoundTexture = InventoryIcons.Find(InPickupType))
	{
		return FoundTexture->Get();
	}

	return nullptr;
}

UTexture2D* UCodexInvenInventoryIconSubsystem::GetInventorySlotBackground(const ECodexInvenPickupRarity InRarity) const
{
	if (const TObjectPtr<UTexture2D>* FoundTexture = InventorySlotBackgrounds.Find(InRarity))
	{
		return FoundTexture->Get();
	}

	return nullptr;
}

UTexture2D* UCodexInvenInventoryIconSubsystem::LoadInventoryIconTexture(const FCodexInvenPickupDefinition& InDefinition) const
{
	UTexture2D* const Texture = InDefinition.InventoryIcon.LoadSynchronous();
#if WITH_EDITOR
	if (Texture != nullptr)
	{
		FTextureCompilingManager::Get().FinishCompilation({ Texture });
	}
#endif
	return Texture;
}

UTexture2D* UCodexInvenInventoryIconSubsystem::CreateInventorySlotBackgroundTexture(const ECodexInvenPickupRarity InRarity) const
{
	TArray64<uint8> PixelData;
	BuildInventorySlotBackgroundPixels(InRarity, PixelData);

	const FName TextureName = MakeUniqueObjectName(
		GetTransientPackage(),
		UTexture2D::StaticClass(),
		FName(*FString::Printf(TEXT("InventorySlotBackground_%d"), static_cast<int32>(InRarity))));

	UTexture2D* Texture = UTexture2D::CreateTransient(InventoryIconSize, InventoryIconSize, PF_B8G8R8A8, TextureName, PixelData);
	if (Texture == nullptr)
	{
		return nullptr;
	}

	Texture->LODGroup = TEXTUREGROUP_UI;
	Texture->MipGenSettings = TMGS_NoMipmaps;
	Texture->NeverStream = true;
	Texture->SRGB = true;
	Texture->Filter = TF_Bilinear;
	Texture->UpdateResource();
	return Texture;
}

void UCodexInvenInventoryIconSubsystem::BuildInventorySlotBackgroundPixels(const ECodexInvenPickupRarity InRarity, TArray64<uint8>& OutPixels)
{
	OutPixels.Init(0, static_cast<int64>(InventoryIconSize) * InventoryIconSize * 4);

	if (InRarity != ECodexInvenPickupRarity::Gold)
	{
		return;
	}

	constexpr int32 GoldBorderThickness = 2;
	const FLinearColor BorderColor(0.98f, 0.88f, 0.45f, 0.95f);
	const FLinearColor TopFillColor(0.64f, 0.52f, 0.15f, 0.82f);
	const FLinearColor BottomFillColor(0.17f, 0.12f, 0.02f, 0.94f);
	const FLinearColor TopInnerHighlightColor(0.90f, 0.78f, 0.31f, 0.20f);

	for (int32 Y = 0; Y < InventoryIconSize; ++Y)
	{
		const float NormalizedY = static_cast<float>(Y) / static_cast<float>(InventoryIconSize - 1);
		for (int32 X = 0; X < InventoryIconSize; ++X)
		{
			const bool bIsBorderPixel =
				X < GoldBorderThickness ||
				Y < GoldBorderThickness ||
				X >= InventoryIconSize - GoldBorderThickness ||
				Y >= InventoryIconSize - GoldBorderThickness;

			if (bIsBorderPixel)
			{
				SetPixel(OutPixels, X, Y, BorderColor.GetClamped().ToFColorSRGB());
				continue;
			}

			FLinearColor FillColor = FMath::Lerp(TopFillColor, BottomFillColor, FMath::InterpEaseIn(0.0f, 1.0f, NormalizedY, 1.6f));
			if (Y < 10)
			{
				const float HighlightAlpha = 1.0f - (static_cast<float>(Y) / 10.0f);
				FillColor = FMath::Lerp(FillColor, TopInnerHighlightColor, HighlightAlpha * TopInnerHighlightColor.A);
				FillColor.A = FMath::Max(FillColor.A, TopFillColor.A);
			}

			SetPixel(OutPixels, X, Y, FillColor.GetClamped().ToFColorSRGB());
		}
	}
}
