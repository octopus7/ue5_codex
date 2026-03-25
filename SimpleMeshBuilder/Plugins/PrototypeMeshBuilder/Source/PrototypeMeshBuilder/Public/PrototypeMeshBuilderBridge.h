#pragma once

#include "PrototypeMeshBuilderTypes.h"

class IPrototypeMeshBridge
{
public:
	virtual ~IPrototypeMeshBridge() = default;

	virtual FPrototypeBridgeResult GenerateDsl(const FPrototypeMeshRequest& Request) = 0;
	virtual bool StartGenerateDslAsync(const FPrototypeMeshRequest& Request, FPrototypeBridgeJobHandle& OutHandle, FString& OutError) = 0;
	virtual bool PollGenerateDsl(const FPrototypeBridgeJobHandle& Handle, bool& bOutCompleted, FPrototypeBridgeResult& OutResult, FString& OutError) = 0;
	virtual void CancelGenerateDsl(const FPrototypeBridgeJobHandle& Handle) = 0;
	virtual void CancelAllGenerateDsl() = 0;
};
