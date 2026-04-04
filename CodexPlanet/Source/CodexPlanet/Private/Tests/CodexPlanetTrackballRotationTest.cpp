// Copyright Epic Games, Inc. All Rights Reserved.

#if WITH_DEV_AUTOMATION_TESTS

#if WITH_EDITOR
#include "Blueprint/WidgetTree.h"
#include "WidgetBlueprint.h"
#endif
#include "Engine/StaticMesh.h"
#include "Misc/AutomationTest.h"
#include "Player/CodexPlanetPlayerController.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCodexPlanetTrackballRotationTest,
	"CodexPlanet.M1P1.TrackballRotationMath",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCodexPlanetSurfaceAlignmentTest,
	"CodexPlanet.M1P2.SurfaceAlignmentMath",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCodexPlanetPropCatalogAssetTest,
	"CodexPlanet.M1P3.PropCatalogAssets",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FCodexPlanetOrbitControlsWidgetAssetTest,
	"CodexPlanet.M1P4.OrbitControlsWidgetAsset",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCodexPlanetTrackballRotationTest::RunTest(const FString& Parameters)
{
	const FQuat ZeroRotation = ACodexPlanetPlayerController::BuildTrackballDeltaRotation(FRotator::ZeroRotator, FVector2D::ZeroVector, 0.16f);
	TestTrue(TEXT("Zero drag returns identity rotation"), ZeroRotation.Equals(FQuat::Identity));

	const FQuat HorizontalRotation = ACodexPlanetPlayerController::BuildTrackballDeltaRotation(FRotator::ZeroRotator, FVector2D(10.0f, 0.0f), 0.16f);
	TestFalse(TEXT("Horizontal drag produces a non-identity rotation"), HorizontalRotation.Equals(FQuat::Identity));
	TestTrue(TEXT("Horizontal drag rotates around camera up"), HorizontalRotation.GetRotationAxis().Equals(-FVector::UpVector, 0.01f));

	const FQuat VerticalRotation = ACodexPlanetPlayerController::BuildTrackballDeltaRotation(FRotator::ZeroRotator, FVector2D(0.0f, 10.0f), 0.16f);
	TestTrue(TEXT("Vertical drag rotates around camera right"), VerticalRotation.GetRotationAxis().Equals(FVector::RightVector, 0.01f));

	const FQuat LargerHorizontalRotation = ACodexPlanetPlayerController::BuildTrackballDeltaRotation(FRotator::ZeroRotator, FVector2D(20.0f, 0.0f), 0.16f);
	TestTrue(TEXT("Larger drag increases rotation angle"), LargerHorizontalRotation.GetAngle() > HorizontalRotation.GetAngle());

	return true;
}

bool FCodexPlanetSurfaceAlignmentTest::RunTest(const FString& Parameters)
{
	const FQuat UpSurfaceRotation = ACodexPlanetPlayerController::BuildSurfaceAlignedRotation(FVector::UpVector, FVector::ForwardVector);
	TestTrue(TEXT("Surface rotation keeps local Z aligned to world up"), UpSurfaceRotation.GetAxisZ().Equals(FVector::UpVector, 0.01f));
	TestTrue(TEXT("Surface rotation keeps local X aligned to projected view forward"), UpSurfaceRotation.GetAxisX().Equals(FVector::ForwardVector, 0.01f));

	const FQuat SideSurfaceRotation = ACodexPlanetPlayerController::BuildSurfaceAlignedRotation(FVector::RightVector, FVector::ForwardVector);
	TestTrue(TEXT("Side surface rotation aligns local Z to surface normal"), SideSurfaceRotation.GetAxisZ().Equals(FVector::RightVector, 0.01f));

	const FQuat FallbackRotation = ACodexPlanetPlayerController::BuildSurfaceAlignedRotation(FVector::UpVector, FVector::UpVector);
	TestTrue(TEXT("Fallback alignment still preserves the requested normal"), FallbackRotation.GetAxisZ().Equals(FVector::UpVector, 0.01f));

	return true;
}

bool FCodexPlanetPropCatalogAssetTest::RunTest(const FString& Parameters)
{
	const TArray<const TCHAR*> AssetPaths = {
		TEXT("/Game/Props/Vox/SM_VOX_Bridge.SM_VOX_Bridge"),
		TEXT("/Game/Props/Vox/SM_VOX_Statue.SM_VOX_Statue"),
		TEXT("/Game/Props/Vox/SM_VOX_PalmTree.SM_VOX_PalmTree"),
		TEXT("/Game/Props/Vox/SM_VOX_Rock.SM_VOX_Rock"),
		TEXT("/Game/Props/Vox/SM_VOX_Bush.SM_VOX_Bush"),
	};

	for (const TCHAR* AssetPath : AssetPaths)
	{
		UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, AssetPath);
		TestNotNull(FString::Printf(TEXT("Imported asset should load: %s"), AssetPath), Mesh);
	}

	return true;
}

bool FCodexPlanetOrbitControlsWidgetAssetTest::RunTest(const FString& Parameters)
{
#if WITH_EDITOR
	UWidgetBlueprint* WidgetBlueprint = LoadObject<UWidgetBlueprint>(nullptr, TEXT("/Game/UI/WBP_PlanetOrbitControls.WBP_PlanetOrbitControls"));
	TestNotNull(TEXT("Orbit controls widget blueprint should load"), WidgetBlueprint);

	if (!WidgetBlueprint)
	{
		return false;
	}

	TestNotNull(TEXT("Widget tree should exist"), WidgetBlueprint->WidgetTree.Get());

	if (WidgetBlueprint->WidgetTree)
	{
		TestNotNull(TEXT("RootCanvas should exist"), WidgetBlueprint->WidgetTree->FindWidget(TEXT("RootCanvas")));
		TestNotNull(TEXT("RootBorder should exist"), WidgetBlueprint->WidgetTree->FindWidget(TEXT("RootBorder")));
		TestNotNull(TEXT("AddRingButton should exist"), WidgetBlueprint->WidgetTree->FindWidget(TEXT("AddRingButton")));
		TestNotNull(TEXT("AddSatelliteButton should exist"), WidgetBlueprint->WidgetTree->FindWidget(TEXT("AddSatelliteButton")));
	}

	return true;
#else
	return false;
#endif
}

#endif
