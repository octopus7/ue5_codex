window.mannyWallClimbKeyposes = {
  id: "wallClimb",
  name: "Wall Climb",
  loop: false,
  frameCount: 24,
  fps: 24,
  coordinateSystem: {
    up: "Y",
    forward: "Z",
    side: "X",
    positionUnits: "Manny demo units",
    rotationUnits: "degrees"
  },
  wall: {
    id: "wallClimbTallMantleWall",
    center: { x: 0, y: 66, z: 43 },
    size: { width: 76, height: 132, thickness: 8 },
    topY: 132,
    surfaceZ: 39,
    farFaceZ: 47,
    topLandingZ: 76,
    leftEdgeX: -38,
    rightEdgeX: 38,
    material: {
      color: 0x5d9bd3,
      edgeColor: 0x3377b1,
      opacity: 0.9
    }
  },
  builderHints: {
    interpolation: "cubic ease between frames with contact-aware hand and foot locks",
    body: "Run, compress, jump, catch the high top edge with both hands, hook one leg, rotate sideways, then stand on the far/top side.",
    contacts: "Use top for hand catches and the single hooked foot. Do not make both feet march on the wall.",
    stance: "Most travel is along +Z; X offsets and yaw/roll sell the sideways mantle over the ledge."
  },
  frames: [
    frame(0, "run_in_right_back", true, [0, 95, -86, -6, 0, 0], [-11, 0, 0], [8, 0, 0], [18, 8, 10], [-32, 72, -8], [-24, -6, -9], [82, 76, -8], [9, 8.3, -70], [9.8, 1.2, -55], [-9, 8.3, -96], [-9.8, 1.2, -81], [22, 78, -76], [-20, 92, -96], c("ground", "ground")),
    frame(1, "run_in_left_drive", false, [-0.5, 96, -74, -9, -1, 0.5], [-16, -1, 0.5], [10, 0.5, 0], [-26, 76, 8], [20, 80, -8], [18, 78, -8], [-30, 74, -8], [8.8, 8.3, -70], [9.6, 1.2, -55], [-8.7, 15, -79], [-9.5, 7.5, -64], [16, 88, -82], [-18, 76, -70], c("ground", "air")),
    frame(2, "low_compression", true, [-0.8, 91, -60, -18, -1.5, 1], [-27, -2, 0.8], [16, 1, 0], [-48, 82, 9], [86, 82, -8], [42, 82, -8], [-34, 78, -8], [8.6, 8.3, -54], [9.4, 1.2, -39], [-8.4, 11, -72], [-9.2, 4, -57], [12, 82, -70], [-12, 106, -58], c("ground", "ground")),
    frame(3, "takeoff_push", false, [-0.6, 100, -43, -20, -1, 0.7], [-29, -1.5, 0.5], [18, 0.5, 0], [-58, 78, 8], [98, 78, -8], [58, 78, -8], [-42, 72, -8], [8.4, 8.3, -54], [9.2, 1.2, -39], [-8, 24, -47], [-8.8, 16, -32], [8, 96, -54], [-10, 116, -34], c("ground", "air")),
    frame(4, "airborne_reach", true, [-0.2, 111, -22, -16, 0, 0], [-25, 0, 0], [18, 0, 0], [82, 54, 6], [122, 38, 0], [82, 54, -6], [122, 38, 0], [8, 31, -45], [8.8, 24, -30], [-8, 26, -55], [-8.8, 19, -40], [13, 129, 28], [-13, 130, 30], c("air", "air")),
    frame(5, "hands_find_edge", false, [0, 118, 1, -20, 0.5, 0], [-33, 1, 0], [21, 0.4, 0], [104, 42, 4], [132, 30, 0], [104, 42, -4], [132, 30, 0], [7, 38, -30], [7.8, 31, -16], [-7, 35, -45], [-7.8, 28, -31], [12, 132, 39], [-12, 132, 39], c("air", "air", "top", "top")),
    frame(6, "both_hands_catch_top", true, [0, 121, 18, -28, 0, 0], [-45, 0, 0], [29, 0, 0], [110, 36, 4], [126, 48, 2], [110, 36, -4], [126, 48, 2], [7, 39, -16], [7.8, 32, -2], [-7, 33, -33], [-7.8, 26, -19], [12, 132, 39], [-12, 132, 39], c("air", "air", "top", "top")),
    frame(7, "loaded_hang", false, [0, 118, 27, -34, 0, 0], [-53, 0, 0], [33, 0, 0], [112, 48, 5], [122, 66, 3], [112, 48, -5], [122, 66, 3], [7, 36, -6], [7.8, 29, 8], [-7, 31, -24], [-7.8, 24, -10], [12, 132, 39], [-12, 132, 39], c("air", "air", "top", "top")),
    frame(8, "pull_to_edge", true, [-1.5, 128, 38, -32, -8, 4], [-47, -10, 5], [27, -3, 0], [103, 66, 9], [104, 86, 6], [104, 68, -4], [104, 86, 5], [8, 44, 8], [8.8, 37, 22], [-8, 34, -14], [-8.8, 27, 0], [12, 132, 39], [-12, 132, 39], c("air", "air", "top", "top")),
    frame(9, "left_knee_lifts_to_ledge", false, [-4, 139, 48, -27, -18, 8], [-35, -21, 9], [20, -6, 0], [92, 86, 12], [88, 104, 8], [96, 84, -2], [92, 100, 5], [13, 108, 43], [14, 116, 57], [-8, 42, -4], [-8.8, 35, 10], [12, 132, 39], [-12, 132, 39], c("air", "air", "top", "top")),
    frame(10, "left_leg_hooks_top", true, [-7, 136, 56, -20, -31, 12], [-24, -34, 12], [13, -9, 0], [78, 100, 14], [78, 112, 10], [86, 94, -1], [82, 110, 5], [15, 137, 49], [16, 132, 66], [-8, 49, 7], [-8.8, 42, 21], [12, 132, 39], [-12, 132, 39], c("top", "air", "top", "top")),
    frame(11, "hooked_leg_loads", false, [-10, 141, 62, -13, -43, 15], [-16, -44, 14], [8, -12, 0], [66, 108, 15], [70, 118, 10], [76, 98, -2], [76, 114, 5], [16, 138, 53], [17, 132, 69], [-7, 59, 17], [-7.8, 52, 31], [12, 132, 39], [-12, 132, 39], c("top", "air", "top", "top")),
    frame(12, "sideways_pull_over", true, [-13, 146, 68, -6, -58, 18], [-8, -56, 15], [4, -15, 0], [54, 112, 14], [64, 122, 8], [62, 104, -4], [70, 118, 4], [16, 138, 59], [17, 132, 75], [-6, 72, 29], [-6.8, 65, 43], [11, 132, 42], [-12, 132, 39], c("top", "air", "top", "top")),
    frame(13, "belly_over_edge", false, [-12, 160, 73, -1, -68, 16], [-2, -62, 12], [1, -16, 0], [44, 106, 10], [60, 120, 6], [52, 102, -6], [66, 116, 4], [15, 138, 65], [16, 132, 81], [-7, 91, 43], [-8, 84, 57], [9, 138, 50], [-10, 138, 50], c("top", "air", "air", "air")),
    frame(14, "trailing_leg_steps_through", true, [-8, 184, 78, 4, -52, 8], [4, -44, 6], [-2, -10, 0], [32, 88, 7], [66, 104, 4], [40, 86, -7], [72, 104, 3], [13, 138, 70], [14, 132, 86], [-12, 122, 59], [-13, 132, 74], [7, 142, 56], [-8, 142, 56], c("top", "top", "air", "air")),
    frame(15, "hands_begin_release", false, [-5, 198, 82, 7, -30, 3], [6, -23, 2], [-3, -5, 0], [18, 72, 5], [86, 86, 2], [22, 70, -5], [88, 86, 2], [12, 138, 73], [13, 132, 89], [-11, 138, 65], [-12, 132, 81], [14, 138, 53], [-14, 138, 53], c("top", "top", "air", "air")),
    frame(16, "low_crouch_on_top", true, [-2, 205, 84, 8, -12, 0], [7, -8, 0], [-4, -2, 0], [8, 60, 3], [92, 74, 1], [10, 60, -3], [92, 74, 1], [10, 138, 75], [11, 132, 91], [-10, 138, 70], [-11, 132, 86], [18, 147, 62], [-18, 147, 62], c("top", "top", "air", "air")),
    frame(17, "rise_from_crouch", false, [-0.5, 213, 86, 6, -5, 0], [5, -3, 0], [-3, -1, 0], [-3, 52, 2], [84, 68, 0], [-2, 52, -2], [84, 68, 0], [10, 138, 76], [11, 132, 92], [-10, 138, 72], [-11, 132, 88], [19, 156, 70], [-19, 156, 70], c("top", "top", "air", "air")),
    frame(18, "stand_unfold", true, [0, 220, 88, 3, -1, 0], [2, -1, 0], [-1, 0, 0], [-10, 46, 1], [70, 62, 0], [-10, 46, -1], [70, 62, 0], [9.5, 138, 77], [10.5, 132, 93], [-9.5, 138, 74], [-10.5, 132, 90], [18, 162, 75], [-18, 162, 75], c("top", "top", "air", "air")),
    frame(19, "balance_check", false, [0, 224, 90, 1, 0, 0], [0.8, 0, 0], [-0.5, 0, 0], [-13, 44, 1], [58, 58, 0], [-13, 44, -1], [58, 58, 0], [9.5, 138, 78], [10.5, 132, 94], [-9.5, 138, 76], [-10.5, 132, 92], [16, 158, 78], [-16, 158, 78], c("top", "top", "air", "air")),
    frame(20, "upright_on_far_side", true, [0, 226, 92, 0, 0, 0], [0, 0, 0], [0, 0, 0], [-14, 40, 0], [48, 52, 0], [-14, 40, 0], [48, 52, 0], [9.3, 138, 79], [10.3, 132, 95], [-9.3, 138, 77], [-10.3, 132, 93], [15, 150, 80], [-15, 150, 80], c("top", "top", "air", "air")),
    frame(21, "settle_step", false, [0, 226.5, 94, 0, 1, 0], [0, 0.5, 0], [0, 0.2, 0], [-13, 38, 0], [44, 50, 0], [-13, 38, 0], [44, 50, 0], [9.3, 138, 80], [10.3, 132, 96], [-9.3, 138, 80], [-10.3, 132, 96], [14, 144, 80], [-14, 144, 80], c("top", "top", "air", "air")),
    frame(22, "final_recover", false, [0, 226.2, 95, 0, 0.5, 0], [0, 0.2, 0], [0, 0, 0], [-14, 38, 0], [40, 48, 0], [-14, 38, 0], [40, 48, 0], [9.3, 138, 81], [10.3, 132, 97], [-9.3, 138, 81], [-10.3, 132, 97], [13, 138, 80], [-13, 138, 80], c("top", "top", "air", "air")),
    frame(23, "standing_after_mantle", true, [0, 226, 95, 0, 0, 0], [0, 0, 0], [0, 0, 0], [-14, 38, 0], [38, 46, 0], [-14, 38, 0], [38, 46, 0], [9.3, 138, 81], [10.3, 132, 97], [-9.3, 138, 81], [-10.3, 132, 97], [12, 132, 80], [-12, 132, 80], c("top", "top", "air", "air"))
  ]
};

