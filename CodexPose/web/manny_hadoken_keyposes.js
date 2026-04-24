(function (global) {
  "use strict";

  const frameCount = 40;
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

  function lerpArray(a, b, t) {
    return a.map((value, index) => round(lerp(value, b[index], t)));
  }

  function lerpFoot(a, b, t) {
    return {
      ankle: lerpArray(a.ankle, b.ankle, t),
      ball: lerpArray(a.ball, b.ball, t),
      planted: true,
      locked: true,
      contactWeight: round(lerp(a.contactWeight, b.contactWeight, t)),
      pivot: t > 0.5 ? b.pivot : a.pivot
    };
  }

  function foot(side, x, z, yaw, weight) {
    const yawRad = (yaw * Math.PI) / 180;
    const toeX = Math.sin(yawRad) * 2.3;
    return {
      ankle: vec(side * x, 8.1, z),
      ball: vec(side * x + toeX, 1.05, z + 15.0),
      planted: true,
      locked: true,
      contactWeight: round(weight),
      pivot: "flat"
    };
  }

  const keyFrames = [
    {
      frame: 0,
      name: "grounded_guard",
      pelvis: vec(0.0, 91.8, 0.0).concat([round(-2.0), round(-8.0), round(0.8)]),
      chest: [round(2.5), round(10.0), round(-1.0)],
      neck: [round(-1.0), round(-3.0), round(0.2)],
      head: [round(-2.0), round(-5.0), round(0.3)],
      lUpper: vec(12.4, -13.8, 8.0),
      lLower: vec(8.2, -18.2, 13.0),
      lHand: vec(7.0, -11.0, 17.0),
      rUpper: vec(-12.2, -13.6, 7.0),
      rLower: vec(-8.4, -18.0, 12.0),
      rHand: vec(-7.2, -11.2, 16.0),
      leftFoot: foot(1, 11.8, 7.4, -5, 0.52),
      rightFoot: foot(-1, 12.0, -8.0, 12, 0.48)
    },
    {
      frame: 6,
      name: "crouched_energy_gather",
      pelvis: vec(0.8, 87.2, -0.8).concat([round(-8.0), round(-16.0), round(-1.2)]),
      chest: [round(7.5), round(20.0), round(1.2)],
      neck: [round(-2.4), round(-6.0), round(-0.3)],
      head: [round(-4.0), round(-8.0), round(-0.5)],
      lUpper: vec(8.5, -17.6, 9.5),
      lLower: vec(5.0, -20.0, 13.5),
      lHand: vec(2.8, -17.0, 15.5),
      rUpper: vec(-8.2, -17.4, 9.0),
      rLower: vec(-4.8, -19.8, 13.2),
      rHand: vec(-2.8, -17.2, 15.3),
      leftFoot: foot(1, 12.0, 7.5, -7, 0.56),
      rightFoot: foot(-1, 12.1, -8.1, 13, 0.44)
    },
    {
      frame: 10,
      name: "torso_coil_charge",
      pelvis: vec(1.4, 85.9, -1.4).concat([round(-10.0), round(-24.0), round(-2.4)]),
      chest: [round(10.0), round(31.0), round(2.3)],
      neck: [round(-3.0), round(-8.5), round(-0.7)],
      head: [round(-5.0), round(-10.0), round(-0.8)],
      lUpper: vec(7.4, -18.4, 11.0),
      lLower: vec(4.1, -20.4, 14.0),
      lHand: vec(1.4, -18.5, 17.0),
      rUpper: vec(-7.4, -18.2, 10.8),
      rLower: vec(-4.0, -20.2, 13.8),
      rHand: vec(-1.4, -18.4, 16.8),
      leftFoot: foot(1, 12.3, 7.6, -9, 0.6),
      rightFoot: foot(-1, 12.2, -8.2, 14, 0.4)
    },
    {
      frame: 15,
      name: "palms_thrust_forward",
      pelvis: vec(-0.4, 89.0, 2.2).concat([round(-4.0), round(5.0), round(0.8)]),
      chest: [round(1.0), round(-4.0), round(-0.8)],
      neck: [round(-1.4), round(1.4), round(0.0)],
      head: [round(-2.5), round(2.0), round(0.0)],
      lUpper: vec(10.0, -13.2, 20.5),
      lLower: vec(6.2, -13.2, 30.5),
      lHand: vec(3.8, -10.5, 41.0),
      rUpper: vec(-10.0, -13.2, 20.5),
      rLower: vec(-6.2, -13.2, 30.5),
      rHand: vec(-3.8, -10.5, 41.0),
      leftFoot: foot(1, 12.0, 7.7, -10, 0.47),
      rightFoot: foot(-1, 12.2, -8.4, 16, 0.53)
    },
    {
      frame: 18,
      name: "projectile_release",
      pelvis: vec(-0.8, 90.4, 3.0).concat([round(-1.5), round(10.0), round(1.4)]),
      chest: [round(-1.5), round(-9.0), round(-1.2)],
      neck: [round(-0.8), round(3.0), round(0.1)],
      head: [round(-1.5), round(4.0), round(0.1)],
      lUpper: vec(10.4, -12.2, 23.5),
      lLower: vec(6.5, -11.6, 35.0),
      lHand: vec(4.2, -9.2, 47.5),
      rUpper: vec(-10.4, -12.2, 23.5),
      rLower: vec(-6.5, -11.6, 35.0),
      rHand: vec(-4.2, -9.2, 47.5),
      leftFoot: foot(1, 12.0, 7.8, -10, 0.43),
      rightFoot: foot(-1, 12.2, -8.4, 16, 0.57)
    },
    {
      frame: 23,
      name: "impact_pulse_hold",
      pelvis: vec(-0.4, 89.8, 2.6).concat([round(-2.0), round(8.0), round(1.0)]),
      chest: [round(-0.8), round(-7.0), round(-0.9)],
      neck: [round(-0.6), round(2.4), round(0.0)],
      head: [round(-1.2), round(3.5), round(0.0)],
      lUpper: vec(10.2, -12.4, 22.2),
      lLower: vec(6.4, -12.0, 33.0),
      lHand: vec(4.0, -9.8, 44.5),
      rUpper: vec(-10.2, -12.4, 22.2),
      rLower: vec(-6.4, -12.0, 33.0),
      rHand: vec(-4.0, -9.8, 44.5),
      leftFoot: foot(1, 12.0, 7.7, -9, 0.46),
      rightFoot: foot(-1, 12.2, -8.3, 15, 0.54)
    },
    {
      frame: 29,
      name: "recoil_follow_through",
      pelvis: vec(0.2, 88.6, 0.8).concat([round(-5.0), round(-2.0), round(0.0)]),
      chest: [round(2.0), round(6.0), round(0.4)],
      neck: [round(-1.0), round(-2.0), round(0.0)],
      head: [round(-2.0), round(-3.0), round(0.0)],
      lUpper: vec(11.0, -14.5, 15.0),
      lLower: vec(7.2, -17.0, 22.0),
      lHand: vec(5.8, -12.8, 27.0),
      rUpper: vec(-11.0, -14.5, 15.0),
      rLower: vec(-7.2, -17.0, 22.0),
      rHand: vec(-5.8, -12.8, 27.0),
      leftFoot: foot(1, 11.9, 7.5, -7, 0.5),
      rightFoot: foot(-1, 12.1, -8.1, 13, 0.5)
    },
    {
      frame: 39,
      name: "return_to_guard",
      pelvis: vec(0.0, 91.6, 0.0).concat([round(-2.0), round(-8.0), round(0.8)]),
      chest: [round(2.5), round(10.0), round(-1.0)],
      neck: [round(-1.0), round(-3.0), round(0.2)],
      head: [round(-2.0), round(-5.0), round(0.3)],
      lUpper: vec(12.0, -13.8, 8.4),
      lLower: vec(8.0, -18.0, 13.4),
      lHand: vec(7.0, -11.2, 17.5),
      rUpper: vec(-12.0, -13.8, 7.5),
      rLower: vec(-8.2, -18.0, 12.4),
      rHand: vec(-7.2, -11.2, 16.5),
      leftFoot: foot(1, 11.8, 7.4, -5, 0.52),
      rightFoot: foot(-1, 12.0, -8.0, 12, 0.48)
    }
  ];

  const keyNames = new Map(keyFrames.map((pose) => [pose.frame, pose.name]));

  function effectAt(frame) {
    const charge = frame <= 12 ? smoothstep(frame / 12) : Math.max(0, 1 - (frame - 12) / 12);
    const releaseFlash = Math.max(0, 1 - Math.abs(frame - 18) / 3);
    const impactPulse = frame >= 20 && frame <= 24 ? 1 - Math.abs(frame - 22) / 3 : 0;
    return {
      charging: frame >= 4 && frame <= 17,
      released: frame >= 18,
      projectileFrame: frame >= 18 && frame <= 27 ? frame - 18 : null,
      energyIntensity: round(clamp(charge * 0.82 + releaseFlash + impactPulse * 0.65, 0, 1.35)),
      palmGlow: round(clamp(charge + releaseFlash * 0.75, 0, 1.2)),
      shockwave: round(clamp(impactPulse, 0, 1))
    };
  }

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

    const t = a.frame === b.frame ? 0 : smoothstep((frame - a.frame) / (b.frame - a.frame));
    const leftKnee = vec(1.4 + (91.8 - lerp(a.pelvis[1], b.pelvis[1], t)) * 0.06, -0.2, 3.0 + t * 0.2);
    const rightKnee = vec(-1.4 - (91.8 - lerp(a.pelvis[1], b.pelvis[1], t)) * 0.06, -0.25, 2.8 + t * 0.25);

    return {
      frame,
      name: keyNames.get(frame) || "hadoken_sample",
      key: keyNames.has(frame),
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
      leftFoot: lerpFoot(a.leftFoot, b.leftFoot, t),
      rightFoot: lerpFoot(a.rightFoot, b.rightFoot, t),
      leftKneeBias: leftKnee,
      rightKneeBias: rightKnee,
      kneeBias: {
        left: leftKnee,
        right: rightKnee
      },
      contacts: {
        leftFoot: "ground",
        rightFoot: "ground",
        weightShift: round(lerp(a.leftFoot.contactWeight, b.leftFoot.contactWeight, t) - 0.5)
      },
      effects: effectAt(frame)
    };
  }

  const frames = Array.from({ length: frameCount }, (_, frame) => makeFrame(frame));

  const data = {
    id: "hadoken",
    name: "Hadoken",
    koreanName: "파동권",
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
    effectTiming: {
      chargeStartFrame: 4,
      chargePeakFrame: 12,
      releaseFrame: 18,
      projectileFrame: 18,
      projectileTravelFrames: [18, 27],
      impactHoldFrames: [20, 24],
      recoilStartFrame: 25,
      returnToGuardFrame: 39,
      energyIntensity: "Per-frame effects.energyIntensity peaks on release and impact pulse."
    },
    style: {
      intent: "A grounded fighting-game energy projectile: crouched charge, torso coil, two-hand thrust, visible release, pulse hold, recoil, and guard recovery.",
      notes: [
        "Both feet stay planted with changing contact weights, knee bend, and foot yaw metadata to avoid sliding.",
        "Hands converge near center during charge, then push forward together without extending past plausible Manny arm reach.",
        "Pelvis and chest counter-rotate during charge and unwind into the release so the projectile feels body-driven."
      ]
    },
    keyposes: frames.filter((frame) => frame.key),
    frames
  };

  data.keys = frames;

  global.mannyHadokenKeyposes = data;
})(typeof window !== "undefined" ? window : globalThis);
