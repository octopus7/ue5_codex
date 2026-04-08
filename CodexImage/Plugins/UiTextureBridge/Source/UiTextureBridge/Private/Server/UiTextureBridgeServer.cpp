#include "Server/UiTextureBridgeServer.h"

#include "Async/Async.h"
#include "Dom/JsonObject.h"
#include "HttpPath.h"
#include "HttpServerConstants.h"
#include "HttpServerModule.h"
#include "HttpServerRequest.h"
#include "HttpServerResponse.h"
#include "IHttpRouter.h"
#include "Import/UiTextureImportService.h"
#include "IPAddress.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonSerializer.h"
#include "Settings/UiTextureBridgeSettings.h"
#include "SocketSubsystem.h"
#include "Sockets.h"

DEFINE_LOG_CATEGORY_STATIC(LogUiTextureBridgeServer, Log, All);

namespace
{
	const TCHAR* ListenerSection = TEXT("HTTPServer.Listeners");
	const TCHAR* ListenerOverridesKey = TEXT("ListenerOverrides");
	const TCHAR* PluginVersion = TEXT("1.0.0");

	FString SerializeJson(const TSharedRef<FJsonObject>& InObject)
	{
		FString Output;
		const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Output);
		FJsonSerializer::Serialize(InObject, Writer);
		return Output;
	}

	EHttpServerResponseCodes ToResponseCode(const int32 InStatusCode)
	{
		switch (InStatusCode)
		{
		case 200:
			return EHttpServerResponseCodes::Ok;
		case 400:
			return EHttpServerResponseCodes::BadRequest;
		case 404:
			return EHttpServerResponseCodes::NotFound;
		default:
			return EHttpServerResponseCodes::ServerError;
		}
	}
}

FUiTextureBridgeServer::FUiTextureBridgeServer()
	: ImportService(MakeUnique<FUiTextureImportService>())
{
}

FUiTextureBridgeServer::~FUiTextureBridgeServer()
{
	Stop();
}

bool FUiTextureBridgeServer::Start(FString& OutError)
{
	if (bRunning)
	{
		return true;
	}

	const UUiTextureBridgeSettings* const SettingsObject = GetDefault<UUiTextureBridgeSettings>();
	if (SettingsObject == nullptr)
	{
		OutError = TEXT("UI Texture Bridge settings object is unavailable.");
		return false;
	}

	const FUiTextureBridgeSettingsSnapshot RequestedSettings = FUiTextureBridgeSettingsSnapshot::FromSettings(*SettingsObject);
	if (!ApplyListenerConfig(RequestedSettings, OutError))
	{
		return false;
	}

	const bool bNeedsNewRouter = !Router.IsValid() || ActiveSettings.Port != RequestedSettings.Port;
	if (bNeedsNewRouter)
	{
		Router = FHttpServerModule::Get().GetHttpRouter(static_cast<uint32>(RequestedSettings.Port));
	}

	if (!Router.IsValid())
	{
		OutError = FString::Printf(TEXT("Failed to create an HTTP router for port %d."), RequestedSettings.Port);
		return false;
	}

	ActiveSettings = RequestedSettings;
	if (!BindRoutes(OutError))
	{
		UnbindRoutes();
		return false;
	}

	FHttpServerModule::Get().StartAllListeners();
	bRunning = true;
	return true;
}

void FUiTextureBridgeServer::Stop()
{
	if (!Router.IsValid())
	{
		bRunning = false;
		return;
	}

	UnbindRoutes();
	FHttpServerModule::Get().StopAllListeners();
	bRunning = false;
}

bool FUiTextureBridgeServer::IsRunning() const
{
	return bRunning;
}

FString FUiTextureBridgeServer::GetBaseUrl() const
{
	return FString::Printf(TEXT("http://%s:%d"), *ActiveSettings.BindAddress, ActiveSettings.Port);
}

