# Motion Request: Stumble Recovery

## Original Intent

- User request: Implement a key-pose-based Stumble Recovery motion: normal stance, foot misstep/drag, torso lurch, arms counterbalance, recovery step, and regain stable stance. Emphasize center-of-mass correction and no foot sliding during planted contacts.

## Display Names

- English: Stumble Recovery
- Korean: 비틀거림 회복

## Timing

- Frame count: 42
- FPS: 24
- Looping: false

## Motion Direction

- Style: Physical balance recovery with a right-foot toe catch, forward/right torso lurch, wide arm counterbalance, right recovery catch step, left recenter step, and stable final stance.
- Key pose requirements: Stable stance, toe catch, maximum lurch, recovery step search, right-foot catch, center-of-mass recenter, left-foot replant, settle, recovered stance.
- Contact/foot locking requirements: Locked planted feet keep fixed ankle and ball coordinates. The dragged right toe is marked as toe drag rather than planted. The right foot locks on the recovery landing; the left foot releases before its recenter step and locks after replant.
- Body parts that must rotate or translate: Pelvis translates forward/right during the stumble, then returns over the support base. Chest, neck, and head pitch into the lurch and counter-rotate during recovery. Arms swing wide for counterbalance.

## Props And Environment

- Props: None.
- Environment: Flat ground, Manny demo coordinate system.

## Implementation Links

- Page: `web/index.html`
- Motion data: `web/manny_stumble_recovery_keyposes.js`
- Supporting assets: None.

## Notes

- The data file exposes `window.mannyStumbleRecoveryKeyposes`, id `stumbleRecovery`, generated `frames`, filtered `keyposes`, and `keys` alias.
