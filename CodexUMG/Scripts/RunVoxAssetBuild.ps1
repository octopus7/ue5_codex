[CmdletBinding()]
param(
	[string]$ProjectPath,
	[string]$ManifestPath,
	[switch]$GenerateSamples,
	[switch]$VerboseBuild,
	[switch]$NoOverwrite
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

$repoRoot = Split-Path -Parent $PSScriptRoot
$projectFullPath = [System.IO.Path]::GetFullPath($(if ([string]::IsNullOrWhiteSpace($ProjectPath)) { Join-Path $repoRoot 'CodexUMG.uproject' } else { $ProjectPath }))
$manifestFullPath = [System.IO.Path]::GetFullPath($(if ([string]::IsNullOrWhiteSpace($ManifestPath)) { Join-Path $repoRoot 'SourceArt/Vox/VoxAssetManifest.json' } else { $ManifestPath }))
$sampleRoot = Join-Path $repoRoot 'SourceArt/Vox/Sources'
$sampleScript = Join-Path $PSScriptRoot 'GenerateSampleVoxSources.ps1'
$editorCmd = Join-Path ${env:ProgramFiles} 'Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe'

if (-not (Test-Path $editorCmd)) {
	throw "UnrealEditor-Cmd.exe not found at '$editorCmd'."
}

$runningEditor = Get-CimInstance Win32_Process |
	Where-Object {
		$_.Name -eq 'UnrealEditor.exe' -and
		$_.CommandLine -and
		$_.CommandLine.Contains($projectFullPath)
	}

if ($runningEditor) {
	Write-Error 'CodexUMG editor session is running, so VOX asset build will stop here. Close the editor and run the script again.'
	exit 3
}

if ($GenerateSamples -or -not (Test-Path (Join-Path $sampleRoot 'SM_Vox_WhiteChicken.vox'))) {
	& $sampleScript -OutputRoot $sampleRoot
}

$commandArgs = @(
	$projectFullPath,
	'-run=CodexVoxAssetBuild',
	"-Manifest=$manifestFullPath",
	'-unattended',
	'-nop4',
	'-nosplash'
)

if ($VerboseBuild) {
	$commandArgs += '-Verbose'
}

if ($NoOverwrite) {
	$commandArgs += '-NoOverwrite'
}

& $editorCmd @commandArgs
exit $LASTEXITCODE
