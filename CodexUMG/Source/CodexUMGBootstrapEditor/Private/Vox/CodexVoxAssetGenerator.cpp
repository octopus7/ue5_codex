#include "Vox/CodexVoxAssetGenerator.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "HAL/PlatformProcess.h"
#include "JsonObjectConverter.h"
#include "Materials/Material.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/StringBuilder.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/BoxElem.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "Vox/CodexVoxMaterialBuilder.h"
#include "Vox/CodexVoxMeshBuilder.h"
#include "Vox/CodexVoxParser.h"

DEFINE_LOG_CATEGORY(LogCodexVox);

namespace
{
	bool LoadManifestJson(
		const FString& ManifestPath,
		TArray<CodexVox::FManifestEntry>& OutEntries,
		float& OutVoxelSize,
		FString& OutError)
	{
		FString ManifestText;
		if (!FFileHelper::LoadFileToString(ManifestText, *ManifestPath))
		{
			OutError = FString::Printf(TEXT("Failed to load manifest '%s'."), *ManifestPath);
			return false;
		}

		TSharedPtr<FJsonObject> RootObject;
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ManifestText);
		if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
		{
			OutError = FString::Printf(TEXT("Failed to parse manifest '%s'."), *ManifestPath);
			return false;
		}

		OutVoxelSize = CodexVox::DefaultVoxelSize;
		double JsonVoxelSize = 0.0;
		if (RootObject->TryGetNumberField(TEXT("voxelSize"), JsonVoxelSize) && JsonVoxelSize > 0.0)
		{
			OutVoxelSize = static_cast<float>(JsonVoxelSize);
		}

		const TArray<TSharedPtr<FJsonValue>>* AssetValues = nullptr;
		if (!RootObject->TryGetArrayField(TEXT("assets"), AssetValues) || AssetValues == nullptr)
		{
			OutError = FString::Printf(TEXT("Manifest '%s' does not contain an 'assets' array."), *ManifestPath);
			return false;
		}

		OutEntries.Reset();
		OutEntries.Reserve(AssetValues->Num());

		for (int32 AssetIndex = 0; AssetIndex < AssetValues->Num(); ++AssetIndex)
		{
			const TSharedPtr<FJsonObject>* AssetObject = nullptr;
			if (!(*AssetValues)[AssetIndex].IsValid() || !(*AssetValues)[AssetIndex]->TryGetObject(AssetObject) || AssetObject == nullptr)
			{
				OutError = FString::Printf(TEXT("Manifest '%s' contains a non-object asset entry at index %d."), *ManifestPath, AssetIndex);
				return false;
			}

			CodexVox::FManifestEntry Entry;
			if (!(*AssetObject)->TryGetStringField(TEXT("id"), Entry.Id)
				|| !(*AssetObject)->TryGetStringField(TEXT("displayName"), Entry.DisplayName)
				|| !(*AssetObject)->TryGetStringField(TEXT("sourceVoxFile"), Entry.SourceVoxFile)
				|| !(*AssetObject)->TryGetStringField(TEXT("targetPackagePath"), Entry.TargetPackagePath)
				|| !(*AssetObject)->TryGetStringField(TEXT("targetAssetName"), Entry.TargetAssetName)
				|| !(*AssetObject)->TryGetStringField(TEXT("category"), Entry.Category))
			{
				OutError = FString::Printf(TEXT("Manifest '%s' is missing required string fields for asset index %d."), *ManifestPath, AssetIndex);
				return false;
			}

			(*AssetObject)->TryGetBoolField(TEXT("repeatable"), Entry.bRepeatable);
			(*AssetObject)->TryGetStringField(TEXT("notes"), Entry.Notes);

			FString PivotRuleString;
			if (!(*AssetObject)->TryGetStringField(TEXT("pivotRule"), PivotRuleString)
				|| !CodexVox::TryParsePivotRule(PivotRuleString, Entry.PivotRule))
			{
				OutError = FString::Printf(TEXT("Manifest '%s' contains an invalid pivotRule for asset '%s'."), *ManifestPath, *Entry.Id);
				return false;
			}

			FString CollisionTypeString;
			if (!(*AssetObject)->TryGetStringField(TEXT("collisionType"), CollisionTypeString)
				|| !CodexVox::TryParseCollisionType(CollisionTypeString, Entry.CollisionType))
			{
				OutError = FString::Printf(TEXT("Manifest '%s' contains an invalid collisionType for asset '%s'."), *ManifestPath, *Entry.Id);
				return false;
			}

			if (!FPackageName::IsValidLongPackageName(Entry.TargetPackagePath))
			{
				OutError = FString::Printf(TEXT("Manifest '%s' contains an invalid targetPackagePath '%s'."), *ManifestPath, *Entry.TargetPackagePath);
				return false;
			}

			OutEntries.Add(MoveTemp(Entry));
		}

