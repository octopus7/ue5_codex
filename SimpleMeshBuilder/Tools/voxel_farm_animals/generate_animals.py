from __future__ import annotations

import argparse
import json
import math
import struct
from collections import defaultdict
from pathlib import Path
from typing import Iterable


ROOT_DIR = Path(__file__).resolve().parents[2]
DEFAULT_SPEC_DIR = Path(__file__).resolve().parent / "specs"
DEFAULT_OUTPUT_DIR = ROOT_DIR / "Generated" / "VoxelFarmAnimals"
SUPPORTED_SHAPES = {
    "ellipsoid",
    "sphere",
    "box",
    "cylinder_x",
    "cylinder_y",
    "cylinder_z",
}
SUPPORTED_OPERATIONS = {"add", "paint", "remove"}
FACE_DIRECTIONS = (
    ((1, 0, 0), lambda x0, x1, y0, y1, z0, z1: ((x1, y0, z0), (x1, y1, z0), (x1, y1, z1), (x1, y0, z1))),
    ((-1, 0, 0), lambda x0, x1, y0, y1, z0, z1: ((x0, y0, z0), (x0, y0, z1), (x0, y1, z1), (x0, y1, z0))),
    ((0, 1, 0), lambda x0, x1, y0, y1, z0, z1: ((x0, y1, z0), (x0, y1, z1), (x1, y1, z1), (x1, y1, z0))),
    ((0, -1, 0), lambda x0, x1, y0, y1, z0, z1: ((x0, y0, z0), (x1, y0, z0), (x1, y0, z1), (x0, y0, z1))),
    ((0, 0, 1), lambda x0, x1, y0, y1, z0, z1: ((x0, y0, z1), (x1, y0, z1), (x1, y1, z1), (x0, y1, z1))),
    ((0, 0, -1), lambda x0, x1, y0, y1, z0, z1: ((x0, y0, z0), (x0, y1, z0), (x1, y1, z0), (x1, y0, z0))),
)


class SpecError(RuntimeError):
    pass


def vector3(values: Iterable[float], field_name: str) -> tuple[float, float, float]:
    sequence = list(values)
    if len(sequence) != 3:
        raise SpecError(f"{field_name} must have exactly 3 numeric values.")
    try:
        return float(sequence[0]), float(sequence[1]), float(sequence[2])
    except (TypeError, ValueError) as exc:
        raise SpecError(f"{field_name} must contain numeric values.") from exc


def parse_hex_color(value: str) -> int:
    text = str(value).strip().lstrip("#")
    if len(text) != 6 or any(ch not in "0123456789abcdefABCDEF" for ch in text):
        raise SpecError(f"Invalid RGB hex color '{value}'.")
    return int(text, 16)


def decode_color(color_value: int) -> tuple[int, int, int]:
    return (color_value >> 16) & 0xFF, (color_value >> 8) & 0xFF, color_value & 0xFF


