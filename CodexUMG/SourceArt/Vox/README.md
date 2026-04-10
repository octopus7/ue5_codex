# VOX Sample Sources

This folder defines the sample VOX source layout used by the test asset pipeline.

## Layout

- `SourceArt/Vox/VoxAssetManifest.json`
- `SourceArt/Vox/Sources/*.vox`

The PowerShell script at `Scripts/GenerateSampleVoxSources.ps1` writes VOX 150 files with the required `SIZE`, `XYZI`, and `RGBA` chunks.

## Usage

```powershell
.\Scripts\GenerateSampleVoxSources.ps1
```

Default output:

- `SourceArt/Vox/Sources`

## Rules

- VOX resolution is fixed to `32x32x32`.
- The manifest stores source paths relative to `SourceArt/Vox`.
- All generated test assets use a shared vertex-color workflow downstream in UE.
