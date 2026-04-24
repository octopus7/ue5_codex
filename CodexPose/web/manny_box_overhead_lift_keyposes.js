(function (global) {
  "use strict";

  const frameCount = 48;
  const fps = 24;
  const durationSeconds = frameCount / fps;
  const boxSize = [38, 22, 28];
  const halfGripWidth = boxSize[0] * 0.5;

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

  function sampleTrack(track, frame) {
    let previous = track[0];
    let next = track[track.length - 1];

    for (let index = 0; index < track.length - 1; index += 1) {
      if (frame >= track[index].frame && frame <= track[index + 1].frame) {
        previous = track[index];
        next = track[index + 1];
        break;
      }
    }

    const span = Math.max(1, next.frame - previous.frame);
    const t = smoothstep((frame - previous.frame) / span);

    return {
      name: previous.name === next.name || frame === previous.frame ? previous.name : "box_overhead_lift_inbetween",
      phase: previous.phase === next.phase || frame === previous.frame ? previous.phase : "transition",
      boxY: lerp(previous.boxY, next.boxY, t),
      boxZ: lerp(previous.boxZ, next.boxZ, t),
      handY: lerp(previous.handY, next.handY, t),
      handZ: lerp(previous.handZ, next.handZ, t),
      handWidth: lerp(previous.handWidth, next.handWidth, t),
      pelvisY: lerp(previous.pelvisY, next.pelvisY, t),
      pelvisZ: lerp(previous.pelvisZ, next.pelvisZ, t),
      pelvisPitch: lerp(previous.pelvisPitch, next.pelvisPitch, t),
      pelvisYaw: lerp(previous.pelvisYaw, next.pelvisYaw, t),
      pelvisRoll: lerp(previous.pelvisRoll, next.pelvisRoll, t),
      chestPitch: lerp(previous.chestPitch, next.chestPitch, t),
      chestYaw: lerp(previous.chestYaw, next.chestYaw, t),
      chestRoll: lerp(previous.chestRoll, next.chestRoll, t),
      headPitch: lerp(previous.headPitch, next.headPitch, t),
      elbowY: lerp(previous.elbowY, next.elbowY, t),
      elbowZ: lerp(previous.elbowZ, next.elbowZ, t),
      elbowOut: lerp(previous.elbowOut, next.elbowOut, t),
      kneeBend: lerp(previous.kneeBend, next.kneeBend, t),
      weightShift: lerp(previous.weightShift, next.weightShift, t),
      brace: lerp(previous.brace, next.brace, t),
      grip: frame >= previous.frame ? previous.grip : next.grip
    };
  }

  const liftKeys = [
    {
      frame: 0,
      name: "stand_over_floor_box",
      phase: "approach",
      boxY: 11,
      boxZ: 33,
      handY: 71,
      handZ: 18,
      handWidth: 24,
      pelvisY: 93,
      pelvisZ: 2,
      pelvisPitch: -3,
      pelvisYaw: 0,
      pelvisRoll: 0,
      chestPitch: 5,
      chestYaw: 0,
      chestRoll: 0,
      headPitch: -4,
      elbowY: 76,
      elbowZ: 20,
      elbowOut: 21,
      kneeBend: 8,
      weightShift: -0.1,
      brace: 0.2,
      grip: "box_on_floor_ungripped"
    },
    {
      frame: 7,
      name: "hip_hinge_reach_down",
      phase: "reach",
      boxY: 11,
      boxZ: 33,
      handY: 26,
      handZ: 31,
      handWidth: 34,
      pelvisY: 78,
      pelvisZ: -3,
      pelvisPitch: -18,
      pelvisYaw: -1,
      pelvisRoll: 1,
      chestPitch: 37,
      chestYaw: 1,
      chestRoll: -1,
      headPitch: -24,
      elbowY: 44,
      elbowZ: 25,
      elbowOut: 24,
      kneeBend: 34,
      weightShift: -0.55,
      brace: 0.45,
      grip: "reaching_for_side_grips"
    },
    {
      frame: 12,
      name: "deep_squat_side_grip",
      phase: "grip",
      boxY: 11,
      boxZ: 33,
      handY: 13,
      handZ: 33,
      handWidth: 38,
      pelvisY: 67,
      pelvisZ: -6,
      pelvisPitch: -24,
      pelvisYaw: 0,
      pelvisRoll: 0,
      chestPitch: 50,
      chestYaw: 0,
      chestRoll: 0,
      headPitch: -30,
      elbowY: 30,
      elbowZ: 29,
      elbowOut: 27,
      kneeBend: 58,
      weightShift: -0.75,
      brace: 0.7,
      grip: "both_hands_on_side_edges_floor"
    },
    {
      frame: 17,
      name: "brace_and_break_from_floor",
      phase: "floor_pull",
      boxY: 18,
      boxZ: 31,
      handY: 18,
      handZ: 31,
      handWidth: 38,
      pelvisY: 69,
      pelvisZ: -5,
      pelvisPitch: -21,
      pelvisYaw: 1,
      pelvisRoll: -0.5,
      chestPitch: 43,
      chestYaw: -1,
      chestRoll: 0.5,
      headPitch: -25,
      elbowY: 34,
      elbowZ: 28,
      elbowOut: 29,
      kneeBend: 56,
      weightShift: -0.62,
      brace: 0.95,
      grip: "both_hands_on_side_edges_lifting"
    },
    {
      frame: 23,
      name: "box_to_knees_close_path",
      phase: "knee_height",
      boxY: 43,
      boxZ: 25,
      handY: 43,
      handZ: 25,
      handWidth: 38,
      pelvisY: 76,
      pelvisZ: -2,
      pelvisPitch: -15,
      pelvisYaw: -1,
      pelvisRoll: 0.5,
      chestPitch: 27,
      chestYaw: 1,
      chestRoll: -0.5,
      headPitch: -17,
      elbowY: 55,
      elbowZ: 22,
      elbowOut: 30,
      kneeBend: 42,
      weightShift: -0.35,
      brace: 1,
      grip: "both_hands_on_side_edges_lifting"
    },
    {
      frame: 29,
      name: "lap_to_chest_catch",
      phase: "chest_catch",
      boxY: 78,
      boxZ: 17,
      handY: 78,
      handZ: 17,
      handWidth: 38,
      pelvisY: 84,
      pelvisZ: 0,
      pelvisPitch: -8,
      pelvisYaw: 1,
      pelvisRoll: -0.5,
      chestPitch: 12,
      chestYaw: -1,
      chestRoll: 0.5,
      headPitch: -11,
      elbowY: 87,
      elbowZ: 12,
      elbowOut: 33,
      kneeBend: 27,
      weightShift: -0.15,
      brace: 1,
      grip: "box_against_chest_side_grips"
    },
    {
      frame: 35,
      name: "leg_drive_and_press",
      phase: "press",
      boxY: 118,
      boxZ: 12,
      handY: 118,
      handZ: 12,
      handWidth: 38,
      pelvisY: 92,
      pelvisZ: 1,
      pelvisPitch: 1,
      pelvisYaw: -1,
      pelvisRoll: 0.5,
      chestPitch: -5,
      chestYaw: 1,
      chestRoll: -0.5,
      headPitch: -17,
      elbowY: 116,
      elbowZ: 8,
      elbowOut: 28,
      kneeBend: 12,
      weightShift: 0.1,
      brace: 0.95,
      grip: "pressing_side_edges_over_face"
    },
    {
      frame: 41,
      name: "overhead_lockout_hold",
      phase: "lockout",
      boxY: 151,
      boxZ: 10,
      handY: 151,
      handZ: 10,
      handWidth: 38,
      pelvisY: 94,
      pelvisZ: 1.5,
      pelvisPitch: 2,
      pelvisYaw: 0,
      pelvisRoll: 0,
      chestPitch: -10,
      chestYaw: 0,
      chestRoll: 0,
      headPitch: -18,
      elbowY: 139,
      elbowZ: 8,
      elbowOut: 21,
      kneeBend: 6,
      weightShift: 0,
      brace: 0.9,
      grip: "overhead_side_edge_lockout"
    },
    {
      frame: 47,
      name: "small_overhead_settle",
      phase: "settle",
      boxY: 148,
      boxZ: 11,
      handY: 148,
      handZ: 11,
      handWidth: 38,
      pelvisY: 93,
      pelvisZ: 1,
      pelvisPitch: 0,
      pelvisYaw: 0,
      pelvisRoll: 0,
      chestPitch: -7,
      chestYaw: 0,
      chestRoll: 0,
      headPitch: -15,
      elbowY: 136,
      elbowZ: 9,
      elbowOut: 22,
      kneeBend: 9,
      weightShift: 0,
      brace: 0.8,
      grip: "overhead_side_edge_controlled_settle"
    }
  ];

  const keyNames = new Map(liftKeys.map((key) => [key.frame, key.name]));

  function makeFoot(side, pose) {
    const baseX = side * 11.2;
    const shiftX = pose.weightShift * 0.35;
    const toeZ = 13.8;
    return {
      ankle: vec(baseX + shiftX, 8.1, -0.5),
      ball: vec(baseX + side * 0.9 + shiftX, 1.1, toeZ),
      planted: true,
      locked: true,
      contactWeight: 1,
      stance: "shoulder_width_planted_no_slide"
    };
  }

  function makeFrame(frame) {
    const pose = sampleTrack(liftKeys, frame);
    const key = keyNames.has(frame);
    const handHalfWidth = pose.handWidth * 0.5;
    const leftHand = vec(handHalfWidth, pose.handY, pose.handZ);
    const rightHand = vec(-handHalfWidth, pose.handY, pose.handZ);
    const leftUpper = vec(pose.elbowOut, pose.elbowY, pose.elbowZ);
    const rightUpper = vec(-pose.elbowOut, pose.elbowY, pose.elbowZ);
    const leftLower = vec(lerp(pose.elbowOut, handHalfWidth, 0.58), lerp(pose.elbowY, pose.handY, 0.58), lerp(pose.elbowZ, pose.handZ, 0.58));
    const rightLower = vec(lerp(-pose.elbowOut, -handHalfWidth, 0.58), lerp(pose.elbowY, pose.handY, 0.58), lerp(pose.elbowZ, pose.handZ, 0.58));
    const kneeForward = 2.2 + pose.kneeBend * 0.095;
    const kneeSide = 1.1 + pose.kneeBend * 0.035;
    const boxOnFloor = pose.boxY <= boxSize[1] * 0.5 + 0.05;

    return {
      frame,
      name: keyNames.get(frame) || pose.name,
      key,
      phase: pose.phase,
      pelvis: vec(pose.weightShift, pose.pelvisY, pose.pelvisZ).concat([
        round(pose.pelvisPitch),
        round(pose.pelvisYaw),
        round(pose.pelvisRoll)
      ]),
      chest: [round(pose.chestPitch), round(pose.chestYaw), round(pose.chestRoll)],
      neck: [round(pose.headPitch * 0.35), round(-pose.chestYaw * 0.25), round(-pose.chestRoll * 0.2)],
      head: [round(pose.headPitch), round(-pose.chestYaw * 0.2), round(-pose.chestRoll * 0.15)],
      lUpper: leftUpper,
      lLower: leftLower,
      lHand: leftHand,
      rUpper: rightUpper,
      rLower: rightLower,
      rHand: rightHand,
      leftFoot: makeFoot(1, pose),
      rightFoot: makeFoot(-1, pose),
      leftKneeBias: vec(kneeSide, -0.35, kneeForward),
      rightKneeBias: vec(-kneeSide, -0.35, kneeForward),
      kneeBias: {
        left: vec(kneeSide, -0.35, kneeForward),
        right: vec(-kneeSide, -0.35, kneeForward)
      },
      contacts: {
        leftFoot: "ground_locked",
        rightFoot: "ground_locked",
        leftHand: pose.grip.indexOf("ungripped") === -1 && pose.grip.indexOf("reaching") === -1 ? "left_box_side_edge" : "reaching",
        rightHand: pose.grip.indexOf("ungripped") === -1 && pose.grip.indexOf("reaching") === -1 ? "right_box_side_edge" : "reaching",
        prop: "box"
      },
      box: {
        center: vec(0, pose.boxY, pose.boxZ),
        size: vec(boxSize[0], boxSize[1], boxSize[2]),
        phase: pose.phase,
        grip: pose.grip,
        onFloor: boxOnFloor,
        handContactWidth: round(pose.handWidth),
        leftGrip: vec(halfGripWidth, pose.handY, pose.handZ),
        rightGrip: vec(-halfGripWidth, pose.handY, pose.handZ),
        path: "starts on floor, stays close to shins and torso, then presses overhead"
      },
      effort: {
        brace: round(pose.brace),
        kneeBend: round(pose.kneeBend),
        weightShift: round(pose.weightShift)
      }
    };
  }

  const frames = Array.from({ length: frameCount }, (_, frame) => makeFrame(frame));

  const data = {
    id: "boxOverheadLift",
    name: "Box Overhead Lift",
    koreanName: "상자 머리위로 들기",
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
      intent: "Manny starts over a box resting on the floor, bends down, grips the side edges, lifts it close to the body, and raises it overhead.",
      notes: [
        "The box begins on the floor with center Y equal to half the box height.",
        "Feet stay planted while hips move back, knees bend, and the torso braces before the pull.",
        "Hands track the box side edges from the grip key pose through overhead lockout.",
        "The box path stays close to shins and torso until the leg drive and press."
      ]
    },
    prop: {
      id: "floor_box",
      type: "box",
      size: vec(boxSize[0], boxSize[1], boxSize[2]),
      initialCenter: vec(0, boxSize[1] * 0.5, 33),
      coordinateSystem: "X side, Y up, Z forward",
      contact: "Both hands grip opposite side edges after the reach-down beat."
    },
    contactNotes: {
      feet: "Both foot balls and ankles remain planted without sliding for the full non-looping lift.",
      hands: "Before frame 12 hands reach toward the box; from frame 12 onward hands are constrained to the side edges.",
      interpolation: "Smoothstep interpolation preserves braced floor pull, knee-height lift, chest catch, press, overhead hold, and settle."
    },
    keyposes: frames.filter((frame) => keyNames.has(frame.frame)),
    frames
  };

  data.keys = frames;

  global.mannyBoxOverheadLiftKeyposes = data;
})(typeof window !== "undefined" ? window : globalThis);
