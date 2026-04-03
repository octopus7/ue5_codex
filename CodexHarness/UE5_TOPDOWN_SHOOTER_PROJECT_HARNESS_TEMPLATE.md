# UE5 Top-Down Shooter Project Harness Template

This file is a reusable request template for building a top-down shooter game from the ground up in an Unreal Engine 5 project.

Use this when the project does not yet have core gameplay foundations such as input, player movement, aiming, combat, or enemy behavior, and you want the assistant to treat the work as a multi-phase project instead of a single isolated task.

## How To Use

Copy the request block below, replace the placeholders, and send it as-is.

## Request Template

```md
Read this file first:
<ABSOLUTE_PATH_TO_THIS_MD_FILE>

Then use it as the working project harness for the following UE5 project.

Project path:
<ABSOLUTE_PROJECT_PATH>

Project goal:
Build a top-down shooter from the ground up.

Current project state:
- Input system: <NOT_IMPLEMENTED / PARTIAL / CUSTOM>
- Player pawn and movement: <NOT_IMPLEMENTED / PARTIAL / CUSTOM>
- Camera and aiming: <NOT_IMPLEMENTED / PARTIAL / CUSTOM>
- Weapon and combat loop: <NOT_IMPLEMENTED / PARTIAL / CUSTOM>
- Enemy and AI loop: <NOT_IMPLEMENTED / PARTIAL / CUSTOM>
- HUD and UI: <NOT_IMPLEMENTED / PARTIAL / CUSTOM>
- Match flow or wave flow: <NOT_IMPLEMENTED / PARTIAL / CUSTOM>
- Save, settings, or progression: <NOT_IMPLEMENTED / PARTIAL / CUSTOM>

Target playable outcome:
<DESCRIBE THE FIRST PLAYABLE VERSION YOU WANT>

Gameplay pillars:
- <PILLAR_1>
- <PILLAR_2>
- <PILLAR_3>

Camera and control style:
- Camera model: <FIXED_TOP_DOWN / FOLLOW_CAMERA / ROTATING_CAMERA / OTHER>
- Movement model: <WASD / CLICK_TO_MOVE / TWIN_STICK / OTHER>
- Aim model: <MOUSE_CURSOR / RIGHT_STICK / AUTO_AIM / OTHER>
- Fire model: <HITSCAN / PROJECTILE / BOTH / OTHER>

Platform and engine constraints:
- Engine version: <UE_VERSION>
- Target platform: <PC / CONSOLE / MOBILE / OTHER>
- Network model: <SINGLE_PLAYER / COOP / PVP / TBD>
- Performance target: <FPS OR HARDWARE TARGET>

Implementation rules:
- C++ ownership: <WHAT SHOULD LIVE IN C++>
- Blueprint ownership: <WHAT MAY LIVE IN BLUEPRINT>
- Folder and naming rules: <RULES>
- Plugin rules: <ALLOWED OR FORBIDDEN PLUGINS>
- Compatibility constraints: <SYSTEMS OR APIS THAT MUST STAY STABLE>

Project scope:
- In scope: <LIST>
- Out of scope: <LIST>

Expected project milestones:
1. Foundation
   - Project bootstrapping, map, game mode, player controller, input setup
2. Core locomotion
   - Top-down camera, movement, facing, aim direction
3. Combat loop
   - Fire input, weapon logic, hit detection, damage, death handling
4. Enemy loop
   - Enemy actor, pursuit or attack behavior, spawn flow, combat interaction
5. HUD and game flow
   - Health or ammo UI, wave or mission flow, restart or fail handling
6. Polish pass
   - Feedback, tuning, cleanup, bug fixes, validation

Project execution rules:
- Maintain a project-level plan and update it as milestones are completed.
- Break work into the smallest coherent phases that still produce a working result.
- Before editing files for a phase, explain the exact scope and planned file changes.
- After each phase, report what changed, what was verified, what remains, and the next recommended phase.
- If blocked by missing assets or an ambiguous product decision, stop and ask one concise question.

Current execution boundary:
- Start from milestone: <NUMBER OR NAME>
- Current phase goal: <EXACT GOAL FOR THIS RUN>
- Stop after: <ONE_PHASE / ONE_MILESTONE / SPECIFIC_GOAL>
- Do not continue beyond that boundary unless I explicitly ask.

Non-negotiable constraints:
- <CONSTRAINT_1>
- <CONSTRAINT_2>
- <CONSTRAINT_3>

Verification requirements:
- <BUILD REQUIREMENT>
- <TEST REQUIREMENT>
- <MANUAL PLAYCHECK REQUIREMENT>

Reporting requirements:
- Summarize the plan before substantial edits.
- After changes, report:
  - changed files
  - implemented behavior
  - verification performed
  - open risks or missing decisions
  - next recommended milestone or phase
```

## Optional Phase Slice Template

Use this smaller block when the overall project harness already exists and you only want to define the next execution slice.

```md
Current phase goal:
<EXACT GOAL>

In scope for this phase:
- <ITEM>
- <ITEM>

Out of scope for this phase:
- <ITEM>
- <ITEM>

Constraints:
- <ITEM>
- <ITEM>

Verification:
- <ITEM>
- <ITEM>

Stop after:
<ONE_PHASE / ONE_MILESTONE / SPECIFIC_RESULT>
```

## Notes

- This is a project-scale harness, not a single-task harness.
- The top-level goal stays stable while the current execution boundary narrows the work for each run.
- If the project is still at zero gameplay implementation, the first milestone should usually establish input, controller or pawn ownership, camera, and movement before combat.
- If you need tighter control, pair this file with a separate task harness for the current milestone.
