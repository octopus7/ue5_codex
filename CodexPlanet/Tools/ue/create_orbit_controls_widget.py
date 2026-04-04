import unreal


ASSET_DIR = "/Game/UI"
ASSET_NAME = "WBP_PlanetOrbitControls"
ASSET_PATH = f"{ASSET_DIR}/{ASSET_NAME}"


def main() -> None:
    if unreal.EditorAssetLibrary.does_asset_exist(ASSET_PATH):
        unreal.EditorAssetLibrary.delete_asset(ASSET_PATH)

    result_path = unreal.CodexEditorAssetLibrary.create_orbit_controls_widget_blueprint(ASSET_PATH)
    unreal.log(f"CODEX_WIDGET_CREATED::{result_path}")


if __name__ == "__main__":
    main()