		return true;
	}

	FString EscapePowerShellSingleQuotedString(const FString& Value)
	{
		FString Escaped(Value);
		Escaped.ReplaceInline(TEXT("'"), TEXT("''"));
		return Escaped;
	}

	bool IsCurrentProjectEditorRunning()
	{
		const FString ProjectFilePath = FPaths::ConvertRelativePathToFull(FPaths::GetProjectFilePath());
		const FString EscapedProjectFilePath = EscapePowerShellSingleQuotedString(ProjectFilePath);
		const FString PowerShellScript = FString::Printf(
			TEXT("$p = Get-CimInstance Win32_Process -Filter \"Name = 'UnrealEditor.exe'\" | Where-Object { $_.CommandLine -and $_.CommandLine -match [regex]::Escape('%s') } | Select-Object -First 1; if ($p) { Write-Output 'RUNNING' }"),
			*EscapedProjectFilePath);

		FString StdOut;
		FString StdErr;
		int32 ReturnCode = 0;
		FPlatformProcess::ExecProcess(
			TEXT("powershell.exe"),
			*FString::Printf(TEXT("-NoProfile -NonInteractive -ExecutionPolicy Bypass -Command \"%s\""), *PowerShellScript.ReplaceCharWithEscapedChar()),
			&ReturnCode,
			&StdOut,
			&StdErr);

		return ReturnCode == 0 && StdOut.Contains(TEXT("RUNNING"));
	}

	bool SaveAsset(UObject& Asset, FString& OutError)
	{
		UPackage* Package = Asset.GetOutermost();
		const FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());

		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.Error = GError;
		SaveArgs.SaveFlags = SAVE_None;

		Package->MarkPackageDirty();
		if (!UPackage::SavePackage(Package, &Asset, *PackageFileName, SaveArgs))
		{
			OutError = FString::Printf(TEXT("Failed to save package '%s'."), *Package->GetName());
			return false;
		}

		return true;
	}

	void ConfigureCollision(UStaticMesh& StaticMesh, CodexVox::ECollisionType CollisionType)
	{
		StaticMesh.CreateBodySetup();
		UBodySetup* BodySetup = StaticMesh.GetBodySetup();
		check(BodySetup != nullptr);

		BodySetup->RemoveSimpleCollision();
		BodySetup->InvalidatePhysicsData();

		switch (CollisionType)
		{
		case CodexVox::ECollisionType::None:
			BodySetup->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseDefault;
			BodySetup->DefaultInstance.SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
			break;
		case CodexVox::ECollisionType::SimpleBox:
		{
			BodySetup->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseSimpleAsComplex;
			BodySetup->DefaultInstance.SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

			const FBoxSphereBounds Bounds = StaticMesh.GetBounds();
			FKBoxElem BoxElem;
			BoxElem.Center = Bounds.Origin;
			BoxElem.X = Bounds.BoxExtent.X * 2.0f;
			BoxElem.Y = Bounds.BoxExtent.Y * 2.0f;
			BoxElem.Z = Bounds.BoxExtent.Z * 2.0f;
			BodySetup->AggGeom.BoxElems.Add(BoxElem);
			break;
		}
		case CodexVox::ECollisionType::SimpleAndComplex:
		{
			BodySetup->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseSimpleAndComplex;
			BodySetup->DefaultInstance.SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

			const FBoxSphereBounds Bounds = StaticMesh.GetBounds();
			FKBoxElem BoxElem;
			BoxElem.Center = Bounds.Origin;
			BoxElem.X = Bounds.BoxExtent.X * 2.0f;
			BoxElem.Y = Bounds.BoxExtent.Y * 2.0f;
			BoxElem.Z = Bounds.BoxExtent.Z * 2.0f;
			BodySetup->AggGeom.BoxElems.Add(BoxElem);
			break;
		}
		case CodexVox::ECollisionType::UseComplexAsSimple:
		default:
			BodySetup->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;
			BodySetup->DefaultInstance.SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
			break;
		}

		BodySetup->CreatePhysicsMeshes();
	}

	bool CreateOrUpdateStaticMeshAsset(
		const CodexVox::FManifestEntry& Entry,
		const CodexVox::FParsedModel& Model,
		UMaterial& SharedMaterial,
		float VoxelSize,
		bool bNoOverwrite,
		bool bVerbose,
		bool& bOutSkipped,
		FString& OutError)
	{
		bOutSkipped = false;

		const FString PackageName = FString::Printf(TEXT("%s/%s"), *Entry.TargetPackagePath, *Entry.TargetAssetName);
		const FString ObjectPath = CodexVox::MakeObjectPath(Entry.TargetPackagePath, Entry.TargetAssetName);

		UPackage* Package = CreatePackage(*PackageName);
		Package->FullyLoad();

		UObject* ExistingObject = LoadObject<UObject>(nullptr, *ObjectPath);
		UStaticMesh* StaticMesh = ExistingObject ? Cast<UStaticMesh>(ExistingObject) : nullptr;
		if (ExistingObject != nullptr && StaticMesh == nullptr)
		{
			OutError = FString::Printf(TEXT("Existing asset '%s' is not a UStaticMesh."), *ObjectPath);
			return false;
		}

		if (StaticMesh != nullptr && bNoOverwrite)
		{
			bOutSkipped = true;
			UE_LOG(LogCodexVox, Display, TEXT("Skipping existing mesh '%s' because -NoOverwrite was used."), *ObjectPath);
			return true;
		}

		const bool bIsNewAsset = StaticMesh == nullptr;
		if (bIsNewAsset)
		{
			StaticMesh = NewObject<UStaticMesh>(Package, *Entry.TargetAssetName, RF_Public | RF_Standalone);
			FAssetRegistryModule::AssetCreated(StaticMesh);
		}
		else
		{
			StaticMesh->Modify();
		}

		CodexVox::FMeshBuildInput MeshBuildInput;
		MeshBuildInput.Model = &Model;
		MeshBuildInput.PivotRule = Entry.PivotRule;
		MeshBuildInput.VoxelSize = VoxelSize;

		CodexVox::FMeshBuildOutput MeshBuildOutput;
		if (!FCodexVoxMeshBuilder::BuildMeshDescription(MeshBuildInput, MeshBuildOutput, OutError))
		{
			return false;
		}

		StaticMesh->SetNumSourceModels(1);
		FStaticMeshSourceModel& SourceModel = StaticMesh->GetSourceModel(0);
		SourceModel.BuildSettings = FMeshBuildSettings();
		SourceModel.BuildSettings.bUseMikkTSpace = false;
		SourceModel.BuildSettings.bRecomputeNormals = false;
		SourceModel.BuildSettings.bRecomputeTangents = false;
		SourceModel.BuildSettings.bRemoveDegenerates = false;
		SourceModel.BuildSettings.bGenerateLightmapUVs = false;
		SourceModel.SourceImportFilename = Entry.SourceVoxFile;
		SourceModel.ScreenSize.Default = 1.0f;

		const TArray<FStaticMaterial> Materials =
		{
			FStaticMaterial(&SharedMaterial, CodexVox::MaterialSlotName, CodexVox::MaterialSlotName)
		};
		StaticMesh->SetStaticMaterials(Materials);

		StaticMesh->GetSectionInfoMap().Clear();
		StaticMesh->GetOriginalSectionInfoMap().Clear();
		FMeshSectionInfo SectionInfo(0);
		SectionInfo.bEnableCollision = Entry.CollisionType != CodexVox::ECollisionType::None;
		StaticMesh->GetSectionInfoMap().Set(0, 0, SectionInfo);
		StaticMesh->GetOriginalSectionInfoMap().Set(0, 0, SectionInfo);

		FMeshNaniteSettings NaniteSettings = StaticMesh->GetNaniteSettings();
		NaniteSettings.bEnabled = false;
		StaticMesh->SetNaniteSettings(NaniteSettings);
		StaticMesh->SetImportVersion(EImportStaticMeshVersion::LastVersion);
		StaticMesh->SetLightingGuid();

		StaticMesh->CreateMeshDescription(0, MoveTemp(MeshBuildOutput.MeshDescription));
		StaticMesh->CommitMeshDescription(0);

		TArray<FText> BuildErrors;
		UStaticMesh::FBuildParameters BuildParameters;
		BuildParameters.bInSilent = true;
		BuildParameters.OutErrors = &BuildErrors;
		StaticMesh->Build(BuildParameters);

		if (!StaticMesh->HasValidRenderData())
		{
			FString BuildErrorText;
			for (const FText& BuildError : BuildErrors)
			{
				if (!BuildErrorText.IsEmpty())
				{
					BuildErrorText += TEXT(" | ");
				}

				BuildErrorText += BuildError.ToString();
			}

			OutError = FString::Printf(TEXT("Static mesh build failed for '%s'. %s"), *ObjectPath, *BuildErrorText);
			return false;
		}

		StaticMesh->SetMaterial(0, &SharedMaterial);
		ConfigureCollision(*StaticMesh, Entry.CollisionType);
		StaticMesh->MarkPackageDirty();
		Package->MarkPackageDirty();

		if (bVerbose)
		{
			UE_LOG(
				LogCodexVox,
				Display,
				TEXT("Built '%s' from '%s' with %d voxels and %d exposed faces."),
				*ObjectPath,
				*Entry.SourceVoxFile,
				Model.Voxels.Num(),
				MeshBuildOutput.ExposedFaceCount);
		}

		return SaveAsset(*StaticMesh, OutError);
	}
}

