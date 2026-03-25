import json
import os
import pathlib
import shutil
import subprocess
import sys
import tempfile


ALLOWED_VOXEL_RESOLUTIONS = (16, 32, 64, 128, 256)


def emit(payload: dict, exit_code: int) -> int:
    print(json.dumps(payload, ensure_ascii=True), flush=True)
    return exit_code


def load_request() -> dict:
    raw = sys.stdin.read()
    if not raw.strip():
        raise ValueError("Bridge request was empty.")
    return json.loads(raw)


def strip_code_fences(raw: str) -> str:
    text = raw.strip()
    if text.startswith("```"):
        lines = text.splitlines()
        if len(lines) >= 3:
            lines = lines[1:]
            if lines and lines[-1].strip() == "```":
                lines = lines[:-1]
            text = "\n".join(lines).strip()
    return text


def normalize_generation_mode(request: dict) -> str:
    mode = str(request.get("generation_mode", "primitive")).strip().lower()
    if mode == "voxel":
        return "voxel"
    return "primitive"


def normalize_voxel_resolution(request: dict) -> int:
    raw_value = request.get("voxel_resolution", 32)
    try:
        resolution = int(raw_value)
    except (TypeError, ValueError):
        resolution = 32

    if resolution not in ALLOWED_VOXEL_RESOLUTIONS:
        resolution = 32

    return resolution


def build_prompt(request: dict, generation_mode: str, voxel_resolution: int) -> str:
    prompt = request["prompt"].strip()
    asset_name = request.get("asset_name", "").strip()
    locale = request.get("locale", "ko-KR").strip() or "ko-KR"
    max_primitive_count = int(request.get("max_primitive_count", 32))
    voxel_count = voxel_resolution ** 3

    return f"""
You generate shared mesh output JSON for an Unreal Engine prototype mesh builder.

Rules:
- Respond with schema-valid JSON only.
- Top-level fields must be `version`, `mesh_name`, `generation_mode`, `notes`, `primitive_mesh`, and `voxel_grid`.
- Set `generation_mode` to `{generation_mode}`.
- If `generation_mode` is `primitive`, set `primitive_mesh` to an object and `voxel_grid` to null.
- If `generation_mode` is `voxel`, set `voxel_grid` to an object and `primitive_mesh` to null.

Primitive mode rules:
- Use centimeters and set `primitive_mesh.units` to `cm`.
- Prefer `primitive_mesh.pivot` = `base_center` unless the prompt clearly needs otherwise.
- Width is Y span, depth is X span, height is Z span.
- Each primitive transform uses Unreal coordinates: X forward, Y right, Z up.
- `transform.location_cm` is the center of the primitive bounds, not the bottom.
- `transform.rotation_deg` is Euler rotation around the local X, Y, and Z axes in degrees.
- Boxes are centered on their transform.
- Cylinders and cones use local +Z as their height axis before rotation.
- Ramps rise toward local +X before rotation.
- Allowed primitive types: box, plane, cylinder, cone, ramp, stair.
- Every primitive must include all numeric fields: `width`, `depth`, `height`, `radius`, `segments`, `steps`.
- Every primitive must include a `color` object with normalized `r`, `g`, `b`, `a` values in the 0..1 range.
- When a numeric field does not apply to that primitive type, set it to `0`.
- Unless transparency is explicitly requested, set `a` to `1.0`.
- Use between 1 and {max_primitive_count} primitives.
- Keep the shape practical for a blockout/prototype mesh.
- Avoid boolean operations, arbitrary topology, self-intersection, or decorative tiny details.
- For stairs, use total width/depth/height plus integer `steps`.
- For cylinder and cone, keep `segments` modest, usually between 8 and 24.

Voxel mode rules:
- Set `voxel_grid.resolution` to x={voxel_resolution}, y={voxel_resolution}, z={voxel_resolution}.
- Set `voxel_grid.color_hex_stream` to exactly {voxel_count * 6} hexadecimal characters.
- Treat the stream as one 6-digit RGB hex chunk per voxel in x-fastest, then y, then z order.
- Use `000000` for an empty voxel sentinel.
- For filled voxels, use a visible RGB color and keep the palette coherent across adjacent regions.
- Do not emit `primitive_mesh` data when generating voxels.

- If an asset name hint is supplied, use a close readable value for `mesh_name`.
- If the prompt mentions colors, materials, or paint, reflect that in the output colors.
- If the prompt does not specify colors, choose plausible flat prototype colors for primitive mode and readable solid regions for voxel mode.
- Keep the result suitable for a prototype mesh build, not a final art asset.

User locale: {locale}
Requested asset name hint: {asset_name or "(none)"}
Requested voxel resolution: {voxel_resolution if generation_mode == "voxel" else "(primitive mode)"}
Prompt:
{prompt}
""".strip()


def load_schema_template(schema_path: pathlib.Path) -> dict:
    return json.loads(schema_path.read_text(encoding="utf-8"))


