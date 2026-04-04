#!/usr/bin/env python3

from __future__ import annotations

from datetime import datetime
from hashlib import sha256
import json
from pathlib import Path
import shutil
import subprocess
from typing import Any


def default_project_root() -> Path:
    return Path(__file__).resolve().parents[2]


def resolve_project_root(project_root: str | None) -> Path:
    root = Path(project_root).resolve() if project_root else default_project_root()
    if not root.exists():
        raise SystemExit(f"Project root does not exist: {root}")
    return root


def now_local_iso() -> str:
    return datetime.now().astimezone().isoformat(timespec="seconds")


def default_run_id() -> str:
    return datetime.now().astimezone().strftime("%Y%m%d-%H%M%S")


def get_paths(project_root: Path) -> dict[str, Path]:
    docs_root = project_root / "Docs"
    return {
        "docs_root": docs_root,
        "harness_seed": docs_root / "HarnessSeed",
        "harness_live": docs_root / "Harness",
        "harness_runs": docs_root / "HarnessRuns",
    }


def relative_file_list(root: Path) -> list[str]:
    if not root.exists():
        return []
    return sorted(
        str(path.relative_to(root)).replace("\\", "/")
        for path in root.rglob("*")
        if path.is_file()
    )


def file_sha256(path: Path) -> str:
    digest = sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def tree_manifest(root: Path) -> list[dict[str, str]]:
    if not root.exists():
        return []
    manifest: list[dict[str, str]] = []
    for relative_path in relative_file_list(root):
        full_path = root / relative_path
        manifest.append(
            {
                "path": relative_path,
                "sha256": file_sha256(full_path),
            }
        )
    return manifest


def maybe_git_head(project_root: Path) -> str | None:
    try:
        result = subprocess.run(
            ["git", "-C", str(project_root), "rev-parse", "HEAD"],
            check=True,
            capture_output=True,
            text=True,
        )
    except (FileNotFoundError, subprocess.CalledProcessError):
        return None
    return result.stdout.strip() or None


def remove_tree(path: Path) -> None:
    if path.exists():
        shutil.rmtree(path)


def copy_tree(src: Path, dst: Path) -> None:
    shutil.copytree(src, dst)


def write_json(path: Path, payload: dict[str, Any]) -> None:
    path.write_text(
        json.dumps(payload, ensure_ascii=True, indent=2) + "\n",
        encoding="utf-8",
    )


def archive_harness_run(
    project_root: Path,
    source_dir: Path,
    archive_runs_root: Path,
    run_id: str,
) -> Path:
    if not source_dir.exists():
        raise SystemExit(f"Source harness directory does not exist: {source_dir}")

    destination_dir = archive_runs_root / run_id
    archived_harness_dir = destination_dir / "Harness"
    seed_dir = get_paths(project_root)["harness_seed"]

    if destination_dir.exists():
        raise SystemExit(f"Archive run already exists: {destination_dir}")

    archive_runs_root.mkdir(parents=True, exist_ok=True)
    copy_tree(source_dir, archived_harness_dir)

    manifest = {
        "run_id": run_id,
        "archived_at": now_local_iso(),
        "project_root": str(project_root),
        "source_dir": str(source_dir),
        "git_head": maybe_git_head(project_root),
        "seed_dir": str(seed_dir),
        "archived_files": tree_manifest(archived_harness_dir),
        "seed_files": tree_manifest(seed_dir),
    }
    write_json(destination_dir / "manifest.json", manifest)
    return destination_dir
