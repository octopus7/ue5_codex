(function (global) {
  "use strict";

  const frameCount = 52;
  const fps = 24;
  const durationSeconds = frameCount / fps;

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

  function pelvis(x, y, z, pitch, yaw, roll) {
    return vec(x, y, z).concat(rot(pitch, yaw, roll));
  }

  function lerpVec(a, b, t) {
    return vec(lerp(a[0], b[0], t), lerp(a[1], b[1], t), lerp(a[2], b[2], t));
  }

  function lerpArray(a, b, t) {
    return a.map((value, index) => round(lerp(value, b[index], t)));
  }

  function makeFoot(ankle, ball, planted, role, contactWeight, lockId) {
    return {
      ankle,
      ball,
      planted,
      locked: Boolean(lockId) && planted && contactWeight > 0.88,
      contactWeight: round(contactWeight),
      role,
      lockId
    };
  }

  function blendFoot(a, b, t) {
    const contactWeight = round(lerp(a.contactWeight || 0, b.contactWeight || 0, t));
    const planted = contactWeight > 0.42;
    const locked = Boolean(a.lockId && a.lockId === b.lockId && a.locked && b.locked && planted);
    return {
      ankle: locked ? a.ankle.slice(0, 3) : lerpVec(a.ankle, b.ankle, t),
      ball: locked ? a.ball.slice(0, 3) : lerpVec(a.ball, b.ball, t),
      planted,
      locked,
      contactWeight,
      role: t < 0.5 ? a.role : b.role,
      lockId: a.lockId === b.lockId ? a.lockId : t < 0.5 ? a.lockId : b.lockId
    };
  }

  const keyFrames = [
    {
      frame: 0,
      name: "crouched_landing_drop",
      phase: "entry_crouch",
      pelvis: pelvis(0, 58.0, -12.0, -28, -4, 0),
      chest: rot(42, 4, 0),
      neck: rot(-16, -2, 0),
      head: rot(-26, -4, 0),
      lUpper: vec(13, -16, 18),
      lLower: vec(8, -22, 16),
      lHand: vec(4, -12, 14),
      rUpper: vec(-13, -16, 18),
      rLower: vec(-8, -22, 16),
      rHand: vec(-4, -12, 14),
      leftFoot: makeFoot(vec(10.6, 8.0, -1.0), vec(11.5, 1.0, 13.8), true, "crouch_left", 1, "entry_left"),
      rightFoot: makeFoot(vec(-10.8, 8.0, -2.5), vec(-10.0, 1.0, 12.2), true, "crouch_right", 1, "entry_right"),
      leftKneeBias: vec(5.2, -2.2, 14.8),
      rightKneeBias: vec(-5.2, -2.2, 14.5)
    },
    {
      frame: 6,
      name: "hands_lower_one_shoulder_leads",
      phase: "roll_entry",
      pelvis: pelvis(-1.2, 49.0, -1.0, -42, -18, -16),
      chest: rot(74, -24, -28),
      neck: rot(-28, 10, 4),
      head: rot(-42, 16, 5),
      lUpper: vec(10, -22, 18),
      lLower: vec(6, -26, 20),
      lHand: vec(2, -14, 12),
      rUpper: vec(-14, -20, 15),
      rLower: vec(-8, -24, 16),
      rHand: vec(-4, -13, 11),
      leftHandTarget: vec(12, 2.5, 18),
      rightHandTarget: vec(-12, 5, 20),
      leftFoot: makeFoot(vec(10.6, 8.0, -1.0), vec(11.5, 1.0, 13.8), true, "toe_pivot_entry_left", 0.92, "entry_left"),
      rightFoot: makeFoot(vec(-10.8, 8.0, -2.5), vec(-10.0, 1.0, 12.2), true, "toe_pivot_entry_right", 0.84, "entry_right"),
      leftKneeBias: vec(6.5, -1.0, 13.5),
      rightKneeBias: vec(-6.0, -0.8, 13.0)
    },
    {
      frame: 10,
      name: "left_shoulder_floor_contact",
      phase: "shoulder_contact",
      pelvis: pelvis(-2.2, 43.0, 11.0, -66, -32, -44),
      chest: rot(108, -34, -62),
      neck: rot(-42, 16, 10),
      head: rot(-58, 20, 12),
      lUpper: vec(8, -20, 10),
      lLower: vec(4, -22, 12),
      lHand: vec(1, -10, 8),
      rUpper: vec(-16, -16, 18),
      rLower: vec(-10, -20, 18),
      rHand: vec(-5, -11, 14),
      leftHandTarget: vec(8, 3, 27),
      rightHandTarget: vec(-11, 8, 25),
      leftFoot: makeFoot(vec(9.0, 21.0, 7.0), vec(9.8, 12.0, 20.0), false, "left_leg_lifting", 0.2, null),
      rightFoot: makeFoot(vec(-8.0, 15.0, 6.0), vec(-7.2, 6.0, 19.0), false, "right_toe_release", 0.28, null),
      leftKneeBias: vec(8.0, 4.0, 9.0),
      rightKneeBias: vec(-5.8, 2.5, 10.0)
    },
    {
      frame: 15,
      name: "shoulder_to_upper_back_roll",
      phase: "upper_back_contact",
      pelvis: pelvis(-2.8, 56.0, 25.0, -116, -44, -74),
      chest: rot(144, -42, -72),
      neck: rot(-54, 18, 11),
      head: rot(-68, 23, 12),
      lUpper: vec(10, -16, 4),
      lLower: vec(6, -20, 6),
      lHand: vec(2, -9, 5),
      rUpper: vec(-14, -15, 8),
      rLower: vec(-10, -18, 9),
      rHand: vec(-5, -9, 7),
      leftFoot: makeFoot(vec(6.5, 43.0, 10.0), vec(7.0, 34.0, 22.0), false, "tucked_over_left", 0, null),
      rightFoot: makeFoot(vec(-8.0, 47.0, 13.0), vec(-6.5, 38.0, 25.0), false, "tucked_over_right", 0, null),
      leftKneeBias: vec(7.0, 10.0, 5.0),
      rightKneeBias: vec(-7.0, 11.0, 6.0)
    },
    {
      frame: 21,
      name: "hips_pass_over_compact_inversion",
      phase: "inverted_tuck",
      pelvis: pelvis(-1.0, 78.0, 39.0, -178, -22, -40),
      chest: rot(176, -18, -34),
      neck: rot(-50, 10, 5),
      head: rot(-62, 14, 6),
      lUpper: vec(12, -12, -2),
      lLower: vec(8, -16, -1),
      lHand: vec(3, -8, 1),
      rUpper: vec(-12, -12, 0),
      rLower: vec(-8, -16, 0),
      rHand: vec(-3, -8, 1),
      leftFoot: makeFoot(vec(9.0, 76.0, 31.0), vec(10.0, 68.0, 43.0), false, "inverted_left_tuck", 0, null),
      rightFoot: makeFoot(vec(-10.0, 78.0, 35.0), vec(-9.0, 70.0, 47.0), false, "inverted_right_tuck", 0, null),
      leftKneeBias: vec(8.0, 14.0, 2.0),
      rightKneeBias: vec(-8.0, 14.0, 2.0)
    },
    {
      frame: 27,
      name: "rounded_back_feet_descend",
      phase: "back_to_feet",
      pelvis: pelvis(0.6, 61.0, 55.0, -132, 6, -18),
      chest: rot(124, 6, -14),
      neck: rot(-36, -2, 1),
      head: rot(-48, -4, 1),
      lUpper: vec(13, -14, 6),
      lLower: vec(8, -18, 8),
      lHand: vec(4, -9, 7),
      rUpper: vec(-13, -14, 6),
      rLower: vec(-8, -18, 8),
      rHand: vec(-4, -9, 7),
      leftFoot: makeFoot(vec(8.8, 34.0, 63.0), vec(9.6, 25.0, 76.0), false, "feet_descending_left", 0.1, null),
      rightFoot: makeFoot(vec(-8.8, 35.0, 65.0), vec(-8.0, 26.0, 78.0), false, "feet_descending_right", 0.1, null),
      leftKneeBias: vec(7.0, 7.0, 9.0),
      rightKneeBias: vec(-7.0, 7.0, 9.0)
    },
    {
      frame: 33,
      name: "feet_return_under_body",
      phase: "feet_under_body",
      pelvis: pelvis(0.8, 48.0, 68.0, -62, 4, -5),
      chest: rot(70, 4, -4),
      neck: rot(-22, -2, 0),
      head: rot(-33, -4, 0),
      lUpper: vec(12, -20, 15),
      lLower: vec(7, -24, 17),
      lHand: vec(3, -12, 11),
      rUpper: vec(-12, -20, 15),
      rLower: vec(-7, -24, 17),
      rHand: vec(-3, -12, 11),
      leftHandTarget: vec(12, 4, 78),
      rightHandTarget: vec(-12, 4, 78),
      leftFoot: makeFoot(vec(10.4, 8.0, 67.0), vec(11.4, 1.0, 82.0), true, "left_foot_return", 0.9, "recover_left"),
      rightFoot: makeFoot(vec(-10.6, 8.0, 66.0), vec(-9.6, 1.0, 81.0), true, "right_foot_return", 0.9, "recover_right"),
      leftKneeBias: vec(6.0, -1.8, 14.5),
      rightKneeBias: vec(-6.0, -1.8, 14.5)
    },
    {
      frame: 41,
      name: "crouched_stand_recover",
      phase: "stand_recover",
      pelvis: pelvis(0.4, 64.0, 76.0, -24, 2, 0),
      chest: rot(34, 2, 0),
      neck: rot(-11, -1, 0),
      head: rot(-18, -2, 0),
      lUpper: vec(13, -15, 16),
      lLower: vec(8, -20, 14),
      lHand: vec(4, -10, 13),
      rUpper: vec(-13, -15, 16),
      rLower: vec(-8, -20, 14),
      rHand: vec(-4, -10, 13),
      leftFoot: makeFoot(vec(10.4, 8.0, 67.0), vec(11.4, 1.0, 82.0), true, "left_recover_support", 1, "recover_left"),
      rightFoot: makeFoot(vec(-10.6, 8.0, 66.0), vec(-9.6, 1.0, 81.0), true, "right_recover_support", 1, "recover_right"),
      leftKneeBias: vec(5.2, -2.0, 14.0),
      rightKneeBias: vec(-5.2, -2.0, 14.0)
    },
    {
      frame: 51,
      name: "upright_recovered_stance",
      phase: "recover",
      pelvis: pelvis(0.0, 91.5, 82.0, -3, 0, 0),
      chest: rot(5, 0, 0),
      neck: rot(-1, 0, 0),
      head: rot(-2, 0, 0),
      lUpper: vec(14, -12, 13),
      lLower: vec(9, -18, 10),
      lHand: vec(5, -10, 13),
      rUpper: vec(-14, -12, 13),
      rLower: vec(-9, -18, 10),
      rHand: vec(-5, -10, 13),
      leftFoot: makeFoot(vec(10.4, 8.1, 67.0), vec(11.4, 1.1, 82.0), true, "left_recovered", 1, "recover_left"),
      rightFoot: makeFoot(vec(-10.6, 8.1, 66.0), vec(-9.6, 1.1, 81.0), true, "right_recovered", 1, "recover_right"),
      leftKneeBias: vec(2.8, -1.0, 12.0),
      rightKneeBias: vec(-2.8, -1.0, 12.0)
    }
  ];

  const keyNames = new Map(keyFrames.map((keyFrame) => [keyFrame.frame, keyFrame.name]));

  function spanForFrame(frame) {
    for (let index = 0; index < keyFrames.length - 1; index += 1) {
      if (frame >= keyFrames[index].frame && frame <= keyFrames[index + 1].frame) {
        return [keyFrames[index], keyFrames[index + 1]];
      }
    }
    return [keyFrames[keyFrames.length - 1], keyFrames[keyFrames.length - 1]];
  }

  function handTargetFor(key, side) {
    if (side === "left") {
      return key.leftHandTarget || null;
    }
    return key.rightHandTarget || null;
  }

  function blendOptionalTarget(a, b, side, t) {
    const from = handTargetFor(a, side);
    const to = handTargetFor(b, side);
    if (from && to) {
      return lerpVec(from, to, t);
    }
    return (t < 0.5 ? from : to) || undefined;
  }

  function contactForFoot(foot) {
    if (!foot.planted) {
      return "air";
    }
    if (foot.role && foot.role.indexOf("return") !== -1) {
      return "ground_replant";
    }
    if (foot.role && foot.role.indexOf("pivot") !== -1) {
      return "toe_pivot";
    }
    return foot.locked ? "ground_locked" : "ground";
  }

  function floorContactPhase(frame) {
    if (frame >= 9 && frame <= 13) {
      return {
        leftShoulder: "floor_primary_contact",
        upperBack: "approaching_floor",
        back: "off_floor"
      };
    }
    if (frame >= 14 && frame <= 27) {
      return {
        leftShoulder: frame <= 17 ? "rolling_off_floor" : "off_floor",
        upperBack: "floor_roll_contact",
        back: "floor_roll_contact"
      };
    }
    return {
      leftShoulder: "off_floor",
      upperBack: "off_floor",
      back: "off_floor"
    };
  }

  function makeFrame(frame) {
    const [a, b] = spanForFrame(frame);
    const span = Math.max(1, b.frame - a.frame);
    const t = smoothstep((frame - a.frame) / span);
    const leftFoot = blendFoot(a.leftFoot, b.leftFoot, t);
    const rightFoot = blendFoot(a.rightFoot, b.rightFoot, t);
    const leftKneeBias = lerpVec(a.leftKneeBias, b.leftKneeBias, t);
    const rightKneeBias = lerpVec(a.rightKneeBias, b.rightKneeBias, t);
    const leftHandTarget = blendOptionalTarget(a, b, "left", t);
    const rightHandTarget = blendOptionalTarget(a, b, "right", t);
    const floorContacts = floorContactPhase(frame);

    const sampled = {
      frame,
      name: keyNames.get(frame) || "forward_roll_inbetween",
      key: keyNames.has(frame),
      phase: frame === a.frame ? a.phase : frame === b.frame ? b.phase : "transition",
      pelvis: lerpArray(a.pelvis, b.pelvis, t),
      chest: lerpArray(a.chest, b.chest, t),
      neck: lerpArray(a.neck, b.neck, t),
      head: lerpArray(a.head, b.head, t),
      lUpper: lerpVec(a.lUpper, b.lUpper, t),
      lLower: lerpVec(a.lLower, b.lLower, t),
      lHand: lerpVec(a.lHand, b.lHand, t),
      rUpper: lerpVec(a.rUpper, b.rUpper, t),
      rLower: lerpVec(a.rLower, b.rLower, t),
      rHand: lerpVec(a.rHand, b.rHand, t),
      leftFoot,
      rightFoot,
      leftKneeBias,
      rightKneeBias,
      kneeBias: {
        left: leftKneeBias.slice(0, 3),
        right: rightKneeBias.slice(0, 3)
      },
      contacts: {
        leftFoot: contactForFoot(leftFoot),
        rightFoot: contactForFoot(rightFoot),
        leftHand: frame >= 5 && frame <= 11 ? "floor_guiding" : frame >= 31 && frame <= 35 ? "floor_assist" : "free",
        rightHand: frame >= 5 && frame <= 11 ? "floor_guiding" : frame >= 31 && frame <= 35 ? "floor_assist" : "free",
        leftShoulder: floorContacts.leftShoulder,
        upperBack: floorContacts.upperBack,
        back: floorContacts.back,
        floor: frame >= 9 && frame <= 27 ? "shoulder_back_roll_path" : "feet_or_hands_only"
      },
      floorContact: {
        leftShoulder: floorContacts.leftShoulder,
        upperBack: floorContacts.upperBack,
        back: floorContacts.back,
        note: "Roll travels diagonally across the left shoulder and upper back before the feet return under the body."
      }
    };

    if (leftHandTarget) {
      sampled.leftHandTarget = leftHandTarget;
    }
    if (rightHandTarget) {
      sampled.rightHandTarget = rightHandTarget;
    }

    return sampled;
  }

  const frames = Array.from({ length: frameCount }, (_, frame) => makeFrame(frame));

  const data = {
    id: "forwardRoll",
    name: "Forward Roll",
    koreanName: "앞구르기 낙법",
    displayNames: {
      en: "Forward Roll",
      ko: "앞구르기 낙법"
    },
    frameCount,
    fps,
    durationSeconds: round(durationSeconds),
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
      intent: "A non-looping forward roll from a crouched landing: Manny drops, rolls over one shoulder with a curled spine, lets the hips pass over, plants both feet under the body, then stands and recovers.",
      notes: [
        "The entry favors the left shoulder so the head avoids a direct floor impact.",
        "Pelvis and chest pitch through an inverted tuck while the feet leave the floor and travel over the body.",
        "The last third of the motion replants both feet under the pelvis before standing upright."
      ]
    },
    environment: {
      ground: "flat padded floor or mat",
      props: "none"
    },
    contactNotes: {
      feet: "Both feet are planted in the crouched entry, leave the floor during the roll, then replant under the body at frame 33.",
      shoulder: "The left shoulder is the first rolling contact at frame 10.",
      back: "Upper-back and back contact carry the roll through frames 14-27 before the feet return under the body.",
      loop: "The motion is non-looping and ends in a recovered stance."
    },
    keyposes: frames.filter((frame) => frame.key),
    frames
  };

  data.keys = frames;

  global.mannyForwardRollKeyposes = data;
})(typeof window !== "undefined" ? window : globalThis);
