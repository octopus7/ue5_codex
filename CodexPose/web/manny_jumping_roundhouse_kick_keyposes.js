(function (global) {
  "use strict";

  const frameCount = 36;
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

  function pelvis(x, y, z, pitch, yaw, roll) {
    return [round(x), round(y), round(z), round(pitch), round(yaw), round(roll)];
  }

  function lerpVec(a, b, t) {
    return vec(lerp(a[0], b[0], t), lerp(a[1], b[1], t), lerp(a[2], b[2], t));
  }

  function makeFoot(ankle, ball, planted, contactWeight, role) {
    return {
      ankle,
      ball,
      planted,
      locked: planted && contactWeight > 0.82,
      contactWeight: round(contactWeight),
      role
    };
  }

  const poseKeys = [
    {
      frame: 0,
      name: "guarded_stance",
      pelvis: pelvis(0, 92.2, 0, -2, -8, 0),
      chest: [4, 10, 0],
      neck: [-1, -3, 0],
      head: [-2, -6, 0],
      lUpper: vec(13, -13, 15),
      lLower: vec(7, -19, 10),
      lHand: vec(5, -11, 18),
      rUpper: vec(-12, -13, 18),
      rLower: vec(-7, -19, 12),
      rHand: vec(-5, -10, 20),
      leftFoot: makeFoot(vec(10.5, 8.1, 1.5), vec(11.2, 1.1, 16.1), true, 1, "support"),
      rightFoot: makeFoot(vec(-10.5, 8.1, -2.5), vec(-10.0, 1.1, 12.0), true, 1, "rear_ready"),
      leftKneeBias: vec(2.5, -1.2, 12),
      rightKneeBias: vec(-2.5, -1.1, 11)
    },
    {
      frame: 4,
      name: "deep_crouched_load",
      pelvis: pelvis(-1.5, 82.4, -2.3, -10, -18, 3),
      chest: [10, 18, -3],
      neck: [-4, -5, 1],
      head: [-5, -8, 1],
      lUpper: vec(15, -10, 18),
      lLower: vec(8, -17, 13),
      lHand: vec(7, -8, 20),
      rUpper: vec(-13, -10, 20),
      rLower: vec(-7, -17, 15),
      rHand: vec(-5, -8, 23),
      leftFoot: makeFoot(vec(10.3, 8.0, 0.4), vec(11.1, 1.0, 15.0), true, 1, "compressed_support"),
      rightFoot: makeFoot(vec(-10.8, 8.0, -4.5), vec(-10.2, 1.0, 10.0), true, 1, "compressed_push"),
      leftKneeBias: vec(4.4, -2.0, 14.5),
      rightKneeBias: vec(-4.6, -2.1, 13.5)
    },
    {
      frame: 8,
      name: "explosive_takeoff",
      pelvis: pelvis(-1.0, 96.5, 1.0, -5, -34, 1),
      chest: [5, 25, -6],
      neck: [-2, -8, 1],
      head: [-3, -11, 1],
      lUpper: vec(18, -7, 10),
      lLower: vec(10, -14, 8),
      lHand: vec(8, -6, 15),
      rUpper: vec(-16, -8, 23),
      rLower: vec(-9, -15, 19),
      rHand: vec(-7, -6, 25),
      leftFoot: makeFoot(vec(10.4, 8.4, 0.1), vec(11.5, 1.4, 14.7), true, 0.82, "toe_drive"),
      rightFoot: makeFoot(vec(-9.0, 14.2, -1.0), vec(-8.4, 5.2, 12.0), false, 0.22, "leaving_ground"),
      leftKneeBias: vec(3.7, 0.8, 16),
      rightKneeBias: vec(-5.6, 3.0, 16)
    },
    {
      frame: 11,
      name: "airborne_hip_turn_chamber",
      pelvis: pelvis(-0.8, 108.2, 5.0, 1, -58, -2),
      chest: [1, 33, -8],
      neck: [-1, -10, 2],
      head: [-2, -13, 2],
      lUpper: vec(19, -8, -1),
      lLower: vec(11, -15, 3),
      lHand: vec(9, -6, 9),
      rUpper: vec(-18, -7, 25),
      rLower: vec(-11, -13, 23),
      rHand: vec(-8, -5, 29),
      leftFoot: makeFoot(vec(8.0, 31.0, -2.0), vec(8.5, 22.0, 10.5), false, 0, "air_tuck"),
      rightFoot: makeFoot(vec(-6.0, 55.0, 9.0), vec(-4.0, 47.0, 22.0), false, 0, "knee_chamber"),
      leftKneeBias: vec(5.0, 9.0, 9.0),
      rightKneeBias: vec(-9.5, 14.0, 18.0)
    },
    {
      frame: 15,
      name: "roundhouse_extension",
      pelvis: pelvis(0.8, 114.0, 9.0, 5, -82, -5),
      chest: [-1, 43, -10],
      neck: [0, -13, 2],
      head: [-1, -17, 2],
      lUpper: vec(17, -11, -8),
      lLower: vec(9, -17, -2),
      lHand: vec(8, -8, 4),
      rUpper: vec(-19, -9, 22),
      rLower: vec(-12, -15, 25),
      rHand: vec(-10, -7, 32),
      leftFoot: makeFoot(vec(6.0, 40.0, -4.0), vec(6.4, 31.5, 8.0), false, 0, "air_counter_tuck"),
      rightFoot: makeFoot(vec(-35.0, 83.0, 25.0), vec(-42.0, 78.0, 35.0), false, 0, "kicking_extension"),
      leftKneeBias: vec(7.0, 13.0, 4.0),
      rightKneeBias: vec(-20.0, 17.0, 28.0)
    },
    {
      frame: 18,
      name: "air_impact_peak",
      pelvis: pelvis(2.0, 112.5, 11.5, 4, -94, -7),
      chest: [-3, 51, -12],
      neck: [0, -15, 2],
      head: [-1, -20, 2],
      lUpper: vec(18, -10, -12),
      lLower: vec(10, -17, -5),
      lHand: vec(9, -8, 1),
      rUpper: vec(-20, -8, 18),
      rLower: vec(-13, -13, 22),
      rHand: vec(-11, -6, 30),
      leftFoot: makeFoot(vec(8.0, 38.5, -6.0), vec(8.0, 30.0, 6.0), false, 0, "air_counter_tuck"),
      rightFoot: makeFoot(vec(-40.0, 86.0, 31.0), vec(-48.0, 82.0, 40.0), false, 0, "impact_line"),
      leftKneeBias: vec(8.0, 12.0, 2.0),
      rightKneeBias: vec(-23.0, 18.0, 32.0)
    },
    {
      frame: 22,
      name: "recoil_rechamber",
      pelvis: pelvis(1.2, 105.5, 9.0, 0, -74, -4),
      chest: [2, 35, -7],
      neck: [-1, -10, 1],
      head: [-2, -14, 1],
      lUpper: vec(16, -12, -3),
      lLower: vec(9, -18, 2),
      lHand: vec(8, -9, 8),
      rUpper: vec(-17, -10, 20),
      rLower: vec(-10, -16, 19),
      rHand: vec(-8, -8, 26),
      leftFoot: makeFoot(vec(9.0, 31.0, -3.0), vec(9.4, 22.5, 9.0), false, 0, "landing_search"),
      rightFoot: makeFoot(vec(-14.0, 60.0, 12.0), vec(-12.0, 52.0, 25.0), false, 0, "recoil_chamber"),
      leftKneeBias: vec(6.0, 8.0, 8.0),
      rightKneeBias: vec(-9.0, 12.0, 18.0)
    },
    {
      frame: 26,
      name: "heavy_landing_contact",
      pelvis: pelvis(0.5, 86.0, 4.0, -8, -46, -2),
      chest: [8, 26, -5],
      neck: [-3, -8, 1],
      head: [-4, -11, 1],
      lUpper: vec(14, -12, 10),
      lLower: vec(8, -19, 8),
      lHand: vec(7, -10, 14),
      rUpper: vec(-15, -11, 20),
      rLower: vec(-9, -18, 16),
      rHand: vec(-7, -9, 23),
      leftFoot: makeFoot(vec(9.4, 8.0, 2.0), vec(10.0, 1.0, 16.2), true, 0.95, "primary_landing"),
      rightFoot: makeFoot(vec(-8.8, 12.0, 7.0), vec(-8.0, 4.0, 20.5), false, 0.28, "following_down"),
      leftKneeBias: vec(4.5, -1.5, 15.5),
      rightKneeBias: vec(-5.5, 1.5, 15.0)
    },
    {
      frame: 30,
      name: "absorbed_two_foot_landing",
      pelvis: pelvis(0, 81.0, 1.0, -12, -26, 0),
      chest: [12, 20, -2],
      neck: [-4, -6, 0],
      head: [-5, -8, 0],
      lUpper: vec(13, -12, 15),
      lLower: vec(7, -19, 11),
      lHand: vec(6, -10, 17),
      rUpper: vec(-13, -12, 18),
      rLower: vec(-7, -19, 14),
      rHand: vec(-6, -10, 21),
      leftFoot: makeFoot(vec(9.7, 8.0, 1.2), vec(10.4, 1.0, 15.8), true, 1, "absorbing"),
      rightFoot: makeFoot(vec(-9.8, 8.0, 2.0), vec(-9.0, 1.0, 16.5), true, 0.88, "secondary_landing"),
      leftKneeBias: vec(4.8, -2.4, 14.5),
      rightKneeBias: vec(-4.8, -2.2, 14.0)
    },
    {
      frame: 35,
      name: "guard_recovered",
      pelvis: pelvis(0, 91.5, 0, -3, -12, 0),
      chest: [5, 12, 0],
      neck: [-1, -4, 0],
      head: [-2, -6, 0],
      lUpper: vec(13, -13, 16),
      lLower: vec(7, -19, 11),
      lHand: vec(5, -11, 18),
      rUpper: vec(-12, -13, 19),
      rLower: vec(-7, -19, 13),
      rHand: vec(-5, -10, 21),
      leftFoot: makeFoot(vec(10.4, 8.1, 1.0), vec(11.1, 1.1, 15.7), true, 1, "recovered_support"),
      rightFoot: makeFoot(vec(-10.2, 8.1, -0.8), vec(-9.5, 1.1, 13.8), true, 1, "recovered_guard"),
      leftKneeBias: vec(2.5, -1.2, 12),
      rightKneeBias: vec(-2.5, -1.1, 11.5)
    }
  ];

  function getSpan(frame) {
    for (let i = 0; i < poseKeys.length - 1; i += 1) {
      if (frame >= poseKeys[i].frame && frame <= poseKeys[i + 1].frame) {
        return [poseKeys[i], poseKeys[i + 1]];
      }
    }
    return [poseKeys[poseKeys.length - 1], poseKeys[poseKeys.length - 1]];
  }

  function lerpArray(a, b, t) {
    return a.map((value, index) => round(lerp(value, b[index], t)));
  }

  function lerpFoot(a, b, t, frame) {
    const airborne = frame >= 10 && frame <= 24;
    const planted = !airborne && a.planted && b.planted;
    const contactWeight = airborne ? 0 : lerp(a.contactWeight, b.contactWeight, t);
    return makeFoot(lerpVec(a.ankle, b.ankle, t), lerpVec(a.ball, b.ball, t), planted, contactWeight, planted ? a.role : b.role);
  }

  const keyNames = new Map(poseKeys.map((pose) => [pose.frame, pose.name]));

  function makeFrame(frame) {
    const [a, b] = getSpan(frame);
    const span = Math.max(1, b.frame - a.frame);
    const t = smoothstep((frame - a.frame) / span);
    const leftFoot = lerpFoot(a.leftFoot, b.leftFoot, t, frame);
    const rightFoot = lerpFoot(a.rightFoot, b.rightFoot, t, frame);
    const leftKneeBias = lerpVec(a.leftKneeBias, b.leftKneeBias, t);
    const rightKneeBias = lerpVec(a.rightKneeBias, b.rightKneeBias, t);

    return {
      frame,
      name: keyNames.get(frame) || "jumping_roundhouse_inbetween",
      key: keyNames.has(frame),
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
        left: leftKneeBias,
        right: rightKneeBias
      },
      contacts: {
        leftFoot: leftFoot.planted ? "ground" : "air",
        rightFoot: rightFoot.planted ? "ground" : "air",
        phase: frame >= 10 && frame <= 24 ? "airborne" : frame >= 26 && frame <= 31 ? "heavy_landing" : "grounded"
      }
    };
  }

  const frames = Array.from({ length: frameCount }, (_, frame) => makeFrame(frame));

  const data = {
    id: "jumpingRoundhouseKick",
    name: "Jumping Roundhouse Kick",
    koreanName: "점프 돌려차기",
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
      intent: "A heavy, realistic jumping roundhouse kick with crouched load, airborne hip turn, impact, recoil, and weighted landing absorption.",
      notes: [
        "The airborne span from frames 10 through 24 deliberately has no foot contacts.",
        "The right leg chambers before extension so the strike reads as hip-driven instead of a straight-leg lift.",
        "Landing drops the pelvis below the starting height and keeps both knees biased forward to sell weight absorption.",
        "Arms remain in guard and counterbalance the spin without rising into an exaggerated pose."
      ]
    },
    contactNotes: {
      takeoff: "Both feet compress, then the right foot leaves first while the left toe finishes the drive.",
      airborne: "Frames 10-24 expose air contacts for both feet.",
      landing: "Left foot catches the landing first, then the right foot follows into a two-foot absorbed guard."
    },
    keyposes: frames.filter((frame) => frame.key),
    frames
  };

  data.keys = frames;

  global.mannyJumpingRoundhouseKickKeyposes = data;
})(typeof window !== "undefined" ? window : globalThis);
