# Side Kick

## Original Intent

Add a new key-pose motion data file and request note for a realistic heavy side kick mode. The motion should read as a short preparation, weight shift, chamber knee, sharp side extension, strong impact hold, recoil, and heavy recovery.

## Display Names

- English: Side Kick
- Korean: 옆차기

## Timing

- Frame count: 32
- FPS: 24
- Duration: 1.333 seconds
- Loop: false

## Style Constraints

- Key-pose-based motion data.
- Heavy, realistic attack rather than a snappy dance kick.
- Use Manny demo coordinates: X side, Y up, Z forward.
- Keep bone lengths plausible by using reachable ankle and ball targets.
- Keep the support foot planted during chamber, impact, and recoil to avoid sliding.

## Key Poses And Contacts

- Frame 0: Guard-ready stance with both feet grounded.
- Frame 3: Short preparation with a small sink and guard compression.
- Frame 7: Weight shifted onto the left support foot.
- Frame 11: Right knee chamber lifted across the body.
- Frame 15: Sharp right-leg side extension toward negative X.
- Frame 18: Strong impact hold with the right foot still extended.
- Frame 22: Recoil back to chamber.
- Frame 27: Heavy recovery step down.
- Frame 31: Guard recovered for a clean end pose.

## Props And Environment

- Props: None.
- Environment: Neutral ground plane only.

## Implementation Files

- `web/manny_side_kick_keyposes.js`
- `Docs/MotionRequests/side_kick.md`

## Notes

- The left foot is the support foot and remains floor-locked through the loaded portion of the attack.
- The right foot is the kicking foot and uses a sideward ball target during impact so the foot reads as a bladed side kick.
