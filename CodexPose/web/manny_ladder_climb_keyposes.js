(function (global) {
  "use strict";

  const frameCount = 48;
  const fps = 24;
  const durationSeconds = frameCount / fps;
  const rungSpacing = 20;
  const cycleVerticalTravel = 20;
  const ladderZ = 38;

  function round(value) {
    return Number(value.toFixed(3));
  }

  function vec(x, y, z) {
    return [round(x), round(y), round(z)];
  }

  function rot(pitch, yaw, roll) {
    return [round(pitch), round(yaw), round(roll)];
  }

  function clamp(value, min, max) {
    return Math.max(min, Math.min(max, value));
  }

  function smoothstep(value) {
    const t = clamp(value, 0, 1);
    return t * t * (3 - t * 2);
  }

  function mix(a, b, t) {
    return a + (b - a) * t;
  }

  function mixVec(a, b, t) {
    return vec(mix(a[0], b[0], t), mix(a[1], b[1], t), mix(a[2], b[2], t));
  }

  function foot(x, rungY, z, planted, locked, contactWeight, rungIndex, role) {
    const side = x >= 0 ? 1 : -1;
    return {
      ankle: vec(x, rungY + 7.2, z - 3.8),
      ball: vec(x + side * 1.1, rungY, z + 6.6),
      planted: Boolean(planted),
      locked: Boolean(locked),
      contactWeight: round(contactWeight),
      rungIndex,
      role
    };
  }

  function hand(x, y, z, contactWeight, rungIndex, role) {
    return {
      target: vec(x, y, z),
      contactWeight: round(contactWeight),
      locked: contactWeight >= 0.86,
      rungIndex,
      role
    };
  }

  function key(frame, name, phase, pelvis, chest, neck, head, leftArm, rightArm, leftHand, rightHand, leftFoot, rightFoot, leftKneeBias, rightKneeBias) {
    return {
      frame,
      name,
      phase,
      pelvis,
      chest,
      neck,
      head,
      lUpper: leftArm[0],
      lLower: leftArm[1],
      lHand: leftArm[2],
      rUpper: rightArm[0],
      rLower: rightArm[1],
      rHand: rightArm[2],
      leftHand,
      rightHand,
      leftFoot,
      rightFoot,
      leftKneeBias,
      rightKneeBias
    };
  }

  const keyFrames = [
    key(
      0,
      "left_high_right_low_set",
      "cycle_start",
      vec(0, 86, 18).concat(rot(-10, 0, 0)),
      rot(16, 0, 0),
      rot(-5, 0, 0),
      rot(-8, 0, 0),
      [vec(12, 14, 10), vec(16, 16, 10), vec(7, 4, 4)],
      [vec(-12, 8, 11), vec(-14, 10, 10), vec(-6, 3, 4)],
      hand(16, 156, ladderZ, 1, 7, "high_hold"),
      hand(-16, 136, ladderZ, 1, 6, "low_hold"),
      foot(10.5, 56, ladderZ - 5, true, true, 1, 2, "high_step_hold"),
      foot(-10.5, 36, ladderZ - 5, true, true, 1, 1, "low_step_hold"),
      vec(3.6, -1.6, 8.2),
      vec(-2.2, -1.2, 5.5)
    ),
    key(
      6,
      "right_hand_reaches_next_rung",
      "right_hand_reach",
      vec(0.8, 89, 20).concat(rot(-14, 3, -2)),
      rot(21, -4, 1),
      rot(-7, 1, 0),
      rot(-10, 2, 0),
      [vec(13, 16, 10), vec(17, 18, 10), vec(8, 5, 4)],
      [vec(-10, 19, 13), vec(-12, 18, 12), vec(-5, 6, 5)],
      hand(16, 156, ladderZ, 1, 7, "left_anchor"),
      hand(-16, 161, ladderZ + 1.5, 0.28, 7, "right_reaching"),
      foot(10.5, 56, ladderZ - 5, true, true, 1, 2, "left_foot_push"),
      foot(-10.5, 36, ladderZ - 5, true, true, 0.88, 1, "right_foot_hold"),
      vec(4.2, -1.8, 8.8),
      vec(-2.4, -1.0, 5.7)
    ),
    key(
      12,
      "right_hand_catches_high",
      "right_hand_catch",
      vec(0.6, 93, 21).concat(rot(-12, 2, -1)),
      rot(18, -2, 1),
      rot(-6, 1, 0),
      rot(-8, 1, 0),
      [vec(12, 14, 10), vec(16, 15, 10), vec(7, 4, 4)],
      [vec(-13, 15, 12), vec(-17, 15, 11), vec(-7, 4, 4)],
      hand(16, 156, ladderZ, 1, 7, "left_anchor"),
      hand(-16, 156, ladderZ, 1, 7, "right_catch_high"),
      foot(10.5, 56, ladderZ - 5, true, true, 1, 2, "left_foot_loaded"),
      foot(-10.5, 36, ladderZ - 5, true, true, 1, 1, "right_foot_hold"),
      vec(3.8, -1.5, 8.5),
      vec(-2.0, -1.0, 5.2)
    ),
    key(
      18,
      "left_foot_steps_up",
      "left_foot_reach",
      vec(1.0, 97, 21).concat(rot(-13, 1, -3)),
      rot(17, -1, 2),
      rot(-5, 0, 0),
      rot(-7, 0, 0),
      [vec(13, 12, 10), vec(17, 12, 9), vec(8, 3, 3)],
      [vec(-12, 14, 12), vec(-16, 14, 11), vec(-7, 4, 4)],
      hand(16, 156, ladderZ, 1, 7, "left_anchor"),
      hand(-16, 156, ladderZ, 1, 7, "right_anchor"),
      foot(10.5, 75, ladderZ - 4, false, false, 0.2, 3, "left_foot_lifting"),
      foot(-10.5, 36, ladderZ - 5, true, true, 1, 1, "right_foot_support"),
      vec(6.8, 1.2, 12.0),
      vec(-3.4, -1.5, 6.8)
    ),
    key(
      24,
      "left_foot_loads_higher_rung",
      "left_foot_catch",
      vec(0.3, 101, 22).concat(rot(-9, -1, 1)),
      rot(14, 1, -1),
      rot(-4, 0, 0),
      rot(-6, 0, 0),
      [vec(12, 10, 9), vec(16, 11, 8), vec(7, 2, 3)],
      [vec(-13, 13, 11), vec(-17, 13, 10), vec(-7, 3, 4)],
      hand(16, 156, ladderZ, 1, 7, "left_anchor"),
      hand(-16, 156, ladderZ, 1, 7, "right_anchor"),
      foot(10.5, 76, ladderZ - 5, true, true, 1, 3, "left_foot_new_support"),
      foot(-10.5, 36, ladderZ - 5, true, true, 0.92, 1, "right_foot_light"),
      vec(3.8, -1.2, 7.5),
      vec(-2.6, -1.1, 5.5)
    ),
    key(
      30,
      "left_hand_reaches_next_rung",
      "left_hand_reach",
      vec(-0.4, 104, 22).concat(rot(-13, -3, 2)),
      rot(20, 4, -1),
      rot(-6, -1, 0),
      rot(-9, -2, 0),
      [vec(12, 20, 12), vec(14, 18, 11), vec(6, 6, 5)],
      [vec(-13, 13, 10), vec(-17, 13, 9), vec(-7, 3, 3)],
      hand(16, 181, ladderZ + 1.5, 0.28, 8, "left_reaching"),
      hand(-16, 156, ladderZ, 1, 7, "right_anchor"),
      foot(10.5, 76, ladderZ - 5, true, true, 1, 3, "left_foot_support"),
      foot(-10.5, 36, ladderZ - 5, true, true, 0.8, 1, "right_foot_light"),
      vec(3.2, -1.0, 7.0),
      vec(-2.4, -0.8, 5.2)
    ),
    key(
      36,
      "left_hand_catches_high",
      "left_hand_catch",
      vec(-0.5, 107, 22).concat(rot(-10, -2, 1)),
      rot(16, 2, 0),
      rot(-5, -1, 0),
      rot(-7, -1, 0),
      [vec(13, 15, 11), vec(17, 15, 10), vec(7, 4, 4)],
      [vec(-12, 11, 9), vec(-16, 11, 8), vec(-7, 2, 3)],
      hand(16, 176, ladderZ, 1, 8, "left_catch_high"),
      hand(-16, 156, ladderZ, 1, 7, "right_anchor"),
      foot(10.5, 76, ladderZ - 5, true, true, 1, 3, "left_foot_support"),
      foot(-10.5, 36, ladderZ - 5, true, true, 0.86, 1, "right_foot_ready"),
      vec(2.8, -0.8, 6.8),
      vec(-2.2, -0.7, 5.0)
    ),
    key(
      42,
      "right_foot_steps_up",
      "right_foot_reach",
      vec(-0.8, 109, 22).concat(rot(-12, -1, 3)),
      rot(16, 1, -2),
      rot(-5, 0, 0),
      rot(-7, 0, 0),
      [vec(13, 14, 10), vec(17, 14, 9), vec(7, 4, 3)],
      [vec(-12, 11, 9), vec(-15, 12, 8), vec(-6, 3, 3)],
      hand(16, 176, ladderZ, 1, 8, "left_anchor"),
      hand(-16, 156, ladderZ, 1, 7, "right_anchor"),
      foot(10.5, 76, ladderZ - 5, true, true, 1, 3, "left_foot_support"),
      foot(-10.5, 55, ladderZ - 4, false, false, 0.2, 2, "right_foot_lifting"),
      vec(2.8, -0.8, 6.8),
      vec(-6.8, 1.1, 12.0)
    ),
    key(
      47,
      "cycle_set_one_rung_higher",
      "cycle_end_matching_start",
      vec(0, 106, 18).concat(rot(-10, 0, 0)),
      rot(16, 0, 0),
      rot(-5, 0, 0),
      rot(-8, 0, 0),
      [vec(12, 14, 10), vec(16, 16, 10), vec(7, 4, 4)],
      [vec(-12, 8, 11), vec(-14, 10, 10), vec(-6, 3, 4)],
      hand(16, 176, ladderZ, 1, 8, "high_hold"),
      hand(-16, 156, ladderZ, 1, 7, "low_hold"),
      foot(10.5, 76, ladderZ - 5, true, true, 1, 3, "high_step_hold"),
      foot(-10.5, 56, ladderZ - 5, true, true, 1, 2, "low_step_hold"),
      vec(3.6, -1.6, 8.2),
      vec(-2.2, -1.2, 5.5)
    )
  ];

  const keyNames = new Map(keyFrames.map((pose) => [pose.frame, pose.name]));

  function cloneFoot(value) {
    return {
      ankle: value.ankle.slice(),
      ball: value.ball.slice(),
      planted: Boolean(value.planted),
      locked: Boolean(value.locked),
      contactWeight: round(value.contactWeight),
      rungIndex: value.rungIndex,
      role: value.role
    };
  }

  function cloneHand(value) {
    return {
      target: value.target.slice(),
      contactWeight: round(value.contactWeight),
      locked: Boolean(value.locked),
      rungIndex: value.rungIndex,
      role: value.role
    };
  }

  function mixFoot(a, b, t) {
    const contactWeight = round(mix(a.contactWeight, b.contactWeight, t));
    return {
      ankle: a.locked && b.locked && a.rungIndex === b.rungIndex ? a.ankle.slice() : mixVec(a.ankle, b.ankle, t),
      ball: a.locked && b.locked && a.rungIndex === b.rungIndex ? a.ball.slice() : mixVec(a.ball, b.ball, t),
      planted: contactWeight > 0.42,
      locked: Boolean(a.locked && b.locked && a.rungIndex === b.rungIndex),
      contactWeight,
      rungIndex: t < 0.5 ? a.rungIndex : b.rungIndex,
      role: t < 0.5 ? a.role : b.role
    };
  }

  function mixHand(a, b, t) {
    const contactWeight = round(mix(a.contactWeight, b.contactWeight, t));
    return {
      target: a.locked && b.locked && a.rungIndex === b.rungIndex ? a.target.slice() : mixVec(a.target, b.target, t),
      contactWeight,
      locked: Boolean(a.locked && b.locked && a.rungIndex === b.rungIndex),
      rungIndex: t < 0.5 ? a.rungIndex : b.rungIndex,
      role: t < 0.5 ? a.role : b.role
    };
  }

  function footContact(value, side) {
    return {
      type: value.contactWeight > 0.42 ? "ladder_rung" : "air",
      side,
      rungIndex: value.rungIndex,
      weight: value.contactWeight,
      locked: value.locked,
      role: value.role,
      ankle: value.ankle.slice(),
      ball: value.ball.slice()
    };
  }

  function handContact(value, side) {
    return {
      type: value.contactWeight > 0.42 ? "ladder_rung" : "air",
      side,
      rungIndex: value.rungIndex,
      weight: value.contactWeight,
      locked: value.locked,
      role: value.role,
      target: value.target.slice()
    };
  }

  function contactsFor(leftHand, rightHand, leftFoot, rightFoot) {
    const contacts = {
      leftHand: handContact(leftHand, "left"),
      rightHand: handContact(rightHand, "right"),
      leftFoot: footContact(leftFoot, "left"),
      rightFoot: footContact(rightFoot, "right"),
      supportPattern: "alternating_hands_and_feet_on_ladder_rungs"
    };

    if (leftHand.contactWeight > 0.42) {
      contacts.hand_l = leftHand.target.slice();
    }
    if (rightHand.contactWeight > 0.42) {
      contacts.hand_r = rightHand.target.slice();
    }
    if (leftFoot.contactWeight > 0.42) {
      contacts.foot_l = leftFoot.ankle.slice();
      contacts.ball_l = leftFoot.ball.slice();
    }
    if (rightFoot.contactWeight > 0.42) {
      contacts.foot_r = rightFoot.ankle.slice();
      contacts.ball_r = rightFoot.ball.slice();
    }

    return contacts;
  }

  function frameFromPose(pose) {
    const leftHand = cloneHand(pose.leftHand);
    const rightHand = cloneHand(pose.rightHand);
    const leftFoot = cloneFoot(pose.leftFoot);
    const rightFoot = cloneFoot(pose.rightFoot);
    const leftKneeBias = pose.leftKneeBias.slice();
    const rightKneeBias = pose.rightKneeBias.slice();

    return {
      frame: pose.frame,
      name: pose.name,
      key: true,
      phase: pose.phase,
      pelvis: pose.pelvis.slice(),
      chest: pose.chest.slice(),
      neck: pose.neck.slice(),
      head: pose.head.slice(),
      lUpper: pose.lUpper.slice(),
      lLower: pose.lLower.slice(),
      lHand: pose.lHand.slice(),
      rUpper: pose.rUpper.slice(),
      rLower: pose.rLower.slice(),
      rHand: pose.rHand.slice(),
      leftHand,
      rightHand,
      leftHandTarget: leftHand.target.slice(),
      rightHandTarget: rightHand.target.slice(),
      leftFoot,
      rightFoot,
      leftKneeBias,
      rightKneeBias,
      kneeBias: {
        left: leftKneeBias.slice(),
        right: rightKneeBias.slice()
      },
      contacts: contactsFor(leftHand, rightHand, leftFoot, rightFoot)
    };
  }

  function interpolateFrame(frame) {
    const exact = keyFrames.find((pose) => pose.frame === frame);
    if (exact) {
      return frameFromPose(exact);
    }

    let a = keyFrames[0];
    let b = keyFrames[keyFrames.length - 1];
    for (let index = 0; index < keyFrames.length - 1; index += 1) {
      if (frame >= keyFrames[index].frame && frame <= keyFrames[index + 1].frame) {
        a = keyFrames[index];
        b = keyFrames[index + 1];
        break;
      }
    }

    const span = Math.max(1, b.frame - a.frame);
    const t = smoothstep((frame - a.frame) / span);
    const leftHand = mixHand(a.leftHand, b.leftHand, t);
    const rightHand = mixHand(a.rightHand, b.rightHand, t);
    const leftFoot = mixFoot(a.leftFoot, b.leftFoot, t);
    const rightFoot = mixFoot(a.rightFoot, b.rightFoot, t);
    const leftKneeBias = mixVec(a.leftKneeBias, b.leftKneeBias, t);
    const rightKneeBias = mixVec(a.rightKneeBias, b.rightKneeBias, t);

    return {
      frame,
      name: "ladder_climb_inbetween",
      key: false,
      phase: "transition",
      pelvis: mixVec(a.pelvis.slice(0, 3), b.pelvis.slice(0, 3), t).concat(mixVec(a.pelvis.slice(3), b.pelvis.slice(3), t)),
      chest: mixVec(a.chest, b.chest, t),
      neck: mixVec(a.neck, b.neck, t),
      head: mixVec(a.head, b.head, t),
      lUpper: mixVec(a.lUpper, b.lUpper, t),
      lLower: mixVec(a.lLower, b.lLower, t),
      lHand: mixVec(a.lHand, b.lHand, t),
      rUpper: mixVec(a.rUpper, b.rUpper, t),
      rLower: mixVec(a.rLower, b.rLower, t),
      rHand: mixVec(a.rHand, b.rHand, t),
      leftHand,
      rightHand,
      leftHandTarget: leftHand.target.slice(),
      rightHandTarget: rightHand.target.slice(),
      leftFoot,
      rightFoot,
      leftKneeBias,
      rightKneeBias,
      kneeBias: {
        left: leftKneeBias.slice(),
        right: rightKneeBias.slice()
      },
      contacts: contactsFor(leftHand, rightHand, leftFoot, rightFoot)
    };
  }

  const frames = Array.from({ length: frameCount }, (_, frame) => interpolateFrame(frame));
  const keyposes = frames.filter((frame) => keyNames.has(frame.frame));

  const data = {
    id: "ladderClimb",
    name: "Ladder Climb",
    displayName: "Ladder Climb",
    koreanName: "사다리 오르기",
    koreanDisplayName: "사다리 오르기",
    displayNames: {
      en: "Ladder Climb",
      ko: "사다리 오르기"
    },
    frameCount,
    fps,
    durationSeconds,
    loop: true,
    rotationUnits: "degrees",
    positionUnits: "Manny demo units",
    coordinateSystem: {
      up: "Y",
      forward: "Z",
      side: "X",
      groundY: 0
    },
    rootMotion: {
      travelAxis: "Y",
      cycleVerticalTravel,
      wrapMode: "subtract cycleVerticalTravel at the loop boundary"
    },
    style: {
      intent: "A repeatable ladder climb cycle with alternating hand and foot contacts on rungs and one rung of upward body travel.",
      notes: [
        "The final frame matches the opening contact pattern one rung higher so a root-motion aware sampler can loop the climb.",
        "Hands use world-space targets on the ladder plane; feet carry rung contact metadata and contact weights.",
        "Body pitch stays forward toward the ladder while the hips rise through the cycle."
      ]
    },
    ladder: {
      id: "ladder_climb_training_ladder",
      type: "straight_ladder",
      planeZ: ladderZ,
      rungSpacing,
      rungRadius: 1.1,
      rungXMin: -24,
      rungXMax: 24,
      visibleRungs: [
        { index: 1, y: 36 },
        { index: 2, y: 56 },
        { index: 3, y: 76 },
        { index: 4, y: 96 },
        { index: 5, y: 116 },
        { index: 6, y: 136 },
        { index: 7, y: 156 },
        { index: 8, y: 176 }
      ],
      sideRails: {
        leftX: 26,
        rightX: -26,
        bottomY: 20,
        topY: 194,
        z: ladderZ
      }
    },
    environment: {
      prop: "ladder",
      surface: "vertical ladder rungs",
      note: "The ladder is metadata only; integration can render rails and rungs from the ladder block."
    },
    contactNotes: {
      rhythm: "Right hand reaches first, left foot steps, then left hand reaches and right foot steps.",
      feet: "Foot contacts alternate between locked rung holds and low-weight reaching phases.",
      hands: "Hand contacts include rung indices and target points for contact-aware rendering."
    },
    keyposes,
    frames
  };

  data.keys = frames;

  global.mannyLadderClimbKeyposes = data;
})(typeof window !== "undefined" ? window : globalThis);
