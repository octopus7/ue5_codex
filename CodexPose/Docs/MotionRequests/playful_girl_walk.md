# Motion Request: playful_girl_walk

## Original Intent

- User request: Prepare separate key-pose-based motion data for a "playful, girlish, feminine walk" that is distinct from the existing Feminine Walk.

## Display Names

- English: Playful Girl Walk
- Korean: 발랄한 소녀 걸음

## Timing

- Frame count: 48
- FPS: 24
- Duration: 2.0 seconds
- Looping: true
- Timing rationale: The requested 48 frames at 24 fps was used because a two-second loop gives enough room for alternating light steps, small side placement, and a visible bounce without feeling rushed.

## Motion Direction

- Style: Light footsteps, a slightly bouncy rhythm, small side steps, soft pelvis yaw/roll, and lively compact arm swings. The motion should read cheerful and feminine while avoiding exaggerated or sexualized posing.
- Key pose requirements: Alternating support phases, toe-brush lift poses, soft two-foot passing contacts, restrained cross-body step placement, vertical pelvis bounce on lift, and matching upper-body attitude at the loop boundary.
- Contact/foot locking requirements: Planted foot keys keep both ankle and ball positions stable through their stance spans. Lift and toe-brush keys are marked unplanted before the next planted target to avoid visible foot pops during interpolation.
- Body parts that must rotate or translate: Pelvis translates forward and slightly side-to-side with restrained yaw/roll; chest counter-rotates softly; head and neck stay relaxed; arms swing opposite the stepping leg with energetic but compact offsets; knees use per-side bias vectors for soft bounce and readable leg arcs.

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
