(function (global) {
  "use strict";

  const frameCount = 56;
  const fps = 24;
  const durationSeconds = frameCount / fps;
  const doorWidth = 68;
  const doorHeight = 116;
  const doorThickness = 4;
  const hinge = [-34, 58, 42];

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

  function foot(side, x, y, z, yaw, planted, role, locked) {
    const yawRad = (yaw * Math.PI) / 180;
    const ballY = planted ? 1.05 : Math.max(1.8, y - 8.5);

    return {
      ankle: vec(side * x, y, z),
      ball: vec(side * x + Math.sin(yawRad) * 2.8, ballY, z + Math.cos(yawRad) * 14.6),
      planted: Boolean(planted),
      locked: Boolean(locked && planted),
      contactWeight: planted ? 1 : 0,
      pivot: role
    };
  }

  function cloneFoot(source) {
    return {
      ankle: source.ankle.slice(),
      ball: source.ball.slice(),
      planted: Boolean(source.planted),
      locked: Boolean(source.locked),
      contactWeight: round(source.contactWeight == null ? (source.planted ? 1 : 0) : source.contactWeight),
      pivot: source.pivot || ""
    };
  }

  function interpolateFoot(a, b, t) {
    const locked = Boolean(a.locked && b.locked && a.planted && b.planted);
    const contactWeight = lerp(a.contactWeight == null ? (a.planted ? 1 : 0) : a.contactWeight, b.contactWeight == null ? (b.planted ? 1 : 0) : b.contactWeight, t);

    return {
      ankle: locked ? a.ankle.slice() : mixVec(a.ankle, b.ankle, t),
      ball: locked ? a.ball.slice() : mixVec(a.ball, b.ball, t),
      planted: contactWeight > 0.45 || (t < 0.5 ? a.planted : b.planted),
      locked,
      contactWeight: round(contactWeight),
      pivot: t < 0.5 ? a.pivot : b.pivot
    };
  }

  function pose(
    frame,
    name,
    phase,
    doorAngle,
    handContactWeight,
    pelvis,
    chest,
    neck,
    head,
    leftHandTarget,
    rightHandTarget,
    leftFoot,
    rightFoot,
    leftKneeBias,
    rightKneeBias
  ) {
    return {
      frame,
      name,
      phase,
      doorAngle,
      handContactWeight,
      pelvis,
      chest,
      neck,
      head,
      lUpper: vec(15, -10, 8),
      lLower: vec(11, -14, 7),
      lHand: vec(6, -8, 5),
      rUpper: vec(-15, -10, 8),
      rLower: vec(-11, -14, 7),
      rHand: vec(-6, -8, 5),
      leftHandTarget,
      rightHandTarget,
      leftFoot,
      rightFoot,
      leftKneeBias,
      rightKneeBias
    };
  }

  const keyFrames = [
    pose(
      0,
      "approach_ready",
      "approach",
      0,
      0,
      vec(0, 93, -31).concat(rot(-1, 0, 0)),
      rot(2, 0, 0),
      rot(0, 0, 0),
      rot(-2, 0, 0),
      vec(15, 86, -18),
      vec(-15, 87, -16),
      foot(1, 10.8, 8.1, -43, 1, true, "rear_walk_contact", true),
      foot(-1, 10.8, 8.1, -24, 0, true, "front_walk_contact", true),
      vec(2.2, -0.5, 4.0),
      vec(-2.1, -0.4, 4.2)
    ),
    pose(
      6,
      "last_approach_step",
      "approach",
      0,
      0,
      vec(0.8, 92.4, -23).concat(rot(-2, 2, 0)),
      rot(4, 1, 0),
      rot(-1, 0, 0),
      rot(-4, 1, 0),
      vec(15, 85, -10),
      vec(-14, 90, -5),
      foot(1, 10.6, 18.5, -21, 3, false, "left_step_swing", false),
      foot(-1, 10.8, 8.1, -24, 0, true, "right_support", true),
      vec(3.4, -0.2, 5.6),
      vec(-2.4, -0.5, 4.4)
    ),
    pose(
      10,
      "left_foot_brace_set",
      "brace",
      0,
      0.15,
      vec(0.7, 91.4, -13).concat(rot(-5, 1, 0)),
      rot(8, -1, 0),
      rot(-2, 0, 0),
      rot(-7, -2, 0),
      vec(16, 88, -2),
      vec(-14, 102, 29),
      foot(1, 11.2, 8.1, -11, 2, true, "front_brace", true),
      foot(-1, 10.8, 8.1, -24, 0, true, "rear_support", true),
      vec(3.5, -0.4, 6.0),
      vec(-2.4, -0.3, 4.8)
    ),
    pose(
      16,
      "right_palm_contacts_heavy_door",
      "contact",
      0,
      1,
      vec(0.8, 89.2, -7).concat(rot(-10, -2, 1)),
      rot(15, -4, 1),
      rot(-4, 1, 0),
      rot(-10, -4, 0),
      vec(18, 90, 2),
      vec(-13, 113, 40),
      foot(1, 11.2, 8.1, -11, 2, true, "front_brace_locked", true),
      foot(-1, 10.8, 8.1, -24, 0, true, "rear_loaded", true),
      vec(4.2, -0.8, 7.2),
      vec(-3.3, -0.7, 6.8)
    ),
    pose(
      24,
      "shoulder_drive_door_breaks_open",
      "push",
      24,
      1,
      vec(1.2, 88.0, 2).concat(rot(-14, -4, 1)),
      rot(20, -7, 2),
      rot(-5, 2, 0),
      rot(-12, -5, 0),
      vec(18, 88, 5),
      vec(-10, 112, 51),
      foot(1, 11.2, 8.1, -11, 2, true, "front_brace_locked", true),
      foot(-1, 10.8, 8.1, -24, -5, true, "rear_push_drive", true),
      vec(4.8, -1.2, 8.0),
      vec(-4.0, -1.0, 7.8)
    ),
    pose(
      32,
      "step_through_opening",
      "step_through",
      53,
      0.88,
      vec(0.8, 90.0, 14).concat(rot(-9, 8, 0)),
      rot(13, 8, 1),
      rot(-3, -3, 0),
      rot(-7, -8, 0),
      vec(15, 88, 13),
      vec(-6, 111, 63),
      foot(1, 11.2, 8.1, -11, 4, true, "left_threshold_brace", true),
      foot(-1, 10.7, 20.0, 12, 8, false, "right_step_through_air", false),
      vec(4.1, -0.8, 6.6),
      vec(-5.7, 1.5, 7.4)
    ),
    pose(
      40,
      "right_foot_lands_inside",
      "passage",
      72,
      0.55,
      vec(0.3, 92.0, 26).concat(rot(-4, 15, -1)),
      rot(6, 16, 0),
      rot(-2, -6, 0),
      rot(-4, -12, 0),
      vec(14, 90, 22),
      vec(1, 107, 67),
      foot(1, 11.2, 8.1, -11, 4, true, "left_trailing_support", true),
      foot(-1, 10.8, 8.1, 20, 9, true, "right_landing_inside", true),
      vec(3.2, -0.3, 5.0),
      vec(-3.1, -0.4, 5.2)
    ),
    pose(
      48,
      "release_and_left_step",
      "release",
      83,
      0,
      vec(-0.2, 92.8, 35).concat(rot(-2, 7, 0)),
      rot(3, 8, 0),
      rot(-1, -3, 0),
      rot(-2, -6, 0),
      vec(13, 88, 32),
      vec(-8, 94, 47),
      foot(1, 10.8, 17.5, 32, 3, false, "left_recovery_step_air", false),
      foot(-1, 10.8, 8.1, 20, 9, true, "right_inside_support", true),
      vec(3.6, 0.5, 5.4),
      vec(-2.5, -0.2, 4.3)
    ),
    pose(
      55,
      "recovered_inside_room",
      "recover",
      86,
      0,
      vec(0, 93.0, 43).concat(rot(-1, 0, 0)),
      rot(2, 0, 0),
      rot(0, 0, 0),
      rot(-2, 0, 0),
      vec(14, 86, 42),
      vec(-14, 86, 42),
      foot(1, 10.8, 8.1, 36, 2, true, "left_recovered_inside", true),
      foot(-1, 10.8, 8.1, 20, 9, true, "right_recovered_inside", true),
      vec(2.2, -0.3, 4.2),
      vec(-2.1, -0.3, 4.2)
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
      name: "door_push_inbetween",
      phase: previous.phase === next.phase ? previous.phase : "transition",
      doorAngle: lerp(previous.doorAngle, next.doorAngle, t),
      handContactWeight: lerp(previous.handContactWeight, next.handContactWeight, t),
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
    const rightHandContact = sample.handContactWeight > 0.65
      ? "right_palm_driving_door"
      : sample.handContactWeight > 0.1
        ? "right_hand_light_door_contact"
        : "free";
    const doorState = sample.doorAngle < 5
      ? "closed"
      : sample.doorAngle < 70
        ? "opening"
        : "open";

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
        leftFoot: leftFoot.planted ? (leftFoot.locked ? "ground_locked" : "ground") : "air",
        rightFoot: rightFoot.planted ? (rightFoot.locked ? "ground_locked" : "ground") : "air",
        rightHand: rightHandContact,
        leftHand: "balance_free",
        prop: "heavy_hinged_door",
        supportFoot: rightFoot.planted && sample.phase === "release" ? "right" : "left"
      },
      door: {
        id: "heavy_hinged_door",
        type: "hinged_door",
        hinge: vec(hinge[0], hinge[1], hinge[2]),
        width: doorWidth,
        height: doorHeight,
        thickness: doorThickness,
        angleDegrees: round(sample.doorAngle),
        yaw: round(sample.doorAngle),
        state: doorState,
        contactPoint: sample.rightHandTarget.slice(),
        handContactWeight: round(sample.handContactWeight),
        opensToward: "forward room side as Manny steps through"
      },
      effort: {
        bodyLean: round(Math.abs(sample.pelvis[3]) + Math.max(0, sample.handContactWeight) * 4),
        pushWeight: round(sample.handContactWeight),
        forwardProgress: round(sample.pelvis[2])
      }
    };
  }

  const frames = Array.from({ length: frameCount }, (_, frame) => frameFromSample(sampleKey(frame)));
  const keyposes = frames.filter((frame) => frame.key);

  const data = {
    id: "doorPush",
    name: "Door Push",
    koreanName: "문 밀고 들어가기",
    displayNames: {
      en: "Door Push",
      ko: "문 밀고 들어가기"
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
      intent: "Manny approaches a heavy hinged door, plants a braced foot, pushes with one palm, steps through the opening, releases, and recovers.",
      notes: [
        "The left foot braces at the threshold while the right palm contacts the door.",
        "The pelvis lowers and leans forward during the heavy push before the door breaks open.",
        "The right foot steps through first, then the left foot recovers inside the room.",
        "The motion is non-looping because the door remains open at the end."
      ]
    },
    prop: {
      id: "heavy_hinged_door",
      type: "hinged_door",
      width: doorWidth,
      height: doorHeight,
      thickness: doorThickness,
      hinge: vec(hinge[0], hinge[1], hinge[2]),
      hingeSide: "left",
      closedPlaneZ: hinge[2],
      opensFromDegrees: 0,
      opensToDegrees: 86,
      drivenBy: "per-frame door.angleDegrees and rightHandTarget contact point"
    },
    environment: {
      id: "doorway",
      type: "interior_threshold",
      thresholdZ: 18,
      note: "Door is a fixed hinged obstacle in front of Manny; no wall geometry is required by the sampler."
    },
    contactNotes: {
      feet: "Approach steps alternate, then the left foot locks as the brace foot until the right foot lands inside.",
      hand: "Right hand contact weight rises at frame 10, drives the door from frames 16-40, and releases before recovery.",
      door: "Door metadata carries hinge dimensions, angle, and the live palm contact point for optional prop rendering."
    },
    keyposes,
    frames
  };

  data.keys = frames;

  global.mannyDoorPushKeyposes = data;
})(typeof window !== "undefined" ? window : globalThis);
