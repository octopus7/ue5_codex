[CmdletBinding(SupportsShouldProcess = $true, ConfirmImpact = "High")]
param(
    [string]$ProjectRoot
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$scriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
if (-not $ProjectRoot) {
    $ProjectRoot = Split-Path -Parent $scriptRoot
}

$resolvedProjectRoot = (Resolve-Path $ProjectRoot).Path
$targets = @(
    (Join-Path $resolvedProjectRoot "Content\Characters"),
    (Join-Path $resolvedProjectRoot "Content\Input"),
    (Join-Path $resolvedProjectRoot "Content\LevelPrototyping")
)

Write-Host "Project root : $resolvedProjectRoot"

foreach ($target in $targets) {
    if (-not (Test-Path $target)) {
        Write-Host "Skipping missing path $target"
        continue
    }

    if ($PSCmdlet.ShouldProcess($target, "Remove shared TPS template content")) {
        Remove-Item $target -Recurse -Force
        Write-Host "Removed $target"
    }
}
