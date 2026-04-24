(function (global) {
  "use strict";

  const frameCount = 60;
  const fps = 24;
  const durationSeconds = frameCount / fps;

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

  function foot(ankleX, ankleY, ankleZ, ballX, ballY, ballZ, planted, locked, contactWeight, role, sliding) {
    return {
      ankle: vec(ankleX, ankleY, ankleZ),
      ball: vec(ballX, ballY, ballZ),
      planted: Boolean(planted),
      locked: Boolean(locked),
      contactWeight: round(contactWeight),
      role,
      sliding: Boolean(sliding)
    };
  }

  function knee(x, y, z, contactWeight, role, sliding) {
    return {
      point: vec(x, y, z),
      contactWeight: round(contactWeight),
      planted: contactWeight > 0.42,
      role,
      sliding: Boolean(sliding)
    };
  }

  function handContact(x, y, z, contactWeight, role) {
    return {
      point: vec(x, y, z),
      contactWeight: round(contactWeight),
      planted: contactWeight > 0.42,
      role
    };
  }

  function key(frame, name, phase, pelvis, chest, neck, head, leftArm, rightArm, leftFoot, rightFoot, leftKneeBias, rightKneeBias, leftKnee, rightKnee, leftHandContact, rightHandContact, slide) {
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
      leftFoot,
      rightFoot,
      leftKneeBias,
      rightKneeBias,
      leftKnee,
      rightKnee,
      leftHandContact,
      rightHandContact,
      slide
    };
  }

  const keyFrames = [
    key(
      0,
      "run_in_left_stride",
      "run_in",
      vec(0, 95, -58).concat(rot(-3, 0, 0)),
      rot(8, -6, 2),
      rot(-2, 2, 0),
      rot(-3, 3, 0),
      [vec(16, -11, -9), vec(11, -19, -4), vec(5, -12, 2)],
      [vec(-18, -7, 12), vec(-13, -16, 9), vec(-6, -12, 12)],
      foot(10.8, 8.2, -45, 12.0, 1.1, -30, true, true, 1, "run_front_plant", false),
      foot(-10.5, 17.0, -70, -11.6, 8.5, -55, false, false, 0.08, "run_rear_swing", false),
      vec(1.8, -0.2, 3.0),
      vec(-4.0, 2.0, 8.0),
      knee(9.5, 33, -39, 0, "air", false),
      knee(-9.5, 42, -61, 0, "air", false),
      handContact(22, 68, -42, 0, "air"),
      handContact(-20, 76, -52, 0, "air"),
      { velocityZ: 24, friction: 0, floorContact: false }
    ),
    key(
      6,
      "load_stride_before_drop",
      "load",
      vec(0.8, 91, -44).concat(rot(-8, 2, -2)),
      rot(12, -2, 1),
      rot(-3, 1, 0),
      rot(-4, 1, 0),
      [vec(18, -8, 0), vec(14, -17, 2), vec(6, -12, 5)],
      [vec(-15, -12, -8), vec(-10, -20, -5), vec(-5, -12, 0)],
      foot(9.4, 10.5, -55, 10.6, 2.8, -40, false, false, 0.25, "left_unweighting", false),
      foot(-11.5, 8.1, -33, -12.5, 1.1, -18, true, true, 1, "right_brake_plant", false),
      vec(3.6, 1.4, 7.8),
      vec(-2.0, -0.4, 3.8),
      knee(9.0, 28, -47, 0.05, "descending", false),
      knee(-10.0, 30, -28, 0.05, "descending", false),
      handContact(24, 70, -30, 0, "air"),
      handContact(-20, 68, -40, 0, "air"),
      { velocityZ: 29, friction: 0.05, floorContact: false }
    ),
    key(
      12,
      "drop_commitment",
      "drop",
      vec(0.4, 76, -29).concat(rot(-18, 1, 0)),
      rot(7, 1, 0),
      rot(-2, 0, 0),
      rot(-4, 0, 0),
      [vec(24, -6, 9), vec(18, -15, 11), vec(9, -11, 8)],
      [vec(-24, -6, 9), vec(-18, -15, 11), vec(-9, -11, 8)],
      foot(10.5, 8.5, -28, 11.6, 1.1, -13, true, false, 0.55, "left_toe_drag_start", true),
      foot(-10.5, 8.0, -31, -11.6, 1.1, -16, true, false, 0.62, "right_toe_drag_start", true),
      vec(4.8, -8.0, 13.0),
      vec(-4.8, -8.0, 13.0),
      knee(9.8, 6.0, -13, 0.32, "near_floor", false),
      knee(-9.8, 5.8, -13, 0.32, "near_floor", false),
      handContact(26, 56, -6, 0, "arms_out"),
      handContact(-26, 56, -6, 0, "arms_out"),
      { velocityZ: 34, friction: 0.18, floorContact: false }
    ),
    key(
      18,
      "double_knee_impact",
      "knee_impact",
      vec(0.0, 54, -12).concat(rot(-23, 0, 0)),
      rot(-20, 0, 0),
      rot(6, 0, 0),
      rot(10, 0, 0),
      [vec(30, -3, 6), vec(24, -11, 8), vec(14, -11, 5)],
      [vec(-30, -3, 6), vec(-24, -11, 8), vec(-14, -11, 5)],
      foot(10.4, 7.0, -29, 11.5, 1.2, -14, false, false, 0.2, "left_laces_drag", true),
      foot(-10.4, 7.0, -29, -11.5, 1.2, -14, false, false, 0.2, "right_laces_drag", true),
      vec(4.4, -18.0, 22.0),
      vec(-4.4, -18.0, 22.0),
      knee(9.8, 2.2, -1.5, 1, "left_knee_floor_impact", true),
      knee(-9.8, 2.2, -1.5, 1, "right_knee_floor_impact", true),
      handContact(31, 42, 3, 0, "arms_out_balance"),
      handContact(-31, 42, 3, 0, "arms_out_balance"),
      { velocityZ: 32, friction: 0.42, floorContact: true }
    ),
    key(
      24,
      "fast_slide_torso_back",
      "fast_slide",
      vec(0.0, 47, 10).concat(rot(-27, 0, 0)),
      rot(-30, 0, 0),
      rot(8, 0, 0),
      rot(12, 0, 0),
      [vec(33, 0, 4), vec(27, -8, 5), vec(16, -10, 3)],
      [vec(-33, 0, 4), vec(-27, -8, 5), vec(-16, -10, 3)],
      foot(10.0, 6.8, -18, 11.2, 1.2, -3, false, false, 0.16, "left_laces_sliding", true),
      foot(-10.0, 6.8, -18, -11.2, 1.2, -3, false, false, 0.16, "right_laces_sliding", true),
      vec(4.0, -18.5, 24.0),
      vec(-4.0, -18.5, 24.0),
      knee(9.6, 1.8, 18, 1, "left_knee_slide", true),
      knee(-9.6, 1.8, 18, 1, "right_knee_slide", true),
      handContact(34, 34, 28, 0, "wide_balance"),
      handContact(-34, 34, 28, 0, "wide_balance"),
      { velocityZ: 27, friction: 0.58, floorContact: true }
    ),
    key(
      34,
      "friction_slows_slide",
      "slow_slide",
      vec(0.0, 46, 34).concat(rot(-22, 0, 0)),
      rot(-22, 0, 0),
      rot(6, 0, 0),
      rot(9, 0, 0),
      [vec(31, -4, 6), vec(23, -13, 10), vec(12, -13, 8)],
      [vec(-31, -4, 6), vec(-23, -13, 10), vec(-12, -13, 8)],
      foot(9.6, 6.8, 1, 10.8, 1.2, 16, false, false, 0.12, "left_laces_dragging", true),
      foot(-9.6, 6.8, 1, -10.8, 1.2, 16, false, false, 0.12, "right_laces_dragging", true),
      vec(3.5, -17.0, 22.0),
      vec(-3.5, -17.0, 22.0),
      knee(9.5, 1.8, 42, 0.9, "left_knee_slide_slowing", true),
      knee(-9.5, 1.8, 42, 0.9, "right_knee_slide_slowing", true),
      handContact(32, 22, 54, 0.1, "hands_lowering"),
      handContact(-32, 22, 54, 0.1, "hands_lowering"),
      { velocityZ: 15, friction: 0.76, floorContact: true }
    ),
    key(
      42,
      "left_hand_plants_right_foot_threads",
      "brace_to_rise",
      vec(1.5, 51, 52).concat(rot(-12, -8, 4)),
      rot(-8, -12, 5),
      rot(2, 4, -1),
      rot(3, 7, -1),
      [vec(20, -30, 18), vec(12, -33, 15), vec(5, -24, 7)],
      [vec(-24, -12, 8), vec(-18, -19, 8), vec(-8, -13, 5)],
      foot(9.8, 7.0, 18, 11.0, 1.2, 33, false, false, 0.16, "left_laces_settling", true),
      foot(-10.5, 8.1, 61, -11.6, 1.1, 76, true, true, 0.78, "right_foot_plant_to_rise", false),
      vec(2.5, -14.0, 17.0),
      vec(-2.8, -5.0, 9.0),
      knee(9.3, 2.0, 55, 0.7, "left_knee_last_slide_contact", false),
      knee(-9.0, 8.0, 49, 0.25, "right_knee_lifting", false),
      handContact(23, 1.4, 66, 1, "left_hand_floor_plant"),
      handContact(-28, 24, 58, 0, "right_hand_counterbalance"),
      { velocityZ: 6, friction: 0.9, floorContact: true }
    ),
    key(
      50,
      "push_up_from_slide",
      "rising",
      vec(3.0, 72, 64).concat(rot(-5, -5, 2)),
      rot(8, -5, 1),
      rot(-2, 2, 0),
      rot(-3, 3, 0),
      [vec(16, -18, 10), vec(11, -24, 12), vec(5, -16, 8)],
      [vec(-18, -10, -2), vec(-13, -19, 1), vec(-6, -12, 5)],
      foot(10.0, 8.0, 48, 11.1, 1.1, 63, true, false, 0.58, "left_foot_tucking_under", false),
      foot(-10.6, 8.1, 68, -11.7, 1.1, 83, true, true, 1, "right_foot_drive_plant", false),
      vec(2.8, -3.0, 8.0),
      vec(-2.2, -1.0, 5.0),
      knee(9.5, 12, 58, 0.18, "left_knee_lifting", false),
      knee(-9.5, 26, 63, 0, "right_knee_air", false),
      handContact(23, 4, 66, 0.55, "left_hand_push_off"),
      handContact(-20, 58, 64, 0, "right_hand_free"),
      { velocityZ: 3, friction: 0.4, floorContact: false }
    ),
    key(
      59,
      "recovered_standing",
      "recover",
      vec(0.0, 93, 78).concat(rot(-2, 0, 0)),
      rot(5, 0, 0),
      rot(-1, 0, 0),
      rot(-2, 0, 0),
      [vec(15, -12, 9), vec(10, -19, 8), vec(5, -12, 9)],
      [vec(-15, -12, 9), vec(-10, -19, 8), vec(-5, -12, 9)],
      foot(10.5, 8.1, 68, 11.6, 1.1, 83, true, true, 1, "left_recovery_plant", false),
      foot(-10.5, 8.1, 88, -11.6, 1.1, 103, true, true, 1, "right_recovery_plant", false),
      vec(2.0, 0.0, 2.8),
      vec(-2.0, 0.0, 2.8),
      knee(9.8, 42, 74, 0, "air", false),
      knee(-9.8, 42, 94, 0, "air", false),
      handContact(18, 72, 82, 0, "air"),
      handContact(-18, 72, 82, 0, "air"),
      { velocityZ: 0, friction: 0, floorContact: false }
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
      role: value.role,
      sliding: Boolean(value.sliding)
    };
  }

  function cloneContact(value) {
    return {
      point: value.point.slice(),
      contactWeight: round(value.contactWeight),
      planted: Boolean(value.planted),
      role: value.role,
      sliding: Boolean(value.sliding)
    };
  }

  function cloneSlide(value) {
    return {
      velocityZ: round(value.velocityZ),
      friction: round(value.friction),
      floorContact: Boolean(value.floorContact)
    };
  }

  function mixFoot(a, b, t) {
    const contactWeight = round(mix(a.contactWeight, b.contactWeight, t));
    const locked = Boolean(a.locked && b.locked);
    return {
      ankle: locked ? a.ankle.slice() : mixVec(a.ankle, b.ankle, t),
      ball: locked ? a.ball.slice() : mixVec(a.ball, b.ball, t),
      planted: contactWeight > 0.42 || (t < 0.5 ? a.planted : b.planted),
      locked,
      contactWeight,
      role: t < 0.5 ? a.role : b.role,
      sliding: t < 0.5 ? a.sliding : b.sliding
    };
  }

  function mixContact(a, b, t) {
    const contactWeight = round(mix(a.contactWeight, b.contactWeight, t));
    return {
      point: mixVec(a.point, b.point, t),
      contactWeight,
      planted: contactWeight > 0.42,
      role: t < 0.5 ? a.role : b.role,
      sliding: t < 0.5 ? a.sliding : b.sliding
    };
  }

  function mixSlide(a, b, t) {
    const friction = round(mix(a.friction, b.friction, t));
    return {
      velocityZ: round(mix(a.velocityZ, b.velocityZ, t)),
      friction,
      floorContact: friction > 0.28 || (t < 0.5 ? a.floorContact : b.floorContact)
    };
  }

  function footContact(value, side) {
    return {
      type: value.sliding ? "floor_drag" : value.planted ? "floor_plant" : "air",
      side,
      weight: value.contactWeight,
      locked: value.locked,
      sliding: value.sliding,
      role: value.role,
      ankle: value.ankle.slice(),
      ball: value.ball.slice()
    };
  }

  function bodyContact(value, side, part) {
    return {
      type: value.contactWeight > 0.42 ? "floor" : "air",
      side,
      part,
      weight: value.contactWeight,
      sliding: value.sliding,
      role: value.role,
      point: value.point.slice()
    };
  }

  function contactsFor(leftFoot, rightFoot, leftKnee, rightKnee, leftHand, rightHand) {
    const contacts = {
      leftFoot: footContact(leftFoot, "left"),
      rightFoot: footContact(rightFoot, "right"),
      leftKnee: bodyContact(leftKnee, "left", "knee"),
      rightKnee: bodyContact(rightKnee, "right", "knee"),
      leftHand: bodyContact(leftHand, "left", "hand"),
      rightHand: bodyContact(rightHand, "right", "hand"),
      floorContact: leftKnee.contactWeight > 0.42 || rightKnee.contactWeight > 0.42,
      primaryContact: leftKnee.contactWeight + rightKnee.contactWeight > 1.2 ? "both_knees" : "feet_or_hand"
    };

    if (leftKnee.contactWeight > 0.42) {
      contacts.knee_l = leftKnee.point.slice();
    }
    if (rightKnee.contactWeight > 0.42) {
      contacts.knee_r = rightKnee.point.slice();
    }
    if (leftHand.contactWeight > 0.42) {
      contacts.hand_l = leftHand.point.slice();
    }
    if (rightHand.contactWeight > 0.42) {
      contacts.hand_r = rightHand.point.slice();
    }

    return contacts;
  }

  function frameFromPose(pose) {
    const leftFoot = cloneFoot(pose.leftFoot);
    const rightFoot = cloneFoot(pose.rightFoot);
    const leftKnee = cloneContact(pose.leftKnee);
    const rightKnee = cloneContact(pose.rightKnee);
    const leftHand = cloneContact(pose.leftHandContact);
    const rightHand = cloneContact(pose.rightHandContact);
    const leftKneeBias = pose.leftKneeBias.slice();
    const rightKneeBias = pose.rightKneeBias.slice();
    const slide = cloneSlide(pose.slide);

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
      leftFoot,
      rightFoot,
      leftKnee,
      rightKnee,
      leftHandContact: leftHand,
      rightHandContact: rightHand,
      leftKneeBias,
      rightKneeBias,
      kneeBias: {
        left: leftKneeBias.slice(),
        right: rightKneeBias.slice()
      },
      slide,
      contacts: contactsFor(leftFoot, rightFoot, leftKnee, rightKnee, leftHand, rightHand)
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
    const leftFoot = mixFoot(a.leftFoot, b.leftFoot, t);
    const rightFoot = mixFoot(a.rightFoot, b.rightFoot, t);
    const leftKnee = mixContact(a.leftKnee, b.leftKnee, t);
    const rightKnee = mixContact(a.rightKnee, b.rightKnee, t);
    const leftHand = mixContact(a.leftHandContact, b.leftHandContact, t);
    const rightHand = mixContact(a.rightHandContact, b.rightHandContact, t);
    const leftKneeBias = mixVec(a.leftKneeBias, b.leftKneeBias, t);
    const rightKneeBias = mixVec(a.rightKneeBias, b.rightKneeBias, t);
    const slide = mixSlide(a.slide, b.slide, t);

    return {
      frame,
      name: "knee_slide_inbetween",
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
      leftFoot,
      rightFoot,
      leftKnee,
      rightKnee,
      leftHandContact: leftHand,
      rightHandContact: rightHand,
      leftKneeBias,
      rightKneeBias,
      kneeBias: {
        left: leftKneeBias.slice(),
        right: rightKneeBias.slice()
      },
      slide,
      contacts: contactsFor(leftFoot, rightFoot, leftKnee, rightKnee, leftHand, rightHand)
    };
  }

  const frames = Array.from({ length: frameCount }, (_, frame) => interpolateFrame(frame));
  const keyposes = frames.filter((frame) => keyNames.has(frame.frame));

  const data = {
    id: "kneeSlide",
    name: "Knee Slide",
    displayName: "Knee Slide",
    koreanName: "무릎 슬라이드",
    koreanDisplayName: "무릎 슬라이드",
    displayNames: {
      en: "Knee Slide",
      ko: "무릎 슬라이드"
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
      intent: "A forward knee slide that starts from a run-in, drops onto both knees, slides forward with a leaned-back torso, slows under friction, braces, and rises.",
      notes: [
        "Knee contact is explicit from impact through the slow slide.",
        "Feet transition from running contacts to low-weight lace drags, then one foot plants for recovery.",
        "The slide metadata exposes forward velocity and friction so an integrator can tune timing or effects."
      ]
    },
    environment: {
      surface: "smooth floor",
      floorY: 0,
      contactMaterial: "medium_friction_stage_floor",
      note: "No prop is required; knee and hand contact points are supplied as metadata."
    },
    contactNotes: {
      knees: "Both knees hit the floor around frame 18 and remain the dominant sliding contact through frame 34.",
      hands: "The left hand plants around frame 42 to help stop and push up.",
      recovery: "The right foot plants first, then both feet settle into a standing recovery."
    },
    keyposes,
    frames
  };

  data.keys = frames;

  global.mannyKneeSlideKeyposes = data;
})(typeof window !== "undefined" ? window : globalThis);