bool FUiTextureBridgeServer::ApplyListenerConfig(const FUiTextureBridgeSettingsSnapshot& InSettings, FString& OutError)
{
	const int32 Port = ClampPort(InSettings.Port);
	if (Port < 0)
	{
		OutError = FString::Printf(TEXT("Port %d is outside the valid TCP range."), InSettings.Port);
		return false;
	}

	FString BindAddress = InSettings.BindAddress;
	BindAddress.TrimStartAndEndInline();
	if (BindAddress.IsEmpty())
	{
		OutError = TEXT("Bind address must not be empty.");
		return false;
	}

	if (!CheckPortAvailability(BindAddress, Port, OutError))
	{
		return false;
	}

	TArray<FString> ListenerOverrides;
	GConfig->GetArray(ListenerSection, ListenerOverridesKey, ListenerOverrides, GEngineIni);
	ListenerOverrides.RemoveAll([Port](const FString& InEntry)
	{
		uint32 ConfiguredPort = 0;
		return FParse::Value(*InEntry, TEXT("Port="), ConfiguredPort) && static_cast<int32>(ConfiguredPort) == Port;
	});
	ListenerOverrides.Add(FString::Printf(TEXT("(Port=%d,BindAddress=%s)"), Port, *BindAddress));
	GConfig->SetArray(ListenerSection, ListenerOverridesKey, ListenerOverrides, GEngineIni);

	return true;
}

bool FUiTextureBridgeServer::CheckPortAvailability(const FString& InBindAddress, int32 InPort, FString& OutError) const
{
	ISocketSubsystem* const SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	if (SocketSubsystem == nullptr)
	{
		OutError = TEXT("Socket subsystem is unavailable.");
		return false;
	}

	TSharedRef<FInternetAddr> SocketAddress = SocketSubsystem->CreateInternetAddr();
	bool bIsValidAddress = true;
	if (InBindAddress.Equals(TEXT("localhost"), ESearchCase::IgnoreCase) || InBindAddress.Equals(TEXT("127.0.0.1"), ESearchCase::IgnoreCase))
	{
		SocketAddress->SetLoopbackAddress();
	}
	else if (InBindAddress.Equals(TEXT("any"), ESearchCase::IgnoreCase))
	{
		SocketAddress->SetAnyAddress();
	}
	else
	{
		SocketAddress->SetIp(*InBindAddress, bIsValidAddress);
	}

	if (!bIsValidAddress)
	{
		OutError = FString::Printf(TEXT("Bind address '%s' is not a valid IPv4 address."), *InBindAddress);
		return false;
	}

	SocketAddress->SetPort(static_cast<uint16>(InPort));
	FSocket* const ProbeSocket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("UiTextureBridgePortProbe"), false);
	if (ProbeSocket == nullptr)
	{
		OutError = TEXT("Failed to create a TCP socket for port probing.");
		return false;
	}

	const bool bWasAvailable = ProbeSocket->Bind(*SocketAddress);
	ProbeSocket->Close();
	SocketSubsystem->DestroySocket(ProbeSocket);

	if (!bWasAvailable)
	{
		OutError = FString::Printf(TEXT("Failed to bind %s:%d. Another process may already be using that port."), *InBindAddress, InPort);
		return false;
	}

	return true;
}

bool FUiTextureBridgeServer::BindRoutes(FString& OutError)
{
	check(Router.IsValid());

	HealthRouteHandle = Router->BindRoute(
		FHttpPath(TEXT("/api/health")),
		EHttpServerRequestVerbs::VERB_GET,
		FHttpRequestHandler::CreateRaw(this, &FUiTextureBridgeServer::HandleHealthRequest));

	if (!HealthRouteHandle.IsValid())
	{
		OutError = TEXT("Failed to bind the health endpoint.");
		return false;
	}

	ImportRouteHandle = Router->BindRoute(
		FHttpPath(TEXT("/api/ui-texture/import")),
		EHttpServerRequestVerbs::VERB_POST,
		FHttpRequestHandler::CreateRaw(this, &FUiTextureBridgeServer::HandleImportRequest));

	if (!ImportRouteHandle.IsValid())
	{
		Router->UnbindRoute(HealthRouteHandle);
		HealthRouteHandle = FHttpRouteHandle();
		OutError = TEXT("Failed to bind the import endpoint.");
		return false;
	}

	return true;
}

