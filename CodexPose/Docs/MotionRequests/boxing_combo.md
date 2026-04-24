# Motion Request: Boxing Combo

## Original Intent

- User request: Implement a key-pose-based Boxing Combo motion: guard, jab, cross with rear-foot pivot, hook with hip/torso rotation, guarded recoil/recover. Non-looping.

## Display Names

- English: Boxing Combo
- Korean: 복싱 3연타

## Timing

- Frame count: 46
- FPS: 24
- Looping: false

## Motion Direction

- Style: Orthodox boxing three-hit combo with compact guard, straight lead jab, rear cross driven by pivoting rear foot, lead hook driven by hip/chest rotation, and hands returning to guard.
- Key pose requirements: Orthodox guard, jab load, jab extension, jab recoil, rear cross drive, full cross extension, hook load, lead hook, hook follow-through, guarded recover, settled guard.
- Contact/foot locking requirements: Both feet remain planted for the combo. Foot metadata identifies the rear ball pivot during the cross and the lead toe pivot during the hook.
- Body parts that must rotate or translate: Pelvis and chest rotate into the cross and hook; head/neck counter-rotate to keep target focus; arms alternate between guard, extension, and recoil.

## Props And Environment

- Props: None.
- Environment: Flat ground, Manny demo coordinate system.

## Implementation Links

- Page: `web/index.html`
- Motion data: `web/manny_boxing_combo_keyposes.js`
- Supporting assets: None.

## Notes

- The data file exposes `window.mannyBoxingComboKeyposes`, id `boxingCombo`, generated `frames`, filtered `keyposes`, and `keys` alias.
