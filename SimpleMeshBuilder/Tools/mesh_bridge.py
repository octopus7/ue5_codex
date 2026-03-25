import json
import os
import pathlib
import shutil
import subprocess
import sys
import tempfile


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


def build_prompt(request: dict) -> str:
    prompt = request["prompt"].strip()
    asset_name = request.get("asset_name", "").strip()
    locale = request.get("locale", "ko-KR").strip() or "ko-KR"
    max_primitive_count = int(request.get("max_primitive_count", 32))

    return f"""
You generate Shape DSL JSON for an Unreal Engine prototype mesh builder.

Rules:
- Respond with schema-valid JSON only.
- Units must be centimeters and `units` must be `cm`.
- Prefer `pivot` = `base_center` unless the prompt clearly needs otherwise.
- Width is Y span, depth is X span, height is Z span.
- Allowed primitive types: box, plane, cylinder, cone, ramp, stair.
- Every primitive must include all numeric fields: `width`, `depth`, `height`, `radius`, `segments`, `steps`.
- When a numeric field does not apply to that primitive type, set it to `0`.
- Use between 1 and {max_primitive_count} primitives.
- Keep the shape practical for a blockout/prototype mesh.
- Avoid boolean operations, arbitrary topology, self-intersection, or decorative tiny details.
- For stairs, use total width/depth/height plus integer `steps`.
- For cylinder and cone, keep `segments` modest, usually between 8 and 24.
- If an asset name hint is supplied, use a close readable value for `mesh_name`.

User locale: {locale}
Requested asset name hint: {asset_name or "(none)"}
Prompt:
{prompt}
""".strip()


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

    schema_path = pathlib.Path(__file__).with_name("mesh_dsl_schema.json")
    if not schema_path.exists():
        return emit(
            {
                "success": False,
                "error": f"Schema file not found: {schema_path}"
            },
            1,
        )

    prompt = build_prompt(request)
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
        raw_dsl = pathlib.Path(output_path).read_text(encoding="utf-8").strip()
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

    raw_dsl = strip_code_fences(raw_dsl)
    try:
        dsl_json = json.loads(raw_dsl)
    except json.JSONDecodeError as exc:
        return emit(
            {
                "success": False,
                "error": f"codex output was not valid JSON: {exc}",
                "diagnostics": diagnostics,
                "raw_output": raw_dsl,
            },
            1,
        )

    return emit(
        {
            "success": True,
            "dsl": dsl_json,
            "raw_json": json.dumps(dsl_json, ensure_ascii=False),
            "diagnostics": diagnostics,
        },
        0,
    )


if __name__ == "__main__":
    raise SystemExit(main())
