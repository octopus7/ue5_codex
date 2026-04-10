[CmdletBinding()]
param(
	[string]$ProjectPath,
	[string]$ManifestPath,
	[string]$EngineRoot,
	[string]$EditorCmdPath,
	[switch]$GenerateSamples,
	[switch]$NoOverwrite,
	[switch]$VerboseBuild
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$projectRoot = Split-Path -Parent $PSScriptRoot
if (-not $ProjectPath) {
	$ProjectPath = Join-Path $projectRoot 'CodexUMG.uproject'
}

$ProjectPath = [System.IO.Path]::GetFullPath($ProjectPath)

if (-not $ManifestPath) {
	$ManifestPath = Join-Path $projectRoot 'SourceArt\Vox\VoxAssetManifest.json'
}

$ManifestPath = [System.IO.Path]::GetFullPath($ManifestPath)

if ($GenerateSamples) {
	& (Join-Path $PSScriptRoot 'GenerateSampleVoxSources.ps1')
}

$editorProcesses = Get-CimInstance Win32_Process -Filter "Name = 'UnrealEditor.exe'" |
	Where-Object {
		$_.CommandLine -and $_.CommandLine -match [regex]::Escape($ProjectPath)
	}

if ($editorProcesses) {
	throw 'An Unreal Editor session for this project is running. Stop the editor and rerun the VOX asset build.'
}

if (-not $EngineRoot -and -not $EditorCmdPath) {
	$uproject = Get-Content -Raw $ProjectPath | ConvertFrom-Json
	if ($uproject.EngineAssociation) {
		$EngineRoot = Join-Path 'C:\Program Files\Epic Games' ("UE_{0}" -f $uproject.EngineAssociation)
	}
}

if (-not $EditorCmdPath) {
	if (-not $EngineRoot) {
		throw 'Unable to determine EngineRoot. Pass -EngineRoot or -EditorCmdPath.'
	}

	$EditorCmdPath = Join-Path $EngineRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
}

$EditorCmdPath = [System.IO.Path]::GetFullPath($EditorCmdPath)
if (-not (Test-Path $EditorCmdPath)) {
	throw "UnrealEditor-Cmd.exe not found at '$EditorCmdPath'."
}

$arguments = @(
	$ProjectPath
	'-run=CodexUMGBootstrapEditor.CodexVoxAssetBuildCommandlet'
	("-Manifest=$ManifestPath")
	'-unattended'
	'-nop4'
	'-nosplash'
)

if ($NoOverwrite) {
	$arguments += '-NoOverwrite'
}

if ($VerboseBuild) {
	$arguments += '-Verbose'
}

& $EditorCmdPath @arguments
exit $LASTEXITCODE
