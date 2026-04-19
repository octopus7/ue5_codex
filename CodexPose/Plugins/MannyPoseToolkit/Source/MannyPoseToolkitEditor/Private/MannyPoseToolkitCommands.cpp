#include "MannyPoseToolkitCommands.h"

#define LOCTEXT_NAMESPACE "FMannyPoseToolkitCommands"

void FMannyPoseToolkitCommands::RegisterCommands()
{
    UI_COMMAND(OpenPluginWindow, "Manny Pose Toolkit", "Open the Manny Pose Toolkit window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
