#pragma once

#include "CMWCombatTypes.generated.h"

UENUM(BlueprintType)
enum class ECMWAttackMode : uint8
{
	Projectile UMETA(DisplayName = "Projectile"),
	Melee UMETA(DisplayName = "Melee")
};
