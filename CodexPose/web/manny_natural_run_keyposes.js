(function (global) {
  "use strict";

  const frameCount = 24;
  const fps = 30;
  const rootTravelPerFrame = 12.0;
  const footLeadZ = 42.0;
  const footBallForward = 15.8;
  const footSpacing = 9.8;
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

  function cycleFrame(frame, offset) {
    return ((frame - offset) % frameCount + frameCount) % frameCount;
  }

  function sampleLinear(keys, frame) {
    let localFrame = frame;
    while (localFrame < keys[0].frame) {
      localFrame += frameCount;
    }
    while (localFrame > keys[keys.length - 1].frame) {
      localFrame -= frameCount;
    }

    for (let index = 0; index < keys.length - 1; index += 1) {
      const a = keys[index];
      const b = keys[index + 1];
      if (localFrame >= a.frame && localFrame <= b.frame) {
        const t = a.frame === b.frame ? 0 : smoothstep((localFrame - a.frame) / (b.frame - a.frame));
        const result = {};
        for (const key of Object.keys(a)) {
          if (key === "frame" || key === "name" || key === "planted") {
            continue;
          }
          result[key] = lerp(a[key], b[key], t);
        }
        return result;
      }
    }

    return keys[keys.length - 1];
  }

  const bodyKeys = [
    {
      frame: 0,
      name: "right_contact_load",
      rootY: 93.2,
      rootZ: 0.0,
      pelvisPitch: 8.0,
      chestPitch: 13.0,
      headPitch: -5.0
    },
    {
      frame: 2,
      name: "right_compression",
      rootY: 92.45,
      rootZ: 1.0,
      pelvisPitch: 10.0,
      chestPitch: 15.0,
      headPitch: -6.0
    },
    {
      frame: 5,
      name: "right_push_off",
      rootY: 95.1,
      rootZ: 2.4,
      pelvisPitch: 8.6,
      chestPitch: 13.6,
      headPitch: -4.8
    },
    {
      frame: 8,
      name: "first_flight_apex",
      rootY: 97.35,
      rootZ: 3.0,
      pelvisPitch: 7.4,
      chestPitch: 12.2,
      headPitch: -4.2
    },
    {
      frame: 11,
      name: "left_pre_contact",
      rootY: 95.45,
      rootZ: 2.1,
      pelvisPitch: 8.8,
      chestPitch: 14.1,
      headPitch: -5.4
    },
    {
      frame: 12,
      name: "left_contact_load",
      rootY: 93.2,
      rootZ: 0.0,
      pelvisPitch: 8.0,
      chestPitch: 13.0,
      headPitch: -5.0
    },
    {
      frame: 14,
      name: "left_compression",
      rootY: 92.45,
      rootZ: 1.0,
      pelvisPitch: 10.0,
      chestPitch: 15.0,
      headPitch: -6.0
    },
    {
      frame: 17,
      name: "left_push_off",
      rootY: 95.1,
      rootZ: 2.4,
      pelvisPitch: 8.6,
      chestPitch: 13.6,
      headPitch: -4.8
    },
    {
      frame: 20,
      name: "second_flight_apex",
      rootY: 97.35,
      rootZ: 3.0,
      pelvisPitch: 7.4,
      chestPitch: 12.2,
      headPitch: -4.2
    },
    {
      frame: 23,
      name: "right_pre_contact",
      rootY: 95.45,
      rootZ: 2.1,
      pelvisPitch: 8.8,
      chestPitch: 14.1,
      headPitch: -5.4
    },
    {
      frame: 24,
      name: "right_contact_loop",
      rootY: 93.2,
      rootZ: 0.0,
      pelvisPitch: 8.0,
      chestPitch: 13.0,
      headPitch: -5.0
    }
  ];

  const footCycleKeys = [
    {
      frame: 0,
      name: "contact",
      z: footLeadZ,
      ankleY: 7.9,
      ballY: 0.95,
      hipPitch: 34,
      hipYaw: 2.5,
      hipRoll: -1.2,
      kneeBend: 30,
      anklePitch: -4,
      toeRoll: 0
    },
    {
      frame: 2,
      name: "load",
      z: footLeadZ - rootTravelPerFrame * 2,
      ankleY: 7.55,
      ballY: 0.85,
      hipPitch: 21,
      hipYaw: 1.2,
      hipRoll: -0.6,
      kneeBend: 46,
      anklePitch: -8,
      toeRoll: 0
    },
    {
      frame: 5,
      name: "toe_off",
      z: footLeadZ - rootTravelPerFrame * 5,
      ankleY: 10.3,
      ballY: 1.0,
      hipPitch: -19,
      hipYaw: -2.0,
      hipRoll: 1.0,
      kneeBend: 24,
      anklePitch: 17,
      toeRoll: 18
    },
    {
      frame: 6,
      name: "release",
      z: -27.5,
      ankleY: 15.4,
      ballY: 5.8,
      hipPitch: -26,
      hipYaw: -2.3,
      hipRoll: 1.0,
      kneeBend: 48,
      anklePitch: 19,
      toeRoll: 10
    },
    {
      frame: 9,
      name: "rear_tuck",
      z: -29.0,
      ankleY: 38.0,
      ballY: 20.2,
      hipPitch: -10,
      hipYaw: -1.0,
      hipRoll: 0.4,
      kneeBend: 104,
      anklePitch: 6,
      toeRoll: 0
    },
    {
      frame: 12,
      name: "under_body",
      z: -4.0,
      ankleY: 40.6,
      ballY: 24.0,
      hipPitch: 18,
      hipYaw: 0.4,
      hipRoll: -0.2,
      kneeBend: 92,
      anklePitch: -4,
      toeRoll: 0
    },
    {
      frame: 15,
      name: "front_recovery",
      z: 26.5,
      ankleY: 28.0,
      ballY: 16.0,
      hipPitch: 43,
      hipYaw: 1.4,
      hipRoll: -0.7,
      kneeBend: 58,
      anklePitch: -10,
      toeRoll: 0
    },
    {
      frame: 18,
      name: "reach",
      z: 45.5,
      ankleY: 15.2,
      ballY: 8.5,
      hipPitch: 38,
      hipYaw: 2.0,
      hipRoll: -0.8,
      kneeBend: 29,
      anklePitch: -13,
      toeRoll: 0
    },
    {
      frame: 21,
      name: "pre_contact",
      z: 47.0,
      ankleY: 9.7,
      ballY: 3.0,
      hipPitch: 33,
      hipYaw: 2.5,
      hipRoll: -1.0,
      kneeBend: 18,
      anklePitch: -8,
      toeRoll: 0
    },
    {
      frame: 24,
      name: "contact_loop",
      z: footLeadZ,
      ankleY: 7.9,
      ballY: 0.95,
      hipPitch: 34,
      hipYaw: 2.5,
      hipRoll: -1.2,
      kneeBend: 30,
      anklePitch: -4,
      toeRoll: 0
    }
  ];

  const namedKeyposes = [
    { frame: 0, name: "right_contact_load", rightFoot: "planted", leftFoot: "swing_reach" },
    { frame: 2, name: "right_compression", rightFoot: "planted", leftFoot: "pre_contact_recovery" },
    { frame: 5, name: "right_push_off", rightFoot: "toe_planted", leftFoot: "front_reach" },
    { frame: 8, name: "flight_tuck_to_left", rightFoot: "rear_tuck", leftFoot: "lowering" },
    { frame: 11, name: "left_pre_contact", rightFoot: "under_body", leftFoot: "pre_contact" },
    { frame: 12, name: "left_contact_load", rightFoot: "swing", leftFoot: "planted" },
    { frame: 14, name: "left_compression", rightFoot: "swing", leftFoot: "planted" },
    { frame: 17, name: "left_push_off", rightFoot: "front_reach", leftFoot: "toe_planted" },
    { frame: 20, name: "flight_tuck_to_right", rightFoot: "lowering", leftFoot: "rear_tuck" },
    { frame: 23, name: "right_pre_contact", rightFoot: "pre_contact", leftFoot: "under_body" }
  ];

  function plantedForCycle(localFrame) {
    return localFrame >= 0 && localFrame <= 5;
  }

  function buildFoot(frame, side, offset) {
    const local = cycleFrame(frame, offset);
    const data = sampleLinear(footCycleKeys, local);
    const planted = plantedForCycle(local);
    const sideX = side * footSpacing;
    const rootZ = frame * rootTravelPerFrame;
    const ankle = vec(sideX, data.ankleY, data.z);
    const ball = vec(sideX + side * 1.0, data.ballY, data.z + footBallForward);
    const contactFrame = frame - local;
    const stanceAnchorZ = contactFrame * rootTravelPerFrame + footLeadZ;
    const stanceBallAnchorZ = stanceAnchorZ + footBallForward;
    const worldAnkleZ = planted ? stanceAnchorZ : rootZ + data.z;
    const worldBallZ = planted ? stanceBallAnchorZ : rootZ + data.z + footBallForward;
    const contactWeight = planted ? 1 : clamp(1 - Math.abs(local - 21) / 5, 0, 0.55);

    return {
      foot: {
        planted,
        contactWeight: round(contactWeight),
        stanceAnchor: planted ? vec(sideX, 0, stanceAnchorZ) : null,
        ankle,
        ball,
        worldAnkle: vec(sideX, data.ankleY, worldAnkleZ),
        worldBall: vec(sideX + side * 1.0, data.ballY, worldBallZ),
        toeRoll: round(data.toeRoll),
        anklePitch: round(data.anklePitch)
      },
      leg: {
        hip: {
          pitch: round(data.hipPitch),
          yaw: round(data.hipYaw * side),
          roll: round(data.hipRoll * side)
        },
        knee: {
          bend: round(data.kneeBend),
          bias: vec(side * (1.4 + Math.max(0, data.kneeBend - 50) * 0.015), 0, 1.2)
        },
        ankle: {
          pitch: round(data.anklePitch),
          yaw: round(side * 1.2),
          roll: round(side * -0.6)
        }
      },
      localFrame: local
    };
  }

  function buildArm(side, drive, rootY, chestPitch) {
    const forward = drive;
    const elbowForward = forward * 0.55;
    const lift = Math.max(0, forward);
    const back = Math.max(0, -forward);
    const x = side * (24.0 - Math.abs(forward) * 1.8);
    const y = rootY - 23.0 + lift * 7.2 - back * 5.2;
    const z = 3.0 + forward * 30.0 + chestPitch * 0.22;
    const fistLift = lift * 7.0 - back * 4.8;
    const upperSegment = vec(side * 12.0, -11.4 + lift * 0.8, elbowForward * 30.0 + chestPitch * 0.18);
    const lowerSegment = vec(side * 7.4, -17.0 - Math.abs(forward) * 1.7, elbowForward * 18.0 + chestPitch * 0.08);
    const handSegment = vec(side * 3.1, -12.6 + fistLift * 1.12, forward * 28.0 - 1.0);

    return {
      drive: round(forward),
      shoulder: {
        pitch: round(-8 + elbowForward * 42),
        yaw: round(side * (6 - Math.abs(forward) * 2.5)),
        roll: round(side * (-8 - elbowForward * 3.5))
      },
      elbow: {
        bend: round(72 + Math.abs(elbowForward) * 12 + lift * 3),
        twist: round(side * (-5 + elbowForward * 2))
      },
      wrist: {
        pitch: round(-8 + forward * 9),
        yaw: round(side * 2.5),
        roll: round(side * -4)
      },
      fist: vec(x, y, z),
      upperSegment,
      lowerSegment,
      handSegment
    };
  }

  function buildFrame(frame) {
    const phase = frame / frameCount;
    const body = sampleLinear(bodyKeys, frame);
    const wave = Math.sin(phase * tau);
    const supportSide = -Math.cos(phase * tau);
    const twist = Math.sin(phase * tau + 0.2);
    const left = buildFoot(frame, 1, 12);
    const right = buildFoot(frame, -1, 0);
    const airborne = !left.foot.planted && !right.foot.planted;
    const rootX = supportSide * 1.55 + wave * 0.35;
    const rootY = body.rootY;
    const localRootZ = body.rootZ;
    const worldRootZ = frame * rootTravelPerFrame;
    const pelvisYaw = twist * 7.5;
    const pelvisRoll = supportSide * 4.4;
    const chestYaw = -twist * 9.0;
    const chestRoll = -pelvisRoll * 0.62;
    const headYaw = chestYaw * -0.28;
    const leftArmDrive = Math.cos(phase * tau);
    const rightArmDrive = -leftArmDrive;
    const leftArm = buildArm(1, leftArmDrive, rootY, body.chestPitch);
    const rightArm = buildArm(-1, rightArmDrive, rootY, body.chestPitch);
    const name = namedKeyposes.find((key) => key.frame === frame)?.name
      || (airborne ? "flight_sample" : "stance_sample");

    return {
      frame,
      name,
      phase: round(phase),
      contact: {
        left: left.foot.planted,
        right: right.foot.planted,
        airborne
      },
      root: {
        x: round(rootX),
        y: round(rootY),
        z: round(localRootZ),
        worldZ: round(worldRootZ)
      },
      pelvis: {
        x: round(rootX),
        y: round(rootY),
        z: round(localRootZ),
        yaw: round(pelvisYaw),
        pitch: round(body.pelvisPitch),
        roll: round(pelvisRoll)
      },
      spine: {
        lower: {
          yaw: round(pelvisYaw * 0.42),
          pitch: round(body.pelvisPitch + 1.2),
          roll: round(pelvisRoll * 0.52)
        },
        mid: {
          yaw: round(chestYaw * 0.48),
          pitch: round(body.chestPitch - 0.6),
          roll: round(chestRoll * 0.45)
        },
        upper: {
          yaw: round(chestYaw),
          pitch: round(body.chestPitch),
          roll: round(chestRoll)
        }
      },
      chest: {
        yaw: round(chestYaw),
        pitch: round(body.chestPitch),
        roll: round(chestRoll)
      },
      head: {
        yaw: round(headYaw),
        pitch: round(body.headPitch),
        roll: round(-chestRoll * 0.25)
      },
      leftArm,
      rightArm,
      lUpper: leftArm.upperSegment,
      lLower: leftArm.lowerSegment,
      lHand: leftArm.handSegment,
      rUpper: rightArm.upperSegment,
      rLower: rightArm.lowerSegment,
      rHand: rightArm.handSegment,
      leftLeg: left.leg,
      rightLeg: right.leg,
      leftFoot: left.foot,
      rightFoot: right.foot,
      notes: airborne
        ? "Both feet are off the ground; root height rises only a few centimeters to avoid floaty bounce."
        : "One foot is planted. Use worldAnkle/worldBall plus root.worldZ if fixed stance contact is needed."
    };
  }

  const payload = {
    id: "naturalRunKeypose",
    version: 1,
    frameCount,
    fps,
    description: "Natural Manny-proportion running cycle, key-pose sampled at 24 frames with clear but controlled flight phases.",
    coordinateSystem: {
      x: "side, positive left",
      y: "up",
      z: "forward"
    },
    units: "centimeters",
    angles: "degrees",
    spaces: {
      root: "local display root plus worldZ forward travel",
      pelvis: "local display pelvis/root position and rotation",
      footAnkleBall: "root-local display coordinates",
      footWorldAnkleBall: "root-motion coordinates; planted contacts remain fixed during stance"
    },
    rootMotion: {
      forwardPerFrame: rootTravelPerFrame,
      forwardPerCycle: round(rootTravelPerFrame * frameCount),
      hasFlight: true
    },
    keyposes: namedKeyposes,
    sourceKeys: {
      body: bodyKeys,
      footCycle: footCycleKeys
    },
    frames: Array.from({ length: frameCount }, (_, frame) => buildFrame(frame))
  };

  global.mannyNaturalRunKeyposes = payload;
})(typeof window !== "undefined" ? window : globalThis);
