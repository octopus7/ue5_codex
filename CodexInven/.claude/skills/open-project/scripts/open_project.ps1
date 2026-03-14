param()

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\\..\\..\\..")).Path
$projectFile = Get-ChildItem -Path $repoRoot -Filter *.uproject -File | Sort-Object Name | Select-Object -First 1

if (-not $projectFile) {
    throw "No .uproject file was found in $repoRoot."
}

Start-Process -FilePath $projectFile.FullName -WorkingDirectory $repoRoot
Write-Output "Opened $($projectFile.FullName)"
