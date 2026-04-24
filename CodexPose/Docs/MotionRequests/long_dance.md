# Motion Request: Long Dance

## Original Intent

- User request: Prepare a roughly 10 second long dancing motion as external key-pose based data for the CodexPose web renderer. Do not modify `web/index.html`; create only the owned data and request-note files.

## Display Names

- English: Long Dance
- Korean: 긴 춤

## Timing

- Frame count: 240
- FPS: 24
- Duration: 10 seconds
- Looping: true

## Motion Direction

- Style: Confident character dance loop. The motion should show rhythm through the upper body, pelvis, arms, and feet without being overly sexualized.
- Key pose requirements: Use a long-form sequence with about 8-12 readable sections rather than a single repeated move. The implemented sections are intro groove, left side step, shoulder pop, hip sway, arm sweep, turn accent, low bounce, cross step, clap reach, and recovery loop.
- Contact/foot locking requirements: Account for foot contact. Dance slides and steps are allowed, but planted frames should keep ankle, ball, and heel positions stable enough to avoid unnecessary popping.
- Body parts that must rotate or translate: Pelvis, chest, neck, head, both upper/lower/hand arm segments, legs, knees, ankles, and both feet.

## Props And Environment

- Props: None.
- Environment: Neutral character dance preview space.

## Implementation Links

- Page: `web/index.html` consumes external motion data when wired by the renderer.
- Motion data: `web/manny_long_dance_keyposes.js`
- Supporting assets: None.

## Notes

- Defines `window.mannyLongDanceKeyposes`.
- No UE conversion-only fields are included.
