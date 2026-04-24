# Floor Pickup

## Original Intent

- User request: Implement the Floor Pickup proposed test motion as key-pose-based external motion data. Manny should stand, squat and hip hinge using knees and pelvis, reach to a floor object, grasp it, lift it close to the body, stand, and settle.

## Display Names

- English: Floor Pickup
- Korean: 바닥 물건 줍기

## Timing

- Frame count: 56
- FPS: 24
- Duration: 2.333 seconds
- Looping: false

## Style Constraints

- Key-pose-based motion data in a separate web data file.
- Manny demo coordinates: X side, Y up, Z forward.
- The pickup should look stable and weight-aware, with planted feet and a close object path.
- Knees and pelvis drive the descent rather than a pure spine fold.
- Hands should reach to the small floor object, establish a grasp, and keep it close while standing.

## Key Pose And Contact Requirements

- Frame 0: Standing over or near a small object on the floor.
- Frame 6: Head and torso look down while weight prepares to drop.
- Frame 12: Hip hinge and knee bend begin.
- Frame 20: Deep squat reach to the floor object.
- Frame 25: Two-hand grasp on the floor object.
- Frame 32: Object lifts to shin height.
- Frame 40: Object stays close to the torso through the lift.
- Frame 49: Manny stands with the object held close.
- Frame 55: Small controlled settle while holding the object.

## Body Requirements

- Both feet remain planted and locked for the full action.
- Knees bend strongly during the descent and extend through the stand.
- Pelvis lowers and shifts back during the hinge, then rises under the object.
- Chest folds forward to reach the object, then recovers upright.
- Head looks down at the object during the reach and returns toward neutral during the stand.
- Hands remain paired around the prop once the grasp is established.

## Props And Environment

- Prop: small floor object.
- Approximate object size: 12 x 8 x 12 Manny demo units.
- Initial object center: `[0, 4, 36]`, putting the bottom on the ground.
- Environment: neutral flat floor only.
- Per-frame `object` metadata includes `center`, `size`, `onFloor`, `held`, and grip target data.

## Implementation Links

- Motion data: `web/manny_floor_pickup_keyposes.js`
- Request note: `Docs/MotionRequests/floor_pickup.md`
- Supporting assets: none

## Notes

- The implementation exposes `window.mannyFloorPickupKeyposes`.
- The data includes generated dense `frames`, filtered `keyposes`, and `keys` as a `frames` alias.
- Integrated into `web/index.html` and `web/manny_viewer.js`; `web_deploy.zip` was rebuilt after the web-facing change.
