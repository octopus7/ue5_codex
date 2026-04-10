# VOX Sample Sources

This folder defines the sample VOX source layout used by the test asset pipeline.

## Layout

- `SourceArt/Vox/VoxAssetManifest.json`
- `SourceArt/Vox/Sources/*.vox`

The PowerShell script at `Scripts/GenerateSampleVoxSources.ps1` writes MagicaVoxel VOX 150 files with the required `SIZE`, `XYZI`, and `RGBA` chunks.
It also generates `SM_Vox_RainbowDiagnostic.vox`, a pure high-saturation rainbow sample used to verify whether muted colors come from authored source palettes or from a broken color-space conversion path.

## Usage

```powershell
.\Scripts\GenerateSampleVoxSources.ps1
```

Generate sources and angled preview PNGs together:

```powershell
.\Scripts\GenerateSampleVoxSources.ps1 -GeneratePreviews
```

Generate previews from existing VOX files only:

```powershell
python .\Scripts\GenerateVoxPreviewPngs.py --manifest .\SourceArt\Vox\VoxAssetManifest.json --source-root .\SourceArt\Vox --output-root .\SourceArt\Vox\Previews
```

Default output:

- `SourceArt/Vox/Sources`
- `SourceArt/Vox/Previews`

## Rules

- VOX resolution is fixed to `32x32x32`.
- The manifest stores source paths relative to `SourceArt/Vox`.
- All generated test assets use a shared vertex-color workflow downstream in UE.
- The authored sample palette is intentionally pushed toward higher saturation for easier in-editor readability.
- `SM_Vox_RainbowDiagnostic` is expected to render vivid red, orange, yellow, green, cyan, blue, and magenta stripes in `Unlit` mode.
- Preview PNGs are rendered in an angled orthographic voxel view so color and silhouette can be checked before opening UE.
