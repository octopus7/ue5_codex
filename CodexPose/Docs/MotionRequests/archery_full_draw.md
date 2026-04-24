# Archery Full Draw

- Request: add generated-image-based archery pose references and a key-pose archery shooting motion.
- Display names: `Archery Full Draw` / `활 당기기`
- Type: key-pose based non-looping action plus separate generated image pose-extraction references.
- Frame count / fps: 48 frames at 24 fps.
- Motion data file: `web/manny_archery_full_draw_keyposes.js`
- Viewer integration: `web/manny_viewer.js`, `web/index.html`
- Generated image files: `imagegen/generated/archery_pose_occluded_front_half.png`, `imagegen/generated/archery_pose_opposite_reveal_half.png`

## Intent

Demonstrate a right-handed archer setting an arrow, raising the bow, drawing to a cheek anchor, releasing, and holding follow-through. The generated reference images remain separate pose-extraction references.

## Style Constraints

- Right-handed archer with planted feet and a stable side-on stance.
- Left bow arm extends and braces forward.
- Right shoulder and draw arm move back during the draw.
- Draw hand anchors near the cheek before release.
- Release moves the draw hand rearward while the bow arm remains steady.

## Key Pose And Contact Requirements

- Both feet remain planted through the shot.
- Key contacts: front/braced foot, rear grounded foot, left hand on bow grip, right hand on string at full draw.
- Prop points drive a visible recurve bow, bowstring, and arrow.

## Revision Notes

- Corrected the torso yaw direction so the right shoulder no longer pushes forward during the draw.
- Moved the right draw-hand/string anchor backward on the forward axis to make the pull direction read correctly.
