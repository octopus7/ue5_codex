[CmdletBinding()]
param(
	[string]$OutputRoot,
	[switch]$Clean,
	[switch]$GeneratePreviews,
	[string]$PythonExe = 'python'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$projectRoot = Split-Path -Parent $PSScriptRoot
if (-not $OutputRoot) {
	$OutputRoot = Join-Path $projectRoot 'SourceArt\Vox'
}

$resolution = 32
$sourcesRoot = Join-Path $OutputRoot 'Sources'
$manifestPath = Join-Path $OutputRoot 'VoxAssetManifest.json'
$previewsRoot = Join-Path $OutputRoot 'Previews'

function New-VoxelGrid {
	return New-Object 'byte[,,]' $resolution, $resolution, $resolution
}

function Set-Voxel {
	param(
		[byte[,,]]$Grid,
		[int]$X,
		[int]$Y,
		[int]$Z,
		[byte]$ColorIndex
	)

	if ($X -lt 0 -or $X -ge $resolution -or $Y -lt 0 -or $Y -ge $resolution -or $Z -lt 0 -or $Z -ge $resolution) {
		return
	}

	$Grid[$X, $Y, $Z] = $ColorIndex
}

function Add-Box {
	param(
		[byte[,,]]$Grid,
		[int]$MinX,
		[int]$MinY,
		[int]$MinZ,
		[int]$MaxX,
		[int]$MaxY,
		[int]$MaxZ,
		[byte]$ColorIndex
	)

	for ($x = $MinX; $x -le $MaxX; $x++) {
		for ($y = $MinY; $y -le $MaxY; $y++) {
			for ($z = $MinZ; $z -le $MaxZ; $z++) {
				Set-Voxel -Grid $Grid -X $x -Y $y -Z $z -ColorIndex $ColorIndex
			}
		}
	}
}

function Add-Ellipsoid {
	param(
		[byte[,,]]$Grid,
		[double]$CenterX,
		[double]$CenterY,
		[double]$CenterZ,
		[double]$RadiusX,
		[double]$RadiusY,
		[double]$RadiusZ,
		[byte]$ColorIndex
	)

	$minX = [Math]::Max(0, [Math]::Floor($CenterX - $RadiusX))
	$maxX = [Math]::Min($resolution - 1, [Math]::Ceiling($CenterX + $RadiusX))
	$minY = [Math]::Max(0, [Math]::Floor($CenterY - $RadiusY))
	$maxY = [Math]::Min($resolution - 1, [Math]::Ceiling($CenterY + $RadiusY))
	$minZ = [Math]::Max(0, [Math]::Floor($CenterZ - $RadiusZ))
	$maxZ = [Math]::Min($resolution - 1, [Math]::Ceiling($CenterZ + $RadiusZ))

	for ($x = $minX; $x -le $maxX; $x++) {
		for ($y = $minY; $y -le $maxY; $y++) {
			for ($z = $minZ; $z -le $maxZ; $z++) {
				$dx = ($x - $CenterX) / $RadiusX
				$dy = ($y - $CenterY) / $RadiusY
				$dz = ($z - $CenterZ) / $RadiusZ
				if (($dx * $dx) + ($dy * $dy) + ($dz * $dz) -le 1.0) {
					Set-Voxel -Grid $Grid -X $x -Y $y -Z $z -ColorIndex $ColorIndex
				}
			}
		}
	}
}

function Add-CylinderY {
	param(
		[byte[,,]]$Grid,
		[double]$CenterX,
		[double]$CenterZ,
		[double]$Radius,
		[int]$MinY,
		[int]$MaxY,
		[byte]$ColorIndex
	)

	$minX = [Math]::Max(0, [Math]::Floor($CenterX - $Radius))
	$maxX = [Math]::Min($resolution - 1, [Math]::Ceiling($CenterX + $Radius))
	$minZ = [Math]::Max(0, [Math]::Floor($CenterZ - $Radius))
	$maxZ = [Math]::Min($resolution - 1, [Math]::Ceiling($CenterZ + $Radius))

	for ($x = $minX; $x -le $maxX; $x++) {
		for ($z = $minZ; $z -le $maxZ; $z++) {
			$dx = ($x - $CenterX) / $Radius
			$dz = ($z - $CenterZ) / $Radius
			if (($dx * $dx) + ($dz * $dz) -le 1.0) {
				for ($y = $MinY; $y -le $MaxY; $y++) {
					Set-Voxel -Grid $Grid -X $x -Y $y -Z $z -ColorIndex $ColorIndex
				}
			}
		}
	}
}

function Add-Flower {
	param(
		[byte[,,]]$Grid,
		[int]$StemHeight,
		[byte]$StemColor,
		[byte]$PetalColor,
		[byte]$CenterColor,
		[int]$PetalRadius,
		[int]$CenterX = 16,
		[int]$CenterZ = 16
	)

	Add-Box -Grid $Grid -MinX $CenterX -MinY 0 -MinZ $CenterZ -MaxX $CenterX -MaxY $StemHeight -MaxZ $CenterZ -ColorIndex $StemColor
	Add-Box -Grid $Grid -MinX ($CenterX - 2) -MinY ([Math]::Floor($StemHeight * 0.5)) -MinZ $CenterZ -MaxX ($CenterX - 1) -MaxY ([Math]::Floor($StemHeight * 0.5) + 1) -MaxZ ($CenterZ + 1) -ColorIndex $StemColor
	Add-Ellipsoid -Grid $Grid -CenterX $CenterX -CenterY ($StemHeight + 1) -CenterZ $CenterZ -RadiusX 1.5 -RadiusY 1.5 -RadiusZ 1.5 -ColorIndex $CenterColor

	$petalOffsets = @(
		@(-$PetalRadius, 0),
		@($PetalRadius, 0),
		@(0, -$PetalRadius),
		@(0, $PetalRadius),
		@(-1, -1),
		@(-1, 1),
		@(1, -1),
		@(1, 1)
	)

	foreach ($offset in $petalOffsets) {
		Add-Ellipsoid -Grid $Grid -CenterX ($CenterX + $offset[0]) -CenterY ($StemHeight + 1) -CenterZ ($CenterZ + $offset[1]) -RadiusX 1.5 -RadiusY 1.5 -RadiusZ 1.5 -ColorIndex $PetalColor
	}
}

function Build-Chicken {
	param(
		[byte[,,]]$Grid,
		[byte]$BodyColor,
		[byte]$BeakColor,
		[byte]$CombColor,
		[byte]$LegColor
	)

	Add-Ellipsoid -Grid $Grid -CenterX 16 -CenterY 9 -CenterZ 15 -RadiusX 5.5 -RadiusY 5.5 -RadiusZ 4.5 -ColorIndex $BodyColor
	Add-Ellipsoid -Grid $Grid -CenterX 16 -CenterY 15 -CenterZ 19 -RadiusX 3.0 -RadiusY 3.0 -RadiusZ 3.0 -ColorIndex $BodyColor
	Add-Box -Grid $Grid -MinX 15 -MinY 14 -MinZ 22 -MaxX 16 -MaxY 15 -MaxZ 24 -ColorIndex $BeakColor
	Add-Box -Grid $Grid -MinX 14 -MinY 18 -MinZ 18 -MaxX 17 -MaxY 18 -MaxZ 19 -ColorIndex $CombColor
	Add-Box -Grid $Grid -MinX 14 -MinY 0 -MinZ 14 -MaxX 14 -MaxY 5 -MaxZ 14 -ColorIndex $LegColor
	Add-Box -Grid $Grid -MinX 18 -MinY 0 -MinZ 14 -MaxX 18 -MaxY 5 -MaxZ 14 -ColorIndex $LegColor
	Add-Box -Grid $Grid -MinX 12 -MinY 9 -MinZ 11 -MaxX 13 -MaxY 11 -MaxZ 13 -ColorIndex $BodyColor
}

function Build-Chick {
	param(
		[byte[,,]]$Grid,
		[byte]$BodyColor,
		[byte]$BeakColor,
		[byte]$LegColor
	)

	Add-Ellipsoid -Grid $Grid -CenterX 16 -CenterY 7 -CenterZ 15 -RadiusX 4.5 -RadiusY 4.5 -RadiusZ 4.0 -ColorIndex $BodyColor
	Add-Ellipsoid -Grid $Grid -CenterX 16 -CenterY 11 -CenterZ 19 -RadiusX 2.5 -RadiusY 2.5 -RadiusZ 2.5 -ColorIndex $BodyColor
	Add-Box -Grid $Grid -MinX 15 -MinY 10 -MinZ 21 -MaxX 16 -MaxY 11 -MaxZ 22 -ColorIndex $BeakColor
	Add-Box -Grid $Grid -MinX 14 -MinY 0 -MinZ 14 -MaxX 14 -MaxY 3 -MaxZ 14 -ColorIndex $LegColor
	Add-Box -Grid $Grid -MinX 18 -MinY 0 -MinZ 14 -MaxX 18 -MaxY 3 -MaxZ 14 -ColorIndex $LegColor
}

function Build-Pig {
	param(
		[byte[,,]]$Grid,
		[byte]$BodyColor,
		[byte]$AccentColor
	)

	Add-Ellipsoid -Grid $Grid -CenterX 16 -CenterY 10 -CenterZ 15 -RadiusX 7.0 -RadiusY 5.0 -RadiusZ 5.0 -ColorIndex $BodyColor
	Add-Box -Grid $Grid -MinX 12 -MinY 8 -MinZ 19 -MaxX 19 -MaxY 12 -MaxZ 24 -ColorIndex $BodyColor
	Add-Box -Grid $Grid -MinX 14 -MinY 9 -MinZ 24 -MaxX 17 -MaxY 11 -MaxZ 26 -ColorIndex $AccentColor
	Add-Box -Grid $Grid -MinX 11 -MinY 13 -MinZ 20 -MaxX 12 -MaxY 15 -MaxZ 21 -ColorIndex $BodyColor
	Add-Box -Grid $Grid -MinX 19 -MinY 13 -MinZ 20 -MaxX 20 -MaxY 15 -MaxZ 21 -ColorIndex $BodyColor
	Add-Box -Grid $Grid -MinX 11 -MinY 0 -MinZ 12 -MaxX 12 -MaxY 5 -MaxZ 13 -ColorIndex $AccentColor
	Add-Box -Grid $Grid -MinX 19 -MinY 0 -MinZ 12 -MaxX 20 -MaxY 5 -MaxZ 13 -ColorIndex $AccentColor
	Add-Box -Grid $Grid -MinX 11 -MinY 0 -MinZ 17 -MaxX 12 -MaxY 5 -MaxZ 18 -ColorIndex $AccentColor
	Add-Box -Grid $Grid -MinX 19 -MinY 0 -MinZ 17 -MaxX 20 -MaxY 5 -MaxZ 18 -ColorIndex $AccentColor
}

function Build-Apple {
	param(
		[byte[,,]]$Grid,
		[byte]$FruitColor,
		[byte]$StemColor,
		[byte]$LeafColor
	)

	Add-Ellipsoid -Grid $Grid -CenterX 16 -CenterY 9 -CenterZ 16 -RadiusX 6.0 -RadiusY 7.0 -RadiusZ 6.0 -ColorIndex $FruitColor
	Add-Box -Grid $Grid -MinX 15 -MinY 16 -MinZ 15 -MaxX 16 -MaxY 20 -MaxZ 16 -ColorIndex $StemColor
	Add-Ellipsoid -Grid $Grid -CenterX 19 -CenterY 18 -CenterZ 17 -RadiusX 3.0 -RadiusY 1.5 -RadiusZ 2.5 -ColorIndex $LeafColor
}

function Build-MilkBottle {
	param(
		[byte[,,]]$Grid,
		[byte]$BottleColor,
		[byte]$CapColor
	)

	Add-CylinderY -Grid $Grid -CenterX 16 -CenterZ 16 -Radius 4.5 -MinY 0 -MaxY 20 -ColorIndex $BottleColor
	Add-CylinderY -Grid $Grid -CenterX 16 -CenterZ 16 -Radius 2.5 -MinY 21 -MaxY 26 -ColorIndex $BottleColor
	Add-Box -Grid $Grid -MinX 14 -MinY 27 -MinZ 14 -MaxX 17 -MaxY 30 -MaxZ 17 -ColorIndex $CapColor
}

function Build-Strawberry {
	param(
		[byte[,,]]$Grid,
		[byte]$BodyColor,
		[byte]$LeafColor
	)

	Add-Ellipsoid -Grid $Grid -CenterX 16 -CenterY 6 -CenterZ 16 -RadiusX 4.5 -RadiusY 5.0 -RadiusZ 4.5 -ColorIndex $BodyColor
	Add-Ellipsoid -Grid $Grid -CenterX 16 -CenterY 11 -CenterZ 16 -RadiusX 5.5 -RadiusY 6.5 -RadiusZ 5.5 -ColorIndex $BodyColor
	Add-Box -Grid $Grid -MinX 12 -MinY 16 -MinZ 15 -MaxX 19 -MaxY 17 -MaxZ 16 -ColorIndex $LeafColor
	Add-Box -Grid $Grid -MinX 15 -MinY 16 -MinZ 12 -MaxX 16 -MaxY 17 -MaxZ 19 -ColorIndex $LeafColor
	Add-Box -Grid $Grid -MinX 15 -MinY 18 -MinZ 15 -MaxX 16 -MaxY 20 -MaxZ 16 -ColorIndex $LeafColor
}

function Build-Banana {
	param(
		[byte[,,]]$Grid,
		[byte]$BodyColor,
		[byte]$TipColor
	)

	$segments = @(
		@(10, 8, 11),
		@(13, 9, 13),
		@(16, 10, 16),
		@(19, 11, 18),
		@(22, 12, 19)
	)

	foreach ($segment in $segments) {
		Add-Ellipsoid -Grid $Grid -CenterX $segment[0] -CenterY $segment[1] -CenterZ $segment[2] -RadiusX 3.0 -RadiusY 2.0 -RadiusZ 2.0 -ColorIndex $BodyColor
	}

	Add-Box -Grid $Grid -MinX 8 -MinY 7 -MinZ 10 -MaxX 9 -MaxY 8 -MaxZ 11 -ColorIndex $TipColor
	Add-Box -Grid $Grid -MinX 23 -MinY 12 -MinZ 19 -MaxX 24 -MaxY 13 -MaxZ 20 -ColorIndex $TipColor
}

function Build-GrapeCluster {
	param(
		[byte[,,]]$Grid,
		[byte]$FruitColor,
		[byte]$StemColor
	)

	$grapes = @(
		@(16, 15, 14),
		@(13, 13, 16),
		@(19, 13, 16),
		@(16, 11, 18),
		@(13, 9, 14),
		@(19, 9, 14),
		@(16, 7, 16)
	)

	foreach ($grape in $grapes) {
		Add-Ellipsoid -Grid $Grid -CenterX $grape[0] -CenterY $grape[1] -CenterZ $grape[2] -RadiusX 2.5 -RadiusY 2.5 -RadiusZ 2.5 -ColorIndex $FruitColor
	}

	Add-Box -Grid $Grid -MinX 15 -MinY 18 -MinZ 13 -MaxX 16 -MaxY 22 -MaxZ 14 -ColorIndex $StemColor
}

function Build-RawMeat {
	param(
		[byte[,,]]$Grid,
		[byte]$MeatColor,
		[byte]$FatColor
	)

	Add-Ellipsoid -Grid $Grid -CenterX 16 -CenterY 7 -CenterZ 16 -RadiusX 7.0 -RadiusY 4.5 -RadiusZ 5.5 -ColorIndex $MeatColor
	Add-Box -Grid $Grid -MinX 10 -MinY 6 -MinZ 11 -MaxX 13 -MaxY 8 -MaxZ 19 -ColorIndex $FatColor
}

function Build-BoneInRoast {
	param(
		[byte[,,]]$Grid,
		[byte]$RoastColor,
		[byte]$BoneColor
	)

	Add-Ellipsoid -Grid $Grid -CenterX 16 -CenterY 8 -CenterZ 16 -RadiusX 7.0 -RadiusY 5.0 -RadiusZ 5.0 -ColorIndex $RoastColor
	Add-Box -Grid $Grid -MinX 9 -MinY 7 -MinZ 15 -MaxX 12 -MaxY 9 -MaxZ 16 -ColorIndex $BoneColor
	Add-Box -Grid $Grid -MinX 20 -MinY 7 -MinZ 15 -MaxX 23 -MaxY 9 -MaxZ 16 -ColorIndex $BoneColor
}

function Build-Fence {
	param(
		[byte[,,]]$Grid,
		[byte]$WoodColor
	)

	Add-Box -Grid $Grid -MinX 5 -MinY 0 -MinZ 14 -MaxX 7 -MaxY 20 -MaxZ 17 -ColorIndex $WoodColor
	Add-Box -Grid $Grid -MinX 24 -MinY 0 -MinZ 14 -MaxX 26 -MaxY 20 -MaxZ 17 -ColorIndex $WoodColor
	Add-Box -Grid $Grid -MinX 7 -MinY 6 -MinZ 15 -MaxX 24 -MaxY 8 -MaxZ 16 -ColorIndex $WoodColor
	Add-Box -Grid $Grid -MinX 7 -MinY 12 -MinZ 15 -MaxX 24 -MaxY 14 -MaxZ 16 -ColorIndex $WoodColor
}

function Build-Bush {
	param(
		[byte[,,]]$Grid,
		[byte]$LeafColor,
		[byte]$WoodColor
	)

	Add-Ellipsoid -Grid $Grid -CenterX 16 -CenterY 8 -CenterZ 16 -RadiusX 7.5 -RadiusY 6.0 -RadiusZ 7.5 -ColorIndex $LeafColor
	Add-Box -Grid $Grid -MinX 15 -MinY 0 -MinZ 15 -MaxX 16 -MaxY 3 -MaxZ 16 -ColorIndex $WoodColor
}

function Build-Grass {
	param(
		[byte[,,]]$Grid,
		[byte]$GrassColor
	)

	Add-Box -Grid $Grid -MinX 14 -MinY 0 -MinZ 15 -MaxX 14 -MaxY 7 -MaxZ 15 -ColorIndex $GrassColor
	Add-Box -Grid $Grid -MinX 16 -MinY 0 -MinZ 16 -MaxX 16 -MaxY 8 -MaxZ 16 -ColorIndex $GrassColor
	Add-Box -Grid $Grid -MinX 18 -MinY 0 -MinZ 14 -MaxX 18 -MaxY 6 -MaxZ 14 -ColorIndex $GrassColor
	Add-Box -Grid $Grid -MinX 15 -MinY 0 -MinZ 18 -MaxX 15 -MaxY 5 -MaxZ 18 -ColorIndex $GrassColor
}

function Build-Sunflower {
	param(
		[byte[,,]]$Grid,
		[byte]$StemColor,
		[byte]$PetalColor,
		[byte]$CenterColor
	)

	Add-Flower -Grid $Grid -StemHeight 22 -StemColor $StemColor -PetalColor $PetalColor -CenterColor $CenterColor -PetalRadius 3
}

function Build-PebbleGroundTile {
	param(
		[byte[,,]]$Grid,
		[byte]$BaseColor,
		[byte]$PebbleColor
	)

	Add-Box -Grid $Grid -MinX 0 -MinY 0 -MinZ 0 -MaxX 31 -MaxY 1 -MaxZ 31 -ColorIndex $BaseColor
	Add-Ellipsoid -Grid $Grid -CenterX 9 -CenterY 2 -CenterZ 10 -RadiusX 4.0 -RadiusY 1.5 -RadiusZ 3.5 -ColorIndex $PebbleColor
	Add-Ellipsoid -Grid $Grid -CenterX 18 -CenterY 2 -CenterZ 12 -RadiusX 5.0 -RadiusY 1.5 -RadiusZ 4.0 -ColorIndex $PebbleColor
	Add-Ellipsoid -Grid $Grid -CenterX 13 -CenterY 2 -CenterZ 21 -RadiusX 4.0 -RadiusY 1.5 -RadiusZ 4.0 -ColorIndex $PebbleColor
	Add-Ellipsoid -Grid $Grid -CenterX 23 -CenterY 2 -CenterZ 23 -RadiusX 4.5 -RadiusY 1.5 -RadiusZ 3.5 -ColorIndex $PebbleColor
}

function Build-WaterLily {
	param(
		[byte[,,]]$Grid,
		[byte]$PadColor,
		[byte]$BlossomColor
	)

	Add-Ellipsoid -Grid $Grid -CenterX 14 -CenterY 1 -CenterZ 16 -RadiusX 8.0 -RadiusY 1.0 -RadiusZ 7.0 -ColorIndex $PadColor
	Add-Ellipsoid -Grid $Grid -CenterX 19 -CenterY 1 -CenterZ 14 -RadiusX 6.0 -RadiusY 1.0 -RadiusZ 6.0 -ColorIndex $PadColor
	Add-Flower -Grid $Grid -StemHeight 3 -StemColor $PadColor -PetalColor $BlossomColor -CenterColor $BlossomColor -PetalRadius 2 -CenterX 16 -CenterZ 16
}

function Build-RainbowDiagnostic {
	param(
		[byte[,,]]$Grid,
		[byte[]]$StripeColors
	)

	$startX = 2
	$stripeWidth = 4
	$minY = 0
	$maxY = 23
	$minZ = 10
	$maxZ = 17

	for ($stripeIndex = 0; $stripeIndex -lt $StripeColors.Length; $stripeIndex++) {
		$minX = $startX + ($stripeIndex * $stripeWidth)
		$maxX = $minX + $stripeWidth - 1
		Add-Box -Grid $Grid -MinX $minX -MinY $minY -MinZ $minZ -MaxX $maxX -MaxY $maxY -MaxZ $maxZ -ColorIndex $StripeColors[$stripeIndex]
	}
}

function New-ChunkBytes {
	param(
		[string]$Id,
		[byte[]]$Content,
		[byte[]]$Children
	)

	if (-not $Content) {
		$Content = [byte[]]::new(0)
	}
	if (-not $Children) {
		$Children = [byte[]]::new(0)
	}

	$stream = New-Object System.IO.MemoryStream
	$writer = New-Object System.IO.BinaryWriter($stream)
	$idBytes = [System.Text.Encoding]::ASCII.GetBytes($Id)
	$stream.Write($idBytes, 0, $idBytes.Length)
	$writer.Write([int]$Content.Length)
	$writer.Write([int]$Children.Length)
	if ($Content.Length -gt 0) {
		$stream.Write($Content, 0, $Content.Length)
	}
	if ($Children.Length -gt 0) {
		$stream.Write($Children, 0, $Children.Length)
	}
	$writer.Flush()
	return ,$stream.ToArray()
}

function New-SizeChunkContent {
	$stream = New-Object System.IO.MemoryStream
	$writer = New-Object System.IO.BinaryWriter($stream)
	$writer.Write([int]$resolution)
	$writer.Write([int]$resolution)
	$writer.Write([int]$resolution)
	$writer.Flush()
	return ,$stream.ToArray()
}

function New-XyziChunkContent {
	param([byte[,,]]$Grid)

	$voxels = New-Object 'System.Collections.Generic.List[byte[]]'
	for ($x = 0; $x -lt $resolution; $x++) {
		for ($y = 0; $y -lt $resolution; $y++) {
			for ($z = 0; $z -lt $resolution; $z++) {
				$color = $Grid[$x, $y, $z]
				if ($color -ne 0) {
					$voxels.Add([byte[]]@([byte]$x, [byte]$y, [byte]$z, [byte]$color))
				}
			}
		}
	}

	$stream = New-Object System.IO.MemoryStream
	$writer = New-Object System.IO.BinaryWriter($stream)
	$writer.Write([int]$voxels.Count)
	foreach ($voxel in $voxels) {
		$stream.Write($voxel, 0, $voxel.Length)
	}
	$writer.Flush()
	return ,$stream.ToArray()
}

function New-RgbaChunkContent {
	param([object[]]$Palette)

	$stream = New-Object System.IO.MemoryStream
	$writer = New-Object System.IO.BinaryWriter($stream)
	for ($rawIndex = 0; $rawIndex -lt 255; $rawIndex++) {
		$entry = $Palette[$rawIndex + 1]
		$writer.Write([byte]$entry.R)
		$writer.Write([byte]$entry.G)
		$writer.Write([byte]$entry.B)
		$writer.Write([byte]$entry.A)
	}
	$writer.Write([byte]0)
	$writer.Write([byte]0)
	$writer.Write([byte]0)
	$writer.Write([byte]0)
	$writer.Flush()
	return ,$stream.ToArray()
}

function Write-VoxFile {
	param(
		[byte[,,]]$Grid,
		[object[]]$Palette,
		[string]$FilePath
	)

	$sizeChunk = New-ChunkBytes -Id 'SIZE' -Content (New-SizeChunkContent) -Children ([byte[]]::new(0))
	$xyziChunk = New-ChunkBytes -Id 'XYZI' -Content (New-XyziChunkContent -Grid $Grid) -Children ([byte[]]::new(0))
	$rgbaChunk = New-ChunkBytes -Id 'RGBA' -Content (New-RgbaChunkContent -Palette $Palette) -Children ([byte[]]::new(0))
	[byte[]]$mainChildren = $sizeChunk + $xyziChunk + $rgbaChunk
	$mainChunk = New-ChunkBytes -Id 'MAIN' -Content ([byte[]]::new(0)) -Children $mainChildren

	$directory = Split-Path -Parent $FilePath
	if (-not (Test-Path $directory)) {
		New-Item -ItemType Directory -Path $directory -Force | Out-Null
	}

	$stream = [System.IO.File]::Open($FilePath, [System.IO.FileMode]::Create, [System.IO.FileAccess]::Write)
	try {
		$writer = New-Object System.IO.BinaryWriter($stream)
		$headerBytes = [System.Text.Encoding]::ASCII.GetBytes('VOX ')
		$stream.Write($headerBytes, 0, $headerBytes.Length)
		$writer.Write([int]150)
		$stream.Write($mainChunk, 0, $mainChunk.Length)
		$writer.Flush()
	}
	finally {
		$stream.Dispose()
	}
}

if ($Clean -and (Test-Path $sourcesRoot)) {
	Remove-Item $sourcesRoot -Recurse -Force
}

if ($Clean -and (Test-Path $previewsRoot)) {
	Remove-Item $previewsRoot -Recurse -Force
}

New-Item -ItemType Directory -Path $sourcesRoot -Force | Out-Null

$palette = New-Object object[] 256
for ($index = 0; $index -lt $palette.Length; $index++) {
	$palette[$index] = [pscustomobject]@{ R = 0; G = 0; B = 0; A = 255 }
}

$paletteDefinitions = @(
	@{ Name = 'WhiteChicken'; R = 248; G = 243; B = 230 },
	@{ Name = 'BrownChicken'; R = 189; G = 117; B = 42 },
	@{ Name = 'ChickYellow'; R = 255; G = 225; B = 72 },
	@{ Name = 'PigPink'; R = 255; G = 146; B = 175 },
	@{ Name = 'Red'; R = 248; G = 48; B = 48 },
	@{ Name = 'AppleGreen'; R = 136; G = 214; B = 52 },
	@{ Name = 'MilkBlue'; R = 104; G = 188; B = 248 },
	@{ Name = 'BananaYellow'; R = 250; G = 224; B = 52 },
	@{ Name = 'GrapePurple'; R = 150; G = 82; B = 226 },
	@{ Name = 'MeatRed'; R = 198; G = 60; B = 60 },
	@{ Name = 'RoastBrown'; R = 165; G = 88; B = 38 },
	@{ Name = 'LeafGreen'; R = 84; G = 224; B = 78 },
	@{ Name = 'FenceBrown'; R = 156; G = 96; B = 36 },
	@{ Name = 'BushGreen'; R = 64; G = 176; B = 72 },
	@{ Name = 'GrassGreen'; R = 86; G = 210; B = 84 },
	@{ Name = 'YellowFlower'; R = 255; G = 210; B = 38 },
	@{ Name = 'WhitePetal'; R = 248; G = 245; B = 238 },
	@{ Name = 'FlowerRed'; R = 242; G = 72; B = 96 },
	@{ Name = 'SunflowerBrown'; R = 136; G = 82; B = 24 },
	@{ Name = 'PebbleGray'; R = 154; G = 148; B = 142 },
	@{ Name = 'WaterLilyPink'; R = 248; G = 162; B = 214 },
	@{ Name = 'SoilBrown'; R = 118; G = 76; B = 34 },
	@{ Name = 'StemGreen'; R = 74; G = 174; B = 64 },
	@{ Name = 'Bone'; R = 242; G = 229; B = 194 },
	@{ Name = 'PureRed'; R = 255; G = 0; B = 0 },
	@{ Name = 'PureOrange'; R = 255; G = 127; B = 0 },
	@{ Name = 'PureYellow'; R = 255; G = 255; B = 0 },
	@{ Name = 'PureGreen'; R = 0; G = 255; B = 0 },
	@{ Name = 'PureCyan'; R = 0; G = 255; B = 255 },
	@{ Name = 'PureBlue'; R = 0; G = 0; B = 255 },
	@{ Name = 'PureMagenta'; R = 255; G = 0; B = 255 }
)

$colorIndex = @{}
for ($i = 0; $i -lt $paletteDefinitions.Count; $i++) {
	$index = $i + 1
	$definition = $paletteDefinitions[$i]
	$palette[$index] = [pscustomobject]@{
		R = [byte]$definition.R
		G = [byte]$definition.G
		B = [byte]$definition.B
		A = [byte]255
	}
	$colorIndex[$definition.Name] = [byte]$index
}

function Get-ColorIndex {
	param([string]$Name)
	return [byte]$colorIndex[$Name]
}

$assets = @(
	[ordered]@{ Id = 'white_chicken'; DisplayName = 'White Chicken'; SourceVoxFile = 'Sources/SM_Vox_WhiteChicken.vox'; TargetPackagePath = '/Game/Vox/Meshes/Characters'; TargetAssetName = 'SM_Vox_WhiteChicken'; Category = 'Characters'; Repeatable = $false; PivotRule = 'GroundCentered'; CollisionType = 'UseComplexAsSimple'; Notes = 'Sample white chicken.'; Builder = 'WhiteChicken' },
	[ordered]@{ Id = 'brown_chicken'; DisplayName = 'Brown Chicken'; SourceVoxFile = 'Sources/SM_Vox_BrownChicken.vox'; TargetPackagePath = '/Game/Vox/Meshes/Characters'; TargetAssetName = 'SM_Vox_BrownChicken'; Category = 'Characters'; Repeatable = $false; PivotRule = 'GroundCentered'; CollisionType = 'UseComplexAsSimple'; Notes = 'Sample brown chicken.'; Builder = 'BrownChicken' },
	[ordered]@{ Id = 'chick'; DisplayName = 'Chick'; SourceVoxFile = 'Sources/SM_Vox_Chick.vox'; TargetPackagePath = '/Game/Vox/Meshes/Characters'; TargetAssetName = 'SM_Vox_Chick'; Category = 'Characters'; Repeatable = $false; PivotRule = 'GroundCentered'; CollisionType = 'UseComplexAsSimple'; Notes = 'Sample chick.'; Builder = 'Chick' },
	[ordered]@{ Id = 'pig'; DisplayName = 'Pig'; SourceVoxFile = 'Sources/SM_Vox_Pig.vox'; TargetPackagePath = '/Game/Vox/Meshes/Characters'; TargetAssetName = 'SM_Vox_Pig'; Category = 'Characters'; Repeatable = $false; PivotRule = 'GroundCentered'; CollisionType = 'UseComplexAsSimple'; Notes = 'Sample pig.'; Builder = 'Pig' },
	[ordered]@{ Id = 'apple'; DisplayName = 'Apple'; SourceVoxFile = 'Sources/SM_Vox_Apple.vox'; TargetPackagePath = '/Game/Vox/Meshes/Food'; TargetAssetName = 'SM_Vox_Apple'; Category = 'Food'; Repeatable = $false; PivotRule = 'GroundCentered'; CollisionType = 'UseComplexAsSimple'; Notes = 'Sample apple.'; Builder = 'Apple' },
	[ordered]@{ Id = 'milk_bottle'; DisplayName = 'Milk Bottle'; SourceVoxFile = 'Sources/SM_Vox_MilkBottle.vox'; TargetPackagePath = '/Game/Vox/Meshes/Food'; TargetAssetName = 'SM_Vox_MilkBottle'; Category = 'Food'; Repeatable = $false; PivotRule = 'GroundCentered'; CollisionType = 'UseComplexAsSimple'; Notes = 'Sample milk bottle.'; Builder = 'MilkBottle' },
	[ordered]@{ Id = 'strawberry'; DisplayName = 'Strawberry'; SourceVoxFile = 'Sources/SM_Vox_Strawberry.vox'; TargetPackagePath = '/Game/Vox/Meshes/Food'; TargetAssetName = 'SM_Vox_Strawberry'; Category = 'Food'; Repeatable = $false; PivotRule = 'GroundCentered'; CollisionType = 'UseComplexAsSimple'; Notes = 'Red body uses palette index 5 and leaves use index 12.'; Builder = 'Strawberry' },
	[ordered]@{ Id = 'banana'; DisplayName = 'Banana'; SourceVoxFile = 'Sources/SM_Vox_Banana.vox'; TargetPackagePath = '/Game/Vox/Meshes/Food'; TargetAssetName = 'SM_Vox_Banana'; Category = 'Food'; Repeatable = $false; PivotRule = 'GroundCentered'; CollisionType = 'UseComplexAsSimple'; Notes = 'Sample banana.'; Builder = 'Banana' },
	[ordered]@{ Id = 'grape_cluster'; DisplayName = 'Grape Cluster'; SourceVoxFile = 'Sources/SM_Vox_GrapeCluster.vox'; TargetPackagePath = '/Game/Vox/Meshes/Food'; TargetAssetName = 'SM_Vox_GrapeCluster'; Category = 'Food'; Repeatable = $false; PivotRule = 'GroundCentered'; CollisionType = 'UseComplexAsSimple'; Notes = 'Sample grapes.'; Builder = 'GrapeCluster' },
	[ordered]@{ Id = 'raw_meat'; DisplayName = 'Raw Meat'; SourceVoxFile = 'Sources/SM_Vox_RawMeat.vox'; TargetPackagePath = '/Game/Vox/Meshes/Food'; TargetAssetName = 'SM_Vox_RawMeat'; Category = 'Food'; Repeatable = $false; PivotRule = 'GroundCentered'; CollisionType = 'UseComplexAsSimple'; Notes = 'Sample raw meat.'; Builder = 'RawMeat' },
	[ordered]@{ Id = 'bone_in_roast'; DisplayName = 'Bone In Roast'; SourceVoxFile = 'Sources/SM_Vox_BoneInRoast.vox'; TargetPackagePath = '/Game/Vox/Meshes/Food'; TargetAssetName = 'SM_Vox_BoneInRoast'; Category = 'Food'; Repeatable = $false; PivotRule = 'GroundCentered'; CollisionType = 'UseComplexAsSimple'; Notes = 'Sample roasted meat with bone.'; Builder = 'BoneInRoast' },
	[ordered]@{ Id = 'fence'; DisplayName = 'Fence'; SourceVoxFile = 'Sources/SM_Vox_Fence.vox'; TargetPackagePath = '/Game/Vox/Meshes/Props'; TargetAssetName = 'SM_Vox_Fence'; Category = 'Props'; Repeatable = $true; PivotRule = 'GroundCentered'; CollisionType = 'SimpleBox'; Notes = 'Repeatable fence segment.'; Builder = 'Fence' },
	[ordered]@{ Id = 'bush'; DisplayName = 'Bush'; SourceVoxFile = 'Sources/SM_Vox_Bush.vox'; TargetPackagePath = '/Game/Vox/Meshes/Foliage'; TargetAssetName = 'SM_Vox_Bush'; Category = 'Foliage'; Repeatable = $true; PivotRule = 'GroundCentered'; CollisionType = 'None'; Notes = 'Sample bush.'; Builder = 'Bush' },
	[ordered]@{ Id = 'grass'; DisplayName = 'Grass'; SourceVoxFile = 'Sources/SM_Vox_Grass.vox'; TargetPackagePath = '/Game/Vox/Meshes/Foliage'; TargetAssetName = 'SM_Vox_Grass'; Category = 'Foliage'; Repeatable = $true; PivotRule = 'GroundCentered'; CollisionType = 'None'; Notes = 'Sample grass patch.'; Builder = 'Grass' },
	[ordered]@{ Id = 'yellow_flower'; DisplayName = 'Yellow Flower'; SourceVoxFile = 'Sources/SM_Vox_YellowFlower.vox'; TargetPackagePath = '/Game/Vox/Meshes/Foliage'; TargetAssetName = 'SM_Vox_YellowFlower'; Category = 'Foliage'; Repeatable = $true; PivotRule = 'GroundCentered'; CollisionType = 'None'; Notes = 'Sample yellow flower.'; Builder = 'YellowFlower' },
	[ordered]@{ Id = 'white_flower'; DisplayName = 'White Flower'; SourceVoxFile = 'Sources/SM_Vox_WhiteFlower.vox'; TargetPackagePath = '/Game/Vox/Meshes/Foliage'; TargetAssetName = 'SM_Vox_WhiteFlower'; Category = 'Foliage'; Repeatable = $true; PivotRule = 'GroundCentered'; CollisionType = 'None'; Notes = 'Sample white flower.'; Builder = 'WhiteFlower' },
	[ordered]@{ Id = 'red_flower'; DisplayName = 'Red Flower'; SourceVoxFile = 'Sources/SM_Vox_RedFlower.vox'; TargetPackagePath = '/Game/Vox/Meshes/Foliage'; TargetAssetName = 'SM_Vox_RedFlower'; Category = 'Foliage'; Repeatable = $true; PivotRule = 'GroundCentered'; CollisionType = 'None'; Notes = 'Sample red flower.'; Builder = 'RedFlower' },
	[ordered]@{ Id = 'sunflower'; DisplayName = 'Sunflower'; SourceVoxFile = 'Sources/SM_Vox_Sunflower.vox'; TargetPackagePath = '/Game/Vox/Meshes/Foliage'; TargetAssetName = 'SM_Vox_Sunflower'; Category = 'Foliage'; Repeatable = $true; PivotRule = 'GroundCentered'; CollisionType = 'None'; Notes = 'Sample sunflower.'; Builder = 'Sunflower' },
	[ordered]@{ Id = 'pebble_ground_tile'; DisplayName = 'Pebble Ground Tile'; SourceVoxFile = 'Sources/SM_Vox_PebbleGroundTile.vox'; TargetPackagePath = '/Game/Vox/Meshes/Ground'; TargetAssetName = 'SM_Vox_PebbleGroundTile'; Category = 'Ground'; Repeatable = $true; PivotRule = 'Centered'; CollisionType = 'SimpleBox'; Notes = 'Flat repeatable ground tile.'; Builder = 'PebbleGroundTile' },
	[ordered]@{ Id = 'water_lily'; DisplayName = 'Water Lily'; SourceVoxFile = 'Sources/SM_Vox_WaterLily.vox'; TargetPackagePath = '/Game/Vox/Meshes/Foliage'; TargetAssetName = 'SM_Vox_WaterLily'; Category = 'Foliage'; Repeatable = $true; PivotRule = 'GroundCentered'; CollisionType = 'None'; Notes = 'Sample water lily.'; Builder = 'WaterLily' },
	[ordered]@{ Id = 'rainbow_diagnostic'; DisplayName = 'Rainbow Diagnostic'; SourceVoxFile = 'Sources/SM_Vox_RainbowDiagnostic.vox'; TargetPackagePath = '/Game/Vox/Meshes/Props'; TargetAssetName = 'SM_Vox_RainbowDiagnostic'; Category = 'Props'; Repeatable = $false; PivotRule = 'GroundCentered'; CollisionType = 'UseComplexAsSimple'; Notes = 'Pure high-saturation sRGB rainbow stripes for color-space diagnostics.'; Builder = 'RainbowDiagnostic' }
)

$manifestAssets = New-Object 'System.Collections.Generic.List[object]'

foreach ($asset in $assets) {
	$grid = New-VoxelGrid

	switch ($asset.Builder) {
		'WhiteChicken' { Build-Chicken -Grid $grid -BodyColor (Get-ColorIndex 'WhiteChicken') -BeakColor (Get-ColorIndex 'YellowFlower') -CombColor (Get-ColorIndex 'Red') -LegColor (Get-ColorIndex 'FenceBrown') }
		'BrownChicken' { Build-Chicken -Grid $grid -BodyColor (Get-ColorIndex 'BrownChicken') -BeakColor (Get-ColorIndex 'YellowFlower') -CombColor (Get-ColorIndex 'Red') -LegColor (Get-ColorIndex 'FenceBrown') }
		'Chick' { Build-Chick -Grid $grid -BodyColor (Get-ColorIndex 'ChickYellow') -BeakColor (Get-ColorIndex 'YellowFlower') -LegColor (Get-ColorIndex 'FenceBrown') }
		'Pig' { Build-Pig -Grid $grid -BodyColor (Get-ColorIndex 'PigPink') -AccentColor (Get-ColorIndex 'WhitePetal') }
		'Apple' { Build-Apple -Grid $grid -FruitColor (Get-ColorIndex 'Red') -StemColor (Get-ColorIndex 'FenceBrown') -LeafColor (Get-ColorIndex 'LeafGreen') }
		'MilkBottle' { Build-MilkBottle -Grid $grid -BottleColor (Get-ColorIndex 'WhitePetal') -CapColor (Get-ColorIndex 'MilkBlue') }
		'Strawberry' { Build-Strawberry -Grid $grid -BodyColor (Get-ColorIndex 'Red') -LeafColor (Get-ColorIndex 'LeafGreen') }
		'Banana' { Build-Banana -Grid $grid -BodyColor (Get-ColorIndex 'BananaYellow') -TipColor (Get-ColorIndex 'FenceBrown') }
		'GrapeCluster' { Build-GrapeCluster -Grid $grid -FruitColor (Get-ColorIndex 'GrapePurple') -StemColor (Get-ColorIndex 'StemGreen') }
		'RawMeat' { Build-RawMeat -Grid $grid -MeatColor (Get-ColorIndex 'MeatRed') -FatColor (Get-ColorIndex 'WhitePetal') }
		'BoneInRoast' { Build-BoneInRoast -Grid $grid -RoastColor (Get-ColorIndex 'RoastBrown') -BoneColor (Get-ColorIndex 'Bone') }
		'Fence' { Build-Fence -Grid $grid -WoodColor (Get-ColorIndex 'FenceBrown') }
		'Bush' { Build-Bush -Grid $grid -LeafColor (Get-ColorIndex 'BushGreen') -WoodColor (Get-ColorIndex 'FenceBrown') }
		'Grass' { Build-Grass -Grid $grid -GrassColor (Get-ColorIndex 'GrassGreen') }
		'YellowFlower' { Add-Flower -Grid $grid -StemHeight 16 -StemColor (Get-ColorIndex 'StemGreen') -PetalColor (Get-ColorIndex 'YellowFlower') -CenterColor (Get-ColorIndex 'SunflowerBrown') -PetalRadius 2 }
		'WhiteFlower' { Add-Flower -Grid $grid -StemHeight 16 -StemColor (Get-ColorIndex 'StemGreen') -PetalColor (Get-ColorIndex 'WhitePetal') -CenterColor (Get-ColorIndex 'YellowFlower') -PetalRadius 2 }
		'RedFlower' { Add-Flower -Grid $grid -StemHeight 16 -StemColor (Get-ColorIndex 'StemGreen') -PetalColor (Get-ColorIndex 'FlowerRed') -CenterColor (Get-ColorIndex 'YellowFlower') -PetalRadius 2 }
		'Sunflower' { Build-Sunflower -Grid $grid -StemColor (Get-ColorIndex 'StemGreen') -PetalColor (Get-ColorIndex 'YellowFlower') -CenterColor (Get-ColorIndex 'SunflowerBrown') }
		'PebbleGroundTile' { Build-PebbleGroundTile -Grid $grid -BaseColor (Get-ColorIndex 'SoilBrown') -PebbleColor (Get-ColorIndex 'PebbleGray') }
		'WaterLily' { Build-WaterLily -Grid $grid -PadColor (Get-ColorIndex 'LeafGreen') -BlossomColor (Get-ColorIndex 'WaterLilyPink') }
		'RainbowDiagnostic' { Build-RainbowDiagnostic -Grid $grid -StripeColors @((Get-ColorIndex 'PureRed'), (Get-ColorIndex 'PureOrange'), (Get-ColorIndex 'PureYellow'), (Get-ColorIndex 'PureGreen'), (Get-ColorIndex 'PureCyan'), (Get-ColorIndex 'PureBlue'), (Get-ColorIndex 'PureMagenta')) }
		default { throw "Unknown builder '$($asset.Builder)'." }
	}

	$filePath = Join-Path $OutputRoot $asset.SourceVoxFile
	Write-VoxFile -Grid $grid -Palette $palette -FilePath $filePath

	$manifestAssets.Add([ordered]@{
		id = $asset.Id
		displayName = $asset.DisplayName
		sourceVoxFile = $asset.SourceVoxFile
		targetPackagePath = $asset.TargetPackagePath
		targetAssetName = $asset.TargetAssetName
		category = $asset.Category
		repeatable = $asset.Repeatable
		pivotRule = $asset.PivotRule
		collisionType = $asset.CollisionType
		notes = $asset.Notes
	})
}

$manifest = [ordered]@{
	voxelSize = 5.0
	assets = $manifestAssets
}

$manifest | ConvertTo-Json -Depth 8 | Set-Content -Path $manifestPath -Encoding UTF8

Write-Host "Generated $($assets.Count) VOX sample sources under '$sourcesRoot'."
Write-Host "Wrote manifest '$manifestPath'."

if ($GeneratePreviews) {
	$previewScriptPath = Join-Path $projectRoot 'Scripts\GenerateVoxPreviewPngs.py'
	if (-not (Test-Path $previewScriptPath)) {
		throw "Preview script not found at '$previewScriptPath'."
	}

	& $PythonExe $previewScriptPath --manifest $manifestPath --source-root $OutputRoot --output-root $previewsRoot
	if ($LASTEXITCODE -ne 0) {
		throw "Preview generation failed with exit code $LASTEXITCODE."
	}

	Write-Host "Generated angled preview PNGs under '$previewsRoot'."
}
