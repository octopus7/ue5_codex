#include "Effects/CodexHarnessEffectsConfigDataAsset.h"

bool UCodexHarnessEffectsConfigDataAsset::HasRequiredAssets() const
{
	return PlayerHitReactionSystem != nullptr && PlayerHitCameraShakeClass != nullptr;
}
