from __future__ import annotations

import argparse
import json
import math
import struct
import sys
import zlib
from dataclasses import dataclass
from pathlib import Path


TOP_FACE = ((0, 1, 0), (1, 1, 0), (1, 1, 1), (0, 1, 1))
X_FACE = ((1, 0, 0), (1, 1, 0), (1, 1, 1), (1, 0, 1))
Z_FACE = ((0, 0, 1), (1, 0, 1), (1, 1, 1), (0, 1, 1))


@dataclass(frozen=True)
class PreviewAsset:
    display_name: str
    source_file: Path
    output_file: Path


@dataclass
class VoxModel:
    size: tuple[int, int, int]
    voxels: dict[tuple[int, int, int], int]
    palette: list[tuple[int, int, int, int]]


def parse_args() -> argparse.Namespace:
    repo_root = Path(__file__).resolve().parent.parent

    parser = argparse.ArgumentParser(
        description="Render angled preview PNGs from VOX source files."
    )
    parser.add_argument(
        "--manifest",
        type=Path,
        default=repo_root / "SourceArt" / "Vox" / "VoxAssetManifest.json",
        help="Path to VoxAssetManifest.json.",
    )
    parser.add_argument(
        "--source-root",
        type=Path,
        default=repo_root / "SourceArt" / "Vox",
        help="Root folder used to resolve sourceVoxFile entries.",
    )
    parser.add_argument(
        "--output-root",
        type=Path,
        default=repo_root / "SourceArt" / "Vox" / "Previews",
        help="Folder where preview PNGs will be written.",
    )
    parser.add_argument(
        "--scale",
        type=int,
        default=8,
        help="Base projection scale in pixels.",
    )
    return parser.parse_args()


def load_preview_assets(
    manifest_path: Path, source_root: Path, output_root: Path
) -> list[PreviewAsset]:
    manifest = json.loads(manifest_path.read_text(encoding="utf-8-sig"))
    assets: list[PreviewAsset] = []

    for asset in manifest.get("assets", []):
        category = asset.get("category", "Uncategorized")
        source_file = source_root / asset["sourceVoxFile"]
        output_file = output_root / category / f"{asset['targetAssetName']}.png"
        assets.append(
            PreviewAsset(
                display_name=asset.get("displayName", asset["targetAssetName"]),
                source_file=source_file,
                output_file=output_file,
            )
        )

    return assets


def parse_vox_file(path: Path) -> VoxModel:
    data = path.read_bytes()
    if len(data) < 20 or data[:4] != b"VOX ":
        raise ValueError(f"{path} is not a valid VOX file.")

    version = struct.unpack_from("<I", data, 4)[0]
    if version != 150:
        raise ValueError(f"{path} uses unsupported VOX version {version}.")

    if data[8:12] != b"MAIN":
        raise ValueError(f"{path} is missing a MAIN chunk.")

    main_content_size, main_children_size = struct.unpack_from("<II", data, 12)
    children_offset = 20 + main_content_size

    size: tuple[int, int, int] | None = None
    size_ref: dict[str, tuple[int, int, int] | None] = {"value": size}
    voxels: dict[tuple[int, int, int], int] = {}
    palette: list[tuple[int, int, int, int]] = [(0, 0, 0, 0) for _ in range(256)]

    parse_chunks(
        data,
        children_offset,
        main_children_size,
        lambda chunk_id, content: handle_chunk(chunk_id, content, voxels, palette, path, size_ref),
    )

    size = size_ref["value"]
    if size is None:
        raise ValueError(f"{path} is missing a SIZE chunk.")

    return VoxModel(size=size, voxels=voxels, palette=palette)


def parse_chunks(
    data: bytes, offset: int, total_size: int, handle_chunk_fn
) -> None:
    end = offset + total_size
    while offset < end:
        chunk_id = data[offset : offset + 4].decode("ascii")
        content_size, children_size = struct.unpack_from("<II", data, offset + 4)
        content_start = offset + 12
        content_end = content_start + content_size
        children_start = content_end

        handle_chunk_fn(chunk_id, data[content_start:content_end])
        if children_size > 0:
            parse_chunks(data, children_start, children_size, handle_chunk_fn)

        offset = children_start + children_size


def handle_chunk(
    chunk_id: str,
    content: bytes,
    voxels: dict[tuple[int, int, int], int],
    palette: list[tuple[int, int, int, int]],
    path: Path,
    size_ref: dict[str, tuple[int, int, int] | None],
) -> None:
    if chunk_id == "SIZE":
        size_ref["value"] = struct.unpack_from("<III", content, 0)
        return

    if chunk_id == "XYZI":
        count = struct.unpack_from("<I", content, 0)[0]
        for index in range(count):
            x, y, z, color_index = struct.unpack_from("<BBBB", content, 4 + (index * 4))
            voxels[(x, y, z)] = color_index
        return

    if chunk_id == "RGBA":
        if len(content) != 1024:
            raise ValueError(f"{path} has an invalid RGBA chunk length.")

        for raw_index in range(255):
            start = raw_index * 4
            r, g, b, a = struct.unpack_from("<BBBB", content, start)
            palette[raw_index + 1] = (r, g, b, a)


