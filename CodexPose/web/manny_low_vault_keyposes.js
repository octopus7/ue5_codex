(function (global) {
  "use strict";

  const frameCount = 44;
  const fps = 24;
  const durationSeconds = frameCount / fps;
  const obstacleSize = [72, 34, 18];
  const obstacleCenter = [0, obstacleSize[1] * 0.5, 48];
  const obstacleTopY = obstacleSize[1];

  function clamp(value, min, max) {
    return Math.max(min, Math.min(max, value));
  }

  function smoothstep(value) {
    const t = clamp(value, 0, 1);
    return t * t * (3 - t * 2);
  }

  function lerp(a, b, t) {
    return a + (b - a) * t;
  }

  function round(value) {
    return Number(value.toFixed(3));
  }

  function vec(x, y, z) {
    return [round(x), round(y), round(z)];
  }

  function rot(pitch, yaw, roll) {
    return [round(pitch), round(yaw), round(roll)];
  }

  function pelvis(x, y, z, pitch, yaw, roll) {
    return vec(x, y, z).concat(rot(pitch, yaw, roll));
  }

  function lerpVec(a, b, t) {
    return vec(lerp(a[0], b[0], t), lerp(a[1], b[1], t), lerp(a[2], b[2], t));
  }

  function lerpArray(a, b, t) {
    return a.map((value, index) => round(lerp(value, b[index], t)));
  }

  function makeFoot(ankle, ball, planted, role, contactWeight, lockId) {
    return {
      ankle,
      ball,
      planted,
      locked: Boolean(lockId) && planted && contactWeight > 0.88,
      contactWeight: round(contactWeight),
      role,
      lockId
    };
  }

  const keyFrames = [
    {
      frame: 0,
      name: "run_in_stride_left_lead",
      phase: "run_in",
      pelvis: pelvis(0.0, 91.8, -30.0, -4, 1, -3),
      chest: rot(6, -5, 4),
      neck: rot(-2, 2, -1),
      head: rot(-3, 4, -1),
      lUpper: vec(12, -8, -9),
      lLower: vec(8, -15, -4),
      lHand: vec(4, -8, -5),
      rUpper: vec(-16, -11, 15),
      rLower: vec(-10, -18, 11),
      rHand: vec(-6, -10, 14),
      leftFoot: makeFoot(vec(10.8, 8.1, -17.0), vec(11.8, 1.1, -2.0), true, "approach_left_stride", 1, "left_run_0"),
      rightFoot: makeFoot(vec(-11.6, 14.0, -35.0), vec(-12.2, 6.0, -21.0), false, "rear_leg_recovering", 0.18, null),
      leftKneeBias: vec(2.8, -0.8, 10.8),
      rightKneeBias: vec(-4.5, 2.6, 12.5)
    },
    {
      frame: 5,
      name: "final_stride_lowering_to_rail",
      phase: "run_in",
      pelvis: pelvis(-1.0, 86.5, -6.0, -12, -5, 2),
      chest: rot(18, -6, -2),
      neck: rot(-6, 3, 1),
      head: rot(-9, 5, 1),
      lUpper: vec(15, -13, 8),
      lLower: vec(9, -19, 13),
      lHand: vec(5, -10, 15),
      rUpper: vec(-14, -10, -6),
      rLower: vec(-9, -17, -1),
      rHand: vec(-5, -9, -3),
      leftFoot: makeFoot(vec(10.8, 13.5, -3.0), vec(11.7, 5.2, 11.0), false, "leaving_ground", 0.22, null),
      rightFoot: makeFoot(vec(-11.2, 8.1, 8.0), vec(-11.8, 1.1, 23.0), true, "last_push_step", 1, "right_push_5"),
      leftKneeBias: vec(4.0, 2.0, 13.5),
      rightKneeBias: vec(-4.8, -1.0, 14.0)
    },
    {
      frame: 9,
      name: "both_hands_plant_on_obstacle",
      phase: "hand_plant",
      pelvis: pelvis(-0.8, 74.0, 22.0, -26, -3, -2),
      chest: rot(48, -2, -2),
      neck: rot(-18, 1, 0),
      head: rot(-28, 2, 0),
      lUpper: vec(12, -22, 17),
      lLower: vec(7, -25, 19),
      lHand: vec(2, -13, 10),
      rUpper: vec(-12, -22, 17),
      rLower: vec(-7, -25, 19),
      rHand: vec(-2, -13, 10),
      leftHandTarget: vec(14, obstacleTopY + 1, obstacleCenter[2] - 4),
      rightHandTarget: vec(-14, obstacleTopY + 1, obstacleCenter[2] - 4),
      leftFoot: makeFoot(vec(8.0, 22.0, 18.0), vec(8.8, 13.0, 31.0), false, "takeoff_air", 0.05, null),
      rightFoot: makeFoot(vec(-10.5, 10.2, 14.0), vec(-11.0, 2.0, 27.0), true, "toe_drive_releasing", 0.7, "right_push_5"),
      leftKneeBias: vec(6.0, 5.0, 10.0),
      rightKneeBias: vec(-6.5, 1.5, 13.5)
    },
    {
      frame: 13,
      name: "compressed_knees_tuck_over_hands",
      phase: "vault_tuck",
      pelvis: pelvis(3.0, 86.0, 38.0, -16, 15, -12),
      chest: rot(32, -10, -8),
      neck: rot(-14, 4, 2),
      head: rot(-22, 6, 2),
      lUpper: vec(12, -23, 18),
      lLower: vec(7, -26, 20),
      lHand: vec(2, -12, 10),
      rUpper: vec(-12, -23, 18),
      rLower: vec(-7, -26, 20),
      rHand: vec(-2, -12, 10),
      leftHandTarget: vec(14, obstacleTopY + 1, obstacleCenter[2] - 2),
      rightHandTarget: vec(-14, obstacleTopY + 1, obstacleCenter[2] - 2),
      leftFoot: makeFoot(vec(30.0, 49.0, 34.0), vec(32.0, 41.0, 45.0), false, "left_knee_tuck", 0, null),
      rightFoot: makeFoot(vec(12.0, 44.0, 31.0), vec(13.0, 36.0, 43.0), false, "right_knee_tuck", 0, null),
      leftKneeBias: vec(15.0, 9.0, 8.0),
      rightKneeBias: vec(2.0, 8.0, 9.0)
    },
    {
      frame: 18,
      name: "hips_clear_rail_legs_sweep_sideways",
      phase: "clearance",
      pelvis: pelvis(6.5, 98.0, 52.0, -4, 32, -20),
      chest: rot(14, -20, -12),
      neck: rot(-8, 8, 3),
      head: rot(-12, 12, 3),
      lUpper: vec(11, -23, 17),
      lLower: vec(6, -25, 19),
      lHand: vec(2, -12, 9),
      rUpper: vec(-11, -23, 17),
      rLower: vec(-6, -25, 19),
      rHand: vec(-2, -12, 9),
      leftHandTarget: vec(13, obstacleTopY + 1, obstacleCenter[2] + 2),
      rightHandTarget: vec(-13, obstacleTopY + 1, obstacleCenter[2] + 2),
      leftFoot: makeFoot(vec(39.0, 55.0, 58.0), vec(49.0, 50.0, 67.0), false, "lead_leg_clearing_side", 0, null),
      rightFoot: makeFoot(vec(25.0, 50.0, 52.0), vec(36.0, 45.0, 61.0), false, "trail_leg_clearing_side", 0, null),
      leftKneeBias: vec(18.0, 8.0, 4.0),
      rightKneeBias: vec(11.0, 7.0, 5.0)
    },
    {
      frame: 23,
      name: "hand_release_trailing_leg_clear",
      phase: "release",
      pelvis: pelvis(5.0, 93.0, 67.0, 4, 20, -10),
      chest: rot(8, -10, -4),
      neck: rot(-4, 4, 1),
      head: rot(-6, 7, 1),
      lUpper: vec(14, -12, 4),
      lLower: vec(10, -18, 5),
      lHand: vec(5, -9, 7),
      rUpper: vec(-14, -13, 8),
      rLower: vec(-9, -19, 8),
      rHand: vec(-5, -10, 10),
      leftHandTarget: vec(16, obstacleTopY + 10, obstacleCenter[2] + 12),
      rightHandTarget: vec(-15, obstacleTopY + 8, obstacleCenter[2] + 12),
      leftFoot: makeFoot(vec(25.0, 37.0, 78.0), vec(29.0, 28.0, 90.0), false, "lead_leg_reaching_down", 0.05, null),
      rightFoot: makeFoot(vec(12.0, 45.0, 70.0), vec(17.0, 36.0, 82.0), false, "trail_leg_clear", 0, null),
      leftKneeBias: vec(11.0, 4.0, 10.0),
      rightKneeBias: vec(6.0, 7.0, 9.0)
    },
    {
      frame: 29,
      name: "lead_foot_staggered_landing",
      phase: "landing",
      pelvis: pelvis(2.5, 78.0, 82.0, -12, 8, -5),
      chest: rot(18, -3, 1),
      neck: rot(-6, 1, 0),
      head: rot(-9, 2, 0),
      lUpper: vec(15, -10, 12),
      lLower: vec(10, -17, 10),
      lHand: vec(5, -9, 12),
      rUpper: vec(-16, -9, 5),
      rLower: vec(-10, -17, 4),
      rHand: vec(-6, -9, 6),
      leftFoot: makeFoot(vec(11.0, 8.0, 83.0), vec(12.0, 1.0, 98.0), true, "lead_landing", 1, "left_land_29"),
      rightFoot: makeFoot(vec(-5.0, 25.0, 78.0), vec(-4.0, 15.0, 92.0), false, "trail_leg_following", 0.2, null),
      leftKneeBias: vec(5.5, -1.5, 14.5),
      rightKneeBias: vec(-2.0, 4.0, 13.0)
    },
    {
      frame: 36,
      name: "trail_foot_sets_down",
      phase: "landing_recover",
      pelvis: pelvis(0.8, 84.5, 94.0, -7, 2, 1),
      chest: rot(10, 3, 0),
      neck: rot(-3, -1, 0),
      head: rot(-5, -2, 0),
      lUpper: vec(14, -12, 12),
      lLower: vec(9, -18, 10),
      lHand: vec(5, -10, 13),
      rUpper: vec(-14, -12, 12),
      rLower: vec(-9, -18, 10),
      rHand: vec(-5, -10, 13),
      leftFoot: makeFoot(vec(11.0, 8.0, 83.0), vec(12.0, 1.0, 98.0), true, "lead_support_locked", 1, "left_land_29"),
      rightFoot: makeFoot(vec(-10.2, 8.1, 91.0), vec(-10.8, 1.1, 106.0), true, "trail_landing", 0.95, "right_land_36"),
      leftKneeBias: vec(4.2, -1.2, 13.0),
      rightKneeBias: vec(-4.3, -1.1, 13.5)
    },
    {
      frame: 43,
      name: "recover_forward_runout",
      phase: "recover",
      pelvis: pelvis(0.0, 92.0, 105.0, -3, 0, 0),
      chest: rot(4, 0, 0),
      neck: rot(-1, 0, 0),
      head: rot(-2, 0, 0),
      lUpper: vec(14, -12, 8),
      lLower: vec(9, -18, 6),
      lHand: vec(5, -10, 8),
      rUpper: vec(-15, -12, 15),
      rLower: vec(-10, -18, 11),
      rHand: vec(-6, -10, 14),
      leftFoot: makeFoot(vec(9.8, 8.1, 104.0), vec(10.6, 1.1, 119.0), true, "runout_left", 1, "left_recover_43"),
      rightFoot: makeFoot(vec(-11.0, 9.5, 91.0), vec(-11.8, 2.4, 106.0), true, "runout_rear", 0.82, "right_land_36"),
      leftKneeBias: vec(2.8, -0.8, 11.5),
      rightKneeBias: vec(-3.2, -0.6, 12.0)
    }
  ];

  const keyNames = new Map(keyFrames.map((keyFrame) => [keyFrame.frame, keyFrame.name]));

  function spanForFrame(frame) {
    for (let index = 0; index < keyFrames.length - 1; index += 1) {
      if (frame >= keyFrames[index].frame && frame <= keyFrames[index + 1].frame) {
        return [keyFrames[index], keyFrames[index + 1]];
      }
    }
    return [keyFrames[keyFrames.length - 1], keyFrames[keyFrames.length - 1]];
  }

  function blendFoot(a, b, t) {
    const contactWeight = round(lerp(a.contactWeight || 0, b.contactWeight || 0, t));
    const planted = contactWeight > 0.42;
    const locked = Boolean(a.lockId && a.lockId === b.lockId && a.locked && b.locked && planted);
    const useLockedA = locked && t < 0.55;
    return {
      ankle: useLockedA ? a.ankle.slice(0, 3) : lerpVec(a.ankle, b.ankle, t),
      ball: useLockedA ? a.ball.slice(0, 3) : lerpVec(a.ball, b.ball, t),
      planted,
      locked,
      contactWeight,
      role: t < 0.5 ? a.role : b.role,
      lockId: a.lockId === b.lockId ? a.lockId : t < 0.5 ? a.lockId : b.lockId
    };
  }

  function handTargetFor(key, side) {
    if (side === "left") {
      return key.leftHandTarget || null;
    }
    return key.rightHandTarget || null;
  }

  function blendOptionalTarget(a, b, side, t) {
    const from = handTargetFor(a, side);
    const to = handTargetFor(b, side);
    if (from && to) {
      return lerpVec(from, to, t);
    }
    return (t < 0.5 ? from : to) || undefined;
  }

  function contactForFoot(foot) {
    if (!foot.planted) {
      return "air";
    }
    if (foot.role && foot.role.indexOf("landing") !== -1) {
      return "ground_landing";
    }
    if (foot.role && foot.role.indexOf("push") !== -1) {
      return "toe_push";
    }
    return foot.locked ? "ground_locked" : "ground";
  }

  function makeFrame(frame) {
    const [a, b] = spanForFrame(frame);
    const span = Math.max(1, b.frame - a.frame);
    const t = smoothstep((frame - a.frame) / span);
    const leftFoot = blendFoot(a.leftFoot, b.leftFoot, t);
    const rightFoot = blendFoot(a.rightFoot, b.rightFoot, t);
    const leftKneeBias = lerpVec(a.leftKneeBias, b.leftKneeBias, t);
    const rightKneeBias = lerpVec(a.rightKneeBias, b.rightKneeBias, t);
    const leftHandTarget = blendOptionalTarget(a, b, "left", t);
    const rightHandTarget = blendOptionalTarget(a, b, "right", t);
    const handPlant = frame >= 9 && frame <= 21;

    const sampled = {
      frame,
      name: keyNames.get(frame) || "low_vault_inbetween",
      key: keyNames.has(frame),
      phase: frame === a.frame ? a.phase : frame === b.frame ? b.phase : "transition",
      pelvis: lerpArray(a.pelvis, b.pelvis, t),
      chest: lerpArray(a.chest, b.chest, t),
      neck: lerpArray(a.neck, b.neck, t),
      head: lerpArray(a.head, b.head, t),
      lUpper: lerpVec(a.lUpper, b.lUpper, t),
      lLower: lerpVec(a.lLower, b.lLower, t),
      lHand: lerpVec(a.lHand, b.lHand, t),
      rUpper: lerpVec(a.rUpper, b.rUpper, t),
      rLower: lerpVec(a.rLower, b.rLower, t),
      rHand: lerpVec(a.rHand, b.rHand, t),
      leftFoot,
      rightFoot,
      leftKneeBias,
      rightKneeBias,
      kneeBias: {
        left: leftKneeBias.slice(0, 3),
        right: rightKneeBias.slice(0, 3)
      },
      contacts: {
        leftFoot: contactForFoot(leftFoot),
        rightFoot: contactForFoot(rightFoot),
        leftHand: handPlant ? "obstacle_top" : frame < 9 ? "swinging_to_plant" : "released",
        rightHand: handPlant ? "obstacle_top" : frame < 9 ? "swinging_to_plant" : "released",
        obstacle: handPlant ? "two_hand_support" : "nearby_clearance"
      },
      obstacle: {
        id: "low_vault_obstacle",
        type: "low_rail_or_box",
        center: vec(obstacleCenter[0], obstacleCenter[1], obstacleCenter[2]),
        size: vec(obstacleSize[0], obstacleSize[1], obstacleSize[2]),
        topY: obstacleTopY,
        clearance: frame >= 13 && frame <= 23 ? "legs_clear_sideways_forward" : "approach_or_recover"
      }
    };

    if (leftHandTarget) {
      sampled.leftHandTarget = leftHandTarget;
    }
    if (rightHandTarget) {
      sampled.rightHandTarget = rightHandTarget;
    }

    return sampled;
  }

  const frames = Array.from({ length: frameCount }, (_, frame) => makeFrame(frame));

  const data = {
    id: "lowVault",
    name: "Low Vault",
    koreanName: "낮은 장애물 볼트",
    displayNames: {
      en: "Low Vault",
      ko: "낮은 장애물 볼트"
    },
    frameCount,
    fps,
    durationSeconds: round(durationSeconds),
    loop: false,
    rotationUnits: "degrees",
    positionUnits: "Manny demo units",
    coordinateSystem: {
      up: "Y",
      forward: "Z",
      side: "X",
      groundY: 0
    },
    style: {
      intent: "A run-in low vault where both hands plant on a low obstacle, the knees tuck, the legs clear sideways and forward, then Manny releases into a staggered landing and recovery.",
      notes: [
        "The run-in lowers into a two-hand plant instead of jumping cleanly over the prop.",
        "Frames 13-23 keep both feet airborne with the legs swept toward positive X so the body reads as a low side/forward vault.",
        "The landing is staggered: the lead left foot contacts first, then the right foot follows into a runout recovery."
      ]
    },
    prop: {
      id: "low_vault_obstacle",
      type: "low_rail_or_box",
      size: vec(obstacleSize[0], obstacleSize[1], obstacleSize[2]),
      center: vec(obstacleCenter[0], obstacleCenter[1], obstacleCenter[2]),
      topY: obstacleTopY,
      contact: "Both hands plant on the top surface before the legs clear."
    },
    environment: {
      ground: "flat floor",
      obstacle: "low rail or box placed across the run path at positive Z"
    },
    contactNotes: {
      hands: "Both hands plant on the obstacle top from frame 9 through the clearance beat, then release.",
      feet: "Run-in and landing expose planted foot contacts; the tuck and clearance expose air contacts for both feet.",
      landing: "Left foot lands first at frame 29 and the right foot sets down at frame 36 for a staggered recovery."
    },
    keyposes: frames.filter((frame) => frame.key),
    frames
  };

  data.keys = frames;

  global.mannyLowVaultKeyposes = data;
})(typeof window !== "undefined" ? window : globalThis);
