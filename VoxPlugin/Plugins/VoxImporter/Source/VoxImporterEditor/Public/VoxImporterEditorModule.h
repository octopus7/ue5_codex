#pragma once

#include "Modules/ModuleInterface.h"

class FVoxImporterEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterMenus();
	void GenerateSmoothReconstructions(const TArray<TWeakObjectPtr<class UStaticMesh>>& StaticMeshes) const;
	void BakeVertexColorsToTextures(const TArray<TWeakObjectPtr<class UStaticMesh>>& StaticMeshes) const;
};
