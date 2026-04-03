#include "Effects/CodexHarnessPlayerHitCameraShake.h"

void UCodexHarnessPlayerHitCameraShakePattern::GetShakePatternInfoImpl(FCameraShakeInfo& OutInfo) const
{
	OutInfo.Duration = FCameraShakeDuration(Duration);
	OutInfo.BlendIn = 0.0f;
	OutInfo.BlendOut = 0.0f;
}

void UCodexHarnessPlayerHitCameraShakePattern::StartShakePatternImpl(const FCameraShakePatternStartParams& Params)
{
	static_cast<void>(Params);
	ElapsedTime = 0.0f;
}

void UCodexHarnessPlayerHitCameraShakePattern::UpdateShakePatternImpl(
	const FCameraShakePatternUpdateParams& Params,
	FCameraShakePatternUpdateResult& OutResult)
{
	ElapsedTime += Params.DeltaTime;
	ApplyShakeAtTime(ElapsedTime, Params.GetTotalScale(), OutResult);
}

void UCodexHarnessPlayerHitCameraShakePattern::ScrubShakePatternImpl(
	const FCameraShakePatternScrubParams& Params,
	FCameraShakePatternUpdateResult& OutResult)
{
	ElapsedTime = Params.AbsoluteTime;
	ApplyShakeAtTime(ElapsedTime, Params.GetTotalScale(), OutResult);
}

bool UCodexHarnessPlayerHitCameraShakePattern::IsFinishedImpl() const
{
	return ElapsedTime >= Duration;
}

void UCodexHarnessPlayerHitCameraShakePattern::ApplyShakeAtTime(
	const float TimeSeconds,
	const float Scale,
	FCameraShakePatternUpdateResult& OutResult) const
{
	static_cast<void>(Scale);

	if (Duration <= KINDA_SMALL_NUMBER || TimeSeconds >= Duration)
	{
		return;
	}

	const float Envelope = 1.0f - (TimeSeconds / Duration);
	const float Pitch = PitchAmplitude * Envelope * FMath::Sin(TimeSeconds * PitchFrequency * UE_TWO_PI);
	const float Yaw = YawAmplitude * Envelope * FMath::Sin(TimeSeconds * YawFrequency * UE_TWO_PI);
	const float Roll = RollAmplitude * Envelope * FMath::Sin(TimeSeconds * RollFrequency * UE_TWO_PI);
	OutResult.Rotation += FRotator(Pitch, Yaw, Roll);
}

UCodexHarnessPlayerHitCameraShake::UCodexHarnessPlayerHitCameraShake(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bSingleInstance = true;

	if (GetRootShakePattern() == nullptr)
	{
		UCameraShakePattern* const RootPattern = ObjectInitializer.CreateDefaultSubobject<UCodexHarnessPlayerHitCameraShakePattern>(
			this,
			TEXT("RootShakePattern"));
		SetRootShakePattern(RootPattern);
	}
}
