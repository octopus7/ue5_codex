window.mannyWallClimbKeyposes = {
  id: "wallClimb",
  name: "Wall Climb",
  loop: false,
  frameCount: 24,
  fps: 24,
  coordinateSystem: {
    up: "Y",
    forward: "Z",
    side: "X",
    positionUnits: "Manny demo units",
    rotationUnits: "degrees"
  },
  wall: {
    id: "wallClimbTestWall",
    center: { x: 0, y: 58, z: 42 },
    size: { width: 72, height: 116, thickness: 7 },
    topY: 116,
    surfaceZ: 38.5,
    farFaceZ: 45.5,
    topLandingZ: 69,
    leftEdgeX: -36,
    rightEdgeX: 36,
    material: {
      color: 0x5d9bd3,
      edgeColor: 0x3377b1,
      opacity: 0.88
    }
  },
  builderHints: {
    interpolation: "cubic ease between frames with contact-aware foot and hand locks",
    body: "Drive pelvis/chest/head first, then solve limbs to hand and foot targets.",
    contacts: "When a contact value is ground, wall, or top, hold that target unless the next frame changes contact.",
    stance: "Keep X movement restrained; most travel is along +Z and then +Y over the wall."
  },
  frames: [
    {
      frame: 0,
      name: "approach_gather",
      key: true,
      pelvis: { pos: { x: 0.0, y: 95.0, z: -70.0 }, rot: { pitch: -7.0, yaw: 0.0, roll: 0.0 } },
      chest: { rot: { pitch: -14.0, yaw: 0.0, roll: 0.0 } },
      head: { rot: { pitch: 8.0, yaw: 0.0, roll: 0.0 }, lookAt: { x: 0.0, y: 116.0, z: 38.5 } },
      leftArm: { shoulder: { pitch: 18.0, yaw: 6.0, roll: 8.0 }, elbow: { bend: 70.0 }, wrist: { pitch: -8.0 } },
      rightArm: { shoulder: { pitch: -28.0, yaw: -5.0, roll: -8.0 }, elbow: { bend: 82.0 }, wrist: { pitch: -8.0 } },
      leftFoot: { ankle: { x: 8.8, y: 8.3, z: -57.0 }, ball: { x: 9.6, y: 1.2, z: -42.0 }, rot: { pitch: -4.0, yaw: 0.0, roll: 0.0 }, weight: 0.62 },
      rightFoot: { ankle: { x: -8.6, y: 8.3, z: -81.0 }, ball: { x: -9.4, y: 1.2, z: -66.0 }, rot: { pitch: 5.0, yaw: 0.0, roll: 0.0 }, weight: 0.38 },
      leftHand: { pos: { x: 24.0, y: 84.0, z: -58.0 }, rot: { pitch: -16.0, yaw: 6.0, roll: 18.0 }, grip: 0.0, weight: 0.0 },
      rightHand: { pos: { x: -20.0, y: 72.0, z: -88.0 }, rot: { pitch: -14.0, yaw: -6.0, roll: -16.0 }, grip: 0.0, weight: 0.0 },
      contacts: { leftFoot: "ground", rightFoot: "ground", leftHand: "air", rightHand: "air" }
    },
    {
      frame: 1,
      name: "approach_drive",
      key: false,
      pelvis: { pos: { x: -0.3, y: 95.8, z: -61.0 }, rot: { pitch: -9.0, yaw: -1.0, roll: 0.5 } },
      chest: { rot: { pitch: -17.0, yaw: -1.5, roll: 0.4 } },
      head: { rot: { pitch: 9.0, yaw: 0.5, roll: 0.0 }, lookAt: { x: 0.0, y: 116.0, z: 38.5 } },
      leftArm: { shoulder: { pitch: -18.0, yaw: 7.0, roll: 7.0 }, elbow: { bend: 76.0 }, wrist: { pitch: -10.0 } },
      rightArm: { shoulder: { pitch: 20.0, yaw: -6.0, roll: -8.0 }, elbow: { bend: 80.0 }, wrist: { pitch: -8.0 } },
      leftFoot: { ankle: { x: 8.8, y: 8.3, z: -57.0 }, ball: { x: 9.6, y: 1.2, z: -42.0 }, rot: { pitch: -5.0, yaw: 0.0, roll: 0.0 }, weight: 0.82 },
      rightFoot: { ankle: { x: -8.5, y: 13.8, z: -65.0 }, ball: { x: -9.3, y: 6.0, z: -50.0 }, rot: { pitch: 20.0, yaw: 0.0, roll: 0.0 }, weight: 0.0 },
      leftHand: { pos: { x: 18.0, y: 76.0, z: -70.0 }, rot: { pitch: -20.0, yaw: 6.0, roll: 16.0 }, grip: 0.0, weight: 0.0 },
      rightHand: { pos: { x: -16.0, y: 88.0, z: -54.0 }, rot: { pitch: -16.0, yaw: -5.0, roll: -15.0 }, grip: 0.0, weight: 0.0 },
      contacts: { leftFoot: "ground", rightFoot: "air", leftHand: "air", rightHand: "air" }
    },
    {
      frame: 2,
      name: "approach_push",
      key: true,
      pelvis: { pos: { x: -0.4, y: 96.5, z: -50.0 }, rot: { pitch: -11.0, yaw: -1.4, roll: 0.8 } },
      chest: { rot: { pitch: -20.0, yaw: -2.0, roll: 0.6 } },
      head: { rot: { pitch: 10.0, yaw: 0.8, roll: 0.0 }, lookAt: { x: 0.0, y: 116.0, z: 38.5 } },
      leftArm: { shoulder: { pitch: -30.0, yaw: 8.0, roll: 8.0 }, elbow: { bend: 76.0 }, wrist: { pitch: -12.0 } },
      rightArm: { shoulder: { pitch: 32.0, yaw: -7.0, roll: -7.0 }, elbow: { bend: 78.0 }, wrist: { pitch: -8.0 } },
      leftFoot: { ankle: { x: 8.8, y: 8.3, z: -57.0 }, ball: { x: 9.6, y: 1.2, z: -42.0 }, rot: { pitch: -9.0, yaw: 0.0, roll: 0.0 }, weight: 1.0 },
      rightFoot: { ankle: { x: -7.8, y: 17.0, z: -42.0 }, ball: { x: -8.5, y: 9.0, z: -28.0 }, rot: { pitch: 28.0, yaw: 0.0, roll: 0.0 }, weight: 0.0 },
      leftHand: { pos: { x: 12.0, y: 70.0, z: -78.0 }, rot: { pitch: -22.0, yaw: 7.0, roll: 15.0 }, grip: 0.0, weight: 0.0 },
      rightHand: { pos: { x: -13.0, y: 98.0, z: -35.0 }, rot: { pitch: -14.0, yaw: -5.0, roll: -14.0 }, grip: 0.0, weight: 0.0 },
      contacts: { leftFoot: "ground", rightFoot: "air", leftHand: "air", rightHand: "air" }
    },
    {
      frame: 3,
      name: "wall_step_commit",
      key: false,
      pelvis: { pos: { x: -0.2, y: 98.0, z: -36.0 }, rot: { pitch: -13.0, yaw: -0.8, roll: 0.5 } },
      chest: { rot: { pitch: -23.0, yaw: -1.0, roll: 0.4 } },
      head: { rot: { pitch: 11.0, yaw: 0.5, roll: 0.0 }, lookAt: { x: 0.0, y: 116.0, z: 38.5 } },
      leftArm: { shoulder: { pitch: -36.0, yaw: 8.0, roll: 8.0 }, elbow: { bend: 78.0 }, wrist: { pitch: -12.0 } },
      rightArm: { shoulder: { pitch: 42.0, yaw: -8.0, roll: -8.0 }, elbow: { bend: 76.0 }, wrist: { pitch: -9.0 } },
      leftFoot: { ankle: { x: 8.8, y: 8.3, z: -57.0 }, ball: { x: 9.6, y: 1.2, z: -42.0 }, rot: { pitch: -14.0, yaw: 0.0, roll: 0.0 }, weight: 0.72 },
      rightFoot: { ankle: { x: -7.4, y: 26.0, z: 15.5 }, ball: { x: -8.1, y: 28.0, z: 38.5 }, rot: { pitch: 62.0, yaw: 0.0, roll: 0.0 }, weight: 0.18 },
      leftHand: { pos: { x: 8.0, y: 72.0, z: -66.0 }, rot: { pitch: -24.0, yaw: 7.0, roll: 14.0 }, grip: 0.0, weight: 0.0 },
      rightHand: { pos: { x: -12.0, y: 108.0, z: -14.0 }, rot: { pitch: -12.0, yaw: -4.0, roll: -12.0 }, grip: 0.0, weight: 0.0 },
      contacts: { leftFoot: "ground", rightFoot: "wall", leftHand: "air", rightHand: "air" }
    },
    {
      frame: 4,
      name: "first_wall_plant",
      key: true,
      pelvis: { pos: { x: -0.1, y: 101.5, z: -23.0 }, rot: { pitch: -17.0, yaw: 0.0, roll: 0.0 } },
      chest: { rot: { pitch: -28.0, yaw: 0.0, roll: 0.0 } },
      head: { rot: { pitch: 14.0, yaw: 0.0, roll: 0.0 }, lookAt: { x: 0.0, y: 116.0, z: 38.5 } },
      leftArm: { shoulder: { pitch: -42.0, yaw: 8.0, roll: 8.0 }, elbow: { bend: 82.0 }, wrist: { pitch: -14.0 } },
      rightArm: { shoulder: { pitch: 58.0, yaw: -8.0, roll: -8.0 }, elbow: { bend: 72.0 }, wrist: { pitch: -10.0 } },
      leftFoot: { ankle: { x: 8.8, y: 8.3, z: -57.0 }, ball: { x: 9.6, y: 1.2, z: -42.0 }, rot: { pitch: -18.0, yaw: 0.0, roll: 0.0 }, weight: 0.35 },
      rightFoot: { ankle: { x: -7.2, y: 39.0, z: 18.0 }, ball: { x: -7.8, y: 43.0, z: 38.5 }, rot: { pitch: 74.0, yaw: 0.0, roll: 0.0 }, weight: 0.85 },
      leftHand: { pos: { x: 5.5, y: 80.0, z: -50.0 }, rot: { pitch: -26.0, yaw: 6.0, roll: 12.0 }, grip: 0.0, weight: 0.0 },
      rightHand: { pos: { x: -9.8, y: 115.0, z: 8.0 }, rot: { pitch: -8.0, yaw: -4.0, roll: -10.0 }, grip: 0.0, weight: 0.0 },
      contacts: { leftFoot: "ground", rightFoot: "wall", leftHand: "air", rightHand: "air" }
    },
    {
      frame: 5,
      name: "wall_drive_up",
      key: false,
      pelvis: { pos: { x: 0.1, y: 108.0, z: -9.0 }, rot: { pitch: -20.0, yaw: 1.0, roll: -0.5 } },
      chest: { rot: { pitch: -31.0, yaw: 1.4, roll: -0.6 } },
      head: { rot: { pitch: 17.0, yaw: 0.5, roll: 0.0 }, lookAt: { x: 0.0, y: 116.0, z: 38.5 } },
      leftArm: { shoulder: { pitch: -20.0, yaw: 7.0, roll: 9.0 }, elbow: { bend: 88.0 }, wrist: { pitch: -12.0 } },
      rightArm: { shoulder: { pitch: 72.0, yaw: -7.0, roll: -8.0 }, elbow: { bend: 66.0 }, wrist: { pitch: -8.0 } },
      leftFoot: { ankle: { x: 8.0, y: 16.0, z: -28.0 }, ball: { x: 8.6, y: 8.0, z: -13.0 }, rot: { pitch: 24.0, yaw: 0.0, roll: 0.0 }, weight: 0.0 },
      rightFoot: { ankle: { x: -7.2, y: 39.0, z: 18.0 }, ball: { x: -7.8, y: 43.0, z: 38.5 }, rot: { pitch: 76.0, yaw: 0.0, roll: 0.0 }, weight: 1.0 },
      leftHand: { pos: { x: 6.0, y: 92.0, z: -30.0 }, rot: { pitch: -22.0, yaw: 5.0, roll: 10.0 }, grip: 0.0, weight: 0.0 },
      rightHand: { pos: { x: -8.0, y: 121.0, z: 24.0 }, rot: { pitch: -4.0, yaw: -3.0, roll: -8.0 }, grip: 0.0, weight: 0.0 },
      contacts: { leftFoot: "air", rightFoot: "wall", leftHand: "air", rightHand: "air" }
    },
    {
      frame: 6,
      name: "second_wall_touch",
      key: true,
      pelvis: { pos: { x: 0.2, y: 114.5, z: 3.0 }, rot: { pitch: -24.0, yaw: 1.5, roll: -0.8 } },
      chest: { rot: { pitch: -35.0, yaw: 2.0, roll: -0.8 } },
      head: { rot: { pitch: 20.0, yaw: 0.8, roll: 0.0 }, lookAt: { x: 0.0, y: 116.0, z: 38.5 } },
      leftArm: { shoulder: { pitch: 8.0, yaw: 6.0, roll: 8.0 }, elbow: { bend: 88.0 }, wrist: { pitch: -8.0 } },
      rightArm: { shoulder: { pitch: 85.0, yaw: -7.0, roll: -7.0 }, elbow: { bend: 58.0 }, wrist: { pitch: -5.0 } },
      leftFoot: { ankle: { x: 7.4, y: 59.0, z: 19.0 }, ball: { x: 8.0, y: 63.0, z: 38.5 }, rot: { pitch: 72.0, yaw: 0.0, roll: 0.0 }, weight: 0.55 },
      rightFoot: { ankle: { x: -7.2, y: 42.0, z: 18.0 }, ball: { x: -7.8, y: 46.0, z: 38.5 }, rot: { pitch: 77.0, yaw: 0.0, roll: 0.0 }, weight: 0.45 },
      leftHand: { pos: { x: 6.5, y: 105.0, z: -8.0 }, rot: { pitch: -16.0, yaw: 4.0, roll: 9.0 }, grip: 0.0, weight: 0.0 },
      rightHand: { pos: { x: -8.5, y: 124.0, z: 35.0 }, rot: { pitch: 0.0, yaw: -2.0, roll: -8.0 }, grip: 0.15, weight: 0.15 },
      contacts: { leftFoot: "wall", rightFoot: "wall", leftHand: "air", rightHand: "wall" }
    },
    {
      frame: 7,
      name: "reach_for_edge",
      key: false,
      pelvis: { pos: { x: 0.1, y: 121.0, z: 13.0 }, rot: { pitch: -27.0, yaw: 1.2, roll: -0.6 } },
      chest: { rot: { pitch: -39.0, yaw: 1.6, roll: -0.6 } },
      head: { rot: { pitch: 23.0, yaw: 0.6, roll: 0.0 }, lookAt: { x: 0.0, y: 116.0, z: 45.5 } },
      leftArm: { shoulder: { pitch: 46.0, yaw: 5.0, roll: 7.0 }, elbow: { bend: 70.0 }, wrist: { pitch: -4.0 } },
      rightArm: { shoulder: { pitch: 96.0, yaw: -6.0, roll: -7.0 }, elbow: { bend: 46.0 }, wrist: { pitch: -2.0 } },
      leftFoot: { ankle: { x: 7.4, y: 59.0, z: 19.0 }, ball: { x: 8.0, y: 63.0, z: 38.5 }, rot: { pitch: 73.0, yaw: 0.0, roll: 0.0 }, weight: 0.72 },
      rightFoot: { ankle: { x: -7.1, y: 46.0, z: 18.0 }, ball: { x: -7.7, y: 50.0, z: 38.5 }, rot: { pitch: 76.0, yaw: 0.0, roll: 0.0 }, weight: 0.28 },
      leftHand: { pos: { x: 8.5, y: 116.0, z: 31.0 }, rot: { pitch: -4.0, yaw: 3.0, roll: 8.0 }, grip: 0.1, weight: 0.1 },
      rightHand: { pos: { x: -8.5, y: 116.0, z: 38.5 }, rot: { pitch: 0.0, yaw: -2.0, roll: -8.0 }, grip: 0.45, weight: 0.55 },
      contacts: { leftFoot: "wall", rightFoot: "wall", leftHand: "wall", rightHand: "top" }
    },
    {
      frame: 8,
      name: "both_hands_catch_top",
      key: true,
      pelvis: { pos: { x: 0.0, y: 127.0, z: 22.0 }, rot: { pitch: -30.0, yaw: 0.6, roll: 0.0 } },
      chest: { rot: { pitch: -43.0, yaw: 1.0, roll: 0.0 } },
      head: { rot: { pitch: 25.0, yaw: 0.2, roll: 0.0 }, lookAt: { x: 0.0, y: 120.0, z: 60.0 } },
      leftArm: { shoulder: { pitch: 104.0, yaw: 4.0, roll: 6.0 }, elbow: { bend: 36.0 }, wrist: { pitch: 0.0 } },
      rightArm: { shoulder: { pitch: 104.0, yaw: -4.0, roll: -6.0 }, elbow: { bend: 36.0 }, wrist: { pitch: 0.0 } },
      leftFoot: { ankle: { x: 7.4, y: 62.0, z: 19.0 }, ball: { x: 8.0, y: 66.0, z: 38.5 }, rot: { pitch: 74.0, yaw: 0.0, roll: 0.0 }, weight: 0.58 },
      rightFoot: { ankle: { x: -7.1, y: 49.0, z: 18.0 }, ball: { x: -7.7, y: 53.0, z: 38.5 }, rot: { pitch: 76.0, yaw: 0.0, roll: 0.0 }, weight: 0.22 },
      leftHand: { pos: { x: 10.5, y: 116.0, z: 38.5 }, rot: { pitch: 0.0, yaw: 2.0, roll: 8.0 }, grip: 0.85, weight: 0.9 },
      rightHand: { pos: { x: -10.5, y: 116.0, z: 38.5 }, rot: { pitch: 0.0, yaw: -2.0, roll: -8.0 }, grip: 0.85, weight: 0.9 },
      contacts: { leftFoot: "wall", rightFoot: "wall", leftHand: "top", rightHand: "top" }
    },
    {
      frame: 9,
      name: "loaded_hang",
      key: false,
      pelvis: { pos: { x: 0.0, y: 124.5, z: 28.0 }, rot: { pitch: -32.0, yaw: 0.0, roll: 0.0 } },
      chest: { rot: { pitch: -47.0, yaw: 0.0, roll: 0.0 } },
      head: { rot: { pitch: 28.0, yaw: 0.0, roll: 0.0 }, lookAt: { x: 0.0, y: 121.0, z: 67.0 } },
      leftArm: { shoulder: { pitch: 108.0, yaw: 4.0, roll: 5.0 }, elbow: { bend: 48.0 }, wrist: { pitch: 2.0 } },
      rightArm: { shoulder: { pitch: 108.0, yaw: -4.0, roll: -5.0 }, elbow: { bend: 48.0 }, wrist: { pitch: 2.0 } },
      leftFoot: { ankle: { x: 7.2, y: 64.0, z: 19.0 }, ball: { x: 7.9, y: 68.0, z: 38.5 }, rot: { pitch: 74.0, yaw: 0.0, roll: 0.0 }, weight: 0.48 },
      rightFoot: { ankle: { x: -6.9, y: 47.0, z: 17.0 }, ball: { x: -7.7, y: 51.0, z: 38.5 }, rot: { pitch: 77.0, yaw: 0.0, roll: 0.0 }, weight: 0.12 },
      leftHand: { pos: { x: 10.5, y: 116.0, z: 38.5 }, rot: { pitch: 0.0, yaw: 2.0, roll: 8.0 }, grip: 1.0, weight: 1.0 },
      rightHand: { pos: { x: -10.5, y: 116.0, z: 38.5 }, rot: { pitch: 0.0, yaw: -2.0, roll: -8.0 }, grip: 1.0, weight: 1.0 },
      contacts: { leftFoot: "wall", rightFoot: "wall", leftHand: "top", rightHand: "top" }
    },
    {
      frame: 10,
      name: "pull_start",
      key: true,
      pelvis: { pos: { x: -0.2, y: 133.0, z: 35.0 }, rot: { pitch: -30.0, yaw: -1.0, roll: 0.5 } },
      chest: { rot: { pitch: -40.0, yaw: -1.5, roll: 0.4 } },
      head: { rot: { pitch: 23.0, yaw: -0.4, roll: 0.0 }, lookAt: { x: 0.0, y: 124.0, z: 74.0 } },
      leftArm: { shoulder: { pitch: 98.0, yaw: 4.0, roll: 7.0 }, elbow: { bend: 78.0 }, wrist: { pitch: 6.0 } },
      rightArm: { shoulder: { pitch: 98.0, yaw: -4.0, roll: -7.0 }, elbow: { bend: 78.0 }, wrist: { pitch: 6.0 } },
      leftFoot: { ankle: { x: 7.1, y: 70.0, z: 20.0 }, ball: { x: 7.8, y: 74.0, z: 38.5 }, rot: { pitch: 76.0, yaw: 0.0, roll: 0.0 }, weight: 0.58 },
      rightFoot: { ankle: { x: -6.5, y: 43.0, z: 16.0 }, ball: { x: -7.4, y: 47.0, z: 38.5 }, rot: { pitch: 78.0, yaw: 0.0, roll: 0.0 }, weight: 0.08 },
      leftHand: { pos: { x: 10.5, y: 116.0, z: 38.5 }, rot: { pitch: 2.0, yaw: 2.0, roll: 9.0 }, grip: 1.0, weight: 1.0 },
      rightHand: { pos: { x: -10.5, y: 116.0, z: 38.5 }, rot: { pitch: 2.0, yaw: -2.0, roll: -9.0 }, grip: 1.0, weight: 1.0 },
      contacts: { leftFoot: "wall", rightFoot: "wall", leftHand: "top", rightHand: "top" }
    },
    {
      frame: 11,
      name: "chest_over_edge",
      key: false,
      pelvis: { pos: { x: -0.4, y: 143.0, z: 43.0 }, rot: { pitch: -26.0, yaw: -1.5, roll: 0.8 } },
      chest: { rot: { pitch: -31.0, yaw: -2.0, roll: 0.8 } },
      head: { rot: { pitch: 18.0, yaw: -0.5, roll: 0.0 }, lookAt: { x: 0.0, y: 128.0, z: 81.0 } },
      leftArm: { shoulder: { pitch: 88.0, yaw: 5.0, roll: 8.0 }, elbow: { bend: 96.0 }, wrist: { pitch: 10.0 } },
      rightArm: { shoulder: { pitch: 90.0, yaw: -5.0, roll: -8.0 }, elbow: { bend: 96.0 }, wrist: { pitch: 10.0 } },
      leftFoot: { ankle: { x: 6.8, y: 78.0, z: 21.0 }, ball: { x: 7.6, y: 82.0, z: 38.5 }, rot: { pitch: 78.0, yaw: 0.0, roll: 0.0 }, weight: 0.42 },
      rightFoot: { ankle: { x: -6.2, y: 38.0, z: 10.0 }, ball: { x: -7.0, y: 42.0, z: 32.0 }, rot: { pitch: 70.0, yaw: 0.0, roll: 0.0 }, weight: 0.0 },
      leftHand: { pos: { x: 10.5, y: 116.0, z: 38.5 }, rot: { pitch: 8.0, yaw: 3.0, roll: 10.0 }, grip: 1.0, weight: 1.0 },
      rightHand: { pos: { x: -10.5, y: 116.0, z: 38.5 }, rot: { pitch: 8.0, yaw: -3.0, roll: -10.0 }, grip: 1.0, weight: 1.0 },
      contacts: { leftFoot: "wall", rightFoot: "air", leftHand: "top", rightHand: "top" }
    },
    {
      frame: 12,
      name: "hip_to_top",
      key: true,
      pelvis: { pos: { x: -0.5, y: 154.0, z: 52.0 }, rot: { pitch: -18.0, yaw: -1.8, roll: 1.0 } },
      chest: { rot: { pitch: -20.0, yaw: -2.0, roll: 0.8 } },
      head: { rot: { pitch: 11.0, yaw: -0.5, roll: 0.0 }, lookAt: { x: 0.0, y: 131.0, z: 87.0 } },
      leftArm: { shoulder: { pitch: 74.0, yaw: 5.0, roll: 8.0 }, elbow: { bend: 112.0 }, wrist: { pitch: 13.0 } },
      rightArm: { shoulder: { pitch: 78.0, yaw: -5.0, roll: -8.0 }, elbow: { bend: 112.0 }, wrist: { pitch: 13.0 } },
      leftFoot: { ankle: { x: 6.3, y: 89.0, z: 24.0 }, ball: { x: 7.1, y: 93.0, z: 38.5 }, rot: { pitch: 79.0, yaw: 0.0, roll: 0.0 }, weight: 0.18 },
      rightFoot: { ankle: { x: -7.0, y: 57.0, z: 28.0 }, ball: { x: -8.0, y: 61.0, z: 45.0 }, rot: { pitch: 62.0, yaw: 0.0, roll: 0.0 }, weight: 0.0 },
      leftHand: { pos: { x: 10.5, y: 116.0, z: 38.5 }, rot: { pitch: 13.0, yaw: 3.0, roll: 10.0 }, grip: 1.0, weight: 1.0 },
      rightHand: { pos: { x: -10.5, y: 116.0, z: 38.5 }, rot: { pitch: 13.0, yaw: -3.0, roll: -10.0 }, grip: 1.0, weight: 1.0 },
      contacts: { leftFoot: "wall", rightFoot: "air", leftHand: "top", rightHand: "top" }
    },
    {
      frame: 13,
      name: "left_knee_search",
      key: false,
      pelvis: { pos: { x: -0.8, y: 162.0, z: 59.0 }, rot: { pitch: -12.0, yaw: -2.2, roll: 1.2 } },
      chest: { rot: { pitch: -13.0, yaw: -2.5, roll: 1.0 } },
      head: { rot: { pitch: 7.0, yaw: -0.6, roll: 0.0 }, lookAt: { x: 1.0, y: 134.0, z: 91.0 } },
      leftArm: { shoulder: { pitch: 62.0, yaw: 6.0, roll: 9.0 }, elbow: { bend: 118.0 }, wrist: { pitch: 15.0 } },
      rightArm: { shoulder: { pitch: 66.0, yaw: -6.0, roll: -9.0 }, elbow: { bend: 118.0 }, wrist: { pitch: 15.0 } },
      leftFoot: { ankle: { x: 8.8, y: 112.0, z: 49.0 }, ball: { x: 9.6, y: 117.2, z: 62.0 }, rot: { pitch: 18.0, yaw: 2.0, roll: 0.0 }, weight: 0.18 },
      rightFoot: { ankle: { x: -7.0, y: 72.0, z: 32.0 }, ball: { x: -8.0, y: 76.0, z: 48.0 }, rot: { pitch: 60.0, yaw: 0.0, roll: 0.0 }, weight: 0.0 },
      leftHand: { pos: { x: 10.5, y: 116.0, z: 38.5 }, rot: { pitch: 16.0, yaw: 4.0, roll: 10.0 }, grip: 1.0, weight: 1.0 },
      rightHand: { pos: { x: -10.5, y: 116.0, z: 38.5 }, rot: { pitch: 16.0, yaw: -4.0, roll: -10.0 }, grip: 1.0, weight: 1.0 },
      contacts: { leftFoot: "top", rightFoot: "air", leftHand: "top", rightHand: "top" }
    },
    {
      frame: 14,
      name: "left_foot_load",
      key: true,
      pelvis: { pos: { x: -0.6, y: 171.0, z: 64.0 }, rot: { pitch: -7.0, yaw: -1.6, roll: 0.7 } },
      chest: { rot: { pitch: -8.0, yaw: -2.0, roll: 0.5 } },
      head: { rot: { pitch: 5.0, yaw: -0.4, roll: 0.0 }, lookAt: { x: 0.0, y: 138.0, z: 94.0 } },
      leftArm: { shoulder: { pitch: 50.0, yaw: 5.0, roll: 8.0 }, elbow: { bend: 124.0 }, wrist: { pitch: 16.0 } },
      rightArm: { shoulder: { pitch: 56.0, yaw: -6.0, roll: -8.0 }, elbow: { bend: 122.0 }, wrist: { pitch: 16.0 } },
      leftFoot: { ankle: { x: 8.6, y: 124.3, z: 53.0 }, ball: { x: 9.5, y: 117.2, z: 68.0 }, rot: { pitch: -4.0, yaw: 1.0, roll: 0.0 }, weight: 0.58 },
      rightFoot: { ankle: { x: -7.0, y: 88.0, z: 40.0 }, ball: { x: -8.0, y: 92.0, z: 55.0 }, rot: { pitch: 54.0, yaw: 0.0, roll: 0.0 }, weight: 0.0 },
      leftHand: { pos: { x: 10.5, y: 116.0, z: 38.5 }, rot: { pitch: 18.0, yaw: 4.0, roll: 10.0 }, grip: 1.0, weight: 0.9 },
      rightHand: { pos: { x: -10.5, y: 116.0, z: 38.5 }, rot: { pitch: 18.0, yaw: -4.0, roll: -10.0 }, grip: 1.0, weight: 0.95 },
      contacts: { leftFoot: "top", rightFoot: "air", leftHand: "top", rightHand: "top" }
    },
    {
      frame: 15,
      name: "mantle_push",
      key: false,
      pelvis: { pos: { x: -0.2, y: 181.0, z: 69.0 }, rot: { pitch: -3.0, yaw: -1.0, roll: 0.2 } },
      chest: { rot: { pitch: -4.0, yaw: -1.0, roll: 0.2 } },
      head: { rot: { pitch: 3.0, yaw: -0.2, roll: 0.0 }, lookAt: { x: 0.0, y: 150.0, z: 100.0 } },
      leftArm: { shoulder: { pitch: 38.0, yaw: 5.0, roll: 7.0 }, elbow: { bend: 116.0 }, wrist: { pitch: 18.0 } },
      rightArm: { shoulder: { pitch: 44.0, yaw: -5.0, roll: -7.0 }, elbow: { bend: 116.0 }, wrist: { pitch: 18.0 } },
      leftFoot: { ankle: { x: 8.6, y: 124.3, z: 53.0 }, ball: { x: 9.5, y: 117.2, z: 68.0 }, rot: { pitch: -5.0, yaw: 1.0, roll: 0.0 }, weight: 0.78 },
      rightFoot: { ankle: { x: -8.6, y: 113.0, z: 53.0 }, ball: { x: -9.6, y: 117.2, z: 66.0 }, rot: { pitch: 18.0, yaw: -1.0, roll: 0.0 }, weight: 0.18 },
      leftHand: { pos: { x: 10.5, y: 116.0, z: 38.5 }, rot: { pitch: 20.0, yaw: 4.0, roll: 10.0 }, grip: 0.85, weight: 0.72 },
      rightHand: { pos: { x: -10.5, y: 116.0, z: 38.5 }, rot: { pitch: 20.0, yaw: -4.0, roll: -10.0 }, grip: 0.9, weight: 0.8 },
      contacts: { leftFoot: "top", rightFoot: "top", leftHand: "top", rightHand: "top" }
    },
    {
      frame: 16,
      name: "right_foot_catches_top",
      key: true,
      pelvis: { pos: { x: 0.2, y: 190.0, z: 73.0 }, rot: { pitch: 1.0, yaw: -0.2, roll: -0.3 } },
      chest: { rot: { pitch: 0.0, yaw: -0.3, roll: -0.2 } },
      head: { rot: { pitch: 0.0, yaw: 0.0, roll: 0.0 }, lookAt: { x: 0.0, y: 164.0, z: 106.0 } },
      leftArm: { shoulder: { pitch: 28.0, yaw: 4.0, roll: 6.0 }, elbow: { bend: 100.0 }, wrist: { pitch: 16.0 } },
      rightArm: { shoulder: { pitch: 32.0, yaw: -4.0, roll: -6.0 }, elbow: { bend: 100.0 }, wrist: { pitch: 16.0 } },
      leftFoot: { ankle: { x: 8.6, y: 124.3, z: 53.0 }, ball: { x: 9.5, y: 117.2, z: 68.0 }, rot: { pitch: -4.0, yaw: 1.0, roll: 0.0 }, weight: 0.72 },
      rightFoot: { ankle: { x: -8.6, y: 124.3, z: 54.0 }, ball: { x: -9.6, y: 117.2, z: 69.0 }, rot: { pitch: -2.0, yaw: -1.0, roll: 0.0 }, weight: 0.58 },
      leftHand: { pos: { x: 10.5, y: 116.0, z: 38.5 }, rot: { pitch: 18.0, yaw: 4.0, roll: 10.0 }, grip: 0.62, weight: 0.48 },
      rightHand: { pos: { x: -10.5, y: 116.0, z: 38.5 }, rot: { pitch: 18.0, yaw: -4.0, roll: -10.0 }, grip: 0.66, weight: 0.5 },
      contacts: { leftFoot: "top", rightFoot: "top", leftHand: "top", rightHand: "top" }
    },
    {
      frame: 17,
      name: "hands_release_start",
      key: false,
      pelvis: { pos: { x: 0.4, y: 197.0, z: 77.0 }, rot: { pitch: 3.0, yaw: 0.2, roll: -0.5 } },
      chest: { rot: { pitch: 2.0, yaw: 0.3, roll: -0.4 } },
      head: { rot: { pitch: -1.0, yaw: 0.0, roll: 0.0 }, lookAt: { x: 0.0, y: 174.0, z: 110.0 } },
      leftArm: { shoulder: { pitch: 18.0, yaw: 3.0, roll: 5.0 }, elbow: { bend: 90.0 }, wrist: { pitch: 12.0 } },
      rightArm: { shoulder: { pitch: 20.0, yaw: -3.0, roll: -5.0 }, elbow: { bend: 90.0 }, wrist: { pitch: 12.0 } },
      leftFoot: { ankle: { x: 8.6, y: 124.3, z: 56.0 }, ball: { x: 9.5, y: 117.2, z: 71.0 }, rot: { pitch: -3.0, yaw: 1.0, roll: 0.0 }, weight: 0.62 },
      rightFoot: { ankle: { x: -8.6, y: 124.3, z: 58.0 }, ball: { x: -9.6, y: 117.2, z: 73.0 }, rot: { pitch: -2.0, yaw: -1.0, roll: 0.0 }, weight: 0.74 },
      leftHand: { pos: { x: 13.0, y: 122.0, z: 47.0 }, rot: { pitch: 12.0, yaw: 4.0, roll: 10.0 }, grip: 0.35, weight: 0.2 },
      rightHand: { pos: { x: -13.0, y: 122.0, z: 47.0 }, rot: { pitch: 12.0, yaw: -4.0, roll: -10.0 }, grip: 0.35, weight: 0.2 },
      contacts: { leftFoot: "top", rightFoot: "top", leftHand: "air", rightHand: "air" }
    },
    {
      frame: 18,
      name: "rise_from_crouch",
      key: true,
      pelvis: { pos: { x: 0.3, y: 202.0, z: 80.0 }, rot: { pitch: 4.0, yaw: 0.5, roll: -0.3 } },
      chest: { rot: { pitch: 3.0, yaw: 0.6, roll: -0.2 } },
      head: { rot: { pitch: -2.0, yaw: 0.2, roll: 0.0 }, lookAt: { x: 0.0, y: 184.0, z: 114.0 } },
      leftArm: { shoulder: { pitch: 8.0, yaw: 3.0, roll: 4.0 }, elbow: { bend: 78.0 }, wrist: { pitch: 8.0 } },
      rightArm: { shoulder: { pitch: 10.0, yaw: -3.0, roll: -4.0 }, elbow: { bend: 78.0 }, wrist: { pitch: 8.0 } },
      leftFoot: { ankle: { x: 8.6, y: 124.3, z: 60.0 }, ball: { x: 9.5, y: 117.2, z: 75.0 }, rot: { pitch: -2.0, yaw: 1.0, roll: 0.0 }, weight: 0.6 },
      rightFoot: { ankle: { x: -8.6, y: 124.3, z: 62.0 }, ball: { x: -9.6, y: 117.2, z: 77.0 }, rot: { pitch: -2.0, yaw: -1.0, roll: 0.0 }, weight: 0.82 },
      leftHand: { pos: { x: 17.0, y: 135.0, z: 59.0 }, rot: { pitch: 4.0, yaw: 5.0, roll: 12.0 }, grip: 0.0, weight: 0.0 },
      rightHand: { pos: { x: -17.0, y: 135.0, z: 59.0 }, rot: { pitch: 4.0, yaw: -5.0, roll: -12.0 }, grip: 0.0, weight: 0.0 },
      contacts: { leftFoot: "top", rightFoot: "top", leftHand: "air", rightHand: "air" }
    },
    {
      frame: 19,
      name: "weight_settle",
      key: false,
      pelvis: { pos: { x: 0.1, y: 206.0, z: 82.0 }, rot: { pitch: 2.5, yaw: 0.4, roll: -0.1 } },
      chest: { rot: { pitch: 1.6, yaw: 0.4, roll: 0.0 } },
      head: { rot: { pitch: -1.2, yaw: 0.2, roll: 0.0 }, lookAt: { x: 0.0, y: 190.0, z: 116.0 } },
      leftArm: { shoulder: { pitch: 0.0, yaw: 2.0, roll: 3.0 }, elbow: { bend: 68.0 }, wrist: { pitch: 4.0 } },
      rightArm: { shoulder: { pitch: 1.0, yaw: -2.0, roll: -3.0 }, elbow: { bend: 68.0 }, wrist: { pitch: 4.0 } },
      leftFoot: { ankle: { x: 8.8, y: 124.3, z: 63.0 }, ball: { x: 9.6, y: 117.2, z: 78.0 }, rot: { pitch: -1.0, yaw: 0.5, roll: 0.0 }, weight: 0.54 },
      rightFoot: { ankle: { x: -8.8, y: 124.3, z: 64.0 }, ball: { x: -9.6, y: 117.2, z: 79.0 }, rot: { pitch: -1.0, yaw: -0.5, roll: 0.0 }, weight: 0.86 },
      leftHand: { pos: { x: 18.0, y: 146.0, z: 66.0 }, rot: { pitch: 0.0, yaw: 5.0, roll: 12.0 }, grip: 0.0, weight: 0.0 },
      rightHand: { pos: { x: -18.0, y: 146.0, z: 66.0 }, rot: { pitch: 0.0, yaw: -5.0, roll: -12.0 }, grip: 0.0, weight: 0.0 },
      contacts: { leftFoot: "top", rightFoot: "top", leftHand: "air", rightHand: "air" }
    },
    {
      frame: 20,
      name: "stand_up",
      key: true,
      pelvis: { pos: { x: 0.0, y: 209.8, z: 84.0 }, rot: { pitch: 0.8, yaw: 0.1, roll: 0.0 } },
      chest: { rot: { pitch: 0.2, yaw: 0.1, roll: 0.0 } },
      head: { rot: { pitch: -0.4, yaw: 0.0, roll: 0.0 }, lookAt: { x: 0.0, y: 194.0, z: 120.0 } },
      leftArm: { shoulder: { pitch: -7.0, yaw: 1.5, roll: 2.0 }, elbow: { bend: 56.0 }, wrist: { pitch: 0.0 } },
      rightArm: { shoulder: { pitch: -6.0, yaw: -1.5, roll: -2.0 }, elbow: { bend: 56.0 }, wrist: { pitch: 0.0 } },
      leftFoot: { ankle: { x: 9.0, y: 124.3, z: 66.0 }, ball: { x: 9.8, y: 117.2, z: 81.0 }, rot: { pitch: 0.0, yaw: 0.0, roll: 0.0 }, weight: 0.52 },
      rightFoot: { ankle: { x: -9.0, y: 124.3, z: 66.0 }, ball: { x: -9.8, y: 117.2, z: 81.0 }, rot: { pitch: 0.0, yaw: 0.0, roll: 0.0 }, weight: 0.82 },
      leftHand: { pos: { x: 18.0, y: 157.0, z: 72.0 }, rot: { pitch: -5.0, yaw: 4.0, roll: 10.0 }, grip: 0.0, weight: 0.0 },
      rightHand: { pos: { x: -18.0, y: 157.0, z: 72.0 }, rot: { pitch: -5.0, yaw: -4.0, roll: -10.0 }, grip: 0.0, weight: 0.0 },
      contacts: { leftFoot: "top", rightFoot: "top", leftHand: "air", rightHand: "air" }
    },
    {
      frame: 21,
      name: "recover_balance",
      key: false,
      pelvis: { pos: { x: 0.0, y: 211.0, z: 84.5 }, rot: { pitch: 0.0, yaw: 0.0, roll: 0.0 } },
      chest: { rot: { pitch: -0.3, yaw: 0.0, roll: 0.0 } },
      head: { rot: { pitch: 0.0, yaw: 0.0, roll: 0.0 }, lookAt: { x: 0.0, y: 196.0, z: 121.0 } },
      leftArm: { shoulder: { pitch: -11.0, yaw: 1.0, roll: 1.5 }, elbow: { bend: 48.0 }, wrist: { pitch: -2.0 } },
      rightArm: { shoulder: { pitch: -10.0, yaw: -1.0, roll: -1.5 }, elbow: { bend: 48.0 }, wrist: { pitch: -2.0 } },
      leftFoot: { ankle: { x: 9.0, y: 124.3, z: 66.0 }, ball: { x: 9.8, y: 117.2, z: 81.0 }, rot: { pitch: 0.0, yaw: 0.0, roll: 0.0 }, weight: 0.5 },
      rightFoot: { ankle: { x: -9.0, y: 124.3, z: 66.0 }, ball: { x: -9.8, y: 117.2, z: 81.0 }, rot: { pitch: 0.0, yaw: 0.0, roll: 0.0 }, weight: 0.82 },
      leftHand: { pos: { x: 17.0, y: 150.0, z: 75.0 }, rot: { pitch: -8.0, yaw: 3.0, roll: 8.0 }, grip: 0.0, weight: 0.0 },
      rightHand: { pos: { x: -17.0, y: 150.0, z: 75.0 }, rot: { pitch: -8.0, yaw: -3.0, roll: -8.0 }, grip: 0.0, weight: 0.0 },
      contacts: { leftFoot: "top", rightFoot: "top", leftHand: "air", rightHand: "air" }
    },
    {
      frame: 22,
      name: "final_settle",
      key: false,
      pelvis: { pos: { x: 0.0, y: 210.6, z: 84.2 }, rot: { pitch: 0.0, yaw: 0.0, roll: 0.0 } },
      chest: { rot: { pitch: 0.0, yaw: 0.0, roll: 0.0 } },
      head: { rot: { pitch: 0.0, yaw: 0.0, roll: 0.0 }, lookAt: { x: 0.0, y: 196.0, z: 121.0 } },
      leftArm: { shoulder: { pitch: -13.0, yaw: 0.6, roll: 1.0 }, elbow: { bend: 42.0 }, wrist: { pitch: -4.0 } },
      rightArm: { shoulder: { pitch: -13.0, yaw: -0.6, roll: -1.0 }, elbow: { bend: 42.0 }, wrist: { pitch: -4.0 } },
      leftFoot: { ankle: { x: 9.0, y: 124.3, z: 66.0 }, ball: { x: 9.8, y: 117.2, z: 81.0 }, rot: { pitch: 0.0, yaw: 0.0, roll: 0.0 }, weight: 0.5 },
      rightFoot: { ankle: { x: -9.0, y: 124.3, z: 66.0 }, ball: { x: -9.8, y: 117.2, z: 81.0 }, rot: { pitch: 0.0, yaw: 0.0, roll: 0.0 }, weight: 0.78 },
      leftHand: { pos: { x: 16.0, y: 139.0, z: 76.0 }, rot: { pitch: -10.0, yaw: 2.0, roll: 6.0 }, grip: 0.0, weight: 0.0 },
      rightHand: { pos: { x: -16.0, y: 139.0, z: 76.0 }, rot: { pitch: -10.0, yaw: -2.0, roll: -6.0 }, grip: 0.0, weight: 0.0 },
      contacts: { leftFoot: "top", rightFoot: "top", leftHand: "air", rightHand: "air" }
    },
    {
      frame: 23,
      name: "standing_on_top",
      key: true,
      pelvis: { pos: { x: 0.0, y: 210.2, z: 84.0 }, rot: { pitch: 0.0, yaw: 0.0, roll: 0.0 } },
      chest: { rot: { pitch: 0.0, yaw: 0.0, roll: 0.0 } },
      head: { rot: { pitch: 0.0, yaw: 0.0, roll: 0.0 }, lookAt: { x: 0.0, y: 196.0, z: 121.0 } },
      leftArm: { shoulder: { pitch: -14.0, yaw: 0.0, roll: 0.0 }, elbow: { bend: 38.0 }, wrist: { pitch: -5.0 } },
      rightArm: { shoulder: { pitch: -14.0, yaw: 0.0, roll: 0.0 }, elbow: { bend: 38.0 }, wrist: { pitch: -5.0 } },
      leftFoot: { ankle: { x: 9.0, y: 124.3, z: 66.0 }, ball: { x: 9.8, y: 117.2, z: 81.0 }, rot: { pitch: 0.0, yaw: 0.0, roll: 0.0 }, weight: 0.5 },
      rightFoot: { ankle: { x: -9.0, y: 124.3, z: 66.0 }, ball: { x: -9.8, y: 117.2, z: 81.0 }, rot: { pitch: 0.0, yaw: 0.0, roll: 0.0 }, weight: 0.5 },
      leftHand: { pos: { x: 15.0, y: 132.0, z: 75.0 }, rot: { pitch: -12.0, yaw: 0.0, roll: 4.0 }, grip: 0.0, weight: 0.0 },
      rightHand: { pos: { x: -15.0, y: 132.0, z: 75.0 }, rot: { pitch: -12.0, yaw: 0.0, roll: -4.0 }, grip: 0.0, weight: 0.0 },
      contacts: { leftFoot: "top", rightFoot: "top", leftHand: "air", rightHand: "air" }
    }
  ]
};

