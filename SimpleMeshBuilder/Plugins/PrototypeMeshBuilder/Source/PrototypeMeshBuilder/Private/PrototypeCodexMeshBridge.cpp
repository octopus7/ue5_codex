#include "PrototypeCodexMeshBridge.h"

#include "Dom/JsonObject.h"
#include "HAL/PlatformProcess.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace
{
	constexpr double BridgeTimeoutSeconds = 180.0;

	void ClosePipePair(void*& ReadPipe, void*& WritePipe)
	{
		if (ReadPipe || WritePipe)
		{
			FPlatformProcess::ClosePipe(ReadPipe, WritePipe);
			ReadPipe = nullptr;
			WritePipe = nullptr;
		}
	}

	void CleanupRunningJob(FPrototypeCodexMeshBridge::FRunningBridgeJob& Job)
	{
		if (Job.ProcessHandle.IsValid())
		{
			FPlatformProcess::CloseProc(Job.ProcessHandle);
			Job.ProcessHandle.Reset();
		}

		ClosePipePair(Job.OutputReadPipe, Job.OutputWritePipe);
		ClosePipePair(Job.InputReadPipe, Job.InputWritePipe);
	}

	bool TryParseBridgeResponse(const FString& StdOut, int32 ReturnCode, FPrototypeBridgeResult& OutResult)
	{
		OutResult = FPrototypeBridgeResult();

		TSharedPtr<FJsonObject> ResponseObject;
		const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(StdOut);
		if (!FJsonSerializer::Deserialize(Reader, ResponseObject) || !ResponseObject.IsValid())
		{
			OutResult.ErrorMessage = ReturnCode != 0
				? FString::Printf(TEXT("Python bridge exited with code %d."), ReturnCode)
				: TEXT("Mesh bridge returned an invalid JSON response.");
			OutResult.Diagnostics = StdOut;
			return false;
		}

		bool bSuccess = false;
		if (!ResponseObject->TryGetBoolField(TEXT("success"), bSuccess) || !bSuccess)
		{
			ResponseObject->TryGetStringField(TEXT("error"), OutResult.ErrorMessage);
			ResponseObject->TryGetStringField(TEXT("diagnostics"), OutResult.Diagnostics);
			if (OutResult.ErrorMessage.IsEmpty())
			{
				OutResult.ErrorMessage = TEXT("Mesh bridge reported failure.");
			}
			return false;
		}

		OutResult.bSuccess = true;
		ResponseObject->TryGetStringField(TEXT("raw_json"), OutResult.RawDslJson);
		ResponseObject->TryGetStringField(TEXT("raw_last_message"), OutResult.RawLastMessage);
		ResponseObject->TryGetStringField(TEXT("diagnostics"), OutResult.Diagnostics);
		if (OutResult.RawDslJson.IsEmpty())
		{
			OutResult.ErrorMessage = TEXT("Mesh bridge succeeded but did not return DSL JSON.");
			OutResult.bSuccess = false;
			return false;
		}

		if (ReturnCode != 0)
		{
			OutResult.ErrorMessage = FString::Printf(TEXT("Python bridge exited with code %d despite returning DSL output."), ReturnCode);
			OutResult.bSuccess = false;
			return false;
		}

		return true;
	}

	bool BuildBridgeRequestPayload(const FPrototypeMeshRequest& Request, FString& OutPayload, FString& OutError)
	{
		TSharedRef<FJsonObject> RequestJson = MakeShared<FJsonObject>();
		RequestJson->SetStringField(TEXT("prompt"), Request.Prompt);
		RequestJson->SetStringField(TEXT("asset_name"), Request.AssetName);
		RequestJson->SetStringField(TEXT("content_path"), Request.ContentPath);
		RequestJson->SetStringField(TEXT("locale"), Request.Locale);
		RequestJson->SetStringField(TEXT("reasoning_effort"), Request.ReasoningEffort);
		RequestJson->SetNumberField(TEXT("max_primitive_count"), Request.MaxPrimitiveCount);
		RequestJson->SetNumberField(TEXT("timeout_seconds"), 150.0);
		RequestJson->SetStringField(TEXT("project_dir"), FPaths::ProjectDir());

		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutPayload);
		if (!FJsonSerializer::Serialize(RequestJson, Writer))
		{
			OutError = TEXT("Failed to serialize bridge request JSON.");
			return false;
		}

		return true;
	}

	bool GetBridgeScriptPath(FString& OutScriptPath, FString& OutError)
	{
		OutScriptPath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Tools"), TEXT("mesh_bridge.py"));
		if (!FPaths::FileExists(OutScriptPath))
		{
			OutError = FString::Printf(TEXT("Bridge script not found: %s"), *OutScriptPath);
			return false;
		}

		return true;
	}

	bool LaunchBridgeJob(const FPrototypeMeshRequest& Request, FPrototypeCodexMeshBridge::FRunningBridgeJob& OutJob, FString& OutError)
	{
		FString ScriptPath;
		if (!GetBridgeScriptPath(ScriptPath, OutError))
		{
			return false;
		}

		FString Payload;
		if (!BuildBridgeRequestPayload(Request, Payload, OutError))
		{
			return false;
		}

		if (!FPlatformProcess::CreatePipe(OutJob.OutputReadPipe, OutJob.OutputWritePipe))
		{
			OutError = TEXT("Failed to create stdout pipe for mesh bridge.");
			return false;
		}

		if (!FPlatformProcess::CreatePipe(OutJob.InputReadPipe, OutJob.InputWritePipe, true))
		{
			ClosePipePair(OutJob.OutputReadPipe, OutJob.OutputWritePipe);
			OutError = TEXT("Failed to create stdin pipe for mesh bridge.");
			return false;
		}

		const FString Parameters = FString::Printf(TEXT("\"%s\""), *ScriptPath);
		uint32 ProcessId = 0;
		OutJob.ProcessHandle = FPlatformProcess::CreateProc(
			TEXT("python"),
			*Parameters,
			false,
			true,
			true,
			&ProcessId,
			0,
			*FPaths::ProjectDir(),
			OutJob.OutputWritePipe,
			OutJob.InputReadPipe);

		if (!OutJob.ProcessHandle.IsValid())
		{
			ClosePipePair(OutJob.OutputReadPipe, OutJob.OutputWritePipe);
			ClosePipePair(OutJob.InputReadPipe, OutJob.InputWritePipe);
			OutError = TEXT("Failed to launch Python mesh bridge.");
			return false;
		}

		FPlatformProcess::WritePipe(OutJob.InputWritePipe, Payload);
		ClosePipePair(OutJob.InputReadPipe, OutJob.InputWritePipe);

		OutJob.Request = Request;
		OutJob.StartTimeSeconds = FPlatformTime::Seconds();
		return true;
	}

	bool RunBridgeProcessBlocking(const FPrototypeMeshRequest& Request, FString& OutStdOut, int32& OutReturnCode, FString& OutError)
	{
		FPrototypeCodexMeshBridge::FRunningBridgeJob Job;
		if (!LaunchBridgeJob(Request, Job, OutError))
		{
			return false;
		}

		while (FPlatformProcess::IsProcRunning(Job.ProcessHandle))
		{
			OutStdOut += FPlatformProcess::ReadPipe(Job.OutputReadPipe);
			if ((FPlatformTime::Seconds() - Job.StartTimeSeconds) > BridgeTimeoutSeconds)
			{
				FPlatformProcess::TerminateProc(Job.ProcessHandle, true);
				CleanupRunningJob(Job);
				OutError = TEXT("Python mesh bridge timed out.");
				return false;
			}

			FPlatformProcess::Sleep(0.02f);
		}

		OutStdOut += FPlatformProcess::ReadPipe(Job.OutputReadPipe);
		OutReturnCode = 0;
		FPlatformProcess::GetProcReturnCode(Job.ProcessHandle, &OutReturnCode);
		CleanupRunningJob(Job);
		return true;
	}
}

