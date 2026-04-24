# Low Vault

## Original Intent

Implement the Low Vault proposed test motion as external key-pose-based Manny motion data. The motion should show a run-in, both hands planting on a low obstacle, knees tucking, legs clearing sideways/forward, a release, staggered landing, and recovery.

## Display Names

- English: Low Vault
- Korean: 낮은 장애물 볼트

## Timing

- Frame count: 44
- FPS: 24
- Duration: 1.833 seconds
- Loop: false

## Style Constraints

- Key-pose-based motion data generated into dense in-between frames.
- Use Manny demo coordinates: X side, Y up, Z forward.
- Read as a practical low side/forward vault rather than a clean high jump.
- Keep the obstacle low enough for two-hand support and visible leg clearance.
- End with a non-looping staggered runout recovery.

## Key Poses And Contacts

- Frame 0: Run-in stride with left foot planted and right leg recovering.
- Frame 5: Final lowering stride with the right foot pushing off.
- Frame 9: Both hands plant on the obstacle top.
- Frame 13: Knees tuck over the planted hands.
- Frame 18: Hips clear the rail/box while both legs sweep sideways and forward.
- Frame 23: Hands release as the trailing leg clears.
- Frame 29: Lead left foot lands beyond the obstacle.
- Frame 36: Trail right foot sets down into the staggered landing.
- Frame 43: Manny recovers into a forward runout stance.

## Props And Environment

- Props: Low rail or box obstacle, represented as metadata in the motion data.
- Environment: Flat ground plane with the obstacle across the run path at positive Z.

## Implementation Files

- `web/manny_low_vault_keyposes.js`
- `Docs/MotionRequests/low_vault.md`

## Notes

- The data exposes `frames`, `keyposes`, and `keys` for the generic Manny sampler.
- Foot contacts mark run-in, airborne clearance, and staggered landing phases.
- Hand contacts identify the two-hand obstacle support span from plant through clearance.