def project_point(
    point: tuple[float, float, float], half_width: int, half_height: int, elevation: int
) -> tuple[float, float]:
    x, y, z = point
    return ((x - z) * half_width, (x + z) * half_height - (y * elevation))


def shade_color(color: tuple[int, int, int, int], factor: float) -> tuple[int, int, int, int]:
    r, g, b, a = color
    return (
        max(0, min(255, int(round(r * factor)))),
        max(0, min(255, int(round(g * factor)))),
        max(0, min(255, int(round(b * factor)))),
        a,
    )


def dark_outline(color: tuple[int, int, int, int]) -> tuple[int, int, int, int]:
    return shade_color(color, 0.45)


def build_faces(
    model: VoxModel, half_width: int, half_height: int, elevation: int
) -> list[tuple[tuple[int, int, int, int], list[tuple[float, float]], tuple[int, int, int, int]]]:
    occupied = set(model.voxels.keys())
    face_data: list[
        tuple[
            tuple[int, int, int, int],
            list[tuple[float, float]],
            tuple[int, int, int, int],
        ]
    ] = []

    voxel_items = sorted(
        model.voxels.items(),
        key=lambda item: (item[0][0] + item[0][2], item[0][1], item[0][0], item[0][2]),
    )

    for (x, y, z), color_index in voxel_items:
        base_color = model.palette[color_index]

        if (x, y, z + 1) not in occupied:
            face_data.append(
                (
                    (x + z, y, 0, x),
                    project_face((x, y, z), Z_FACE, half_width, half_height, elevation),
                    shade_color(base_color, 0.78),
                )
            )

        if (x + 1, y, z) not in occupied:
            face_data.append(
                (
                    (x + z, y, 1, x),
                    project_face((x, y, z), X_FACE, half_width, half_height, elevation),
                    shade_color(base_color, 0.64),
                )
            )

        if (x, y + 1, z) not in occupied:
            face_data.append(
                (
                    (x + z, y, 2, x),
                    project_face((x, y, z), TOP_FACE, half_width, half_height, elevation),
                    shade_color(base_color, 1.0),
                )
            )

    face_data.sort(key=lambda item: item[0])
    return face_data


def project_face(
    voxel_origin: tuple[int, int, int],
    face_offsets: tuple[tuple[int, int, int], ...],
    half_width: int,
    half_height: int,
    elevation: int,
) -> list[tuple[float, float]]:
    ox, oy, oz = voxel_origin
    polygon: list[tuple[float, float]] = []
    for dx, dy, dz in face_offsets:
        polygon.append(project_point((ox + dx, oy + dy, oz + dz), half_width, half_height, elevation))
    return polygon


def compute_bounds(
    faces: list[tuple[tuple[int, int, int, int], list[tuple[float, float]], tuple[int, int, int, int]]],
    padding: int,
) -> tuple[int, int, int, int]:
    min_x = math.floor(min(point[0] for _, polygon, _ in faces for point in polygon)) - padding
    max_x = math.ceil(max(point[0] for _, polygon, _ in faces for point in polygon)) + padding
    min_y = math.floor(min(point[1] for _, polygon, _ in faces for point in polygon)) - padding
    max_y = math.ceil(max(point[1] for _, polygon, _ in faces for point in polygon)) + padding
    return min_x, min_y, max_x, max_y


def shift_polygon(
    polygon: list[tuple[float, float]], shift_x: int, shift_y: int
) -> list[tuple[float, float]]:
    return [(point[0] + shift_x, point[1] + shift_y) for point in polygon]


