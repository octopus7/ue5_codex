# Box Overhead Lift

## Original Intent

Completely replace the previous chest-height interpretation. Manny must start with a box placed on the floor, bend down, grip it, lift it from the floor, bring it up, and raise it overhead.

## Display Names

- English: Box Overhead Lift
- Korean: 상자 머리위로 들기

## Timing

- Frame count: 48
- FPS: 24
- Duration: 2.0 seconds
- Loop: false

## Style Constraints

- Key-pose-based motion data in a separate web data file.
- Manny demo coordinates: X side, Y up, Z forward.
- Ground is around Y=0; the box starts on the floor with its center at half box height.
- Realistic and weighty lift: hips back, spine braced, knees bent, feet planted.
- The box path stays close to the shins and torso before the overhead press.
- Avoid straight-line robotic arms; hands track the box side edges once the grip is established.
- Discard the previous ready-with-box and chest-height lift interpretation.

## Key Poses And Contacts

- Frame 0: Standing over or near the box while it rests visibly on the floor.
- Frame 7: Hip hinge and squat reach toward shoulder-width side grips.
- Frame 12: Deep squat with both hands gripping the side edges while the box remains on the floor.
- Frame 17: Brace and break the box from the floor.
- Frame 23: Pull the box to knee height along a close vertical path.
- Frame 29: Bring the box to chest height with elbows bent and torso braced.
- Frame 35: Leg drive and press the box upward past the face.
- Frame 41: Overhead lockout hold.
- Frame 47: Small controlled settle with the box still overhead.

## Body Requirements

- Feet remain planted with no sliding.
- Knees bend strongly during the reach and floor pull, then extend through the drive.
- Pelvis lowers and shifts back during the squat/hinge, then rises under the load.
- Chest folds forward for the grip, braces during the floor pull, then extends under the overhead box.
- Head looks down during the grip and slightly upward during press/lockout.
- Hands remain on opposite side edges from the grip frame through the settle.

## Props And Environment

- Prop: floor box, approximately 38 x 22 x 28 Manny demo units.
- Initial box center: approximately `[0, 11, 33]`, so the bottom is on the ground.
- Environment: neutral ground plane; no extra environment required.
- Per-frame `box` metadata includes `center`, `size`, `phase`, and `grip`.

## Implementation Files

- `web/manny_box_overhead_lift_keyposes.js`
- `Docs/MotionRequests/box_overhead_lift.md`
