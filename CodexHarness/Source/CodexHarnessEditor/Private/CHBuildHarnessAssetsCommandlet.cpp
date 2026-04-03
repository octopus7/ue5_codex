#include "CHBuildHarnessAssetsCommandlet.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Factories/MaterialFactoryNew.h"
#include "HAL/FileManager.h"
#include "MaterialEditingLibrary.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionVertexColor.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

DEFINE_LOG_CATEGORY_STATIC(LogCHBuildHarnessAssets, Log, All);

namespace CHHarnessAssetBootstrap
{
	static constexpr TCHAR VoxSourceRoot[] = TEXT("SourceArt/Vox");
	static constexpr TCHAR SmokeVoxRelativePath[] = TEXT("SourceArt/Vox/Smoke/CH_SmokeCube.vox");
	static constexpr TCHAR VoxAssetRoot[] = TEXT("/Game/TopDownShooter/Vox");
	static constexpr TCHAR VoxGeneratedAssetRoot[] = TEXT("/Game/TopDownShooter/Vox/Generated");
	static constexpr TCHAR VoxMaterialPackagePath[] = TEXT("/Game/TopDownShooter/Vox/Materials");
	static constexpr TCHAR VoxMaterialPackageName[] = TEXT("/Game/TopDownShooter/Vox/Materials/M_VoxBase");
	static constexpr TCHAR VoxMaterialObjectPath[] = TEXT("/Game/TopDownShooter/Vox/Materials/M_VoxBase.M_VoxBase");
	static constexpr TCHAR VoxMaterialAssetName[] = TEXT("M_VoxBase");
	static constexpr TCHAR DefaultReportRelativePath[] = TEXT("Saved/Harness/M0_P1_BootstrapReport.txt");

	struct FVoxelSample
	{
		uint8 X;
		uint8 Y;
		uint8 Z;
		uint8 ColorIndex;
	};

	static void AppendInt32LE(TArray<uint8>& Buffer, const int32 Value)
	{
		Buffer.Add(static_cast<uint8>(Value & 0xFF));
		Buffer.Add(static_cast<uint8>((Value >> 8) & 0xFF));
		Buffer.Add(static_cast<uint8>((Value >> 16) & 0xFF));
		Buffer.Add(static_cast<uint8>((Value >> 24) & 0xFF));
	}

	static void AppendChunk(TArray<uint8>& Buffer, const ANSICHAR ChunkId[4], const TArray<uint8>& Content)
	{
		Buffer.Append(reinterpret_cast<const uint8*>(ChunkId), 4);
		AppendInt32LE(Buffer, Content.Num());
		AppendInt32LE(Buffer, 0);
		Buffer.Append(Content);
	}

	static bool WriteSmokeCubeVox(const FString& OutputPath)
	{
		static const FVoxelSample Voxels[] =
		{
			{0, 0, 0, 249},
			{1, 0, 0, 249},
			{0, 1, 0, 249},
			{1, 1, 0, 249},
			{0, 0, 1, 249},
			{1, 0, 1, 249},
			{0, 1, 1, 249},
			{1, 1, 1, 249}
		};

		TArray<uint8> SizeContent;
		AppendInt32LE(SizeContent, 4);
		AppendInt32LE(SizeContent, 4);
		AppendInt32LE(SizeContent, 4);

		TArray<uint8> XyziContent;
		AppendInt32LE(XyziContent, UE_ARRAY_COUNT(Voxels));
		for (const FVoxelSample& Voxel : Voxels)
		{
			XyziContent.Add(Voxel.X);
			XyziContent.Add(Voxel.Y);
			XyziContent.Add(Voxel.Z);
			XyziContent.Add(Voxel.ColorIndex);
		}

		TArray<uint8> MainChildren;
		static constexpr ANSICHAR SizeChunkId[] = {'S', 'I', 'Z', 'E'};
		static constexpr ANSICHAR XyziChunkId[] = {'X', 'Y', 'Z', 'I'};
		AppendChunk(MainChildren, SizeChunkId, SizeContent);
		AppendChunk(MainChildren, XyziChunkId, XyziContent);

		TArray<uint8> FileData;
		static constexpr ANSICHAR VoxHeader[] = {'V', 'O', 'X', ' '};
		static constexpr ANSICHAR MainChunkId[] = {'M', 'A', 'I', 'N'};
		FileData.Append(reinterpret_cast<const uint8*>(VoxHeader), 4);
		AppendInt32LE(FileData, 150);
		FileData.Append(reinterpret_cast<const uint8*>(MainChunkId), 4);
		AppendInt32LE(FileData, 0);
		AppendInt32LE(FileData, MainChildren.Num());
		FileData.Append(MainChildren);

		IFileManager::Get().MakeDirectory(*FPaths::GetPath(OutputPath), true);
		return FFileHelper::SaveArrayToFile(FileData, *OutputPath);
	}

