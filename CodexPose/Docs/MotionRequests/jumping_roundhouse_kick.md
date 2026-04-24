# Motion Request: jumping_roundhouse_kick

## Original Intent

- User request: Add a new key-pose motion data file and request note for a realistic heavy JUMPING ROUNDHOUSE KICK mode.

## Display Names

- English: Jumping Roundhouse Kick
- Korean: 점프 돌려차기

## Timing

- Frame count: 36
- FPS: 24
- Duration: 1.5 seconds
- Looping: false
- Timing rationale: The motion uses a compact 36-frame attack phrase so the load, airborne strike, recoil, and landing remain readable without feeling floaty.

## Motion Direction

- Style: Heavy realistic attack with a crouched load, jump takeoff, airborne hip turn and chamber, roundhouse extension, in-air impact, recoil, and grounded landing with clear weight absorption.
- Key pose requirements: Guarded stance, deep crouched load, explosive takeoff, airborne hip-turn chamber, roundhouse extension, impact peak, recoil rechamber, first landing contact, absorbed two-foot landing, and recovered guard.
- Contact/foot locking requirements: Both feet are grounded during the load, the right foot leaves first, the left foot finishes the toe drive, frames 10-24 are fully airborne with no foot contacts, the left foot catches the landing first, and the right foot follows into a two-foot absorbed stance.
- Body parts that must rotate or translate: Pelvis drops during load and landing, rises through the jump, and yaws strongly into the kick; chest and head counter-rotate toward the target; arms stay in guard and counterbalance the turn; right leg chambers then extends; left leg tucks as the airborne counterweight; knee bias fields support bent-knee loading and landing.
- Constraints: Avoid bone length distortion, impossible foot distances, floaty timing, foot contacts during the airborne phase, and exaggerated arm positions.

## Props And Environment

- Props: None.
- Environment: Flat ground only.

## Implementation Links

- Page: `web/index.html`
- Motion data: `web/manny_jumping_roundhouse_kick_keyposes.js`
- Supporting assets: None.

## Notes

- Defines `window.mannyJumpingRoundhouseKickKeyposes`.
- Data exposes `frames`, `keyposes`, and `keys` aliases using the external motion data shape expected by the generic Manny sampler.
- Uses Y up, Z forward, and X side.
- The implementation keeps full key-pose data in a separate web data file and does not edit the page, renderer, styles, timeline controls, or existing motion files.
