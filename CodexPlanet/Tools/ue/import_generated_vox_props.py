import json
from pathlib import Path

import unreal


PROJECT_DIR = Path(unreal.Paths.project_dir())
MANIFEST_PATH = PROJECT_DIR / "SourceArt" / "Vox" / "Props" / "manifest.json"
DESTINATION_PATH = "/Game/Props/Vox"


def build_import_task(source_path: str, asset_name: str) -> unreal.AssetImportTask:
    task = unreal.AssetImportTask()
    task.filename = source_path
    task.destination_path = DESTINATION_PATH
    task.destination_name = asset_name
    task.automated = True
    task.replace_existing = True
    task.save = True

    options = unreal.FbxImportUI()
    options.import_mesh = True
    options.import_as_skeletal = False
    options.import_materials = False
    options.import_textures = False
    options.import_animations = False
    options.static_mesh_import_data.combine_meshes = True
    task.options = options
    return task


def main() -> None:
    manifest = json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()

    tasks = [build_import_task(entry["obj_path"], entry["asset_name"]) for entry in manifest]
    asset_tools.import_asset_tasks(tasks)
    unreal.EditorAssetLibrary.save_directory(DESTINATION_PATH, False, True)

    for task in tasks:
        imported_paths = [str(path) for path in task.get_editor_property("imported_object_paths")]
        unreal.log(f"CODEX_IMPORTED::{task.destination_name}::{';'.join(imported_paths)}")


if __name__ == "__main__":
    main()
