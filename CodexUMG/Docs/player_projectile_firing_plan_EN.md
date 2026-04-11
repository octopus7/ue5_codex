# Player Projectile Firing Implementation Plan

## Document Purpose
- This is a top-level planning document for designing and directing the feature where the player fires a projectile.
- It does not lock in the project's current implementation state in advance. It only defines flexible criteria that can apply to different structures.
- The highest priority is reducing lead time, and work that can be parallelized should be split by agent whenever possible.

## Core Principles
- The projectile class must be created as a real Blueprint Asset.
- References to the projectile class are stored in a `DA_`-family Data Asset.
- That Data Asset is wired and served through `GameInstance`.
- Player-related classes obtain the reference through `GameInstance` or an access point exposed through it.
- Player classes must not hold or wire a specific projectile Blueprint directly.
- The projectile Blueprint must include a Static Mesh by default and be visible in the world immediately after spawn.
- The projectile's Static Mesh must remain replaceable later from the `BP_Projectile_*` editor setup.
- Implementation agents must not evaluate their own output.
- Artifact evaluation must always be performed by a separate evaluation-only agent.

## Target State
- When a firing request occurs on the player side, player-related logic should be able to fire without directly knowing the projectile Blueprint.
- Projectile type, default settings, and extension points required for firing must be swappable through Data Assets.
- Input handling should reuse the existing structure first, and only add the minimum scope of new input assets when reuse is impossible.
- The structure should remain extensible for single shot, autofire, charge shots, elemental rounds, and different projectiles per weapon.

## Input Handling Branch Rules

### 1. Reuse the existing firing flow first
- If a firing-related flow is already connected to the player, follow that approach first.
- In that case, do not force in a new input system. First review a direction where only the projectile data reference structure is cleaned up behind the existing firing entry point.

### 2. No existing firing flow, but an existing `IMC_` exists
- If no firing-related flow is connected to the player, add a firing `IA_` to the existing `IMC_` that the player is already using.
- Add the new input asset with the smallest possible change inside the current input system, and do not create a separate parallel input structure.
- Follow project naming rules, but from the document's perspective `IA_Fire` or an equivalent name is recommended.
- If a new firing action is added, fix its default mapping to `Left Mouse Button`.

### 3. No trace of an `IMC_` connection at all
- If the player is not connected to any `IMC_`, do not start this task.
- In that case, require the user to implement a minimum level of `Enhanced Input` first.
- The minimum expected prerequisite is roughly:
  - a path exists that connects an `IMC_` to the player or equivalent input receiver
  - at least one `IA_` is connected to a real input event
  - a basic input pipeline exists that can later accept firing input

## Recommended Structure

### 1. Asset structure
- `BP_Projectile_*`
  - a real spawnable projectile Blueprint Asset
  - includes a Static Mesh Component by default
  - has a default mesh that is recognizable in the world on spawn
  - later mesh swaps in the editor must not break the firing structure itself
- `DA_Projectile_*` or a higher-level `DA_` that contains it
  - projectile class reference
  - extension fields such as speed, lifetime, effects, collision policy, or damage policy if needed
- `GameInstance`
  - decides which `DA_` to use
  - exposes an entry point accessible by player-related systems
- `IMC_*`
  - the base input mapping context that reuses the existing input structure
- `IA_Fire` or an equivalent firing action asset
  - only becomes a candidate if no existing firing flow exists
  - if created, its default input mapping uses `Left Mouse Button`

### 2. Reference flow
1. A firing request occurs through an existing firing flow or input action.
2. Player-related classes pass only the firing intent and do not reference the projectile BP directly.
3. The request-handling point queries the required projectile definition from `DA_` through `GameInstance`.
4. Spawn the projectile from the resolved Blueprint Class.
5. Apply initial parameters and default visibility after spawn.

### 3. Responsibility split
- Player classes
  - pass the firing intent
  - may own the entry point that decides whether the existing firing flow is reused
  - direct projectile class references are prohibited
