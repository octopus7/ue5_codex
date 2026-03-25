#include "PrototypeCodexMeshBridge.h"

#include "Dom/JsonObject.h"
#include "HAL/PlatformProcess.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

namespace
{
	bool RunBridgeProcess(const FString& ExePath, const FString& Parameters, const FString& WorkingDirectory, const FString& StdInPayload, FString& OutStdOut, int32& OutReturnCode, FString& OutError)
	{
		void* OutputReadPipe = nullptr;
		void* OutputWritePipe = nullptr;
		void* InputReadPipe = nullptr;
		void* InputWritePipe = nullptr;

		if (!FPlatformProcess::CreatePipe(OutputReadPipe, OutputWritePipe))
		{
			OutError = TEXT("Failed to create stdout pipe for mesh bridge.");
			return false;
		}

		if (!FPlatformProcess::CreatePipe(InputReadPipe, InputWritePipe, true))
		{
			FPlatformProcess::ClosePipe(OutputReadPipe, OutputWritePipe);
			OutError = TEXT("Failed to create stdin pipe for mesh bridge.");
			return false;
		}

		uint32 ProcessId = 0;
		FProcHandle ProcessHandle = FPlatformProcess::CreateProc(
			*ExePath,
			*Parameters,
			false,
			true,
			true,
			&ProcessId,
			0,
			*WorkingDirectory,
			OutputWritePipe,
			InputReadPipe);

		if (!ProcessHandle.IsValid())
		{
			FPlatformProcess::ClosePipe(OutputReadPipe, OutputWritePipe);
			FPlatformProcess::ClosePipe(InputReadPipe, InputWritePipe);
			OutError = TEXT("Failed to launch Python mesh bridge.");
			return false;
		}

		FPlatformProcess::WritePipe(InputWritePipe, StdInPayload);
		FPlatformProcess::ClosePipe(InputReadPipe, InputWritePipe);

		const double StartTime = FPlatformTime::Seconds();
		while (FPlatformProcess::IsProcRunning(ProcessHandle))
		{
			OutStdOut += FPlatformProcess::ReadPipe(OutputReadPipe);
			if ((FPlatformTime::Seconds() - StartTime) > 180.0)
			{
				FPlatformProcess::TerminateProc(ProcessHandle, true);
				OutError = TEXT("Python mesh bridge timed out.");
				break;
			}

			FPlatformProcess::Sleep(0.02f);
		}

		OutStdOut += FPlatformProcess::ReadPipe(OutputReadPipe);

		OutReturnCode = 0;
		FPlatformProcess::GetProcReturnCode(ProcessHandle, &OutReturnCode);
		FPlatformProcess::CloseProc(ProcessHandle);
		FPlatformProcess::ClosePipe(OutputReadPipe, OutputWritePipe);

		if (!OutError.IsEmpty())
		{
			return false;
		}

		return true;
	}
}

FPrototypeBridgeResult FPrototypeCodexMeshBridge::GenerateDsl(const FPrototypeMeshRequest& Request)
{
	FPrototypeBridgeResult Result;

	const FString ScriptPath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Tools"), TEXT("mesh_bridge.py"));
	if (!FPaths::FileExists(ScriptPath))
	{
		Result.ErrorMessage = FString::Printf(TEXT("Bridge script not found: %s"), *ScriptPath);
		return Result;
	}

	TSharedRef<FJsonObject> RequestJson = MakeShared<FJsonObject>();
	RequestJson->SetStringField(TEXT("prompt"), Request.Prompt);
	RequestJson->SetStringField(TEXT("asset_name"), Request.AssetName);
	RequestJson->SetStringField(TEXT("content_path"), Request.ContentPath);
	RequestJson->SetStringField(TEXT("locale"), Request.Locale);
	RequestJson->SetStringField(TEXT("reasoning_effort"), Request.ReasoningEffort);
	RequestJson->SetNumberField(TEXT("max_primitive_count"), Request.MaxPrimitiveCount);
	RequestJson->SetNumberField(TEXT("timeout_seconds"), 150.0);
	RequestJson->SetStringField(TEXT("project_dir"), FPaths::ProjectDir());

	FString RequestPayload;
	{
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestPayload);
		FJsonSerializer::Serialize(RequestJson, Writer);
	}

	const FString Parameters = FString::Printf(TEXT("\"%s\""), *ScriptPath);
	FString StdOut;
	int32 ReturnCode = 0;
	FString LaunchError;
	if (!RunBridgeProcess(TEXT("python"), Parameters, FPaths::ProjectDir(), RequestPayload, StdOut, ReturnCode, LaunchError))
	{
		Result.ErrorMessage = LaunchError.IsEmpty() ? StdOut : LaunchError;
		Result.Diagnostics = StdOut;
		return Result;
	}

	TSharedPtr<FJsonObject> ResponseObject;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(StdOut);
	if (!FJsonSerializer::Deserialize(Reader, ResponseObject) || !ResponseObject.IsValid())
	{
		Result.ErrorMessage = ReturnCode != 0
			? FString::Printf(TEXT("Python bridge exited with code %d."), ReturnCode)
			: TEXT("Mesh bridge returned an invalid JSON response.");
		Result.Diagnostics = StdOut;
		return Result;
	}

	bool bSuccess = false;
	if (!ResponseObject->TryGetBoolField(TEXT("success"), bSuccess) || !bSuccess)
	{
		ResponseObject->TryGetStringField(TEXT("error"), Result.ErrorMessage);
		ResponseObject->TryGetStringField(TEXT("diagnostics"), Result.Diagnostics);
		if (Result.ErrorMessage.IsEmpty())
		{
			Result.ErrorMessage = TEXT("Mesh bridge reported failure.");
		}
		return Result;
	}

	Result.bSuccess = true;
	ResponseObject->TryGetStringField(TEXT("raw_json"), Result.RawDslJson);
	ResponseObject->TryGetStringField(TEXT("diagnostics"), Result.Diagnostics);
	if (Result.RawDslJson.IsEmpty())
	{
		Result.ErrorMessage = TEXT("Mesh bridge succeeded but did not return DSL JSON.");
		Result.bSuccess = false;
	}
	else if (ReturnCode != 0)
	{
		Result.ErrorMessage = FString::Printf(TEXT("Python bridge exited with code %d despite returning DSL output."), ReturnCode);
		Result.bSuccess = false;
	}

	return Result;
}
