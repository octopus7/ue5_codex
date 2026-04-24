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
      ankle: vec(side * 15.0, 8.1, 34.0),
      ball: vec(side * 15.8, 1.05, 48.0),
      planted: true,
      locked: true,
      contactWeight: 1,
      role: "seated_floor_contact"
    };
  }

  const chair = {
    seatCenter: vec(0, 43, -18),
    seatSize: vec(52, 5, 44),
    backCenter: vec(0, 76, -42),
    backSize: vec(52, 46, 5),
    legHeight: 42
  };

  const table = {
    topCenter: vec(48, 59, 30),
    topSize: vec(62, 5, 44),
    legHeight: 58
  };

  const keyposes = [
    {
      frame: 0,
      name: "seated_idle_bottle_on_table",
      pelvis: vec(0, 63.5, -15).concat([round(-7), round(-4), round(0)]),
      chest: [round(4), round(7), round(0)],
      neck: [round(-1), round(2), round(0)],
      head: [round(-2), round(5), round(0)],
      rightHandTarget: vec(-10, 64, 9),
      lUpper: vec(12, -13, 7),
      lLower: vec(8, -20, 6),
      lHand: vec(3, -13, 6),
      rUpper: vec(-12, -13, 7),
      rLower: vec(-8, -20, 6),
      rHand: vec(-3, -13, 6),
      bottle: {
        bottom: vec(48, 62, 30),
        top: vec(48, 94, 30),
        center: vec(48, 78, 30),
        contact: "on_table"
      }
    },
    {
      frame: 10,
      name: "lean_and_reach_to_bottle",
      pelvis: vec(1.5, 63, -14).concat([round(-9), round(4), round(-2)]),
      chest: [round(14), round(18), round(-4)],
      neck: [round(-4), round(4), round(1)],
      head: [round(-8), round(9), round(1)],
      rightHandTarget: vec(37, 82, 30),
      lUpper: vec(12, -12, 7),
      lLower: vec(8, -20, 6),
      lHand: vec(3, -13, 6),
      rUpper: vec(-10, -7, 12),
      rLower: vec(-7, -11, 16),
      rHand: vec(-3, -7, 15),
      bottle: {
        bottom: vec(48, 62, 30),
        top: vec(48, 94, 30),
        center: vec(48, 78, 30),
        contact: "on_table"
      }
    },
    {
      frame: 18,
      name: "grip_bottle_on_table",
      pelvis: vec(2, 62.8, -13).concat([round(-10), round(9), round(-3)]),
      chest: [round(16), round(28), round(-5)],
      neck: [round(-5), round(6), round(1)],
      head: [round(-9), round(12), round(1)],
      rightHandTarget: vec(48, 79, 30),
      lUpper: vec(12, -12, 7),
      lLower: vec(8, -20, 6),
      lHand: vec(3, -13, 6),
      rUpper: vec(-8, -6, 14),
      rLower: vec(-5, -10, 17),
      rHand: vec(-2, -7, 16),
      bottle: {
        bottom: vec(48, 62, 30),
        top: vec(48, 94, 30),
        center: vec(48, 78, 30),
        contact: "right_hand_grip"
      }
    },
    {
      frame: 28,
      name: "lift_bottle_clear_of_table",
      pelvis: vec(1.2, 63.2, -14).concat([round(-8), round(5), round(-2)]),
      chest: [round(8), round(18), round(-3)],
      neck: [round(-2), round(3), round(0)],
      head: [round(-5), round(9), round(0)],
      rightHandTarget: vec(35, 98, 24),
      lUpper: vec(12, -13, 7),
      lLower: vec(8, -20, 6),
      lHand: vec(3, -13, 6),
      rUpper: vec(-8, -5, 13),
      rLower: vec(-5, -9, 14),
      rHand: vec(-2, -6, 13),
      bottle: {
        bottom: vec(35, 83, 24),
        top: vec(35, 115, 24),
        center: vec(35, 99, 24),
        contact: "in_right_hand"
      }
    },
    {
      frame: 38,
      name: "bring_bottle_to_mouth",
      pelvis: vec(0.5, 63.5, -15).concat([round(-5), round(0), round(-1)]),
      chest: [round(3), round(5), round(-1)],
      neck: [round(2), round(-2), round(0)],
      head: [round(7), round(-2), round(0)],
      rightHandTarget: vec(14, 119, 13),
      lUpper: vec(12, -13, 7),
      lLower: vec(8, -20, 6),
      lHand: vec(3, -13, 6),
      rUpper: vec(-7, -3, 10),
      rLower: vec(-5, -6, 8),
      rHand: vec(-2, -3, 7),
      bottle: {
        bottom: vec(13, 105, 12),
        top: vec(19, 136, 17),
        center: vec(16, 120, 14),
        contact: "near_mouth"
      }
    },
    {
      frame: 46,
      name: "drink_tilt_hold",
      pelvis: vec(0, 63.5, -15).concat([round(-4), round(-1), round(0)]),
      chest: [round(0), round(2), round(0)],
      neck: [round(5), round(-2), round(0)],
      head: [round(12), round(-2), round(0)],
      rightHandTarget: vec(13, 123, 12),
      lUpper: vec(12, -13, 7),
      lLower: vec(8, -20, 6),
      lHand: vec(3, -13, 6),
      rUpper: vec(-7, -2, 9),
      rLower: vec(-5, -5, 7),
      rHand: vec(-2, -2, 6),
      bottle: {
        bottom: vec(11, 109, 11),
        top: vec(24, 137, 19),
        center: vec(17, 123, 15),
        contact: "drinking"
      }
    },
    {
      frame: 56,
      name: "lower_bottle_from_mouth",
      pelvis: vec(0.8, 63.3, -14).concat([round(-6), round(4), round(-1)]),
      chest: [round(5), round(12), round(-2)],
      neck: [round(0), round(2), round(0)],
      head: [round(-3), round(7), round(0)],
      rightHandTarget: vec(33, 98, 24),
      lUpper: vec(12, -13, 7),
      lLower: vec(8, -20, 6),
      lHand: vec(3, -13, 6),
      rUpper: vec(-8, -5, 12),
      rLower: vec(-5, -8, 14),
      rHand: vec(-2, -5, 12),
      bottle: {
        bottom: vec(33, 83, 24),
        top: vec(33, 115, 24),
        center: vec(33, 99, 24),
        contact: "in_right_hand"
      }
    },
    {
      frame: 66,
      name: "return_bottle_to_table",
      pelvis: vec(1.5, 63, -14).concat([round(-9), round(7), round(-2)]),
      chest: [round(13), round(22), round(-4)],
      neck: [round(-4), round(5), round(1)],
      head: [round(-7), round(10), round(1)],
      rightHandTarget: vec(48, 79, 30),
      lUpper: vec(12, -12, 7),
      lLower: vec(8, -20, 6),
      lHand: vec(3, -13, 6),
      rUpper: vec(-8, -6, 14),
      rLower: vec(-5, -10, 17),
      rHand: vec(-2, -7, 16),
      bottle: {
        bottom: vec(48, 62, 30),
        top: vec(48, 94, 30),
        center: vec(48, 78, 30),
        contact: "placing_on_table"
      }
    },
    {
      frame: 71,
      name: "seated_settle_bottle_on_table",
      pelvis: vec(0, 63.5, -15).concat([round(-7), round(-4), round(0)]),
      chest: [round(4), round(7), round(0)],
      neck: [round(-1), round(2), round(0)],
      head: [round(-2), round(5), round(0)],
      rightHandTarget: vec(-10, 64, 9),
      lUpper: vec(12, -13, 7),
      lLower: vec(8, -20, 6),
      lHand: vec(3, -13, 6),
      rUpper: vec(-12, -13, 7),
      rLower: vec(-8, -20, 6),
      rHand: vec(-3, -13, 6),
      bottle: {
        bottom: vec(48, 62, 30),
        top: vec(48, 94, 30),
        center: vec(48, 78, 30),
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
    return {
      frame,
      name: frame === previous.frame ? previous.name : "seated_bottle_drink_inbetween",
      pelvis: lerpVec(previous.pelvis, next.pelvis, t).concat(lerpVec(previous.pelvis.slice(3), next.pelvis.slice(3), t)),
      chest: lerpVec(previous.chest, next.chest, t),
      neck: lerpVec(previous.neck, next.neck, t),
      head: lerpVec(previous.head, next.head, t),
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
  }

  const frames = Array.from({ length: frameCount }, (_, frame) => sampleTrack(frame));

  const data = {
    id: "seatedBottleDrink",
    name: "Seated Bottle Drink",
    koreanName: "앉아서 병음료 마시기",
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
        "Feet remain on the floor while the pelvis stays seated on the chair."
      ]
    },
    keyposes,
    frames
  };

  data.keys = frames;

  global.mannySeatedBottleDrinkKeyposes = data;
})(typeof window !== "undefined" ? window : globalThis);
