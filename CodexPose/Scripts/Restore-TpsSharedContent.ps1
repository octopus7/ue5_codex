[CmdletBinding()]
param(
    [string]$ProjectRoot,
    [string]$EngineRoot,
    [switch]$Force
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Resolve-EngineRoot {
    param(
        [string]$ExplicitEngineRoot
    )

    $candidates = @()

    if ($ExplicitEngineRoot) {
        $candidates += $ExplicitEngineRoot
    }

    foreach ($envVar in @("UE_ENGINE_ROOT", "UE5_ROOT", "UNREAL_ENGINE_ROOT")) {
        $value = [Environment]::GetEnvironmentVariable($envVar)
        if ($value) {
            $candidates += $value
        }
    }

    foreach ($basePath in @(
        "C:\Program Files\Epic Games",
        "D:\Program Files\Epic Games",
        "C:\Epic Games",
        "D:\Epic Games"
    )) {
        if (Test-Path $basePath) {
            $candidates += Get-ChildItem $basePath -Directory -Filter "UE_*" |
                Sort-Object Name -Descending |
                Select-Object -ExpandProperty FullName
        }
    }

    foreach ($candidate in $candidates | Select-Object -Unique) {
        if (Test-Path (Join-Path $candidate "Templates")) {
            return (Resolve-Path $candidate).Path
        }
    }

    throw "Unable to find an Unreal Engine installation. Pass -EngineRoot or set UE_ENGINE_ROOT."
}

function Copy-TemplateFolder {
    param(
        [string]$Name,
        [string]$SourcePath,
        [string]$DestinationPath,
        [switch]$Overwrite
    )

    if (-not (Test-Path $SourcePath)) {
        throw "Missing template source for ${Name}: $SourcePath"
    }

    $destinationHasFiles = (Test-Path $DestinationPath) -and
        (Get-ChildItem $DestinationPath -Recurse -File -ErrorAction SilentlyContinue | Select-Object -First 1)

    if ($destinationHasFiles -and -not $Overwrite) {
        Write-Host "Skipping $Name because it already exists at $DestinationPath"
        return
    }

    New-Item -ItemType Directory -Force -Path $DestinationPath | Out-Null

    $robocopyArgs = @(
        $SourcePath,
        $DestinationPath,
        "/E",
        "/R:2",
        "/W:1",
        "/NFL",
        "/NDL",
        "/NJH",
        "/NJS",
        "/NP"
    )

    & robocopy @robocopyArgs | Out-Null
    $robocopyExitCode = $LASTEXITCODE

    if ($robocopyExitCode -gt 7) {
        throw "robocopy failed for ${Name} with exit code $robocopyExitCode"
    }

    Write-Host "Restored $Name from $SourcePath"
}

$scriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
if (-not $ProjectRoot) {
    $ProjectRoot = Split-Path -Parent $scriptRoot
}

$resolvedProjectRoot = (Resolve-Path $ProjectRoot).Path
$resolvedEngineRoot = Resolve-EngineRoot -ExplicitEngineRoot $EngineRoot

$mappings = @(
    @{
        Name = "Characters"
        SourcePath = Join-Path $resolvedEngineRoot "Templates\TemplateResources\High\Characters\Content"
        DestinationPath = Join-Path $resolvedProjectRoot "Content\Characters"
    },
    @{
        Name = "Input"
        SourcePath = Join-Path $resolvedEngineRoot "Templates\TemplateResources\High\Input\Content"
        DestinationPath = Join-Path $resolvedProjectRoot "Content\Input"
    },
    @{
        Name = "LevelPrototyping"
        SourcePath = Join-Path $resolvedEngineRoot "Templates\TemplateResources\High\LevelPrototyping\Content"
        DestinationPath = Join-Path $resolvedProjectRoot "Content\LevelPrototyping"
    }
)

Write-Host "Project root : $resolvedProjectRoot"
Write-Host "Engine root  : $resolvedEngineRoot"

foreach ($mapping in $mappings) {
    Copy-TemplateFolder `
        -Name $mapping.Name `
        -SourcePath $mapping.SourcePath `
        -DestinationPath $mapping.DestinationPath `
        -Overwrite:$Force
}
