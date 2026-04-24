(function (global) {
  "use strict";

  const frameCount = 72;
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

  function lerpVec(a, b, t) {
    return vec(lerp(a[0], b[0], t), lerp(a[1], b[1], t), lerp(a[2], b[2], t));
  }

  function foot(side) {
    return {
      ankle: vec(side * 13.2, 8.1, 37.0),
      ball: vec(side * 14.0, 1.05, 51.5),
      planted: true,
      locked: true,
      contactWeight: 1,
      role: "seated_floor_contact"
    };
  }

  const chair = {
    seatCenter: vec(0, 47, -12),
    seatSize: vec(52, 6, 44),
    backCenter: vec(0, 79, -35),
    backSize: vec(52, 62, 6),
    legHeight: 47
  };

  const table = {
    topCenter: vec(36, 62, 24),
    topSize: vec(48, 5, 40),
    legHeight: 61
  };

  function pointLerp(a, b, alpha) {
    return vec(
      lerp(a[0], b[0], alpha),
      lerp(a[1], b[1], alpha),
      lerp(a[2], b[2], alpha)
    );
  }

  function armPoints(clavicle, hand, side, elbowLift, elbowForward) {
    const upperBase = pointLerp(clavicle, hand, 0.35);
    const lowerBase = pointLerp(clavicle, hand, 0.70);
    return {
      upper: vec(upperBase[0] + side * 4.0, upperBase[1] + elbowLift, upperBase[2] - 4.0),
      lower: vec(lowerBase[0] + side * 2.0, lowerBase[1] + elbowLift * 0.35, lowerBase[2] + elbowForward),
      hand
    };
  }

  function makeSeatedPoints(key) {
    const pelvis = key.pelvis.slice(0, 3);
    const forwardLean = key.chest[0] * 0.18;
    const reachSide = key.chest[1] * 0.045;
    const points = {};

    points.pelvis = vec(pelvis[0], pelvis[1], pelvis[2]);
    points.spine_01 = vec(pelvis[0] + reachSide * 0.12, pelvis[1] + 11.2, pelvis[2] + forwardLean * 0.4);
    points.spine_02 = vec(pelvis[0] + reachSide * 0.26, pelvis[1] + 23.5, pelvis[2] + forwardLean * 0.9);
    points.spine_03 = vec(pelvis[0] + reachSide * 0.44, pelvis[1] + 36.0, pelvis[2] + forwardLean * 1.5);
    points.spine_04 = vec(pelvis[0] + reachSide * 0.58, pelvis[1] + 46.2, pelvis[2] + forwardLean * 1.9);
    points.spine_05 = vec(pelvis[0] + reachSide * 0.68, pelvis[1] + 55.4, pelvis[2] + forwardLean * 2.2);
    points.neck_01 = vec(pelvis[0] + reachSide * 0.72, pelvis[1] + 67.2, pelvis[2] + forwardLean * 2.45);
    points.head = vec(pelvis[0] + reachSide * 0.76, pelvis[1] + 83.5, pelvis[2] + forwardLean * 2.7);

    points.clavicle_l = vec(points.spine_05[0] + 17.3, points.spine_05[1] + 2.8, points.spine_05[2] + 1.3);
    points.clavicle_r = vec(points.spine_05[0] - 17.3, points.spine_05[1] + 2.8, points.spine_05[2] - 1.3);

    const leftArm = armPoints(points.clavicle_l, key.leftHandTarget, 1, -9.0, 5.0);
    const rightArm = armPoints(points.clavicle_r, key.rightHandTarget, -1, -8.0, 4.0);
    points.upperarm_l = leftArm.upper;
    points.lowerarm_l = leftArm.lower;
    points.hand_l = leftArm.hand;
    points.upperarm_r = rightArm.upper;
    points.lowerarm_r = rightArm.lower;
    points.hand_r = rightArm.hand;

    points.thigh_l = vec(pelvis[0] + 10.2, pelvis[1] - 4.4, pelvis[2] + 1.5);
    points.thigh_r = vec(pelvis[0] - 10.2, pelvis[1] - 4.4, pelvis[2] + 1.5);
    points.calf_l = vec(pelvis[0] + 12.0, 43.0, 23.0);
    points.calf_r = vec(pelvis[0] - 12.0, 43.0, 23.0);
    points.foot_l = vec(pelvis[0] + 13.2, 8.1, 37.0);
    points.foot_r = vec(pelvis[0] - 13.2, 8.1, 37.0);
    points.ball_l = vec(pelvis[0] + 14.0, 1.05, 51.5);
    points.ball_r = vec(pelvis[0] - 14.0, 1.05, 51.5);
    return points;
  }

  const keyposes = [
    {
      frame: 0,
      name: "seated_idle_bottle_on_table",
      pelvis: vec(0, 58.5, -13).concat([round(-6), round(-4), round(0)]),
      chest: [round(4), round(7), round(0)],
      neck: [round(-1), round(2), round(0)],
      head: [round(-2), round(5), round(0)],
      leftHandTarget: vec(14, 60, 9),
      rightHandTarget: vec(-14, 60, 10),
      lUpper: vec(12, -13, 7),
      lLower: vec(8, -20, 6),
      lHand: vec(3, -13, 6),
      rUpper: vec(-12, -13, 7),
      rLower: vec(-8, -20, 6),
      rHand: vec(-3, -13, 6),
      leftKneeBias: vec(0, 0, 0),
      rightKneeBias: vec(0, 0, 0),
      bottle: {
        bottom: vec(36, 65, 24),
        top: vec(36, 97, 24),
        center: vec(36, 81, 24),
        contact: "on_table"
      }
    },
    {
      frame: 10,
      name: "lean_and_reach_to_bottle",
      pelvis: vec(1.2, 58.2, -12.5).concat([round(-8), round(4), round(-2)]),
      chest: [round(14), round(18), round(-4)],
      neck: [round(-4), round(4), round(1)],
      head: [round(-8), round(9), round(1)],
      leftHandTarget: vec(13, 60, 9),
      rightHandTarget: vec(26, 80, 22),
      lUpper: vec(12, -12, 7),
      lLower: vec(8, -20, 6),
      lHand: vec(3, -13, 6),
      rUpper: vec(-10, -7, 12),
      rLower: vec(-7, -11, 16),
      rHand: vec(-3, -7, 15),
      leftKneeBias: vec(0, 0, 0),
      rightKneeBias: vec(0, 0, 0),
      bottle: {
        bottom: vec(36, 65, 24),
        top: vec(36, 97, 24),
        center: vec(36, 81, 24),
        contact: "on_table"
      }
    },
    {
      frame: 18,
      name: "grip_bottle_on_table",
      pelvis: vec(1.8, 58.0, -12).concat([round(-9), round(9), round(-3)]),
      chest: [round(16), round(28), round(-5)],
      neck: [round(-5), round(6), round(1)],
      head: [round(-9), round(12), round(1)],
      leftHandTarget: vec(12, 60, 9),
      rightHandTarget: vec(35, 80, 24),
      lUpper: vec(12, -12, 7),
      lLower: vec(8, -20, 6),
      lHand: vec(3, -13, 6),
      rUpper: vec(-8, -6, 14),
      rLower: vec(-5, -10, 17),
      rHand: vec(-2, -7, 16),
      leftKneeBias: vec(0, 0, 0),
      rightKneeBias: vec(0, 0, 0),
      bottle: {
        bottom: vec(36, 65, 24),
        top: vec(36, 97, 24),
        center: vec(36, 81, 24),
        contact: "right_hand_grip"
      }
    },
    {
      frame: 28,
      name: "lift_bottle_clear_of_table",
      pelvis: vec(1.0, 58.2, -12.6).concat([round(-7), round(5), round(-2)]),
      chest: [round(8), round(18), round(-3)],
      neck: [round(-2), round(3), round(0)],
      head: [round(-5), round(9), round(0)],
      leftHandTarget: vec(13, 60, 9),
      rightHandTarget: vec(29, 99, 16),
      lUpper: vec(12, -13, 7),
      lLower: vec(8, -20, 6),
      lHand: vec(3, -13, 6),
      rUpper: vec(-8, -5, 13),
      rLower: vec(-5, -9, 14),
      rHand: vec(-2, -6, 13),
      leftKneeBias: vec(0, 0, 0),
      rightKneeBias: vec(0, 0, 0),
      bottle: {
        bottom: vec(29, 85, 16),
        top: vec(31, 117, 17),
        center: vec(30, 101, 16.5),
        contact: "in_right_hand"
      }
    },
    {
      frame: 38,
      name: "bring_bottle_to_mouth",
      pelvis: vec(0.3, 58.5, -13).concat([round(-4), round(0), round(-1)]),
      chest: [round(3), round(5), round(-1)],
      neck: [round(2), round(-2), round(0)],
      head: [round(7), round(-2), round(0)],
      leftHandTarget: vec(14, 60, 9),
      rightHandTarget: vec(8, 119, 0),
      lUpper: vec(12, -13, 7),
      lLower: vec(8, -20, 6),
      lHand: vec(3, -13, 6),
      rUpper: vec(-7, -3, 10),
      rLower: vec(-5, -6, 8),
      rHand: vec(-2, -3, 7),
      leftKneeBias: vec(0, 0, 0),
      rightKneeBias: vec(0, 0, 0),
      bottle: {
        bottom: vec(7, 104, -1),
        top: vec(18, 133, 8),
        center: vec(12.5, 118.5, 3.5),
        contact: "near_mouth"
      }
    },
    {
      frame: 46,
      name: "drink_tilt_hold",
      pelvis: vec(0, 58.5, -13).concat([round(-3), round(-1), round(0)]),
      chest: [round(0), round(2), round(0)],
      neck: [round(5), round(-2), round(0)],
      head: [round(12), round(-2), round(0)],
      leftHandTarget: vec(14, 60, 9),
      rightHandTarget: vec(7, 121, -1),
      lUpper: vec(12, -13, 7),
      lLower: vec(8, -20, 6),
      lHand: vec(3, -13, 6),
      rUpper: vec(-7, -2, 9),
      rLower: vec(-5, -5, 7),
      rHand: vec(-2, -2, 6),
      leftKneeBias: vec(0, 0, 0),
      rightKneeBias: vec(0, 0, 0),
      bottle: {
        bottom: vec(6, 107, -2),
        top: vec(23, 132, 8),
        center: vec(14.5, 119.5, 3),
        contact: "drinking"
      }
    },
    {
      frame: 56,
      name: "lower_bottle_from_mouth",
      pelvis: vec(0.7, 58.3, -12.5).concat([round(-6), round(4), round(-1)]),
      chest: [round(5), round(12), round(-2)],
      neck: [round(0), round(2), round(0)],
      head: [round(-3), round(7), round(0)],
      leftHandTarget: vec(14, 60, 9),
      rightHandTarget: vec(28, 99, 16),
      lUpper: vec(12, -13, 7),
      lLower: vec(8, -20, 6),
      lHand: vec(3, -13, 6),
      rUpper: vec(-8, -5, 12),
      rLower: vec(-5, -8, 14),
      rHand: vec(-2, -5, 12),
      leftKneeBias: vec(0, 0, 0),
      rightKneeBias: vec(0, 0, 0),
      bottle: {
        bottom: vec(28, 85, 16),
        top: vec(30, 117, 17),
        center: vec(29, 101, 16.5),
        contact: "in_right_hand"
      }
    },
    {
      frame: 66,
      name: "return_bottle_to_table",
      pelvis: vec(1.4, 58.1, -12.3).concat([round(-8), round(7), round(-2)]),
      chest: [round(13), round(22), round(-4)],
      neck: [round(-4), round(5), round(1)],
      head: [round(-7), round(10), round(1)],
      leftHandTarget: vec(13, 60, 9),
      rightHandTarget: vec(35, 80, 24),
      lUpper: vec(12, -12, 7),
      lLower: vec(8, -20, 6),
      lHand: vec(3, -13, 6),
      rUpper: vec(-8, -6, 14),
      rLower: vec(-5, -10, 17),
      rHand: vec(-2, -7, 16),
      leftKneeBias: vec(0, 0, 0),
      rightKneeBias: vec(0, 0, 0),
      bottle: {
        bottom: vec(36, 65, 24),
        top: vec(36, 97, 24),
        center: vec(36, 81, 24),
        contact: "placing_on_table"
      }
    },
    {
      frame: 71,
      name: "seated_settle_bottle_on_table",
      pelvis: vec(0, 58.5, -13).concat([round(-6), round(-4), round(0)]),
      chest: [round(4), round(7), round(0)],
      neck: [round(-1), round(2), round(0)],
      head: [round(-2), round(5), round(0)],
      leftHandTarget: vec(14, 60, 9),
      rightHandTarget: vec(-14, 60, 10),
      lUpper: vec(12, -13, 7),
      lLower: vec(8, -20, 6),
      lHand: vec(3, -13, 6),
      rUpper: vec(-12, -13, 7),
      rLower: vec(-8, -20, 6),
      rHand: vec(-3, -13, 6),
      leftKneeBias: vec(0, 0, 0),
      rightKneeBias: vec(0, 0, 0),
      bottle: {
        bottom: vec(36, 65, 24),
        top: vec(36, 97, 24),
        center: vec(36, 81, 24),
        contact: "on_table"
      }
    }
  ];

  function sampleTrack(frame) {
    let previous = keyposes[0];
    let next = keyposes[keyposes.length - 1];
    for (let index = 0; index < keyposes.length - 1; index += 1) {
      if (frame >= keyposes[index].frame && frame <= keyposes[index + 1].frame) {
        previous = keyposes[index];
        next = keyposes[index + 1];
        break;
      }
    }
    const t = smoothstep((frame - previous.frame) / Math.max(1, next.frame - previous.frame));
    const sampled = {
      frame,
      name: frame === previous.frame ? previous.name : "seated_bottle_drink_inbetween",
      pelvis: lerpVec(previous.pelvis, next.pelvis, t).concat(lerpVec(previous.pelvis.slice(3), next.pelvis.slice(3), t)),
      chest: lerpVec(previous.chest, next.chest, t),
      neck: lerpVec(previous.neck, next.neck, t),
      head: lerpVec(previous.head, next.head, t),
      leftHandTarget: lerpVec(previous.leftHandTarget, next.leftHandTarget, t),
      rightHandTarget: lerpVec(previous.rightHandTarget, next.rightHandTarget, t),
      lUpper: lerpVec(previous.lUpper, next.lUpper, t),
      lLower: lerpVec(previous.lLower, next.lLower, t),
      lHand: lerpVec(previous.lHand, next.lHand, t),
      rUpper: lerpVec(previous.rUpper, next.rUpper, t),
      rLower: lerpVec(previous.rLower, next.rLower, t),
      rHand: lerpVec(previous.rHand, next.rHand, t),
      leftFoot: foot(1),
      rightFoot: foot(-1),
      leftKneeBias: lerpVec(previous.leftKneeBias, next.leftKneeBias, t),
      rightKneeBias: lerpVec(previous.rightKneeBias, next.rightKneeBias, t),
      bottle: {
        bottom: lerpVec(previous.bottle.bottom, next.bottle.bottom, t),
        top: lerpVec(previous.bottle.top, next.bottle.top, t),
        center: lerpVec(previous.bottle.center, next.bottle.center, t),
        contact: t < 0.5 ? previous.bottle.contact : next.bottle.contact
      },
      contacts: {
        seated: true,
        chair: true,
        table: true,
        bottle: t < 0.5 ? previous.bottle.contact : next.bottle.contact,
        leftFoot: "floor",
        rightFoot: "floor"
      }
    };
    sampled.points = makeSeatedPoints(sampled);
    return sampled;
  }

  const frames = Array.from({ length: frameCount }, (_, frame) => sampleTrack(frame));

  const data = {
    id: "seatedBottleDrink",
    name: "Seated Bottle Drink",
    koreanName: "Seated Bottle Drink",
    frameCount,
    fps,
    durationSeconds,
    loop: false,
    rotationUnits: "degrees",
    positionUnits: "Manny demo units",
    referenceImage: "../imagegen/generated/seated_bottle_drink_reference.png",
    coordinateSystem: {
      up: "Y",
      forward: "Z",
      side: "X",
      groundY: 0
    },
    props: {
      chair,
      table,
      bottle: {
        radius: 3.2,
        height: 32,
        contentsColor: "amber"
      }
    },
    style: {
      intent: "A seated character reaches to a bottle drink on a table, lifts it, drinks, lowers it, and returns it to the table.",
      notes: [
        "Generated image reference is stored in imagegen/generated.",
        "Chair, table, and bottle are viewer props rather than baked mesh data.",
        "The seated pose uses explicit joint points so the pelvis stays on the chair and both feet stay planted."
      ]
    },
    keyposes,
    frames
  };

  data.keys = frames;

  global.mannySeatedBottleDrinkKeyposes = data;
})(typeof window !== "undefined" ? window : globalThis);
