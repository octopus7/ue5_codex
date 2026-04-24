# Motion Request: roundhouse_kick

## Original Intent

- User request: Add a new key-pose motion data file and request note for a realistic heavy Roundhouse Kick mode.
- Clarified intent: The kick must visibly rotate the body axis through approximately one full turn, not only swing the kicking leg.

## Display Names

- English: Roundhouse Kick
- Korean: 돌려차기

## Timing

- Frame count: 28
- FPS: 24
- Duration: 1.167 seconds
- Looping: false

## Motion Direction

- Style: Heavy, realistic right-leg roundhouse attack with visible body mass, planted support, full-turn hip/body-axis rotation, guarded arms, and a readable recovery.
- Key pose requirements: Guarded stance, loaded pivot, knee chamber, shin whip, impact hold, recoil chamber, step down, and guard recovery.
- Contact/foot locking requirements: Left support foot stays grounded throughout and pivots on the ball. Right kicking foot starts grounded, lifts into chamber, arcs around at mid-height, holds at impact, recoils, then returns to ground.
- Body parts that must rotate or translate: Pelvis and support-foot yaw should use unwrapped values to show roughly 360 degrees of body-axis rotation across the motion. Chest, neck, and head should lag/counter-rotate through the chamber and impact for weight, spotting, and balance. Arms stay in a practical guard/counterbalance shape. Right knee bias follows the chamber and whipping arc without stretching bone lengths.

## Props And Environment

- Props: None.
- Environment: Flat ground only.

## Implementation Links

- Page: Not wired in this request.
- Motion data: `web/manny_roundhouse_kick_keyposes.js`
- Supporting assets: None.

## Notes

- Defines `window.mannyRoundhouseKickKeyposes`.
- Uses Y up, Z forward, and X side.
- Data exposes `frames`, `keyposes`, and `keys` aliases for the external key-pose sampler shape.
- The authored timing emphasizes load and chamber before a short impact hold at frames 14-16, then a controlled recoil and grounded recovery.
- Revision note: The yaw track now advances from the starting guard through a full-turn recovery while preserving the planted left-foot pivot and the right-foot airborne/impact/recovery contact states.
