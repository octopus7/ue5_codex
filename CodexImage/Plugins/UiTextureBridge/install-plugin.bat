@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "PLUGIN_NAME=UiTextureBridge"
set "SCRIPT_DIR=%~dp0"
set "SOURCE_PLUGIN_DIR=%SCRIPT_DIR:~0,-1%"

if "%~1"=="" (
    call :PrintUsage
    exit /b 1
)

call :ResolveUProjectPath "%~1"
if errorlevel 1 exit /b 1

for %%I in ("%RESOLVED_UPROJECT%") do (
    set "TARGET_UPROJECT=%%~fI"
    set "TARGET_PROJECT_DIR=%%~dpI"
)

set "TARGET_PLUGINS_DIR=%TARGET_PROJECT_DIR%Plugins"
set "TARGET_PLUGIN_DIR=%TARGET_PLUGINS_DIR%\%PLUGIN_NAME%"

if not exist "%TARGET_PLUGINS_DIR%" (
    mkdir "%TARGET_PLUGINS_DIR%" || (
        echo Failed to create Plugins directory: "%TARGET_PLUGINS_DIR%"
        exit /b 1
    )
)

if exist "%TARGET_PLUGIN_DIR%" (
    call :BuildBackupPath "%TARGET_PLUGIN_DIR%"
    echo Existing plugin found. Backing up to:
    echo   "!BACKUP_PLUGIN_DIR!"
    move "%TARGET_PLUGIN_DIR%" "!BACKUP_PLUGIN_DIR!" >nul || (
        echo Failed to back up existing plugin folder.
        exit /b 1
    )
)

echo Installing "%PLUGIN_NAME%" into:
echo   "%TARGET_PLUGIN_DIR%"

robocopy "%SOURCE_PLUGIN_DIR%" "%TARGET_PLUGIN_DIR%" /E /R:1 /W:1 /NFL /NDL /NJH /NJS /NP /XD Binaries Intermediate Saved .git .vs >nul
set "ROBOCOPY_EXIT=%ERRORLEVEL%"
if %ROBOCOPY_EXIT% GEQ 8 (
    echo robocopy failed with exit code %ROBOCOPY_EXIT%.
    exit /b %ROBOCOPY_EXIT%
)

cscript //nologo "%SCRIPT_DIR%install-plugin.js" "%TARGET_UPROJECT%" "%PLUGIN_NAME%"
if errorlevel 1 (
    echo Failed to enable "%PLUGIN_NAME%" inside "%TARGET_UPROJECT%".
    exit /b 1
)

echo.
echo Installed "%PLUGIN_NAME%" successfully.
echo Next step: regenerate project files or build the Editor target for the target project.
exit /b 0

:ResolveUProjectPath
set "INPUT_PATH=%~1"
if not exist "%INPUT_PATH%" (
    echo Path not found: "%INPUT_PATH%"
    exit /b 1
)

for %%I in ("%INPUT_PATH%") do (
    if /I "%%~xI"==".uproject" (
        set "RESOLVED_UPROJECT=%%~fI"
        exit /b 0
    )
)

if not exist "%INPUT_PATH%\" (
    echo Expected a .uproject file or a project directory:
    echo   "%INPUT_PATH%"
    exit /b 1
)

set "UPROJECT_COUNT=0"
set "LAST_UPROJECT="
for %%I in ("%INPUT_PATH%\*.uproject") do (
    set /a UPROJECT_COUNT+=1
    set "LAST_UPROJECT=%%~fI"
)

if !UPROJECT_COUNT! EQU 1 (
    set "RESOLVED_UPROJECT=!LAST_UPROJECT!"
    exit /b 0
)

if !UPROJECT_COUNT! EQU 0 (
    echo No .uproject file found in:
    echo   "%INPUT_PATH%"
    exit /b 1
)

echo Multiple .uproject files found in:
echo   "%INPUT_PATH%"
echo Pass the exact .uproject path instead.
exit /b 1

:BuildBackupPath
set "BASE_PLUGIN_DIR=%~1"
set "BACKUP_PLUGIN_DIR=%BASE_PLUGIN_DIR%.backup"
if not exist "%BACKUP_PLUGIN_DIR%" exit /b 0

set /a BACKUP_INDEX=1
:BackupLoop
set "BACKUP_PLUGIN_DIR=%BASE_PLUGIN_DIR%.backup%BACKUP_INDEX%"
if exist "%BACKUP_PLUGIN_DIR%" (
    set /a BACKUP_INDEX+=1
    goto :BackupLoop
)
exit /b 0

:PrintUsage
echo Usage:
echo   install-plugin.bat ^<TargetProject.uproject^>
echo   install-plugin.bat ^<TargetProjectDirectory^>
echo.
echo Example:
echo   install-plugin.bat "D:\UnrealProjects\MyGame\MyGame.uproject"
exit /b 0
