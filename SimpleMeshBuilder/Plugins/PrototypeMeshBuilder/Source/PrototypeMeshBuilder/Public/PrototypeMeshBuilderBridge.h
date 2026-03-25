#pragma once

#include "PrototypeMeshBuilderTypes.h"

class IPrototypeMeshBridge
{
public:
	virtual ~IPrototypeMeshBridge() = default;

	virtual FPrototypeBridgeResult GenerateDsl(const FPrototypeMeshRequest& Request) = 0;
};
