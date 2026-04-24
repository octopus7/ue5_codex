(function (global) {
  "use strict";

  const frameCount = 24;
  const fps = 24;
  const tau = Math.PI * 2;

  function round(value) {
    return Number(value.toFixed(3));
  }

  function vec(x, y, z) {
    return [round(x), round(y), round(z)];
  }

  function rot(pitch, yaw, roll) {
    return { pitch: round(pitch), yaw: round(yaw), roll: round(roll) };
  }

  function scalarPulse(local, center, width) {
    const distance = Math.abs(local - center);
    return Math.max(0, 1 - distance / width);
  }

  const activeFootKeys = [
    { name: "heel_prep", x: 10.2, y: 0.2, z: 0.0, pitch: -18, yaw: 15, roll: 5, contact: 0.56, slide: 0.42, planted: false, pivot: "heel" },
    { name: "toe_flick", x: 11.4, y: 0.8, z: 5.2, pitch: 15, yaw: -19, roll: -4, contact: 0.46, slide: 0.62, planted: false, pivot: "toe" },
    { name: "cross_start", x: 7.2, y: 1.4, z: 12.4, pitch: 9, yaw: -26, roll: -6, contact: 0.34, slide: 0.86, planted: false, pivot: "ball" },
    { name: "cross_slide", x: 1.0, y: 1.2, z: 18.0, pitch: 5, yaw: -35, roll: -8, contact: 0.42, slide: 0.96, planted: false, pivot: "ball" },
    { name: "cross_front", x: -5.6, y: 0.7, z: 20.8, pitch: -3, yaw: -41, roll: -9, contact: 0.52, slide: 0.94, planted: false, pivot: "flat" },
    { name: "cross_check", x: -3.4, y: 0.5, z: 15.5, pitch: -10, yaw: -20, roll: -5, contact: 0.62, slide: 0.74, planted: false, pivot: "heel" },
    { name: "uncross_slide", x: 4.8, y: 0.9, z: 8.8, pitch: 12, yaw: 16, roll: 4, contact: 0.54, slide: 0.72, planted: false, pivot: "toe" },
    { name: "outside_land", x: 12.3, y: 0.2, z: 3.4, pitch: -8, yaw: 20, roll: 5, contact: 0.86, slide: 0.30, planted: true, pivot: "flat" },
    { name: "load_flat", x: 11.5, y: 0.0, z: -1.4, pitch: -2, yaw: 8, roll: 2, contact: 1.00, slide: 0.08, planted: true, pivot: "flat" },
    { name: "heel_toe_reset", x: 10.4, y: 0.2, z: -2.4, pitch: -13, yaw: -11, roll: -2, contact: 0.92, slide: 0.18, planted: true, pivot: "heel" },
    { name: "toe_reset", x: 9.6, y: 0.5, z: -1.0, pitch: 10, yaw: 13, roll: 3, contact: 0.72, slide: 0.34, planted: false, pivot: "toe" },
    { name: "ready_switch", x: 10.0, y: 0.1, z: 0.0, pitch: -4, yaw: 4, roll: 1, contact: 0.84, slide: 0.20, planted: true, pivot: "flat" }
  ];

  function mirrorFootKey(key) {
    return {
      name: key.name,
      x: -key.x,
      y: key.y,
      z: key.z,
      pitch: key.pitch,
      yaw: -key.yaw,
      roll: -key.roll,
      contact: key.contact,
      slide: key.slide,
      planted: key.planted,
      pivot: key.pivot
    };
  }

  function makeFoot(key, sideSign, local, active) {
    const yawRad = (key.yaw * Math.PI) / 180;
    const forwardX = Math.sin(yawRad) * 2.1;
    const heelX = -Math.sin(yawRad) * 1.4;
    const ankleY = 8.3 + key.y;
    const ballY = 1.2 + key.y * 0.35;
    const heelY = 2.6 + key.y * 0.25;

    return {
      planted: key.planted,
      ankle: vec(key.x, ankleY, key.z),
      ball: vec(key.x + forwardX, ballY, key.z + 15.2),
      heel: vec(key.x + heelX, heelY, key.z - 6.6),
      rot: rot(key.pitch, key.yaw, key.roll),
      slideWeight: round(key.slide),
      contactWeight: round(key.contact),
      lateralWeight: round(sideSign * (active ? 0.6 + scalarPulse(local, 8, 3) * 0.4 : 0.92)),
      pivot: key.pivot,
      floorLock: key.contact > 0.82 ? "strong" : key.contact > 0.5 ? "soft" : "glide"
    };
  }

  function supportFoot(frame, sideSign, local) {
    const pulse = Math.sin((local / 12) * tau);
    const heelToe = Math.sin(((frame + (sideSign > 0 ? 2 : 8)) / 6) * tau);
    const yaw = sideSign * (4 + heelToe * 9);
    const key = {
      name: "support_shuffle_pivot",
      x: sideSign * (10.4 + pulse * 0.8),
      y: Math.max(0, -heelToe) * 0.35,
      z: -2.2 + Math.cos((local / 12) * tau) * 1.8,
      pitch: heelToe > 0 ? 8 : -10,
      yaw,
      roll: sideSign * heelToe * 3,
      contact: 0.96,
      slide: 0.08 + Math.max(0, heelToe) * 0.14,
      planted: true,
      pivot: heelToe > 0 ? "toe" : "heel"
    };
    return makeFoot(key, sideSign, local, false);
  }

  function bodyAt(frame, activeSide, local) {
    const phase = (frame / frameCount) * tau;
    const fastPhase = (local / 12) * tau;
    const crossPulse = scalarPulse(local, 4, 3);
    const landPulse = scalarPulse(local, 8, 2.6);
    const sideShift = Math.sin(phase) * 3.2 + activeSide * crossPulse * 1.0 - activeSide * landPulse * 0.8;
    const bounce = 93.0 + Math.abs(Math.sin(fastPhase)) * 1.35 - landPulse * 0.55;
    const pelvisYaw = activeSide * (Math.sin(fastPhase) * 10.0 + crossPulse * 6.5 - landPulse * 4.0);
    const pelvisRoll = -sideShift * 1.65 + activeSide * crossPulse * 3.0;
    const pelvisPitch = -2.0 - landPulse * 1.6 + crossPulse * 1.2;

    return {
      root: {
        pos: { x: round(sideShift * 0.24), y: 0.0, z: 0.0 },
        rot: rot(0.0, activeSide * Math.sin(fastPhase) * 1.5, 0.0)
      },
      pelvis: {
        pos: { x: round(sideShift), y: round(bounce), z: round(Math.cos(phase) * 1.2) },
        rot: rot(pelvisPitch, pelvisYaw, pelvisRoll),
        weightShift: round(sideShift / 4.4),
        bounce: round(bounce - 93.0)
      },
      spine: { rot: rot(2.0 + landPulse * 1.4, -pelvisYaw * 0.38, -pelvisRoll * 0.32) },
      chest: { rot: rot(4.0 + crossPulse * 2.8, -pelvisYaw * 0.72, -pelvisRoll * 0.48) },
      head: {
        rot: rot(-2.0 - crossPulse * 1.2, -pelvisYaw * 0.24, -pelvisRoll * 0.10),
        lookAt: { x: round(sideShift * 0.28), y: 122.0, z: 42.0 }
      }
    };
  }

  function armsAt(frame, activeSide, local) {
    const phase = (frame / frameCount) * tau;
    const beat = Math.sin((local / 12) * tau);
    const crossPulse = scalarPulse(local, 4, 3);
    const leftDrive = activeSide > 0 ? 1 : -1;
    const rightDrive = -leftDrive;
    const leftSwing = Math.sin(phase + Math.PI * 0.65);
    const rightSwing = Math.sin(phase + Math.PI * 1.65);

    return {
      leftArm: {
        shoulder: rot(10 + leftSwing * 26 - leftDrive * crossPulse * 10, 8 + leftDrive * beat * 10, 10 + leftDrive * crossPulse * 8),
        elbow: { bend: round(72 + Math.abs(leftSwing) * 18 + crossPulse * 8) },
        wrist: rot(-8 + beat * 4, leftDrive * 9, leftDrive * -6),
        hand: vec(18 + leftSwing * 6, 72 + Math.abs(beat) * 3, 13 + leftSwing * 10),
        rhythmWeight: round(0.8 + Math.abs(beat) * 0.2)
      },
      rightArm: {
        shoulder: rot(10 + rightSwing * 26 - rightDrive * crossPulse * 10, -8 + rightDrive * beat * 10, -10 + rightDrive * crossPulse * 8),
        elbow: { bend: round(72 + Math.abs(rightSwing) * 18 + crossPulse * 8) },
        wrist: rot(-8 - beat * 4, rightDrive * 9, rightDrive * -6),
        hand: vec(-18 + rightSwing * 6, 72 + Math.abs(beat) * 3, 13 + rightSwing * 10),
        rhythmWeight: round(0.8 + Math.abs(beat) * 0.2)
      }
    };
  }

  function legsAt(leftFoot, rightFoot, activeSide, local) {
    const crossPulse = scalarPulse(local, 4, 3);
    const landPulse = scalarPulse(local, 8, 2.6);
    const leftActive = activeSide > 0;

    return {
      leftLeg: {
        hip: rot(leftActive ? 12 + crossPulse * 14 : -4 - landPulse * 5, leftActive ? -18 * crossPulse : 4, leftActive ? 8 * crossPulse : -3),
        knee: { bend: round(leftActive ? 30 + crossPulse * 28 : 18 + landPulse * 10) },
        ankle: leftFoot.rot,
        contactWeight: leftFoot.contactWeight,
        driveRole: leftActive ? "crossing_slide" : "support_pivot"
      },
      rightLeg: {
        hip: rot(!leftActive ? 12 + crossPulse * 14 : -4 - landPulse * 5, !leftActive ? 18 * crossPulse : -4, !leftActive ? -8 * crossPulse : 3),
        knee: { bend: round(!leftActive ? 30 + crossPulse * 28 : 18 + landPulse * 10) },
        ankle: rightFoot.rot,
        contactWeight: rightFoot.contactWeight,
        driveRole: !leftActive ? "crossing_slide" : "support_pivot"
      }
    };
  }

  function makeFrame(frame) {
    const leftActive = frame < 12;
    const activeSide = leftActive ? 1 : -1;
    const local = leftActive ? frame : frame - 12;
    const rawActiveKey = activeFootKeys[local];
    const activeKey = leftActive ? rawActiveKey : mirrorFootKey(rawActiveKey);
    const leftFoot = leftActive ? makeFoot(activeKey, 1, local, true) : supportFoot(frame, 1, local);
    const rightFoot = leftActive ? supportFoot(frame, -1, local) : makeFoot(activeKey, -1, local, true);
    const body = bodyAt(frame, activeSide, local);
    const arms = armsAt(frame, activeSide, local);
    const legs = legsAt(leftFoot, rightFoot, activeSide, local);
    const key = frame % 3 === 0 || frame === 4 || frame === 16;

    return {
      frame,
      name: `${leftActive ? "left" : "right"}_${rawActiveKey.name}`,
      key,
      phase: round(frame / frameCount),
      root: body.root,
      pelvis: body.pelvis,
      spine: body.spine,
      chest: body.chest,
      head: body.head,
      leftArm: arms.leftArm,
      rightArm: arms.rightArm,
      leftLeg: legs.leftLeg,
      rightLeg: legs.rightLeg,
      leftFoot,
      rightFoot,
      contacts: {
        leftFoot: leftFoot.floorLock,
        rightFoot: rightFoot.floorLock,
        dominantFoot: leftFoot.contactWeight >= rightFoot.contactWeight ? "left" : "right",
        slideFoot: leftActive ? "left" : "right",
        leftWeight: round(leftFoot.contactWeight / (leftFoot.contactWeight + rightFoot.contactWeight)),
        rightWeight: round(rightFoot.contactWeight / (leftFoot.contactWeight + rightFoot.contactWeight))
      }
    };
  }

  const frames = Array.from({ length: frameCount }, (_, frame) => makeFrame(frame));

  global.mannyShuffleDanceKeyposes = {
    id: "shuffleDance",
    name: "Shuffle Dance",
    loop: true,
    frameCount,
    fps,
    coordinateSystem: "X side, Y up, Z forward",
    units: {
      positions: "Manny demo units",
      rotations: "degrees"
    },
    builderHints: {
      interpolation: "Use cubic ease for body/chest/head and contact-aware linear slides for feet.",
      footwork: "Keep the high contact foot floor-locked; allow the slideFoot to glide using slideWeight while preserving ball or heel pivot metadata.",
      style: "Energetic heel-toe shuffle with lateral weight shifts, quick crossing steps, and counter-rotating chest/arms.",
      loop: "Frame 23 returns both feet near neutral support so frame 0 can restart the opposite heel-toe preparation."
    },
    keyposes: [
      { frame: 0, name: "left_heel_prep", note: "Left heel opens while right foot supports." },
      { frame: 3, name: "left_cross_slide", note: "Left foot slides across the body with light ball contact." },
      { frame: 6, name: "left_uncross_slide", note: "Left foot pulls back out; pelvis reverses." },
      { frame: 8, name: "left_load_flat", note: "Left foot lands wide and absorbs the beat." },
      { frame: 12, name: "right_heel_prep", note: "Mirror phrase begins on the right foot." },
      { frame: 15, name: "right_cross_slide", note: "Right foot crosses with toe-out shuffle angle." },
      { frame: 18, name: "right_uncross_slide", note: "Right foot uncrosses while chest counters." },
      { frame: 20, name: "right_load_flat", note: "Right foot lands wide before resetting the loop." }
    ],
    sourceKeys: {
      activeFoot: activeFootKeys.map((key, frame) => ({ frame, ...key })),
      mirroredPhraseStartsAt: 12
    },
    frames
  };
})(window);