- Input layer
  - either reuses the existing firing flow or adds `IA_` to the existing `IMC_`
  - checks whether the "stop work because no input system exists" condition is met
- Data access layer
  - provides the currently active `DA_` through `GameInstance`
- Firing layer
  - applies spawn position, direction, and initial properties
- Projectile asset layer
  - handles real visuals, collision, movement, and on-hit behavior

## Mandatory Requirements
- A real spawnable `BP_Projectile_*` asset must exist.
- `BP_Projectile_*` must include a default Static Mesh.
- That Static Mesh must be recognizable by the player immediately on spawn.
- The mesh asset must remain replaceable later in the `BP_Projectile_*` editor.
- The `DA_` must reference the projectile Blueprint Class.
- `GameInstance` must wire that `DA_` and expose an access path to player-related classes.
- Player-related classes must not reference the projectile Blueprint directly.
- Input handling must follow this priority:
  - reuse the existing firing flow
  - if none exists, add `IA_` to the existing `IMC_`
  - if no `IMC_` exists at all, forbid starting the task and ask the user to implement minimum `Enhanced Input` first
- If no existing firing flow exists and `IA_Fire` must be added, it must be wired into the existing `IMC_` with a `Left Mouse Button` mapping.

## Proposed Scope

### Required scope
- create a real projectile Blueprint Asset
- add a default Static Mesh to the projectile Blueprint
- wire the projectile class reference into `DA_`
- wire the `DA_` into `GameInstance`
- organize the structure so player-related classes access the data through `GameInstance`
- remove or prohibit direct player-side references to the projectile Blueprint
- apply the input-branching rules
- complete one baseline firing flow

### Optional scope
- switching across multiple projectile types
- extending data-driven firing parameters
- animation, effects, and sound integration
- server authority, prediction, and resync policy
- debug firing logs and visualization

## Agent Operating Model For Reducing Lead Time

### Base policy
- Aggressively delegate work that can run in parallel to agents.
- Minimize dependencies between agents. Agree quickly on interfaces and artifact formats first, then work simultaneously.
- Each agent is responsible only for its own artifacts and does not make the final quality judgment.

### Recommended agent split

#### Agent A: projectile asset owner
- Goals
  - define the creation standard for `BP_Projectile_*`
  - define the structure that includes a default Static Mesh
  - define the configuration standard that keeps mesh replacement possible in the editor
- Main artifacts
  - projectile BP naming rules
  - default component layout
  - default mesh visibility criteria

#### Agent B: data assets and `GameInstance`
- Goals
  - define the structure of `DA_Projectile_*` or the higher-level `DA_`
  - define how `GameInstance` wires and exposes `DA_`
- Main artifacts
  - initial Data Asset fields
  - `GameInstance -> DA_ -> Projectile Class` access flow
  - initialization timing, access function shape, and failure handling rules

#### Agent C: input integration owner
- Goals
  - organize the firing input rules as a decision tree
- Main artifacts
  - criteria for reusing the existing firing flow
  - minimum work standard for adding `IA_` to an existing `IMC_`
  - a draft user-facing message for stopping work when no `IMC_` trace exists

#### Agent D: player firing request owner
- Goals
  - organize a request path where player-related classes do not hold the projectile BP directly
- Main artifacts
  - firing-request interface or call flow
  - a way to minimize responsibility inside player classes

#### Agent E: spawn and initialization owner
- Goals
  - organize the minimum procedures needed when the projectile actually spawns
- Main artifacts
  - rules for spawn position, direction, owner, instigator, and initial parameters
  - criteria for confirming that the default mesh is visible to the user
  - safe fallback rules on failure

#### Agent F: extension-point owner
- Goals
  - define options so the data structure does not break when the feature expands later
- Main artifacts
  - support points for elemental rounds, weapon-specific projectiles, charge shots, and multishot
  - extension hooks for networking, effects, and sound

#### Agent G: document integration owner
- Goals
  - integrate artifacts from all agents into one executable work document
