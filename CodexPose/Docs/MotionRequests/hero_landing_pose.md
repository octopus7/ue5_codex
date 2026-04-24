# Hero Landing Pose

- Request: add the imagegen-derived hero landing as a dropdown item.
- Type: single-frame static pose.
- UI label: show it as a pose with an icon-style prefix, e.g. `◆ Pose: Hero Landing` / `◆ 포즈: 히어로 랜딩`.
- Source assets:
  - `imagegen/generated/hero_landing_imagegen_reference.png`
  - `imagegen/generated/hero_landing_pose_extracted.png`
  - `imagegen/generated/hero_landing_pose_overlay.png`
  - `imagegen/generated/hero_landing_pose_points.json`

## Motion Intent

Manny holds a dramatic hero landing: pelvis low, torso pitched forward, one hand planted near the ground, one leg forward, one leg swept back, and head/chest lifted enough that the front of the face remains readable.

## Implementation Notes

- Data file: `web/manny_hero_landing_pose_keyposes.js`
- Frame count: 1
- FPS: 1
- Loop: false
- Feet and the planted hand are treated as contacts.
- This is intentionally categorized as a pose rather than an animation loop.
