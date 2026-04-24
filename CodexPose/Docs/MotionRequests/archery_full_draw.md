# Archery Full Draw

- Request: add two separate archery-related pieces of work: generated-image-based pose extraction references for a bowstring-draw pose, and a learned-data-based archery shooting motion.
- Original intent: "생성이미지 기반 활시위 당기는 포즈 추가하고 (필요한 만큼 이미지 생성, 작업 끝나면 이미지는 절반 사이즈로 보관). 너의 학습 데이터 기반 활쏘는 모션도 추가해. 각각 별도 작업이야."
- Display names: `Archery Full Draw` / `Archery Pose Extraction - Occluded View` / `Archery Pose Extraction - Opposite View`
- Type: key-pose based non-looping action plus separate generated image pose-extraction references.
- Frame count / fps: 48 frames at 24 fps.
- Motion data file: `web/manny_archery_full_draw_keyposes.js`
- Viewer integration: `web/manny_viewer.js`, `web/index.html`
- Generated image files: `imagegen/generated/archery_pose_occluded_front_half.png`, `imagegen/generated/archery_pose_opposite_reveal_half.png`

## Intent

Demonstrate an archer setting an arrow, raising the bow, drawing to a cheek anchor, releasing, and holding follow-through. Separately, provide generated reference images where the front view partially hides the draw arm and the opposite view reveals the shoulder-elbow-wrist-hand chain for pose extraction comparison.

## Style Constraints

- Right-handed archer with planted feet and a stable side-on stance.
- Bow arm extends and braces forward.
- Draw hand anchors near the cheek before release.
- Release moves the draw hand rearward while the bow arm remains steady.
- Keep the motion readable as a technical demo rather than a cinematic flourish.

## Key Pose And Contact Requirements

- Both feet remain planted through the shot.
- Key contacts: front/braced foot, rear grounded foot, left hand on bow grip, right hand on string at full draw.
- Prop points drive a visible recurve bow, bowstring, and arrow.

## Props And Environment

- Recurve bow, string, and arrow are rendered as web preview props.
- Generated pose-extraction images use a plain studio background and are stored at half of their generated resolution.
