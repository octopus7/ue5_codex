#pragma once

#include "CoreMinimal.h"
#include "HttpRequestHandler.h"
#include "HttpResultCallback.h"
#include "HttpRouteHandle.h"
#include "Settings/UiTextureBridgeSettings.h"

class FUiTextureImportService;
class IHttpRouter;
struct FHttpServerResponse;
struct FHttpServerRequest;

class FUiTextureBridgeServer
{
public:
	FUiTextureBridgeServer();
	~FUiTextureBridgeServer();

	bool Start(FString& OutError);
	void Stop();
	bool IsRunning() const;
	FString GetBaseUrl() const;

private:
	bool ApplyListenerConfig(const FUiTextureBridgeSettingsSnapshot& InSettings, FString& OutError);
	bool CheckPortAvailability(const FString& InBindAddress, int32 InPort, FString& OutError) const;
	bool BindRoutes(FString& OutError);
	void UnbindRoutes();

	bool HandleHealthRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);
	bool HandleImportRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

	static TUniquePtr<FHttpServerResponse> MakeJsonResponse(const FString& InBody, int32 InStatusCode);
	static int32 ClampPort(int32 InPort);

	TSharedPtr<IHttpRouter> Router;
	FHttpRouteHandle HealthRouteHandle;
	FHttpRouteHandle ImportRouteHandle;
	TUniquePtr<FUiTextureImportService> ImportService;
	FUiTextureBridgeSettingsSnapshot ActiveSettings;
	bool bRunning = false;
};
