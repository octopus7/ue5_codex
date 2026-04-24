# Ladder Climb

## Original Intent

Implement a key-pose-based Ladder Climb motion as one of the proposed test motions, with alternating hand and foot contacts on ladder rungs, vertical body translation, and a repeatable climbing rhythm. The user allowed a loopable version if the first and last frames are compatible. Include ladder prop and environment metadata.

## Display Names

- English: Ladder Climb
- Korean: 사다리 오르기

## Timing

- Frame count: 48
- FPS: 24
- Duration: 2.0 seconds
- Loop: true

## Style Constraints

- Key-pose-based motion data in a separate web data file.
- Use Manny demo coordinates: X side, Y up, Z forward.
- Keep the body pitched toward the ladder while the pelvis rises one rung over the cycle.
- Maintain contact-aware metadata for both hands and both feet.
- The final key pose matches the opening contact pattern one rung higher for root-motion-aware looping.

## Key Poses And Contacts

- Frame 0: Left hand and left foot high, right hand and right foot lower, all on rungs.
- Frame 6: Right hand releases and reaches to the next rung while the left side anchors.
- Frame 12: Right hand catches the higher rung.
- Frame 18: Left foot steps upward while the right foot supports.
- Frame 24: Left foot loads on the higher rung.
- Frame 30: Left hand releases and reaches to the next rung.
- Frame 36: Left hand catches high.
- Frame 42: Right foot steps upward.
- Frame 47: Cycle set repeats the opening pattern one rung higher.

## Props And Environment

- Prop: Straight ladder metadata with rung positions, rung spacing, side rails, and ladder plane Z.
- Environment: Vertical ladder rungs in front of Manny; no additional scene assets required in this file.

## Implementation Files

- `web/manny_ladder_climb_keyposes.js`
- `Docs/MotionRequests/ladder_climb.md`

## Notes

- The data object exposes `frameCount`, `fps`, `loop`, `frames`, `keyposes`, and `keys`.
- Per-frame contacts include rung indices and target points for hands, feet, and balls of the feet.
- Root motion metadata declares one rung of upward travel per cycle.
