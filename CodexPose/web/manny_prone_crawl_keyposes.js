(function () {
  "use strict";

  const target = typeof window !== "undefined" ? window : globalThis;
  const frameCount = 24;
  const fps = 24;
  const cycleTravelZ = 72;
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

  function point(x, y, z) {
    return [round(x), round(y), round(z)];
  }

  function vec(x, y, z) {
    return { x: round(x), y: round(y), z: round(z) };
  }

  function rot(pitch, yaw, roll) {
    return { pitch: round(pitch), yaw: round(yaw), roll: round(roll) };
  }

  function phase(frame, offset) {
    return ((frame + offset) % frameCount) / frameCount;
  }

  function contactWeightFromPhase(p) {
    if (p < 0.46) {
      return 1;
    }
    if (p < 0.56) {
      return round(1 - smoothstep((p - 0.46) / 0.1));
    }
    if (p < 0.84) {
      return 0;
    }
    return round(smoothstep((p - 0.84) / 0.16));
  }

  function armState(frame, side, offset, bodyZ) {
    const p = phase(frame, offset);
    const isLeft = side === "left";
    const xBase = isLeft ? 14 : -14;
    const outward = isLeft ? 1 : -1;
    const contactWeight = contactWeightFromPhase(p);
    const pullT = smoothstep(clamp(p / 0.46, 0, 1));
    const recoverT = smoothstep(clamp((p - 0.56) / 0.28, 0, 1));
    const latePlantT = smoothstep(clamp((p - 0.84) / 0.16, 0, 1));
    const inAir = contactWeight < 0.2;

    let relZ;
    let liftY;
    if (p < 0.56) {
      relZ = lerp(30, -11, pullT);
      liftY = 1.0;
    } else {
      relZ = lerp(-10, 32, recoverT);
      liftY = 2.2 + Math.sin(clamp((p - 0.56) / 0.28, 0, 1) * Math.PI) * 7.0;
    }
    if (p >= 0.84) {
      relZ = lerp(30, 32, latePlantT);
      liftY = lerp(3.4, 1.0, latePlantT);
    }

    const hand = point(xBase + outward * 1.8 * Math.sin(p * tau), liftY, bodyZ + relZ);
    const elbow = point(xBase - outward * 3.0, 1.1 + (inAir ? 2.0 : 0.0), bodyZ + relZ - 10.5);
    const shoulderPitch = lerp(58, -18, pullT) + (inAir ? lerp(8, 46, recoverT) : 0);
    const shoulderYaw = outward * (16 + Math.sin(p * tau) * 4);
    const shoulderRoll = outward * (10 + contactWeight * 8);
    const elbowBend = inAir ? lerp(92, 122, Math.sin(recoverT * Math.PI)) : lerp(126, 82, pullT);

    return {
      phase: round(p),
      contactWeight,
      shoulder: rot(shoulderPitch, shoulderYaw, shoulderRoll),
      elbow: { bend: round(elbowBend) },
      wrist: rot(-18 + contactWeight * 8, outward * 5, outward * -8),
      elbowPoint: elbow,
      handPoint: hand,
      forearmContactWeight: contactWeight,
      pullWeight: round(contactWeight * (1 - pullT * 0.25)),
      recovery: round(inAir ? recoverT : 0),
      planted: contactWeight >= 0.7
    };
  }

  function legState(frame, side, offset, bodyZ) {
    const p = phase(frame, offset);
    const isLeft = side === "left";
    const xBase = isLeft ? 8.5 : -8.5;
    const outward = isLeft ? 1 : -1;
    const contactWeight = contactWeightFromPhase(p);
    const pushT = smoothstep(clamp(p / 0.46, 0, 1));
    const recoverT = smoothstep(clamp((p - 0.56) / 0.28, 0, 1));
    const lateSettleT = smoothstep(clamp((p - 0.84) / 0.16, 0, 1));
    const inAir = contactWeight < 0.25;

    let kneeRelZ;
    let footRelZ;
    let kneeY;
    let footY;
    if (p < 0.56) {
      kneeRelZ = lerp(8, -18, pushT);
      footRelZ = lerp(-8, -28, pushT);
      kneeY = 1.0;
      footY = 1.0;
    } else {
      kneeRelZ = lerp(-18, 12, recoverT);
      footRelZ = lerp(-28, -2, recoverT);
      kneeY = 1.0 + Math.sin(recoverT * Math.PI) * 4.8;
      footY = 1.0 + Math.sin(recoverT * Math.PI) * 2.2;
    }
    if (p >= 0.84) {
      kneeRelZ = lerp(10, 8, lateSettleT);
      footRelZ = lerp(-3, -8, lateSettleT);
      kneeY = lerp(3.4, 1.0, lateSettleT);
      footY = lerp(1.8, 1.0, lateSettleT);
    }

    const knee = point(xBase + outward * 2.8, kneeY, bodyZ + kneeRelZ);
    const ankle = point(xBase + outward * 3.8, footY + 1.2, bodyZ + footRelZ - 13);
    const ball = point(xBase + outward * 4.5, footY, bodyZ + footRelZ);
    const hipPitch = lerp(42, 74, pushT) - (inAir ? recoverT * 28 : 0);
    const kneeBend = inAir ? lerp(122, 78, recoverT) : lerp(86, 132, pushT);

    return {
      phase: round(p),
      contactWeight,
      hip: rot(hipPitch, outward * 5, outward * 3),
      knee: { bend: round(kneeBend) },
      ankleRot: rot(-12 + pushT * 18, outward * 2, outward * 3),
      kneePoint: knee,
      anklePoint: ankle,
      ballPoint: ball,
      pushWeight: round(contactWeight * (0.45 + pushT * 0.55)),
      dragWeight: round(contactWeight * 0.45),
      recovery: round(inAir ? recoverT : 0),
      planted: contactWeight >= 0.7
    };
  }

  function frameName(frame) {
    const names = {
      0: "right_forearm_left_knee_set",
      3: "right_pull_left_knee_drive",
      6: "body_glide_recover_left_arm",
      9: "left_forearm_reach_right_knee_sets",
      12: "left_forearm_right_knee_set",
      15: "left_pull_right_knee_drive",
      18: "body_glide_recover_right_arm",
      21: "right_forearm_reach_left_knee_sets"
    };
    return names[frame] || `prone_crawl_${String(frame + 1).padStart(2, "0")}`;
  }

  function buildFrame(frame) {
    const t = frame / frameCount;
    const bodyZ = -cycleTravelZ * 0.5 + cycleTravelZ * t;
    const bodyCycle = Math.sin(tau * t);
    const doubleCycle = Math.sin(tau * t * 2);
    const leftArm = armState(frame, "left", 12, bodyZ);
    const rightArm = armState(frame, "right", 0, bodyZ);
    const leftLeg = legState(frame, "left", 0, bodyZ);
    const rightLeg = legState(frame, "right", 12, bodyZ);
    const rightSupport = rightArm.contactWeight * leftLeg.contactWeight;
    const leftSupport = leftArm.contactWeight * rightLeg.contactWeight;
    const supportBias = rightSupport - leftSupport;
    const xSway = supportBias * -1.8 + Math.sin(tau * t + Math.PI * 0.25) * 0.5;
    const pelvisY = 19.2 + Math.max(rightSupport, leftSupport) * -0.8 + Math.abs(doubleCycle) * 0.7;
    const chestY = 21.0 + Math.max(rightArm.contactWeight, leftArm.contactWeight) * -1.0 + Math.abs(bodyCycle) * 0.35;
    const headY = 26.4 + Math.max(rightArm.contactWeight, leftArm.contactWeight) * -0.3;
    const keyFrames = new Set([0, 3, 6, 9, 12, 15, 18, 21]);

    return {
      frame,
      name: frameName(frame),
      key: keyFrames.has(frame),
      root: {
        pos: vec(round(xSway * 0.3), 0, bodyZ),
        travelZ: round(bodyZ),
        cycleTravelZ
      },
      pelvis: {
        pos: vec(xSway, pelvisY, bodyZ - 5.0),
        rot: rot(-78 + Math.abs(doubleCycle) * 3.0, supportBias * 7.0, supportBias * -5.0)
      },
      chest: {
        pos: vec(xSway * 0.55, chestY, bodyZ + 11.5),
        rot: rot(-82 + Math.max(rightSupport, leftSupport) * 2.5, supportBias * -10.0, supportBias * 7.0)
      },
      head: {
        pos: vec(xSway * 0.28, headY, bodyZ + 27.5),
        rot: rot(-22 + Math.abs(bodyCycle) * 3.0, supportBias * -8.0, supportBias * 2.5),
        lookAt: vec(xSway * 0.15, 24.0, bodyZ + 64.0)
      },
      leftArm: {
        shoulder: leftArm.shoulder,
        elbow: leftArm.elbow,
        wrist: leftArm.wrist,
        elbowPoint: leftArm.elbowPoint,
        handPoint: leftArm.handPoint,
        recovery: leftArm.recovery,
        pullWeight: leftArm.pullWeight
      },
      rightArm: {
        shoulder: rightArm.shoulder,
        elbow: rightArm.elbow,
        wrist: rightArm.wrist,
        elbowPoint: rightArm.elbowPoint,
        handPoint: rightArm.handPoint,
        recovery: rightArm.recovery,
        pullWeight: rightArm.pullWeight
      },
      leftLeg: {
        hip: leftLeg.hip,
        knee: leftLeg.knee,
        ankle: leftLeg.ankleRot,
        kneePoint: leftLeg.kneePoint,
        anklePoint: leftLeg.anklePoint,
        ballPoint: leftLeg.ballPoint,
        recovery: leftLeg.recovery,
        pushWeight: leftLeg.pushWeight
      },
      rightLeg: {
        hip: rightLeg.hip,
        knee: rightLeg.knee,
        ankle: rightLeg.ankleRot,
        kneePoint: rightLeg.kneePoint,
        anklePoint: rightLeg.anklePoint,
        ballPoint: rightLeg.ballPoint,
        recovery: rightLeg.recovery,
        pushWeight: rightLeg.pushWeight
      },
      leftHand: {
        planted: leftArm.planted,
        point: leftArm.handPoint,
        position: leftArm.handPoint,
        contact: leftArm.planted ? "ground" : "air",
        contactWeight: leftArm.contactWeight,
        dragLock: leftArm.planted
      },
      rightHand: {
        planted: rightArm.planted,
        point: rightArm.handPoint,
        position: rightArm.handPoint,
        contact: rightArm.planted ? "ground" : "air",
        contactWeight: rightArm.contactWeight,
        dragLock: rightArm.planted
      },
      leftElbow: {
        planted: leftArm.planted,
        point: leftArm.elbowPoint,
        position: leftArm.elbowPoint,
        contact: leftArm.planted ? "ground" : "air",
        contactWeight: leftArm.forearmContactWeight
      },
      rightElbow: {
        planted: rightArm.planted,
        point: rightArm.elbowPoint,
        position: rightArm.elbowPoint,
        contact: rightArm.planted ? "ground" : "air",
        contactWeight: rightArm.forearmContactWeight
      },
      leftKnee: {
        planted: leftLeg.planted,
        point: leftLeg.kneePoint,
        position: leftLeg.kneePoint,
        contact: leftLeg.planted ? "ground" : "air",
        contactWeight: leftLeg.contactWeight,
        pushWeight: leftLeg.pushWeight
      },
      rightKnee: {
        planted: rightLeg.planted,
        point: rightLeg.kneePoint,
        position: rightLeg.kneePoint,
        contact: rightLeg.planted ? "ground" : "air",
        contactWeight: rightLeg.contactWeight,
        pushWeight: rightLeg.pushWeight
      },
      leftFoot: {
        planted: leftLeg.contactWeight >= 0.35,
        ankle: leftLeg.anklePoint,
        ball: leftLeg.ballPoint,
        contact: leftLeg.contactWeight >= 0.35 ? "drag" : "air",
        contactWeight: round(leftLeg.contactWeight * 0.65),
        dragWeight: leftLeg.dragWeight
      },
      rightFoot: {
        planted: rightLeg.contactWeight >= 0.35,
        ankle: rightLeg.anklePoint,
        ball: rightLeg.ballPoint,
        contact: rightLeg.contactWeight >= 0.35 ? "drag" : "air",
        contactWeight: round(rightLeg.contactWeight * 0.65),
        dragWeight: rightLeg.dragWeight
      },
      contacts: {
        supportPair: rightSupport >= leftSupport ? "right_forearm_left_knee" : "left_forearm_right_knee",
        leftHand: leftArm.planted ? "ground" : "air",
        rightHand: rightArm.planted ? "ground" : "air",
        leftElbow: leftArm.planted ? "ground" : "air",
        rightElbow: rightArm.planted ? "ground" : "air",
        leftKnee: leftLeg.planted ? "ground" : "air",
        rightKnee: rightLeg.planted ? "ground" : "air",
        leftFoot: leftLeg.contactWeight >= 0.35 ? "drag" : "air",
        rightFoot: rightLeg.contactWeight >= 0.35 ? "drag" : "air",
        bodyLow: true
      }
    };
  }

  target.mannyProneCrawlKeyposes = {
    id: "proneCrawl",
    name: "Prone Crawl",
    loop: true,
    frameCount,
    fps,
    coordinateSystem: "X side, Y up, Z forward",
    coordinateSystemDetail: {
      side: "X",
      up: "Y",
      forward: "Z",
      positionUnits: "Manny demo units",
      rotationUnits: "degrees"
    },
    rootMotion: {
      travelAxis: "Z",
      cycleTravelZ,
      wrapMode: "subtract cycleTravelZ at loop boundary"
    },
    builderHints: {
      interpolation: "Cubic ease between frames; hold planted hands, elbows, knees, and dragging feet while contactWeight is high.",
      pose: "Keep pelvis, chest, and head low. Solve spine first, then arms to forearm/hand contacts, then knees and dragging feet.",
      contacts: "Forearm plus opposite knee drives the crawl; feet mostly drag and only push lightly.",
      style: "Low tactical prone crawl, non-explicit, alternating elbow pulls and knee pushes."
    },
    keyposes: [
      { frame: 0, name: "right forearm plants while left knee loads" },
      { frame: 3, name: "right elbow pulls, left knee pushes body forward" },
      { frame: 6, name: "center glide while left arm recovers" },
      { frame: 9, name: "left forearm reaches and right knee sets" },
      { frame: 12, name: "left forearm plants while right knee loads" },
      { frame: 15, name: "left elbow pulls, right knee pushes body forward" },
      { frame: 18, name: "center glide while right arm recovers" },
      { frame: 21, name: "right forearm reaches and left knee sets" }
    ],
    frames: Array.from({ length: frameCount }, (_, frame) => buildFrame(frame))
  };
}());