	static bool SaveAssetPackage(UObject* Asset)
	{
		if (!Asset)
		{
			return false;
		}

		UPackage* const Package = Asset->GetOutermost();
		const FString PackageFilename = FPackageName::LongPackageNameToFilename(
			Package->GetName(),
			FPackageName::GetAssetPackageExtension());

		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.Error = GError;
		SaveArgs.bWarnOfLongFilename = false;

		return UPackage::SavePackage(Package, Asset, *PackageFilename, SaveArgs);
	}

	static UMaterial* EnsureVoxBaseMaterial(bool& bOutCreated)
	{
		bOutCreated = false;

		FString ExistingPackageFilename;
		UMaterial* Material = nullptr;
		if (FPackageName::DoesPackageExist(VoxMaterialPackageName, &ExistingPackageFilename))
		{
			Material = LoadObject<UMaterial>(nullptr, VoxMaterialObjectPath);
		}

		if (!Material)
		{
			FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
			UMaterialFactoryNew* Factory = NewObject<UMaterialFactoryNew>();
			Material = Cast<UMaterial>(AssetToolsModule.Get().CreateAsset(
				VoxMaterialAssetName,
				VoxMaterialPackagePath,
				UMaterial::StaticClass(),
				Factory));

			if (!Material)
			{
				return nullptr;
			}

			bOutCreated = true;
		}

		Material->Modify();
		UMaterialEditingLibrary::DeleteAllMaterialExpressions(Material);

		UMaterialExpressionVertexColor* const VertexColor = Cast<UMaterialExpressionVertexColor>(
			UMaterialEditingLibrary::CreateMaterialExpression(Material, UMaterialExpressionVertexColor::StaticClass(), -600, 0));
		UMaterialExpressionScalarParameter* const Roughness = Cast<UMaterialExpressionScalarParameter>(
			UMaterialEditingLibrary::CreateMaterialExpression(Material, UMaterialExpressionScalarParameter::StaticClass(), -300, 180));
		UMaterialExpressionScalarParameter* const Metallic = Cast<UMaterialExpressionScalarParameter>(
			UMaterialEditingLibrary::CreateMaterialExpression(Material, UMaterialExpressionScalarParameter::StaticClass(), -300, 320));
		UMaterialExpressionScalarParameter* const EmissiveStrength = Cast<UMaterialExpressionScalarParameter>(
			UMaterialEditingLibrary::CreateMaterialExpression(Material, UMaterialExpressionScalarParameter::StaticClass(), -300, -180));
		UMaterialExpressionMultiply* const EmissiveMultiply = Cast<UMaterialExpressionMultiply>(
			UMaterialEditingLibrary::CreateMaterialExpression(Material, UMaterialExpressionMultiply::StaticClass(), -40, -120));

		if (!VertexColor || !Roughness || !Metallic || !EmissiveStrength || !EmissiveMultiply)
		{
			return nullptr;
		}

		Roughness->ParameterName = TEXT("Roughness");
		Roughness->DefaultValue = 0.85f;

		Metallic->ParameterName = TEXT("Metallic");
		Metallic->DefaultValue = 0.0f;

		EmissiveStrength->ParameterName = TEXT("EmissiveStrength");
		EmissiveStrength->DefaultValue = 0.0f;

		UMaterialEditingLibrary::ConnectMaterialProperty(VertexColor, TEXT(""), MP_BaseColor);
		UMaterialEditingLibrary::ConnectMaterialProperty(Roughness, TEXT(""), MP_Roughness);
		UMaterialEditingLibrary::ConnectMaterialProperty(Metallic, TEXT(""), MP_Metallic);
		UMaterialEditingLibrary::ConnectMaterialExpressions(VertexColor, TEXT(""), EmissiveMultiply, TEXT("A"));
		UMaterialEditingLibrary::ConnectMaterialExpressions(EmissiveStrength, TEXT(""), EmissiveMultiply, TEXT("B"));
		UMaterialEditingLibrary::ConnectMaterialProperty(EmissiveMultiply, TEXT(""), MP_EmissiveColor);
		UMaterialEditingLibrary::LayoutMaterialExpressions(Material);
		UMaterialEditingLibrary::RecompileMaterial(Material);
		Material->MarkPackageDirty();

		return Material;
	}
}

UCHBuildHarnessAssetsCommandlet::UCHBuildHarnessAssetsCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
	ShowErrorCount = true;
}