def build_output_schema(request: dict, schema_path: pathlib.Path) -> dict:
    generation_mode = normalize_generation_mode(request)
    voxel_resolution = normalize_voxel_resolution(request)
    voxel_count = voxel_resolution ** 3

    schema = load_schema_template(schema_path)
    schema["properties"]["generation_mode"]["const"] = generation_mode

    if generation_mode == "primitive":
        schema["properties"]["primitive_mesh"] = {"$ref": "#/$defs/primitive_mesh"}
        schema["properties"]["voxel_grid"] = {"type": "null"}
    else:
        schema["properties"]["primitive_mesh"] = {"type": "null"}
        schema["properties"]["voxel_grid"] = {"$ref": "#/$defs/voxel_grid"}
        schema["$defs"]["voxel_grid"]["properties"]["resolution"]["properties"]["x"]["const"] = voxel_resolution
        schema["$defs"]["voxel_grid"]["properties"]["resolution"]["properties"]["y"]["const"] = voxel_resolution
        schema["$defs"]["voxel_grid"]["properties"]["resolution"]["properties"]["z"]["const"] = voxel_resolution
        schema["$defs"]["voxel_grid"]["properties"]["color_hex_stream"]["minLength"] = voxel_count * 6
        schema["$defs"]["voxel_grid"]["properties"]["color_hex_stream"]["maxLength"] = voxel_count * 6

    return schema


def main() -> int:
    try:
        request = load_request()
    except Exception as exc:
        return emit({"success": False, "error": f"Invalid bridge request: {exc}"}, 1)

    if not request.get("prompt", "").strip():
        return emit({"success": False, "error": "Prompt is required."}, 1)

    codex_path = shutil.which("codex")
    if not codex_path:
        return emit(
            {
                "success": False,
                "error": "codex CLI was not found on PATH. Install or expose codex-cli first."
            },
            1,
        )

    template_schema_path = pathlib.Path(__file__).with_name("mesh_output_schema.json")
    if not template_schema_path.exists():
        return emit(
            {
                "success": False,
                "error": f"Schema file not found: {template_schema_path}"
            },
            1,
        )

    generation_mode = normalize_generation_mode(request)
    voxel_resolution = normalize_voxel_resolution(request)
    prompt = build_prompt(request, generation_mode, voxel_resolution)

    schema_path = None
    try:
        with tempfile.NamedTemporaryFile("w", suffix=".json", delete=False, encoding="utf-8") as schema_file:
            schema_path = pathlib.Path(schema_file.name)
            schema_payload = build_output_schema(request, template_schema_path)
            schema_file.write(json.dumps(schema_payload, ensure_ascii=False, indent=2))
    except Exception as exc:
        if schema_path is not None:
            try:
                os.unlink(schema_path)
            except OSError:
                pass
        return emit({"success": False, "error": f"Failed to prepare output schema: {exc}"}, 1)

    timeout_seconds = int(request.get("timeout_seconds", 150))
    working_dir = request.get("project_dir") or os.getcwd()
    reasoning_effort = str(request.get("reasoning_effort", "medium")).strip().lower() or "medium"
    if reasoning_effort not in {"medium", "high", "xhigh"}:
        reasoning_effort = "medium"

    with tempfile.NamedTemporaryFile("w", suffix=".json", delete=False, encoding="utf-8") as output_file:
        output_path = output_file.name

    command = [
        codex_path,
        "exec",
        "-c",
        f'model_reasoning_effort="{reasoning_effort}"',
        "--ephemeral",
        "--color",
        "never",
        "--sandbox",
        "read-only",
        "--output-schema",
        str(schema_path),
        "--output-last-message",
        output_path,
        "-",
    ]

    try:
        result = subprocess.run(
            command,
            input=prompt,
            text=True,
            encoding="utf-8",
            errors="replace",
            capture_output=True,
            cwd=working_dir,
            timeout=timeout_seconds,
        )
    except subprocess.TimeoutExpired:
        return emit({"success": False, "error": "codex exec timed out."}, 1)
    except Exception as exc:
        return emit({"success": False, "error": f"Failed to launch codex exec: {exc}"}, 1)
    finally:
        if schema_path is not None:
            try:
                os.unlink(schema_path)
            except OSError:
                pass

    diagnostics = "\n".join(
        part.strip() for part in [result.stdout, result.stderr] if part and part.strip()
    ).strip()

    if result.returncode != 0:
        message = diagnostics or "codex exec returned a non-zero exit code."
        lowered = message.lower()
        if "login" in lowered or "authenticate" in lowered or "sign in" in lowered:
            message = "codex CLI is not authenticated. Run `codex login` first."
        return emit(
            {
                "success": False,
                "error": message,
                "diagnostics": diagnostics,
            },
            result.returncode,
        )

    try:
        raw_last_message = pathlib.Path(output_path).read_text(encoding="utf-8")
    except Exception as exc:
        return emit(
            {
                "success": False,
                "error": f"Failed to read codex output file: {exc}",
                "diagnostics": diagnostics,
            },
            1,
        )
    finally:
        try:
            os.unlink(output_path)
        except OSError:
            pass

    raw_payload = strip_code_fences(raw_last_message)
    try:
        payload_json = json.loads(raw_payload)
    except json.JSONDecodeError as exc:
        return emit(
            {
                "success": False,
                "error": f"codex output was not valid JSON: {exc}",
                "diagnostics": diagnostics,
                "raw_last_message": raw_last_message,
            },
            1,
        )

    return emit(
        {
            "success": True,
            "payload": payload_json,
            "raw_json": json.dumps(payload_json, ensure_ascii=False),
            "raw_last_message": raw_last_message,
            "diagnostics": diagnostics,
        },
        0,
    )


if __name__ == "__main__":
    raise SystemExit(main())
