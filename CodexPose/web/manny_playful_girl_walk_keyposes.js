(function (global) {
  "use strict";

  const frameCount = 48;
  const fps = 30;
  const durationSeconds = 1.6;
  const tau = Math.PI * 2;

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

  function footAt(phase, side) {
    const cycle = ((phase % 1) + 1) % 1;
    const stanceRatio = 0.52;
    const sideSway = Math.sin(phase * tau) * 0.9;
    const x = side * 9.8 + sideSway * 0.38;
    if (cycle < stanceRatio) {
      const t = smoothstep(cycle / stanceRatio);
      const z = lerp(9.2, -9.2, t);
      return {
        ankle: vec(x, 8.15, z),
        ball: vec(x + side * 0.9, 1.05, z + 14.8),
        planted: true,
        locked: true,
        contactWeight: round(1 - Math.sin(t * Math.PI) * 0.08)
      };
    }

    const t = smoothstep((cycle - stanceRatio) / (1 - stanceRatio));
    const lift = Math.sin(t * Math.PI);
    const flick = Math.sin(t * Math.PI * 2);
    const z = lerp(-9.2, 9.2, t);
    return {
      ankle: vec(x + side * lift * 1.35, 8.15 + lift * 12.4, z + flick * 1.0),
      ball: vec(x + side * 1.05, 1.05 + lift * 4.0, z + 14.8 + flick * 1.25),
      planted: false,
      locked: false,
      contactWeight: round(0.10 + Math.max(0, 1 - lift) * 0.36),
      toeBrush: lift < 0.32
    };
  }

  function armOffsets(side, drive, bounce, phaseOffset) {
    const forward = drive;
    const forwardLift = Math.max(0, forward);
    const backLift = Math.max(0, -forward);
    const wristFlick = Math.sin(phaseOffset * tau * 2 + side * 0.75) * 0.9;
    const elbowPulse = Math.max(0, Math.sin(phaseOffset * tau * 2 + Math.PI * 0.2));
    return {
      upper: vec(side * (12.2 + elbowPulse * 0.9), -16.2 + forwardLift * 1.8 - backLift * 0.8, forward * 10.6),
      lower: vec(side * (6.2 + wristFlick * 0.8), -21.0 + bounce * 0.65 + elbowPulse * 1.15, forward * 7.0 + wristFlick),
      hand: vec(side * (2.6 + wristFlick * 0.65), -13.2 + forwardLift * 2.4 - backLift * 1.1 + elbowPulse * 0.8, forward * 5.2 + wristFlick * 1.2)
    };
  }

  const keyNames = new Map([
    [0, "right_support_left_swing"],
    [6, "left_toe_brush"],
    [12, "left_support_bouncy_up"],
    [18, "right_swing_light_skip"],
    [24, "left_support_right_forward"],
    [30, "right_toe_brush"],
    [36, "right_support_bouncy_up"],
    [42, "left_swing_loop_prep"],
    [47, "loop_settle"]
  ]);

  function makeFrame(frame) {
    const phase = frame / frameCount;
    const step = Math.sin(phase * tau);
    const side = Math.sin(phase * tau + Math.PI * 0.18);
    const bounce = (1 - Math.cos(phase * tau * 2)) * 0.5;
    const hop = Math.max(0, Math.sin(phase * tau * 2 + Math.PI * 0.2));
    const contactSettle = Math.max(0, Math.cos(phase * tau * 2));
    const hipAccent = Math.sin(phase * tau * 2 - Math.PI * 0.12);
    const pelvisX = side * 1.95 + hipAccent * 0.22;
    const pelvisY = 93.9 + bounce * 2.25 + hop * 0.85 - contactSettle * 0.35;
    const pelvisZ = Math.sin(phase * tau + Math.PI * 0.15) * 0.85 + hipAccent * 0.12;
    const pelvisYaw = step * 8.2 + hipAccent * 1.1;
    const pelvisRoll = -side * 6.1 - hipAccent * 0.85;
    const pelvisPitch = -1.0 + bounce * 1.05 - contactSettle * 0.2;
    const chestYaw = -pelvisYaw * 0.58 + hipAccent * 0.2;
    const chestRoll = -pelvisRoll * 0.46;
    const chestPitch = 0.55 - bounce * 0.7 + hop * 0.18;
    const leftDrive = -step;
    const rightDrive = step;
    const leftArm = armOffsets(1, leftDrive, bounce, phase + 0.5);
    const rightArm = armOffsets(-1, rightDrive, bounce, phase);
    const leftFoot = footAt(phase + 0.5, 1);
    const rightFoot = footAt(phase, -1);
    const leftAir = leftFoot.planted ? 0 : 1;
    const rightAir = rightFoot.planted ? 0 : 1;

    return {
      frame,
      name: keyNames.get(frame) || "playful_walk_sample",
      pelvis: vec(pelvisX, pelvisY, pelvisZ).concat([round(pelvisPitch), round(pelvisYaw), round(pelvisRoll)]),
      chest: [round(chestPitch), round(chestYaw), round(chestRoll)],
      neck: [round(-0.35 + bounce * 0.28), round(-chestYaw * 0.16), round(-chestRoll * 0.14)],
      head: [round(-1.1 + bounce * 0.22), round(-chestYaw * 0.24), round(-chestRoll * 0.18)],
      lUpper: leftArm.upper,
      lLower: leftArm.lower,
      lHand: leftArm.hand,
      rUpper: rightArm.upper,
      rLower: rightArm.lower,
      rHand: rightArm.hand,
      leftFoot,
      rightFoot,
      leftKneeBias: vec(1.0 + leftAir * 1.05, -0.3 + leftAir * 0.8, 0.9 + leftAir * 1.85 + bounce * 0.35),
      rightKneeBias: vec(-1.0 - rightAir * 1.05, -0.3 + rightAir * 0.8, 0.9 + rightAir * 1.85 + bounce * 0.35),
      kneeBias: {
        left: vec(1.0 + leftAir * 1.05, -0.3 + leftAir * 0.8, 0.9 + leftAir * 1.85 + bounce * 0.35),
        right: vec(-1.0 - rightAir * 1.05, -0.3 + rightAir * 0.8, 0.9 + rightAir * 1.85 + bounce * 0.35)
      },
      contacts: {
        leftFoot: leftFoot.planted ? "ground" : (leftFoot.toeBrush ? "toe_brush" : "air"),
        rightFoot: rightFoot.planted ? "ground" : (rightFoot.toeBrush ? "toe_brush" : "air")
      }
    };
  }

  const frames = Array.from({ length: frameCount }, (_, frame) => makeFrame(frame));

  const data = {
    id: "playfulGirlWalk",
    name: "Playful Girl Walk",
    koreanName: "발랄한 소녀 걸음",
    frameCount,
    fps,
    durationSeconds,
    loop: true,
    rotationUnits: "degrees",
    positionUnits: "Manny demo units",
    coordinateSystem: {
      up: "Y",
      forward: "Z",
      side: "X"
    },
    style: {
      intent: "A bright, bouncy, youthful skip-like walk that stays readable as walking instead of running.",
      notes: [
        "Arms stay below shoulder height and get their liveliness from elbow pulse and wrist flick instead of locked straight swings.",
        "Pelvis timing has a clearer spring through the knees and ankles, with a small hop-like lift after each contact.",
        "Pelvis yaw and roll are deliberately stronger, with subtle secondary hip timing and soft chest/head counter-balance for a feminine silhouette.",
        "Footwork is compact and upbeat with toe-brush transitions, no run stride, and no accumulating root travel snap at the loop boundary."
      ]
    },
    contactNotes: {
      interpolation: "Cubic or smoothstep interpolation is recommended between adjacent frame samples.",
      plantedFeet: "The stance foot uses a compact treadmill-style path with locked planted samples; swing foot uses a light toe brush before lift and landing.",
      loop: "Frame 47 flows back into frame 0 without forward root accumulation."
    },
    keyposes: frames.filter((frame) => keyNames.has(frame.frame)),
    frames
  };

  data.keys = frames;

  global.mannyPlayfulGirlWalkKeyposes = data;
})(typeof window !== "undefined" ? window : globalThis);