int32 UCHBuildHarnessAssetsCommandlet::Main(const FString& Params)
{
	const bool bRecreateSmokeVox = FParse::Param(*Params, TEXT("RecreateSmokeVox"));
	const FString SmokeVoxAbsolutePath = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(FPaths::ProjectDir(), CHHarnessAssetBootstrap::SmokeVoxRelativePath));
	const FString ReportAbsolutePath = FPaths::ConvertRelativePathToFull(
		FPaths::Combine(FPaths::ProjectDir(), CHHarnessAssetBootstrap::DefaultReportRelativePath));

	const bool bSmokeVoxExists = IFileManager::Get().FileExists(*SmokeVoxAbsolutePath);
	bool bSmokeVoxCreated = false;
	if (!bSmokeVoxExists || bRecreateSmokeVox)
	{
		bSmokeVoxCreated = CHHarnessAssetBootstrap::WriteSmokeCubeVox(SmokeVoxAbsolutePath);
		if (!bSmokeVoxCreated)
		{
			UE_LOG(LogCHBuildHarnessAssets, Error, TEXT("스모크 테스트 VOX 파일 생성에 실패했습니다: %s"), *SmokeVoxAbsolutePath);
			return 1;
		}
	}

	bool bMaterialCreated = false;
	UMaterial* const VoxBaseMaterial = CHHarnessAssetBootstrap::EnsureVoxBaseMaterial(bMaterialCreated);
	if (!VoxBaseMaterial)
	{
		UE_LOG(LogCHBuildHarnessAssets, Error, TEXT("M_VoxBase 머터리얼을 준비하지 못했습니다."));
		return 2;
	}

	if (!CHHarnessAssetBootstrap::SaveAssetPackage(VoxBaseMaterial))
	{
		UE_LOG(LogCHBuildHarnessAssets, Error, TEXT("M_VoxBase 저장에 실패했습니다: %s"), CHHarnessAssetBootstrap::VoxMaterialObjectPath);
		return 3;
	}

	IFileManager::Get().MakeDirectory(*FPaths::GetPath(ReportAbsolutePath), true);

	const FString ReportText = FString::Printf(
		TEXT("Commandlet=CHBuildHarnessAssets\n")
		TEXT("SmokeVoxSource=%s\n")
		TEXT("SmokeVoxCreated=%s\n")
		TEXT("SmokeVoxRecreated=%s\n")
		TEXT("VoxSourceRoot=%s\n")
		TEXT("VoxAssetRoot=%s\n")
		TEXT("VoxGeneratedAssetRoot=%s\n")
		TEXT("VoxBaseMaterial=%s\n")
		TEXT("VoxBaseMaterialCreated=%s\n"),
		*SmokeVoxAbsolutePath,
		bSmokeVoxCreated ? TEXT("true") : TEXT("false"),
		(!bSmokeVoxExists || bRecreateSmokeVox) ? TEXT("true") : TEXT("false"),
		CHHarnessAssetBootstrap::VoxSourceRoot,
		CHHarnessAssetBootstrap::VoxAssetRoot,
		CHHarnessAssetBootstrap::VoxGeneratedAssetRoot,
		CHHarnessAssetBootstrap::VoxMaterialObjectPath,
		bMaterialCreated ? TEXT("true") : TEXT("false"));

	if (!FFileHelper::SaveStringToFile(ReportText, *ReportAbsolutePath))
	{
		UE_LOG(LogCHBuildHarnessAssets, Error, TEXT("부트스트랩 리포트 저장에 실패했습니다: %s"), *ReportAbsolutePath);
		return 4;
	}

	UE_LOG(LogCHBuildHarnessAssets, Display, TEXT("헤드리스 애셋 부트스트랩 완료"));
	UE_LOG(LogCHBuildHarnessAssets, Display, TEXT("VOX 소스 루트: %s"), CHHarnessAssetBootstrap::VoxSourceRoot);
	UE_LOG(LogCHBuildHarnessAssets, Display, TEXT("스모크 VOX: %s"), *SmokeVoxAbsolutePath);
	UE_LOG(LogCHBuildHarnessAssets, Display, TEXT("VOX 애셋 루트: %s"), CHHarnessAssetBootstrap::VoxAssetRoot);
	UE_LOG(LogCHBuildHarnessAssets, Display, TEXT("VOX 머터리얼: %s"), CHHarnessAssetBootstrap::VoxMaterialObjectPath);
	UE_LOG(LogCHBuildHarnessAssets, Display, TEXT("리포트: %s"), *ReportAbsolutePath);

	return 0;
}
