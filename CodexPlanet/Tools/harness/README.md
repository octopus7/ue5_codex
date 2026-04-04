# Harness Tools

## Scripts

- `reset_harness.py`
  - reset `Docs/Harness` from `Docs/HarnessSeed`
- `archive_harness.py`
  - archive `Docs/Harness` into `Docs/HarnessRuns/<run-id>`

## Examples

```bash
python Tools/harness/reset_harness.py --force
python Tools/harness/archive_harness.py --run-id baseline
```