FPrototypeBridgeResult FPrototypeCodexMeshBridge::GenerateDsl(const FPrototypeMeshRequest& Request)
{
	FPrototypeBridgeResult Result;

	FString StdOut;
	int32 ReturnCode = 0;
	FString LaunchError;
	if (!RunBridgeProcessBlocking(Request, StdOut, ReturnCode, LaunchError))
	{
		Result.ErrorMessage = LaunchError.IsEmpty() ? StdOut : LaunchError;
		Result.Diagnostics = StdOut;
		return Result;
	}

	TryParseBridgeResponse(StdOut, ReturnCode, Result);
	return Result;
}

bool FPrototypeCodexMeshBridge::StartGenerateDslAsync(const FPrototypeMeshRequest& Request, FPrototypeBridgeJobHandle& OutHandle, FString& OutError)
{
	OutHandle = FPrototypeBridgeJobHandle();

	TUniquePtr<FRunningBridgeJob> Job = MakeUnique<FRunningBridgeJob>();
	if (!LaunchBridgeJob(Request, *Job, OutError))
	{
		return false;
	}

	OutHandle.Id = FGuid::NewGuid();
	ActiveJobs.Add(OutHandle.Id, MoveTemp(Job));
	return true;
}

bool FPrototypeCodexMeshBridge::PollGenerateDsl(const FPrototypeBridgeJobHandle& Handle, bool& bOutCompleted, FPrototypeBridgeResult& OutResult, FString& OutError)
{
	bOutCompleted = false;
	OutResult = FPrototypeBridgeResult();
	OutError.Empty();

	if (!Handle.IsValid())
	{
		OutError = TEXT("Bridge job handle is invalid.");
		return false;
	}

	TUniquePtr<FRunningBridgeJob>* JobPtr = ActiveJobs.Find(Handle.Id);
	if (!JobPtr || !JobPtr->IsValid())
	{
		OutError = TEXT("Bridge job was not found.");
		return false;
	}

	FRunningBridgeJob& Job = *JobPtr->Get();
	Job.StdOut += FPlatformProcess::ReadPipe(Job.OutputReadPipe);

	if (FPlatformProcess::IsProcRunning(Job.ProcessHandle))
	{
		if ((FPlatformTime::Seconds() - Job.StartTimeSeconds) > BridgeTimeoutSeconds)
		{
			FPlatformProcess::TerminateProc(Job.ProcessHandle, true);
			OutResult.ErrorMessage = TEXT("Python mesh bridge timed out.");
			OutResult.Diagnostics = Job.StdOut;
			CleanupRunningJob(Job);
			ActiveJobs.Remove(Handle.Id);
			bOutCompleted = true;
		}

		return true;
	}

	Job.StdOut += FPlatformProcess::ReadPipe(Job.OutputReadPipe);
	int32 ReturnCode = 0;
	FPlatformProcess::GetProcReturnCode(Job.ProcessHandle, &ReturnCode);
	TryParseBridgeResponse(Job.StdOut, ReturnCode, OutResult);
	CleanupRunningJob(Job);
	ActiveJobs.Remove(Handle.Id);
	bOutCompleted = true;
	return true;
}

void FPrototypeCodexMeshBridge::CancelGenerateDsl(const FPrototypeBridgeJobHandle& Handle)
{
	if (!Handle.IsValid())
	{
		return;
	}

	if (TUniquePtr<FRunningBridgeJob>* JobPtr = ActiveJobs.Find(Handle.Id))
	{
		if (JobPtr->IsValid() && (*JobPtr)->ProcessHandle.IsValid())
		{
			FPlatformProcess::TerminateProc((*JobPtr)->ProcessHandle, true);
			CleanupRunningJob(*(*JobPtr));
		}

		ActiveJobs.Remove(Handle.Id);
	}
}

void FPrototypeCodexMeshBridge::CancelAllGenerateDsl()
{
	for (TPair<FGuid, TUniquePtr<FRunningBridgeJob>>& Pair : ActiveJobs)
	{
		if (Pair.Value.IsValid() && Pair.Value->ProcessHandle.IsValid())
		{
			FPlatformProcess::TerminateProc(Pair.Value->ProcessHandle, true);
			CleanupRunningJob(*Pair.Value);
		}
	}

	ActiveJobs.Reset();
}

FPrototypeCodexMeshBridge::~FPrototypeCodexMeshBridge()
{
	CancelAllGenerateDsl();
}