function c(leftFoot = "air", rightFoot = "air", leftHand = "air", rightHand = "air") {
  return { leftFoot, rightFoot, leftHand, rightHand };
}

function frame(frameIndex, name, key, pelvis, chest, head, leftArm, leftElbow, rightArm, rightElbow, leftAnkle, leftBall, rightAnkle, rightBall, leftHand, rightHand, contacts) {
  return {
    frame: frameIndex,
    name,
    key,
    pelvis: { pos: p(pelvis[0], pelvis[1], pelvis[2]), rot: r(pelvis[3], pelvis[4], pelvis[5]) },
    chest: { rot: r(chest[0], chest[1], chest[2]) },
    head: { rot: r(head[0], head[1], head[2]), lookAt: p(0, 134, 48) },
    leftArm: { shoulder: r(leftArm[0], leftArm[1], leftArm[2]), elbow: { bend: elbowBend(leftElbow) }, wrist: { pitch: wristPitch(leftElbow) } },
    rightArm: { shoulder: r(rightArm[0], rightArm[1], rightArm[2]), elbow: { bend: elbowBend(rightElbow) }, wrist: { pitch: wristPitch(rightElbow) } },
    leftFoot: { ankle: p(leftAnkle[0], leftAnkle[1], leftAnkle[2]), ball: p(leftBall[0], leftBall[1], leftBall[2]), rot: r(leftElbow[2], 0, 0), weight: contactWeight(contacts.leftFoot, frameIndex) },
    rightFoot: { ankle: p(rightAnkle[0], rightAnkle[1], rightAnkle[2]), ball: p(rightBall[0], rightBall[1], rightBall[2]), rot: r(rightElbow[2], 0, 0), weight: contactWeight(contacts.rightFoot, frameIndex) },
    leftHand: { pos: p(leftHand[0], leftHand[1], leftHand[2]), rot: r(0, 4, 9), grip: grip(contacts.leftHand), weight: handWeight(contacts.leftHand, frameIndex) },
    rightHand: { pos: p(rightHand[0], rightHand[1], rightHand[2]), rot: r(0, -4, -9), grip: grip(contacts.rightHand), weight: handWeight(contacts.rightHand, frameIndex) },
    contacts
  };
}