def render_preview(model: VoxModel, output_path: Path, scale: int) -> None:
    half_width = scale
    half_height = max(1, scale // 2)
    elevation = scale
    padding = max(12, scale * 3)

    faces = build_faces(model, half_width, half_height, elevation)
    if not faces:
        raise ValueError(f"{output_path.name} has no visible faces to render.")

    min_x, min_y, max_x, max_y = compute_bounds(faces, padding)
    width = max_x - min_x + 1
    height = max_y - min_y + 1
    pixels = bytearray(width * height * 4)

    for _, polygon, color in faces:
        shifted = shift_polygon(polygon, -min_x, -min_y)
        draw_quad(pixels, width, height, shifted, color)
        draw_outline(pixels, width, height, shifted, dark_outline(color))

    output_path.parent.mkdir(parents=True, exist_ok=True)
    write_png(output_path, width, height, pixels)


def draw_quad(
    pixels: bytearray,
    width: int,
    height: int,
    polygon: list[tuple[float, float]],
    color: tuple[int, int, int, int],
) -> None:
    draw_triangle(pixels, width, height, polygon[0], polygon[1], polygon[2], color)
    draw_triangle(pixels, width, height, polygon[0], polygon[2], polygon[3], color)


def draw_triangle(
    pixels: bytearray,
    width: int,
    height: int,
    a: tuple[float, float],
    b: tuple[float, float],
    c: tuple[float, float],
    color: tuple[int, int, int, int],
) -> None:
    min_x = max(0, int(math.floor(min(a[0], b[0], c[0]))))
    max_x = min(width - 1, int(math.ceil(max(a[0], b[0], c[0]))))
    min_y = max(0, int(math.floor(min(a[1], b[1], c[1]))))
    max_y = min(height - 1, int(math.ceil(max(a[1], b[1], c[1]))))

    area = edge_function(a, b, c)
    if area == 0:
        return

    for y in range(min_y, max_y + 1):
        py = y + 0.5
        for x in range(min_x, max_x + 1):
            px = x + 0.5
            point = (px, py)
            w0 = edge_function(b, c, point)
            w1 = edge_function(c, a, point)
            w2 = edge_function(a, b, point)

            if area > 0:
                inside = w0 >= 0 and w1 >= 0 and w2 >= 0
            else:
                inside = w0 <= 0 and w1 <= 0 and w2 <= 0

            if inside:
                set_pixel(pixels, width, x, y, color)


def edge_function(
    a: tuple[float, float], b: tuple[float, float], c: tuple[float, float]
) -> float:
    return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0])


def draw_outline(
    pixels: bytearray,
    width: int,
    height: int,
    polygon: list[tuple[float, float]],
    color: tuple[int, int, int, int],
) -> None:
    for index in range(len(polygon)):
        start = polygon[index]
        end = polygon[(index + 1) % len(polygon)]
        draw_line(pixels, width, height, start, end, color)


def draw_line(
    pixels: bytearray,
    width: int,
    height: int,
    start: tuple[float, float],
    end: tuple[float, float],
    color: tuple[int, int, int, int],
) -> None:
    x0 = int(round(start[0]))
    y0 = int(round(start[1]))
    x1 = int(round(end[0]))
    y1 = int(round(end[1]))

    dx = abs(x1 - x0)
    dy = -abs(y1 - y0)
    sx = 1 if x0 < x1 else -1
    sy = 1 if y0 < y1 else -1
    error = dx + dy

    while True:
        if 0 <= x0 < width and 0 <= y0 < height:
            set_pixel(pixels, width, x0, y0, color)

        if x0 == x1 and y0 == y1:
            break

        doubled_error = error * 2
        if doubled_error >= dy:
            error += dy
            x0 += sx
        if doubled_error <= dx:
            error += dx
            y0 += sy


def set_pixel(
    pixels: bytearray, width: int, x: int, y: int, color: tuple[int, int, int, int]
) -> None:
    offset = ((y * width) + x) * 4
    pixels[offset : offset + 4] = bytes(color)


def write_png(path: Path, width: int, height: int, pixels: bytearray) -> None:
    scanlines = bytearray()
    stride = width * 4
    for row in range(height):
        scanlines.append(0)
        start = row * stride
        scanlines.extend(pixels[start : start + stride])

    compressed = zlib.compress(bytes(scanlines), level=9)

    with path.open("wb") as handle:
        handle.write(b"\x89PNG\r\n\x1a\n")
        handle.write(make_png_chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)))
        handle.write(make_png_chunk(b"IDAT", compressed))
        handle.write(make_png_chunk(b"IEND", b""))


def make_png_chunk(chunk_type: bytes, chunk_data: bytes) -> bytes:
    chunk = struct.pack(">I", len(chunk_data)) + chunk_type + chunk_data
    checksum = zlib.crc32(chunk_type)
    checksum = zlib.crc32(chunk_data, checksum)
    return chunk + struct.pack(">I", checksum & 0xFFFFFFFF)


def main() -> int:
    args = parse_args()

    assets = load_preview_assets(args.manifest, args.source_root, args.output_root)
    if not assets:
        print("No VOX assets were found in the manifest.", file=sys.stderr)
        return 1

    generated = 0
    for asset in assets:
        model = parse_vox_file(asset.source_file)
        render_preview(model, asset.output_file, args.scale)
        generated += 1
        print(f"Generated {asset.output_file} from {asset.source_file.name}")

    print(f"Generated {generated} angled VOX preview PNGs under {args.output_root}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
