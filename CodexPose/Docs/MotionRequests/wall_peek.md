# Motion Request: wall_peek

## Original Intent

- User request: Replace the existing procedural wall peek (`wallPeek`) with an external key-pose based implementation. Prepare the motion data and request record only, without editing `web/index.html`.

## Display Names

- English: Wall Peek
- Korean: 벽빼꼼

## Timing

- Frame count: 24
- FPS: 24
- Looping: true

## Motion Direction

- Style: Natural wall peek with the character leaning back against a fixed-width wall, facing the camera first, then peeking past the wall's right side.
- Key pose requirements: The head leads with a large yaw toward the right side beyond the wall. Neck follows strongly, while chest and pelvis follow later and with smaller rotation. Arms and legs respond progressively to the upper-body twist.
- Contact/foot locking requirements: Both feet remain planted on the ground. Ankle and ball targets are stable across the loop, with only knee bias and body rotation implying weight transfer.
- Body parts that must rotate or translate: Pelvis translates slightly right and back during the peek, chest rotates after the head, neck bridges the lead motion, head reaches the largest yaw, arms counterbalance, and knees bias without breaking foot contact.

## Props And Environment

- Props: None.
- Environment: Fixed-width wall included in the motion data `wall` object. The motion peeks over the wall's right edge.

## Implementation Links

- Page: `web/index.html` consumes the data externally; this request does not modify the page.
- Motion data: `web/manny_wall_peek_keyposes.js`
- Supporting assets: None.

## Notes

- No UE conversion data is included.
- Data is exposed as `window.mannyWallPeekKeyposes`.