- Main artifacts
  - final implementation checklist
  - ordered dependencies and parallel work sections

#### Agent H: evaluation-only owner
- Goals
  - independently review the output produced by the other agents
- Main artifacts
  - checks for structural violations
  - evaluation of whether parallelization was sufficient from a lead-time perspective
  - confirmation that the "no direct reference from player classes" rule is preserved
  - confirmation that the input-branching rules are complete
  - confirmation that Static Mesh visibility and replaceability requirements are covered
- Note
  - implementation agents do not evaluate their own work
  - evaluation output is only used as input for change requests; the evaluation agent does not also implement the fixes

## Recommended Work Order

### First parallel section
- Agent A: define projectile asset structure
- Agent B: define `DA_` and `GameInstance` wiring
- Agent C: define input-branching rules

### Second parallel section
- Agent D: define the player firing-request structure
- Agent E: organize spawn and initialization rules
- Agent F: organize extension points

### Integration section
- Agent G: integrate all artifacts

### Evaluation section
- Agent H: perform independent evaluation

## Minimum Interface Agreement
- Player-related classes only pass the fact that "a firing request has occurred."
- The input layer first decides whether the existing firing flow can be reused.
- If no existing firing flow exists, only extend by adding a firing `IA_` to the existing `IMC_`.
- If `IA_Fire` is added because no existing firing flow exists, map it to `Left Mouse Button`.
- If there is no trace of an `IMC_` connection, stop implementation and first require the user to provide a minimum `Enhanced Input` base.
- `GameInstance` must provide the currently valid `DA_` or an equivalent data access point.
- The `DA_` must be able to return a real projectile Blueprint Class.
- The firing layer spawns from the returned class.
- The spawned projectile must be immediately recognizable through its default Static Mesh.

## Prohibited
- putting a specific `BP_Projectile_*` directly into a player-class variable
- hardcoding projectile type inside a player class without going through `DA_`
- duplicating a separate input system when an existing firing path already exists
- introducing a new `IMC_` in parallel when an existing `IMC_` already exists
- starting arbitrary implementation when there is no trace of any `IMC_` connection
- when a new firing action must be added because no existing firing path exists, assigning an arbitrary default key such as `Space Bar` instead of `Left Mouse Button`
- spawning a projectile in a state the user cannot recognize
- locking the projectile mesh in a way that makes BP-editor replacement difficult
- letting the implementation agent also handle the final evaluation of its own artifacts
- unnecessarily concentrating parallelizable work into one agent and increasing lead time

## Checklist
- Has a real spawnable projectile Blueprint Asset been defined
- Does the projectile Blueprint include a default Static Mesh
- Is the projectile recognizable to the user immediately after spawn
- Can the mesh be replaced later in the `BP_Projectile_*` editor
- Does the `DA_` reference the projectile Blueprint Class
- Does `GameInstance` wire the `DA_` and make it accessible
- Do player-related classes avoid referencing the projectile Blueprint directly
- Does the plan first decide whether to reuse the existing firing flow
- If the existing firing flow is absent, does the plan reflect the rule of adding `IA_` to the existing `IMC_`
- If the existing firing flow is absent, is the default `IA_Fire` mapping fixed to `Left Mouse Button`
- If there is no trace of an `IMC_` connection, does the plan reflect the rule to stop work and request user action
- Are firing requests and actual spawning responsibilities separated
- Are items that can run in parallel actually split across agents
- Is the final evaluation planned as work for a separate evaluation agent

## Notes
- Actual class names, function names, asset names, and folder structures can be adjusted to fit project rules.
- This document is not a detailed implementation spec. It is a planning document for work direction and structural alignment.
- The detailed design can be compressed or expanded based on this document, but the principles below must remain:
  - projectile references flow through `DA_` and `GameInstance`
  - player classes do not wire the projectile Blueprint directly
  - input handling reuses the existing structure first
  - if there is no input foundation, ask the user to implement a minimum `Enhanced Input` base first
