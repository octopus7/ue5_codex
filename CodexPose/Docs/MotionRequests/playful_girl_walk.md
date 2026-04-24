# Motion Request: playful_girl_walk

## Original Intent

- User request: Prepare separate key-pose-based motion data for a "playful, girlish, feminine walk" that is distinct from the existing Feminine Walk.
- Revision request: Make the existing Playful Girl Walk read as bright, bouncy, youthful, and lively instead of slow or dragging. The result should feel like an upbeat playful walk or skip-like walk, not a run.
- Latest revision request: Increase visible pelvis yaw and roll so the motion reads more feminine, while keeping the bouncy/youthful cadence and avoiding slow, exaggerated, or sexualized posing.

## Display Names

- English: Playful Girl Walk
- Korean: 발랄한 소녀 걸음

## Timing

- Frame count: 48
- FPS: 30
- Duration: 1.6 seconds
- Looping: true
- Timing rationale: The revision keeps the existing 48-sample data shape for the generic sampler, but raises playback to 30 fps so the loop has a quicker, brighter cadence. The shorter 1.6-second cycle helps the walk feel lively while preserving readable alternating contacts.

## Motion Direction

- Style: Bright, bouncy, youthful, feminine, and lively. The motion should have a clearer spring through knees and ankles, small hop-like lift after contacts, visible pelvis yaw/roll rhythm, and compact playful side sway while staying tasteful and readable as a walk.
- Key pose requirements: Alternating support phases, toe-brush pickup and landing moments, compact swing-foot lift, bouncy support-up keys, logical planted stance spans, and matching upper-body attitude at the loop boundary.
- Contact/foot locking requirements: Planted foot samples keep ankle and ball targets stable through stance and expose locked/contact-weight data. Swing-foot samples use toe-brush labels near pickup and landing, then air during the lifted part of the step to reduce sliding and popping.
- Body parts that must rotate or translate: Pelvis has a stronger vertical spring, compact side sway, and more visible yaw/roll with subtle secondary hip timing; chest counter-rotates softly; head and neck add small counter-balancing offsets so the silhouette reads feminine without becoming exaggerated; arms swing opposite the stepping leg with elbow pulse and wrist flick so they feel lively without becoming stiff or overhead; knees use per-side bias vectors for springy arcs.
- Constraints: Avoid overhead arms, locked elbows, run-like stride length, or foot sliding. Hands should stay natural around torso/shoulder height with loose wrist and elbow motion.

## Props And Environment

- Props: None.
- Environment: Flat ground only.

## Implementation Links

- Page: `web/index.html`
- Motion data: `web/manny_playful_girl_walk_keyposes.js`
- Supporting assets: None.

## Notes

- Defines `window.mannyPlayfulGirlWalkKeyposes`.
- Data exposes `frames`, `keyposes`, and `keys` aliases so the existing web renderer can consume the same key-pose array with minimal integration work.
- No UE conversion fields are included.
- The revision keeps motion data in the separate web data file and preserves the Manny-compatible offset structure expected by the sampler.
- Latest revision increases procedural pelvis yaw from about +/-5.9 degrees to about +/-9 degrees including secondary timing, pelvis roll from about +/-4 degrees to about +/-7 degrees including secondary timing, and slightly strengthens chest/head counter-balance while preserving 48 frames at 30 fps.
