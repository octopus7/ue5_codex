from __future__ import annotations

import argparse
import json
import math
import struct
from pathlib import Path


VOX_VERSION = 150
VOXEL_SIZE = 10.0


class VoxelModel:
    def __init__(self, size: tuple[int, int, int]) -> None:
        self.size = size
        self.voxels: dict[tuple[int, int, int], str] = {}

    def set_voxel(self, x: int, y: int, z: int, color: str) -> None:
        size_x, size_y, size_z = self.size
        if 0 <= x < size_x and 0 <= y < size_y and 0 <= z < size_z:
            self.voxels[(x, y, z)] = color

    def fill_box(self, min_corner: tuple[int, int, int], max_corner: tuple[int, int, int], color: str) -> None:
        min_x, min_y, min_z = min_corner
        max_x, max_y, max_z = max_corner
        for x in range(min_x, max_x + 1):
            for y in range(min_y, max_y + 1):
                for z in range(min_z, max_z + 1):
                    self.set_voxel(x, y, z, color)

    def fill_sphere(self, center: tuple[float, float, float], radius: float, color: str) -> None:
        center_x, center_y, center_z = center
        radius_sq = radius * radius
        size_x, size_y, size_z = self.size
        for x in range(size_x):
            for y in range(size_y):
                for z in range(size_z):
                    dx = x - center_x
                    dy = y - center_y
                    dz = z - center_z
                    if (dx * dx) + (dy * dy) + (dz * dz) <= radius_sq:
                        self.set_voxel(x, y, z, color)

    def carve_sphere(self, center: tuple[float, float, float], radius: float) -> None:
        center_x, center_y, center_z = center
        radius_sq = radius * radius
        for voxel in list(self.voxels):
            dx = voxel[0] - center_x
            dy = voxel[1] - center_y
            dz = voxel[2] - center_z
            if (dx * dx) + (dy * dy) + (dz * dz) <= radius_sq:
                self.voxels.pop(voxel, None)


PALETTE = {
    "wood_dark": (102, 63, 36, 255),
    "wood_light": (154, 103, 63, 255),
    "leaf_dark": (44, 133, 74, 255),
    "leaf_light": (95, 180, 90, 255),
    "stone_dark": (99, 99, 110, 255),
    "stone_light": (152, 152, 164, 255),
    "sand": (211, 188, 118, 255),
    "accent": (227, 90, 74, 255),
}


def build_bridge() -> VoxelModel:
    model = VoxelModel((30, 18, 18))
    model.fill_box((2, 6, 0), (4, 11, 4), "wood_dark")
    model.fill_box((25, 6, 0), (27, 11, 4), "wood_dark")

    for x in range(5, 25):
        arch_height = 4 + int(round(3.0 * math.sin(((x - 5) / 19.0) * math.pi)))
        model.fill_box((x, 7, arch_height), (x, 10, arch_height + 1), "wood_light")
        if x % 3 == 0:
            model.fill_box((x, 6, arch_height + 2), (x, 6, arch_height + 4), "wood_dark")
            model.fill_box((x, 11, arch_height + 2), (x, 11, arch_height + 4), "wood_dark")

    model.fill_box((6, 6, 8), (24, 6, 9), "wood_dark")
    model.fill_box((6, 11, 8), (24, 11, 9), "wood_dark")
    return model


def build_statue() -> VoxelModel:
    model = VoxelModel((20, 20, 28))
    model.fill_box((5, 5, 0), (14, 14, 4), "stone_light")
    model.fill_box((6, 6, 5), (13, 13, 7), "stone_dark")
    model.fill_box((8, 8, 8), (11, 11, 16), "stone_light")
    model.fill_box((7, 7, 10), (8, 12, 13), "stone_dark")
    model.fill_box((11, 7, 10), (12, 12, 13), "stone_dark")
    model.fill_box((7, 9, 17), (12, 10, 20), "stone_light")
    model.fill_sphere((9.5, 9.5, 23.0), 3.5, "stone_light")
    model.fill_box((8, 8, 22), (11, 11, 24), "accent")
    return model


