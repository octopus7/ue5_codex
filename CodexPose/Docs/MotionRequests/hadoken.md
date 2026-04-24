# Motion Request: hadoken

## Original Intent

- User request: Add a new key-pose motion data file and request note for a Hadoken-style energy projectile motion.

## Display Names

- English: Hadoken
- Korean: 파동권

## Timing

- Frame count: 40
- FPS: 24
- Duration: 1.667 seconds
- Looping: false

## Motion Direction

- Style: Grounded fighting-game energy projectile with a readable charge, body coil, two-palm release, impact pulse, recoil, and return to guard.
- Key pose requirements: Grounded fighting stance, short crouched energy gather with hands near waist/chest, torso coil, both palms thrust forward, energy projectile release, impact/pulse hold, recoil/follow-through, and return to guard.
- Contact/foot locking requirements: Both feet stay planted for the full action. Weight shifts from guarded stance into a deeper crouch, drives slightly forward on release, holds through the pulse, then settles back into guard.
- Body parts that must rotate or translate: Pelvis lowers and yaws through the coil, chest counter-rotates and unwinds into release, head tracks forward, both arms converge near center during charge and extend together on the thrust, and both knees bend with per-side knee bias.
- Constraints: Hands should stay within plausible Manny arm reach. Feet should not step, skate, or leave the ground.

## Props And Environment

- Props: Optional energy projectile visual driven by metadata; no physical prop required.
- Environment: Flat ground only.

## Implementation Links

- Page: Not wired in this request.
- Motion data: `web/manny_hadoken_keyposes.js`
- Supporting assets: None.

## Notes

- Defines `window.mannyHadokenKeyposes`.
- Uses Y up, Z forward, and X side.
- Data exposes `frames`, `keyposes`, and `keys` aliases for the external key-pose sampler shape.
- Extra effect timing metadata includes charge start/peak, release frame, projectile travel span, impact hold frames, recoil start, return-to-guard frame, and per-frame energy intensity.
