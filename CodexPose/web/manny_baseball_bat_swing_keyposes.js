(function (global) {
  "use strict";

  const frameCount = 36;
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
      ball: vec(side * x + Math.sin(yawRad) * 3.0, 1.05, z + 15.0),
      planted: true,
      locked: true,
      contactWeight: 1,
      pivot: role
    };
  }

  function pose(frame, name, pelvis, chest, neck, head, leftArm, rightArm, leftFoot, rightFoot, leftKneeBias, rightKneeBias, impact) {
    return {
      frame,
      name,
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
      grip: {
        style: "two_hands_close_on_handle",
        note: "Hands are driven by mirrored inward arm offsets so left/right ownership does not flip."
      },
      impact: Boolean(impact)
    };
  }

  const keyposes = [
    pose(
      0,
      "right_hand_batter_stance",
      vec(0.0, 92.4, 0.0).concat([round(-3), round(-34), round(2)]),
      [round(4), round(-52), round(-2)],
      [round(-1), round(12), round(0)],
      [round(-2), round(18), round(0)],
      [vec(-11, 5, -7), vec(-10, 2, -5), vec(-8, -1, -4)],
      [vec(4, 4, -6), vec(3, 2, -4), vec(2, -1, -3)],
      foot(1, 16.5, 18, 8, "front_planted"),
      foot(-1, 17.0, -14, -10, "rear_loaded"),
      vec(3.4, -1.8, 9.5),
      vec(-5.5, -2.5, 12.5)
    ),
    pose(
      6,
      "compact_coil",
      vec(-1.2, 91.6, -1.5).concat([round(-5), round(-68), round(5)]),
      [round(6), round(-96), round(-4)],
      [round(-1), round(22), round(1)],
      [round(-2), round(34), round(1)],
      [vec(-14, 7, -10), vec(-12, 3, -7), vec(-8, -1, -5)],
      [vec(2, 5, -8), vec(2, 2, -6), vec(1, -1, -4)],
      foot(1, 16.7, 18, 10, "front_planted"),
      foot(-1, 17.2, -14, -18, "rear_coiled"),
      vec(4.0, -2.2, 10.2),
      vec(-7.2, -3.0, 14.0)
    ),
    pose(
      11,
      "front_heel_plant_hips_fire",
      vec(0.8, 90.8, 1.0).concat([round(-4), round(-8), round(-1)]),
      [round(4), round(-36), round(-3)],
      [round(-1), round(9), round(0)],
      [round(-2), round(16), round(0)],
      [vec(-8, 0, 2), vec(-8, -3, 5), vec(-7, -2, 4)],
      [vec(7, 0, 2), vec(7, -3, 5), vec(6, -2, 4)],
      foot(1, 16.8, 18, 18, "front_braced"),
      foot(-1, 17.1, -14, -38, "rear_pivot"),
      vec(5.8, -2.6, 11.0),
      vec(-7.8, -1.8, 10.8)
    ),
    pose(
      15,
      "contact_through_zone",
      vec(1.5, 91.5, 2.2).concat([round(-2), round(54), round(-5)]),
      [round(2), round(76), round(-8)],
      [round(-1), round(-16), round(2)],
      [round(-2), round(-24), round(2)],
      [vec(-2, -4, 9), vec(-2, -5, 11), vec(-2, -3, 7)],
      [vec(12, -4, 8), vec(12, -5, 10), vec(8, -3, 7)],
      foot(1, 16.8, 18, 20, "front_braced"),
      foot(-1, 16.9, -14, -64, "rear_toe_pivot"),
      vec(6.8, -2.4, 10.0),
      vec(-4.0, -0.5, 8.0),
      true
    ),
    pose(
      20,
      "long_extension",
      vec(1.0, 92.2, 2.0).concat([round(1), round(98), round(-7)]),
      [round(0), round(130), round(-8)],
      [round(-1), round(-24), round(1)],
      [round(-2), round(-36), round(1)],
      [vec(4, -3, 11), vec(4, -5, 12), vec(3, -3, 8)],
      [vec(15, -3, 10), vec(13, -5, 12), vec(9, -3, 8)],
      foot(1, 16.8, 18, 22, "front_braced"),
      foot(-1, 16.8, -14, -86, "rear_pivoted"),
      vec(5.8, -1.8, 8.8),
      vec(-1.5, 0.0, 6.5)
    ),
    pose(
      28,
      "wrapped_follow_through",
      vec(0.0, 92.6, 0.8).concat([round(2), round(138), round(-4)]),
      [round(-3), round(176), round(-3)],
      [round(0), round(-32), round(0)],
      [round(-2), round(-46), round(0)],
      [vec(0, 7, 6), vec(-4, 4, 7), vec(-5, 0, 5)],
      [vec(8, 7, 7), vec(8, 4, 7), vec(6, 0, 5)],
      foot(1, 16.8, 18, 20, "front_braced"),
      foot(-1, 16.5, -14, -100, "rear_finished_pivot"),
      vec(4.0, -1.0, 7.0),
      vec(1.8, 0.5, 4.5)
    ),
    pose(
      35,
      "balanced_finish",
      vec(-0.4, 92.2, 0.2).concat([round(1), round(156), round(-2)]),
      [round(-2), round(188), round(-1)],
      [round(0), round(-38), round(0)],
      [round(-2), round(-54), round(0)],
      [vec(-5, 10, 2), vec(-6, 6, 4), vec(-5, 1, 3)],
      [vec(4, 10, 3), vec(5, 6, 4), vec(4, 1, 3)],
      foot(1, 16.8, 18, 18, "front_braced"),
      foot(-1, 16.2, -14, -104, "rear_finished_pivot"),
      vec(3.4, -0.8, 6.5),
      vec(2.0, 0.8, 4.0)
    )
  ];

  const data = {
    id: "baseballBatSwing",
    name: "Baseball Bat Swing",
    koreanName: "야구 배트 스윙",
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
      intent: "A heavy right-handed baseball bat swing: compact coil, heel plant, fast contact, long extension, and wrapped follow-through.",
      notes: [
        "Both feet stay planted while the rear foot pivots through the swing.",
        "Left and right arms keep their ownership; hands stay close through mirrored inward offsets.",
        "The bat prop is driven from the rendered hands and body axes to avoid world-space left/right flips."
      ]
    },
    prop: {
      id: "wood_bat",
      type: "baseball_bat",
      drivenBy: "rendered hand center plus body face/right/up axes"
    },
    keyposes
  };

  data.frames = keyposes;
  data.keys = keyposes;

  global.mannyBaseballBatSwingKeyposes = data;
})(typeof window !== "undefined" ? window : globalThis);
