(function (global) {
  "use strict";

  const frameCount = 240;
  const fps = 24;
  const durationSeconds = 10;
  const phraseLength = 24;
  const tau = Math.PI * 2;

  function clamp(value, min, max) {
    return Math.max(min, Math.min(max, value));
  }

  function lerp(a, b, t) {
    return a + (b - a) * t;
  }

  function smoothstep(t) {
    const x = clamp(t, 0, 1);
    return x * x * (3 - x * 2);
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

  function sampleKeys(keys, frame) {
    const wrappedFrame = frame >= frameCount ? 0 : frame;
    for (let i = 0; i < keys.length - 1; i += 1) {
      const a = keys[i];
      const b = keys[i + 1];
      if (wrappedFrame >= a.frame && wrappedFrame <= b.frame) {
        const t = smoothstep((wrappedFrame - a.frame) / (b.frame - a.frame || 1));
        const result = {};
        for (const key of Object.keys(a)) {
          if (key === "frame" || key === "name") {
            continue;
          }
          result[key] = typeof a[key] === "number" ? lerp(a[key], b[key], t) : a[key];
        }
        return result;
      }
    }
    return keys[keys.length - 1];
  }

  function sampleFootKeys(keys, frame) {
    for (let i = 0; i < keys.length - 1; i += 1) {
      const a = keys[i];
      const b = keys[i + 1];
      if (frame >= a.frame && frame <= b.frame) {
        const rawT = (frame - a.frame) / (b.frame - a.frame || 1);
        let t = smoothstep(rawT);
        let planted = a.planted && b.planted;

        if (a.planted && !b.planted) {
          if (rawT < 0.62) {
            t = 0;
            planted = true;
          } else {
            t = smoothstep((rawT - 0.62) / 0.38);
            planted = false;
          }
        } else if (!a.planted && b.planted) {
          if (rawT > 0.84) {
            t = 1;
            planted = true;
          } else {
            t = smoothstep(rawT / 0.84);
            planted = false;
          }
        } else if (a.planted && b.planted) {
          planted = true;
        }

        return {
          x: lerp(a.x, b.x, t),
          y: lerp(a.y, b.y, t),
          z: lerp(a.z, b.z, t),
          yaw: lerp(a.yaw, b.yaw, t),
          planted
        };
      }
    }
    return keys[keys.length - 1];
  }

  const phrases = [
    { frame: 0, name: "intro_groove", display: "Intro Groove", note: "Small confident bounce with both feet grounded." },
    { frame: 24, name: "left_side_step", display: "Left Side Step", note: "Left foot steps wide while the right foot pivots." },
    { frame: 48, name: "shoulder_pop", display: "Shoulder Pop", note: "Chest counters pelvis with two clear shoulder accents." },
    { frame: 72, name: "hip_sway", display: "Hip Sway", note: "Weight rolls through the hips without suggestive exaggeration." },
    { frame: 96, name: "arm_sweep", display: "Arm Sweep", note: "Both arms sweep across and open to the audience." },
    { frame: 120, name: "turn_accent", display: "Turn Accent", note: "Quarter-turn flavor through torso yaw and toe pivots." },
    { frame: 144, name: "low_bounce", display: "Low Bounce", note: "Knees compress into a low rhythmic bounce." },
    { frame: 168, name: "cross_step", display: "Cross Step", note: "Feet cross and uncross with soft sliding contact." },
    { frame: 192, name: "clap_reach", display: "Clap Reach", note: "Hands gather near center, then reach out." },
    { frame: 216, name: "recovery_loop", display: "Recovery Loop", note: "Body returns to the opening groove cleanly." }
  ];

  const bodyKeys = [
    { frame: 0, name: "intro_groove", x: 0, y: 94, z: 0, pelvisPitch: -1, pelvisYaw: 0, pelvisRoll: 0, chestPitch: 4, chestYaw: 0, chestRoll: 0, headPitch: -2, headYaw: 0, headRoll: 0 },
    { frame: 24, name: "left_side_step", x: 5, y: 93, z: 1, pelvisPitch: -2, pelvisYaw: -9, pelvisRoll: -7, chestPitch: 5, chestYaw: 11, chestRoll: 5, headPitch: -2, headYaw: -3, headRoll: -1 },
    { frame: 48, name: "shoulder_pop", x: -2, y: 95, z: 0, pelvisPitch: 0, pelvisYaw: 7, pelvisRoll: 4, chestPitch: 8, chestYaw: -16, chestRoll: -8, headPitch: -4, headYaw: 5, headRoll: 1 },
    { frame: 72, name: "hip_sway", x: -6, y: 93.5, z: -1, pelvisPitch: -3, pelvisYaw: 13, pelvisRoll: 10, chestPitch: 4, chestYaw: -8, chestRoll: -5, headPitch: -2, headYaw: 2, headRoll: 0 },
    { frame: 96, name: "arm_sweep", x: 2, y: 94.5, z: 0, pelvisPitch: -1, pelvisYaw: -5, pelvisRoll: -3, chestPitch: 7, chestYaw: 18, chestRoll: 6, headPitch: -3, headYaw: -5, headRoll: -1 },
    { frame: 120, name: "turn_accent", x: 0, y: 95, z: 1, pelvisPitch: 0, pelvisYaw: 26, pelvisRoll: 2, chestPitch: 5, chestYaw: 34, chestRoll: -2, headPitch: -1, headYaw: 20, headRoll: 0 },
    { frame: 144, name: "low_bounce", x: 3, y: 88.5, z: -1, pelvisPitch: -7, pelvisYaw: -10, pelvisRoll: -4, chestPitch: 12, chestYaw: 8, chestRoll: 3, headPitch: -6, headYaw: -1, headRoll: 1 },
    { frame: 168, name: "cross_step", x: -4, y: 92, z: 2, pelvisPitch: -2, pelvisYaw: 15, pelvisRoll: 8, chestPitch: 6, chestYaw: -17, chestRoll: -6, headPitch: -3, headYaw: 6, headRoll: -1 },
    { frame: 192, name: "clap_reach", x: 0, y: 96, z: 1, pelvisPitch: 1, pelvisYaw: 0, pelvisRoll: 0, chestPitch: 10, chestYaw: 0, chestRoll: 0, headPitch: -4, headYaw: 0, headRoll: 0 },
    { frame: 216, name: "recovery_loop", x: 1, y: 94, z: 0, pelvisPitch: -1, pelvisYaw: -3, pelvisRoll: 2, chestPitch: 5, chestYaw: 4, chestRoll: -2, headPitch: -2, headYaw: -1, headRoll: 0 },
    { frame: 239, name: "loop_ready", x: 0.1, y: 94, z: 0, pelvisPitch: -1, pelvisYaw: -0.5, pelvisRoll: 0.3, chestPitch: 4.2, chestYaw: 0.6, chestRoll: -0.2, headPitch: -2, headYaw: 0, headRoll: 0 }
  ];

  const leftFootKeys = [
    { frame: 0, name: "left_intro_planted", x: 9, y: 8, z: 0, yaw: 3, planted: true },
    { frame: 18, name: "left_step_lift", x: 9, y: 13, z: 0, yaw: 5, planted: false },
    { frame: 30, name: "left_side_land", x: 18, y: 8, z: 4, yaw: -8, planted: true },
    { frame: 60, name: "left_pop_anchor", x: 17, y: 8, z: 2, yaw: -4, planted: true },
    { frame: 84, name: "left_sway_anchor", x: 12, y: 8, z: -3, yaw: 10, planted: true },
    { frame: 108, name: "left_sweep_step", x: 15, y: 8, z: 5, yaw: -12, planted: true },
    { frame: 132, name: "left_turn_pivot", x: 10, y: 8, z: 2, yaw: 26, planted: true },
    { frame: 156, name: "left_low_anchor", x: 11, y: 7.2, z: -2, yaw: 4, planted: true },
    { frame: 174, name: "left_cross_lift", x: 6, y: 14, z: 8, yaw: -18, planted: false },
    { frame: 186, name: "left_cross_soft", x: -4, y: 8, z: 12, yaw: -25, planted: true },
    { frame: 204, name: "left_reach_anchor", x: 10, y: 8, z: 4, yaw: 6, planted: true },
    { frame: 230, name: "left_loop_reset", x: 9, y: 8, z: 0, yaw: 3, planted: true },
    { frame: 239, name: "left_loop_ready", x: 9, y: 8, z: 0, yaw: 3, planted: true }
  ];

  const rightFootKeys = [
    { frame: 0, name: "right_intro_planted", x: -9, y: 8, z: 0, yaw: -3, planted: true },
    { frame: 42, name: "right_side_support", x: -8, y: 8, z: -2, yaw: 4, planted: true },
    { frame: 54, name: "right_pop_lift", x: -8, y: 13, z: -2, yaw: 8, planted: false },
    { frame: 66, name: "right_pop_land", x: -15, y: 8, z: 2, yaw: 14, planted: true },
    { frame: 90, name: "right_sway_anchor", x: -13, y: 8, z: -4, yaw: -8, planted: true },
    { frame: 114, name: "right_sweep_anchor", x: -14, y: 8, z: 4, yaw: 10, planted: true },
    { frame: 126, name: "right_turn_pivot", x: -8, y: 8, z: 1, yaw: -28, planted: true },
    { frame: 150, name: "right_low_anchor", x: -11, y: 7.2, z: -3, yaw: -5, planted: true },
    { frame: 168, name: "right_cross_anchor", x: -12, y: 8, z: 2, yaw: 12, planted: true },
    { frame: 180, name: "right_uncross_lift", x: -8, y: 14, z: 9, yaw: 20, planted: false },
    { frame: 192, name: "right_uncross_land", x: -10, y: 8, z: 4, yaw: -4, planted: true },
    { frame: 216, name: "right_recovery_anchor", x: -10, y: 8, z: 1, yaw: -3, planted: true },
    { frame: 239, name: "right_loop_ready", x: -9, y: 8, z: 0, yaw: -3, planted: true }
  ];

  function footFromKeys(keys, frame, side) {
    const data = sampleFootKeys(keys, frame);
    const planted = Boolean(data.planted);
    const ankleY = planted ? Math.min(data.y, 8) : data.y;
    const ballY = planted ? 0.9 : Math.max(2.2, data.y - 6.2);
    const heelY = planted ? 1.8 : Math.max(2.5, data.y - 5.6);
    const toeForward = 15.2;
    const yawRad = (data.yaw * Math.PI) / 180;
    const toeX = Math.sin(yawRad) * 2.0;
    const heelX = -Math.sin(yawRad) * 1.4;
    const contactWeight = planted ? 0.92 : clamp(1 - (ankleY - 8) / 8, 0.15, 0.58);

    return {
      ankle: vec(data.x, ankleY, data.z),
      ball: vec(data.x + toeX, ballY, data.z + toeForward),
      heel: vec(data.x + heelX, heelY, data.z - 6.5),
      rot: rot(planted ? -2 : 9, data.yaw, side * (planted ? 1.5 : 4)),
      planted,
      contactWeight: round(contactWeight),
      floorLock: planted ? "strong" : "soft",
      pivot: planted ? "flat" : "ball"
    };
  }

  function armsAt(frame, phraseIndex, local, body) {
    const beat = Math.sin((local / phraseLength) * tau);
    const slow = Math.sin((frame / frameCount) * tau);
    const sweep = phraseIndex === 4 ? smoothstep(local / phraseLength) : 0;
    const clap = phraseIndex === 8 ? 1 - Math.abs(local - 10) / 10 : 0;
    const turn = phraseIndex === 5 ? 1 : 0;
    const low = phraseIndex === 6 ? 1 : 0;

    const lUpper = vec(13 + sweep * 12 - clap * 10, -9 - low * 5, 10 + beat * 12 + turn * 5);
    const lLower = vec(7 + sweep * 6 - clap * 8, -16 + clap * 4, 9 + sweep * 18);
    const lHand = vec(4 + sweep * 22 - clap * 11, -12 + sweep * 8 + clap * 14, 12 + sweep * 20);
    const rUpper = vec(-13 - sweep * 10 + clap * 10, -9 - low * 5, 10 - beat * 12 + turn * 5);
    const rLower = vec(-7 - sweep * 6 + clap * 8, -16 + clap * 4, 9 + sweep * 18);
    const rHand = vec(-4 - sweep * 22 + clap * 11, -12 + sweep * 8 + clap * 14, 12 + sweep * 20);

    return {
      lUpper,
      lLower,
      lHand,
      rUpper,
      rLower,
      rHand,
      leftArm: {
        shoulder: rot(8 + beat * 18 + sweep * 20, 8 + body.chestYaw * 0.2, 11 + slow * 8),
        elbow: { bend: round(72 + Math.abs(beat) * 18 + clap * 18) },
        wrist: rot(-6 + sweep * 8, 4, -5)
      },
      rightArm: {
        shoulder: rot(8 - beat * 18 + sweep * 18, -8 + body.chestYaw * 0.2, -11 - slow * 8),
        elbow: { bend: round(72 + Math.abs(beat) * 18 + clap * 18) },
        wrist: rot(-6 + sweep * 8, -4, 5)
      }
    };
  }

  function makeFrame(frame) {
    const phraseIndex = Math.floor(frame / phraseLength);
    const phrase = phrases[phraseIndex];
    const local = frame - phrase.frame;
    const phase = frame / frameCount;
    const localPhase = local / phraseLength;
    const beat = Math.sin(localPhase * tau * 2);
    const groove = Math.sin(localPhase * tau);
    const body = sampleKeys(bodyKeys, frame);
    const bounce = Math.abs(beat) * (phraseIndex === 6 ? 3.5 : 1.6);
    const pelvis = rot(body.pelvisPitch + beat * 1.3, body.pelvisYaw + groove * 2.5, body.pelvisRoll + beat * 1.8);
    const chest = rot(body.chestPitch + Math.max(0, beat) * 2.2, body.chestYaw - groove * 3.5, body.chestRoll - beat * 2.0);
    const neck = rot(-1.2, -body.chestYaw * 0.08, -body.chestRoll * 0.08);
    const head = rot(body.headPitch, body.headYaw - body.chestYaw * 0.18, body.headRoll);
    const arms = armsAt(frame, phraseIndex, local, body);
    const leftFoot = footFromKeys(leftFootKeys, frame, 1);
    const rightFoot = footFromKeys(rightFootKeys, frame, -1);
    const dominantFoot = leftFoot.contactWeight >= rightFoot.contactWeight ? "left" : "right";

    return {
      frame,
      name: `${phrase.name}_${String(local).padStart(2, "0")}`,
      phrase: phrase.name,
      phase: round(phase),
      pelvis: [round(body.x), round(body.y + bounce), round(body.z), ...pelvis],
      chest,
      neck,
      head,
      lUpper: arms.lUpper,
      lLower: arms.lLower,
      lHand: arms.lHand,
      rUpper: arms.rUpper,
      rLower: arms.rLower,
      rHand: arms.rHand,
      leftArm: arms.leftArm,
      rightArm: arms.rightArm,
      leftFoot,
      rightFoot,
      leftLeg: {
        hip: rot(8 + body.pelvisPitch, body.pelvisYaw * 0.35, body.pelvisRoll * 0.25),
        knee: { bend: round(20 + Math.max(0, 94 - body.y) * 3 + Math.abs(beat) * 10), bias: vec(1.8, 0, 1.2) },
        ankle: leftFoot.rot
      },
      rightLeg: {
        hip: rot(8 + body.pelvisPitch, body.pelvisYaw * 0.35, body.pelvisRoll * -0.25),
        knee: { bend: round(20 + Math.max(0, 94 - body.y) * 3 + Math.abs(beat) * 10), bias: vec(-1.8, 0, 1.2) },
        ankle: rightFoot.rot
      },
      kneeBias: {
        left: vec(1.8, 0, 1.2),
        right: vec(-1.8, 0, 1.2)
      },
      contacts: {
        leftFoot: leftFoot.floorLock,
        rightFoot: rightFoot.floorLock,
        dominantFoot,
        leftWeight: round(leftFoot.contactWeight / (leftFoot.contactWeight + rightFoot.contactWeight)),
        rightWeight: round(rightFoot.contactWeight / (leftFoot.contactWeight + rightFoot.contactWeight))
      }
    };
  }

  const keyposes = phrases.map((phrase) => ({
    frame: phrase.frame,
    name: phrase.name,
    display: phrase.display,
    note: phrase.note
  }));

  global.mannyLongDanceKeyposes = {
    id: "longDance",
    version: 1,
    name: "Long Dance",
    displayNames: {
      en: "Long Dance",
      ko: "긴 춤"
    },
    loop: true,
    frameCount,
    fps,
    durationSeconds,
    coordinateSystem: "X side, Y up, Z forward",
    units: {
      positions: "Manny demo centimeters",
      rotations: "degrees"
    },
    style: "Confident character dance loop with visible upper body, pelvis, arms, and foot rhythm; not sexually exaggerated.",
    builderHints: {
      interpolation: "Use smooth cubic interpolation for torso and arms, and contact-aware interpolation for feet.",
      footContact: "When planted is true, keep ankle/ball/heel stable until the next lift key. Soft-contact frames may slide lightly.",
      loop: "Frame 239 is shaped to return into frame 0 without a visible body or foot pop."
    },
    phrases,
    keyposes,
    sourceKeys: {
      body: bodyKeys,
      leftFoot: leftFootKeys,
      rightFoot: rightFootKeys
    },
    frames: Array.from({ length: frameCount }, (_, frame) => makeFrame(frame))
  };
})(typeof window !== "undefined" ? window : globalThis);