void FUiTextureBridgeServer::UnbindRoutes()
{
	if (!Router.IsValid())
	{
		return;
	}

	if (HealthRouteHandle.IsValid())
	{
		Router->UnbindRoute(HealthRouteHandle);
		HealthRouteHandle = FHttpRouteHandle();
	}

	if (ImportRouteHandle.IsValid())
	{
		Router->UnbindRoute(ImportRouteHandle);
		ImportRouteHandle = FHttpRouteHandle();
	}
}

bool FUiTextureBridgeServer::HandleHealthRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	(void)Request;

	const TSharedRef<FJsonObject> RootObject = MakeShared<FJsonObject>();
	RootObject->SetBoolField(TEXT("success"), true);
	RootObject->SetStringField(TEXT("status"), bRunning ? TEXT("running") : TEXT("stopped"));
	RootObject->SetStringField(TEXT("version"), PluginVersion);

	TArray<TSharedPtr<FJsonValue>> PresetValues;
	for (const FString& Preset : FUiTextureImportService::GetSupportedPresets())
	{
		PresetValues.Add(MakeShared<FJsonValueString>(Preset));
	}
	RootObject->SetArrayField(TEXT("presets"), PresetValues);

	OnComplete(MakeJsonResponse(SerializeJson(RootObject), 200));
	return true;
}

bool FUiTextureBridgeServer::HandleImportRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	FString RequestBody;
	FFileHelper::BufferToString(RequestBody, Request.Body.GetData(), Request.Body.Num());

	FUiTextureBridgeImportRequest ImportRequest;
	FUiTextureBridgeImportResult ParseFailure;
	if (!FUiTextureImportService::TryParseRequest(RequestBody, ActiveSettings, ImportRequest, ParseFailure))
	{
		OnComplete(MakeJsonResponse(FUiTextureImportService::BuildResponseJson(ParseFailure), ParseFailure.HttpStatusCode));
		return true;
	}

	FUiTextureBridgeImportResult ImportResult;
	if (IsInGameThread())
	{
		ImportResult = FUiTextureImportService::ExecuteImport(ImportRequest);
	}
	else
	{
		const TSharedPtr<FEvent, ESPMode::ThreadSafe> CompletionEvent =
			MakeShareable(FPlatformProcess::GetSynchEventFromPool(true), [](FEvent* Event)
			{
				FPlatformProcess::ReturnSynchEventToPool(Event);
			});
		const TSharedRef<FUiTextureBridgeImportResult, ESPMode::ThreadSafe> SharedResult =
			MakeShared<FUiTextureBridgeImportResult, ESPMode::ThreadSafe>();

		AsyncTask(ENamedThreads::GameThread, [SharedResult, CompletionEvent, ImportRequest]()
		{
			*SharedResult = FUiTextureImportService::ExecuteImport(ImportRequest);
			CompletionEvent->Trigger();
		});

		const float TimeoutSeconds = FMath::Max(ActiveSettings.RequestTimeoutSeconds, 0.1f);
		if (!CompletionEvent->Wait(FTimespan::FromSeconds(TimeoutSeconds)))
		{
			ImportResult.SourceFile = ImportRequest.SourceFile;
			ImportResult.AssetPath = ImportRequest.PackagePath;
			ImportResult.HttpStatusCode = 500;
			ImportResult.Error = TEXT("Import timed out while waiting for the game thread.");
		}
		else
		{
			ImportResult = *SharedResult;
		}
	}

	OnComplete(MakeJsonResponse(FUiTextureImportService::BuildResponseJson(ImportResult), ImportResult.HttpStatusCode));
	return true;
}

TUniquePtr<FHttpServerResponse> FUiTextureBridgeServer::MakeJsonResponse(const FString& InBody, int32 InStatusCode)
{
	TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(InBody, TEXT("application/json; charset=utf-8"));
	Response->Code = ToResponseCode(InStatusCode);
	Response->Headers.FindOrAdd(TEXT("Cache-Control")).Add(TEXT("no-store"));
	return Response;
}

int32 FUiTextureBridgeServer::ClampPort(int32 InPort)
{
	return InPort >= 1 && InPort <= 65535 ? InPort : -1;
}
