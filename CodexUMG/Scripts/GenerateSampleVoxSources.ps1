[CmdletBinding()]
param(
	[string]$OutputRoot
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

if ([string]::IsNullOrWhiteSpace($OutputRoot)) {
	$OutputRoot = Join-Path (Split-Path -Parent $PSScriptRoot) 'SourceArt/Vox/Sources'
}

$script:Resolution = 32

$script:Colors = [ordered]@{
	White = [byte]1
	Cream = [byte]2
	Yellow = [byte]3
	Orange = [byte]4
	Red = [byte]5
	Brown = [byte]6
	DarkBrown = [byte]7
	Pink = [byte]8
	LightPink = [byte]9
	Green = [byte]10
	DarkGreen = [byte]11
	LeafGreen = [byte]12
	Purple = [byte]13
	Blue = [byte]14
	Bone = [byte]15
	Black = [byte]16
	Gray = [byte]17
	Stone = [byte]18
	Water = [byte]19
	Teal = [byte]20
	Beige = [byte]21
	MeatRaw = [byte]22
	MeatCooked = [byte]23
	Grass = [byte]24
	PetalYellow = [byte]25
	PetalWhite = [byte]26
	PetalRed = [byte]27
	DarkPink = [byte]28
	Gold = [byte]29
	CapBlue = [byte]30
	LeafDark = [byte]31
}

function New-VoxModel {
	[ordered]@{
		Width = $script:Resolution
		Height = $script:Resolution
		Depth = $script:Resolution
		Voxels = New-Object System.Collections.Generic.List[object]
		Keys = New-Object 'System.Collections.Generic.HashSet[string]'
	}
}

function Add-Voxel {
	param([hashtable]$Model, [int]$X, [int]$Y, [int]$Z, [byte]$Color)
	if ($X -lt 0 -or $X -ge $script:Resolution -or $Y -lt 0 -or $Y -ge $script:Resolution -or $Z -lt 0 -or $Z -ge $script:Resolution) { return }
	$key = "$X,$Y,$Z"
	if (-not $Model.Keys.Add($key)) { return }
	$Model.Voxels.Add([pscustomobject]@{ X = [byte]$X; Y = [byte]$Y; Z = [byte]$Z; Color = $Color }) | Out-Null
}

function Add-Box {
	param([hashtable]$Model, [int]$X0, [int]$Y0, [int]$Z0, [int]$X1, [int]$Y1, [int]$Z1, [byte]$Color)
	for ($x = $X0; $x -le $X1; $x++) { for ($y = $Y0; $y -le $Y1; $y++) { for ($z = $Z0; $z -le $Z1; $z++) { Add-Voxel $Model $x $y $z $Color } } }
}

function Add-Ellipsoid {
	param([hashtable]$Model, [int]$CX, [int]$CY, [int]$CZ, [int]$RX, [int]$RY, [int]$RZ, [byte]$Color)
	for ($x = $CX - $RX; $x -le $CX + $RX; $x++) {
		for ($y = $CY - $RY; $y -le $CY + $RY; $y++) {
			for ($z = $CZ - $RZ; $z -le $CZ + $RZ; $z++) {
				$nx = ($x - $CX) / [double]$RX
				$ny = ($y - $CY) / [double]$RY
				$nz = ($z - $CZ) / [double]$RZ
				if (($nx * $nx) + ($ny * $ny) + ($nz * $nz) -le 1.0) { Add-Voxel $Model $x $y $z $Color }
			}
		}
	}
}

function Add-CylinderY {
	param([hashtable]$Model, [int]$CX, [int]$CZ, [int]$Y0, [int]$Y1, [int]$Radius, [byte]$Color)
	for ($y = $Y0; $y -le $Y1; $y++) {
		for ($x = $CX - $Radius; $x -le $CX + $Radius; $x++) {
			for ($z = $CZ - $Radius; $z -le $CZ + $Radius; $z++) {
				$dx = $x - $CX
				$dz = $z - $CZ
				if (($dx * $dx) + ($dz * $dz) -le ($Radius * $Radius)) { Add-Voxel $Model $x $y $z $Color }
			}
		}
	}
}

function Add-Line {
	param([hashtable]$Model, [int]$X0, [int]$Y0, [int]$Z0, [int]$X1, [int]$Y1, [int]$Z1, [byte]$Color)
	$steps = [Math]::Max([Math]::Max([Math]::Abs($X1 - $X0), [Math]::Abs($Y1 - $Y0)), [Math]::Abs($Z1 - $Z0))
	if ($steps -eq 0) { Add-Voxel $Model $X0 $Y0 $Z0 $Color; return }
	for ($i = 0; $i -le $steps; $i++) {
		$t = $i / [double]$steps
		Add-Voxel $Model ([Math]::Round($X0 + (($X1 - $X0) * $t))) ([Math]::Round($Y0 + (($Y1 - $Y0) * $t))) ([Math]::Round($Z0 + (($Z1 - $Z0) * $t))) $Color
	}
}

function Build-Chicken {
	param([hashtable]$Model, [byte]$Body, [byte]$Head)
	Add-Ellipsoid $Model 15 12 16 5 4 3 $Body
	Add-Ellipsoid $Model 20 13 16 2 2 2 $Head
	Add-Voxel $Model 21 13 16 $script:Colors.Orange
	Add-Voxel $Model 22 13 16 $script:Colors.Orange
	Add-Voxel $Model 20 16 16 $script:Colors.Red
	Add-Voxel $Model 20 17 16 $script:Colors.Red
	Add-Voxel $Model 19 17 16 $script:Colors.Red
	Add-Voxel $Model 13 9 14 $script:Colors.Black
	Add-Voxel $Model 13 9 18 $script:Colors.Black
	Add-CylinderY $Model 13 15 4 8 0 $script:Colors.Orange
	Add-CylinderY $Model 18 15 4 8 0 $script:Colors.Orange
	Add-Box $Model 12 3 14 13 4 15 $script:Colors.Orange
	Add-Box $Model 17 3 14 18 4 15 $script:Colors.Orange
}

function Build-Chick {
	param([hashtable]$Model)
	Add-Ellipsoid $Model 16 11 16 3 3 3 $script:Colors.Yellow
	Add-Voxel $Model 18 12 16 $script:Colors.Orange
	Add-Voxel $Model 19 12 16 $script:Colors.Orange
	Add-Voxel $Model 15 9 15 $script:Colors.Black
	Add-Voxel $Model 15 9 17 $script:Colors.Black
	Add-CylinderY $Model 15 16 4 7 0 $script:Colors.Orange
	Add-CylinderY $Model 17 16 4 7 0 $script:Colors.Orange
}

function Build-Pig {
	param([hashtable]$Model)
	Add-Ellipsoid $Model 16 12 16 6 4 4 $script:Colors.Pink
	Add-Ellipsoid $Model 22 12 16 2 2 2 $script:Colors.DarkPink
	Add-Voxel $Model 24 12 15 $script:Colors.DarkPink
	Add-Voxel $Model 24 12 17 $script:Colors.DarkPink
	Add-Voxel $Model 23 9 13 $script:Colors.Black
	Add-Voxel $Model 23 9 19 $script:Colors.Black
	Add-CylinderY $Model 13 13 4 8 0 $script:Colors.DarkPink
	Add-CylinderY $Model 19 13 4 8 0 $script:Colors.DarkPink
	Add-CylinderY $Model 13 19 4 8 0 $script:Colors.DarkPink
	Add-CylinderY $Model 19 19 4 8 0 $script:Colors.DarkPink
	Add-Line $Model 10 13 16 8 15 16 $script:Colors.DarkPink
}

function Build-Apple {
	param([hashtable]$Model)
	Add-Ellipsoid $Model 16 12 16 4 4 4 $script:Colors.Red
	Add-Voxel $Model 16 17 16 $script:Colors.Brown
	Add-Voxel $Model 16 18 16 $script:Colors.Brown
	Add-Voxel $Model 17 17 15 $script:Colors.LeafGreen
	Add-Voxel $Model 18 18 15 $script:Colors.LeafGreen
}

function Build-MilkBottle {
	param([hashtable]$Model)
	Add-Box $Model 13 6 13 19 15 19 $script:Colors.White
	Add-Box $Model 14 15 14 18 19 18 $script:Colors.White
	Add-Box $Model 14 19 14 18 21 18 $script:Colors.CapBlue
	Add-Box $Model 14 10 13 14 13 13 $script:Colors.Blue
	Add-Box $Model 15 11 13 17 13 13 $script:Colors.Blue
	Add-Box $Model 18 10 13 18 13 13 $script:Colors.Blue
}

function Build-Strawberry {
	param([hashtable]$Model)
	Add-Ellipsoid $Model 16 11 16 3 4 3 $script:Colors.Red
	Add-Voxel $Model 16 16 16 $script:Colors.LeafGreen
	Add-Voxel $Model 15 16 15 $script:Colors.LeafGreen
	Add-Voxel $Model 17 16 15 $script:Colors.LeafGreen
	Add-Voxel $Model 15 17 16 $script:Colors.LeafGreen
	Add-Voxel $Model 17 17 16 $script:Colors.LeafGreen
	Add-Voxel $Model 16 10 16 $script:Colors.Yellow
	Add-Voxel $Model 15 12 14 $script:Colors.Yellow
	Add-Voxel $Model 17 12 18 $script:Colors.Yellow
}

function Build-Banana {
	param([hashtable]$Model)
	$points = @(@(12,10,15), @(13,11,15), @(14,12,16), @(15,13,16), @(17,13,16), @(18,12,15), @(19,11,15))
	foreach ($p in $points) { Add-Ellipsoid $Model $p[0] $p[1] $p[2] 2 1 1 $script:Colors.Yellow }
	Add-Voxel $Model 11 10 15 $script:Colors.DarkBrown
	Add-Voxel $Model 20 10 15 $script:Colors.DarkBrown
}

function Build-GrapeCluster {
	param([hashtable]$Model)
	Add-CylinderY $Model 16 15 16 20 0 $script:Colors.Brown
	Add-Ellipsoid $Model 15 13 16 2 2 2 $script:Colors.Purple
	Add-Ellipsoid $Model 18 13 16 2 2 2 $script:Colors.Purple
	Add-Ellipsoid $Model 14 11 15 2 2 2 $script:Colors.Purple
	Add-Ellipsoid $Model 17 11 15 2 2 2 $script:Colors.Purple
	Add-Ellipsoid $Model 16 9 18 2 2 2 $script:Colors.Purple
	Add-Voxel $Model 15 18 15 $script:Colors.LeafGreen
	Add-Voxel $Model 17 18 15 $script:Colors.LeafGreen
}

function Build-RawMeat {
	param([hashtable]$Model)
	Add-Box $Model 12 10 13 20 15 18 $script:Colors.MeatRaw
	Add-Box $Model 15 11 15 19 13 16 $script:Colors.Pink
	Add-Box $Model 18 10 14 19 14 15 $script:Colors.Bone
}

function Build-CookedMeatWithBone {
	param([hashtable]$Model)
	Add-Box $Model 12 10 13 20 15 18 $script:Colors.MeatCooked
	Add-Box $Model 15 11 15 19 13 16 $script:Colors.DarkBrown
	Add-Box $Model 18 10 14 19 14 15 $script:Colors.Bone
	Add-Voxel $Model 11 13 16 $script:Colors.Bone
	Add-Voxel $Model 21 13 16 $script:Colors.Bone
}

function Build-Fence {
	param([hashtable]$Model)
	Add-Box $Model 6 8 14 8 20 17 $script:Colors.Brown
	Add-Box $Model 14 8 14 16 20 17 $script:Colors.Brown
	Add-Box $Model 22 8 14 24 20 17 $script:Colors.Brown
	Add-Box $Model 6 11 14 24 12 17 $script:Colors.DarkBrown
	Add-Box $Model 6 15 14 24 16 17 $script:Colors.DarkBrown
}

function Build-Bush {
	param([hashtable]$Model)
	Add-Ellipsoid $Model 16 11 16 6 4 5 $script:Colors.Green
	Add-Ellipsoid $Model 12 10 15 3 2 3 $script:Colors.DarkGreen
	Add-Ellipsoid $Model 20 10 17 3 2 3 $script:Colors.DarkGreen
	Add-Box $Model 14 6 14 18 7 18 $script:Colors.Brown
}

function Build-Grass {
	param([hashtable]$Model)
	Add-Box $Model 14 4 15 17 5 17 $script:Colors.Brown
	Add-Line $Model 15 5 16 14 10 15 $script:Colors.Grass
	Add-Line $Model 16 5 16 16 11 16 $script:Colors.Grass
	Add-Line $Model 17 5 16 18 10 17 $script:Colors.Grass
	Add-Line $Model 16 6 15 15 10 14 $script:Colors.DarkGreen
}

function Build-Flower {
	param([hashtable]$Model, [byte]$PetalColor)
	Add-Line $Model 16 5 16 16 14 16 $script:Colors.Grass
	Add-Voxel $Model 16 15 16 $script:Colors.Gold
	Add-Voxel $Model 15 15 16 $PetalColor
	Add-Voxel $Model 17 15 16 $PetalColor
	Add-Voxel $Model 16 15 15 $PetalColor
	Add-Voxel $Model 16 15 17 $PetalColor
	Add-Voxel $Model 15 16 15 $PetalColor
	Add-Voxel $Model 17 16 17 $PetalColor
}

function Build-Sunflower {
	param([hashtable]$Model)
	Add-Line $Model 16 5 16 16 18 16 $script:Colors.LeafGreen
	Add-Ellipsoid $Model 16 20 16 2 2 2 $script:Colors.Brown
	Add-Ellipsoid $Model 16 20 16 4 1 4 $script:Colors.PetalYellow
}

function Build-PebbleGroundTile {
	param([hashtable]$Model)
	Add-Box $Model 0 0 0 31 1 31 $script:Colors.Stone
	for ($x = 2; $x -lt 31; $x += 4) {
		for ($z = 2; $z -lt 31; $z += 4) {
			$c = if ((($x + $z) % 3) -eq 0) { $script:Colors.Gray } elseif ((($x + $z) % 3) -eq 1) { $script:Colors.Brown } else { $script:Colors.Beige }
			Add-Ellipsoid $Model $x 2 $z 1 1 1 $c
		}
	}
}

function Build-WaterLily {
	param([hashtable]$Model)
	Add-Box $Model 0 0 0 31 1 31 $script:Colors.Water
	Add-Ellipsoid $Model 16 2 16 5 1 5 $script:Colors.LeafGreen
	Add-Voxel $Model 16 3 16 $script:Colors.PetalWhite
	Add-Voxel $Model 15 3 16 $script:Colors.PetalWhite
	Add-Voxel $Model 17 3 16 $script:Colors.PetalWhite
	Add-Voxel $Model 16 3 15 $script:Colors.PetalWhite
	Add-Voxel $Model 16 3 17 $script:Colors.PetalWhite
}

function Get-PaletteBytes {
	$palette = New-Object byte[] (256 * 4)
	for ($i = 0; $i -lt 256; $i++) {
		$base = $i * 4
		$palette[$base + 0] = 48
		$palette[$base + 1] = 48
		$palette[$base + 2] = 48
		$palette[$base + 3] = 255
	}
	$colors = @(
		@(0,0,0,0), @(255,255,255,255), @(245,236,205,255), @(255,227,102,255), @(255,170,60,255),
		@(232,74,60,255), @(150,100,60,255), @(90,55,35,255), @(245,180,190,255), @(255,215,220,255),
		@(90,180,90,255), @(55,120,55,255), @(130,220,100,255), @(140,90,170,255), @(90,140,240,255),
		@(225,210,180,255), @(20,20,20,255), @(120,120,120,255), @(160,150,130,255), @(90,160,210,255),
		@(60,180,160,255), @(220,200,160,255), @(180,70,60,255), @(160,90,50,255), @(100,190,80,255),
		@(240,210,50,255), @(250,250,245,255), @(220,60,80,255), @(220,150,160,255), @(250,200,70,255),
		@(40,90,40,255), @(70,140,240,255)
	)
	for ($i = 0; $i -lt $colors.Count; $i++) {
		$base = ($i + 1) * 4
		$palette[$base + 0] = [byte]$colors[$i][0]
		$palette[$base + 1] = [byte]$colors[$i][1]
		$palette[$base + 2] = [byte]$colors[$i][2]
		$palette[$base + 3] = [byte]$colors[$i][3]
	}
	return ,$palette
}

function New-Chunk {
	param([string]$Id, [byte[]]$Content, [byte[]]$Children)
	$ms = New-Object System.IO.MemoryStream
	$bw = New-Object System.IO.BinaryWriter($ms, [System.Text.Encoding]::ASCII, $true)
	$bw.Write([System.Text.Encoding]::ASCII.GetBytes($Id))
	$bw.Write([int]$Content.Length)
	$bw.Write([int]$Children.Length)
	$bw.Write($Content)
	$bw.Write($Children)
	$bw.Flush()
	return ,$ms.ToArray()
}

function New-SizeChunk {
	$empty = New-Object byte[] 0
	$ms = New-Object System.IO.MemoryStream
	$bw = New-Object System.IO.BinaryWriter($ms, [System.Text.Encoding]::ASCII, $true)
	$bw.Write([int]$script:Resolution)
	$bw.Write([int]$script:Resolution)
	$bw.Write([int]$script:Resolution)
	$bw.Flush()
	return ,(New-Chunk 'SIZE' $ms.ToArray() $empty)
}

function New-XyziChunk {
	param([System.Collections.Generic.List[object]]$Voxels)
	$empty = New-Object byte[] 0
	$ms = New-Object System.IO.MemoryStream
	$bw = New-Object System.IO.BinaryWriter($ms, [System.Text.Encoding]::ASCII, $true)
	$bw.Write([int]$Voxels.Count)
	foreach ($v in $Voxels) { $bw.Write([byte]$v.X); $bw.Write([byte]$v.Y); $bw.Write([byte]$v.Z); $bw.Write([byte]$v.Color) }
	$bw.Flush()
	return ,(New-Chunk 'XYZI' $ms.ToArray() $empty)
}

function New-RgbaChunk {
	$empty = New-Object byte[] 0
	return ,(New-Chunk 'RGBA' (Get-PaletteBytes) $empty)
}

function Write-VoxFile {
	param([string]$Path, [hashtable]$Model)
	$children = New-Object System.IO.MemoryStream
	foreach ($chunk in @((New-SizeChunk), (New-XyziChunk $Model.Voxels), (New-RgbaChunk))) {
		[byte[]]$chunkBytes = $chunk
		$children.Write($chunkBytes, 0, $chunkBytes.Length) | Out-Null
	}
	$empty = New-Object byte[] 0
	[byte[]]$main = New-Chunk 'MAIN' $empty $children.ToArray()
	$ms = New-Object System.IO.MemoryStream
	$bw = New-Object System.IO.BinaryWriter($ms, [System.Text.Encoding]::ASCII, $true)
	$bw.Write([System.Text.Encoding]::ASCII.GetBytes('VOX '))
	$bw.Write([int]150)
	$bw.Write($main)
	$bw.Flush()
	$dir = Split-Path -Parent $Path
	if ($dir) { New-Item -ItemType Directory -Force -Path $dir | Out-Null }
	[System.IO.File]::WriteAllBytes($Path, $ms.ToArray())
}

function Build-Model {
	param([string]$Id, [hashtable]$Model)
	switch ($Id) {
		'SM_Vox_WhiteChicken' { Build-Chicken $Model $script:Colors.White $script:Colors.White; break }
		'SM_Vox_BrownChicken' { Build-Chicken $Model $script:Colors.Brown $script:Colors.Cream; break }
		'SM_Vox_Chick' { Build-Chick $Model; break }
		'SM_Vox_Pig' { Build-Pig $Model; break }
		'SM_Vox_Apple' { Build-Apple $Model; break }
		'SM_Vox_MilkBottle' { Build-MilkBottle $Model; break }
		'SM_Vox_Strawberry' { Build-Strawberry $Model; break }
		'SM_Vox_Banana' { Build-Banana $Model; break }
		'SM_Vox_GrapeCluster' { Build-GrapeCluster $Model; break }
		'SM_Vox_RawMeat' { Build-RawMeat $Model; break }
		'SM_Vox_CookedMeatWithBone' { Build-CookedMeatWithBone $Model; break }
		'SM_Vox_Fence' { Build-Fence $Model; break }
		'SM_Vox_Bush' { Build-Bush $Model; break }
		'SM_Vox_Grass' { Build-Grass $Model; break }
		'SM_Vox_YellowFlower' { Build-Flower $Model $script:Colors.PetalYellow; break }
		'SM_Vox_WhiteFlower' { Build-Flower $Model $script:Colors.PetalWhite; break }
		'SM_Vox_RedFlower' { Build-Flower $Model $script:Colors.PetalRed; break }
		'SM_Vox_Sunflower' { Build-Sunflower $Model; break }
		'SM_Vox_PebbleGroundTile' { Build-PebbleGroundTile $Model; break }
		'SM_Vox_WaterLily' { Build-WaterLily $Model; break }
		default { throw "Unknown asset id: $Id" }
	}
}

$assets = @(
	@{ Id = 'SM_Vox_WhiteChicken'; File = 'SM_Vox_WhiteChicken.vox' },
	@{ Id = 'SM_Vox_BrownChicken'; File = 'SM_Vox_BrownChicken.vox' },
	@{ Id = 'SM_Vox_Chick'; File = 'SM_Vox_Chick.vox' },
	@{ Id = 'SM_Vox_Pig'; File = 'SM_Vox_Pig.vox' },
	@{ Id = 'SM_Vox_Apple'; File = 'SM_Vox_Apple.vox' },
	@{ Id = 'SM_Vox_MilkBottle'; File = 'SM_Vox_MilkBottle.vox' },
	@{ Id = 'SM_Vox_Strawberry'; File = 'SM_Vox_Strawberry.vox' },
	@{ Id = 'SM_Vox_Banana'; File = 'SM_Vox_Banana.vox' },
	@{ Id = 'SM_Vox_GrapeCluster'; File = 'SM_Vox_GrapeCluster.vox' },
	@{ Id = 'SM_Vox_RawMeat'; File = 'SM_Vox_RawMeat.vox' },
	@{ Id = 'SM_Vox_CookedMeatWithBone'; File = 'SM_Vox_CookedMeatWithBone.vox' },
	@{ Id = 'SM_Vox_Fence'; File = 'SM_Vox_Fence.vox' },
	@{ Id = 'SM_Vox_Bush'; File = 'SM_Vox_Bush.vox' },
	@{ Id = 'SM_Vox_Grass'; File = 'SM_Vox_Grass.vox' },
	@{ Id = 'SM_Vox_YellowFlower'; File = 'SM_Vox_YellowFlower.vox' },
	@{ Id = 'SM_Vox_WhiteFlower'; File = 'SM_Vox_WhiteFlower.vox' },
	@{ Id = 'SM_Vox_RedFlower'; File = 'SM_Vox_RedFlower.vox' },
	@{ Id = 'SM_Vox_Sunflower'; File = 'SM_Vox_Sunflower.vox' },
	@{ Id = 'SM_Vox_PebbleGroundTile'; File = 'SM_Vox_PebbleGroundTile.vox' },
	@{ Id = 'SM_Vox_WaterLily'; File = 'SM_Vox_WaterLily.vox' }
)

New-Item -ItemType Directory -Force -Path $OutputRoot | Out-Null
foreach ($asset in $assets) {
	$model = New-VoxModel
	Build-Model -Id $asset.Id -Model $model
	Write-VoxFile -Path (Join-Path $OutputRoot $asset.File) -Model $model
}
