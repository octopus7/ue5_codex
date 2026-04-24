(function (global) {
  "use strict";

  const frameCount = 56;
  const fps = 24;
  const durationSeconds = frameCount / fps;
  const objectSize = [12, 8, 12];
  const floorObjectStart = [0, objectSize[1] * 0.5, 36];

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

  function rot(pitch, yaw, roll) {
    return [round(pitch), round(yaw), round(roll)];
  }

  function mixVec(a, b, t) {
    return vec(lerp(a[0], b[0], t), lerp(a[1], b[1], t), lerp(a[2], b[2], t));
  }

  function foot(side, x, z, yaw, role) {
    const yawRad = (yaw * Math.PI) / 180;

    return {
      ankle: vec(side * x, 8.1, z),
      ball: vec(side * x + Math.sin(yawRad) * 2.6, 1.05, z + Math.cos(yawRad) * 14.8),
      planted: true,
      locked: true,
      contactWeight: 1,
      pivot: role
    };
  }

  function cloneFoot(source) {
    return {
      ankle: source.ankle.slice(),
      ball: source.ball.slice(),
      planted: Boolean(source.planted),
      locked: Boolean(source.locked),
      contactWeight: round(source.contactWeight == null ? 1 : source.contactWeight),
      pivot: source.pivot || ""
    };
  }

  function interpolateFoot(a, b, t) {
    const locked = Boolean(a.locked && b.locked && a.planted && b.planted);

    return {
      ankle: locked ? a.ankle.slice() : mixVec(a.ankle, b.ankle, t),
      ball: locked ? a.ball.slice() : mixVec(a.ball, b.ball, t),
      planted: true,
      locked,
      contactWeight: 1,
      pivot: t < 0.5 ? a.pivot : b.pivot
    };
  }

  function pose(
    frame,
    name,
    phase,
    pelvis,
    chest,
    neck,
    head,
    leftHandTarget,
    rightHandTarget,
    objectCenter,
    gripWeight,
    kneeBend,
    leftKneeBias,
    rightKneeBias
  ) {
    return {
      frame,
      name,
      phase,
      pelvis,
      chest,
      neck,
      head,
      lUpper: vec(14, -9, 8),
      lLower: vec(10, -13, 7),
      lHand: vec(5, -8, 5),
      rUpper: vec(-14, -9, 8),
      rLower: vec(-10, -13, 7),
      rHand: vec(-5, -8, 5),
      leftHandTarget,
      rightHandTarget,
      objectCenter,
      gripWeight,
      kneeBend,
      leftFoot: foot(1, 12.2, 0, 1, "left_planted_no_slide"),
      rightFoot: foot(-1, 12.2, 0, -1, "right_planted_no_slide"),
      leftKneeBias,
      rightKneeBias
    };
  }

  const keyFrames = [
    pose(
      0,
      "stand_over_floor_object",
      "stand",
      vec(0, 93, 0).concat(rot(-1, 0, 0)),
      rot(3, 0, 0),
      rot(-1, 0, 0),
      rot(-4, 0, 0),
      vec(13, 84, 8),
      vec(-13, 84, 8),
      vec(floorObjectStart[0], floorObjectStart[1], floorObjectStart[2]),
      0,
      6,
      vec(2.0, -0.3, 4.0),
      vec(-2.0, -0.3, 4.0)
    ),
    pose(
      6,
      "look_down_and_shift",
      "prepare",
      vec(0, 91.5, -1).concat(rot(-5, 0, 0)),
      rot(12, 0, 0),
      rot(-4, 0, 0),
      rot(-17, 0, 0),
      vec(13, 76, 15),
      vec(-13, 76, 15),
      vec(floorObjectStart[0], floorObjectStart[1], floorObjectStart[2]),
      0,
      16,
      vec(2.8, -0.4, 5.4),
      vec(-2.8, -0.4, 5.4)
    ),
    pose(
      12,
      "hip_hinge_and_knee_bend",
      "hinge",
      vec(0, 80, -7).concat(rot(-19, 0, 0)),
      rot(40, 0, 0),
      rot(-14, 0, 0),
      rot(-31, 0, 0),
      vec(13, 52, 29),
      vec(-13, 52, 29),
      vec(floorObjectStart[0], floorObjectStart[1], floorObjectStart[2]),
      0,
      36,
      vec(4.0, -0.6, 7.2),
      vec(-4.0, -0.6, 7.2)
    ),
    pose(
      20,
      "deep_squat_reach_to_floor",
      "reach",
      vec(0, 55, -10).concat(rot(-34, 0, 0)),
      rot(78, 0, 0),
      rot(-24, 0, 0),
      rot(-43, 0, 0),
      vec(9, 21, 36),
      vec(-9, 21, 36),
      vec(floorObjectStart[0], floorObjectStart[1], floorObjectStart[2]),
      0.35,
      68,
      vec(5.4, -1.0, 9.2),
      vec(-5.4, -1.0, 9.2)
    ),
    pose(
      25,
      "two_hand_grasp_floor_object",
      "grasp",
      vec(0, 51, -10).concat(rot(-36, 0, 0)),
      rot(84, 0, 0),
      rot(-25, 0, 0),
      rot(-46, 0, 0),
      vec(5.8, 15, 37),
      vec(-5.8, 15, 37),
      vec(floorObjectStart[0], floorObjectStart[1], floorObjectStart[2]),
      1,
      74,
      vec(5.8, -1.2, 9.8),
      vec(-5.8, -1.2, 9.8)
    ),
    pose(
      32,
      "lift_object_to_shins",
      "floor_pull",
      vec(0, 61, -8).concat(rot(-29, 0, 0)),
      rot(68, 0, 0),
      rot(-21, 0, 0),
      rot(-38, 0, 0),
      vec(6, 32, 34),
      vec(-6, 32, 34),
      vec(0, 25, 34),
      1,
      62,
      vec(5.1, -0.9, 8.6),
      vec(-5.1, -0.9, 8.6)
    ),
    pose(
      40,
      "lift_close_to_body",
      "close_lift",
      vec(0, 76, -4).concat(rot(-17, 0, 0)),
      rot(37, 0, 0),
      rot(-13, 0, 0),
      rot(-25, 0, 0),
      vec(7, 58, 25),
      vec(-7, 58, 25),
      vec(0, 51, 25),
      1,
      40,
      vec(3.9, -0.6, 6.7),
      vec(-3.9, -0.6, 6.7)
    ),
    pose(
      49,
      "stand_with_object_close",
      "stand",
      vec(0, 92, 0).concat(rot(-4, 0, 0)),
      rot(9, 0, 0),
      rot(-3, 0, 0),
      rot(-10, 0, 0),
      vec(8, 86, 17),
      vec(-8, 86, 17),
      vec(0, 79, 18),
      1,
      12,
      vec(2.4, -0.3, 4.4),
      vec(-2.4, -0.3, 4.4)
    ),
    pose(
      55,
      "small_settle_hold",
      "settle",
      vec(0, 93, 0).concat(rot(-1, 0, 0)),
      rot(4, 0, 0),
      rot(-1, 0, 0),
      rot(-6, 0, 0),
      vec(8, 84, 17),
      vec(-8, 84, 17),
      vec(0, 77, 18),
      1,
      8,
      vec(2.0, -0.3, 4.0),
      vec(-2.0, -0.3, 4.0)
    )
  ];

  const keyNames = new Map(keyFrames.map((key) => [key.frame, key.name]));

  function sampleKey(frame) {
    const exact = keyFrames.find((key) => key.frame === frame);
    if (exact) {
      return exact;
    }

    let previous = keyFrames[0];
    let next = keyFrames[keyFrames.length - 1];

    for (let index = 0; index < keyFrames.length - 1; index += 1) {
      if (frame >= keyFrames[index].frame && frame <= keyFrames[index + 1].frame) {
        previous = keyFrames[index];
        next = keyFrames[index + 1];
        break;
      }
    }

    const span = Math.max(1, next.frame - previous.frame);
    const t = smoothstep((frame - previous.frame) / span);

    return {
      frame,
      name: "floor_pickup_inbetween",
      phase: previous.phase === next.phase ? previous.phase : "transition",
      pelvis: mixVec(previous.pelvis.slice(0, 3), next.pelvis.slice(0, 3), t).concat(mixVec(previous.pelvis.slice(3, 6), next.pelvis.slice(3, 6), t)),
      chest: mixVec(previous.chest, next.chest, t),
      neck: mixVec(previous.neck, next.neck, t),
      head: mixVec(previous.head, next.head, t),
      lUpper: mixVec(previous.lUpper, next.lUpper, t),
      lLower: mixVec(previous.lLower, next.lLower, t),
      lHand: mixVec(previous.lHand, next.lHand, t),
      rUpper: mixVec(previous.rUpper, next.rUpper, t),
      rLower: mixVec(previous.rLower, next.rLower, t),
      rHand: mixVec(previous.rHand, next.rHand, t),
      leftHandTarget: mixVec(previous.leftHandTarget, next.leftHandTarget, t),
      rightHandTarget: mixVec(previous.rightHandTarget, next.rightHandTarget, t),
      objectCenter: mixVec(previous.objectCenter, next.objectCenter, t),
      gripWeight: lerp(previous.gripWeight, next.gripWeight, t),
      kneeBend: lerp(previous.kneeBend, next.kneeBend, t),
      leftFoot: interpolateFoot(previous.leftFoot, next.leftFoot, t),
      rightFoot: interpolateFoot(previous.rightFoot, next.rightFoot, t),
      leftKneeBias: mixVec(previous.leftKneeBias, next.leftKneeBias, t),
      rightKneeBias: mixVec(previous.rightKneeBias, next.rightKneeBias, t)
    };
  }

  function frameFromSample(sample) {
    const leftFoot = cloneFoot(sample.leftFoot);
    const rightFoot = cloneFoot(sample.rightFoot);
    const leftKneeBias = sample.leftKneeBias.slice();
    const rightKneeBias = sample.rightKneeBias.slice();
    const gripping = sample.gripWeight > 0.65;
    const reaching = sample.gripWeight > 0.05 && !gripping;
    const objectOnFloor = sample.objectCenter[1] <= floorObjectStart[1] + 0.05;

    return {
      frame: sample.frame,
      name: keyNames.get(sample.frame) || sample.name,
      key: keyNames.has(sample.frame),
      phase: sample.phase,
      pelvis: sample.pelvis.slice(),
      chest: sample.chest.slice(),
      neck: sample.neck.slice(),
      head: sample.head.slice(),
      lUpper: sample.lUpper.slice(),
      lLower: sample.lLower.slice(),
      lHand: sample.lHand.slice(),
      rUpper: sample.rUpper.slice(),
      rLower: sample.rLower.slice(),
      rHand: sample.rHand.slice(),
      leftHandTarget: sample.leftHandTarget.slice(),
      rightHandTarget: sample.rightHandTarget.slice(),
      leftFoot,
      rightFoot,
      leftKneeBias,
      rightKneeBias,
      kneeBias: {
        left: leftKneeBias.slice(),
        right: rightKneeBias.slice()
      },
      contacts: {
        leftFoot: "ground_locked",
        rightFoot: "ground_locked",
        leftHand: gripping ? "left_grip_on_floor_object" : reaching ? "reaching_to_object" : "free",
        rightHand: gripping ? "right_grip_on_floor_object" : reaching ? "reaching_to_object" : "free",
        prop: "small_floor_object"
      },
      object: {
        id: "small_floor_object",
        type: "small_pickup_prop",
        center: sample.objectCenter.slice(),
        size: vec(objectSize[0], objectSize[1], objectSize[2]),
        onFloor: objectOnFloor,
        held: gripping && !objectOnFloor,
        gripWeight: round(sample.gripWeight),
        leftGrip: sample.leftHandTarget.slice(),
        rightGrip: sample.rightHandTarget.slice(),
        path: "floor to shins, then close to torso before the standing settle"
      },
      effort: {
        kneeBend: round(sample.kneeBend),
        hipHinge: round(Math.max(0, sample.chest[0])),
        gripWeight: round(sample.gripWeight)
      }
    };
  }

  const frames = Array.from({ length: frameCount }, (_, frame) => frameFromSample(sampleKey(frame)));
  const keyposes = frames.filter((frame) => frame.key);

  const data = {
    id: "floorPickup",
    name: "Floor Pickup",
    koreanName: "바닥 물건 줍기",
    displayNames: {
      en: "Floor Pickup",
      ko: "바닥 물건 줍기"
    },
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
      intent: "Manny stands near a small object on the floor, squats with a hip hinge, grasps it, lifts it close to the body, stands, and settles.",
      notes: [
        "Both feet stay planted and locked so the motion reads as a stable squat pickup.",
        "The pelvis lowers strongly while the chest folds forward and the head looks down at the object.",
        "Hands reach toward the floor object, establish a two-hand grasp, then lift along a close path.",
        "The object remains close to the shins and torso to avoid an outstretched, weightless lift."
      ]
    },
    prop: {
      id: "small_floor_object",
      type: "small_pickup_prop",
      size: vec(objectSize[0], objectSize[1], objectSize[2]),
      initialCenter: vec(floorObjectStart[0], floorObjectStart[1], floorObjectStart[2]),
      contact: "Both hands grasp opposite sides before the object leaves the floor.",
      drivenBy: "per-frame object.center plus left/right hand targets"
    },
    environment: {
      id: "neutral_floor",
      type: "flat_ground",
      groundY: 0,
      note: "No furniture is required; the prop begins directly on the floor in front of Manny."
    },
    contactNotes: {
      feet: "Both feet remain ground-locked for the full non-looping pickup.",
      hands: "Hands reach during the squat, grip from frame 25 onward, and stay close to the object through the stand.",
      interpolation: "Smoothstep in-betweens preserve the hinge, deep squat, floor pull, close-body lift, stand, and settle beats."
    },
    keyposes,
    frames
  };

  data.keys = frames;

  global.mannyFloorPickupKeyposes = data;
})(typeof window !== "undefined" ? window : globalThis);
