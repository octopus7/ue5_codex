(function (global) {
  "use strict";

  const frameCount = 32;
  const fps = 24;
  const durationSeconds = frameCount / fps;

  function round(value) {
    return Number(value.toFixed(3));
  }

  function vec(x, y, z) {
    return [round(x), round(y), round(z)];
  }

  function rot(pitch, yaw, roll) {
    return [round(pitch), round(yaw), round(roll)];
  }

  function clamp(value, min, max) {
    return Math.max(min, Math.min(max, value));
  }

  function smoothstep(value) {
    const t = clamp(value, 0, 1);
    return t * t * (3 - t * 2);
  }

  function mix(a, b, t) {
    return a + (b - a) * t;
  }

  function mixVec(a, b, t) {
    return vec(mix(a[0], b[0], t), mix(a[1], b[1], t), mix(a[2], b[2], t));
  }

  const keyFrames = [
    {
      frame: 0,
      name: "guard_ready",
      pelvis: vec(0, 93, 0).concat(rot(-2, 0, 0)),
      chest: rot(3, 0, 0),
      neck: rot(-1, 0, 0),
      head: rot(-2, 0, 0),
      lUpper: vec(15, -12, 13),
      lLower: vec(10, -18, 10),
      lHand: vec(6, -9, 20),
      rUpper: vec(-15, -12, 12),
      rLower: vec(-9, -18, 9),
      rHand: vec(-5, -9, 20),
      leftFoot: { ankle: vec(10.5, 8.1, 0), ball: vec(11.3, 1.1, 14.8), planted: true },
      rightFoot: { ankle: vec(-10.5, 8.1, 2), ball: vec(-11.3, 1.1, 16.8), planted: true },
      leftKneeBias: vec(2, 0, 2.5),
      rightKneeBias: vec(-2, 0, 2.5)
    },
    {
      frame: 3,
      name: "short_preparation",
      pelvis: vec(2.8, 91.8, -1).concat(rot(-5, -10, -5)),
      chest: rot(6, 8, 3),
      neck: rot(-2, -3, -1),
      head: rot(-4, -5, -1),
      lUpper: vec(18, -10, 8),
      lLower: vec(14, -17, 5),
      lHand: vec(9, -8, 12),
      rUpper: vec(-13, -11, 16),
      rLower: vec(-8, -17, 13),
      rHand: vec(-4, -8, 24),
      leftFoot: { ankle: vec(10.5, 8.1, 0), ball: vec(11.3, 1.1, 14.8), planted: true, locked: true },
      rightFoot: { ankle: vec(-8.6, 7.9, 1.4), ball: vec(-9.4, 1.1, 15.2), planted: true },
      leftKneeBias: vec(2.6, 0.2, 1.6),
      rightKneeBias: vec(-1.8, 0.2, 2.2)
    },
    {
      frame: 7,
      name: "weight_shift_support_loaded",
      pelvis: vec(7.2, 90.2, -1.5).concat(rot(-8, -24, -11)),
      chest: rot(9, 16, 6),
      neck: rot(-3, -7, -2),
      head: rot(-5, -10, -2),
      lUpper: vec(20, -9, 1),
      lLower: vec(16, -17, -2),
      lHand: vec(10, -7, 5),
      rUpper: vec(-10, -10, 20),
      rLower: vec(-7, -17, 17),
      rHand: vec(-4, -7, 29),
      leftFoot: { ankle: vec(10.5, 8.1, 0), ball: vec(11.3, 1.1, 14.8), planted: true, locked: true },
      rightFoot: { ankle: vec(-13.5, 20, 0.5), ball: vec(-14.8, 9, 12), planted: false },
      leftKneeBias: vec(3.2, 0.5, 0.5),
      rightKneeBias: vec(-7, 3, 3.5)
    },
    {
      frame: 11,
      name: "knee_chamber",
      pelvis: vec(8.2, 92.5, -0.8).concat(rot(-4, -38, -15)),
      chest: rot(8, 24, 7),
      neck: rot(-3, -9, -2),
      head: rot(-4, -15, -2),
      lUpper: vec(21, -8, -4),
      lLower: vec(17, -16, -7),
      lHand: vec(11, -7, -1),
      rUpper: vec(-7, -9, 21),
      rLower: vec(-4, -15, 19),
      rHand: vec(-2, -6, 30),
      leftFoot: { ankle: vec(10.5, 8.1, 0), ball: vec(11.3, 1.1, 14.8), planted: true, locked: true },
      rightFoot: { ankle: vec(-24, 55, 2), ball: vec(-21, 48, 14), planted: false },
      leftKneeBias: vec(3.5, 0.7, -0.5),
      rightKneeBias: vec(-16, 8, 5)
    },
    {
      frame: 15,
      name: "sharp_side_extension",
      pelvis: vec(6.5, 94.5, 0.8).concat(rot(3, -55, -20)),
      chest: rot(5, 35, 10),
      neck: rot(-2, -12, -2),
      head: rot(-3, -21, -2),
      lUpper: vec(23, -8, -8),
      lLower: vec(18, -16, -11),
      lHand: vec(12, -6, -4),
      rUpper: vec(-5, -8, 18),
      rLower: vec(-2, -16, 14),
      rHand: vec(-1, -6, 24),
      leftFoot: { ankle: vec(10.5, 8.1, 0), ball: vec(11.3, 1.1, 14.8), planted: true, locked: true },
      rightFoot: { ankle: vec(-57, 58, 3.5), ball: vec(-72, 55, 2.8), planted: false, strike: true },
      leftKneeBias: vec(3.8, 0.8, -1.3),
      rightKneeBias: vec(-30, 5, 1)
    },
    {
      frame: 18,
      name: "impact_hold",
      pelvis: vec(5.6, 93.4, 1.2).concat(rot(2, -58, -23)),
      chest: rot(4, 39, 12),
      neck: rot(-2, -13, -3),
      head: rot(-3, -24, -3),
      lUpper: vec(24, -8, -9),
      lLower: vec(19, -16, -12),
      lHand: vec(13, -6, -5),
      rUpper: vec(-4, -8, 16),
      rLower: vec(-1, -16, 12),
      rHand: vec(0, -6, 21),
      leftFoot: { ankle: vec(10.5, 8.1, 0), ball: vec(11.3, 1.1, 14.8), planted: true, locked: true },
      rightFoot: { ankle: vec(-60, 57.5, 3.6), ball: vec(-75, 54.5, 2.8), planted: false, strike: true, hold: true },
      leftKneeBias: vec(3.5, 0.5, -1.5),
      rightKneeBias: vec(-32, 4, 0.5)
    },
    {
      frame: 22,
      name: "recoil_to_chamber",
      pelvis: vec(7.4, 92.2, 0).concat(rot(-3, -42, -16)),
      chest: rot(7, 27, 8),
      neck: rot(-3, -10, -2),
      head: rot(-4, -17, -2),
      lUpper: vec(21, -9, -3),
      lLower: vec(17, -17, -6),
      lHand: vec(11, -7, 0),
      rUpper: vec(-7, -9, 20),
      rLower: vec(-4, -16, 17),
      rHand: vec(-2, -7, 28),
      leftFoot: { ankle: vec(10.5, 8.1, 0), ball: vec(11.3, 1.1, 14.8), planted: true, locked: true },
      rightFoot: { ankle: vec(-25, 54, 1), ball: vec(-22, 46, 12), planted: false },
      leftKneeBias: vec(3.3, 0.6, -0.2),
      rightKneeBias: vec(-17, 7, 5)
    },
    {
      frame: 27,
      name: "heavy_recovery_step_down",
      pelvis: vec(3, 90.5, -0.8).concat(rot(-7, -18, -8)),
      chest: rot(9, 12, 4),
      neck: rot(-3, -5, -1),
      head: rot(-5, -8, -1),
      lUpper: vec(18, -10, 6),
      lLower: vec(14, -17, 3),
      lHand: vec(9, -8, 10),
      rUpper: vec(-11, -11, 17),
      rLower: vec(-7, -18, 14),
      rHand: vec(-3, -8, 25),
      leftFoot: { ankle: vec(10.5, 8.1, 0), ball: vec(11.3, 1.1, 14.8), planted: true, locked: true },
      rightFoot: { ankle: vec(-11.5, 9, 1.2), ball: vec(-12.5, 1.2, 15.8), planted: true, landing: true },
      leftKneeBias: vec(2.8, 0.3, 1),
      rightKneeBias: vec(-2.8, 0.5, 2.1)
    },
    {
      frame: 31,
      name: "guard_recovered",
      pelvis: vec(0.8, 92.7, 0).concat(rot(-2, -4, -1)),
      chest: rot(4, 3, 1),
      neck: rot(-1, -1, 0),
      head: rot(-2, -2, 0),
      lUpper: vec(15, -12, 12),
      lLower: vec(10, -18, 9),
      lHand: vec(6, -9, 19),
      rUpper: vec(-15, -12, 12),
      rLower: vec(-9, -18, 9),
      rHand: vec(-5, -9, 20),
      leftFoot: { ankle: vec(10.5, 8.1, 0), ball: vec(11.3, 1.1, 14.8), planted: true },
      rightFoot: { ankle: vec(-10.5, 8.1, 2), ball: vec(-11.3, 1.1, 16.8), planted: true },
      leftKneeBias: vec(2, 0, 2.5),
      rightKneeBias: vec(-2, 0, 2.5)
    }
  ];

  function cloneFoot(foot) {
    return {
      ankle: foot.ankle.slice(),
      ball: foot.ball.slice(),
      planted: Boolean(foot.planted),
      locked: Boolean(foot.locked),
      contactWeight: foot.planted ? 1 : 0,
      strike: Boolean(foot.strike),
      hold: Boolean(foot.hold),
      landing: Boolean(foot.landing)
    };
  }

  function interpolateFrame(frame) {
    const exactKey = keyFrames.find((keyFrame) => keyFrame.frame === frame);
    if (exactKey) {
      const leftFoot = cloneFoot(exactKey.leftFoot);
      const rightFoot = cloneFoot(exactKey.rightFoot);
      const leftKneeBias = exactKey.leftKneeBias.slice();
      const rightKneeBias = exactKey.rightKneeBias.slice();

      return {
        frame,
        name: exactKey.name,
        key: true,
        pelvis: exactKey.pelvis.slice(),
        chest: exactKey.chest.slice(),
        neck: exactKey.neck.slice(),
        head: exactKey.head.slice(),
        lUpper: exactKey.lUpper.slice(),
        lLower: exactKey.lLower.slice(),
        lHand: exactKey.lHand.slice(),
        rUpper: exactKey.rUpper.slice(),
        rLower: exactKey.rLower.slice(),
        rHand: exactKey.rHand.slice(),
        leftFoot,
        rightFoot,
        leftKneeBias,
        rightKneeBias,
        kneeBias: {
          left: leftKneeBias.slice(),
          right: rightKneeBias.slice()
        },
        contacts: {
          leftFoot: leftFoot.planted ? "locked_support" : "air",
          rightFoot: rightFoot.strike ? "side_kick_strike" : rightFoot.planted ? "ground" : "air",
          supportFoot: "left",
          kickingFoot: "right"
        }
      };
    }

    let a = keyFrames[0];
    let b = keyFrames[keyFrames.length - 1];
    for (let index = 0; index < keyFrames.length - 1; index += 1) {
      if (frame >= keyFrames[index].frame && frame <= keyFrames[index + 1].frame) {
        a = keyFrames[index];
        b = keyFrames[index + 1];
        break;
      }
    }

    const span = Math.max(1, b.frame - a.frame);
    const t = smoothstep((frame - a.frame) / span);
    const rightAir = !a.rightFoot.planted || !b.rightFoot.planted;
    const leftFoot = cloneFoot(a.leftFoot);
    const rightFoot = {
      ankle: mixVec(a.rightFoot.ankle, b.rightFoot.ankle, t),
      ball: mixVec(a.rightFoot.ball, b.rightFoot.ball, t),
      planted: !rightAir,
      locked: false,
      contactWeight: rightAir ? round(frame >= 27 ? 0.7 : 0.05) : 1,
      strike: Boolean(a.rightFoot.strike || b.rightFoot.strike),
      hold: Boolean(a.rightFoot.hold || b.rightFoot.hold),
      landing: Boolean(a.rightFoot.landing || b.rightFoot.landing)
    };

    const leftKneeBias = mixVec(a.leftKneeBias, b.leftKneeBias, t);
    const rightKneeBias = mixVec(a.rightKneeBias, b.rightKneeBias, t);

    return {
      frame,
      name: "side_kick_inbetween",
      key: false,
      pelvis: mixVec(a.pelvis.slice(0, 3), b.pelvis.slice(0, 3), t).concat(mixVec(a.pelvis.slice(3), b.pelvis.slice(3), t)),
      chest: mixVec(a.chest, b.chest, t),
      neck: mixVec(a.neck, b.neck, t),
      head: mixVec(a.head, b.head, t),
      lUpper: mixVec(a.lUpper, b.lUpper, t),
      lLower: mixVec(a.lLower, b.lLower, t),
      lHand: mixVec(a.lHand, b.lHand, t),
      rUpper: mixVec(a.rUpper, b.rUpper, t),
      rLower: mixVec(a.rLower, b.rLower, t),
      rHand: mixVec(a.rHand, b.rHand, t),
      leftFoot,
      rightFoot,
      leftKneeBias,
      rightKneeBias,
      kneeBias: {
        left: leftKneeBias.slice(),
        right: rightKneeBias.slice()
      },
      contacts: {
        leftFoot: leftFoot.planted ? "locked_support" : "air",
        rightFoot: rightFoot.strike ? "side_kick_strike" : rightFoot.planted ? "ground" : "air",
        supportFoot: "left",
        kickingFoot: "right"
      }
    };
  }

  const frames = Array.from({ length: frameCount }, (_, frame) => interpolateFrame(frame));
  const keyposes = frames.filter((frame) => frame.key || frame.frame === frameCount - 1);

  const data = {
    id: "sideKick",
    name: "Side Kick",
    koreanName: "옆차기",
    frameCount,
    fps,
    durationSeconds,
    loop: false,
    rotationUnits: "degrees",
    positionUnits: "Manny demo units",
    coordinateSystem: {
      up: "Y",
      forward: "Z",
      side: "X"
    },
    style: {
      intent: "A realistic heavy right-leg side kick with grounded support, sharp extension, impact hold, recoil, and weighted recovery.",
      notes: [
        "The left support foot remains planted through the load, chamber, strike, and recoil to avoid sliding.",
        "The right foot travels from guard to chamber, extends toward negative X for the side kick, holds impact briefly, then retracts before landing.",
        "Pelvis yaw and roll load over the support leg, while chest, head, and guard arms counterbalance the attack."
      ]
    },
    contactNotes: {
      support: "Left foot is the locked support foot from preparation through recoil.",
      strike: "Right ball target is offset farther along negative X during extension so the foot presents a side-kick blade.",
      recovery: "Final frame returns near the opening guard, but loop is false because this is a non-looping attack."
    },
    keyposes,
    frames
  };

  data.keys = frames;

  global.mannySideKickKeyposes = data;
})(typeof window !== "undefined" ? window : globalThis);
