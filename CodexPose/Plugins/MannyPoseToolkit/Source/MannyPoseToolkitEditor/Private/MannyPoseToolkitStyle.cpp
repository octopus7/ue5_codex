#include "MannyPoseToolkitStyle.h"

#include "Framework/Application/SlateApplication.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateTypes.h"
#include "Brushes/SlateImageBrush.h"

TSharedPtr<FSlateStyleSet> FMannyPoseToolkitStyle::StyleInstance = nullptr;

void FMannyPoseToolkitStyle::Initialize()
{
    if (!StyleInstance.IsValid())
    {
        StyleInstance = Create();
        FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
    }
}

void FMannyPoseToolkitStyle::Shutdown()
{
    if (StyleInstance.IsValid())
    {
        FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
        ensure(StyleInstance.IsUnique());
        StyleInstance.Reset();
    }
}

FName FMannyPoseToolkitStyle::GetStyleSetName()
{
    static FName StyleSetName(TEXT("MannyPoseToolkitStyle"));
    return StyleSetName;
}

TSharedRef<FSlateStyleSet> FMannyPoseToolkitStyle::Create()
{
    TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet("MannyPoseToolkitStyle"));
    const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("MannyPoseToolkit"));
    Style->SetContentRoot(Plugin->GetBaseDir() / TEXT("Resources"));
    return Style;
}

void FMannyPoseToolkitStyle::ReloadTextures()
{
    FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
}

const ISlateStyle& FMannyPoseToolkitStyle::Get()
{
    return *StyleInstance;
}
