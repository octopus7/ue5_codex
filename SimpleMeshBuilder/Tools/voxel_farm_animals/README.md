Voxel Farm Animals

This folder contains a small voxel asset pipeline for generating cute 64^3 farm-animal models as MagicaVoxel `.vox` files and surface-exported `.obj` files.

Usage

```powershell
python Tools\voxel_farm_animals\generate_animals.py
```

Outputs are written to `Generated\VoxelFarmAnimals`.

Spec format

- `name`: output base name, for example `cow_res64_codex`
- `resolution`: must be `64`
- `description`: short free-form note
- `palette`: object mapping material names to 6-digit RGB hex strings
- `primitives`: ordered array of voxelized primitives

Primitive fields

- `name`: readable primitive name
- `shape`: `ellipsoid`, `sphere`, `box`, `cylinder_x`, `cylinder_y`, or `cylinder_z`
- `material`: palette key, required for `add` and `paint`
- `operation`: `add`, `paint`, or `remove` (defaults to `add`)
- `center`: `[x, y, z]` voxel-space center
- `size`: `[sx, sy, sz]` full size in voxels
- `rotation_deg`: optional `[rx, ry, rz]`, defaults to `[0, 0, 0]`

Notes

- Later primitives override earlier ones.
- `paint` only affects already-filled voxels.
- `remove` carves voxels out of the current shape.
- Coordinates are in a `64 x 64 x 64` voxel grid with `z` up.
- Keep the model inside the grid and leave at least a small margin from the borders.