(function normalizeWallClimbRendererTargets(data) {
  if (!data || !Array.isArray(data.frames)) {
    return;
  }

  const degToRad = Math.PI / 180;
  const armReach = 58.0;
  const legReach = 82.0;

  function vec(x, y, z) {
    return { x: Number(x) || 0, y: Number(y) || 0, z: Number(z) || 0 };
  }

  function add(a, b) {
    return vec(a.x + b.x, a.y + b.y, a.z + b.z);
  }

  function sub(a, b) {
    return vec(a.x - b.x, a.y - b.y, a.z - b.z);
  }

  function mul(a, scale) {
    return vec(a.x * scale, a.y * scale, a.z * scale);
  }

  function len(a) {
    return Math.hypot(a.x, a.y, a.z);
  }

  function norm(a) {
    const length = len(a) || 1;
    return mul(a, 1 / length);
  }

  function arr(a) {
    return [
      Number(a.x.toFixed(3)),
      Number(a.y.toFixed(3)),
      Number(a.z.toFixed(3))
    ];
  }

  function rotate(offset, rot) {
    const yaw = (rot?.yaw || 0) * degToRad;
    const pitch = (rot?.pitch || 0) * degToRad;
    const roll = (rot?.roll || 0) * degToRad;
    const cy = Math.cos(yaw);
    const sy = Math.sin(yaw);
    const cp = Math.cos(pitch);
    const sp = Math.sin(pitch);
    const cr = Math.cos(roll);
    const sr = Math.sin(roll);

    const py = offset.y * cp - offset.z * sp;
    const pz = offset.y * sp + offset.z * cp;
    const rx = offset.x * cr - py * sr;
    const ry = offset.x * sr + py * cr;

    return vec(
      rx * cy + pz * sy,
      ry,
      -rx * sy + pz * cy
    );
  }

  function clampTarget(origin, target, maxDistance) {
    const delta = sub(target, origin);
    const distance = len(delta);
    if (distance <= maxDistance) {
      return target;
    }
    return add(origin, mul(delta, maxDistance / distance));
  }

  function solveElbow(shoulder, hand, side) {
    const upper = 31.5;
    const lower = 29.5;
    const delta = sub(hand, shoulder);
    const distance = Math.max(1, Math.min(len(delta), upper + lower - 0.25));
    const direction = norm(delta);
    const along = (upper * upper - lower * lower + distance * distance) / (2 * distance);
    const height = Math.sqrt(Math.max(0, upper * upper - along * along));
    const bend = norm(vec(side * 0.82, -0.35, -direction.z * 0.45));
    return add(add(shoulder, mul(direction, along)), mul(bend, height * 0.62));
  }

  function solveKnee(hip, ankle, side) {
    const upper = 43.3;
    const lower = 42.2;
    const delta = sub(ankle, hip);
    const distance = Math.max(1, Math.min(len(delta), upper + lower - 0.25));
    const direction = norm(delta);
    const along = (upper * upper - lower * lower + distance * distance) / (2 * distance);
    const height = Math.sqrt(Math.max(0, upper * upper - along * along));
    let bend = norm(vec(0, -direction.z, direction.y));
    if (bend.z < 0) {
      bend = mul(bend, -1);
    }
    return add(add(add(hip, mul(direction, along)), mul(bend, height * 0.72)), vec(side * 0.8, 0, 0));
  }

  function isLocked(contact, weight) {
    return contact && contact !== "air" && Number(weight || 0) >= 0.25;
  }

  data.frames.forEach((frame) => {
    const pelvis = vec(frame.pelvis.pos.x, frame.pelvis.pos.y, frame.pelvis.pos.z);
    const pelvisRot = frame.pelvis.rot || {};
    const chestRot = frame.chest?.rot || {};
    const neckRot = { pitch: (frame.head?.rot?.pitch || 0) * 0.35, yaw: (frame.head?.rot?.yaw || 0) * 0.35, roll: 0 };
    const headRot = frame.head?.rot || {};
    const points = {};

    points.pelvis = pelvis;
    points.spine_01 = add(pelvis, rotate(vec(0, 12.4, 1.2), pelvisRot));
    points.spine_02 = add(points.spine_01, rotate(vec(0.1, 12.8, 1.4), pelvisRot));
    points.spine_03 = add(points.spine_02, rotate(vec(0.2, 12.8, 1.5), chestRot));
    points.spine_04 = add(points.spine_03, rotate(vec(0.4, 10.0, 1.2), chestRot));
    points.spine_05 = add(points.spine_04, rotate(vec(0.5, 9.7, 0.9), chestRot));
    points.neck_01 = add(add(points.spine_05, rotate(vec(0.2, 12.2, 0.4), chestRot)), rotate(vec(0, 0.5, 0.1), neckRot));
    points.head = add(points.neck_01, rotate(vec(0.1, 16.2, 0.3), headRot));
    points.clavicle_l = add(points.spine_05, rotate(vec(17.4, 2.8, 1.1), chestRot));
    points.clavicle_r = add(points.spine_05, rotate(vec(-17.4, 2.8, -1.1), chestRot));

    [
      ["l", "left", 1, points.clavicle_l],
      ["r", "right", -1, points.clavicle_r]
    ].forEach(([shortName, sideName, side, clavicle]) => {
      const handTarget = frame[`${sideName}Hand`]?.pos || add(clavicle, rotate(vec(side * 24, -37, -1), chestRot));
      const hand = clampTarget(clavicle, vec(handTarget.x, handTarget.y, handTarget.z), armReach);
      const elbow = solveElbow(clavicle, hand, side);
      points[`upperarm_${shortName}`] = add(clavicle, mul(sub(elbow, clavicle), 0.48));
      points[`lowerarm_${shortName}`] = elbow;
      points[`hand_${shortName}`] = hand;
    });

    points.thigh_l = add(pelvis, rotate(vec(10.1, -3.8, 1.2), pelvisRot));
    points.thigh_r = add(pelvis, rotate(vec(-10.1, -3.8, -1.2), pelvisRot));

    [
      ["l", "left", 1, points.thigh_l],
      ["r", "right", -1, points.thigh_r]
    ].forEach(([shortName, sideName, side, thigh]) => {
      const foot = frame[`${sideName}Foot`];
      const ankle = clampTarget(thigh, vec(foot.ankle.x, foot.ankle.y, foot.ankle.z), legReach);
      const sourceBall = vec(foot.ball.x, foot.ball.y, foot.ball.z);
      const ankleToBall = clampTarget(ankle, sourceBall, 17.2);
      points[`foot_${shortName}`] = ankle;
      points[`ball_${shortName}`] = ankleToBall;
      points[`calf_${shortName}`] = solveKnee(thigh, ankle, side);
    });

    frame.points = Object.fromEntries(Object.entries(points).map(([name, point]) => [name, arr(point)]));

    const contactTargets = {};
    if (isLocked(frame.contacts?.leftHand, frame.leftHand?.weight)) {
      contactTargets.hand_l = frame.points.hand_l;
    }
    if (isLocked(frame.contacts?.rightHand, frame.rightHand?.weight)) {
      contactTargets.hand_r = frame.points.hand_r;
    }
    if (isLocked(frame.contacts?.leftFoot, frame.leftFoot?.weight)) {
      contactTargets.foot_l = frame.points.foot_l;
      contactTargets.ball_l = frame.points.ball_l;
    }
    if (isLocked(frame.contacts?.rightFoot, frame.rightFoot?.weight)) {
      contactTargets.foot_r = frame.points.foot_r;
      contactTargets.ball_r = frame.points.ball_r;
    }
    frame.contactTargets = contactTargets;
  });
})(window.mannyWallClimbKeyposes);