function p(x, y, z) {
  return { x, y, z };
}

function r(pitch, yaw, roll) {
  return { pitch, yaw, roll };
}

function elbowBend(spec) {
  return Math.max(36, Math.min(128, Math.abs(Number(spec[0]) || 0)));
}

function wristPitch(spec) {
  return Math.max(-18, Math.min(18, Number(spec[1]) || 0));
}

function grip(contact) {
  return contact === "top" ? 1 : 0;
}

function handWeight(contact, frameIndex) {
  if (contact !== "top") {
    return 0;
  }
  return frameIndex >= 15 ? 0.2 : 1;
}

function contactWeight(contact, frameIndex) {
  if (contact === "air") {
    return 0;
  }
  if (contact === "ground") {
    return 0.9;
  }
  if (frameIndex >= 14) {
    return 0.85;
  }
  return 0.72;
}

(function normalizeWallClimbRendererTargets(data) {
  if (!data || !Array.isArray(data.frames)) {
    return;
  }

  const degToRad = Math.PI / 180;
  const armReach = 58.0;
  const legReach = 82.0;

  function vec(x, y, z) {
    return { x: Number(x) || 0, y: Number(y) || 0, z: Number(z) || 0 };
  }

  function add(a, b) {
    return vec(a.x + b.x, a.y + b.y, a.z + b.z);
  }

  function sub(a, b) {
    return vec(a.x - b.x, a.y - b.y, a.z - b.z);
  }

  function mul(a, scale) {
    return vec(a.x * scale, a.y * scale, a.z * scale);
  }

  function len(a) {
    return Math.hypot(a.x, a.y, a.z);
  }

  function norm(a) {
    const length = len(a) || 1;
    return mul(a, 1 / length);
  }

  function arr(a) {
    return [
      Number(a.x.toFixed(3)),
      Number(a.y.toFixed(3)),
      Number(a.z.toFixed(3))
    ];
  }

  function rotate(offset, rot) {
    const yaw = (rot?.yaw || 0) * degToRad;
    const pitch = (rot?.pitch || 0) * degToRad;
    const roll = (rot?.roll || 0) * degToRad;
    const cy = Math.cos(yaw);
    const sy = Math.sin(yaw);
    const cp = Math.cos(pitch);
    const sp = Math.sin(pitch);
    const cr = Math.cos(roll);
    const sr = Math.sin(roll);

    const py = offset.y * cp - offset.z * sp;
    const pz = offset.y * sp + offset.z * cp;
    const rx = offset.x * cr - py * sr;
    const ry = offset.x * sr + py * cr;

    return vec(
      rx * cy + pz * sy,
      ry,
      -rx * sy + pz * cy
    );
  }

  function clampTarget(origin, target, maxDistance) {
    const delta = sub(target, origin);
    const distance = len(delta);
    if (distance <= maxDistance) {
      return target;
    }
    return add(origin, mul(delta, maxDistance / distance));
  }

  function solveElbow(shoulder, hand, side) {
    const upper = 31.5;
    const lower = 29.5;
    const delta = sub(hand, shoulder);
    const distance = Math.max(1, Math.min(len(delta), upper + lower - 0.25));
    const direction = norm(delta);
    const along = (upper * upper - lower * lower + distance * distance) / (2 * distance);
    const height = Math.sqrt(Math.max(0, upper * upper - along * along));
    const bend = norm(vec(side * 0.82, -0.35, -direction.z * 0.45));
    return add(add(shoulder, mul(direction, along)), mul(bend, height * 0.62));
  }

  function solveKnee(hip, ankle, side) {
    const upper = 43.3;
    const lower = 42.2;
    const delta = sub(ankle, hip);
    const distance = Math.max(1, Math.min(len(delta), upper + lower - 0.25));
    const direction = norm(delta);
    const along = (upper * upper - lower * lower + distance * distance) / (2 * distance);
    const height = Math.sqrt(Math.max(0, upper * upper - along * along));
    let bend = norm(vec(0, -direction.z, direction.y));
    if (bend.z < 0) {
      bend = mul(bend, -1);
    }
    return add(add(add(hip, mul(direction, along)), mul(bend, height * 0.72)), vec(side * 0.8, 0, 0));
  }

  function isLocked(contact, weight) {
    return contact && contact !== "air" && Number(weight || 0) >= 0.25;
  }

  data.frames.forEach((frameData) => {
    const pelvis = vec(frameData.pelvis.pos.x, frameData.pelvis.pos.y, frameData.pelvis.pos.z);
    const pelvisRot = frameData.pelvis.rot || {};
    const chestRot = frameData.chest?.rot || {};
    const neckRot = { pitch: (frameData.head?.rot?.pitch || 0) * 0.35, yaw: (frameData.head?.rot?.yaw || 0) * 0.35, roll: 0 };
    const headRot = frameData.head?.rot || {};
    const points = {};

    points.pelvis = pelvis;
    points.spine_01 = add(pelvis, rotate(vec(0, 12.4, 1.2), pelvisRot));
    points.spine_02 = add(points.spine_01, rotate(vec(0.1, 12.8, 1.4), pelvisRot));
    points.spine_03 = add(points.spine_02, rotate(vec(0.2, 12.8, 1.5), chestRot));
    points.spine_04 = add(points.spine_03, rotate(vec(0.4, 10.0, 1.2), chestRot));
    points.spine_05 = add(points.spine_04, rotate(vec(0.5, 9.7, 0.9), chestRot));
    points.neck_01 = add(add(points.spine_05, rotate(vec(0.2, 12.2, 0.4), chestRot)), rotate(vec(0, 0.5, 0.1), neckRot));
    points.head = add(points.neck_01, rotate(vec(0.1, 16.2, 0.3), headRot));
    points.clavicle_l = add(points.spine_05, rotate(vec(17.4, 2.8, 1.1), chestRot));
    points.clavicle_r = add(points.spine_05, rotate(vec(-17.4, 2.8, -1.1), chestRot));

    [
      ["l", "left", 1, points.clavicle_l],
      ["r", "right", -1, points.clavicle_r]
    ].forEach(([shortName, sideName, side, clavicle]) => {
      const handTarget = frameData[`${sideName}Hand`]?.pos || add(clavicle, rotate(vec(side * 24, -37, -1), chestRot));
      const hand = clampTarget(clavicle, vec(handTarget.x, handTarget.y, handTarget.z), armReach);
      const elbow = solveElbow(clavicle, hand, side);
      points[`upperarm_${shortName}`] = add(clavicle, mul(sub(elbow, clavicle), 0.48));
      points[`lowerarm_${shortName}`] = elbow;
      points[`hand_${shortName}`] = hand;
    });

    points.thigh_l = add(pelvis, rotate(vec(10.1, -3.8, 1.2), pelvisRot));
    points.thigh_r = add(pelvis, rotate(vec(-10.1, -3.8, -1.2), pelvisRot));

    [
      ["l", "left", 1, points.thigh_l],
      ["r", "right", -1, points.thigh_r]
    ].forEach(([shortName, sideName, side, thigh]) => {
      const foot = frameData[`${sideName}Foot`];
      const ankle = clampTarget(thigh, vec(foot.ankle.x, foot.ankle.y, foot.ankle.z), legReach);
      const sourceBall = vec(foot.ball.x, foot.ball.y, foot.ball.z);
      const ankleToBall = clampTarget(ankle, sourceBall, 17.2);
      points[`foot_${shortName}`] = ankle;
      points[`ball_${shortName}`] = ankleToBall;
      points[`calf_${shortName}`] = solveKnee(thigh, ankle, side);
    });

    frameData.points = Object.fromEntries(Object.entries(points).map(([name, point]) => [name, arr(point)]));

    const contactTargets = {};
    if (isLocked(frameData.contacts?.leftHand, frameData.leftHand?.weight)) {
      contactTargets.hand_l = frameData.points.hand_l;
    }
    if (isLocked(frameData.contacts?.rightHand, frameData.rightHand?.weight)) {
      contactTargets.hand_r = frameData.points.hand_r;
    }
    if (isLocked(frameData.contacts?.leftFoot, frameData.leftFoot?.weight)) {
      contactTargets.foot_l = frameData.points.foot_l;
      contactTargets.ball_l = frameData.points.ball_l;
    }
    if (isLocked(frameData.contacts?.rightFoot, frameData.rightFoot?.weight)) {
      contactTargets.foot_r = frameData.points.foot_r;
      contactTargets.ball_r = frameData.points.ball_r;
    }
    frameData.contactTargets = contactTargets;
  });
})(window.mannyWallClimbKeyposes);
