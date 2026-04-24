# Door Push

## Original Intent

- User request: Implement the Door Push proposed test motion as key-pose-based external motion data. Manny should approach and brace, place one hand on a heavy door, lean in, open the door while stepping through, release the hand, and recover.

## Display Names

- English: Door Push
- Korean: 문 밀고 들어가기

## Timing

- Frame count: 56
- FPS: 24
- Duration: 2.333 seconds
- Looping: false

## Style Constraints

- Key-pose-based motion data in a separate web data file.
- Manny demo coordinates: X side, Y up, Z forward.
- The motion should read as a heavy hinged-door push, not a light tap.
- Body weight shifts into the door through a planted brace foot, forward pelvis lean, and chest drive.
- One hand, the right hand in this implementation, contacts and drives the door.
- The door remains open at the end, so the action does not loop.

## Key Pose And Contact Requirements

- Frame 0: Approach-ready walk stance behind the closed door.
- Frame 6: Last approach step with the left foot swinging forward.
- Frame 10: Left foot sets as the threshold brace while the right hand reaches toward the door.
- Frame 16: Right palm contacts the closed heavy door.
- Frame 24: Shoulder and body drive break the door open.
- Frame 32: Manny steps through the opening while the right hand guides the moving door.
- Frame 40: Right foot lands inside as the door opens wide.
- Frame 48: Right hand releases and the left foot steps through.
- Frame 55: Manny recovers inside the room with the door open.

## Body Requirements

- Left foot locks as the brace foot during the main push.
- Right foot steps through first after the door breaks open.
- Pelvis lowers and pitches forward during the push.
- Chest follows the push with visible forward lean.
- Head and neck look toward the door and then into the room.
- Right hand contact weight rises, drives the door, then drops to free on release.

## Props And Environment

- Prop: heavy hinged door.
- Door metadata: width 68, height 116, thickness 4 Manny demo units.
- Hinge: approximately `[-34, 58, 42]`.
- Door angle: starts at 0 degrees and opens to about 86 degrees.
- Environment: simple doorway or threshold; no additional wall geometry is required by the sampler.

## Implementation Links

- Motion data: `web/manny_door_push_keyposes.js`
- Request note: `Docs/MotionRequests/door_push.md`
- Supporting assets: none

## Notes

- The implementation exposes `window.mannyDoorPushKeyposes`.
- The data includes generated dense `frames`, filtered `keyposes`, and `keys` as a `frames` alias.
- Integrated into `web/index.html` and `web/manny_viewer.js`; `web_deploy.zip` was rebuilt after the web-facing change.
