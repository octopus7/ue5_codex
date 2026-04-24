(function (global) {
  "use strict";

  const frameCount = 48;
  const fps = 24;
  const durationSeconds = frameCount / fps;

  function round(value) {
    return Number(value.toFixed(3));
  }

  function vec(x, y, z) {
    return [round(x), round(y), round(z)];
  }

  function foot(side, x, z, yaw, role) {
    const yawRad = (yaw * Math.PI) / 180;
    return {
      ankle: vec(side * x, 8.1, z),
      ball: vec(side * x + Math.sin(yawRad) * 3.2, 1.05, z + 15.2),
      planted: true,
      locked: true,
      contactWeight: 1,
      pivot: role
    };
  }

  function bow(grip, top, bottom, stringHand, arrowTip, released) {
    return { grip, top, bottom, stringHand, arrowTip, released: Boolean(released) };
  }

  const keyposes = [
    {
      frame: 0,
      name: "side_stance_arrow_set",
      pelvis: vec(0, 92.4, 0).concat([round(-1), round(22), round(1)]),
      chest: [round(1), round(38), round(-1)],
      neck: [round(0), round(10), round(0)],
      head: [round(-1), round(16), round(0)],
      leftHandTarget: vec(24, 116, 27),
      rightHandTarget: vec(-10, 112, 7),
      lUpper: vec(16, -9, 14),
      lLower: vec(13, -12, 11),
      lHand: vec(7, -7, 7),
      rUpper: vec(-14, -10, 9),
      rLower: vec(-11, -13, 7),
      rHand: vec(-6, -7, 5),
      leftFoot: foot(1, 16.5, 18, 18, "front_planted"),
      rightFoot: foot(-1, 17.0, -17, -16, "rear_planted"),
      leftKneeBias: vec(3.2, -1.2, 8.0),
      rightKneeBias: vec(-3.8, -1.5, 7.0),
      archery: bow(vec(24, 116, 27), vec(27, 158, 30), vec(21, 75, 24), vec(-10, 112, 7), vec(55, 116, 55), false)
    },
    {
      frame: 8,
      name: "bow_raised_pre_draw",
      pelvis: vec(0.2, 92.0, 0.4).concat([round(-2), round(34), round(1)]),
      chest: [round(0), round(58), round(-1)],
      neck: [round(0), round(16), round(0)],
      head: [round(-1), round(25), round(0)],
      leftHandTarget: vec(31, 122, 42),
      rightHandTarget: vec(2, 120, 24),
      lUpper: vec(17, -7, 17),
      lLower: vec(15, -8, 17),
      lHand: vec(8, -5, 9),
      rUpper: vec(-15, -7, 12),
      rLower: vec(-10, -10, 10),
      rHand: vec(-5, -5, 7),
      leftFoot: foot(1, 16.8, 18, 18, "front_planted"),
      rightFoot: foot(-1, 17.1, -17, -18, "rear_planted"),
      leftKneeBias: vec(3.0, -1.0, 7.5),
      rightKneeBias: vec(-4.0, -1.4, 7.0),
      archery: bow(vec(31, 122, 42), vec(35, 166, 46), vec(27, 82, 38), vec(2, 120, 24), vec(60, 123, 67), false)
    },
    {
      frame: 16,
      name: "draw_expands",
      pelvis: vec(0.2, 91.6, 0.8).concat([round(-2), round(43), round(1)]),
      chest: [round(-1), round(72), round(-2)],
      neck: [round(0), round(20), round(0)],
      head: [round(-1), round(34), round(0)],
      leftHandTarget: vec(36, 124, 52),
      rightHandTarget: vec(-9, 123, 16),
      lUpper: vec(18, -6, 18),
      lLower: vec(16, -6, 20),
      lHand: vec(9, -4, 9),
      rUpper: vec(-16, -5, 15),
      rLower: vec(-8, -7, 15),
      rHand: vec(-4, -4, 9),
      leftFoot: foot(1, 16.8, 18, 20, "front_planted"),
      rightFoot: foot(-1, 17.1, -17, -18, "rear_planted"),
      leftKneeBias: vec(2.8, -1.1, 7.0),
      rightKneeBias: vec(-4.0, -1.4, 6.8),
      archery: bow(vec(36, 124, 52), vec(41, 169, 58), vec(31, 82, 46), vec(-9, 123, 16), vec(70, 124, 76), false)
    },
    {
      frame: 26,
      name: "full_draw_anchor",
      pelvis: vec(0.0, 91.4, 0.8).concat([round(-2), round(48), round(0)]),
      chest: [round(-2), round(82), round(-2)],
      neck: [round(0), round(24), round(0)],
      head: [round(-1), round(42), round(0)],
      leftHandTarget: vec(40, 124, 58),
      rightHandTarget: vec(-16, 124, 13),
      lUpper: vec(18, -5, 19),
      lLower: vec(17, -5, 21),
      lHand: vec(9, -3, 10),
      rUpper: vec(-17, -3, 17),
      rLower: vec(-6, -5, 17),
      rHand: vec(-3, -3, 9),
      leftFoot: foot(1, 16.8, 18, 20, "front_braced"),
      rightFoot: foot(-1, 17.1, -17, -18, "rear_grounded"),
      leftKneeBias: vec(2.8, -1.0, 6.8),
      rightKneeBias: vec(-4.1, -1.4, 6.6),
      archery: bow(vec(40, 124, 58), vec(46, 169, 65), vec(34, 81, 50), vec(-16, 124, 13), vec(77, 124, 87), false)
    },
    {
      frame: 31,
      name: "release_snap",
      pelvis: vec(0.0, 91.8, 0.8).concat([round(-1), round(50), round(0)]),
      chest: [round(-1), round(86), round(-1)],
      neck: [round(0), round(24), round(0)],
      head: [round(-1), round(44), round(0)],
      leftHandTarget: vec(41, 124, 59),
      rightHandTarget: vec(-27, 124, 9),
      lUpper: vec(18, -5, 19),
      lLower: vec(17, -5, 21),
      lHand: vec(9, -3, 10),
      rUpper: vec(-16, -2, 16),
      rLower: vec(-7, -4, 13),
      rHand: vec(-4, -3, 6),
      leftFoot: foot(1, 16.8, 18, 20, "front_braced"),
      rightFoot: foot(-1, 17.1, -17, -18, "rear_grounded"),
      leftKneeBias: vec(2.8, -1.0, 6.8),
      rightKneeBias: vec(-4.1, -1.4, 6.6),
      archery: bow(vec(41, 124, 59), vec(45, 169, 64), vec(35, 81, 51), vec(41, 124, 59), vec(105, 124, 116), true)
    },
    {
      frame: 38,
      name: "follow_through_hold",
      pelvis: vec(0.0, 92.0, 0.5).concat([round(-1), round(48), round(0)]),
      chest: [round(0), round(80), round(-1)],
      neck: [round(0), round(22), round(0)],
      head: [round(-1), round(40), round(0)],
      leftHandTarget: vec(39, 123, 56),
      rightHandTarget: vec(-30, 123, 4),
      lUpper: vec(18, -5, 18),
      lLower: vec(16, -5, 20),
      lHand: vec(9, -3, 9),
      rUpper: vec(-15, -2, 13),
      rLower: vec(-8, -4, 10),
      rHand: vec(-5, -3, 5),
      leftFoot: foot(1, 16.8, 18, 20, "front_braced"),
      rightFoot: foot(-1, 17.1, -17, -18, "rear_grounded"),
      leftKneeBias: vec(2.8, -1.0, 6.8),
      rightKneeBias: vec(-4.0, -1.4, 6.6),
      archery: bow(vec(39, 123, 56), vec(43, 167, 61), vec(34, 82, 49), vec(39, 123, 56), vec(142, 123, 146), true)
    },
    {
      frame: 47,
      name: "settled_after_shot",
      pelvis: vec(0.0, 92.3, 0.0).concat([round(0), round(36), round(0)]),
      chest: [round(1), round(62), round(0)],
      neck: [round(0), round(16), round(0)],
      head: [round(0), round(28), round(0)],
      leftHandTarget: vec(34, 120, 45),
      rightHandTarget: vec(-22, 119, 2),
      lUpper: vec(17, -7, 16),
      lLower: vec(14, -7, 17),
      lHand: vec(8, -4, 8),
      rUpper: vec(-14, -5, 11),
      rLower: vec(-9, -6, 8),
      rHand: vec(-5, -4, 4),
      leftFoot: foot(1, 16.8, 18, 18, "front_planted"),
      rightFoot: foot(-1, 17.1, -17, -16, "rear_planted"),
      leftKneeBias: vec(3.0, -1.0, 7.0),
      rightKneeBias: vec(-3.8, -1.3, 6.8),
      archery: bow(vec(34, 120, 45), vec(38, 163, 50), vec(30, 83, 39), vec(34, 120, 45), vec(170, 120, 168), true)
    }
  ];

  const data = {
    id: "archeryFullDraw",
    name: "Archery Full Draw",
    koreanName: "Archery Full Draw",
    frameCount,
    fps,
    durationSeconds,
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
      intent: "A right-handed archer sets the arrow, raises the bow, draws to cheek anchor, releases, and holds follow-through.",
      notes: [
        "The feet stay planted through the shot.",
        "The left bow arm extends and braces while the right draw hand anchors at the cheek.",
        "The bow, string, and arrow are separate prop points for web preview."
      ]
    },
    prop: {
      id: "recurve_bow_arrow",
      type: "archery_bow",
      drivenBy: "archery.grip, archery.top, archery.bottom, archery.stringHand, archery.arrowTip"
    },
    keyposes
  };

  data.frames = keyposes;
  data.keys = keyposes;

  global.mannyArcheryFullDrawKeyposes = data;
})(typeof window !== "undefined" ? window : globalThis);