bool FCodexVoxAssetGenerator::RunBuild(const CodexVox::FBuildOptions& Options, FString& OutError)
{
	const FString ManifestPath = FPaths::ConvertRelativePathToFull(Options.ManifestPath);
	if (!FPaths::FileExists(ManifestPath))
	{
		OutError = FString::Printf(TEXT("Manifest file '%s' does not exist."), *ManifestPath);
		return false;
	}

	if (IsCurrentProjectEditorRunning())
	{
		OutError = TEXT("An Unreal Editor session for this project is running. Stop the editor and rerun the VOX asset build.");
		return false;
	}

	TArray<CodexVox::FManifestEntry> ManifestEntries;
	float ManifestVoxelSize = Options.VoxelSize;
	if (!LoadManifestJson(ManifestPath, ManifestEntries, ManifestVoxelSize, OutError))
	{
		return false;
	}

	UMaterial* SharedMaterial = nullptr;
	if (Options.bSkipMaterialUpdate)
	{
		const FString SharedMaterialObjectPath = CodexVox::MakeObjectPath(CodexVox::MaterialPackagePath, CodexVox::MaterialAssetName);
		SharedMaterial = LoadObject<UMaterial>(nullptr, *SharedMaterialObjectPath);
		if (SharedMaterial == nullptr)
		{
			OutError = FString::Printf(
				TEXT("Shared VOX material '%s' does not exist. Run the build without -SkipMaterialUpdate first."),
				*SharedMaterialObjectPath);
			return false;
		}

		SharedMaterial->GetOutermost()->FullyLoad();
	}
	else
	{
		if (!FCodexVoxMaterialBuilder::CreateOrUpdateSharedMaterial(SharedMaterial, OutError))
		{
			return false;
		}

		if (!SaveAsset(*SharedMaterial, OutError))
		{
			return false;
		}
	}

	const FString ManifestDirectory = FPaths::GetPath(ManifestPath);
	for (const CodexVox::FManifestEntry& Entry : ManifestEntries)
	{
		const FString SourceFilePath = FPaths::ConvertRelativePathToFull(FPaths::Combine(ManifestDirectory, Entry.SourceVoxFile));
		if (!FPaths::FileExists(SourceFilePath))
		{
			OutError = FString::Printf(TEXT("VOX source '%s' referenced by asset '%s' does not exist."), *SourceFilePath, *Entry.Id);
			return false;
		}

		CodexVox::FParsedModel ParsedModel;
		if (!FCodexVoxParser::LoadModelFromFile(SourceFilePath, ParsedModel, OutError))
		{
			return false;
		}

		bool bSkipped = false;
		if (!CreateOrUpdateStaticMeshAsset(
			Entry,
			ParsedModel,
			*SharedMaterial,
			ManifestVoxelSize,
			Options.bNoOverwrite,
			Options.bVerbose,
			bSkipped,
			OutError))
		{
			return false;
		}
	}

	return true;
}
