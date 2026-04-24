# Forward Roll

## Original Intent

Implement the Forward Roll proposed test motion as external key-pose-based Manny motion data. The motion should drop from a crouched/landing posture, contact the floor on one shoulder, curl the spine through the roll, let the hips pass over, return the feet under the body, and stand/recover.

## Display Names

- English: Forward Roll
- Korean: 앞구르기 낙법

## Timing

- Frame count: 52
- FPS: 24
- Duration: 2.167 seconds
- Loop: false

## Style Constraints

- Key-pose-based motion data generated into dense in-between frames.
- Use Manny demo coordinates: X side, Y up, Z forward.
- Non-looping recovery into a stable upright stance.
- Favor a shoulder roll path so the head does not take direct floor contact.
- Keep the body compact through the inverted tuck and replant both feet before standing.

## Key Poses And Contacts

- Frame 0: Crouched landing/drop entry with both feet planted.
- Frame 6: Hands lower and one shoulder leads toward the floor.
- Frame 10: Left shoulder contacts the floor.
- Frame 15: Contact rolls from shoulder to upper back.
- Frame 21: Hips pass over in a compact inverted tuck.
- Frame 27: Rounded back carries the roll as feet descend.
- Frame 33: Both feet return under the body.
- Frame 41: Crouched stand/recover.
- Frame 51: Upright recovered stance.

## Props And Environment

- Props: None.
- Environment: Flat padded floor or mat.

## Implementation Files

- `web/manny_forward_roll_keyposes.js`
- `Docs/MotionRequests/forward_roll.md`

## Notes

- The data exposes `frames`, `keyposes`, and `keys` for the generic Manny sampler.
- Contact metadata includes foot contacts plus left-shoulder, upper-back, back, and floor roll-path notes.
- The motion is intentionally non-looping.