def load_spec(spec_path: Path) -> dict:
    try:
        payload = json.loads(spec_path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        raise SpecError(f"{spec_path.name}: invalid JSON: {exc}") from exc

    if not isinstance(payload, dict):
        raise SpecError(f"{spec_path.name}: top level must be an object.")

    name = str(payload.get("name", "")).strip()
    if not name:
        raise SpecError(f"{spec_path.name}: missing 'name'.")

    resolution = int(payload.get("resolution", 0))
    if resolution != 64:
        raise SpecError(f"{spec_path.name}: resolution must be 64.")

    palette_payload = payload.get("palette")
    if not isinstance(palette_payload, dict) or not palette_payload:
        raise SpecError(f"{spec_path.name}: palette must be a non-empty object.")

    palette: dict[str, int] = {}
    for material_name, color_text in palette_payload.items():
        key = str(material_name).strip()
        if not key:
            raise SpecError(f"{spec_path.name}: palette keys must be non-empty strings.")
        palette[key] = parse_hex_color(str(color_text))

    primitives = payload.get("primitives")
    if not isinstance(primitives, list) or not primitives:
        raise SpecError(f"{spec_path.name}: primitives must be a non-empty array.")

    validated_primitives: list[dict] = []
    for primitive in primitives:
        if not isinstance(primitive, dict):
            raise SpecError(f"{spec_path.name}: each primitive must be an object.")

        primitive_name = str(primitive.get("name", "")).strip()
        if not primitive_name:
            raise SpecError(f"{spec_path.name}: each primitive requires a non-empty name.")

        shape = str(primitive.get("shape", "")).strip().lower()
        if shape not in SUPPORTED_SHAPES:
            raise SpecError(f"{spec_path.name}: unsupported shape '{shape}' in primitive '{primitive_name}'.")

        operation = str(primitive.get("operation", "add")).strip().lower()
        if operation not in SUPPORTED_OPERATIONS:
            raise SpecError(
                f"{spec_path.name}: unsupported operation '{operation}' in primitive '{primitive_name}'."
            )

        material = primitive.get("material")
        if operation != "remove":
            material_name = str(material).strip()
            if material_name not in palette:
                raise SpecError(
                    f"{spec_path.name}: primitive '{primitive_name}' refers to unknown material '{material_name}'."
                )
        else:
            material_name = ""

        center = vector3(primitive.get("center", []), f"{spec_path.name}:{primitive_name}:center")
        size = vector3(primitive.get("size", []), f"{spec_path.name}:{primitive_name}:size")
        if any(component <= 0 for component in size):
            raise SpecError(f"{spec_path.name}: primitive '{primitive_name}' size values must be positive.")

        rotation = vector3(primitive.get("rotation_deg", [0, 0, 0]), f"{spec_path.name}:{primitive_name}:rotation_deg")

        validated_primitives.append(
            {
                "name": primitive_name,
                "shape": shape,
                "material": material_name,
                "operation": operation,
                "center": center,
                "size": size,
                "rotation_deg": rotation,
            }
        )

    return {
        "name": name,
        "resolution": resolution,
        "description": str(payload.get("description", "")).strip(),
        "palette": palette,
        "primitives": validated_primitives,
        "source_path": str(spec_path),
    }


def rotation_matrix_xyz(rotation_deg: tuple[float, float, float]) -> tuple[tuple[float, float, float], ...]:
    rx, ry, rz = (math.radians(value) for value in rotation_deg)

    cx, sx = math.cos(rx), math.sin(rx)
    cy, sy = math.cos(ry), math.sin(ry)
    cz, sz = math.cos(rz), math.sin(rz)

    mx = ((1.0, 0.0, 0.0), (0.0, cx, -sx), (0.0, sx, cx))
    my = ((cy, 0.0, sy), (0.0, 1.0, 0.0), (-sy, 0.0, cy))
    mz = ((cz, -sz, 0.0), (sz, cz, 0.0), (0.0, 0.0, 1.0))

    return matmul3(mz, matmul3(my, mx))


def matmul3(
    a: tuple[tuple[float, float, float], ...],
    b: tuple[tuple[float, float, float], ...],
) -> tuple[tuple[float, float, float], ...]:
    rows = []
    for row in range(3):
        rows.append(
            (
                a[row][0] * b[0][0] + a[row][1] * b[1][0] + a[row][2] * b[2][0],
                a[row][0] * b[0][1] + a[row][1] * b[1][1] + a[row][2] * b[2][1],
                a[row][0] * b[0][2] + a[row][1] * b[1][2] + a[row][2] * b[2][2],
            )
        )
    return tuple(rows)


def transpose3(matrix: tuple[tuple[float, float, float], ...]) -> tuple[tuple[float, float, float], ...]:
    return (
        (matrix[0][0], matrix[1][0], matrix[2][0]),
        (matrix[0][1], matrix[1][1], matrix[2][1]),
        (matrix[0][2], matrix[1][2], matrix[2][2]),
    )


def transform_local(
    point: tuple[float, float, float],
    center: tuple[float, float, float],
    inverse_rotation: tuple[tuple[float, float, float], ...],
) -> tuple[float, float, float]:
    px = point[0] - center[0]
    py = point[1] - center[1]
    pz = point[2] - center[2]
    return (
        inverse_rotation[0][0] * px + inverse_rotation[0][1] * py + inverse_rotation[0][2] * pz,
        inverse_rotation[1][0] * px + inverse_rotation[1][1] * py + inverse_rotation[1][2] * pz,
        inverse_rotation[2][0] * px + inverse_rotation[2][1] * py + inverse_rotation[2][2] * pz,
    )


def primitive_bounds(
    center: tuple[float, float, float],
    half_size: tuple[float, float, float],
    rotation: tuple[tuple[float, float, float], ...],
    resolution: int,
) -> tuple[range, range, range]:
    abs_rot = tuple(
        tuple(abs(rotation[row][column]) for column in range(3))
        for row in range(3)
    )
    extent_x = abs_rot[0][0] * half_size[0] + abs_rot[0][1] * half_size[1] + abs_rot[0][2] * half_size[2]
    extent_y = abs_rot[1][0] * half_size[0] + abs_rot[1][1] * half_size[1] + abs_rot[1][2] * half_size[2]
    extent_z = abs_rot[2][0] * half_size[0] + abs_rot[2][1] * half_size[1] + abs_rot[2][2] * half_size[2]

    x_min = max(0, int(math.floor(center[0] - extent_x - 1.0)))
    x_max = min(resolution - 1, int(math.ceil(center[0] + extent_x + 1.0)))
    y_min = max(0, int(math.floor(center[1] - extent_y - 1.0)))
    y_max = min(resolution - 1, int(math.ceil(center[1] + extent_y + 1.0)))
    z_min = max(0, int(math.floor(center[2] - extent_z - 1.0)))
    z_max = min(resolution - 1, int(math.ceil(center[2] + extent_z + 1.0)))

    return range(x_min, x_max + 1), range(y_min, y_max + 1), range(z_min, z_max + 1)


def point_inside_primitive(local_point: tuple[float, float, float], shape: str, half_size: tuple[float, float, float]) -> bool:
    hx, hy, hz = half_size
    lx, ly, lz = local_point

    if shape in {"ellipsoid", "sphere"}:
        return (lx / hx) ** 2 + (ly / hy) ** 2 + (lz / hz) ** 2 <= 1.0
    if shape == "box":
        return abs(lx) <= hx and abs(ly) <= hy and abs(lz) <= hz
    if shape == "cylinder_x":
        return abs(lx) <= hx and (ly / hy) ** 2 + (lz / hz) ** 2 <= 1.0
    if shape == "cylinder_y":
        return abs(ly) <= hy and (lx / hx) ** 2 + (lz / hz) ** 2 <= 1.0
    if shape == "cylinder_z":
        return abs(lz) <= hz and (lx / hx) ** 2 + (ly / hy) ** 2 <= 1.0
    raise SpecError(f"Unsupported shape '{shape}'.")


def fill_grid(spec: dict) -> list[int]:
    resolution = spec["resolution"]
    grid = [0] * (resolution * resolution * resolution)

    def index(x: int, y: int, z: int) -> int:
        return x + (y * resolution) + (z * resolution * resolution)

    for primitive in spec["primitives"]:
        center = primitive["center"]
        half_size = (primitive["size"][0] / 2.0, primitive["size"][1] / 2.0, primitive["size"][2] / 2.0)
        rotation = rotation_matrix_xyz(primitive["rotation_deg"])
        inverse_rotation = transpose3(rotation)
        x_range, y_range, z_range = primitive_bounds(center, half_size, rotation, resolution)

        for z in z_range:
            for y in y_range:
                for x in x_range:
                    local_point = transform_local((float(x), float(y), float(z)), center, inverse_rotation)
                    if not point_inside_primitive(local_point, primitive["shape"], half_size):
                        continue

                    grid_index = index(x, y, z)
                    operation = primitive["operation"]
                    if operation == "remove":
                        grid[grid_index] = 0
                    elif operation == "paint":
                        if grid[grid_index] != 0:
                            grid[grid_index] = spec["palette"][primitive["material"]]
                    else:
                        grid[grid_index] = spec["palette"][primitive["material"]]

    return grid


def occupied_voxels(grid: list[int], resolution: int) -> list[tuple[int, int, int, int]]:
    voxels = []
    for z in range(resolution):
        for y in range(resolution):
            row_offset = y * resolution + z * resolution * resolution
            for x in range(resolution):
                color_value = grid[row_offset + x]
                if color_value:
                    voxels.append((x, y, z, color_value))
    return voxels


def vox_palette(voxels: list[tuple[int, int, int, int]]) -> tuple[dict[int, int], list[tuple[int, int, int, int]]]:
    palette_indices: dict[int, int] = {}
    palette_entries: list[tuple[int, int, int, int]] = []

    for _, _, _, color_value in voxels:
        if color_value in palette_indices:
            continue
        palette_index = len(palette_entries) + 1
        if palette_index > 255:
            raise SpecError("VOX palette limit exceeded; keep models at 255 colors or fewer.")
        palette_indices[color_value] = palette_index
        r, g, b = decode_color(color_value)
        palette_entries.append((r, g, b, 255))

    while len(palette_entries) < 256:
        palette_entries.append((0, 0, 0, 0))

    return palette_indices, palette_entries


def vox_chunk(chunk_id: bytes, content: bytes, children: bytes = b"") -> bytes:
    return chunk_id + struct.pack("<II", len(content), len(children)) + content + children


def write_vox(path: Path, resolution: int, grid: list[int]) -> int:
    voxels = occupied_voxels(grid, resolution)
    if not voxels:
        raise SpecError(f"{path.name}: generated an empty voxel model.")

    palette_indices, palette_entries = vox_palette(voxels)
    size_chunk = vox_chunk(b"SIZE", struct.pack("<III", resolution, resolution, resolution))

    xyzi_content = bytearray()
    xyzi_content.extend(struct.pack("<I", len(voxels)))
    for x, y, z, color_value in voxels:
        xyzi_content.extend(struct.pack("<BBBB", x, y, z, palette_indices[color_value]))
    xyzi_chunk = vox_chunk(b"XYZI", bytes(xyzi_content))

    rgba_content = bytearray()
    for rgba in palette_entries:
        rgba_content.extend(struct.pack("<BBBB", *rgba))
    rgba_chunk = vox_chunk(b"RGBA", bytes(rgba_content))

    main_children = size_chunk + xyzi_chunk + rgba_chunk
    payload = b"VOX " + struct.pack("<I", 150) + vox_chunk(b"MAIN", b"", main_children)
    path.write_bytes(payload)
    return len(voxels)


def write_obj(path: Path, resolution: int, grid: list[int]) -> tuple[int, int]:
    mtl_path = path.with_suffix(".mtl")
    faces_by_color: dict[int, list[tuple[tuple[float, float, float], ...]]] = defaultdict(list)
    face_count = 0

    def index(x: int, y: int, z: int) -> int:
        return x + (y * resolution) + (z * resolution * resolution)

    def color_at(x: int, y: int, z: int) -> int:
        if x < 0 or y < 0 or z < 0 or x >= resolution or y >= resolution or z >= resolution:
            return 0
        return grid[index(x, y, z)]

    x_offset = resolution / 2.0
    y_offset = resolution / 2.0

    for z in range(resolution):
        for y in range(resolution):
            for x in range(resolution):
                color_value = color_at(x, y, z)
                if color_value == 0:
                    continue

                x0 = float(x) - x_offset
                x1 = x0 + 1.0
                y0 = float(y) - y_offset
                y1 = y0 + 1.0
                z0 = float(z)
                z1 = z0 + 1.0

                for (dx, dy, dz), quad_factory in FACE_DIRECTIONS:
                    if color_at(x + dx, y + dy, z + dz) != 0:
                        continue
                    faces_by_color[color_value].append(quad_factory(x0, x1, y0, y1, z0, z1))
                    face_count += 1

    if face_count == 0:
        raise SpecError(f"{path.name}: generated an empty surface mesh.")

    obj_lines = [
        f"mtllib {mtl_path.name}",
        f"o {path.stem}",
    ]
    mtl_lines = []
    vertex_index = 1

    for color_value in sorted(faces_by_color):
        material_name = f"c_{color_value:06X}"
        r, g, b = decode_color(color_value)
        mtl_lines.extend(
            [
                f"newmtl {material_name}",
                f"Kd {r / 255.0:.6f} {g / 255.0:.6f} {b / 255.0:.6f}",
                f"Ka {r / 255.0:.6f} {g / 255.0:.6f} {b / 255.0:.6f}",
                "d 1.0",
                "illum 1",
                "",
            ]
        )
        obj_lines.append(f"usemtl {material_name}")

        for quad in faces_by_color[color_value]:
            for vx, vy, vz in quad:
                obj_lines.append(f"v {vx:.3f} {vy:.3f} {vz:.3f}")
            obj_lines.append(f"f {vertex_index} {vertex_index + 1} {vertex_index + 2}")
            obj_lines.append(f"f {vertex_index} {vertex_index + 2} {vertex_index + 3}")
            vertex_index += 4

    path.write_text("\n".join(obj_lines) + "\n", encoding="utf-8")
    mtl_path.write_text("\n".join(mtl_lines), encoding="utf-8")
    triangle_count = face_count * 2
    return face_count, triangle_count


def compute_bounds(grid: list[int], resolution: int) -> dict[str, list[int]]:
    xs: list[int] = []
    ys: list[int] = []
    zs: list[int] = []
    for z in range(resolution):
        for y in range(resolution):
            row_offset = y * resolution + z * resolution * resolution
            for x in range(resolution):
                if grid[row_offset + x]:
                    xs.append(x)
                    ys.append(y)
                    zs.append(z)

    if not xs:
        return {"min": [0, 0, 0], "max": [0, 0, 0]}

    return {
        "min": [min(xs), min(ys), min(zs)],
        "max": [max(xs), max(ys), max(zs)],
    }


def iter_spec_files(spec_dir: Path, only_names: set[str]) -> list[Path]:
    spec_files = sorted(path for path in spec_dir.glob("*.json") if path.is_file())
    if not only_names:
        return spec_files
    filtered = [path for path in spec_files if path.stem in only_names or path.name in only_names]
    if not filtered:
        raise SpecError("No spec files matched --only values.")
    return filtered


def generate_assets(spec_dir: Path, output_dir: Path, only_names: set[str]) -> list[dict]:
    output_dir.mkdir(parents=True, exist_ok=True)
    manifest: list[dict] = []

    for spec_path in iter_spec_files(spec_dir, only_names):
        spec = load_spec(spec_path)
        grid = fill_grid(spec)
        bounds = compute_bounds(grid, spec["resolution"])

        output_base = output_dir / spec["name"]
        occupied_count = write_vox(output_base.with_suffix(".vox"), spec["resolution"], grid)
        face_count, triangle_count = write_obj(output_base.with_suffix(".obj"), spec["resolution"], grid)

        manifest.append(
            {
                "name": spec["name"],
                "description": spec["description"],
                "resolution": spec["resolution"],
                "occupied_voxels": occupied_count,
                "quad_faces": face_count,
                "triangles": triangle_count,
                "bounds": bounds,
                "source_path": spec["source_path"],
                "vox_path": str(output_base.with_suffix(".vox")),
                "obj_path": str(output_base.with_suffix(".obj")),
                "mtl_path": str(output_base.with_suffix(".mtl")),
            }
        )

    manifest_path = output_dir / "manifest.json"
    manifest_path.write_text(json.dumps(manifest, ensure_ascii=False, indent=2), encoding="utf-8")
    return manifest


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate cute 64^3 voxel farm animals as VOX and OBJ assets.")
    parser.add_argument("--spec-dir", type=Path, default=DEFAULT_SPEC_DIR, help="Directory containing animal spec JSON files.")
    parser.add_argument("--output-dir", type=Path, default=DEFAULT_OUTPUT_DIR, help="Directory for generated VOX/OBJ assets.")
    parser.add_argument("--only", nargs="*", default=(), help="Optional subset of spec file stems or file names to generate.")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        manifest = generate_assets(args.spec_dir.resolve(), args.output_dir.resolve(), set(args.only))
    except SpecError as exc:
        print(f"ERROR: {exc}")
        return 1

    for entry in manifest:
        print(
            f"{entry['name']}: voxels={entry['occupied_voxels']} "
            f"triangles={entry['triangles']} bounds={entry['bounds']['min']}..{entry['bounds']['max']}"
        )
    print(f"Generated {len(manifest)} asset(s) in {args.output_dir.resolve()}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

