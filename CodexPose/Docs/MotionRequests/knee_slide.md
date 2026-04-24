# Knee Slide

## Original Intent

Implement a key-pose-based Knee Slide motion as one of the proposed test motions: run in and load, drop onto both knees, slide forward with the torso leaning back and arms out, slow under friction, plant a hand and foot, then rise and recover. The motion should be non-looping and emphasize knee/floor contact.

## Display Names

- English: Knee Slide
- Korean: 무릎 슬라이드

## Timing

- Frame count: 60
- FPS: 24
- Duration: 2.5 seconds
- Loop: false

## Style Constraints

- Key-pose-based motion data in a separate web data file.
- Use Manny demo coordinates: X side, Y up, Z forward.
- Run-in should transition into a committed knee drop rather than starting already on the floor.
- During the slide, the torso leans back and the arms counterbalance wide.
- Friction slows forward travel before the hand and foot brace.

## Key Poses And Contacts

- Frame 0: Run-in with a left stride and right leg swinging.
- Frame 6: Load stride before the drop.
- Frame 12: Drop commitment with toes beginning to drag.
- Frame 18: Double-knee impact on the floor.
- Frame 24: Fast knee slide with torso back and arms wide.
- Frame 34: Friction slows the slide.
- Frame 42: Left hand plants and right foot threads forward to brace.
- Frame 50: Push up from the slide.
- Frame 59: Recovered standing pose.

## Props And Environment

- Props: None.
- Environment: Smooth floor with medium friction.
- Contact metadata includes both knees, both feet, and hand brace points.

## Implementation Files

- `web/manny_knee_slide_keyposes.js`
- `Docs/MotionRequests/knee_slide.md`

## Notes

- The data object exposes `frameCount`, `fps`, `loop`, `frames`, `keyposes`, and `keys`.
- Per-frame `slide` metadata includes forward velocity, friction, and floor-contact state.
- Knee contact remains dominant from the impact through the slow-slide phase.
