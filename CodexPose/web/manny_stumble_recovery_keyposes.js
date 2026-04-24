(function (global) {
  "use strict";

  const frameCount = 42;
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

  function sameArray(a, b) {
    return a.every((value, index) => Math.abs(value - b[index]) < 0.001);
  }

  function foot(x, z, yaw, options) {
    const settings = options || {};
    const yawRad = (yaw * Math.PI) / 180;
    const planted = Boolean(settings.planted);
    return {
      ankle: vec(x, settings.ankleY ?? 8.1, z),
      ball: vec(x + Math.sin(yawRad) * 3.0, settings.ballY ?? 1.05, z + 15.0),
      planted,
      locked: Boolean(settings.locked),
      contactWeight: round(settings.contactWeight ?? (planted ? 1 : 0)),
      role: settings.role || (planted ? "ground" : "air"),
      drag: Boolean(settings.drag),
      landing: Boolean(settings.landing),
      recovery: Boolean(settings.recovery)
    };
  }

  function cloneFoot(source) {
    return {
      ankle: source.ankle.slice(),
      ball: source.ball.slice(),
      planted: Boolean(source.planted),
      locked: Boolean(source.locked),
      contactWeight: round(source.contactWeight ?? (source.planted ? 1 : 0)),
      role: source.role,
      drag: Boolean(source.drag),
      landing: Boolean(source.landing),
      recovery: Boolean(source.recovery)
    };
  }

  function interpolateFoot(a, b, t) {
    const locked = Boolean(
      a.planted && b.planted && a.locked && b.locked &&
      sameArray(a.ankle, b.ankle) && sameArray(a.ball, b.ball)
    );
    const source = t < 0.5 ? a : b;
    return {
      ankle: locked ? a.ankle.slice() : lerpArray(a.ankle, b.ankle, t),
      ball: locked ? a.ball.slice() : lerpArray(a.ball, b.ball, t),
      planted: locked || (a.planted && b.planted),
      locked,
      contactWeight: locked ? 1 : round(lerp(a.contactWeight ?? 0, b.contactWeight ?? 0, t)),
      role: source.role,
      drag: Boolean((a.drag || b.drag) && !locked),
      landing: Boolean((a.landing || b.landing) && t > 0.55),
      recovery: Boolean(a.recovery || b.recovery)
    };
  }

  function footContact(source) {
    if (source.planted && source.locked && source.landing) {
      return "locked_landing";
    }
    if (source.planted && source.locked) {
      return "locked_ground";
    }
    if (source.drag) {
      return "toe_drag";
    }
    if (source.contactWeight > 0.2) {
      return "light_contact";
    }
    return "air";
  }

  function pose(frame, name, phase, pelvis, chest, neck, head, leftArm, rightArm, leftFoot, rightFoot, leftKneeBias, rightKneeBias, balance) {
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
      balance
    };
  }

  const leftStart = foot(10.8, 0.0, -4, { planted: true, locked: true, role: "left_planted_start" });
  const rightStart = foot(-10.8, 2.0, 6, { planted: true, locked: true, role: "right_planted_start" });
  const rightCatch = foot(-9.0, 14.0, 2, {
    planted: false,
    contactWeight: 0.35,
    ankleY: 6.2,
    ballY: 0.65,
    role: "right_toe_catch",
    drag: true
  });
  const rightReach = foot(-12.2, 27.5, 5, {
    planted: false,
    contactWeight: 0.08,
    ankleY: 14.0,
    ballY: 6.5,
    role: "right_recovery_step_air"
  });
  const rightPlanted = foot(-12.2, 27.5, 5, {
    planted: true,
    locked: true,
    contactWeight: 1,
    role: "right_recovery_step_locked",
    landing: true,
    recovery: true
  });
  const leftUnweighted = foot(8.5, 11.5, -6, {
    planted: false,
    contactWeight: 0.05,
    ankleY: 12.2,
    ballY: 5.0,
    role: "left_recenter_air",
    recovery: true
  });
  const leftRecovered = foot(10.6, 17.4, -3, {
    planted: true,
    locked: true,
    contactWeight: 1,
    role: "left_recovered_locked",
    landing: true,
    recovery: true
  });

  const keyFrames = [
    pose(
      0,
      "stable_stance",
      "balanced_start",
      vec(0.0, 92.5, 0.0).concat(rot(-2.0, 0.0, 0.0)),
      rot(3.0, 0.0, 0.0),
      rot(-1.0, 0.0, 0.0),
      rot(-2.0, 0.0, 0.0),
      [vec(14, -12, 9), vec(9, -18, 11), vec(6, -10, 18)],
      [vec(-14, -12, 9), vec(-9, -18, 11), vec(-6, -10, 18)],
      leftStart,
      rightStart,
      vec(2.1, -0.4, 3.2),
      vec(-2.1, -0.4, 3.1),
      { centerOfMass: vec(0.0, 92.0, 6.5), correction: "neutral", support: "both" }
    ),
    pose(
      5,
      "right_toe_catches",
      "misstep_drag",
      vec(-2.3, 90.2, 4.8).concat(rot(-9.0, 4.0, 6.0)),
      rot(13.0, -8.0, -10.0),
      rot(-4.0, 3.0, 2.0),
      rot(-7.0, 6.0, 3.0),
      [vec(22, -8, 12), vec(18, -10, 21), vec(17, -5, 32)],
      [vec(-25, -7, 6), vec(-22, -9, 11), vec(-24, -5, 22)],
      leftStart,
      rightCatch,
      vec(3.0, -0.2, 4.0),
      vec(-4.8, 1.2, 2.0),
      { centerOfMass: vec(-4.0, 88.5, 13.0), correction: "falling_forward_right", support: "left" }
    ),
    pose(
      9,
      "torso_lurch_counterbalance",
      "largest_lurch",
      vec(-5.4, 87.4, 10.6).concat(rot(-17.0, 11.0, 12.0)),
      rot(23.0, -18.0, -17.0),
      rot(-7.0, 7.0, 3.0),
      rot(-11.0, 10.0, 4.0),
      [vec(31, -5, 8), vec(29, -6, 18), vec(30, -2, 31)],
      [vec(-32, -5, 0), vec(-31, -6, 7), vec(-34, -3, 18)],
      leftStart,
      foot(-8.0, 19.0, 3, {
        planted: false,
        contactWeight: 0.28,
        ankleY: 7.0,
        ballY: 0.75,
        role: "right_toe_dragging_forward",
        drag: true
      }),
      vec(3.7, 0.0, 4.8),
      vec(-5.8, 1.5, 1.4),
      { centerOfMass: vec(-6.5, 86.8, 19.5), correction: "arms_wide_to_slow_fall", support: "left" }
    ),
    pose(
      14,
      "recovery_step_reaches",
      "right_step_air",
      vec(-2.4, 88.3, 15.4).concat(rot(-12.0, -2.0, 7.0)),
      rot(16.0, 4.0, -8.0),
      rot(-5.0, -1.0, 2.0),
      rot(-8.0, -2.0, 2.0),
      [vec(26, -8, 7), vec(23, -11, 12), vec(20, -8, 21)],
      [vec(-28, -7, 12), vec(-24, -10, 22), vec(-19, -7, 33)],
      leftStart,
      rightReach,
      vec(3.4, -0.2, 4.3),
      vec(-7.0, 3.5, 5.2),
      { centerOfMass: vec(-3.6, 87.8, 23.0), correction: "step_searches_for_support", support: "left" }
    ),
    pose(
      18,
      "right_step_plants_catch",
      "right_landing",
      vec(-3.2, 89.2, 18.2).concat(rot(-8.0, -5.0, 5.0)),
      rot(9.0, 7.0, -6.0),
      rot(-3.0, -2.0, 1.0),
      rot(-5.0, -3.0, 1.0),
      [vec(21, -10, 9), vec(18, -13, 14), vec(15, -9, 20)],
      [vec(-23, -9, 15), vec(-19, -12, 24), vec(-15, -8, 34)],
      leftStart,
      rightPlanted,
      vec(3.1, -0.2, 4.0),
      vec(-3.4, -0.4, 4.8),
      { centerOfMass: vec(-3.0, 89.0, 24.0), correction: "right_foot_catches_mass", support: "right" }
    ),
    pose(
      23,
      "center_of_mass_over_new_support",
      "recenter",
      vec(-0.8, 90.5, 20.0).concat(rot(-5.0, 1.0, 1.5)),
      rot(6.0, 2.0, -2.0),
      rot(-2.0, 0.0, 0.5),
      rot(-4.0, 0.0, 0.5),
      [vec(18, -11, 10), vec(14, -15, 13), vec(10, -10, 18)],
      [vec(-19, -10, 12), vec(-14, -14, 16), vec(-9, -10, 22)],
      leftUnweighted,
      rightPlanted,
      vec(5.0, 2.8, 3.4),
      vec(-2.6, -0.6, 4.4),
      { centerOfMass: vec(-0.6, 90.0, 25.0), correction: "mass_moves_back_inside_base", support: "right" }
    ),
    pose(
      29,
      "left_foot_replants_under_body",
      "left_replant",
      vec(0.2, 91.2, 21.2).concat(rot(-3.0, 0.0, -0.8)),
      rot(4.0, 1.0, 0.8),
      rot(-1.0, 0.0, 0.0),
      rot(-2.5, 0.0, 0.0),
      [vec(16, -12, 9), vec(11, -17, 11), vec(7, -10, 18)],
      [vec(-16, -12, 10), vec(-11, -17, 12), vec(-7, -10, 19)],
      leftRecovered,
      rightPlanted,
      vec(2.6, -0.4, 3.6),
      vec(-2.5, -0.4, 4.0),
      { centerOfMass: vec(0.0, 91.0, 23.0), correction: "two_feet_support_again", support: "both" }
    ),
    pose(
      35,
      "balance_settle",
      "settle",
      vec(0.1, 92.0, 21.8).concat(rot(-2.0, 0.0, -0.2)),
      rot(3.2, 0.0, 0.2),
      rot(-1.0, 0.0, 0.0),
      rot(-2.0, 0.0, 0.0),
      [vec(14.5, -12, 9), vec(9.5, -18, 11), vec(6.2, -10, 18)],
      [vec(-14.5, -12, 9), vec(-9.5, -18, 11), vec(-6.2, -10, 18)],
      leftRecovered,
      rightPlanted,
      vec(2.2, -0.4, 3.3),
      vec(-2.2, -0.4, 3.4),
      { centerOfMass: vec(0.0, 91.8, 23.0), correction: "damped_sway", support: "both" }
    ),
    pose(
      41,
      "stable_stance_recovered",
      "recovered",
      vec(0.0, 92.5, 21.8).concat(rot(-1.5, 0.0, 0.0)),
      rot(2.8, 0.0, 0.0),
      rot(-1.0, 0.0, 0.0),
      rot(-2.0, 0.0, 0.0),
      [vec(14, -12, 9), vec(9, -18, 11), vec(6, -10, 18)],
      [vec(-14, -12, 9), vec(-9, -18, 11), vec(-6, -10, 18)],
      leftRecovered,
      rightPlanted,
      vec(2.1, -0.4, 3.2),
      vec(-2.1, -0.4, 3.2),
      { centerOfMass: vec(0.0, 92.0, 23.0), correction: "stable_after_step", support: "both" }
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
          leftFoot: footContact(leftFoot),
          rightFoot: footContact(rightFoot),
          supportFoot: exact.balance.support,
          centerOfMass: exact.balance.centerOfMass.slice()
        },
        balance: {
          centerOfMass: exact.balance.centerOfMass.slice(),
          correction: exact.balance.correction,
          support: exact.balance.support
        }
      };
    }

    const t = smoothstep((frame - a.frame) / Math.max(1, b.frame - a.frame));
    const leftFoot = interpolateFoot(a.leftFoot, b.leftFoot, t);
    const rightFoot = interpolateFoot(a.rightFoot, b.rightFoot, t);
    const leftKneeBias = lerpArray(a.leftKneeBias, b.leftKneeBias, t);
    const rightKneeBias = lerpArray(a.rightKneeBias, b.rightKneeBias, t);
    const centerOfMass = lerpArray(a.balance.centerOfMass, b.balance.centerOfMass, t);
    const support = t < 0.5 ? a.balance.support : b.balance.support;

    return {
      frame,
      name: "stumble_recovery_inbetween",
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
        leftFoot: footContact(leftFoot),
        rightFoot: footContact(rightFoot),
        supportFoot: support,
        centerOfMass: centerOfMass.slice()
      },
      balance: {
        centerOfMass,
        correction: t < 0.5 ? a.balance.correction : b.balance.correction,
        support
      }
    };
  }

  const frames = Array.from({ length: frameCount }, (_, frame) => makeFrame(frame));

  const data = {
    id: "stumbleRecovery",
    name: "Stumble Recovery",
    displayName: "Stumble Recovery",
    koreanName: "비틀거림 회복",
    displayNames: {
      en: "Stumble Recovery",
      ko: "비틀거림 회복"
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
      family: "locomotion_balance_recovery",
      sampler: "generic_manny_keypose",
      source: "Motion request: stumble recovery"
    },
    style: {
      intent: "A normal stance breaks into a right-foot toe catch and drag, the torso lurches forward, arms counterbalance, and recovery steps bring the center of mass back over a stable base.",
      notes: [
        "Planted support feet keep identical ankle and ball coordinates while locked.",
        "The right foot is marked as toe_drag during the misstep instead of planted, so the dragged contact does not count as a sliding plant.",
        "The right recovery step catches the body first, then the left foot re-centers under the torso before the final stable stance."
      ]
    },
    contactNotes: {
      initialSupport: "Both feet start locked; the left foot remains locked while the right toe catches and drags.",
      recoveryStep: "The right foot is airborne during the step search, then locks on landing at frame 18.",
      recenterStep: "The left foot releases after the catch and replants under the body at frame 29.",
      noFootSliding: "Only feet with planted=true and locked=true are treated as ground-locked contacts; moving feet are unplanted or toe-drag contacts."
    },
    footContacts: {
      left: "locked start support, airborne recenter, locked final support",
      right: "locked start, toe drag during stumble, airborne recovery step, locked catch support"
    },
    keyposes: frames.filter((frame) => keyNames.has(frame.frame)),
    frames
  };

  data.keys = frames;

  global.mannyStumbleRecoveryKeyposes = data;
})(typeof window !== "undefined" ? window : globalThis);
