# Multi-Plan Batch Execution Timeline Template

## Document Purpose
- This is the official chronological log template for recording the actual execution times of multi-plan batch work with timestamps.
- If parallel agents are involved, the main agent collects each report and records it into one unified timeline.
- When real work begins, copy this file to `Docs/multi_plan_batch_execution_timeline_EN.md` and use it from there.

## Logging Rules
- All records use the `YYYY-MM-DD HH:mm:ss KST` format.
- The minimum events to record are start, completion, blocked, unblocked, execution-order rearrangement, handoff, merge, and final verification.
- Times for acquiring and releasing shared files, shared Blueprints, shared assets, shared `Commandlet`s, and shared editor modules are also recorded.
- Parallel agents report status to the main agent, and the main agent reflects those updates in this document.
- Right after copying, begin replacing the rows with real execution timestamps starting from the first line.

## Timeline Log
| Time | Recorder | Phase/Track | Event | Details | Follow-up |
| --- | --- | --- | --- | --- | --- |
| `<YYYY-MM-DD HH:mm:ss KST>` | Main agent | Operations setup | Working copy created | Copied the template into a working file and recorded the initial state | Record actual work events in sequence afterward |