def build_palm_tree() -> VoxelModel:
    model = VoxelModel((30, 30, 30))
    trunk_points = [
        (14, 14, 0),
        (14, 14, 4),
        (15, 14, 8),
        (15, 15, 12),
        (16, 15, 16),
        (16, 16, 20),
    ]

    for center_x, center_y, center_z in trunk_points:
        model.fill_box((center_x - 1, center_y - 1, center_z), (center_x + 1, center_y + 1, center_z + 3), "wood_light")

    leaf_centers = [
        (9, 16, 22),
        (12, 22, 22),
        (18, 23, 22),
        (22, 17, 22),
        (19, 10, 22),
        (12, 9, 22),
    ]
    leaf_offsets = [(-2, 0), (-1, 1), (1, 1), (2, 0), (1, -1), (-1, -1)]

    for center_x, center_y, center_z in leaf_centers:
        model.fill_sphere((center_x, center_y, center_z), 2.8, "leaf_light")
        for offset_x, offset_y in leaf_offsets:
            model.fill_box(
                (center_x + offset_x * 2, center_y + offset_y * 2, center_z - 1),
                (center_x + offset_x * 3, center_y + offset_y * 3, center_z + 1),
                "leaf_dark",
            )

    return model


def build_rock() -> VoxelModel:
    model = VoxelModel((18, 18, 14))
    model.fill_sphere((8.0, 8.0, 5.0), 5.8, "stone_light")
    model.fill_sphere((11.0, 7.0, 6.5), 4.2, "stone_dark")
    model.fill_sphere((6.0, 10.5, 4.5), 3.5, "stone_dark")
    model.carve_sphere((4.5, 4.5, 9.0), 2.2)
    model.carve_sphere((13.5, 12.0, 10.5), 2.0)
    return model


def build_bush() -> VoxelModel:
    model = VoxelModel((22, 22, 16))
    model.fill_box((9, 9, 0), (12, 12, 3), "wood_dark")
    model.fill_sphere((7.0, 10.0, 8.0), 4.5, "leaf_light")
    model.fill_sphere((13.0, 10.0, 8.0), 4.8, "leaf_light")
    model.fill_sphere((10.0, 7.0, 8.0), 4.2, "leaf_dark")
    model.fill_sphere((10.0, 13.0, 8.0), 4.2, "leaf_dark")
    model.fill_sphere((10.0, 10.0, 10.0), 4.5, "leaf_light")
    return model


MODEL_BUILDERS = {
    "bridge": ("SM_VOX_Bridge", build_bridge),
    "statue": ("SM_VOX_Statue", build_statue),
    "palm_tree": ("SM_VOX_PalmTree", build_palm_tree),
    "rock": ("SM_VOX_Rock", build_rock),
    "bush": ("SM_VOX_Bush", build_bush),
}


def chunk(chunk_id: bytes, content: bytes, children: bytes = b"") -> bytes:
    return chunk_id + struct.pack("<II", len(content), len(children)) + content + children


def write_vox_file(path: Path, model: VoxelModel) -> None:
    palette_entries = list(PALETTE.values())
    color_lookup = {name: index + 1 for index, name in enumerate(PALETTE)}
    voxels = sorted(model.voxels.items())

    size_chunk = chunk(b"SIZE", struct.pack("<III", *model.size))

    xyzi_content = struct.pack("<I", len(voxels))
    for (x, y, z), color_name in voxels:
        xyzi_content += struct.pack("<BBBB", x, y, z, color_lookup[color_name])
    xyzi_chunk = chunk(b"XYZI", xyzi_content)

    rgba_content = bytearray(1024)
    for index, rgba in enumerate(palette_entries, start=1):
        offset = (index - 1) * 4
        rgba_content[offset : offset + 4] = bytes(rgba)
    rgba_chunk = chunk(b"RGBA", bytes(rgba_content))

    children = size_chunk + xyzi_chunk + rgba_chunk
    main_chunk = chunk(b"MAIN", b"", children)
    path.write_bytes(b"VOX " + struct.pack("<I", VOX_VERSION) + main_chunk)


