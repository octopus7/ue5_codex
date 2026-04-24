(function (global) {
  "use strict";

  const frameCount = 46;
  const fps = 24;
  const durationSeconds = frameCount / fps;

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

  function lerpArray(a, b, t) {
    return a.map((value, index) => round(lerp(value, b[index], t)));
  }

  function foot(x, z, yaw, role, options) {
    const settings = options || {};
    const yawRad = (yaw * Math.PI) / 180;
    return {
      ankle: vec(x, 8.1, z),
      ball: vec(x + Math.sin(yawRad) * 2.8, 1.05, z + 15.0),
      planted: true,
      locked: true,
      contactWeight: 1,
      yaw: round(yaw),
      role,
      pivot: settings.pivot || "flat",
      weight: round(settings.weight ?? 0.5)
    };
  }

  function cloneFoot(source) {
    return {
      ankle: source.ankle.slice(),
      ball: source.ball.slice(),
      planted: true,
      locked: true,
      contactWeight: 1,
      yaw: source.yaw,
      role: source.role,
      pivot: source.pivot,
      weight: source.weight
    };
  }

  function interpolateFoot(a, b, t) {
    const pivot = t < 0.5 ? a.pivot : b.pivot;
    return {
      ankle: lerpArray(a.ankle, b.ankle, t),
      ball: lerpArray(a.ball, b.ball, t),
      planted: true,
      locked: true,
      contactWeight: 1,
      yaw: round(lerp(a.yaw, b.yaw, t)),
      role: t < 0.5 ? a.role : b.role,
      pivot,
      weight: round(lerp(a.weight, b.weight, t))
    };
  }

  function pose(frame, name, phase, pelvis, chest, neck, head, leftArm, rightArm, leftFoot, rightFoot, leftKneeBias, rightKneeBias, strike) {
    return {
      frame,
      name,
      phase,
      pelvis,
      chest,
      neck,
      head,
      lUpper: leftArm[0],
      lLower: leftArm[1],
      lHand: leftArm[2],
      rUpper: rightArm[0],
      rLower: rightArm[1],
      rHand: rightArm[2],
      leftFoot,
      rightFoot,
      leftKneeBias,
      rightKneeBias,
      strike
    };
  }

  function strike(hand, type, active, power) {
    return {
      hand,
      type,
      active: Boolean(active),
      power: round(power || 0),
      targetLine: active ? "head_height_forward_centerline" : "guard"
    };
  }

  const leftGuardFoot = foot(10.2, 14.5, -8, "lead_front_planted", { weight: 0.52 });
  const rightGuardFoot = foot(-12.0, -8.5, 18, "rear_loaded_planted", { weight: 0.48 });
  const leftHookPivotFoot = foot(10.2, 14.5, -28, "lead_hook_toe_pivot", { pivot: "lead_toe_pivot", weight: 0.45 });
  const rightCrossPivotFoot = foot(-12.0, -8.5, 52, "rear_cross_ball_pivot", { pivot: "rear_ball_pivot", weight: 0.62 });

  const keyFrames = [
    pose(
      0,
      "orthodox_guard",
      "guard",
      vec(0.0, 91.8, 2.0).concat(rot(-3.0, -8.0, 0.5)),
      rot(4.0, -12.0, -1.0),
      rot(-1.0, 4.0, 0.0),
      rot(-2.0, 6.0, 0.0),
      [vec(11, -12, 12), vec(8, -16, 18), vec(7, -8, 25)],
      [vec(-11, -12, 9), vec(-7, -16, 14), vec(-6, -8, 21)],
      leftGuardFoot,
      rightGuardFoot,
      vec(2.6, -0.8, 5.0),
      vec(-3.0, -1.0, 6.0),
      strike("none", "guard", false, 0)
    ),
    pose(
      5,
      "jab_load_guard_tight",
      "jab_load",
      vec(0.5, 90.8, 3.8).concat(rot(-5.0, -11.0, -0.5)),
      rot(6.0, -16.0, -1.5),
      rot(-1.5, 5.0, 0.0),
      rot(-2.5, 7.0, 0.0),
      [vec(12, -11, 14), vec(9, -15, 21), vec(8, -8, 28)],
      [vec(-10, -12, 9), vec(-7, -16, 14), vec(-6, -8, 20)],
      foot(10.2, 14.5, -10, "lead_front_loaded", { weight: 0.57 }),
      foot(-12.0, -8.5, 20, "rear_guard_planted", { weight: 0.43 }),
      vec(3.0, -1.0, 5.8),
      vec(-3.0, -0.8, 5.6),
      strike("left", "jab_chamber", false, 0.25)
    ),
    pose(
      9,
      "lead_jab_extension",
      "jab",
      vec(1.0, 91.0, 5.8).concat(rot(-3.0, -5.0, -1.0)),
      rot(2.0, -4.0, -2.0),
      rot(-1.0, 1.0, 0.0),
      rot(-2.0, 2.0, 0.0),
      [vec(14, -9, 21), vec(12, -10, 34), vec(9, -7, 49)],
      [vec(-11, -12, 8), vec(-8, -16, 13), vec(-6, -8, 20)],
      foot(10.2, 14.5, -10, "lead_front_braced_for_jab", { weight: 0.6 }),
      foot(-12.0, -8.5, 20, "rear_guard_planted", { weight: 0.4 }),
      vec(3.2, -1.0, 5.6),
      vec(-2.8, -0.8, 5.4),
      strike("left", "jab", true, 0.8)
    ),
    pose(
      13,
      "jab_recoil_guard",
      "jab_recoil",
      vec(0.3, 91.3, 4.0).concat(rot(-3.0, -9.0, 0.0)),
      rot(3.0, -12.0, -0.5),
      rot(-1.0, 4.0, 0.0),
      rot(-2.0, 5.0, 0.0),
      [vec(11, -12, 13), vec(8, -16, 19), vec(7, -8, 25)],
      [vec(-11, -12, 9), vec(-7, -16, 14), vec(-6, -8, 21)],
      leftGuardFoot,
      rightGuardFoot,
      vec(2.7, -0.8, 5.2),
      vec(-3.0, -1.0, 5.8),
      strike("left", "jab_recoil", false, 0.15)
    ),
    pose(
      17,
      "rear_cross_drive",
      "cross_drive",
      vec(-0.6, 90.6, 5.5).concat(rot(-5.0, 9.0, 0.8)),
      rot(5.0, 15.0, 1.0),
      rot(-1.0, -4.0, 0.0),
      rot(-2.0, -6.0, 0.0),
      [vec(10, -12, 12), vec(7, -16, 17), vec(6, -8, 23)],
      [vec(-13, -10, 15), vec(-10, -12, 28), vec(-8, -8, 40)],
      foot(10.2, 14.5, -6, "lead_front_braced_for_cross", { weight: 0.38 }),
      rightCrossPivotFoot,
      vec(2.4, -0.8, 5.4),
      vec(-3.8, -0.4, 5.0),
      strike("right", "cross_drive", true, 0.65)
    ),
    pose(
      21,
      "rear_cross_full_extension",
      "cross",
      vec(-1.0, 91.0, 6.8).concat(rot(-2.0, 24.0, 1.5)),
      rot(1.0, 36.0, 2.0),
      rot(-1.0, -10.0, 0.0),
      rot(-2.0, -14.0, 0.0),
      [vec(9, -12, 10), vec(6, -16, 15), vec(5, -8, 22)],
      [vec(-14, -8, 23), vec(-12, -9, 36), vec(-9, -6, 50)],
      foot(10.2, 14.5, -4, "lead_front_braced_for_cross", { weight: 0.34 }),
      foot(-12.0, -8.5, 66, "rear_cross_pivoted", { pivot: "rear_ball_pivot", weight: 0.66 }),
      vec(2.2, -0.8, 5.2),
      vec(-4.2, -0.2, 4.6),
      strike("right", "cross", true, 1.0)
    ),
    pose(
      25,
      "cross_recoil_hook_load",
      "hook_load",
      vec(0.2, 90.5, 5.0).concat(rot(-5.0, -18.0, -1.5)),
      rot(7.0, -30.0, -2.0),
      rot(-1.5, 8.0, 0.0),
      rot(-2.5, 12.0, 0.0),
      [vec(15, -10, 14), vec(13, -13, 20), vec(11, -7, 28)],
      [vec(-10, -12, 11), vec(-7, -16, 15), vec(-6, -8, 22)],
      foot(10.2, 14.5, -18, "lead_front_hook_load", { pivot: "lead_toe_pivot", weight: 0.55 }),
      foot(-12.0, -8.5, 34, "rear_cross_recoil_planted", { pivot: "rear_ball_pivot", weight: 0.45 }),
      vec(3.2, -0.8, 5.4),
      vec(-3.0, -0.8, 5.4),
      strike("left", "hook_chamber", false, 0.35)
    ),
    pose(
      30,
      "lead_hook_hip_torso_rotation",
      "hook",
      vec(0.4, 91.0, 5.8).concat(rot(-2.0, 31.0, -2.0)),
      rot(0.0, 52.0, -3.0),
      rot(-1.0, -14.0, 0.5),
      rot(-2.0, -20.0, 0.5),
      [vec(21, -8, 18), vec(24, -9, 22), vec(20, -6, 31)],
      [vec(-9, -12, 9), vec(-7, -16, 13), vec(-6, -8, 21)],
      leftHookPivotFoot,
      foot(-12.0, -8.5, 28, "rear_hook_anchor", { weight: 0.55 }),
      vec(3.8, -0.4, 4.8),
      vec(-2.8, -0.8, 5.8),
      strike("left", "hook", true, 0.95)
    ),
    pose(
      34,
      "hook_follow_through_guard_check",
      "hook_follow_through",
      vec(0.0, 91.2, 5.0).concat(rot(-2.0, 18.0, -1.0)),
      rot(1.0, 28.0, -1.0),
      rot(-1.0, -8.0, 0.0),
      rot(-2.0, -12.0, 0.0),
      [vec(16, -10, 14), vec(13, -13, 18), vec(10, -8, 24)],
      [vec(-10, -12, 9), vec(-7, -16, 14), vec(-6, -8, 21)],
      foot(10.2, 14.5, -18, "lead_hook_recovering", { pivot: "lead_toe_pivot", weight: 0.48 }),
      foot(-12.0, -8.5, 24, "rear_hook_anchor", { weight: 0.52 }),
      vec(3.0, -0.6, 5.2),
      vec(-2.8, -0.8, 5.6),
      strike("left", "hook_recoil", false, 0.25)
    ),
    pose(
      39,
      "guarded_recover",
      "recover",
      vec(0.0, 91.6, 3.0).concat(rot(-3.0, -4.0, 0.2)),
      rot(4.0, -8.0, -0.5),
      rot(-1.0, 3.0, 0.0),
      rot(-2.0, 5.0, 0.0),
      [vec(12, -12, 12), vec(8, -16, 18), vec(7, -8, 25)],
      [vec(-11, -12, 9), vec(-7, -16, 14), vec(-6, -8, 21)],
      foot(10.2, 14.5, -10, "lead_front_recovered", { weight: 0.52 }),
      foot(-12.0, -8.5, 20, "rear_recovered", { weight: 0.48 }),
      vec(2.7, -0.8, 5.2),
      vec(-3.0, -1.0, 5.8),
      strike("none", "guard_recover", false, 0)
    ),
    pose(
      45,
      "orthodox_guard_settled",
      "settled_guard",
      vec(0.0, 91.8, 2.0).concat(rot(-3.0, -8.0, 0.5)),
      rot(4.0, -12.0, -1.0),
      rot(-1.0, 4.0, 0.0),
      rot(-2.0, 6.0, 0.0),
      [vec(11, -12, 12), vec(8, -16, 18), vec(7, -8, 25)],
      [vec(-11, -12, 9), vec(-7, -16, 14), vec(-6, -8, 21)],
      leftGuardFoot,
      rightGuardFoot,
      vec(2.6, -0.8, 5.0),
      vec(-3.0, -1.0, 6.0),
      strike("none", "guard", false, 0)
    )
  ];

  const keyNames = new Map(keyFrames.map((key) => [key.frame, key.name]));

  function makeFrame(frame) {
    let a = keyFrames[0];
    let b = keyFrames[keyFrames.length - 1];
    for (let index = 0; index < keyFrames.length - 1; index += 1) {
      if (frame >= keyFrames[index].frame && frame <= keyFrames[index + 1].frame) {
        a = keyFrames[index];
        b = keyFrames[index + 1];
        break;
      }
    }

    const exact = frame === a.frame ? a : frame === b.frame ? b : null;
    if (exact) {
      const leftFoot = cloneFoot(exact.leftFoot);
      const rightFoot = cloneFoot(exact.rightFoot);
      const leftKneeBias = exact.leftKneeBias.slice();
      const rightKneeBias = exact.rightKneeBias.slice();
      return {
        frame,
        name: exact.name,
        key: true,
        phase: exact.phase,
        pelvis: exact.pelvis.slice(),
        chest: exact.chest.slice(),
        neck: exact.neck.slice(),
        head: exact.head.slice(),
        lUpper: exact.lUpper.slice(),
        lLower: exact.lLower.slice(),
        lHand: exact.lHand.slice(),
        rUpper: exact.rUpper.slice(),
        rLower: exact.rLower.slice(),
        rHand: exact.rHand.slice(),
        leftFoot,
        rightFoot,
        leftKneeBias,
        rightKneeBias,
        kneeBias: {
          left: leftKneeBias.slice(),
          right: rightKneeBias.slice()
        },
        contacts: {
          leftFoot: leftFoot.pivot === "lead_toe_pivot" ? "lead_toe_pivot" : "ground_locked",
          rightFoot: rightFoot.pivot === "rear_ball_pivot" ? "rear_ball_pivot" : "ground_locked",
          guard: exact.phase.indexOf("guard") >= 0 ? "both_hands_guarding" : "active_strike"
        },
        strike: Object.assign({}, exact.strike)
      };
    }

    const t = smoothstep((frame - a.frame) / Math.max(1, b.frame - a.frame));
    const leftFoot = interpolateFoot(a.leftFoot, b.leftFoot, t);
    const rightFoot = interpolateFoot(a.rightFoot, b.rightFoot, t);
    const leftKneeBias = lerpArray(a.leftKneeBias, b.leftKneeBias, t);
    const rightKneeBias = lerpArray(a.rightKneeBias, b.rightKneeBias, t);
    const strikeSource = t < 0.5 ? a.strike : b.strike;

    return {
      frame,
      name: "boxing_combo_inbetween",
      key: false,
      phase: t < 0.5 ? a.phase : b.phase,
      pelvis: lerpArray(a.pelvis, b.pelvis, t),
      chest: lerpArray(a.chest, b.chest, t),
      neck: lerpArray(a.neck, b.neck, t),
      head: lerpArray(a.head, b.head, t),
      lUpper: lerpArray(a.lUpper, b.lUpper, t),
      lLower: lerpArray(a.lLower, b.lLower, t),
      lHand: lerpArray(a.lHand, b.lHand, t),
      rUpper: lerpArray(a.rUpper, b.rUpper, t),
      rLower: lerpArray(a.rLower, b.rLower, t),
      rHand: lerpArray(a.rHand, b.rHand, t),
      leftFoot,
      rightFoot,
      leftKneeBias,
      rightKneeBias,
      kneeBias: {
        left: leftKneeBias.slice(),
        right: rightKneeBias.slice()
      },
      contacts: {
        leftFoot: leftFoot.pivot === "lead_toe_pivot" ? "lead_toe_pivot" : "ground_locked",
        rightFoot: rightFoot.pivot === "rear_ball_pivot" ? "rear_ball_pivot" : "ground_locked",
        guard: strikeSource.active ? "active_strike" : "guard_or_recoil"
      },
      strike: {
        hand: strikeSource.hand,
        type: strikeSource.type,
        active: strikeSource.active,
        power: round(lerp(a.strike.power, b.strike.power, t)),
        targetLine: strikeSource.targetLine
      }
    };
  }

  const frames = Array.from({ length: frameCount }, (_, frame) => makeFrame(frame));

  const data = {
    id: "boxingCombo",
    name: "Boxing Combo",
    displayName: "Boxing Combo",
    koreanName: "복싱 3연타",
    displayNames: {
      en: "Boxing Combo",
      ko: "복싱 3연타"
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
    metadata: {
      family: "combat_boxing",
      sampler: "generic_manny_keypose",
      source: "Motion request: boxing combo"
    },
    style: {
      intent: "A non-looping orthodox boxing three-hit combination: guard, lead jab, rear cross with pivot, lead hook with hip and torso rotation, then guarded recoil.",
      notes: [
        "The lead hand jabs straight down the centerline and recoils before the cross.",
        "The rear foot pivots on the ball during the cross so the hip and torso can rotate through the punch.",
        "The lead foot pivots for the hook while the rear side anchors the rotation, then both hands return to guard."
      ]
    },
    contactNotes: {
      feet: "Both feet remain planted for the full combo, with pivot metadata identifying rear-ball and lead-toe turns.",
      hands: "Strike metadata marks jab, cross, and hook active windows while guard/recoil frames keep the inactive hand near the head.",
      recovery: "The final pose returns to a guarded stance and loop is false."
    },
    footContacts: {
      left: "lead front foot planted; lead-toe pivot during hook",
      right: "rear foot planted; rear-ball pivot during cross"
    },
    keyposes: frames.filter((frame) => keyNames.has(frame.frame)),
    frames
  };

  data.keys = frames;

  global.mannyBoxingComboKeyposes = data;
})(typeof window !== "undefined" ? window : globalThis);
