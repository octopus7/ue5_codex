#pragma once

#include "HAL/PlatformProcess.h"
#include "PrototypeMeshBuilderBridge.h"

class FPrototypeCodexMeshBridge final : public IPrototypeMeshBridge
{
public:
	struct FRunningBridgeJob
	{
		FPrototypeMeshRequest Request;
		FProcHandle ProcessHandle;
		void* OutputReadPipe = nullptr;
		void* OutputWritePipe = nullptr;
		void* InputReadPipe = nullptr;
		void* InputWritePipe = nullptr;
		FString StdOut;
		double StartTimeSeconds = 0.0;
	};

	virtual FPrototypeBridgeResult GenerateDsl(const FPrototypeMeshRequest& Request) override;
	virtual bool StartGenerateDslAsync(const FPrototypeMeshRequest& Request, FPrototypeBridgeJobHandle& OutHandle, FString& OutError) override;
	virtual bool PollGenerateDsl(const FPrototypeBridgeJobHandle& Handle, bool& bOutCompleted, FPrototypeBridgeResult& OutResult, FString& OutError) override;
	virtual void CancelGenerateDsl(const FPrototypeBridgeJobHandle& Handle) override;
	virtual void CancelAllGenerateDsl() override;
	virtual ~FPrototypeCodexMeshBridge() override;

private:
	TMap<FGuid, TUniquePtr<FRunningBridgeJob>> ActiveJobs;
};