FACE_DEFINITIONS = [
    ((1, 0, 0), ((1, 0, 0), (1, 1, 0), (1, 1, 1), (1, 0, 1))),
    ((-1, 0, 0), ((0, 1, 0), (0, 0, 0), (0, 0, 1), (0, 1, 1))),
    ((0, 1, 0), ((0, 1, 0), (1, 1, 0), (1, 1, 1), (0, 1, 1))),
    ((0, -1, 0), ((1, 0, 0), (0, 0, 0), (0, 0, 1), (1, 0, 1))),
    ((0, 0, 1), ((0, 0, 1), (1, 0, 1), (1, 1, 1), (0, 1, 1))),
    ((0, 0, -1), ((0, 1, 0), (1, 1, 0), (1, 0, 0), (0, 0, 0))),
]


def write_obj_file(path: Path, model: VoxelModel) -> None:
    occupied = set(model.voxels)
    if not occupied:
        path.write_text("")
        return

    min_x = min(x for x, _, _ in occupied)
    max_x = max(x for x, _, _ in occupied)
    min_y = min(y for _, y, _ in occupied)
    max_y = max(y for _, y, _ in occupied)
    min_z = min(z for _, _, z in occupied)

    center_x = (min_x + max_x + 1) / 2.0
    center_y = (min_y + max_y + 1) / 2.0

    vertices: list[tuple[float, float, float]] = []
    normals: list[tuple[int, int, int]] = []
    faces: list[tuple[list[int], int]] = []

    def convert_vertex(vertex: tuple[int, int, int]) -> tuple[float, float, float]:
        x, y, z = vertex
        return (
            (x - center_x) * VOXEL_SIZE,
            (y - center_y) * VOXEL_SIZE,
            (z - min_z) * VOXEL_SIZE,
        )

    for voxel in sorted(occupied):
        voxel_x, voxel_y, voxel_z = voxel
        for normal, face_vertices in FACE_DEFINITIONS:
            neighbor = (voxel_x + normal[0], voxel_y + normal[1], voxel_z + normal[2])
            if neighbor in occupied:
                continue

            normal_index = len(normals) + 1
            normals.append(normal)

            face_indices: list[int] = []
            for vertex in face_vertices:
                world_vertex = (voxel_x + vertex[0], voxel_y + vertex[1], voxel_z + vertex[2])
                vertices.append(convert_vertex(world_vertex))
                face_indices.append(len(vertices))
            faces.append((face_indices, normal_index))

    lines = ["# Generated by generate_vox_props.py"]
    lines.extend(f"v {x:.4f} {y:.4f} {z:.4f}" for x, y, z in vertices)
    lines.extend(f"vn {x} {y} {z}" for x, y, z in normals)

    for face_indices, normal_index in faces:
        a, b, c, d = face_indices
        lines.append(f"f {a}//{normal_index} {b}//{normal_index} {c}//{normal_index}")
        lines.append(f"f {a}//{normal_index} {c}//{normal_index} {d}//{normal_index}")

    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def generate_assets(output_dir: Path) -> None:
    output_dir.mkdir(parents=True, exist_ok=True)
    manifest: list[dict[str, str]] = []

    for model_key, (asset_name, builder) in MODEL_BUILDERS.items():
        model = builder()
        vox_path = output_dir / f"{model_key}.vox"
        obj_path = output_dir / f"{model_key}.obj"

        write_vox_file(vox_path, model)
        write_obj_file(obj_path, model)

        manifest.append(
            {
                "id": model_key,
                "asset_name": asset_name,
                "vox_path": str(vox_path.resolve()),
                "obj_path": str(obj_path.resolve()),
            }
        )

    manifest_path = output_dir / "manifest.json"
    manifest_path.write_text(json.dumps(manifest, indent=2), encoding="utf-8")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=Path("SourceArt/Vox/Props"),
    )
    args = parser.parse_args()
    generate_assets(args.output_dir)


if __name__ == "__main__":
    main()
