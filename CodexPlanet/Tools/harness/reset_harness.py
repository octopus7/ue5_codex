#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path
import sys


SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

from common import (
    archive_harness_run,
    copy_tree,
    default_run_id,
    get_paths,
    relative_file_list,
    remove_tree,
    resolve_project_root,
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Reset Docs/Harness from Docs/HarnessSeed."
    )
    parser.add_argument("--project-root", help="Project root path. Defaults to script autodetect.")
    parser.add_argument(
        "--force",
        action="store_true",
        help="Allow deleting the existing live harness before copying the seed.",
    )
    parser.add_argument(
        "--archive-existing",
        action="store_true",
        help="Archive the current live harness before resetting it.",
    )
    parser.add_argument(
        "--archive-run-id",
        help="Run id to use with --archive-existing. Defaults to <timestamp>-pre-reset.",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Print planned actions without modifying files.",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    project_root = resolve_project_root(args.project_root)
    paths = get_paths(project_root)

    seed_dir = paths["harness_seed"]
    live_dir = paths["harness_live"]

    if not seed_dir.exists():
        raise SystemExit(f"Harness seed directory does not exist: {seed_dir}")

    live_has_files = live_dir.exists() and bool(relative_file_list(live_dir))
    if live_has_files and not args.force and not args.archive_existing:
        raise SystemExit(
            "Refusing to reset a non-empty live harness without --force or --archive-existing."
        )

    archive_run_id = args.archive_run_id or f"{default_run_id()}-pre-reset"

    if args.dry_run:
        if live_has_files and args.archive_existing:
            print(f"[dry-run] archive current harness to: {paths['harness_runs'] / archive_run_id}")
        if live_dir.exists():
            print(f"[dry-run] remove live harness: {live_dir}")
        print(f"[dry-run] copy seed harness from {seed_dir} to {live_dir}")
        return 0

    if live_has_files and args.archive_existing:
        archived_dir = archive_harness_run(
            project_root,
            live_dir,
            paths["harness_runs"],
            archive_run_id,
        )
        print(f"Archived existing harness to: {archived_dir}")

    if live_dir.exists():
        remove_tree(live_dir)

    live_dir.parent.mkdir(parents=True, exist_ok=True)
    copy_tree(seed_dir, live_dir)
    print(f"Reset harness from seed: {seed_dir} -> {live_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
