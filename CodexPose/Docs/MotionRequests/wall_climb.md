# Wall Climb

## Original Intent

Revise the existing Wall Climb motion because the current version reads as climbing with both feet on the wall. The intended action is a parkour-style high-wall mantle: run/jump toward a tall wall, catch the top edge with both hands, hook one foot/leg over the ledge, pull the body sideways over the wall, then stand up.

## Display Names

- Wall Climb

## Timing

- Frame count: 24
- FPS: 24
- Loop: false

## Style Constraints

- Key-pose-based motion data.
- The wall must read as tall enough that a jump and climb are required.
- Avoid the two-feet-wall-climb feeling.
- Do not show both feet marching up the vertical wall.
- Only one leg should hook/load during the pull-over; the other trails, then steps through onto the top.
- Keep Manny-compatible target distances and rely on renderer-side contact clamping only as a fallback.

## Key Poses And Contacts

- Approach run with ground contacts.
- Low compression and takeoff into an airborne reach.
- Both hands catch the top edge using `top` hand contacts.
- Loaded hang below the ledge.
- Left leg/knee hooks over the wall top using a single `top` foot contact.
- Torso yaws/rolls sideways over the ledge during the pull.
- Trailing right leg remains airborne until the step-through phase.
- Both feet settle on the top/far side before the character rises to stand.

## Props And Environment

- Tall wall data is defined in the motion file as `wallClimbTallMantleWall`.
- Top edge contacts use explicit `top` labels for hands and hooked/standing feet.

## Implementation Files

- `web/manny_wall_climb_keyposes.js`
- `Docs/MotionRequests/wall_climb.md`
