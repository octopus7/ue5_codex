(function (global) {
  "use strict";

  const frameCount = 1;
  const fps = 1;
  const durationSeconds = 1;

  function round(value) {
    return Number(value.toFixed(3));
  }

  function vec(x, y, z) {
    return [round(x), round(y), round(z)];
  }

  const pose = {
    frame: 0,
    name: "hero_landing_pose_static",
    type: "pose",
    source: {
      referenceImage: "../imagegen/hero_landing_imagegen_reference.png",
      extractedPose: "../imagegen/hero_landing_pose_extracted.png",
      note: "Approximate 3D Manny key pose derived from the generated hero landing reference."
    },
    pelvis: vec(0.0, 42.0, 10.0).concat([round(-26.0), round(10.0), round(-8.0)]),
    chest: [round(66.0), round(-12.0), round(8.0)],
    neck: [round(-22.0), round(5.0), round(-2.0)],
    head: [round(-20.0), round(5.0), round(-2.0)],
    leftHandTarget: vec(-49.0, 95.0, -34.0),
    rightHandTarget: vec(-24.0, 5.0, 48.0),
    lUpper: vec(16.0, 4.0, -12.0),
    lLower: vec(17.0, 13.0, -20.0),
    lHand: vec(10.0, 7.0, -19.0),
    rUpper: vec(-14.0, -22.0, 14.0),
    rLower: vec(-10.0, -29.0, 19.0),
    rHand: vec(-4.0, -19.0, 18.0),
    leftFoot: {
      ankle: vec(34.0, 8.1, 68.0),
      ball: vec(37.0, 1.05, 84.0),
      planted: true,
      locked: true,
      contactWeight: 1
    },
    rightFoot: {
      ankle: vec(-25.0, 8.1, -36.0),
      ball: vec(-28.0, 1.05, -21.0),
      planted: true,
      locked: true,
      contactWeight: 1
    },
    leftKneeBias: vec(8.0, -9.0, 13.0),
    rightKneeBias: vec(-8.0, -3.0, -8.0),
    contacts: {
      leftFoot: true,
      rightFoot: true,
      rightHand: true
    }
  };

  const data = {
    id: "heroLandingPose",
    name: "Hero Landing Pose",
    koreanName: "히어로 랜딩 포즈",
    type: "pose",
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
      intent: "Single-frame hero landing pose: low pelvis, one hand on the ground, one leg forward, one leg swept back, and a lifted chest/head line.",
      notes: [
        "This is a static pose mode, not a looped animation.",
        "The right hand, both feet, and toe balls are planted for a grounded landing silhouette.",
        "Pose is based on the generated image reference saved under the project imagegen directory."
      ]
    },
    keyposes: [pose],
    frames: [pose]
  };

  data.keys = data.frames;

  global.mannyHeroLandingPoseKeyposes = data;
})(typeof window !== "undefined" ? window : globalThis);
