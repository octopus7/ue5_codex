import argparse
import json
import os
import sys
import time
from pathlib import Path

DEFAULT_PROJECT_NAME = "CodexInven"
DEFAULT_ASSET_PATH = "/Game/UI/WBP_CodexClock"
DEFAULT_WIDGET_NAME = "ClockContainerBorder"
DEFAULT_COLOR = (0.25, 0.14, 0.08, 0.88)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Update a UMG Border brush color through Unreal Python Remote Execution.",
    )
    parser.add_argument(
        "--engine-root",
        default=None,
        help="Optional Unreal Engine root that contains the PythonScriptPlugin.",
    )
    parser.add_argument(
        "--project-name",
        default=DEFAULT_PROJECT_NAME,
        help="Project name used to select the target Unreal Editor node.",
    )
    parser.add_argument(
        "--asset-path",
        default=DEFAULT_ASSET_PATH,
        help="Widget blueprint asset path.",
    )
    parser.add_argument(
        "--widget-name",
        default=DEFAULT_WIDGET_NAME,
        help="Widget name inside the widget tree.",
    )
    parser.add_argument(
        "--color",
        nargs=4,
        type=float,
        metavar=("R", "G", "B", "A"),
        default=DEFAULT_COLOR,
        help="Target LinearColor as four float values.",
    )
    parser.add_argument(
        "--timeout-seconds",
        type=float,
        default=5.0,
        help="How long to wait for a matching Unreal Editor remote node.",
    )
    return parser.parse_args()


def resolve_engine_root(engine_root_arg: str | None) -> Path:
    candidates = []
    if engine_root_arg:
        candidates.append(Path(engine_root_arg))

    env_engine_root = os.environ.get("UE_ENGINE_ROOT")
    if env_engine_root:
        candidates.append(Path(env_engine_root))

    for candidate in candidates:
        plugin_python_dir = candidate / "Plugins" / "Experimental" / "PythonScriptPlugin" / "Content" / "Python"
        if plugin_python_dir.is_dir():
            return candidate

    searched_paths = "\n".join(f"- {candidate}" for candidate in candidates) or "- <none>"
    raise RuntimeError(
        "Unable to locate a valid Unreal Engine root.\n"
        "Pass --engine-root or set UE_ENGINE_ROOT.\n"
        f"Searched:\n{searched_paths}"
    )


def import_remote_execution(engine_root: Path):
    plugin_python_dir = engine_root / "Plugins" / "Experimental" / "PythonScriptPlugin" / "Content" / "Python"
    if not plugin_python_dir.is_dir():
        raise RuntimeError(f"PythonScriptPlugin path not found: {plugin_python_dir}")

    sys.path.insert(0, str(plugin_python_dir))
    import remote_execution  # type: ignore

    remote_execution.set_log_level(40)
    return remote_execution


def find_target_node(remote_exec, project_name: str, timeout_seconds: float) -> dict:
    deadline = time.time() + timeout_seconds
    while time.time() < deadline:
        nodes = remote_exec.remote_nodes
        for node in nodes:
            if node.get("project_name") == project_name:
                return node
        time.sleep(0.25)

    raise RuntimeError(
        f"No Unreal Editor remote node was found for project '{project_name}'. "
        "Make sure UnrealEditor is running and Remote Execution is enabled."
    )


def build_remote_command(asset_path: str, widget_name: str, color: tuple[float, float, float, float]) -> str:
    asset_path_literal = json.dumps(asset_path)
    widget_name_literal = json.dumps(widget_name)
    r, g, b, a = color
    return f"""
import unreal

asset_path = {asset_path_literal}
widget_name = {widget_name_literal}
target_color = unreal.LinearColor({r}, {g}, {b}, {a})
widget_tree_class = unreal.load_class(None, "/Script/UMG.WidgetTree")

bp = unreal.EditorAssetLibrary.load_asset(asset_path)
if not bp:
    raise RuntimeError(f"Failed to load {{asset_path}}")

tree = unreal.find_object(bp, "WidgetTree", widget_tree_class)
if not tree:
    raise RuntimeError(f"WidgetTree not found in {{asset_path}}")

border = unreal.find_object(tree, widget_name, unreal.Border)
if not border:
    raise RuntimeError(f"{{widget_name}} not found in {{asset_path}}")

before = border.get_editor_property("brush_color")
border.modify()
border.set_brush_color(target_color)
after = border.get_editor_property("brush_color")
bp.modify()
unreal.BlueprintEditorLibrary.compile_blueprint(bp)
if not unreal.EditorAssetLibrary.save_loaded_asset(bp, False):
    raise RuntimeError(f"Failed to save {{asset_path}}")

print(
    f"Updated {{asset_path}}:{{widget_name}} "
    f"from ({{before.r:.4f}},{{before.g:.4f}},{{before.b:.4f}},{{before.a:.4f}}) "
    f"to ({{after.r:.4f}},{{after.g:.4f}},{{after.b:.4f}},{{after.a:.4f}})"
)
"""


def main() -> int:
    args = parse_args()
    remote_execution = import_remote_execution(resolve_engine_root(args.engine_root))

    remote_exec = remote_execution.RemoteExecution()
    remote_exec.start()
    try:
        node = find_target_node(remote_exec, args.project_name, args.timeout_seconds)
        remote_exec.open_command_connection(node["node_id"])

        result = remote_exec.run_command(
            build_remote_command(args.asset_path, args.widget_name, tuple(args.color)),
            exec_mode=remote_execution.MODE_EXEC_FILE,
            raise_on_failure=True,
        )

        for log_entry in result.get("output", []):
            output = log_entry.get("output", "").rstrip()
            if output:
                print(output)

        return 0
    finally:
        remote_exec.stop()


if __name__ == "__main__":
    raise SystemExit(main())
