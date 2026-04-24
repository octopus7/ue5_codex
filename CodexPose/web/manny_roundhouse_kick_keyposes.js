(function (global) {
  "use strict";

  const frameCount = 28;
  const fps = 24;
  const durationSeconds = frameCount / fps;

  function clamp(value, min, max) {
    return Math.max(min, Math.min(max, value));
  }

  function smoothstep(value) {
    const t = clamp(value, 0, 1);
    return t * t * (3 - t * 2);
  }

  function round(value) {
    return Number(value.toFixed(3));
  }

  function vec(x, y, z) {
    return [round(x), round(y), round(z)];
  }

  function lerp(a, b, t) {
    return a + (b - a) * t;
  }

  function lerpVec(a, b, t) {
    return a.map((value, index) => round(lerp(value, b[index], t)));
  }

  function sampleTrack(track, frame) {
    for (let i = 0; i < track.length - 1; i += 1) {
      const a = track[i];
      const b = track[i + 1];
      if (frame >= a.frame && frame <= b.frame) {
        const t = smoothstep((frame - a.frame) / Math.max(1, b.frame - a.frame));
        const sample = {};
        Object.keys(a).forEach((key) => {
          if (key !== "frame" && key !== "name") {
            sample[key] = Array.isArray(a[key]) ? lerpVec(a[key], b[key], t) : round(lerp(a[key], b[key], t));
          }
        });
        return sample;
      }
    }
    return track[track.length - 1];
  }

  const poseKeys = [
    {
      frame: 0,
      name: "guarded_stance",
      pelvis: [0.0, 93.2, 0.0, -2.0, -10.0, 2.0],
      chest: [3.0, 8.0, -1.0],
      neck: [-1.0, -2.0, 0.0],
      head: [-2.0, -4.0, 0.0],
      lUpper: [14.0, -12.0, 20.0],
      lLower: [8.0, -17.0, 12.0],
      lHand: [3.0, -8.0, 9.0],
      rUpper: [-13.0, -12.0, 18.0],
      rLower: [-7.0, -16.0, 11.0],
      rHand: [-3.0, -8.0, 8.0],
      rightAnkle: [-9.2, 8.2, -4.0],
      rightBall: [-8.5, 1.0, 10.5],
      supportYaw: -8,
      supportRoll: 0
    },
    {
      frame: 5,
      name: "load_pivot",
      pelvis: [-1.8, 91.4, -1.0, -7.0, 34.0, 7.0],
      chest: [7.0, -14.0, -5.0],
      neck: [-1.5, -16.0, 1.0],
      head: [-2.0, -20.0, 1.0],
      lUpper: [15.5, -10.0, 17.0],
      lLower: [8.5, -15.0, 8.0],
      lHand: [2.0, -7.0, 5.0],
      rUpper: [-17.0, -10.0, 10.0],
      rLower: [-14.0, -14.0, 0.0],
      rHand: [-12.0, -8.0, -3.0],
      rightAnkle: [-11.0, 8.6, -7.5],
      rightBall: [-10.0, 1.4, 6.0],
      supportYaw: 38,
      supportRoll: -2
    },
    {
      frame: 9,
      name: "knee_chamber",
      pelvis: [-2.8, 94.0, 0.5, -3.0, 112.0, 10.0],
      chest: [3.0, 54.0, -6.0],
      neck: [-1.0, -18.0, 1.0],
      head: [-2.0, -24.0, 1.0],
      lUpper: [17.0, -9.0, 24.0],
      lLower: [10.0, -13.0, 17.0],
      lHand: [4.0, -6.0, 14.0],
      rUpper: [-19.0, -9.0, 3.0],
      rLower: [-18.0, -13.0, -8.0],
      rHand: [-16.0, -7.0, -10.0],
      rightAnkle: [-6.5, 38.0, 5.0],
      rightBall: [-3.0, 31.0, 18.0],
      supportYaw: 116,
      supportRoll: -3
    },
    {
      frame: 14,
      name: "shin_whip",
      pelvis: [-1.2, 95.2, 1.0, 2.0, 214.0, 6.0],
      chest: [1.0, 126.0, -5.0],
      neck: [-1.0, -30.0, 1.0],
      head: [-2.0, -36.0, 1.0],
      lUpper: [18.0, -8.0, 28.0],
      lLower: [12.0, -12.0, 20.0],
      lHand: [6.0, -5.0, 16.0],
      rUpper: [-21.0, -10.0, -2.0],
      rLower: [-21.0, -15.0, -12.0],
      rHand: [-20.0, -8.0, -15.0],
      rightAnkle: [7.0, 61.0, 21.0],
      rightBall: [20.5, 58.0, 28.5],
      supportYaw: 218,
      supportRoll: -4
    },
    {
      frame: 16,
      name: "impact_hold",
      pelvis: [-0.8, 94.6, 1.4, 1.0, 286.0, 4.0],
      chest: [2.0, 190.0, -4.0],
      neck: [-1.0, -34.0, 1.0],
      head: [-2.0, -42.0, 1.0],
      lUpper: [18.0, -7.0, 30.0],
      lLower: [13.0, -11.0, 21.0],
      lHand: [7.0, -4.0, 17.0],
      rUpper: [-22.0, -11.0, -4.0],
      rLower: [-22.0, -16.0, -14.0],
      rHand: [-21.0, -9.0, -17.0],
      rightAnkle: [13.0, 62.5, 25.0],
      rightBall: [27.5, 59.0, 30.5],
      supportYaw: 292,
      supportRoll: -4
    },
    {
      frame: 19,
      name: "recoil_chamber",
      pelvis: [-1.7, 94.0, 0.8, -2.0, 338.0, 7.0],
      chest: [3.0, 262.0, -4.0],
      neck: [-1.0, -22.0, 1.0],
      head: [-2.0, -28.0, 1.0],
      lUpper: [17.0, -9.0, 24.0],
      lLower: [10.0, -13.0, 16.0],
      lHand: [4.0, -6.0, 13.0],
      rUpper: [-18.0, -10.0, 5.0],
      rLower: [-17.0, -14.0, -6.0],
      rHand: [-15.0, -8.0, -9.0],
      rightAnkle: [-2.5, 42.0, 9.0],
      rightBall: [1.0, 35.0, 21.0],
      supportYaw: 344,
      supportRoll: -2
    },
    {
      frame: 24,
      name: "step_down",
      pelvis: [0.3, 92.4, -0.4, -4.0, 364.0, 2.0],
      chest: [4.0, 336.0, -1.0],
      neck: [-1.0, -10.0, 0.0],
      head: [-2.0, -14.0, 0.0],
      lUpper: [14.0, -11.0, 20.0],
      lLower: [8.0, -16.0, 12.0],
      lHand: [3.0, -7.0, 9.0],
      rUpper: [-14.0, -12.0, 15.0],
      rLower: [-8.0, -16.0, 8.0],
      rHand: [-4.0, -8.0, 6.0],
      rightAnkle: [-8.8, 9.0, 0.0],
      rightBall: [-7.8, 1.3, 14.0],
      supportYaw: 366,
      supportRoll: 0
    },
    {
      frame: 27,
      name: "guard_recover",
      pelvis: [0.0, 93.0, 0.0, -2.0, 350.0, 1.0],
      chest: [3.0, 348.0, -1.0],
      neck: [-1.0, -3.0, 0.0],
      head: [-2.0, -5.0, 0.0],
      lUpper: [14.0, -12.0, 20.0],
      lLower: [8.0, -17.0, 12.0],
      lHand: [3.0, -8.0, 9.0],
      rUpper: [-13.0, -12.0, 18.0],
      rLower: [-7.0, -16.0, 11.0],
      rHand: [-3.0, -8.0, 8.0],
      rightAnkle: [-9.2, 8.2, -4.0],
      rightBall: [-8.5, 1.0, 10.5],
      supportYaw: 352,
      supportRoll: 0
    }
  ];

  const keyNameByFrame = new Map(poseKeys.map((pose) => [pose.frame, pose.name]));

  function makeLeftFoot(sample) {
    const yawRad = (sample.supportYaw * Math.PI) / 180;
    const ballX = 9.2 + Math.sin(yawRad) * 2.0;
    const ballZ = 9.5 + Math.cos(yawRad) * 5.5;
    return {
      ankle: vec(9.2, 8.05, -4.0),
      ball: vec(ballX, 1.0, ballZ),
      heel: vec(9.2 - Math.sin(yawRad) * 1.2, 2.0, -8.5),
      planted: true,
      locked: true,
      contactWeight: 1,
      pivot: "ball",
      yaw: round(sample.supportYaw),
      roll: round(sample.supportRoll)
    };
  }

  function makeRightFoot(frame, sample) {
    const airborne = frame > 5 && frame < 24;
    const contactWeight = airborne ? 0 : frame < 5 ? 0.86 : 0.92;
    return {
      ankle: sample.rightAnkle,
      ball: sample.rightBall,
      planted: !airborne,
      locked: !airborne,
      contactWeight: round(contactWeight),
      pivot: airborne ? "air" : "ball",
      strikeSurface: frame >= 14 && frame <= 16 ? "instep_shin" : undefined
    };
  }

  function makeFrame(frame) {
    const sample = sampleTrack(poseKeys, frame);
    const rightFoot = makeRightFoot(frame, sample);
    const leftFoot = makeLeftFoot(sample);
    const airborne = rightFoot.planted ? 0 : 1;

    return {
      frame,
      name: keyNameByFrame.get(frame) || "roundhouse_inbetween",
      pelvis: sample.pelvis,
      chest: sample.chest,
      neck: sample.neck,
      head: sample.head,
      lUpper: sample.lUpper,
      lLower: sample.lLower,
      lHand: sample.lHand,
      rUpper: sample.rUpper,
      rLower: sample.rLower,
      rHand: sample.rHand,
      leftFoot,
      rightFoot,
      leftKneeBias: vec(1.0, -0.25, 1.2),
      rightKneeBias: vec(-1.8 + airborne * 2.8, 0.7 + airborne * 1.1, 2.0 + airborne * 2.2),
      kneeBias: {
        left: vec(1.0, -0.25, 1.2),
        right: vec(-1.8 + airborne * 2.8, 0.7 + airborne * 1.1, 2.0 + airborne * 2.2)
      },
      contacts: {
        leftFoot: "ground_pivot",
        rightFoot: rightFoot.planted ? "ground" : frame >= 14 && frame <= 16 ? "impact_air_hold" : "air",
        supportFoot: "left",
        attackingFoot: "right"
      }
    };
  }

  const frames = Array.from({ length: frameCount }, (_, frame) => makeFrame(frame));

  const data = {
    id: "roundhouseKick",
    name: "Roundhouse Kick",
    koreanName: "돌려차기",
    frameCount,
    fps,
    durationSeconds: round(durationSeconds),
    loop: false,
    rotationUnits: "degrees",
    positionUnits: "Manny demo units",
    coordinateSystem: {
      up: "Y",
      forward: "Z",
      side: "X"
    },
    style: {
      intent: "A heavy realistic right-leg roundhouse kick with planted left-foot pivot, a full-turn body-axis rotation, hip-driven chamber, whipping shin path, impact hold, recoil, and guarded recovery.",
      notes: [
        "Left support foot remains grounded and pivots around the ball instead of translating.",
        "Pelvis and support-foot yaw advance through roughly 360 degrees using unwrapped yaw values so the body visibly spins instead of only swinging the leg.",
        "Chest, neck, and head yaw lag the pelvis through chamber and impact to preserve counter-rotation, spotting, and heavy balance.",
        "The kicking foot follows a chamber-to-arc path, holds briefly at impact height, then recoils before stepping down.",
        "Arm positions preserve a practical guard and counterbalance the rotating hips."
      ]
    },
    contactNotes: {
      interpolation: "Smoothstep interpolation is used to generate sampled frames from the authored key poses.",
      plantedFeet: "The left foot is locked for all frames with changing pivot yaw metadata; the right foot leaves the ground from chamber through recoil.",
      impact: "Frames 14-16 mark the heavy instep/shin impact hold."
    },
    keyposes: frames.filter((frame) => keyNameByFrame.has(frame.frame)),
    frames
  };

  data.keys = frames;

  global.mannyRoundhouseKickKeyposes = data;
})(typeof window !== "undefined" ? window : globalThis);
