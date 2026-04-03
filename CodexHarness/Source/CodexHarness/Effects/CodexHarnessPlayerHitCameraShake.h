#pragma once

#include "Camera/CameraShakeBase.h"
#include "CodexHarnessPlayerHitCameraShake.generated.h"

UCLASS(NotBlueprintable)
class CODEXHARNESS_API UCodexHarnessPlayerHitCameraShakePattern : public UCameraShakePattern
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shake", meta = (ClampMin = "0.01"))
	float Duration = 0.22f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shake", meta = (ClampMin = "0.0"))
	float PitchAmplitude = 2.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shake", meta = (ClampMin = "0.0"))
	float YawAmplitude = 1.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shake", meta = (ClampMin = "0.0"))
	float RollAmplitude = 0.7f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shake", meta = (ClampMin = "0.0"))
	float PitchFrequency = 18.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shake", meta = (ClampMin = "0.0"))
	float YawFrequency = 23.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Shake", meta = (ClampMin = "0.0"))
	float RollFrequency = 16.0f;

protected:
	virtual void GetShakePatternInfoImpl(FCameraShakeInfo& OutInfo) const override;
	virtual void StartShakePatternImpl(const FCameraShakePatternStartParams& Params) override;
	virtual void UpdateShakePatternImpl(const FCameraShakePatternUpdateParams& Params, FCameraShakePatternUpdateResult& OutResult) override;
	virtual void ScrubShakePatternImpl(const FCameraShakePatternScrubParams& Params, FCameraShakePatternUpdateResult& OutResult) override;
	virtual bool IsFinishedImpl() const override;

private:
	void ApplyShakeAtTime(float TimeSeconds, float Scale, FCameraShakePatternUpdateResult& OutResult) const;

	float ElapsedTime = 0.0f;
};

UCLASS(Blueprintable)
class CODEXHARNESS_API UCodexHarnessPlayerHitCameraShake : public UCameraShakeBase
{
	GENERATED_BODY()

public:
	UCodexHarnessPlayerHitCameraShake(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
