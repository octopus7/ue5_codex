#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path
import sys


SCRIPT_DIR = Path(__file__).resolve().parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

from common import archive_harness_run, default_run_id, get_paths, resolve_project_root


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Archive Docs/Harness into Docs/HarnessRuns/<run-id>."
    )
    parser.add_argument("--project-root", help="Project root path. Defaults to script autodetect.")
    parser.add_argument("--run-id", help="Archive run id. Defaults to a timestamp.")
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Print planned actions without copying files.",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    project_root = resolve_project_root(args.project_root)
    paths = get_paths(project_root)
    run_id = args.run_id or default_run_id()

    source_dir = paths["harness_live"]
    destination_dir = paths["harness_runs"] / run_id

    if not source_dir.exists():
        raise SystemExit(f"Live harness directory does not exist: {source_dir}")

    if args.dry_run:
        print(f"[dry-run] archive source: {source_dir}")
        print(f"[dry-run] archive destination: {destination_dir}")
        return 0

    archived_dir = archive_harness_run(project_root, source_dir, paths["harness_runs"], run_id)
    print(f"Archived harness to: {archived_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
