#pragma once

#include "PrototypeMeshBuilderBridge.h"

class FPrototypeCodexMeshBridge final : public IPrototypeMeshBridge
{
public:
	virtual FPrototypeBridgeResult GenerateDsl(const FPrototypeMeshRequest& Request) override;
};
