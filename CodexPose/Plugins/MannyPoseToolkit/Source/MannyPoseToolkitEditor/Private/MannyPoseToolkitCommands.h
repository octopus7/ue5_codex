#pragma once

#include "Framework/Commands/Commands.h"
#include "Styling/AppStyle.h"

class FMannyPoseToolkitCommands : public TCommands<FMannyPoseToolkitCommands>
{
public:
    FMannyPoseToolkitCommands()
        : TCommands<FMannyPoseToolkitCommands>(TEXT("MannyPoseToolkit"), NSLOCTEXT("Contexts", "MannyPoseToolkit", "Manny Pose Toolkit"), NAME_None, FAppStyle::GetAppStyleSetName())
    {
    }

    virtual void RegisterCommands() override;

public:
    TSharedPtr<FUICommandInfo> OpenPluginWindow;
};
