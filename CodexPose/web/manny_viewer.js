(() => {
      const canvas = document.getElementById("walkCanvas");
      const fallback = document.getElementById("fallback");
      const languageToggle = document.getElementById("languageToggle");
      const modeSelect = document.getElementById("walkModeSelect");
      const walkModeDescription = document.getElementById("walkModeDescription");
      const gizmoButtons = Array.from(document.querySelectorAll("[data-view]"));
      const naturalRunSource = window.mannyNaturalRunKeyposes || {};
      const naturalRunRawKeys = selectNaturalRunKeys(naturalRunSource);
      const naturalRunFallbackFrameCount = naturalRunRawKeys.length
        ? naturalRunRawKeys.reduce((maxFrame, key, index) => Math.max(maxFrame, readNaturalKeyFrame(key, index)), 0) + 1
        : 24;
      const naturalRunFrameCount = Math.max(2, Math.round(readNumberOption(
        naturalRunSource,
        ["frameCount", "frame_count", "totalFrames", "durationFrames", "numFrames", "config.frameCount", "meta.frameCount"],
        naturalRunFallbackFrameCount
      )));
      const naturalRunFps = Math.max(1, readNumberOption(
        naturalRunSource,
        ["fps", "frameRate", "frame_rate", "config.fps", "config.frameRate", "meta.fps", "meta.frameRate"],
        30
      ));
      const wallPeekSource = window.mannyWallPeekKeyposes || {};
      const wallPeekRawKeys = selectWallPeekKeys(wallPeekSource);
      const wallPeekFallbackFrameCount = wallPeekRawKeys.length
        ? wallPeekRawKeys.reduce((maxFrame, key, index) => Math.max(maxFrame, readNaturalKeyFrame(key, index)), 0) + 1
        : 24;
      const wallPeekFrameCount = Math.max(2, Math.round(readNumberOption(
        wallPeekSource,
        ["frameCount", "frame_count", "totalFrames", "durationFrames", "numFrames", "config.frameCount", "meta.frameCount"],
        wallPeekFallbackFrameCount
      )));
      const wallPeekFps = Math.max(1, readNumberOption(
        wallPeekSource,
        ["fps", "frameRate", "frame_rate", "config.fps", "config.frameRate", "meta.fps", "meta.frameRate"],
        24
      ));
      const wallClimbSource = window.mannyWallClimbKeyposes || {};
      const wallClimbRawKeys = selectWallClimbKeys(wallClimbSource);
      const wallClimbFallbackFrameCount = wallClimbRawKeys.length
        ? wallClimbRawKeys.reduce((maxFrame, key, index) => Math.max(maxFrame, readNaturalKeyFrame(key, index)), 0) + 1
        : 24;
      const wallClimbFrameCount = Math.max(2, Math.round(readNumberOption(
        wallClimbSource,
        ["frameCount", "frame_count", "totalFrames", "durationFrames", "numFrames", "config.frameCount", "meta.frameCount"],
        wallClimbFallbackFrameCount
      )));
      const wallClimbFps = Math.max(1, readNumberOption(
        wallClimbSource,
        ["fps", "frameRate", "frame_rate", "config.fps", "config.frameRate", "meta.fps", "meta.frameRate"],
        24
      ));
      const proneCrawlSource = window.mannyProneCrawlKeyposes || {};
      const proneCrawlRawKeys = selectProneCrawlKeys(proneCrawlSource);
      const proneCrawlFallbackFrameCount = proneCrawlRawKeys.length
        ? proneCrawlRawKeys.reduce((maxFrame, key, index) => Math.max(maxFrame, readNaturalKeyFrame(key, index)), 0) + 1
        : 24;
      const proneCrawlFrameCount = Math.max(2, Math.round(readNumberOption(
        proneCrawlSource,
        ["frameCount", "frame_count", "totalFrames", "durationFrames", "numFrames", "config.frameCount", "meta.frameCount"],
        proneCrawlFallbackFrameCount
      )));
      const proneCrawlFps = Math.max(1, readNumberOption(
        proneCrawlSource,
        ["fps", "frameRate", "frame_rate", "config.fps", "config.frameRate", "meta.fps", "meta.frameRate"],
        24
      ));
      const shuffleDanceSource = window.mannyShuffleDanceKeyposes || {};
      const shuffleDanceRawKeys = selectShuffleDanceKeys(shuffleDanceSource);
      const shuffleDanceFallbackFrameCount = shuffleDanceRawKeys.length
        ? shuffleDanceRawKeys.reduce((maxFrame, key, index) => Math.max(maxFrame, readNaturalKeyFrame(key, index)), 0) + 1
        : 24;
      const shuffleDanceFrameCount = Math.max(2, Math.round(readNumberOption(
        shuffleDanceSource,
        ["frameCount", "frame_count", "totalFrames", "durationFrames", "numFrames", "config.frameCount", "meta.frameCount"],
        shuffleDanceFallbackFrameCount
      )));
      const shuffleDanceFps = Math.max(1, readNumberOption(
        shuffleDanceSource,
        ["fps", "frameRate", "frame_rate", "config.fps", "config.frameRate", "meta.fps", "meta.frameRate"],
        24
      ));
      const longDanceSource = window.mannyLongDanceKeyposes || {};
      const longDanceRawKeys = selectShuffleDanceKeys(longDanceSource);
      const longDanceFallbackFrameCount = longDanceRawKeys.length
        ? longDanceRawKeys.reduce((maxFrame, key, index) => Math.max(maxFrame, readNaturalKeyFrame(key, index)), 0) + 1
        : 240;
      const longDanceFrameCount = Math.max(2, Math.round(readNumberOption(
        longDanceSource,
        ["frameCount", "frame_count", "totalFrames", "durationFrames", "numFrames", "config.frameCount", "meta.frameCount"],
        longDanceFallbackFrameCount
      )));
      const longDanceFps = Math.max(1, readNumberOption(
        longDanceSource,
        ["fps", "frameRate", "frame_rate", "config.fps", "config.frameRate", "meta.fps", "meta.frameRate"],
        24
      ));
      const playfulGirlWalkSource = window.mannyPlayfulGirlWalkKeyposes || {};
      const playfulGirlWalkRawKeys = selectShuffleDanceKeys(playfulGirlWalkSource);
      const playfulGirlWalkFallbackFrameCount = playfulGirlWalkRawKeys.length
        ? playfulGirlWalkRawKeys.reduce((maxFrame, key, index) => Math.max(maxFrame, readNaturalKeyFrame(key, index)), 0) + 1
        : 48;
      const playfulGirlWalkFrameCount = Math.max(2, Math.round(readNumberOption(
        playfulGirlWalkSource,
        ["frameCount", "frame_count", "totalFrames", "durationFrames", "numFrames", "config.frameCount", "meta.frameCount"],
        playfulGirlWalkFallbackFrameCount
      )));
      const playfulGirlWalkFps = Math.max(1, readNumberOption(
        playfulGirlWalkSource,
        ["fps", "frameRate", "frame_rate", "config.fps", "config.frameRate", "meta.fps", "meta.frameRate"],
        24
      ));
      const boxOverheadLiftSource = window.mannyBoxOverheadLiftKeyposes || {};
      const boxOverheadLiftRawKeys = selectShuffleDanceKeys(boxOverheadLiftSource);
      const boxOverheadLiftFallbackFrameCount = boxOverheadLiftRawKeys.length
        ? boxOverheadLiftRawKeys.reduce((maxFrame, key, index) => Math.max(maxFrame, readNaturalKeyFrame(key, index)), 0) + 1
        : 40;
      const boxOverheadLiftFrameCount = Math.max(2, Math.round(readNumberOption(
        boxOverheadLiftSource,
        ["frameCount", "frame_count", "totalFrames", "durationFrames", "numFrames", "config.frameCount", "meta.frameCount"],
        boxOverheadLiftFallbackFrameCount
      )));
      const boxOverheadLiftFps = Math.max(1, readNumberOption(
        boxOverheadLiftSource,
        ["fps", "frameRate", "frame_rate", "config.fps", "config.frameRate", "meta.fps", "meta.frameRate"],
        24
      ));
      const hadokenSource = window.mannyHadokenKeyposes || {};
      const hadokenRawKeys = selectShuffleDanceKeys(hadokenSource);
      const hadokenFallbackFrameCount = hadokenRawKeys.length
        ? hadokenRawKeys.reduce((maxFrame, key, index) => Math.max(maxFrame, readNaturalKeyFrame(key, index)), 0) + 1
        : 40;
      const hadokenFrameCount = Math.max(2, Math.round(readNumberOption(
        hadokenSource,
        ["frameCount", "frame_count", "totalFrames", "durationFrames", "numFrames", "config.frameCount", "meta.frameCount"],
        hadokenFallbackFrameCount
      )));
      const hadokenFps = Math.max(1, readNumberOption(
        hadokenSource,
        ["fps", "frameRate", "frame_rate", "config.fps", "config.frameRate", "meta.fps", "meta.frameRate"],
        24
      ));
      const heroLandingPoseSource = window.mannyHeroLandingPoseKeyposes || {};
      const heroLandingPoseRawKeys = selectShuffleDanceKeys(heroLandingPoseSource);
      const heroLandingPoseFallbackFrameCount = heroLandingPoseRawKeys.length
        ? heroLandingPoseRawKeys.reduce((maxFrame, key, index) => Math.max(maxFrame, readNaturalKeyFrame(key, index)), 0) + 1
        : 1;
      const heroLandingPoseFrameCount = Math.max(1, Math.round(readNumberOption(
        heroLandingPoseSource,
        ["frameCount", "frame_count", "totalFrames", "durationFrames", "numFrames", "config.frameCount", "meta.frameCount"],
        heroLandingPoseFallbackFrameCount
      )));
      const heroLandingPoseFps = Math.max(1, readNumberOption(
        heroLandingPoseSource,
        ["fps", "frameRate", "frame_rate", "config.fps", "config.frameRate", "meta.fps", "meta.frameRate"],
        1
      ));
      const baseballBatSwingSource = window.mannyBaseballBatSwingKeyposes || {};
      const baseballBatSwingRawKeys = selectShuffleDanceKeys(baseballBatSwingSource);
      const baseballBatSwingFallbackFrameCount = baseballBatSwingRawKeys.length
        ? baseballBatSwingRawKeys.reduce((maxFrame, key, index) => Math.max(maxFrame, readNaturalKeyFrame(key, index)), 0) + 1
        : 36;
      const baseballBatSwingFrameCount = Math.max(2, Math.round(readNumberOption(
        baseballBatSwingSource,
        ["frameCount", "frame_count", "totalFrames", "durationFrames", "numFrames", "config.frameCount", "meta.frameCount"],
        baseballBatSwingFallbackFrameCount
      )));
      const baseballBatSwingFps = Math.max(1, readNumberOption(
        baseballBatSwingSource,
        ["fps", "frameRate", "frame_rate", "config.fps", "config.frameRate", "meta.fps", "meta.frameRate"],
        24
      ));
      const seatedBottleDrinkSource = window.mannySeatedBottleDrinkKeyposes || {};
      const seatedBottleDrinkRawKeys = selectShuffleDanceKeys(seatedBottleDrinkSource);
      const seatedBottleDrinkFallbackFrameCount = seatedBottleDrinkRawKeys.length
        ? seatedBottleDrinkRawKeys.reduce((maxFrame, key, index) => Math.max(maxFrame, readNaturalKeyFrame(key, index)), 0) + 1
        : 72;
      const seatedBottleDrinkFrameCount = Math.max(2, Math.round(readNumberOption(
        seatedBottleDrinkSource,
        ["frameCount", "frame_count", "totalFrames", "durationFrames", "numFrames", "config.frameCount", "meta.frameCount"],
        seatedBottleDrinkFallbackFrameCount
      )));
      const seatedBottleDrinkFps = Math.max(1, readNumberOption(
        seatedBottleDrinkSource,
        ["fps", "frameRate", "frame_rate", "config.fps", "config.frameRate", "meta.fps", "meta.frameRate"],
        24
      ));
      const archeryFullDrawSource = window.mannyArcheryFullDrawKeyposes || {};
      const archeryFullDrawRawKeys = selectShuffleDanceKeys(archeryFullDrawSource);
      const archeryFullDrawFallbackFrameCount = archeryFullDrawRawKeys.length
        ? archeryFullDrawRawKeys.reduce((maxFrame, key, index) => Math.max(maxFrame, readNaturalKeyFrame(key, index)), 0) + 1
        : 48;
      const archeryFullDrawFrameCount = Math.max(2, Math.round(readNumberOption(
        archeryFullDrawSource,
        ["frameCount", "frame_count", "totalFrames", "durationFrames", "numFrames", "config.frameCount", "meta.frameCount"],
        archeryFullDrawFallbackFrameCount
      )));
      const archeryFullDrawFps = Math.max(1, readNumberOption(
        archeryFullDrawSource,
        ["fps", "frameRate", "frame_rate", "config.fps", "config.frameRate", "meta.fps", "meta.frameRate"],
        24
      ));
      const sideKickSource = window.mannySideKickKeyposes || {};
      const sideKickRawKeys = selectShuffleDanceKeys(sideKickSource);
      const sideKickFallbackFrameCount = sideKickRawKeys.length
        ? sideKickRawKeys.reduce((maxFrame, key, index) => Math.max(maxFrame, readNaturalKeyFrame(key, index)), 0) + 1
        : 32;
      const sideKickFrameCount = Math.max(2, Math.round(readNumberOption(
        sideKickSource,
        ["frameCount", "frame_count", "totalFrames", "durationFrames", "numFrames", "config.frameCount", "meta.frameCount"],
        sideKickFallbackFrameCount
      )));
      const sideKickFps = Math.max(1, readNumberOption(
        sideKickSource,
        ["fps", "frameRate", "frame_rate", "config.fps", "config.frameRate", "meta.fps", "meta.frameRate"],
        24
      ));
      const roundhouseKickSource = window.mannyRoundhouseKickKeyposes || {};
      const roundhouseKickRawKeys = selectShuffleDanceKeys(roundhouseKickSource);
      const roundhouseKickFallbackFrameCount = roundhouseKickRawKeys.length
        ? roundhouseKickRawKeys.reduce((maxFrame, key, index) => Math.max(maxFrame, readNaturalKeyFrame(key, index)), 0) + 1
        : 28;
      const roundhouseKickFrameCount = Math.max(2, Math.round(readNumberOption(
        roundhouseKickSource,
        ["frameCount", "frame_count", "totalFrames", "durationFrames", "numFrames", "config.frameCount", "meta.frameCount"],
        roundhouseKickFallbackFrameCount
      )));
      const roundhouseKickFps = Math.max(1, readNumberOption(
        roundhouseKickSource,
        ["fps", "frameRate", "frame_rate", "config.fps", "config.frameRate", "meta.fps", "meta.frameRate"],
        24
      ));
      const jumpingRoundhouseKickSource = window.mannyJumpingRoundhouseKickKeyposes || {};
      const jumpingRoundhouseKickRawKeys = selectShuffleDanceKeys(jumpingRoundhouseKickSource);
      const jumpingRoundhouseKickFallbackFrameCount = jumpingRoundhouseKickRawKeys.length
        ? jumpingRoundhouseKickRawKeys.reduce((maxFrame, key, index) => Math.max(maxFrame, readNaturalKeyFrame(key, index)), 0) + 1
        : 36;
      const jumpingRoundhouseKickFrameCount = Math.max(2, Math.round(readNumberOption(
        jumpingRoundhouseKickSource,
        ["frameCount", "frame_count", "totalFrames", "durationFrames", "numFrames", "config.frameCount", "meta.frameCount"],
        jumpingRoundhouseKickFallbackFrameCount
      )));
      const jumpingRoundhouseKickFps = Math.max(1, readNumberOption(
        jumpingRoundhouseKickSource,
        ["fps", "frameRate", "frame_rate", "config.fps", "config.frameRate", "meta.fps", "meta.frameRate"],
        24
      ));
      const lowVaultSource = window.mannyLowVaultKeyposes || {};
      const lowVaultMotion = readGenericExternalMotion(lowVaultSource, 44, 24);
      const forwardRollSource = window.mannyForwardRollKeyposes || {};
      const forwardRollMotion = readGenericExternalMotion(forwardRollSource, 48, 24);
      const stumbleRecoverySource = window.mannyStumbleRecoveryKeyposes || {};
      const stumbleRecoveryMotion = readGenericExternalMotion(stumbleRecoverySource, 44, 24);
      const boxingComboSource = window.mannyBoxingComboKeyposes || {};
      const boxingComboMotion = readGenericExternalMotion(boxingComboSource, 42, 24);
      const doorPushSource = window.mannyDoorPushKeyposes || {};
      const doorPushMotion = readGenericExternalMotion(doorPushSource, 56, 24);
      const floorPickupSource = window.mannyFloorPickupKeyposes || {};
      const floorPickupMotion = readGenericExternalMotion(floorPickupSource, 52, 24);
      const ladderClimbSource = window.mannyLadderClimbKeyposes || {};
      const ladderClimbMotion = readGenericExternalMotion(ladderClimbSource, 48, 24);
      const kneeSlideSource = window.mannyKneeSlideKeyposes || {};
      const kneeSlideMotion = readGenericExternalMotion(kneeSlideSource, 56, 24);
      const swordSlashSource = window.mannySwordSlashKeyposes || {};
      const swordSlashRawKeys = selectSwordSlashKeys(swordSlashSource);
      const swordSlashFallbackFrameCount = swordSlashRawKeys.length
        ? swordSlashRawKeys.reduce((maxFrame, key, index) => Math.max(maxFrame, readNaturalKeyFrame(key, index)), 0) + 1
        : 16;
      const swordSlashFrameCount = Math.max(2, Math.round(readNumberOption(
        swordSlashSource,
        ["frameCount", "frame_count", "totalFrames", "durationFrames", "numFrames", "config.frameCount", "meta.frameCount"],
        swordSlashFallbackFrameCount
      )));
      const swordSlashFps = Math.max(1, readNumberOption(
        swordSlashSource,
        ["fps", "frameRate", "frame_rate", "config.fps", "config.frameRate", "meta.fps", "meta.frameRate"],
        24
      ));
      const walkModes = {
        basic: { frameCount: 24, fps: 24, descriptionKey: "basicWalkDescription" },
        feminine: { frameCount: 24, fps: 24, descriptionKey: "feminineWalkDescription" },
        running: { frameCount: 16, fps: 32, descriptionKey: "runningDescription" },
        naturalRun: { frameCount: naturalRunFrameCount, fps: naturalRunFps, descriptionKey: "naturalRunDescription" },
        overheadWave: { frameCount: 24, fps: 24, descriptionKey: "overheadWaveDescription" },
        idleShift: { frameCount: 24, fps: 24, descriptionKey: "idleShiftDescription" },
        wallPeek: { frameCount: wallPeekFrameCount, fps: wallPeekFps, descriptionKey: "wallPeekDescription" },
        wallClimb: { frameCount: wallClimbFrameCount, fps: wallClimbFps, descriptionKey: "wallClimbDescription" },
        proneCrawl: { frameCount: proneCrawlFrameCount, fps: proneCrawlFps, descriptionKey: "proneCrawlDescription" },
        swordSlash: { frameCount: swordSlashFrameCount, fps: swordSlashFps, descriptionKey: "swordSlashDescription" },
        crouchTwerk: { frameCount: 16, fps: 24, descriptionKey: "crouchTwerkDescription" },
        shuffleDance: { frameCount: shuffleDanceFrameCount, fps: shuffleDanceFps, descriptionKey: "shuffleDanceDescription" },
        longDance: { frameCount: longDanceFrameCount, fps: longDanceFps, descriptionKey: "longDanceDescription" },
        playfulGirlWalk: { frameCount: playfulGirlWalkFrameCount, fps: playfulGirlWalkFps, descriptionKey: "playfulGirlWalkDescription" },
        boxOverheadLift: { frameCount: boxOverheadLiftFrameCount, fps: boxOverheadLiftFps, descriptionKey: "boxOverheadLiftDescription" },
        hadoken: { frameCount: hadokenFrameCount, fps: hadokenFps, descriptionKey: "hadokenDescription" },
        heroLandingPose: { frameCount: heroLandingPoseFrameCount, fps: heroLandingPoseFps, descriptionKey: "heroLandingPoseDescription" },
        baseballBatSwing: { frameCount: baseballBatSwingFrameCount, fps: baseballBatSwingFps, descriptionKey: "baseballBatSwingDescription" },
        seatedBottleDrink: { frameCount: seatedBottleDrinkFrameCount, fps: seatedBottleDrinkFps, descriptionKey: "seatedBottleDrinkDescription" },
        archeryFullDraw: { frameCount: archeryFullDrawFrameCount, fps: archeryFullDrawFps, descriptionKey: "archeryFullDrawDescription" },
        sideKick: { frameCount: sideKickFrameCount, fps: sideKickFps, descriptionKey: "sideKickDescription" },
        roundhouseKick: { frameCount: roundhouseKickFrameCount, fps: roundhouseKickFps, descriptionKey: "roundhouseKickDescription" },
        jumpingRoundhouseKick: { frameCount: jumpingRoundhouseKickFrameCount, fps: jumpingRoundhouseKickFps, descriptionKey: "jumpingRoundhouseKickDescription" },
        lowVault: { frameCount: lowVaultMotion.frameCount, fps: lowVaultMotion.fps, descriptionKey: "lowVaultDescription" },
        forwardRoll: { frameCount: forwardRollMotion.frameCount, fps: forwardRollMotion.fps, descriptionKey: "forwardRollDescription" },
        stumbleRecovery: { frameCount: stumbleRecoveryMotion.frameCount, fps: stumbleRecoveryMotion.fps, descriptionKey: "stumbleRecoveryDescription" },
        boxingCombo: { frameCount: boxingComboMotion.frameCount, fps: boxingComboMotion.fps, descriptionKey: "boxingComboDescription" },
        doorPush: { frameCount: doorPushMotion.frameCount, fps: doorPushMotion.fps, descriptionKey: "doorPushDescription" },
        floorPickup: { frameCount: floorPickupMotion.frameCount, fps: floorPickupMotion.fps, descriptionKey: "floorPickupDescription" },
        ladderClimb: { frameCount: ladderClimbMotion.frameCount, fps: ladderClimbMotion.fps, descriptionKey: "ladderClimbDescription" },
        kneeSlide: { frameCount: kneeSlideMotion.frameCount, fps: kneeSlideMotion.fps, descriptionKey: "kneeSlideDescription" }
      };
      const travelLineHiddenModes = new Set([
        "wallPeek", "wallClimb", "proneCrawl", "swordSlash", "crouchTwerk",
        "shuffleDance", "longDance", "playfulGirlWalk", "boxOverheadLift",
        "hadoken", "heroLandingPose", "baseballBatSwing", "seatedBottleDrink",
        "archeryFullDraw", "sideKick", "roundhouseKick", "jumpingRoundhouseKick",
        "lowVault", "forwardRoll", "stumbleRecovery", "boxingCombo", "doorPush",
        "floorPickup", "ladderClimb", "kneeSlide"
      ]);
      const lowTargetModes = new Set(["heroLandingPose", "seatedBottleDrink", "forwardRoll", "floorPickup", "kneeSlide"]);
      let frameIndex = 0;
      let currentMode = "basic";
      let frameCount = walkModes.basic.frameCount;
      let currentLanguage = "en";
      const copy = {
        en: {
          documentTitle: "CodexPose Web",
          languageButton: "한국어",
          pageTitle: "CodexPose Web",
          pageDescription: "UE5 Manny-based demo for web previews and reference assets.",
          fallbackTitle: "Three.js did not load.",
          fallbackBody: "Connect to the internet or place a local Three.js build next to this HTML file.",
          walkTitle: "Manny Walk Cycle 3D",
          walkDescription: "24 frames at 24 fps, Manny-proportioned skeleton, forward-axis walk.",
          basicWalk: "Basic Walk",
          feminineWalk: "Feminine Walk",
          running: "Run",
          naturalRunMode: "Natural Run",
          overheadWaveMode: "Overhead Wave",
          idleShiftMode: "Idle Shift",
          wallPeekMode: "Wall Peek",
          wallClimbMode: "Wall Climb",
          proneCrawlMode: "Prone Crawl",
          swordSlashMode: "Sword Slash",
          crouchTwerkMode: "Crouch Twerk",
          shuffleDanceMode: "Shuffle Dance",
          longDanceMode: "Long Dance",
          playfulGirlWalkMode: "Playful Girl Walk",
          boxOverheadLiftMode: "Box Overhead Lift",
          hadokenMode: "Hadoken",
          heroLandingPoseMode: "◆ Pose: Hero Landing",
          baseballBatSwingMode: "Baseball Bat Swing",
          seatedBottleDrinkMode: "Seated Bottle Drink",
          archeryFullDrawMode: "Archery Full Draw",
          basicWalkDescription: "24 frames at 24 fps, Manny-proportioned skeleton, forward-axis walk.",
          feminineWalkDescription: "24 frames at 24 fps, graceful pelvis yaw and roll with lighter steps.",
          runningDescription: "16 frames at 32 fps, fast forward-axis run with clear airborne phases, heavier landings, and aggressive fist pumps.",
          naturalRunDescription: "External key-pose data driven run loop with planted stance, flight, torso lean, and counter-rotation.",
          overheadWaveDescription: "24 frames at 24 fps, overhead hand wave loop in the same 3D view.",
          idleShiftDescription: "24 frames at 24 fps, calm alternating weight shift with no walking translation.",
          wallPeekDescription: "24 frames at 24 fps, leaning against a fixed-width wall before peeking around its right edge.",
          wallClimbDescription: "External key-pose parkour mantle: jump, hand catch, one-leg hook, sideways pull-over, and stand-up.",
          proneCrawlDescription: "External key-pose data driven low tactical crawl loop with contact-aware hands, knees, and feet.",
          swordSlashDescription: "16 key-pose frames at 24 fps, heavy right-hand sword downward slash with planted feet.",
          crouchTwerkDescription: "16 frames at 24 fps, non-explicit crouched dance loop with planted feet, hip bounce, pelvis roll, and torso counter-motion.",
          shuffleDanceDescription: "External key-pose data driven shuffle dance with heel-toe footwork, crossing steps, lateral weight shifts, and balancing arms.",
          longDanceDescription: "10-second external key-pose dance loop with multiple groove phrases, footwork, hip sway, arm sweeps, and recovery accents.",
          playfulGirlWalkDescription: "1.6-second external key-pose loop with a bouncy, youthful feminine walk, emphasized pelvis yaw/roll, and lively elbow/wrist swing.",
          boxOverheadLiftDescription: "Key-pose action where Manny squats to grip a floor box, lifts it close to the body, and presses it overhead with both hands.",
          hadokenDescription: "Key-pose energy projectile action: grounded charge, both-palms thrust, blue projectile release, recoil, and guard recovery.",
          heroLandingPoseDescription: "Single-frame static pose derived from the imagegen hero landing reference.",
          baseballBatSwingDescription: "36 key-pose frames at 24 fps, heavy right-handed baseball bat swing with planted feet, rear-foot pivot, fast contact, and wrapped follow-through.",
          seatedBottleDrinkDescription: "72 key-pose frames at 24 fps, generated-image-based seated scene: pick up a bottle drink from a table, drink, then return it.",
          archeryFullDrawDescription: "48 key-pose frames at 24 fps, right-handed archery shot with planted stance, full draw, release, follow-through, bow, string, and arrow prop.",
          sideKickMode: "Side Kick",
          sideKickDescription: "32 key-pose frames at 24 fps, heavy realistic right-leg side kick with planted support foot, chamber, impact hold, recoil, and recovery.",
          roundhouseKickMode: "Roundhouse Kick",
          roundhouseKickDescription: "28 key-pose frames at 24 fps, heavy planted-pivot roundhouse kick with hip rotation, impact hold, recoil, and recovery.",
          jumpingRoundhouseKickMode: "Jumping Roundhouse Kick",
          jumpingRoundhouseKickDescription: "36 key-pose frames at 24 fps, crouch load, airborne hip turn, roundhouse impact, and heavy absorbed landing.",
          lowVaultMode: "Low Vault",
          lowVaultDescription: "Key-pose parkour vault over a low obstacle with hand plants, tucked legs, release, and staggered landing.",
          forwardRollMode: "Forward Roll",
          forwardRollDescription: "Key-pose breakfall roll with crouch drop, shoulder contact, curled spine, feet-under recovery, and stand-up.",
          stumbleRecoveryMode: "Stumble Recovery",
          stumbleRecoveryDescription: "Key-pose stumble test with a misstep, torso lurch, counterbalancing arms, recovery step, and stable finish.",
          boxingComboMode: "Boxing Combo",
          boxingComboDescription: "Key-pose guard, jab, cross, hook, and guarded recovery with rear-foot pivot and torso rotation.",
          doorPushMode: "Door Push",
          doorPushDescription: "Key-pose heavy door interaction with brace, one-hand push, body lean, step-through, release, and recovery.",
          floorPickupMode: "Floor Pickup",
          floorPickupDescription: "Key-pose squat-and-hinge pickup with floor reach, object grasp, close-body lift, stand, and settle.",
          ladderClimbMode: "Ladder Climb",
          ladderClimbDescription: "Key-pose ladder climb rhythm with alternating hand and foot rung contacts and vertical root travel.",
          kneeSlideMode: "Knee Slide",
          kneeSlideDescription: "Key-pose slide test with run-in, drop to knees, forward skid, friction slowdown, plant, and rise.",
          pause: "Pause",
          play: "Play",
          stop: "Stop",
          loop: "Loop",
          frame: "Frame",
          rightSide: "right side",
          leftSide: "left side",
          centerChain: "center chain",
          overheadTitle: "Manny Overhead Wave Loop 3D",
          overheadDescription: "Rotatable 3D preview, 24 frames at 24 fps, with view-axis shortcuts.",
          referenceTitle: "Manny Reference Front Pose PNG",
          referenceDescription: "Static front-view bone reference image.",
          open: "Open"
        },
        ko: {
          lowVaultMode: "낮은 장애물 볼트",
          lowVaultDescription: "손 짚기, 다리 접기, 장애물 통과, 릴리즈, 엇박 착지가 있는 낮은 장애물 볼트 키포즈입니다.",
          forwardRollMode: "앞구르기 낙법",
          forwardRollDescription: "웅크린 드롭, 어깨 접촉, 말린 척추, 발 회수, 일어서기 회복을 가진 앞구르기 낙법입니다.",
          stumbleRecoveryMode: "비틀거림 회복",
          stumbleRecoveryDescription: "헛디딤, 상체 쏠림, 팔 균형, 회복 스텝, 안정 자세 복귀를 확인하는 키포즈입니다.",
          boxingComboMode: "복싱 3연타",
          boxingComboDescription: "가드, 잽, 크로스, 훅, 가드 복귀를 후족 피벗과 상체 회전으로 보여주는 키포즈입니다.",
          doorPushMode: "문 밀고 들어가기",
          doorPushDescription: "무거운 문에 기대 한 손으로 밀고, 몸을 통과시킨 뒤 손을 떼고 회복하는 키포즈입니다.",
          floorPickupMode: "바닥 물건 줍기",
          floorPickupDescription: "무릎과 골반을 함께 쓰는 스쿼트/힌지, 바닥 물건 그립, 가까운 리프트, 기립 회복 키포즈입니다.",
          ladderClimbMode: "사다리 오르기",
          ladderClimbDescription: "손과 발이 번갈아 가로대를 잡고 밟으며 수직으로 이동하는 사다리 오르기 키포즈입니다.",
          kneeSlideMode: "무릎 슬라이드",
          kneeSlideDescription: "달려와 무릎으로 떨어지고 앞으로 미끄러진 뒤 감속, 지지, 일어서기를 보여주는 키포즈입니다.",
          documentTitle: "CodexPose Web",
          languageButton: "English",
          pageTitle: "CodexPose Web",
          pageDescription: "웹 미리보기와 참고 에셋을 위한 UE5 Manny 기반 데모입니다.",
          fallbackTitle: "Three.js를 불러오지 못했습니다.",
          fallbackBody: "인터넷에 연결하거나 이 HTML 파일 옆에 로컬 Three.js 빌드를 배치하세요.",
          walkTitle: "Manny 걷기 사이클 3D",
          walkDescription: "24fps 기준 24프레임, Manny 비율의 전방축 보행입니다.",
          basicWalk: "기본 걸음",
          feminineWalk: "여성 걸음",
          running: "뛰기",
          naturalRunMode: "자연스러운 뛰기",
          overheadWaveMode: "손 크게흔들기",
          idleShiftMode: "짝다리 대기",
          wallPeekMode: "벽 옆 빼꼼",
          wallClimbMode: "벽 타고 오르기",
          proneCrawlMode: "포복 기어가기",
          swordSlashMode: "오른손 검 내려베기",
          crouchTwerkMode: "쪼그려 트월킹",
          shuffleDanceMode: "셔플댄스",
          longDanceMode: "긴 춤",
          playfulGirlWalkMode: "발랄한 소녀 걸음",
          boxOverheadLiftMode: "상자 머리위로 들기",
          hadokenMode: "파동권",
          heroLandingPoseMode: "◆ 포즈: 히어로 랜딩",
          baseballBatSwingMode: "야구 배트 스윙",
          seatedBottleDrinkMode: "앉아서 병음료 마시기",
          basicWalkDescription: "24fps 기준 24프레임, Manny 비율의 전방축 보행입니다.",
          feminineWalkDescription: "24fps 기준 24프레임, 우아한 골반 yaw/roll과 가벼운 보행입니다.",
          runningDescription: "32fps 기준 16프레임, 체공 구간과 묵직한 착지, 강한 주먹 스윙을 가진 빠른 전방축 뛰기입니다.",
          naturalRunDescription: "외부 키포즈 데이터 기반으로 접지, 체공, 상체 전경사와 카운터 회전을 표현한 자연스러운 뛰기 루프입니다.",
          overheadWaveDescription: "24fps 기준 24프레임, 같은 3D 뷰에서 보는 손 크게흔들기 루프입니다.",
          idleShiftDescription: "24fps 기준 24프레임, 걷기 이동 없이 짝다리 체중을 번갈아 이동하는 차분한 대기 자세입니다.",
          wallPeekDescription: "24fps 기준 24프레임, 고정 폭 벽에 등을 기대고 있다가 벽 오른쪽으로 빼꼼 내미는 루프입니다.",
          wallClimbDescription: "외부 키포즈 데이터 기반의 벽오르기입니다. 뛰어 손을 짚고, 한발을 걸쳐 옆으로 당겨 올라선 뒤 서는 모션입니다.",
          proneCrawlDescription: "외부 키포즈 데이터 기반으로 손, 무릎, 발 접지를 고려해 낮게 기어가는 포복 루프입니다.",
          swordSlashDescription: "24fps 기준 16개 키포즈 프레임, 발을 고정한 묵직한 오른손 검 내려베기입니다.",
          crouchTwerkDescription: "24fps 기준 16프레임, 발을 고정한 깊은 쪼그린 자세에서 골반 바운스와 롤, 상체 카운터 모션을 반복하는 비노골적 댄스 루프입니다.",
          shuffleDanceDescription: "외부 키포즈 데이터 기반으로 힐토 발동작, 교차 스텝, 좌우 체중 이동과 균형 잡는 팔동작을 표현한 셔플댄스 루프입니다.",
          longDanceDescription: "외부 키포즈 데이터 기반 10초 루프로, 여러 그루브 구간과 발동작, 골반 스웨이, 팔 스윕, 회복 액센트를 포함한 긴 춤 모션입니다.",
          playfulGirlWalkDescription: "외부 키포즈 데이터 기반 1.6초 루프로, 통통 튀는 리듬과 강조된 골반 yaw/roll, 활기 있는 팔꿈치/손목 스윙을 가진 소녀 걸음입니다.",
          boxOverheadLiftDescription: "키포즈 기반 액션입니다. Manny가 바닥에 놓인 어깨 폭 정도의 상자를 숙여 잡고 몸 가까이 들어 올린 뒤 머리 위로 고정합니다.",
          hadokenDescription: "키포즈 기반 에너지 발사 액션입니다. 지면을 딛고 기를 모은 뒤 양손을 뻗어 푸른 투사체를 발사하고 회복합니다.",
          heroLandingPoseDescription: "이미지젠 히어로 랜딩 참고 이미지에서 추출한 1프레임 정적 포즈입니다.",
          baseballBatSwingDescription: "24fps 기준 36개 키포즈 프레임, 발을 고정하고 뒷발 피벗으로 몸이 먼저 배트를 끌고 나가는 묵직한 오른손 타자 스윙입니다.",
          seatedBottleDrinkDescription: "24fps 기준 72개 키포즈 프레임, 생성 이미지 기반으로 의자에 앉아 테이블의 병음료를 들어 마신 뒤 다시 내려놓는 장면입니다.",
          archeryFullDrawMode: "활 당기기",
          archeryFullDrawDescription: "24fps 기준 48개 키포즈 프레임, 오른손으로 시위를 당기고 오른쪽 어깨가 뒤로 빠지는 활쏘기 모션입니다.",
          sideKickMode: "옆차기",
          sideKickDescription: "24fps 기준 32개 키포즈 프레임, 지지발 고정과 챔버, 타격 홀드, 회수, 회복이 있는 묵직하고 사실적인 오른발 옆차기입니다.",
          roundhouseKickMode: "돌려차기",
          roundhouseKickDescription: "24fps 기준 28개 키포즈 프레임, 지지발 피벗과 골반 회전, 타격 홀드, 회수, 회복이 있는 묵직한 돌려차기입니다.",
          jumpingRoundhouseKickMode: "점프 돌려차기",
          jumpingRoundhouseKickDescription: "24fps 기준 36개 키포즈 프레임, 깊은 로드, 체공 중 골반 회전, 돌려차기 타격, 묵직한 착지 흡수가 있는 점프 돌려차기입니다.",
          pause: "일시정지",
          play: "재생",
          stop: "정지",
          loop: "루프",
          frame: "프레임",
          rightSide: "오른쪽",
          leftSide: "왼쪽",
          centerChain: "중앙 체인",
          overheadTitle: "Manny 손 크게흔들기 3D",
          overheadDescription: "24fps 기준 24프레임 회전형 3D 미리보기와 시점 축 바로가기가 있습니다.",
          referenceTitle: "Manny 정면 기준 포즈 PNG",
          referenceDescription: "정적 정면 본 구성 참고 이미지입니다.",
          open: "열기"
        }
      };      const colors = {
        left: 0x23aeea,
        right: 0xf4a12d,
        center: 0x414a57,
        joint: 0x2f3a49,
        footPlant: 0x1f9d55,
        envelope: 0x526878
      };

      function t(key) {
        return copy[currentLanguage][key] || copy.en[key] || key;
      }

      const timelineControls = window.MannyTimelineControls.create({
        playButton: document.getElementById("walkPlayButton"),
        stopButton: document.getElementById("walkStopButton"),
        timelineInput: document.getElementById("walkTimeline"),
        loopInput: document.getElementById("walkLoopToggle"),
        frameReadout: document.getElementById("walkFrameReadout"),
        strip: document.getElementById("walkStrip"),
        getText: t,
        onFrameChange(index) {
          frameIndex = index;
          renderPoseFrame(index);
        }
      });

      function updateModeDescription() {
        walkModeDescription.textContent = t(walkModes[currentMode].descriptionKey);
      }

      function setLanguage(language) {
        currentLanguage = language;
        document.documentElement.lang = language;
        document.title = t("documentTitle");
        document.querySelectorAll("[data-i18n]").forEach((node) => {
          node.textContent = t(node.dataset.i18n);
        });
        languageToggle.textContent = t("languageButton");
        timelineControls.refreshLabels();
        updateModeDescription();
      }

      languageToggle.addEventListener("click", () => {
        setLanguage(currentLanguage === "en" ? "ko" : "en");
      });
      setLanguage("en");

      if (!window.THREE) {
        fallback.style.display = "grid";
        return;
      }

      const scene = new THREE.Scene();
      scene.background = new THREE.Color(0xf8fafc);
      scene.fog = null;

      const renderer = new THREE.WebGLRenderer({ canvas, antialias: true, alpha: false });
      renderer.setPixelRatio(Math.min(window.devicePixelRatio || 1, 3));
      renderer.outputEncoding = THREE.sRGBEncoding;

      const camera = new THREE.PerspectiveCamera(42, 1, 0.1, 1200);
      const defaultTargetY = 105;
      const lowPoseTargetY = 70;
      const target = new THREE.Vector3(0, defaultTargetY, 0);
      const orbit = { theta: -1.05, phi: 1.28, radius: 360 };
      const viewDirections = {
        xPlus: new THREE.Vector3(1, 0, 0),
        xMinus: new THREE.Vector3(-1, 0, 0),
        yPlus: new THREE.Vector3(0, 0, 1),
        yMinus: new THREE.Vector3(0, 0, -1),
        zPlus: new THREE.Vector3(0, 1, 0),
        zMinus: new THREE.Vector3(0, -1, 0)
      };

      const hemi = new THREE.HemisphereLight(0xffffff, 0xd8e1ea, 1.28);
      scene.add(hemi);
      const key = new THREE.DirectionalLight(0xffffff, 1.7);
      key.position.set(90, 180, 130);
      scene.add(key);

      const grid = new THREE.GridHelper(160, 16, 0xcdd6df, 0xe6ecf2);
      scene.add(grid);

      const travelLine = new THREE.Line(
        new THREE.BufferGeometry().setFromPoints([
          new THREE.Vector3(0, 0.5, -48),
          new THREE.Vector3(0, 0.5, 48)
        ]),
        new THREE.LineBasicMaterial({ color: 0xb7c5d3, transparent: true, opacity: 0.7 })
      );
      scene.add(travelLine);

      const wallPeekWallOptions = wallPeekSource.wall || {};
      const wallPeekWallCenter = naturalRunArray(wallPeekWallOptions.center, 3, null);
      const wallPeekWall = {
        centerX: readNumberOption(wallPeekWallOptions, ["centerX", "x", "center.x"], wallPeekWallCenter?.[0] ?? -10),
        y: readNumberOption(wallPeekWallOptions, ["y", "centerY", "center.y"], wallPeekWallCenter?.[1] ?? 92),
        z: readNumberOption(wallPeekWallOptions, ["z", "centerZ", "center.z"], wallPeekWallCenter?.[2] ?? -10),
        width: readNumberOption(wallPeekWallOptions, ["width", "size.width"], 56),
        height: readNumberOption(wallPeekWallOptions, ["height", "size.height"], 184),
        thickness: readNumberOption(wallPeekWallOptions, ["thickness", "depth", "size.thickness", "size.depth"], 5)
      };
      wallPeekWall.rightEdgeX = readNumberOption(
        wallPeekWallOptions,
        ["rightEdgeX", "rightEdge", "edgeX"],
        wallPeekWall.centerX + wallPeekWall.width * 0.5
      );
      const wallGroup = new THREE.Group();
      const wallMaterial = new THREE.MeshStandardMaterial({
        color: 0x74a6d8,
        transparent: true,
        opacity: 0.96,
        roughness: 0.76,
        metalness: 0.01
      });
      const wallFace = new THREE.Mesh(
        new THREE.BoxGeometry(wallPeekWall.width, wallPeekWall.height, wallPeekWall.thickness),
        wallMaterial
      );
      wallFace.position.set(wallPeekWall.centerX, wallPeekWall.y, wallPeekWall.z);
      wallGroup.add(wallFace);
      const wallEdge = new THREE.Mesh(
        new THREE.BoxGeometry(3, wallPeekWall.height, wallPeekWall.thickness + 1.2),
        new THREE.MeshStandardMaterial({
          color: 0x3f7fb8,
          transparent: true,
          opacity: 0.95,
          roughness: 0.78,
          metalness: 0.01
        })
      );
      wallEdge.position.set(wallPeekWall.rightEdgeX, wallPeekWall.y, wallPeekWall.z + 0.1);
      wallGroup.add(wallEdge);
      wallGroup.visible = false;
      scene.add(wallGroup);

      const wallClimbWallOptions = wallClimbSource.wall || {};
      const wallClimbWallCenter = naturalRunArray(wallClimbWallOptions.center, 3, null);
      const wallClimbWall = {
        centerX: readNumberOption(wallClimbWallOptions, ["centerX", "x", "center.x"], wallClimbWallCenter?.[0] ?? 0),
        y: readNumberOption(wallClimbWallOptions, ["y", "centerY", "center.y"], wallClimbWallCenter?.[1] ?? 56),
        z: readNumberOption(wallClimbWallOptions, ["z", "centerZ", "center.z"], wallClimbWallCenter?.[2] ?? 48),
        width: readNumberOption(wallClimbWallOptions, ["width", "size.width"], 100),
        height: readNumberOption(wallClimbWallOptions, ["height", "size.height"], 112),
        thickness: readNumberOption(wallClimbWallOptions, ["thickness", "depth", "size.thickness", "size.depth"], 7)
      };
      wallClimbWall.topY = readNumberOption(
        wallClimbWallOptions,
        ["topY", "top.y"],
        wallClimbWall.y + wallClimbWall.height * 0.5
      );
      wallClimbWall.frontZ = readNumberOption(
        wallClimbWallOptions,
        ["frontZ", "faceZ", "surfaceZ"],
        wallClimbWall.z - wallClimbWall.thickness * 0.5
      );
      const wallClimbGroup = new THREE.Group();
      const wallClimbMaterial = new THREE.MeshStandardMaterial({
        color: 0x4f93c5,
        transparent: true,
        opacity: 0.82,
        roughness: 0.74,
        metalness: 0.02
      });
      const wallClimbFace = new THREE.Mesh(
        new THREE.BoxGeometry(wallClimbWall.width, wallClimbWall.height, wallClimbWall.thickness),
        wallClimbMaterial
      );
      wallClimbFace.position.set(wallClimbWall.centerX, wallClimbWall.y, wallClimbWall.z);
      wallClimbGroup.add(wallClimbFace);
      const wallClimbTop = new THREE.Mesh(
        new THREE.BoxGeometry(wallClimbWall.width, 3, 34),
        new THREE.MeshStandardMaterial({
          color: 0x2f6f9e,
          transparent: true,
          opacity: 0.88,
          roughness: 0.76,
          metalness: 0.02
        })
      );
      wallClimbTop.position.set(wallClimbWall.centerX, wallClimbWall.topY + 1.5, wallClimbWall.z + 14);
      wallClimbGroup.add(wallClimbTop);
      const wallClimbTopEdge = new THREE.Mesh(
        new THREE.BoxGeometry(wallClimbWall.width, 3.2, 4.2),
        new THREE.MeshStandardMaterial({
          color: 0x255a82,
          transparent: true,
          opacity: 0.94,
          roughness: 0.78,
          metalness: 0.02
        })
      );
      wallClimbTopEdge.position.set(wallClimbWall.centerX, wallClimbWall.topY + 1.8, wallClimbWall.frontZ - 0.4);
      wallClimbGroup.add(wallClimbTopEdge);
      wallClimbGroup.visible = false;
      scene.add(wallClimbGroup);

      const swordGroup = new THREE.Group();
      const swordMetalMaterial = new THREE.MeshStandardMaterial({
        color: 0xb8c2cc,
        roughness: 0.34,
        metalness: 0.52
      });
      const swordGripMaterial = new THREE.MeshStandardMaterial({
        color: 0x364152,
        roughness: 0.62,
        metalness: 0.14
      });
      const swordGuardMaterial = new THREE.MeshStandardMaterial({
        color: 0xd9a441,
        roughness: 0.42,
        metalness: 0.28
      });
      const swordBlade = new THREE.Mesh(new THREE.CylinderGeometry(0.55, 0.85, 1, 12), swordMetalMaterial);
      const swordHandle = new THREE.Mesh(new THREE.CylinderGeometry(1.05, 1.05, 1, 12), swordGripMaterial);
      const swordGuard = new THREE.Mesh(new THREE.CylinderGeometry(0.65, 0.65, 1, 12), swordGuardMaterial);
      const swordArcCurve = new THREE.CatmullRomCurve3([
        v(-34, 178, -26),
        v(-29, 168, -8),
        v(-16, 142, 15),
        v(0, 106, 33),
        v(18, 66, 40),
        v(26, 42, 31)
      ]);
      const swordArc = new THREE.Mesh(
        new THREE.TubeGeometry(swordArcCurve, 48, 0.72, 8, false),
        new THREE.MeshBasicMaterial({
          color: 0xe3ad3a,
          transparent: true,
          opacity: 0.45,
          depthWrite: false
        })
      );
      [swordBlade, swordHandle, swordGuard, swordArc].forEach((mesh) => {
        mesh.renderOrder = 2;
        swordGroup.add(mesh);
      });
      swordGroup.visible = false;
      scene.add(swordGroup);

      const liftBoxGroup = new THREE.Group();
      const liftBoxMesh = new THREE.Mesh(
        new THREE.BoxGeometry(1, 1, 1),
        new THREE.MeshStandardMaterial({
          color: 0xa86f3f,
          roughness: 0.72,
          metalness: 0.02
        })
      );
      const liftBoxEdges = new THREE.LineSegments(
        new THREE.EdgesGeometry(new THREE.BoxGeometry(1, 1, 1)),
        new THREE.LineBasicMaterial({
          color: 0x6b4428,
          transparent: true,
          opacity: 0.72
        })
      );
      liftBoxMesh.renderOrder = 2;
      liftBoxEdges.renderOrder = 3;
      liftBoxGroup.add(liftBoxMesh, liftBoxEdges);
      liftBoxGroup.visible = false;
      scene.add(liftBoxGroup);

      const hadokenGroup = new THREE.Group();
      const hadokenCore = new THREE.Mesh(
        new THREE.SphereGeometry(1, 32, 18),
        new THREE.MeshBasicMaterial({
          color: 0x49d9ff,
          transparent: true,
          opacity: 0.82,
          depthWrite: false
        })
      );
      const hadokenGlow = new THREE.Mesh(
        new THREE.SphereGeometry(1, 32, 18),
        new THREE.MeshBasicMaterial({
          color: 0x1f77ff,
          transparent: true,
          opacity: 0.28,
          depthWrite: false
        })
      );
      const hadokenRings = [0, 1, 2].map(() => new THREE.Mesh(
        new THREE.TorusGeometry(1, 0.08, 8, 36),
        new THREE.MeshBasicMaterial({
          color: 0xb9f5ff,
          transparent: true,
          opacity: 0.58,
          depthWrite: false
        })
      ));
      [hadokenGlow, hadokenCore, ...hadokenRings].forEach((mesh) => {
        mesh.renderOrder = 4;
        hadokenGroup.add(mesh);
      });
      hadokenGroup.visible = false;
      scene.add(hadokenGroup);

      const baseballBatGroup = new THREE.Group();
      const baseballBatHandle = new THREE.Mesh(
        new THREE.CylinderGeometry(0.72, 0.9, 1, 14),
        new THREE.MeshStandardMaterial({
          color: 0x5f3a1e,
          roughness: 0.66,
          metalness: 0.02
        })
      );
      const baseballBatBarrel = new THREE.Mesh(
        new THREE.CylinderGeometry(2.05, 1.2, 1, 18),
        new THREE.MeshStandardMaterial({
          color: 0xb67a3a,
          roughness: 0.58,
          metalness: 0.02
        })
      );
      const baseballBatKnob = new THREE.Mesh(
        new THREE.SphereGeometry(1.9, 16, 10),
        new THREE.MeshStandardMaterial({
          color: 0x4b2c18,
          roughness: 0.64,
          metalness: 0.02
        })
      );
      [baseballBatHandle, baseballBatBarrel, baseballBatKnob].forEach((mesh) => {
        mesh.renderOrder = 3;
        baseballBatGroup.add(mesh);
      });
      baseballBatGroup.visible = false;
      scene.add(baseballBatGroup);

      const seatedDrinkGroup = new THREE.Group();
      const seatedDrinkChairMaterial = new THREE.MeshStandardMaterial({
        color: 0x6d4d32,
        roughness: 0.72,
        metalness: 0.02
      });
      const seatedDrinkMetalMaterial = new THREE.MeshStandardMaterial({
        color: 0x303743,
        roughness: 0.48,
        metalness: 0.18
      });
      const seatedDrinkTableMaterial = new THREE.MeshStandardMaterial({
        color: 0x8d6038,
        roughness: 0.66,
        metalness: 0.02
      });
      const seatedDrinkBottleMaterial = new THREE.MeshStandardMaterial({
        color: 0xb36b23,
        transparent: true,
        opacity: 0.78,
        roughness: 0.24,
        metalness: 0.02
      });
      const seatedDrinkCapMaterial = new THREE.MeshStandardMaterial({
        color: 0x1d232b,
        roughness: 0.45,
        metalness: 0.08
      });
      function addSceneBox(group, center, size, material) {
        const mesh = new THREE.Mesh(new THREE.BoxGeometry(1, 1, 1), material);
        mesh.position.set(center[0], center[1], center[2]);
        mesh.scale.set(size[0], size[1], size[2]);
        mesh.renderOrder = 1;
        group.add(mesh);
        return mesh;
      }
      function addSceneLeg(group, x, y, z, height, material) {
        const mesh = new THREE.Mesh(new THREE.CylinderGeometry(1.15, 1.15, 1, 10), material);
        mesh.position.set(x, y + height * 0.5, z);
        mesh.scale.set(1, height, 1);
        mesh.renderOrder = 1;
        group.add(mesh);
        return mesh;
      }
      addSceneBox(seatedDrinkGroup, [0, 47, -12], [52, 6, 44], seatedDrinkChairMaterial);
      addSceneBox(seatedDrinkGroup, [0, 79, -35], [52, 62, 6], seatedDrinkChairMaterial);
      [[-22, -30], [22, -30], [-22, 6], [22, 6]].forEach(([x, z]) => addSceneLeg(seatedDrinkGroup, x, 0, z, 47, seatedDrinkMetalMaterial));
      addSceneBox(seatedDrinkGroup, [36, 62, 24], [48, 5, 40], seatedDrinkTableMaterial);
      [[16, 8], [56, 8], [16, 40], [56, 40]].forEach(([x, z]) => addSceneLeg(seatedDrinkGroup, x, 0, z, 61, seatedDrinkMetalMaterial));
      const seatedDrinkBottleBody = new THREE.Mesh(new THREE.CylinderGeometry(3.2, 3.2, 1, 18), seatedDrinkBottleMaterial);
      const seatedDrinkBottleCap = new THREE.Mesh(new THREE.CylinderGeometry(2.3, 2.3, 1, 14), seatedDrinkCapMaterial);
      seatedDrinkBottleBody.renderOrder = 3;
      seatedDrinkBottleCap.renderOrder = 4;
      seatedDrinkGroup.add(seatedDrinkBottleBody, seatedDrinkBottleCap);
      seatedDrinkGroup.visible = false;
      scene.add(seatedDrinkGroup);

      const archeryGroup = new THREE.Group();
      const archeryBowMaterial = new THREE.MeshStandardMaterial({
        color: 0x8a5a2b,
        roughness: 0.58,
        metalness: 0.02
      });
      const archeryStringMaterial = new THREE.LineBasicMaterial({
        color: 0xe6edf4,
        transparent: true,
        opacity: 0.86
      });
      const archeryArrowMaterial = new THREE.MeshStandardMaterial({
        color: 0xd7dde3,
        roughness: 0.36,
        metalness: 0.18
      });
      const archeryBowUpper = new THREE.Mesh(new THREE.CylinderGeometry(0.9, 1.25, 1, 14), archeryBowMaterial);
      const archeryBowLower = new THREE.Mesh(new THREE.CylinderGeometry(1.25, 0.9, 1, 14), archeryBowMaterial);
      const archeryGrip = new THREE.Mesh(
        new THREE.SphereGeometry(2.2, 16, 10),
        new THREE.MeshStandardMaterial({ color: 0x4a2c18, roughness: 0.66, metalness: 0.02 })
      );
      const archeryString = new THREE.Line(new THREE.BufferGeometry(), archeryStringMaterial);
      const archeryArrow = new THREE.Mesh(new THREE.CylinderGeometry(0.42, 0.42, 1, 10), archeryArrowMaterial);
      const archeryArrowHead = new THREE.Mesh(
        new THREE.ConeGeometry(1.35, 4.0, 14),
        new THREE.MeshStandardMaterial({ color: 0xaeb8c2, roughness: 0.32, metalness: 0.34 })
      );
      [archeryBowUpper, archeryBowLower, archeryGrip, archeryString, archeryArrow, archeryArrowHead].forEach((mesh) => {
        mesh.renderOrder = 3;
        archeryGroup.add(mesh);
      });
      archeryGroup.visible = false;
      scene.add(archeryGroup);

      const propBlueMaterial = new THREE.MeshStandardMaterial({
        color: 0x4f93c5,
        roughness: 0.72,
        metalness: 0.02
      });
      const propDarkMaterial = new THREE.MeshStandardMaterial({
        color: 0x334155,
        roughness: 0.62,
        metalness: 0.05
      });
      const propWoodMaterial = new THREE.MeshStandardMaterial({
        color: 0x9a6b3d,
        roughness: 0.68,
        metalness: 0.02
      });
      const propMatMaterial = new THREE.MeshStandardMaterial({
        color: 0x5c7c8f,
        transparent: true,
        opacity: 0.34,
        roughness: 0.84,
        metalness: 0.01
      });

      const lowVaultPropGroup = new THREE.Group();
      addSceneBox(lowVaultPropGroup, [0, 20, 30], [78, 40, 12], propBlueMaterial);
      addSceneBox(lowVaultPropGroup, [0, 42, 30], [84, 4, 16], propDarkMaterial);
      lowVaultPropGroup.visible = false;
      scene.add(lowVaultPropGroup);

      const doorPushPropGroup = new THREE.Group();
      addSceneBox(doorPushPropGroup, [-32, 52, 34], [4, 104, 8], propDarkMaterial);
      addSceneBox(doorPushPropGroup, [0, 104, 34], [66, 4, 8], propDarkMaterial);
      const doorLeafPivot = new THREE.Group();
      doorLeafPivot.position.set(-30, 52, 34);
      const doorLeaf = new THREE.Mesh(new THREE.BoxGeometry(1, 1, 1), propWoodMaterial);
      doorLeaf.position.set(30, 0, 0);
      doorLeaf.scale.set(60, 96, 4);
      doorLeaf.renderOrder = 1;
      doorLeafPivot.add(doorLeaf);
      doorPushPropGroup.add(doorLeafPivot);
      doorPushPropGroup.visible = false;
      scene.add(doorPushPropGroup);

      const floorPickupPropGroup = new THREE.Group();
      addSceneBox(floorPickupPropGroup, [24, 4, 32], [12, 8, 12], propWoodMaterial);
      floorPickupPropGroup.visible = false;
      scene.add(floorPickupPropGroup);

      const ladderPropGroup = new THREE.Group();
      addSceneBox(ladderPropGroup, [-18, 78, 28], [4, 152, 4], propDarkMaterial);
      addSceneBox(ladderPropGroup, [18, 78, 28], [4, 152, 4], propDarkMaterial);
      [18, 38, 58, 78, 98, 118, 138].forEach((y) => {
        addSceneBox(ladderPropGroup, [0, y, 28], [42, 3.5, 4], propBlueMaterial);
      });
      ladderPropGroup.visible = false;
      scene.add(ladderPropGroup);

      const slideMatPropGroup = new THREE.Group();
      addSceneBox(slideMatPropGroup, [0, 0.35, 18], [44, 0.7, 124], propMatMaterial);
      slideMatPropGroup.visible = false;
      scene.add(slideMatPropGroup);

      const segments = [
        ["pelvis", "spine_01", "center"],
        ["spine_01", "spine_02", "center"],
        ["spine_02", "spine_03", "center"],
        ["spine_03", "spine_04", "center"],
        ["spine_04", "spine_05", "center"],
        ["spine_05", "neck_01", "center"],
        ["neck_01", "head", "center"],
        ["spine_05", "clavicle_l", "center"],
        ["spine_05", "clavicle_r", "center"],
        ["clavicle_l", "upperarm_l", "left"],
        ["upperarm_l", "lowerarm_l", "left"],
        ["lowerarm_l", "hand_l", "left"],
        ["clavicle_r", "upperarm_r", "right"],
        ["upperarm_r", "lowerarm_r", "right"],
        ["lowerarm_r", "hand_r", "right"],
        ["clavicle_l", "clavicle_r", "center"],
        ["pelvis", "thigh_l", "left"],
        ["thigh_l", "thigh_r", "center"],
        ["thigh_l", "calf_l", "left"],
        ["calf_l", "foot_l", "left"],
        ["foot_l", "ball_l", "left"],
        ["pelvis", "thigh_r", "right"],
        ["thigh_r", "calf_r", "right"],
        ["calf_r", "foot_r", "right"],
        ["foot_r", "ball_r", "right"]
      ];

      const jointNames = [
        "pelvis", "spine_01", "spine_02", "spine_03", "spine_04", "spine_05",
        "neck_01", "head", "clavicle_l", "clavicle_r", "upperarm_l", "upperarm_r",
        "lowerarm_l", "lowerarm_r", "hand_l", "hand_r", "thigh_l", "thigh_r",
        "calf_l", "calf_r", "foot_l", "foot_r", "ball_l", "ball_r"
      ];

      function v(x, y, z) {
        return new THREE.Vector3(x, y, z);
      }

      function lerp(a, b, t) {
        return a + (b - a) * t;
      }

      function wave(phase) {
        return Math.sin(phase * Math.PI * 2);
      }

      function clamp(value, min, max) {
        return Math.max(min, Math.min(max, value));
      }

      function smoothstep(value) {
        return value * value * (3 - value * 2);
      }

      function readNumberOption(source, names, fallback) {
        for (const name of names) {
          const rawValue = source ? (name.includes(".") ? naturalRunPath(source, name) : source[name]) : undefined;
          const value = Number(rawValue);
          if (Number.isFinite(value)) {
            return value;
          }
        }
        return fallback;
      }

      function readNaturalKeyFrame(key, fallback) {
        const value = key ? Number(key.frame ?? key.index ?? key.frameIndex ?? key.time) : NaN;
        return Number.isFinite(value) ? value : fallback;
      }

      function naturalRunKeyHasPose(key) {
        return Boolean(key && (
          key.points || key.joints || key.pose || key.root || key.pelvis ||
          key.chest || key.spine || key.leftLeg || key.rightLeg ||
          key.leftArm || key.rightArm || key.leftFoot?.ankle || key.rightFoot?.ankle
        ));
      }

      function selectNaturalRunKeys(source) {
        const candidates = [source?.frames, source?.keyposes, source?.keys];
        const firstDetailed = candidates.find((keys) => (
          Array.isArray(keys) && keys.some((key) => naturalRunKeyHasPose(key))
        ));
        if (firstDetailed) {
          return firstDetailed;
        }
        return candidates.find((keys) => Array.isArray(keys)) || [];
      }

      function wallPeekKeyHasPose(key) {
        return Boolean(key && (
          naturalRunKeyHasPose(key) || key.leftFoot || key.rightFoot ||
          key.lUpper || key.rUpper || key.leftHand || key.rightHand ||
          key.contacts || key.contact
        ));
      }

      function selectWallPeekKeys(source) {
        const candidates = [source?.frames, source?.keyposes, source?.keys, source?.keyFrames, source?.sequence];
        const firstDetailed = candidates.find((keys) => (
          Array.isArray(keys) && keys.some((key) => wallPeekKeyHasPose(key))
        ));
        if (firstDetailed) {
          return firstDetailed;
        }
        return candidates.find((keys) => Array.isArray(keys)) || [];
      }

      function wallClimbKeyHasPose(key) {
        return Boolean(key && (
          naturalRunKeyHasPose(key) || key.contacts || key.contactTargets ||
          key.leftFoot || key.rightFoot || key.lHand || key.rHand ||
          key.hand_l || key.hand_r || key.foot_l || key.foot_r
        ));
      }

      function selectWallClimbKeys(source) {
        const candidates = [source?.frames, source?.keyposes, source?.keys, source?.keyFrames, source?.sequence];
        const firstDetailed = candidates.find((keys) => (
          Array.isArray(keys) && keys.some((key) => wallClimbKeyHasPose(key))
        ));
        if (firstDetailed) {
          return firstDetailed;
        }
        return candidates.find((keys) => Array.isArray(keys)) || [];
      }

      function proneCrawlKeyHasPose(key) {
        return Boolean(key && (
          naturalRunKeyHasPose(key) || key.leftHand || key.rightHand ||
          key.leftKnee || key.rightKnee || key.leftElbow || key.rightElbow ||
          key.contacts || key.contact
        ));
      }

      function selectProneCrawlKeys(source) {
        const candidates = [source?.frames, source?.keyposes, source?.keys];
        const firstDetailed = candidates.find((keys) => (
          Array.isArray(keys) && keys.some((key) => proneCrawlKeyHasPose(key))
        ));
        if (firstDetailed) {
          return firstDetailed;
        }
        return candidates.find((keys) => Array.isArray(keys)) || [];
      }

      function shuffleDanceKeyHasPose(key) {
        return Boolean(key && (
          naturalRunKeyHasPose(key) || key.leftHeel || key.rightHeel ||
          key.leftToe || key.rightToe || key.heelToe || key.step ||
          key.contacts || key.contact || key.footwork
        ));
      }

      function selectShuffleDanceKeys(source) {
        const candidates = [source?.frames, source?.keyposes, source?.keys];
        const firstDetailed = candidates.find((keys) => (
          Array.isArray(keys) && keys.some((key) => shuffleDanceKeyHasPose(key))
        ));
        if (firstDetailed) {
          return firstDetailed;
        }
        return candidates.find((keys) => Array.isArray(keys)) || [];
      }

      function readGenericExternalMotion(source, fallbackFrameCount, fallbackFps) {
        const rawKeys = selectShuffleDanceKeys(source);
        const fallbackCount = rawKeys.length
          ? rawKeys.reduce((maxFrame, key, index) => Math.max(maxFrame, readNaturalKeyFrame(key, index)), 0) + 1
          : fallbackFrameCount;
        return {
          rawKeys,
          frameCount: Math.max(2, Math.round(readNumberOption(
            source,
            ["frameCount", "frame_count", "totalFrames", "durationFrames", "numFrames", "config.frameCount", "meta.frameCount"],
            fallbackCount
          ))),
          fps: Math.max(1, readNumberOption(
            source,
            ["fps", "frameRate", "frame_rate", "config.fps", "config.frameRate", "meta.fps", "meta.frameRate"],
            fallbackFps
          ))
        };
      }

      function swordSlashKeyHasPose(key) {
        return Boolean(key && Array.isArray(key.pelvis) && Array.isArray(key.chest) &&
          Array.isArray(key.rUpper) && Array.isArray(key.rLower) &&
          Array.isArray(key.rHand) && Array.isArray(key.swordTip));
      }

      function selectSwordSlashKeys(source) {
        const candidates = [source?.keys, source?.frames, source?.keyposes];
        const firstDetailed = candidates.find((keys) => (
          Array.isArray(keys) && keys.some((key) => swordSlashKeyHasPose(key))
        ));
        if (firstDetailed) {
          return firstDetailed;
        }
        return candidates.find((keys) => Array.isArray(keys)) || [];
      }

      function rotateYawRoll(offset, yaw, roll) {
        const cosRoll = Math.cos(roll);
        const sinRoll = Math.sin(roll);
        const rollX = offset.x * cosRoll - offset.y * sinRoll;
        const rollY = offset.x * sinRoll + offset.y * cosRoll;
        const cosYaw = Math.cos(yaw);
        const sinYaw = Math.sin(yaw);
        return v(
          rollX * cosYaw + offset.z * sinYaw,
          rollY,
          -rollX * sinYaw + offset.z * cosYaw
        );
      }

      function rotateYawPitchRoll(offset, yaw, pitch, roll) {
        return offset.clone().applyEuler(new THREE.Euler(pitch, yaw, roll, "YXZ"));
      }

      const walkPoseModes = {
        basic: {
          bob: 1.25,
          sideSway: 1.4,
          chestTwist: -4.5,
          pelvisYaw: 0.14,
          pelvisRoll: 0.075,
          chestYawCounter: -0.42,
          chestRollCounter: -0.32,
          stepLength: 26,
          footLift: 15,
          footSpacing: 9.97,
          ballSpacing: 11.4,
          ballForward: 15.4,
          hipX: 9.97,
          hipZ: 1.1,
          upperArmSwing: 10.5,
          lowerArmSwing: 8.0,
          handSwing: 5.3,
          armOut: 1.0,
          armDrop: 0
        },
        feminine: {
          bob: 0.8,
          sideSway: 2.1,
          chestTwist: -2.6,
          pelvisYaw: 0.18,
          pelvisRoll: 0.11,
          chestYawCounter: -0.28,
          chestRollCounter: -0.22,
          stepLength: 20,
          footLift: 10.5,
          footSpacing: 7.1,
          ballSpacing: 8.2,
          ballForward: 13.2,
          hipX: 9.2,
          hipZ: 0.85,
          upperArmSwing: 6.0,
          lowerArmSwing: 4.7,
          handSwing: 3.2,
          armOut: 0.86,
          armDrop: 1.2
        },
        running: {
          bob: 0.08,
          sideSway: 0.58,
          chestTwist: -8.0,
          pelvisYaw: 0.25,
          pelvisRoll: 0.095,
          chestYawCounter: -0.52,
          chestRollCounter: -0.30,
          stepLength: 42,
          footLift: 34,
          footSpacing: 8.6,
          ballSpacing: 9.5,
          ballForward: 18.4,
          hipX: 9.8,
          hipZ: 1.2,
          upperArmSwing: 29,
          lowerArmSwing: 10.5,
          handSwing: 24,
          fistLift: 7.0,
          fistDrop: 5.0,
          elbowPump: 1.7,
          elbowDriveScale: 0.58,
          fistSwingScale: 1.18,
          fistLiftScale: 1.12,
          armOut: 0.78,
          armDrop: -5.2,
          stanceRatio: 0.22,
          torsoLean: 12.2,
          contactDrop: 1.4,
          footContactDrop: 0.55,
          reboundLift: 0.6,
          flightRise: 3.8,
          flightFootClearance: 10.5,
          kneeCompression: 3.0
        }
      };

      const swordSlashFallbackKey = {
        frame: 0,
        name: "fallback_guard",
        pelvis: [0, 0, 0, 0, -5, 1],
        chest: [1, -6, 1],
        neck: [0, -2, 0],
        head: [-1, -3, 0],
        lUpper: [15, -15, 2],
        lLower: [11, -24, 6],
        lHand: [5, -18, 5],
        rUpper: [-14, -15, -3],
        rLower: [-12, -24, 0],
        rHand: [-5, -19, 4],
        swordTip: [-8, 38, -12],
        swordPommel: [3, -8, 2],
        knee: [0, 0, 0, 0, 0, 0]
      };
      const swordSlashKeys = swordSlashRawKeys.length ? swordSlashRawKeys : [swordSlashFallbackKey];
      function interpolateArray(a, b, t) {
        return a.map((value, index) => lerp(value, b[index], t));
      }

      function sampleSwordSlashKey(index) {
        for (let i = 0; i < swordSlashKeys.length - 1; i += 1) {
          const from = swordSlashKeys[i];
          const to = swordSlashKeys[i + 1];
          if (index >= from.frame && index <= to.frame) {
            const raw = (index - from.frame) / Math.max(1, to.frame - from.frame);
            const t = smoothstep(raw);
            return {
              pelvis: interpolateArray(from.pelvis, to.pelvis, t),
              chest: interpolateArray(from.chest, to.chest, t),
              neck: interpolateArray(from.neck, to.neck, t),
              head: interpolateArray(from.head, to.head, t),
              lUpper: interpolateArray(from.lUpper, to.lUpper, t),
              lLower: interpolateArray(from.lLower, to.lLower, t),
              lHand: interpolateArray(from.lHand, to.lHand, t),
              rUpper: interpolateArray(from.rUpper, to.rUpper, t),
              rLower: interpolateArray(from.rLower, to.rLower, t),
              rHand: interpolateArray(from.rHand, to.rHand, t),
              swordTip: interpolateArray(from.swordTip, to.swordTip, t),
              swordPommel: interpolateArray(from.swordPommel, to.swordPommel, t),
              knee: interpolateArray(from.knee, to.knee, t)
            };
          }
        }
        return swordSlashKeys[swordSlashKeys.length - 1];
      }

      function overheadWaveParam(index, count) {
        const phase = (index / count) * Math.PI * 2;
        const reach = Math.sin(phase);
        const secondary = Math.sin(phase * 2);
        return {
          sway: reach * 0.66 + Math.sin(phase * 3) * 0.08,
          reach,
          lift: 0.74 + Math.cos(phase * 2) * 0.20,
          depth: Math.cos(phase) * 0.28 + secondary * 0.04
        };
      }

      function footAt(phase, side, config) {
        const cycle = phase % 1;
        const stance = cycle < 0.5;
        const t = stance ? cycle / 0.5 : (cycle - 0.5) / 0.5;
        const z = stance ? lerp(config.stepLength, -config.stepLength, t) : lerp(-config.stepLength, config.stepLength, t);
        const lift = stance ? 0 : Math.sin(t * Math.PI) * config.footLift;
        return {
          ankle: v(side * config.footSpacing, 8.3 + lift, z),
          ball: v(side * config.ballSpacing, 1.2 + lift * 0.28, z + config.ballForward),
          planted: stance
        };
      }

      function runFootAt(phase, side, config) {
        const cycle = phase % 1;
        const stance = cycle < config.stanceRatio;
        if (stance) {
          const raw = cycle / config.stanceRatio;
          const t = smoothstep(raw);
          const z = lerp(config.stepLength * 0.82, -config.stepLength * 0.74, t);
          const impact = Math.pow(1 - raw, 2.3);
          const push = Math.pow(clamp((raw - 0.52) / 0.48, 0, 1), 0.85);
          return {
            ankle: v(side * config.footSpacing, 8.15 - impact * config.footContactDrop, z),
            ball: v(side * config.ballSpacing, 1.05, z + config.ballForward),
            planted: true,
            impact,
            push
          };
        }

        const t = (cycle - config.stanceRatio) / (1 - config.stanceRatio);
        const swing = smoothstep(t);
        const tuck = Math.sin(t * Math.PI);
        const lift = Math.pow(tuck, 0.86) * config.footLift;
        const z = lerp(-config.stepLength * 0.74, config.stepLength * 0.82, swing);
        return {
          ankle: v(side * config.footSpacing, 8.7 + lift, z),
          ball: v(side * config.ballSpacing, 1.35 + lift * 0.24 + tuck * 1.2, z + config.ballForward),
          planted: false,
          impact: 0,
          push: 0
        };
      }

      function naturalRunPath(source, path) {
        return path.split(".").reduce((value, key) => (value == null ? undefined : value[key]), source);
      }

      function naturalRunArray(value, length, fallback = null) {
        if (Array.isArray(value)) {
          return Array.from({ length }, (_, index) => Number(value[index] ?? fallback?.[index] ?? 0));
        }
        if (value && typeof value === "object" && ("x" in value || "y" in value || "z" in value)) {
          return [
            Number(value.x ?? fallback?.[0] ?? 0),
            Number(value.y ?? fallback?.[1] ?? 0),
            Number(value.z ?? fallback?.[2] ?? 0)
          ].slice(0, length);
        }
        return fallback ? fallback.slice(0, length) : null;
      }

      function readNaturalRunArray(source, paths, length, fallback) {
        for (const path of paths) {
          const value = path.includes(".") ? naturalRunPath(source, path) : source?.[path];
          const result = naturalRunArray(value, length, null);
          if (result) {
            return result;
          }
        }
        return fallback.slice(0, length);
      }

      function readNaturalRunTransform(source, paths, fallback) {
        for (const path of paths) {
          const value = path.includes(".") ? naturalRunPath(source, path) : source?.[path];
          if (Array.isArray(value)) {
            return naturalRunArray(value, 6, fallback);
          }
          if (value && typeof value === "object") {
            const position = naturalRunArray(
              value.position || value.location || value.translation || value,
              3,
              fallback.slice(0, 3)
            );
            const explicitRotation = naturalRunArray(value.rotation || value.rot || value.euler, 3, null);
            const namedRotation = ["pitch", "yaw", "roll"].some((name) => name in value)
              ? [
                Number(value.pitch ?? fallback[3] ?? 0),
                Number(value.yaw ?? fallback[4] ?? 0),
                Number(value.roll ?? fallback[5] ?? 0)
              ]
              : null;
            const rotation = explicitRotation || namedRotation || fallback.slice(3, 6);
            return position.concat(rotation);
          }
        }
        return fallback.slice(0, 6);
      }

      function readNaturalRunRotation(source, paths, fallback) {
        for (const path of paths) {
          const value = path.includes(".") ? naturalRunPath(source, path) : source?.[path];
          const direct = naturalRunArray(value, 3, null);
          if (direct) {
            return direct;
          }
          if (value && typeof value === "object") {
            const rotation = naturalRunArray(value.rotation || value.rot || value.euler, 3, null);
            if (rotation) {
              return rotation;
            }
            if (["pitch", "yaw", "roll"].some((name) => name in value)) {
              return [
                Number(value.pitch ?? fallback[0] ?? 0),
                Number(value.yaw ?? fallback[1] ?? 0),
                Number(value.roll ?? fallback[2] ?? 0)
              ];
            }
          }
        }
        return fallback.slice(0, 3);
      }

      function vecArray(vector) {
        return [vector.x, vector.y, vector.z];
      }

      function makeNaturalRunFallbackKey(index) {
        const fallbackConfig = {
          stepLength: 39,
          footLift: 31,
          footSpacing: 8.8,
          ballSpacing: 9.8,
          ballForward: 18.2,
          stanceRatio: 0.24,
          footContactDrop: 0.55
        };
        const phase = (index % frameCount) / frameCount;
        const leftFoot = runFootAt((phase + 0.5) % 1, 1, fallbackConfig);
        const rightFoot = runFootAt(phase, -1, fallbackConfig);
        const support = leftFoot.planted || rightFoot.planted;
        const impact = Math.max(leftFoot.impact, rightFoot.impact);
        const push = Math.max(leftFoot.push, rightFoot.push);
        const stridePhase = (phase * 2) % 1;
        const stanceSpan = fallbackConfig.stanceRatio * 2;
        const flightProgress = clamp((stridePhase - stanceSpan) / Math.max(0.001, 1 - stanceSpan), 0, 1);
        const flightArc = support ? 0 : Math.sin(flightProgress * Math.PI);
        const sideSway = wave(phase + 0.25) * 0.72;
        const armSwing = wave(phase);
        const lean = 12.5 + flightArc * 3.0 - impact * 2.4;
        const leftDrive = -armSwing;
        const rightDrive = armSwing;
        const leftElbowDrive = leftDrive * 0.56;
        const rightElbowDrive = rightDrive * 0.56;
        const leftLift = Math.max(0, leftDrive) * 7.0 - Math.max(0, -leftDrive) * 4.8;
        const rightLift = Math.max(0, rightDrive) * 7.0 - Math.max(0, -rightDrive) * 4.8;
        const pelvisHeight = -impact * 4.2 + push * 2.0 + flightArc * 8.0;
        const airborneLift = support ? 0 : flightArc * 5.8;
        return {
          frame: index,
          pelvis: [sideSway, pelvisHeight, 1.8 + push * 0.7, lean, armSwing * 8.8, wave(phase + 0.2) * 4.1],
          chest: [lean * 0.58, -armSwing * 9.8, -wave(phase + 0.2) * 2.7],
          neck: [-lean * 0.20, -armSwing * 2.2, wave(phase + 0.1) * 0.9],
          head: [-lean * 0.16, -armSwing * 1.5, wave(phase + 0.1) * 0.6],
          lUpper: [12.0, -11.4 + Math.max(0, leftDrive) * 0.8, leftElbowDrive * 30 + lean * 0.34],
          lLower: [7.4, -17.0 - Math.abs(leftDrive) * 1.7, leftElbowDrive * 18 + lean * 0.08],
          lHand: [3.1, -12.6 + leftLift * 1.12, leftDrive * 28 - 1.0],
          rUpper: [-12.0, -11.4 + Math.max(0, rightDrive) * 0.8, rightElbowDrive * 30 + lean * 0.34],
          rLower: [-7.4, -17.0 - Math.abs(rightDrive) * 1.7, rightElbowDrive * 18 + lean * 0.08],
          rHand: [-3.1, -12.6 + rightLift * 1.12, rightDrive * 28 - 1.0],
          leftFoot: {
            ankle: vecArray(leftFoot.ankle.clone().add(v(0, airborneLift, 0))),
            ball: vecArray(leftFoot.ball.clone().add(v(0, airborneLift * 0.65, 0))),
            planted: leftFoot.planted,
            impact: leftFoot.impact,
            push: leftFoot.push
          },
          rightFoot: {
            ankle: vecArray(rightFoot.ankle.clone().add(v(0, airborneLift, 0))),
            ball: vecArray(rightFoot.ball.clone().add(v(0, airborneLift * 0.65, 0))),
            planted: rightFoot.planted,
            impact: rightFoot.impact,
            push: rightFoot.push
          },
          leftKneeBias: [0.5, -leftFoot.impact * 3.2, leftFoot.push * 2.2],
          rightKneeBias: [-0.5, -rightFoot.impact * 3.2, rightFoot.push * 2.2]
        };
      }

      function readNaturalRunFoot(source, sideName, fallback) {
        const shortName = sideName === "left" ? "l" : "r";
        const foot = source?.[`${sideName}Foot`] || source?.[`${shortName}Foot`] || source?.feet?.[sideName] || {};
        const ankleFallback = readNaturalRunArray(source, [`foot_${shortName}`, `${shortName}Foot`, `${sideName}Ankle`], 3, fallback.ankle);
        const ballFallback = readNaturalRunArray(source, [`ball_${shortName}`, `${shortName}Ball`, `${sideName}Ball`], 3, fallback.ball);
        const plantedValue = foot.planted ?? source?.[`${sideName}Planted`] ?? source?.[`_${sideName}Planted`] ?? fallback.planted;
        return {
          ankle: readNaturalRunArray(foot, ["ankle", "foot", "position"], 3, ankleFallback),
          ball: readNaturalRunArray(foot, ["ball", "toe"], 3, ballFallback),
          planted: Boolean(plantedValue),
          impact: Number(foot.impact ?? source?.[`${sideName}Impact`] ?? fallback.impact ?? 0),
          push: Number(foot.push ?? source?.[`${sideName}Push`] ?? fallback.push ?? 0)
        };
      }

      function normalizeNaturalRunKey(source, fallbackIndex) {
        const fallback = makeNaturalRunFallbackKey(fallbackIndex);
        return {
          frame: readNaturalKeyFrame(source, fallback.frame),
          pelvis: readNaturalRunTransform(source, ["pelvis", "root"], fallback.pelvis),
          chest: readNaturalRunRotation(source, ["chest", "spine", "spine_05", "bones.spine_05"], fallback.chest),
          neck: readNaturalRunRotation(source, ["neck", "neck_01", "bones.neck_01"], fallback.neck),
          head: readNaturalRunRotation(source, ["head", "bones.head"], fallback.head),
          lUpper: readNaturalRunArray(source, ["lUpper", "leftUpper", "leftUpperArm", "upperarm_l"], 3, fallback.lUpper),
          lLower: readNaturalRunArray(source, ["lLower", "leftLower", "leftLowerArm", "lowerarm_l"], 3, fallback.lLower),
          lHand: readNaturalRunArray(source, ["lHand", "leftHand", "hand_l"], 3, fallback.lHand),
          rUpper: readNaturalRunArray(source, ["rUpper", "rightUpper", "rightUpperArm", "upperarm_r"], 3, fallback.rUpper),
          rLower: readNaturalRunArray(source, ["rLower", "rightLower", "rightLowerArm", "lowerarm_r"], 3, fallback.rLower),
          rHand: readNaturalRunArray(source, ["rHand", "rightHand", "hand_r"], 3, fallback.rHand),
          leftFoot: readNaturalRunFoot(source, "left", fallback.leftFoot),
          rightFoot: readNaturalRunFoot(source, "right", fallback.rightFoot),
          leftKneeBias: readNaturalRunArray(source, ["leftKneeBias", "knee_l", "calfBias_l", "leftLeg.knee.bias"], 3, fallback.leftKneeBias),
          rightKneeBias: readNaturalRunArray(source, ["rightKneeBias", "knee_r", "calfBias_r", "rightLeg.knee.bias"], 3, fallback.rightKneeBias)
        };
      }

      function naturalRunPair(index) {
        if (naturalRunRawKeys.length < 2) {
          return null;
        }
        const entries = naturalRunRawKeys
          .map((key, order) => ({ key, frame: readNaturalKeyFrame(key, order) }))
          .sort((a, b) => a.frame - b.frame);
        const sampleFrame = ((index % frameCount) + frameCount) % frameCount;
        for (let i = 0; i < entries.length - 1; i += 1) {
          const from = entries[i];
          const to = entries[i + 1];
          if (sampleFrame >= from.frame && sampleFrame <= to.frame) {
            const raw = (sampleFrame - from.frame) / Math.max(0.001, to.frame - from.frame);
            return { from, to, t: smoothstep(raw) };
          }
        }
        const first = entries[0];
        const last = entries[entries.length - 1];
        if (sampleFrame < first.frame) {
          const raw = (sampleFrame + frameCount - last.frame) / Math.max(0.001, first.frame + frameCount - last.frame);
          return { from: last, to: first, t: smoothstep(raw) };
        }
        const raw = (sampleFrame - last.frame) / Math.max(0.001, first.frame + frameCount - last.frame);
        return { from: last, to: first, t: smoothstep(raw) };
      }

      function interpolateNaturalRunArray(a, b, t) {
        const length = Math.max(a.length, b.length);
        return Array.from({ length }, (_, index) => lerp(Number(a[index] ?? 0), Number(b[index] ?? 0), t));
      }

      function interpolateNaturalRunFoot(a, b, t) {
        const bothPlanted = a.planted && b.planted;
        return {
          ankle: bothPlanted ? a.ankle.slice(0, 3) : interpolateNaturalRunArray(a.ankle, b.ankle, t),
          ball: bothPlanted ? a.ball.slice(0, 3) : interpolateNaturalRunArray(a.ball, b.ball, t),
          planted: t < 0.5 ? a.planted : b.planted,
          impact: lerp(a.impact || 0, b.impact || 0, t),
          push: lerp(a.push || 0, b.push || 0, t)
        };
      }

      function interpolateNaturalRunKey(fromSource, toSource, t) {
        const from = normalizeNaturalRunKey(fromSource, readNaturalKeyFrame(fromSource, 0));
        const to = normalizeNaturalRunKey(toSource, readNaturalKeyFrame(toSource, 0));
        return {
          pelvis: interpolateNaturalRunArray(from.pelvis, to.pelvis, t),
          chest: interpolateNaturalRunArray(from.chest, to.chest, t),
          neck: interpolateNaturalRunArray(from.neck, to.neck, t),
          head: interpolateNaturalRunArray(from.head, to.head, t),
          lUpper: interpolateNaturalRunArray(from.lUpper, to.lUpper, t),
          lLower: interpolateNaturalRunArray(from.lLower, to.lLower, t),
          lHand: interpolateNaturalRunArray(from.lHand, to.lHand, t),
          rUpper: interpolateNaturalRunArray(from.rUpper, to.rUpper, t),
          rLower: interpolateNaturalRunArray(from.rLower, to.rLower, t),
          rHand: interpolateNaturalRunArray(from.rHand, to.rHand, t),
          leftFoot: interpolateNaturalRunFoot(from.leftFoot, to.leftFoot, t),
          rightFoot: interpolateNaturalRunFoot(from.rightFoot, to.rightFoot, t),
          leftKneeBias: interpolateNaturalRunArray(from.leftKneeBias, to.leftKneeBias, t),
          rightKneeBias: interpolateNaturalRunArray(from.rightKneeBias, to.rightKneeBias, t)
        };
      }

      function sampleNaturalRunKey(index) {
        const pair = naturalRunPair(index);
        if (!pair) {
          return makeNaturalRunFallbackKey(index);
        }
        return interpolateNaturalRunKey(pair.from.key, pair.to.key, pair.t);
      }

      function naturalRunPointSource(key) {
        return key && (key.points || key.joints || key.pose);
      }

      function buildNaturalRunPointPose(index) {
        const pair = naturalRunPair(index);
        if (!pair) {
          return null;
        }
        const fromPoints = naturalRunPointSource(pair.from.key);
        const toPoints = naturalRunPointSource(pair.to.key);
        if (!fromPoints || !toPoints) {
          return null;
        }
        const points = {};
        for (const name of jointNames) {
          const a = naturalRunArray(fromPoints[name], 3, null);
          const b = naturalRunArray(toPoints[name], 3, null);
          if (!a || !b) {
            return null;
          }
          const plantedFoot = (name === "foot_l" || name === "ball_l")
            ? Boolean(pair.from.key.leftPlanted ?? pair.from.key._leftPlanted) && Boolean(pair.to.key.leftPlanted ?? pair.to.key._leftPlanted)
            : (name === "foot_r" || name === "ball_r")
              ? Boolean(pair.from.key.rightPlanted ?? pair.from.key._rightPlanted) && Boolean(pair.to.key.rightPlanted ?? pair.to.key._rightPlanted)
              : false;
          const vector = plantedFoot ? a : interpolateNaturalRunArray(a, b, pair.t);
          points[name] = v(vector[0], vector[1], vector[2]);
        }
        points._leftPlanted = pair.t < 0.5 ? Boolean(pair.from.key.leftPlanted ?? pair.from.key._leftPlanted) : Boolean(pair.to.key.leftPlanted ?? pair.to.key._leftPlanted);
        points._rightPlanted = pair.t < 0.5 ? Boolean(pair.from.key.rightPlanted ?? pair.from.key._rightPlanted) : Boolean(pair.to.key.rightPlanted ?? pair.to.key._rightPlanted);
        return points;
      }

      function keyposePair(rawKeys, index, count) {
        if (!Array.isArray(rawKeys) || rawKeys.length < 2) {
          return null;
        }
        const total = Math.max(2, Math.round(count || frameCount || 24));
        const entries = rawKeys
          .map((key, order) => ({ key, frame: readNaturalKeyFrame(key, order) }))
          .sort((a, b) => a.frame - b.frame);
        const sampleFrame = ((index % total) + total) % total;
        for (let i = 0; i < entries.length - 1; i += 1) {
          const from = entries[i];
          const to = entries[i + 1];
          if (sampleFrame >= from.frame && sampleFrame <= to.frame) {
            const raw = (sampleFrame - from.frame) / Math.max(0.001, to.frame - from.frame);
            return { from, to, t: smoothstep(raw) };
          }
        }
        const first = entries[0];
        const last = entries[entries.length - 1];
        const raw = sampleFrame < first.frame
          ? (sampleFrame + total - last.frame) / Math.max(0.001, first.frame + total - last.frame)
          : (sampleFrame - last.frame) / Math.max(0.001, first.frame + total - last.frame);
        return { from: last, to: first, t: smoothstep(raw) };
      }

      function numberOption(value, fallback) {
        const number = Number(value);
        return Number.isFinite(number) ? number : fallback;
      }

      function makeShuffleDanceFallbackKey(index) {
        const count = Math.max(2, frameCount || 24);
        const phase = (((index % count) + count) % count) / count;
        const theta = phase * Math.PI * 2;
        const beat = theta * 2;
        const fastBeat = theta * 4;
        const lateral = Math.sin(beat + 0.32) * 3.6;
        const cross = Math.sin(beat);
        const weight = Math.sin(beat + 0.45);
        const bounce = (1 - Math.cos(fastBeat)) * 0.72;

        function footForSide(side) {
          const sideBeat = beat + (side > 0 ? 0 : Math.PI);
          const liftPulse = Math.max(0, Math.sin(sideBeat));
          const heelToe = Math.sin(fastBeat + side * Math.PI * 0.55);
          const crossIn = Math.max(0, side > 0 ? cross : -cross);
          const release = Math.max(0, side > 0 ? -cross : cross);
          const slidePulse = Math.cos(sideBeat);
          const lift = Math.pow(liftPulse, 1.65) * 4.6;
          const x = side * 10.8 + lateral - side * crossIn * 13.8 + side * release * 3.2;
          const z = 8.0 + Math.cos(sideBeat) * 6.8 + Math.sin(fastBeat + side * 0.65) * 1.8;
          const contactWeight = clamp(1 - liftPulse * 0.84, 0.16, 1);
          return {
            ankle: [
              x,
              8.25 + lift + Math.max(0, heelToe) * 2.4,
              z
            ],
            ball: [
              x + side * heelToe * 2.9,
              1.2 + lift * 0.22 + Math.max(0, -heelToe) * 1.1,
              z + 14.8 + Math.cos(fastBeat + side * 0.8) * 2.8
            ],
            planted: contactWeight > 0.42,
            sliding: contactWeight > 0.55 && Math.abs(slidePulse) > 0.35,
            contactWeight
          };
        }

        const leftFoot = footForSide(1);
        const rightFoot = footForSide(-1);
        const armCounter = Math.sin(beat + Math.PI * 0.5);
        return {
          frame: index,
          pelvis: [lateral + weight * 0.6, -0.4 + bounce, -0.6, 1.0 + bounce * 0.55, Math.sin(beat) * 10.0, -weight * 5.4],
          chest: [-1.2 - bounce * 0.35, -Math.sin(beat) * 8.0, weight * 3.5],
          neck: [0.4, -Math.sin(beat) * 1.6, -weight * 0.9],
          head: [0.2, -Math.sin(beat) * 1.2, -weight * 0.7],
          lUpper: [15.8, -11.2 + Math.max(0, armCounter) * 1.8, -7.0 - armCounter * 9.8],
          lLower: [11.4, -21.5 - Math.abs(armCounter) * 2.2, 5.4 - armCounter * 4.4],
          lHand: [4.8, -16.8 + Math.max(0, armCounter) * 3.0, 4.4 - armCounter * 5.8],
          rUpper: [-15.8, -11.2 + Math.max(0, -armCounter) * 1.8, 7.0 - armCounter * 9.8],
          rLower: [-11.4, -21.5 - Math.abs(armCounter) * 2.2, -5.4 - armCounter * 4.4],
          rHand: [-4.8, -16.8 + Math.max(0, -armCounter) * 3.0, -4.4 - armCounter * 5.8],
          leftFoot,
          rightFoot,
          leftKneeBias: [cross * -2.2, -Math.max(0, cross) * 1.4, Math.sin(fastBeat) * 2.0],
          rightKneeBias: [cross * -2.2, -Math.max(0, -cross) * 1.4, -Math.sin(fastBeat) * 2.0]
        };
      }

      function readShuffleDanceArm(source, sideName, segment, fallback) {
        const shortName = sideName === "left" ? "l" : "r";
        const sidePrefix = sideName === "left" ? "left" : "right";
        const directPrefix = sideName === "left" ? "l" : "r";
        const paths = segment === "upper"
          ? [`${directPrefix}Upper`, `${sidePrefix}Upper`, `${sidePrefix}UpperArm`, `upperarm_${shortName}`, `${sidePrefix}Arm.upper`, `${sidePrefix}Arm.upperArm`, `arms.${sidePrefix}.upper`]
          : segment === "lower"
            ? [`${directPrefix}Lower`, `${sidePrefix}Lower`, `${sidePrefix}LowerArm`, `lowerarm_${shortName}`, `${sidePrefix}Arm.lower`, `${sidePrefix}Arm.forearm`, `arms.${sidePrefix}.lower`]
            : [`${directPrefix}Hand`, `${sidePrefix}Hand`, `hand_${shortName}`, `${sidePrefix}Arm.hand`, `arms.${sidePrefix}.hand`];
        return readNaturalRunArray(source, paths, 3, fallback);
      }

      function readShuffleDanceOptionalArray(source, paths, length) {
        for (const path of paths) {
          const value = path.includes(".") ? naturalRunPath(source, path) : source?.[path];
          const result = naturalRunArray(value, length, null);
          if (result) {
            return result;
          }
        }
        return null;
      }

      function readShuffleDanceFoot(source, sideName, fallback) {
        const shortName = sideName === "left" ? "l" : "r";
        const footValue = source?.[`${sideName}Foot`] || source?.[`${shortName}Foot`] || source?.feet?.[sideName];
        const foot = footValue && typeof footValue === "object" && !Array.isArray(footValue) ? footValue : {};
        const contacts = source?.contacts || source?.contact || {};
        const contactValue = contacts?.[`${sideName}Foot`] ?? contacts?.[sideName] ?? contacts?.[shortName];
        const contact = contactValue && typeof contactValue === "object" ? contactValue : {};
        const contactFlag = typeof contactValue === "boolean" ? contactValue : undefined;
        const contactWeight = numberOption(
          foot.contactWeight ?? foot.weight ?? contact.contactWeight ?? contact.weight ?? source?.[`${sideName}ContactWeight`],
          fallback.contactWeight ?? (fallback.planted ? 1 : 0)
        );
        const slideWeightValue = foot.slideWeight ?? foot.slide_weight ?? contact.slideWeight ?? contact.slide_weight;
        const weightedSlide = slideWeightValue == null ? undefined : Number(slideWeightValue) > 0.45;
        const sliding = Boolean(foot.sliding ?? foot.slide ?? weightedSlide ?? contact.sliding ?? contact.slide ?? source?.[`${sideName}Sliding`] ?? fallback.sliding);
        const locked = Boolean(foot.locked ?? foot.anchor ?? foot.anchored ?? contact.locked ?? contact.anchor ?? contact.anchored);
        const plantedValue = foot.planted ?? foot.contact ?? contact.planted ?? contact.contact ?? contactFlag ?? source?.[`${sideName}Planted`] ?? source?.[`_${sideName}Planted`];
        return {
          ankle: readNaturalRunArray(source, [`${sideName}Foot.worldAnkle`, `${sideName}Foot.ankle`, `${sideName}Foot.foot`, `${sideName}Foot.position`, `${sideName}Ankle`, `foot_${shortName}`, `${shortName}Foot`], 3, fallback.ankle),
          ball: readNaturalRunArray(source, [`${sideName}Foot.worldBall`, `${sideName}Foot.ball`, `${sideName}Foot.toe`, `${sideName}Ball`, `ball_${shortName}`, `${sideName}Toe`], 3, fallback.ball),
          planted: plantedValue == null ? contactWeight > 0.42 || sliding : Boolean(plantedValue),
          sliding,
          locked,
          contactWeight
        };
      }

      function readShuffleDanceNamedRotation(value, fallback) {
        if (value && typeof value === "object" && ["pitch", "yaw", "roll"].some((name) => name in value)) {
          return [
            Number(value.pitch ?? fallback[0] ?? 0),
            Number(value.yaw ?? fallback[1] ?? 0),
            Number(value.roll ?? fallback[2] ?? 0)
          ];
        }
        return null;
      }

      function readShuffleDanceTransform(source, paths, fallback) {
        for (const path of paths) {
          const value = path.includes(".") ? naturalRunPath(source, path) : source?.[path];
          if (Array.isArray(value)) {
            return naturalRunArray(value, 6, fallback);
          }
          if (value && typeof value === "object") {
            const position = naturalRunArray(
              value.position || value.location || value.translation || value.pos || value,
              3,
              fallback.slice(0, 3)
            );
            const rotationValue = value.rotation || value.rot || value.euler || value.angle || value;
            const rotation = naturalRunArray(rotationValue, 3, null)
              || readShuffleDanceNamedRotation(rotationValue, fallback.slice(3, 6))
              || fallback.slice(3, 6);
            return position.concat(rotation);
          }
        }
        return fallback.slice(0, 6);
      }

      function readShuffleDanceRotation(source, paths, fallback) {
        for (const path of paths) {
          const value = path.includes(".") ? naturalRunPath(source, path) : source?.[path];
          const direct = naturalRunArray(value, 3, null) || readShuffleDanceNamedRotation(value, fallback);
          if (direct) {
            return direct;
          }
          if (value && typeof value === "object") {
            const rotationValue = value.rotation || value.rot || value.euler || value.angle;
            const nested = naturalRunArray(rotationValue, 3, null) || readShuffleDanceNamedRotation(rotationValue, fallback);
            if (nested) {
              return nested;
            }
          }
        }
        return fallback.slice(0, 3);
      }

      function normalizeShuffleDanceKey(source, fallbackIndex) {
        const fallback = makeShuffleDanceFallbackKey(fallbackIndex);
        return {
          frame: readNaturalKeyFrame(source, fallback.frame),
          pelvis: readShuffleDanceTransform(source, ["pelvis", "root", "body.root"], fallback.pelvis),
          chest: readShuffleDanceRotation(source, ["chest", "spine", "spine_05", "spine.chest", "bones.spine_05"], fallback.chest),
          neck: readShuffleDanceRotation(source, ["neck", "neck_01", "bones.neck_01"], fallback.neck),
          head: readShuffleDanceRotation(source, ["head", "bones.head"], fallback.head),
          lUpper: readShuffleDanceArm(source, "left", "upper", fallback.lUpper),
          lLower: readShuffleDanceArm(source, "left", "lower", fallback.lLower),
          lHand: readShuffleDanceArm(source, "left", "hand", fallback.lHand),
          rUpper: readShuffleDanceArm(source, "right", "upper", fallback.rUpper),
          rLower: readShuffleDanceArm(source, "right", "lower", fallback.rLower),
          rHand: readShuffleDanceArm(source, "right", "hand", fallback.rHand),
          leftHandTarget: readShuffleDanceOptionalArray(source, ["leftArm.handTarget", "leftArm.hand", "leftHandTarget"], 3),
          rightHandTarget: readShuffleDanceOptionalArray(source, ["rightArm.handTarget", "rightArm.hand", "rightHandTarget"], 3),
          leftFoot: readShuffleDanceFoot(source, "left", fallback.leftFoot),
          rightFoot: readShuffleDanceFoot(source, "right", fallback.rightFoot),
          leftKneeBias: readNaturalRunArray(source, ["leftKneeBias", "kneeBias.left", "knee_l", "leftLeg.kneeBias", "leftLeg.kneeOffset"], 3, fallback.leftKneeBias),
          rightKneeBias: readNaturalRunArray(source, ["rightKneeBias", "kneeBias.right", "knee_r", "rightLeg.kneeBias", "rightLeg.kneeOffset"], 3, fallback.rightKneeBias)
        };
      }

      function interpolateShuffleDanceFoot(a, b, t) {
        const contactWeight = lerp(a.contactWeight || 0, b.contactWeight || 0, t);
        const locked = Boolean(a.locked && b.locked && a.planted && b.planted);
        return {
          ankle: locked ? a.ankle.slice(0, 3) : interpolateNaturalRunArray(a.ankle, b.ankle, t),
          ball: locked ? a.ball.slice(0, 3) : interpolateNaturalRunArray(a.ball, b.ball, t),
          planted: contactWeight > 0.42 || (t < 0.5 ? a.planted : b.planted),
          sliding: t < 0.5 ? a.sliding : b.sliding,
          locked,
          contactWeight
        };
      }

      function interpolateShuffleDanceKey(fromSource, toSource, t) {
        const from = normalizeShuffleDanceKey(fromSource, readNaturalKeyFrame(fromSource, 0));
        const to = normalizeShuffleDanceKey(toSource, readNaturalKeyFrame(toSource, 0));
        return {
          pelvis: interpolateNaturalRunArray(from.pelvis, to.pelvis, t),
          chest: interpolateNaturalRunArray(from.chest, to.chest, t),
          neck: interpolateNaturalRunArray(from.neck, to.neck, t),
          head: interpolateNaturalRunArray(from.head, to.head, t),
          lUpper: interpolateNaturalRunArray(from.lUpper, to.lUpper, t),
          lLower: interpolateNaturalRunArray(from.lLower, to.lLower, t),
          lHand: interpolateNaturalRunArray(from.lHand, to.lHand, t),
          rUpper: interpolateNaturalRunArray(from.rUpper, to.rUpper, t),
          rLower: interpolateNaturalRunArray(from.rLower, to.rLower, t),
          rHand: interpolateNaturalRunArray(from.rHand, to.rHand, t),
          leftHandTarget: from.leftHandTarget && to.leftHandTarget ? interpolateNaturalRunArray(from.leftHandTarget, to.leftHandTarget, t) : null,
          rightHandTarget: from.rightHandTarget && to.rightHandTarget ? interpolateNaturalRunArray(from.rightHandTarget, to.rightHandTarget, t) : null,
          leftFoot: interpolateShuffleDanceFoot(from.leftFoot, to.leftFoot, t),
          rightFoot: interpolateShuffleDanceFoot(from.rightFoot, to.rightFoot, t),
          leftKneeBias: interpolateNaturalRunArray(from.leftKneeBias, to.leftKneeBias, t),
          rightKneeBias: interpolateNaturalRunArray(from.rightKneeBias, to.rightKneeBias, t)
        };
      }

      function sampleShuffleDanceKey(index) {
        const pair = keyposePair(shuffleDanceRawKeys, index, frameCount);
        if (!pair) {
          return makeShuffleDanceFallbackKey(index);
        }
        return interpolateShuffleDanceKey(pair.from.key, pair.to.key, pair.t);
      }

      function buildShuffleDancePointPose(index) {
        const pair = keyposePair(shuffleDanceRawKeys, index, frameCount);
        if (!pair) {
          return null;
        }
        const fromPoints = naturalRunPointSource(pair.from.key);
        const toPoints = naturalRunPointSource(pair.to.key);
        if (!fromPoints || !toPoints) {
          return null;
        }
        const leftFoot = interpolateShuffleDanceFoot(
          readShuffleDanceFoot(pair.from.key, "left", makeShuffleDanceFallbackKey(pair.from.frame).leftFoot),
          readShuffleDanceFoot(pair.to.key, "left", makeShuffleDanceFallbackKey(pair.to.frame).leftFoot),
          pair.t
        );
        const rightFoot = interpolateShuffleDanceFoot(
          readShuffleDanceFoot(pair.from.key, "right", makeShuffleDanceFallbackKey(pair.from.frame).rightFoot),
          readShuffleDanceFoot(pair.to.key, "right", makeShuffleDanceFallbackKey(pair.to.frame).rightFoot),
          pair.t
        );
        const points = {};
        for (const name of jointNames) {
          const a = naturalRunArray(fromPoints[name], 3, null);
          const b = naturalRunArray(toPoints[name], 3, null);
          if (!a || !b) {
            return null;
          }
          const locked = ((name === "foot_l" || name === "ball_l") && leftFoot.locked)
            || ((name === "foot_r" || name === "ball_r") && rightFoot.locked);
          const vector = locked ? a : interpolateNaturalRunArray(a, b, pair.t);
          points[name] = v(vector[0], vector[1], vector[2]);
        }
        points._leftPlanted = leftFoot.planted;
        points._rightPlanted = rightFoot.planted;
        return points;
      }

      function solveKnee(hip, ankle, side) {
        const l1 = 43.3;
        const l2 = 42.2;
        const delta = ankle.clone().sub(hip);
        const distance = clamp(delta.length(), 1, l1 + l2 - 0.25);
        const direction = delta.clone().normalize();
        const along = (l1 * l1 - l2 * l2 + distance * distance) / (2 * distance);
        const height = Math.sqrt(Math.max(0, l1 * l1 - along * along));
        const bend = new THREE.Vector3(0, -direction.z, direction.y).normalize();
        if (bend.z < 0) {
          bend.multiplyScalar(-1);
        }
        return hip.clone()
          .add(direction.multiplyScalar(along))
          .add(bend.multiplyScalar(height * 0.72))
          .add(new THREE.Vector3(side * 0.8, 0, 0));
      }

      function solveElbow(shoulder, hand, side, bias = v(0, 0, 0)) {
        const upper = 31.5;
        const lower = 29.5;
        const delta = hand.clone().sub(shoulder);
        const distance = clamp(delta.length(), 1, upper + lower - 0.25);
        const direction = delta.clone().normalize();
        const along = (upper * upper - lower * lower + distance * distance) / (2 * distance);
        const height = Math.sqrt(Math.max(0, upper * upper - along * along));
        const bend = new THREE.Vector3(side * 0.82, -0.35, -direction.z * 0.45).normalize();
        return shoulder.clone()
          .add(direction.multiplyScalar(along))
          .add(bend.multiplyScalar(height * 0.62))
          .add(bias);
      }

      function clampLimbTarget(origin, target, maxDistance) {
        const delta = target.clone().sub(origin);
        const distance = delta.length();
        if (distance <= maxDistance || distance < 0.001) {
          return target.clone();
        }
        return origin.clone().add(delta.normalize().multiplyScalar(maxDistance));
      }

      function blendValidJoint(rawJoint, solvedJoint, start, end, maxStart, maxEnd, blend = 0.32) {
        if (
          rawJoint.distanceTo(start) <= maxStart &&
          rawJoint.distanceTo(end) <= maxEnd
        ) {
          return solvedJoint.clone().lerp(rawJoint, blend);
        }
        return solvedJoint;
      }

      function wallClimbEntries() {
        return wallClimbRawKeys
          .map((key, order) => ({ key, frame: readNaturalKeyFrame(key, order) }))
          .sort((a, b) => a.frame - b.frame);
      }

      function wallClimbPair(index) {
        const entries = wallClimbEntries();
        if (entries.length < 2) {
          return null;
        }
        const sampleFrame = clamp(index % frameCount, 0, frameCount - 1);
        if (sampleFrame <= entries[0].frame) {
          return { from: entries[0], to: entries[0], t: 0 };
        }
        for (let i = 0; i < entries.length - 1; i += 1) {
          const from = entries[i];
          const to = entries[i + 1];
          if (sampleFrame >= from.frame && sampleFrame <= to.frame) {
            const raw = (sampleFrame - from.frame) / Math.max(0.001, to.frame - from.frame);
            return { from, to, t: smoothstep(raw) };
          }
        }
        const last = entries[entries.length - 1];
        return { from: last, to: last, t: 0 };
      }

      function wallClimbVec(value, fallback) {
        if (Array.isArray(value)) {
          return naturalRunArray(value, 3, fallback);
        }
        if (value && typeof value === "object") {
          return naturalRunArray(value.position || value.location || value.translation || value.target || value, 3, fallback);
        }
        return fallback ? fallback.slice(0, 3) : null;
      }

      function wallClimbContactValue(source, name, fallback = null) {
        if (!source) {
          return fallback ? fallback.slice(0, 3) : null;
        }
        const compact = name.replace("_", "");
        const camel = name.replace(/_([a-z])/g, (_, letter) => letter.toUpperCase());
        const candidates = [
          source?.contacts?.[name],
          source?.contacts?.[compact],
          source?.contacts?.[camel],
          source?.contactTargets?.[name],
          source?.contactTargets?.[compact],
          source?.plants?.[name],
          source?.planted?.[name],
          source?.[`${camel}Contact`],
          source?.[`${camel}Planted`],
          source?.[`_${camel}Planted`]
        ];
        for (const candidate of candidates) {
          if (typeof candidate === "string" && contactActive(candidate)) {
            const namedTarget = {
              hand_l: source.leftHand?.pos || source.leftHand?.point || source.leftHand?.position || source.lHand,
              hand_r: source.rightHand?.pos || source.rightHand?.point || source.rightHand?.position || source.rHand,
              foot_l: source.leftFoot?.ankle || source.leftFoot?.foot || source.leftFoot?.position || source.lFoot,
              foot_r: source.rightFoot?.ankle || source.rightFoot?.foot || source.rightFoot?.position || source.rFoot,
              ball_l: source.leftFoot?.ball || source.leftFoot?.toe || source.leftBall,
              ball_r: source.rightFoot?.ball || source.rightFoot?.toe || source.rightBall
            }[name];
            const contactTarget = wallClimbVec(namedTarget, null);
            if (contactTarget) {
              return contactTarget;
            }
          }
          if (candidate === true) {
            const direct = wallClimbVec(source?.[name] ?? source?.[compact] ?? source?.[camel], null);
            return direct || (fallback ? fallback.slice(0, 3) : true);
          }
          const vector = wallClimbVec(candidate, null);
          if (vector) {
            return vector;
          }
        }
        return fallback ? fallback.slice(0, 3) : null;
      }

      function wallClimbContactsFrom(source, fallback) {
        const contacts = {};
        ["hand_l", "hand_r", "foot_l", "foot_r", "ball_l", "ball_r"].forEach((name) => {
          const value = wallClimbContactValue(source, name, fallback?.[name] || null);
          if (value && value !== true) {
            contacts[name] = value.slice(0, 3);
          }
        });
        return contacts;
      }

      function makeWallClimbFallbackKey(index) {
        const progress = frameCount <= 1 ? 0 : index / (frameCount - 1);
        const wallRun = smoothstep(clamp((progress - 0.16) / 0.24, 0, 1));
        const catchHands = smoothstep(clamp((progress - 0.30) / 0.16, 0, 1));
        const pullUp = smoothstep(clamp((progress - 0.42) / 0.24, 0, 1));
        const standUp = smoothstep(clamp((progress - 0.70) / 0.22, 0, 1));
        const wallZ = wallClimbWall.frontZ - 1.2;
        const topY = wallClimbWall.topY;
        const topZ = wallClimbWall.z + 17;
        const pelvisZ = lerp(lerp(-52, wallZ - 9, wallRun), topZ, standUp);
        const climbY = lerp(94.5, topY - 3, pullUp);
        const pelvisY = lerp(climbY, topY + 94.5, standUp);
        const forwardLean = lerp(10, 27, wallRun * (1 - standUp)) - standUp * 18;
        const twist = Math.sin(progress * Math.PI * 2) * (1 - standUp);
        const leftWallFoot = [10.8, lerp(8.3, topY * 0.48, wallRun), wallZ];
        const rightWallFoot = [-10.8, lerp(8.3, topY * 0.30, wallRun), wallZ];
        const leftTopFoot = [10.8, topY + 8.3, topZ + 2];
        const rightTopFoot = [-10.8, topY + 8.3, topZ - 8];
        const leftFoot = interpolateNaturalRunArray(leftWallFoot, leftTopFoot, standUp);
        const rightFoot = interpolateNaturalRunArray(rightWallFoot, rightTopFoot, standUp);
        const leftBall = [leftFoot[0] + 1.2, Math.max(1.2, leftFoot[1] - 7.1), leftFoot[2] + 13.6];
        const rightBall = [rightFoot[0] - 1.2, Math.max(1.2, rightFoot[1] - 7.1), rightFoot[2] + 13.6];
        const leftHand = interpolateNaturalRunArray([15.0, 98, wallZ - 4], [16.0, topY + 4.5, wallZ - 2.2], catchHands);
        const rightHand = interpolateNaturalRunArray([-15.0, 100, wallZ - 4], [-16.0, topY + 4.5, wallZ - 2.2], catchHands);
        const release = standUp > 0.58;
        const contacts = {};
        if (catchHands > 0.55 && !release) {
          contacts.hand_l = [16.0, topY + 4.5, wallZ - 2.2];
          contacts.hand_r = [-16.0, topY + 4.5, wallZ - 2.2];
        }
        if (wallRun > 0.52 && standUp < 0.35) {
          contacts.foot_l = leftWallFoot;
          contacts.foot_r = rightWallFoot;
        }
        if (standUp > 0.82) {
          contacts.foot_l = leftTopFoot;
          contacts.foot_r = rightTopFoot;
          contacts.ball_l = [leftTopFoot[0] + 1.2, topY + 1.2, leftTopFoot[2] + 13.6];
          contacts.ball_r = [rightTopFoot[0] - 1.2, topY + 1.2, rightTopFoot[2] + 13.6];
        }
        return {
          frame: index,
          pelvis: [Math.sin(progress * Math.PI * 2) * 0.8, pelvisY - 94.5, pelvisZ, forwardLean, twist * 10, -twist * 5],
          chest: [forwardLean * 0.64 - standUp * 6, -twist * 8, twist * 4],
          neck: [-forwardLean * 0.18, -twist * 2.2, 0],
          head: [-forwardLean * 0.14, -twist * 2.0, 0],
          lUpper: interpolateNaturalRunArray([14, -13, 13], [14, 23, 18], catchHands * (1 - standUp * 0.7)),
          lLower: interpolateNaturalRunArray([11, -22, 7], [2, 18, 6], catchHands * (1 - standUp * 0.55)),
          lHand: [leftHand[0] - 18, leftHand[1] - Math.min(pelvisY + 48, topY + 20), leftHand[2] - pelvisZ],
          rUpper: interpolateNaturalRunArray([-14, -13, 13], [-14, 23, 18], catchHands * (1 - standUp * 0.7)),
          rLower: interpolateNaturalRunArray([-11, -22, 7], [-2, 18, 6], catchHands * (1 - standUp * 0.55)),
          rHand: [rightHand[0] + 18, rightHand[1] - Math.min(pelvisY + 48, topY + 20), rightHand[2] - pelvisZ],
          leftFoot: { ankle: leftFoot, ball: leftBall, planted: Boolean(contacts.foot_l || contacts.ball_l) },
          rightFoot: { ankle: rightFoot, ball: rightBall, planted: Boolean(contacts.foot_r || contacts.ball_r) },
          leftKneeBias: [3.0, -2.8, wallRun * 7.0 - standUp * 3.0],
          rightKneeBias: [-3.0, -2.8, wallRun * 7.0 - standUp * 3.0],
          contacts
        };
      }

      function readWallClimbFoot(source, sideName, fallback) {
        const shortName = sideName === "left" ? "l" : "r";
        const foot = source?.[`${sideName}Foot`] || source?.[`${shortName}Foot`] || source?.feet?.[sideName] || {};
        const ankleFallback = readNaturalRunArray(source, [`foot_${shortName}`, `${shortName}Foot`, `${sideName}Ankle`], 3, fallback.ankle);
        const ballFallback = readNaturalRunArray(source, [`ball_${shortName}`, `${shortName}Ball`, `${sideName}Ball`], 3, fallback.ball);
        const plantedValue = foot.planted ?? source?.[`${sideName}Planted`] ?? source?.[`_${sideName}Planted`] ?? fallback.planted;
        return {
          ankle: wallClimbVec(foot.ankle || foot.foot || foot.position, ankleFallback),
          ball: wallClimbVec(foot.ball || foot.toe, ballFallback),
          planted: Boolean(plantedValue),
          impact: Number(foot.impact ?? source?.[`${sideName}Impact`] ?? fallback.impact ?? 0),
          push: Number(foot.push ?? source?.[`${sideName}Push`] ?? fallback.push ?? 0)
        };
      }

      function normalizeWallClimbKey(source, fallbackIndex) {
        const fallback = makeWallClimbFallbackKey(fallbackIndex);
        const contacts = wallClimbContactsFrom(source, fallback.contacts);
        return {
          frame: readNaturalKeyFrame(source, fallback.frame),
          pelvis: readNaturalRunTransform(source, ["pelvis", "root", "hips"], fallback.pelvis),
          chest: readNaturalRunRotation(source, ["chest", "spine", "spine_05", "bones.spine_05"], fallback.chest),
          neck: readNaturalRunRotation(source, ["neck", "neck_01", "bones.neck_01"], fallback.neck),
          head: readNaturalRunRotation(source, ["head", "bones.head"], fallback.head),
          lUpper: readNaturalRunArray(source, ["lUpper", "leftUpper", "leftUpperArm", "upperarm_l"], 3, fallback.lUpper),
          lLower: readNaturalRunArray(source, ["lLower", "leftLower", "leftLowerArm", "lowerarm_l"], 3, fallback.lLower),
          lHand: readNaturalRunArray(source, ["lHand", "leftHand", "hand_l"], 3, fallback.lHand),
          rUpper: readNaturalRunArray(source, ["rUpper", "rightUpper", "rightUpperArm", "upperarm_r"], 3, fallback.rUpper),
          rLower: readNaturalRunArray(source, ["rLower", "rightLower", "rightLowerArm", "lowerarm_r"], 3, fallback.rLower),
          rHand: readNaturalRunArray(source, ["rHand", "rightHand", "hand_r"], 3, fallback.rHand),
          leftFoot: readWallClimbFoot(source, "left", fallback.leftFoot),
          rightFoot: readWallClimbFoot(source, "right", fallback.rightFoot),
          leftKneeBias: readNaturalRunArray(source, ["leftKneeBias", "knee_l", "calfBias_l", "leftLeg.knee.bias"], 3, fallback.leftKneeBias),
          rightKneeBias: readNaturalRunArray(source, ["rightKneeBias", "knee_r", "calfBias_r", "rightLeg.knee.bias"], 3, fallback.rightKneeBias),
          contacts
        };
      }

      function interpolateWallClimbContact(fromContacts, toContacts, name, t) {
        const from = fromContacts[name];
        const to = toContacts[name];
        if (from && to) {
          return from.slice(0, 3);
        }
        if (from || to) {
          return (t < 0.5 ? from : to)?.slice(0, 3) || null;
        }
        return null;
      }

      function interpolateWallClimbKey(fromSource, toSource, t) {
        const from = normalizeWallClimbKey(fromSource, readNaturalKeyFrame(fromSource, 0));
        const to = normalizeWallClimbKey(toSource, readNaturalKeyFrame(toSource, 0));
        const contacts = {};
        ["hand_l", "hand_r", "foot_l", "foot_r", "ball_l", "ball_r"].forEach((name) => {
          const value = interpolateWallClimbContact(from.contacts, to.contacts, name, t);
          if (value) {
            contacts[name] = value;
          }
        });
        return {
          pelvis: interpolateNaturalRunArray(from.pelvis, to.pelvis, t),
          chest: interpolateNaturalRunArray(from.chest, to.chest, t),
          neck: interpolateNaturalRunArray(from.neck, to.neck, t),
          head: interpolateNaturalRunArray(from.head, to.head, t),
          lUpper: interpolateNaturalRunArray(from.lUpper, to.lUpper, t),
          lLower: interpolateNaturalRunArray(from.lLower, to.lLower, t),
          lHand: interpolateNaturalRunArray(from.lHand, to.lHand, t),
          rUpper: interpolateNaturalRunArray(from.rUpper, to.rUpper, t),
          rLower: interpolateNaturalRunArray(from.rLower, to.rLower, t),
          rHand: interpolateNaturalRunArray(from.rHand, to.rHand, t),
          leftFoot: interpolateNaturalRunFoot(from.leftFoot, to.leftFoot, t),
          rightFoot: interpolateNaturalRunFoot(from.rightFoot, to.rightFoot, t),
          leftKneeBias: interpolateNaturalRunArray(from.leftKneeBias, to.leftKneeBias, t),
          rightKneeBias: interpolateNaturalRunArray(from.rightKneeBias, to.rightKneeBias, t),
          contacts
        };
      }

      function sampleWallClimbKey(index) {
        const pair = wallClimbPair(index);
        if (!pair) {
          return makeWallClimbFallbackKey(index);
        }
        return interpolateWallClimbKey(pair.from.key, pair.to.key, pair.t);
      }

      function wallClimbPointSource(key) {
        return key && (key.points || key.joints || key.pose);
      }

      function buildWallClimbPointPose(index) {
        const pair = wallClimbPair(index);
        if (!pair) {
          return null;
        }
        const fromPoints = wallClimbPointSource(pair.from.key);
        const toPoints = wallClimbPointSource(pair.to.key);
        if (!fromPoints || !toPoints) {
          return null;
        }
        const points = {};
        for (const name of jointNames) {
          const a = wallClimbVec(fromPoints[name], null);
          const b = wallClimbVec(toPoints[name], null);
          if (!a || !b) {
            return null;
          }
          const fixed = ["hand_l", "hand_r", "foot_l", "foot_r", "ball_l", "ball_r"].includes(name)
            && wallClimbContactValue(pair.from.key, name, null)
            && wallClimbContactValue(pair.to.key, name, null);
          const value = fixed ? a : interpolateNaturalRunArray(a, b, pair.t);
          points[name] = v(...value);
        }
        const contacts = wallClimbContactsFrom(pair.from.key, null);
        applyWallClimbContacts(points, contacts);
        points._leftPlanted = Boolean(contacts.foot_l || contacts.ball_l);
        points._rightPlanted = Boolean(contacts.foot_r || contacts.ball_r);
        return points;
      }

      function applyWallClimbContacts(points, contacts) {
        if (contacts.hand_l) {
          points.hand_l = clampLimbTarget(points.upperarm_l, v(...contacts.hand_l), 58.0);
          points.lowerarm_l = solveElbow(points.upperarm_l, points.hand_l, 1, v(3.0, -1.2, -2.4));
        }
        if (contacts.hand_r) {
          points.hand_r = clampLimbTarget(points.upperarm_r, v(...contacts.hand_r), 58.0);
          points.lowerarm_r = solveElbow(points.upperarm_r, points.hand_r, -1, v(-3.0, -1.2, -2.4));
        }
        if (contacts.foot_l) {
          points.foot_l = clampLimbTarget(points.thigh_l, v(...contacts.foot_l), 82.0);
        }
        if (contacts.foot_r) {
          points.foot_r = clampLimbTarget(points.thigh_r, v(...contacts.foot_r), 82.0);
        }
        if (contacts.ball_l) {
          points.ball_l = v(...contacts.ball_l);
        } else if (contacts.foot_l) {
          points.ball_l = points.foot_l.clone().add(v(1.2, -7.1, 12.5));
        }
        if (contacts.ball_r) {
          points.ball_r = v(...contacts.ball_r);
        } else if (contacts.foot_r) {
          points.ball_r = points.foot_r.clone().add(v(-1.2, -7.1, 12.5));
        }
        if (contacts.foot_l || contacts.ball_l) {
          points.calf_l = solveKnee(points.thigh_l, points.foot_l, 1);
        }
        if (contacts.foot_r || contacts.ball_r) {
          points.calf_r = solveKnee(points.thigh_r, points.foot_r, -1);
        }
      }

      function buildWallClimbPose(index) {
        const pointPose = buildWallClimbPointPose(index);
        if (pointPose) {
          return pointPose;
        }

        const key = sampleWallClimbKey(index);
        const deg = THREE.MathUtils.degToRad;
        const points = {};
        const pelvisYaw = deg(key.pelvis[4] || 0);
        const pelvisPitch = deg(key.pelvis[3] || 0);
        const pelvisRoll = deg(key.pelvis[5] || 0);
        const chestYaw = deg(key.chest[1] || 0);
        const chestPitch = deg(key.chest[0] || 0);
        const chestRoll = deg(key.chest[2] || 0);
        const neckYaw = deg(key.neck[1] || 0);
        const neckPitch = deg(key.neck[0] || 0);
        const neckRoll = deg(key.neck[2] || 0);
        const headYaw = deg(key.head[1] || 0);
        const headPitch = deg(key.head[0] || 0);
        const headRoll = deg(key.head[2] || 0);
        const pelvisLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), pelvisYaw, pelvisPitch, pelvisRoll);
        const chestLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), chestYaw, chestPitch, chestRoll);
        const neckLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), neckYaw, neckPitch, neckRoll);
        const headLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), headYaw, headPitch, headRoll);

        const pelvisY = Math.abs(key.pelvis[1]) > 40 ? key.pelvis[1] : 94.5 + key.pelvis[1];
        const pelvis = v(key.pelvis[0], pelvisY, key.pelvis[2]);
        points.pelvis = pelvis;
        points.spine_01 = pelvis.clone().add(pelvisLocal(0, 12.4, 1.2));
        points.spine_02 = points.spine_01.clone().add(pelvisLocal(0.1, 12.8, 1.4));
        points.spine_03 = points.spine_02.clone().add(chestLocal(0.2, 12.8, 1.5));
        points.spine_04 = points.spine_03.clone().add(chestLocal(0.4, 10.0, 1.2));
        points.spine_05 = points.spine_04.clone().add(chestLocal(0.5, 9.7, 0.9));
        points.neck_01 = points.spine_05.clone().add(chestLocal(0.2, 12.2, 0.4)).add(neckLocal(0, 0.5, 0.1));
        points.head = points.neck_01.clone().add(headLocal(0.1, 16.2, 0.3));
        points.clavicle_l = points.spine_05.clone().add(chestLocal(17.4, 2.8, 1.1));
        points.clavicle_r = points.spine_05.clone().add(chestLocal(-17.4, 2.8, -1.1));

        points.upperarm_l = points.clavicle_l.clone().add(chestLocal(...key.lUpper));
        points.lowerarm_l = points.upperarm_l.clone().add(chestLocal(...key.lLower));
        points.hand_l = points.lowerarm_l.clone().add(chestLocal(...key.lHand));
        points.upperarm_r = points.clavicle_r.clone().add(chestLocal(...key.rUpper));
        points.lowerarm_r = points.upperarm_r.clone().add(chestLocal(...key.rLower));
        points.hand_r = points.lowerarm_r.clone().add(chestLocal(...key.rHand));

        points.thigh_l = pelvis.clone().add(pelvisLocal(10.1, -3.8, 1.2));
        points.thigh_r = pelvis.clone().add(pelvisLocal(-10.1, -3.8, -1.2));
        points.foot_l = v(...key.leftFoot.ankle);
        points.ball_l = v(...key.leftFoot.ball);
        points.foot_r = v(...key.rightFoot.ankle);
        points.ball_r = v(...key.rightFoot.ball);
        points.calf_l = solveKnee(points.thigh_l, points.foot_l, 1).add(v(...key.leftKneeBias));
        points.calf_r = solveKnee(points.thigh_r, points.foot_r, -1).add(v(...key.rightKneeBias));
        applyWallClimbContacts(points, key.contacts || {});

        const faceYaw = chestYaw * 0.45 + neckYaw * 0.5 + headYaw;
        const facePitch = chestPitch * 0.25 + neckPitch * 0.45 + headPitch;
        const faceRoll = chestRoll * 0.25 + neckRoll * 0.45 + headRoll;
        points._faceRight = rotateYawPitchRoll(v(1, 0, 0), faceYaw, facePitch, faceRoll).normalize();
        points._faceUp = rotateYawPitchRoll(v(0, 1, 0), faceYaw, facePitch, faceRoll).normalize();
        points._faceForward = new THREE.Vector3().crossVectors(points._faceRight, points._faceUp).normalize();
        points._leftPlanted = Boolean(key.leftFoot.planted || key.contacts?.foot_l || key.contacts?.ball_l);
        points._rightPlanted = Boolean(key.rightFoot.planted || key.contacts?.foot_r || key.contacts?.ball_r);
        return points;
      }

      function buildPose(index) {
        if (currentMode === "overheadWave") {
          return buildOverheadPose(index);
        }
        if (currentMode === "running") {
          return buildRunPose(index, walkPoseModes.running);
        }
        if (currentMode === "naturalRun") {
          return buildNaturalRunPose(index);
        }
        if (currentMode === "shuffleDance") {
          return buildShuffleDancePose(index);
        }
        if (currentMode === "longDance") {
          return buildGenericDancePose(index, longDanceRawKeys);
        }
        if (currentMode === "playfulGirlWalk") {
          return buildGenericDancePose(index, playfulGirlWalkRawKeys);
        }
        if (currentMode === "boxOverheadLift") {
          return buildGenericDancePose(index, boxOverheadLiftRawKeys);
        }
        if (currentMode === "hadoken") {
          return buildGenericDancePose(index, hadokenRawKeys);
        }
        if (currentMode === "heroLandingPose") {
          return buildStaticPointPose(heroLandingPoseRawKeys) || buildGenericDancePose(index, heroLandingPoseRawKeys);
        }
        if (currentMode === "baseballBatSwing") {
          return buildGenericDancePose(index, baseballBatSwingRawKeys);
        }
        if (currentMode === "seatedBottleDrink") {
          return buildGenericPointPose(index, seatedBottleDrinkRawKeys) || buildGenericDancePose(index, seatedBottleDrinkRawKeys);
        }
        if (currentMode === "archeryFullDraw") {
          return buildGenericDancePose(index, archeryFullDrawRawKeys);
        }
        if (currentMode === "sideKick") {
          return buildGenericDancePose(index, sideKickRawKeys);
        }
        if (currentMode === "roundhouseKick") {
          return buildGenericDancePose(index, roundhouseKickRawKeys);
        }
        if (currentMode === "jumpingRoundhouseKick") {
          return buildGenericDancePose(index, jumpingRoundhouseKickRawKeys);
        }
        if (currentMode === "lowVault") {
          return buildGenericDancePose(index, lowVaultMotion.rawKeys);
        }
        if (currentMode === "forwardRoll") {
          return buildGenericDancePose(index, forwardRollMotion.rawKeys);
        }
        if (currentMode === "stumbleRecovery") {
          return buildGenericDancePose(index, stumbleRecoveryMotion.rawKeys);
        }
        if (currentMode === "boxingCombo") {
          return buildGenericDancePose(index, boxingComboMotion.rawKeys);
        }
        if (currentMode === "doorPush") {
          return buildGenericDancePose(index, doorPushMotion.rawKeys);
        }
        if (currentMode === "floorPickup") {
          return buildGenericDancePose(index, floorPickupMotion.rawKeys);
        }
        if (currentMode === "ladderClimb") {
          return buildGenericDancePose(index, ladderClimbMotion.rawKeys);
        }
        if (currentMode === "kneeSlide") {
          return buildGenericDancePose(index, kneeSlideMotion.rawKeys);
        }
        if (currentMode === "idleShift") {
          return buildIdleShiftPose(index);
        }
        if (currentMode === "wallPeek") {
          return buildWallPeekPose(index);
        }
        if (currentMode === "wallClimb") {
          return buildWallClimbPose(index);
        }
        if (currentMode === "proneCrawl") {
          return buildProneCrawlPose(index);
        }
        if (currentMode === "swordSlash") {
          return buildSwordSlashPose(index);
        }
        if (currentMode === "crouchTwerk") {
          return buildCrouchTwerkPose(index);
        }
        return buildWalkPose(index, walkPoseModes[currentMode] || walkPoseModes.basic);
      }

      function buildWalkPose(index, config) {
        const phase = index / frameCount;
        const bob = (1 - Math.cos(phase * Math.PI * 4)) * config.bob;
        const sideSway = wave(phase) * config.sideSway;
        const chestTwist = wave(phase) * config.chestTwist;
        const pelvisYaw = wave(phase) * config.pelvisYaw;
        const pelvisRoll = wave(phase + 0.25) * config.pelvisRoll;
        const chestYaw = pelvisYaw * config.chestYawCounter;
        const chestRoll = pelvisRoll * config.chestRollCounter;
        const pelvis = v(sideSway, 94.5 + bob, 0);
        const points = {};
        const pelvisLocal = (x, y, z) => rotateYawRoll(v(x, y, z), pelvisYaw, pelvisRoll);
        const chestLocal = (x, y, z) => rotateYawRoll(v(x, y, z), chestYaw, chestRoll);
        points.pelvis = pelvis;
        points.spine_01 = pelvis.clone().add(pelvisLocal(sideSway * 0.15, 13.0, 0.5));
        points.spine_02 = points.spine_01.clone().add(pelvisLocal(sideSway * 0.12, 13.2, 0.3));
        points.spine_03 = points.spine_02.clone().add(chestLocal(chestTwist * 0.05, 13.0, 0.1));
        points.spine_04 = points.spine_03.clone().add(chestLocal(chestTwist * 0.07, 10.2, -0.1));
        points.spine_05 = points.spine_04.clone().add(chestLocal(chestTwist * 0.11, 10.0, -0.3));
        points.neck_01 = points.spine_05.clone().add(chestLocal(chestTwist * 0.04, 12.8, -0.2));
        points.head = points.neck_01.clone().add(chestLocal(chestTwist * 0.03, 16.8, 0.0));
        points.clavicle_l = points.spine_05.clone().add(chestLocal(17.8, 3.0, 0.8));
        points.clavicle_r = points.spine_05.clone().add(chestLocal(-17.8, 3.0, -0.8));

        const armSwing = wave(phase);
        points.upperarm_l = points.clavicle_l.clone().add(chestLocal(16.4 * config.armOut, -15.0 - config.armDrop, -armSwing * config.upperArmSwing));
        points.lowerarm_l = points.upperarm_l.clone().add(chestLocal(15.3 * config.armOut, -25.2, -armSwing * config.lowerArmSwing));
        points.hand_l = points.lowerarm_l.clone().add(chestLocal(7.0 * config.armOut, -24.5, -armSwing * config.handSwing));
        points.upperarm_r = points.clavicle_r.clone().add(chestLocal(-16.4 * config.armOut, -15.0 - config.armDrop, armSwing * config.upperArmSwing));
        points.lowerarm_r = points.upperarm_r.clone().add(chestLocal(-15.3 * config.armOut, -25.2, armSwing * config.lowerArmSwing));
        points.hand_r = points.lowerarm_r.clone().add(chestLocal(-7.0 * config.armOut, -24.5, armSwing * config.handSwing));

        const leftFoot = footAt((phase + 0.5) % 1, 1, config);
        const rightFoot = footAt(phase, -1, config);
        points.thigh_l = pelvis.clone().add(pelvisLocal(config.hipX + sideSway * 0.15, -3.5, config.hipZ));
        points.thigh_r = pelvis.clone().add(pelvisLocal(-config.hipX + sideSway * 0.15, -3.5, -config.hipZ));
        points.foot_l = leftFoot.ankle;
        points.ball_l = leftFoot.ball;
        points.foot_r = rightFoot.ankle;
        points.ball_r = rightFoot.ball;
        points.calf_l = solveKnee(points.thigh_l, points.foot_l, 1);
        points.calf_r = solveKnee(points.thigh_r, points.foot_r, -1);
        points._leftPlanted = leftFoot.planted;
        points._rightPlanted = rightFoot.planted;
        return points;
      }

      function buildRunPose(index, config) {
        const phase = index / frameCount;
        const leftFoot = runFootAt((phase + 0.5) % 1, 1, config);
        const rightFoot = runFootAt(phase, -1, config);
        const support = leftFoot.planted || rightFoot.planted;
        const impact = Math.max(leftFoot.impact, rightFoot.impact);
        const push = Math.max(leftFoot.push, rightFoot.push);
        const stepStanceRatio = config.stanceRatio * 2;
        const stridePhase = (phase * 2) % 1;
        const stanceProgress = clamp(stridePhase / stepStanceRatio, 0, 1);
        const flightProgress = clamp((stridePhase - stepStanceRatio) / (1 - stepStanceRatio), 0, 1);
        const lateFlightDrop = smoothstep(clamp((flightProgress - 0.62) / 0.28, 0, 1));
        const flightArc = support ? 0 : Math.pow(Math.sin(flightProgress * Math.PI), 0.9) * (1 - lateFlightDrop * 0.82);
        const stanceSettle = support ? Math.sin(stanceProgress * Math.PI) * config.bob : 0;
        const pelvisHeight = 94.8
          + stanceSettle
          - impact * config.contactDrop
          + push * config.reboundLift
          + flightArc * config.flightRise;
        const sideSway = wave(phase + 0.25) * config.sideSway;
        const chestTwist = wave(phase) * config.chestTwist;
        const pelvisYaw = wave(phase) * config.pelvisYaw;
        const pelvisRoll = wave(phase + 0.23) * config.pelvisRoll;
        const chestYaw = pelvisYaw * config.chestYawCounter;
        const chestRoll = pelvisRoll * config.chestRollCounter;
        const pelvis = v(sideSway, pelvisHeight, 1.1 + push * 0.35);
        const points = {};
        const pelvisLocal = (x, y, z) => rotateYawRoll(v(x, y, z), pelvisYaw, pelvisRoll);
        const chestLocal = (x, y, z) => rotateYawRoll(v(x, y, z), chestYaw, chestRoll);
        const lean = config.torsoLean + flightArc * 1.2 - impact * 0.4;

        points.pelvis = pelvis;
        points.spine_01 = pelvis.clone().add(pelvisLocal(sideSway * 0.08, 12.2, lean * 0.30));
        points.spine_02 = points.spine_01.clone().add(pelvisLocal(sideSway * 0.06, 12.8, lean * 0.32));
        points.spine_03 = points.spine_02.clone().add(chestLocal(chestTwist * 0.05, 12.7, lean * 0.36));
        points.spine_04 = points.spine_03.clone().add(chestLocal(chestTwist * 0.07, 10.0, lean * 0.30));
        points.spine_05 = points.spine_04.clone().add(chestLocal(chestTwist * 0.11, 9.7, lean * 0.24));
        points.neck_01 = points.spine_05.clone().add(chestLocal(chestTwist * 0.04, 12.0, lean * 0.20));
        points.head = points.neck_01.clone().add(chestLocal(chestTwist * 0.03, 16.1, lean * 0.18));
        points.clavicle_l = points.spine_05.clone().add(chestLocal(17.4, 2.8, 1.2));
        points.clavicle_r = points.spine_05.clone().add(chestLocal(-17.4, 2.8, -1.2));

        const armSwing = wave(phase);
        const leftDrive = -armSwing;
        const rightDrive = armSwing;
        const elbowDriveScale = config.elbowDriveScale ?? 0.58;
        const fistSwingScale = config.fistSwingScale ?? 1;
        const fistLiftScale = config.fistLiftScale ?? 1;
        const leftElbowDrive = leftDrive * elbowDriveScale;
        const rightElbowDrive = rightDrive * elbowDriveScale;
        const elbowPump = Math.abs(armSwing) * config.elbowPump + push * 0.75;
        const leftFistLift = Math.max(0, leftDrive) * config.fistLift - Math.max(0, -leftDrive) * config.fistDrop;
        const rightFistLift = Math.max(0, rightDrive) * config.fistLift - Math.max(0, -rightDrive) * config.fistDrop;
        points.upperarm_l = points.clavicle_l.clone().add(chestLocal(
          12.4 * config.armOut,
          -12.8 - config.armDrop + Math.max(0, leftDrive) * 0.55,
          leftElbowDrive * config.upperArmSwing + lean * 0.34
        ));
        points.lowerarm_l = points.upperarm_l.clone().add(chestLocal(
          8.6 * config.armOut,
          -17.2 - elbowPump + leftFistLift * 0.02,
          leftElbowDrive * config.lowerArmSwing + lean * 0.09
        ));
        points.hand_l = points.lowerarm_l.clone().add(chestLocal(
          3.8 * config.armOut,
          -12.9 + leftFistLift * fistLiftScale,
          leftDrive * config.handSwing * fistSwingScale - 1.4
        ));
        points.upperarm_r = points.clavicle_r.clone().add(chestLocal(
          -12.4 * config.armOut,
          -12.8 - config.armDrop + Math.max(0, rightDrive) * 0.55,
          rightElbowDrive * config.upperArmSwing + lean * 0.34
        ));
        points.lowerarm_r = points.upperarm_r.clone().add(chestLocal(
          -8.6 * config.armOut,
          -17.2 - elbowPump + rightFistLift * 0.02,
          rightElbowDrive * config.lowerArmSwing + lean * 0.09
        ));
        points.hand_r = points.lowerarm_r.clone().add(chestLocal(
          -3.8 * config.armOut,
          -12.9 + rightFistLift * fistLiftScale,
          rightDrive * config.handSwing * fistSwingScale - 1.4
        ));

        points.thigh_l = pelvis.clone().add(pelvisLocal(config.hipX + sideSway * 0.08, -3.7, config.hipZ));
        points.thigh_r = pelvis.clone().add(pelvisLocal(-config.hipX + sideSway * 0.08, -3.7, -config.hipZ));
        const airborneClearance = support ? 0 : config.flightFootClearance * Math.max(0.55, flightArc);
        const airborneLift = v(0, airborneClearance, 0);
        points.foot_l = leftFoot.ankle.clone().add(airborneLift);
        points.ball_l = leftFoot.ball.clone().add(airborneLift);
        points.foot_r = rightFoot.ankle.clone().add(airborneLift);
        points.ball_r = rightFoot.ball.clone().add(airborneLift);
        points.calf_l = solveKnee(points.thigh_l, points.foot_l, 1)
          .add(v(0, -leftFoot.impact * config.kneeCompression, leftFoot.push * 1.4));
        points.calf_r = solveKnee(points.thigh_r, points.foot_r, -1)
          .add(v(0, -rightFoot.impact * config.kneeCompression, rightFoot.push * 1.4));
        points._leftPlanted = leftFoot.planted;
        points._rightPlanted = rightFoot.planted;
        return points;
      }

      function buildNaturalRunPose(index) {
        const pointPose = buildNaturalRunPointPose(index);
        if (pointPose) {
          return pointPose;
        }

        const key = sampleNaturalRunKey(index);
        const deg = THREE.MathUtils.degToRad;
        const points = {};
        const pelvisYaw = deg(key.pelvis[4] || 0);
        const pelvisPitch = deg(key.pelvis[3] || 0);
        const pelvisRoll = deg(key.pelvis[5] || 0);
        const chestYaw = deg(key.chest[1] || 0);
        const chestPitch = deg(key.chest[0] || 0);
        const chestRoll = deg(key.chest[2] || 0);
        const neckYaw = deg(key.neck[1] || 0);
        const neckPitch = deg(key.neck[0] || 0);
        const neckRoll = deg(key.neck[2] || 0);
        const headYaw = deg(key.head[1] || 0);
        const headPitch = deg(key.head[0] || 0);
        const headRoll = deg(key.head[2] || 0);
        const pelvisLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), pelvisYaw, pelvisPitch, pelvisRoll);
        const chestLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), chestYaw, chestPitch, chestRoll);
        const neckLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), neckYaw, neckPitch, neckRoll);
        const headLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), headYaw, headPitch, headRoll);

        const pelvisY = Math.abs(key.pelvis[1]) > 40 ? key.pelvis[1] : 94.5 + key.pelvis[1];
        const pelvis = v(key.pelvis[0], pelvisY, key.pelvis[2]);
        points.pelvis = pelvis;
        points.spine_01 = pelvis.clone().add(pelvisLocal(0, 12.4, 1.0));
        points.spine_02 = points.spine_01.clone().add(pelvisLocal(0.1, 12.9, 1.0));
        points.spine_03 = points.spine_02.clone().add(chestLocal(0.2, 12.8, 1.2));
        points.spine_04 = points.spine_03.clone().add(chestLocal(0.4, 10.0, 1.0));
        points.spine_05 = points.spine_04.clone().add(chestLocal(0.5, 9.7, 0.8));
        points.neck_01 = points.spine_05.clone().add(chestLocal(0.2, 12.2, 0.4)).add(neckLocal(0, 0.5, 0.1));
        points.head = points.neck_01.clone().add(headLocal(0.1, 16.2, 0.3));
        points.clavicle_l = points.spine_05.clone().add(chestLocal(17.4, 2.8, 1.1));
        points.clavicle_r = points.spine_05.clone().add(chestLocal(-17.4, 2.8, -1.1));

        points.upperarm_l = points.clavicle_l.clone().add(chestLocal(...key.lUpper));
        points.lowerarm_l = points.upperarm_l.clone().add(chestLocal(...key.lLower));
        points.hand_l = points.lowerarm_l.clone().add(chestLocal(...key.lHand));
        points.upperarm_r = points.clavicle_r.clone().add(chestLocal(...key.rUpper));
        points.lowerarm_r = points.upperarm_r.clone().add(chestLocal(...key.rLower));
        points.hand_r = points.lowerarm_r.clone().add(chestLocal(...key.rHand));

        points.thigh_l = pelvis.clone().add(pelvisLocal(9.9, -3.8, 1.15));
        points.thigh_r = pelvis.clone().add(pelvisLocal(-9.9, -3.8, -1.15));
        points.foot_l = v(...key.leftFoot.ankle);
        points.ball_l = v(...key.leftFoot.ball);
        points.foot_r = v(...key.rightFoot.ankle);
        points.ball_r = v(...key.rightFoot.ball);
        points.calf_l = solveKnee(points.thigh_l, points.foot_l, 1).add(v(...key.leftKneeBias));
        points.calf_r = solveKnee(points.thigh_r, points.foot_r, -1).add(v(...key.rightKneeBias));

        const faceYaw = chestYaw * 0.45 + neckYaw * 0.5 + headYaw;
        const facePitch = chestPitch * 0.25 + neckPitch * 0.45 + headPitch;
        const faceRoll = chestRoll * 0.25 + neckRoll * 0.45 + headRoll;
        points._faceRight = rotateYawPitchRoll(v(1, 0, 0), faceYaw, facePitch, faceRoll).normalize();
        points._faceUp = rotateYawPitchRoll(v(0, 1, 0), faceYaw, facePitch, faceRoll).normalize();
        points._faceForward = new THREE.Vector3().crossVectors(points._faceRight, points._faceUp).normalize();
        points._leftPlanted = key.leftFoot.planted;
        points._rightPlanted = key.rightFoot.planted;
        return points;
      }

      function contactActive(value) {
        if (value == null) {
          return false;
        }
        if (typeof value === "boolean") {
          return value;
        }
        if (typeof value === "number") {
          return value > 0.35;
        }
        if (typeof value === "object") {
          return contactActive(value.active ?? value.ground ?? value.planted ?? value.contact ?? value.type ?? value.name ?? value.weight);
        }
        const text = String(value).toLowerCase();
        return text.includes("ground") || text.includes("floor") || text.includes("plant") ||
          text.includes("contact") || text.includes("locked") || text.includes("brace") ||
          text.includes("crawl");
      }

      function readProneCrawlContact(source, names, fallback = false) {
        for (const name of names) {
          const value = name.includes(".") ? naturalRunPath(source, name) : source?.[name];
          if (value != null) {
            return contactActive(value);
          }
        }
        return fallback;
      }

      function readProneCrawlVector(source, paths, fallback) {
        for (const path of paths) {
          const value = path.includes(".") ? naturalRunPath(source, path) : source?.[path];
          const direct = naturalRunArray(value, 3, null);
          if (direct) {
            return direct;
          }
          if (value && typeof value === "object") {
            for (const key of ["point", "pos", "position", "location", "translation", "target", "world", "worldPos", "worldPosition", "elbowPoint", "handPoint", "kneePoint", "anklePoint", "ballPoint"]) {
              const nested = naturalRunArray(value[key], 3, null);
              if (nested) {
                return nested;
              }
            }
          }
        }
        return fallback.slice(0, 3);
      }

      function readProneCrawlRotation(source, paths, fallback) {
        if (source && typeof source === "object" && ["pitch", "yaw", "roll"].some((name) => name in source)) {
          return [
            Number(source.pitch ?? fallback[0] ?? 0),
            Number(source.yaw ?? fallback[1] ?? 0),
            Number(source.roll ?? fallback[2] ?? 0)
          ];
        }
        for (const path of paths) {
          const value = path.includes(".") ? naturalRunPath(source, path) : source?.[path];
          const direct = naturalRunArray(value, 3, null);
          if (direct) {
            return direct;
          }
          if (value && typeof value === "object") {
            const nested = naturalRunArray(value.rot || value.rotation || value.euler || value.angle, 3, null);
            if (nested) {
              return nested;
            }
            if (["pitch", "yaw", "roll"].some((name) => name in value)) {
              return [
                Number(value.pitch ?? fallback[0] ?? 0),
                Number(value.yaw ?? fallback[1] ?? 0),
                Number(value.roll ?? fallback[2] ?? 0)
              ];
            }
          }
        }
        return fallback.slice(0, 3);
      }

      function readProneCrawlTransform(source, paths, fallback) {
        for (const path of paths) {
          const value = path.includes(".") ? naturalRunPath(source, path) : source?.[path];
          if (Array.isArray(value)) {
            return naturalRunArray(value, 6, fallback);
          }
          if (value && typeof value === "object") {
            const position = naturalRunArray(
              value.pos || value.position || value.location || value.translation || value,
              3,
              fallback.slice(0, 3)
            );
            const rotation = readProneCrawlRotation(
              value,
              ["rot", "rotation", "euler", ""].filter(Boolean),
              fallback.slice(3, 6)
            );
            return position.concat(rotation);
          }
        }
        return fallback.slice(0, 6);
      }

      function makeProneCrawlFallbackKey(index) {
        const phase = (index % frameCount) / frameCount;
        const cycle = phase * Math.PI * 2;
        const leftDrive = Math.max(0, Math.sin(cycle));
        const rightDrive = Math.max(0, -Math.sin(cycle));
        const torsoSway = Math.sin(cycle) * 1.5;
        const shoulderCounter = Math.sin(cycle) * 5.0;
        const hipCounter = -Math.sin(cycle) * 4.0;
        const crawlBob = Math.sin(cycle * 2 + 0.3) * 0.7;
        const leftReach = Math.cos(cycle) * 10.0;
        const rightReach = -Math.cos(cycle) * 10.0;
        const leftHandContact = leftDrive < 0.52;
        const rightHandContact = rightDrive < 0.52;
        const leftKneeContact = rightDrive < 0.58;
        const rightKneeContact = leftDrive < 0.58;
        return {
          frame: index,
          pelvis: [torsoSway * 0.25, 26.0 + crawlBob, -6.0, 81.5, hipCounter, torsoSway * 0.7],
          chest: [84.0, shoulderCounter, -torsoSway * 0.9],
          neck: [-5.0, shoulderCounter * 0.16, torsoSway * 0.10],
          head: [-11.0, shoulderCounter * 0.12, torsoSway * 0.08],
          leftElbow: [18.0, 6.2 + leftDrive * 2.0, 18.0 + leftReach * 0.46],
          leftHand: [20.5, 2.2 + leftDrive * 2.3, 37.0 + leftReach],
          rightElbow: [-18.0, 6.2 + rightDrive * 2.0, 18.0 + rightReach * 0.46],
          rightHand: [-20.5, 2.2 + rightDrive * 2.3, 37.0 + rightReach],
          leftKnee: [9.8, 5.6 + rightDrive * 2.2, -15.0 - rightReach * 0.30],
          rightKnee: [-9.8, 5.6 + leftDrive * 2.2, -15.0 - leftReach * 0.30],
          leftFoot: {
            ankle: [11.2, 5.0 + rightDrive * 2.4, -35.0 - rightReach * 0.26],
            ball: [12.0, 1.2 + rightDrive * 1.1, -22.0 - rightReach * 0.20],
            planted: true
          },
          rightFoot: {
            ankle: [-11.2, 5.0 + leftDrive * 2.4, -35.0 - leftReach * 0.26],
            ball: [-12.0, 1.2 + leftDrive * 1.1, -22.0 - leftReach * 0.20],
            planted: true
          },
          contacts: {
            leftHand: leftHandContact,
            rightHand: rightHandContact,
            leftKnee: leftKneeContact,
            rightKnee: rightKneeContact,
            leftFoot: true,
            rightFoot: true
          }
        };
      }

      function readProneCrawlFoot(source, sideName, fallback) {
        const shortName = sideName === "left" ? "l" : "r";
        const foot = source?.[`${sideName}Foot`] || source?.[`${shortName}Foot`] || source?.feet?.[sideName] || {};
        const ankleFallback = readProneCrawlVector(source, [`foot_${shortName}`, `${shortName}Foot`, `${sideName}Ankle`], fallback.ankle);
        const ballFallback = readProneCrawlVector(source, [`ball_${shortName}`, `${shortName}Ball`, `${sideName}Ball`], fallback.ball);
        const planted = readProneCrawlContact(source, [
          `contacts.${sideName}Foot`,
          `${sideName}Foot.contact`,
          `${sideName}Foot.planted`,
          `${sideName}Foot.weight`,
          `${sideName}Planted`,
          `_${sideName}Planted`
        ], fallback.planted);
        return {
          ankle: readProneCrawlVector(foot, ["ankle", "foot", "pos", "position", "location"], ankleFallback),
          ball: readProneCrawlVector(foot, ["ball", "toe"], ballFallback),
          planted
        };
      }

      function normalizeProneCrawlKey(source, fallbackIndex) {
        const fallback = makeProneCrawlFallbackKey(fallbackIndex);
        return {
          frame: readNaturalKeyFrame(source, fallback.frame),
          pelvis: readProneCrawlTransform(source, ["pelvis", "root"], fallback.pelvis),
          chest: readProneCrawlRotation(source, ["chest", "spine", "spine.upper", "spine_05", "bones.spine_05"], fallback.chest),
          neck: readProneCrawlRotation(source, ["neck", "neck_01", "bones.neck_01"], fallback.neck),
          head: readProneCrawlRotation(source, ["head", "bones.head"], fallback.head),
          leftElbow: readProneCrawlVector(source, ["leftElbow", "leftArm.elbow", "elbow_l", "lowerarm_l"], fallback.leftElbow),
          leftHand: readProneCrawlVector(source, ["leftHand", "leftArm.hand", "hand_l"], fallback.leftHand),
          rightElbow: readProneCrawlVector(source, ["rightElbow", "rightArm.elbow", "elbow_r", "lowerarm_r"], fallback.rightElbow),
          rightHand: readProneCrawlVector(source, ["rightHand", "rightArm.hand", "hand_r"], fallback.rightHand),
          leftKnee: readProneCrawlVector(source, ["leftKnee", "leftLeg.knee", "knee_l", "calf_l"], fallback.leftKnee),
          rightKnee: readProneCrawlVector(source, ["rightKnee", "rightLeg.knee", "knee_r", "calf_r"], fallback.rightKnee),
          leftFoot: readProneCrawlFoot(source, "left", fallback.leftFoot),
          rightFoot: readProneCrawlFoot(source, "right", fallback.rightFoot),
          contacts: {
            leftHand: readProneCrawlContact(source, ["contacts.leftHand", "leftHand.contact", "leftHand.weight"], fallback.contacts.leftHand),
            rightHand: readProneCrawlContact(source, ["contacts.rightHand", "rightHand.contact", "rightHand.weight"], fallback.contacts.rightHand),
            leftKnee: readProneCrawlContact(source, ["contacts.leftKnee", "leftKnee.contact", "leftKnee.weight"], fallback.contacts.leftKnee),
            rightKnee: readProneCrawlContact(source, ["contacts.rightKnee", "rightKnee.contact", "rightKnee.weight"], fallback.contacts.rightKnee)
          }
        };
      }

      function proneCrawlPair(index) {
        if (proneCrawlRawKeys.length < 2) {
          return null;
        }
        const entries = proneCrawlRawKeys
          .map((key, order) => ({ key, frame: readNaturalKeyFrame(key, order) }))
          .sort((a, b) => a.frame - b.frame);
        const sampleFrame = ((index % frameCount) + frameCount) % frameCount;
        for (let i = 0; i < entries.length - 1; i += 1) {
          const from = entries[i];
          const to = entries[i + 1];
          if (sampleFrame >= from.frame && sampleFrame <= to.frame) {
            const raw = (sampleFrame - from.frame) / Math.max(0.001, to.frame - from.frame);
            return { from, to, t: smoothstep(raw) };
          }
        }
        const first = entries[0];
        const last = entries[entries.length - 1];
        if (sampleFrame < first.frame) {
          const raw = (sampleFrame + frameCount - last.frame) / Math.max(0.001, first.frame + frameCount - last.frame);
          return { from: last, to: first, t: smoothstep(raw) };
        }
        const raw = (sampleFrame - last.frame) / Math.max(0.001, first.frame + frameCount - last.frame);
        return { from: last, to: first, t: smoothstep(raw) };
      }

      function interpolateProneCrawlVector(a, b, t, lock) {
        return lock ? a.slice(0, 3) : interpolateNaturalRunArray(a, b, t).slice(0, 3);
      }

      function interpolateProneCrawlFoot(a, b, t) {
        const locked = a.planted && b.planted;
        return {
          ankle: interpolateProneCrawlVector(a.ankle, b.ankle, t, locked),
          ball: interpolateProneCrawlVector(a.ball, b.ball, t, locked),
          planted: t < 0.5 ? a.planted : b.planted
        };
      }

      function interpolateProneCrawlKey(fromSource, toSource, t) {
        const from = normalizeProneCrawlKey(fromSource, readNaturalKeyFrame(fromSource, 0));
        const to = normalizeProneCrawlKey(toSource, readNaturalKeyFrame(toSource, 0));
        const leftHandLocked = from.contacts.leftHand && to.contacts.leftHand;
        const rightHandLocked = from.contacts.rightHand && to.contacts.rightHand;
        const leftKneeLocked = from.contacts.leftKnee && to.contacts.leftKnee;
        const rightKneeLocked = from.contacts.rightKnee && to.contacts.rightKnee;
        return {
          pelvis: interpolateNaturalRunArray(from.pelvis, to.pelvis, t),
          chest: interpolateNaturalRunArray(from.chest, to.chest, t),
          neck: interpolateNaturalRunArray(from.neck, to.neck, t),
          head: interpolateNaturalRunArray(from.head, to.head, t),
          leftElbow: interpolateProneCrawlVector(from.leftElbow, to.leftElbow, t, leftHandLocked),
          leftHand: interpolateProneCrawlVector(from.leftHand, to.leftHand, t, leftHandLocked),
          rightElbow: interpolateProneCrawlVector(from.rightElbow, to.rightElbow, t, rightHandLocked),
          rightHand: interpolateProneCrawlVector(from.rightHand, to.rightHand, t, rightHandLocked),
          leftKnee: interpolateProneCrawlVector(from.leftKnee, to.leftKnee, t, leftKneeLocked),
          rightKnee: interpolateProneCrawlVector(from.rightKnee, to.rightKnee, t, rightKneeLocked),
          leftFoot: interpolateProneCrawlFoot(from.leftFoot, to.leftFoot, t),
          rightFoot: interpolateProneCrawlFoot(from.rightFoot, to.rightFoot, t),
          contacts: {
            leftHand: t < 0.5 ? from.contacts.leftHand : to.contacts.leftHand,
            rightHand: t < 0.5 ? from.contacts.rightHand : to.contacts.rightHand,
            leftKnee: t < 0.5 ? from.contacts.leftKnee : to.contacts.leftKnee,
            rightKnee: t < 0.5 ? from.contacts.rightKnee : to.contacts.rightKnee
          }
        };
      }

      function sampleProneCrawlKey(index) {
        const pair = proneCrawlPair(index);
        if (!pair) {
          return makeProneCrawlFallbackKey(index);
        }
        return interpolateProneCrawlKey(pair.from.key, pair.to.key, pair.t);
      }

      function proneCrawlContactForJoint(key, jointName) {
        const map = {
          lowerarm_l: "leftHand",
          hand_l: "leftHand",
          lowerarm_r: "rightHand",
          hand_r: "rightHand",
          calf_l: "leftKnee",
          calf_r: "rightKnee",
          foot_l: "leftFoot",
          ball_l: "leftFoot",
          foot_r: "rightFoot",
          ball_r: "rightFoot"
        };
        const contactName = map[jointName];
        return contactName ? readProneCrawlContact(key, [`contacts.${contactName}`, contactName, `${contactName}.contact`, `${contactName}.planted`], false) : false;
      }

      function buildProneCrawlPointPose(index) {
        const pair = proneCrawlPair(index);
        if (!pair) {
          return null;
        }
        const fromPoints = naturalRunPointSource(pair.from.key);
        const toPoints = naturalRunPointSource(pair.to.key);
        if (!fromPoints || !toPoints) {
          return null;
        }
        const points = {};
        for (const name of jointNames) {
          const a = naturalRunArray(fromPoints[name], 3, null);
          const b = naturalRunArray(toPoints[name], 3, null);
          if (!a || !b) {
            return null;
          }
          const locked = proneCrawlContactForJoint(pair.from.key, name) && proneCrawlContactForJoint(pair.to.key, name);
          const vector = interpolateProneCrawlVector(a, b, pair.t, locked);
          points[name] = v(vector[0], vector[1], vector[2]);
        }
        points._leftPlanted = pair.t < 0.5
          ? proneCrawlContactForJoint(pair.from.key, "foot_l")
          : proneCrawlContactForJoint(pair.to.key, "foot_l");
        points._rightPlanted = pair.t < 0.5
          ? proneCrawlContactForJoint(pair.from.key, "foot_r")
          : proneCrawlContactForJoint(pair.to.key, "foot_r");
        return points;
      }

      function buildProneCrawlPose(index) {
        const pointPose = buildProneCrawlPointPose(index);
        if (pointPose) {
          return pointPose;
        }

        const key = sampleProneCrawlKey(index);
        const deg = THREE.MathUtils.degToRad;
        const points = {};
        const pelvisYaw = deg(key.pelvis[4] || 0);
        const pelvisPitch = deg(key.pelvis[3] || 82);
        const pelvisRoll = deg(key.pelvis[5] || 0);
        const chestYaw = deg(key.chest[1] || 0);
        const chestPitch = deg(key.chest[0] || 84);
        const chestRoll = deg(key.chest[2] || 0);
        const neckYaw = deg(key.neck[1] || 0);
        const neckPitch = deg(key.neck[0] || 0);
        const neckRoll = deg(key.neck[2] || 0);
        const headYaw = deg(key.head[1] || 0);
        const headPitch = deg(key.head[0] || 0);
        const headRoll = deg(key.head[2] || 0);
        const pelvisLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), pelvisYaw, pelvisPitch, pelvisRoll);
        const chestLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), pelvisYaw + chestYaw, chestPitch, pelvisRoll + chestRoll);

        const pelvis = v(key.pelvis[0], key.pelvis[1], key.pelvis[2]);
        points.pelvis = pelvis;
        points.spine_01 = pelvis.clone().add(pelvisLocal(0, 11.8, 0.9));
        points.spine_02 = points.spine_01.clone().add(pelvisLocal(0.1, 12.5, 1.0));
        points.spine_03 = points.spine_02.clone().add(chestLocal(0.2, 12.4, 1.1));
        points.spine_04 = points.spine_03.clone().add(chestLocal(0.3, 9.6, 1.0));
        points.spine_05 = points.spine_04.clone().add(chestLocal(0.4, 9.2, 0.8));
        points.neck_01 = points.spine_05.clone().add(chestLocal(0.1, 9.8, 1.2));
        points.head = points.neck_01.clone().add(chestLocal(headYaw * 1.2, 11.6, 2.1 + headPitch * -0.06));
        points.clavicle_l = points.spine_05.clone().add(chestLocal(17.0, 2.4, 1.0));
        points.clavicle_r = points.spine_05.clone().add(chestLocal(-17.0, 2.4, -1.0));

        const leftElbow = v(...key.leftElbow);
        const rawLeftHand = v(...key.leftHand);
        const rightElbow = v(...key.rightElbow);
        const rawRightHand = v(...key.rightHand);
        points.upperarm_l = points.clavicle_l.clone().lerp(leftElbow, 0.52).add(v(0, 2.5, -1.0));
        points.hand_l = clampLimbTarget(points.upperarm_l, rawLeftHand, 58.0);
        points.lowerarm_l = blendValidJoint(
          leftElbow,
          solveElbow(points.upperarm_l, points.hand_l, 1, v(2.2, -0.9, -1.4)),
          points.upperarm_l,
          points.hand_l,
          35.5,
          33.5,
          0.28
        );
        points.upperarm_r = points.clavicle_r.clone().lerp(rightElbow, 0.52).add(v(0, 2.5, -1.0));
        points.hand_r = clampLimbTarget(points.upperarm_r, rawRightHand, 58.0);
        points.lowerarm_r = blendValidJoint(
          rightElbow,
          solveElbow(points.upperarm_r, points.hand_r, -1, v(-2.2, -0.9, -1.4)),
          points.upperarm_r,
          points.hand_r,
          35.5,
          33.5,
          0.28
        );

        points.thigh_l = pelvis.clone().add(pelvisLocal(9.8, -2.2, -0.8));
        points.thigh_r = pelvis.clone().add(pelvisLocal(-9.8, -2.2, -0.8));
        const leftKnee = v(...key.leftKnee);
        const rightKnee = v(...key.rightKnee);
        points.foot_l = clampLimbTarget(points.thigh_l, v(...key.leftFoot.ankle), 82.0);
        points.ball_l = v(...key.leftFoot.ball);
        points.foot_r = clampLimbTarget(points.thigh_r, v(...key.rightFoot.ankle), 82.0);
        points.ball_r = v(...key.rightFoot.ball);
        points.calf_l = blendValidJoint(
          leftKnee,
          solveKnee(points.thigh_l, points.foot_l, 1),
          points.thigh_l,
          points.foot_l,
          45.5,
          44.5,
          0.25
        );
        points.calf_r = blendValidJoint(
          rightKnee,
          solveKnee(points.thigh_r, points.foot_r, -1),
          points.thigh_r,
          points.foot_r,
          45.5,
          44.5,
          0.25
        );

        const faceYaw = pelvisYaw + chestYaw * 0.65 + neckYaw * 0.45 + headYaw;
        const facePitch = chestPitch + neckPitch * 0.45 + headPitch;
        const faceRoll = pelvisRoll + chestRoll * 0.35 + neckRoll * 0.35 + headRoll;
        points._faceRight = rotateYawPitchRoll(v(1, 0, 0), faceYaw, facePitch, faceRoll).normalize();
        points._faceUp = rotateYawPitchRoll(v(0, 1, 0), faceYaw, facePitch, faceRoll).normalize();
        points._faceForward = new THREE.Vector3().crossVectors(points._faceRight, points._faceUp).normalize();
        points._leftPlanted = key.leftFoot.planted;
        points._rightPlanted = key.rightFoot.planted;
        return points;
      }

      function buildShuffleDancePose(index) {
        const pointPose = buildShuffleDancePointPose(index);
        if (pointPose) {
          return pointPose;
        }

        const key = sampleShuffleDanceKey(index);
        const deg = THREE.MathUtils.degToRad;
        const points = {};
        const pelvisYaw = deg(key.pelvis[4] || 0);
        const pelvisPitch = deg(key.pelvis[3] || 0);
        const pelvisRoll = deg(key.pelvis[5] || 0);
        const chestYaw = deg(key.chest[1] || 0);
        const chestPitch = deg(key.chest[0] || 0);
        const chestRoll = deg(key.chest[2] || 0);
        const neckYaw = deg(key.neck[1] || 0);
        const neckPitch = deg(key.neck[0] || 0);
        const neckRoll = deg(key.neck[2] || 0);
        const headYaw = deg(key.head[1] || 0);
        const headPitch = deg(key.head[0] || 0);
        const headRoll = deg(key.head[2] || 0);
        const pelvisLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), pelvisYaw, pelvisPitch, pelvisRoll);
        const chestLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), chestYaw, chestPitch, chestRoll);
        const neckLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), neckYaw, neckPitch, neckRoll);
        const headLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), headYaw, headPitch, headRoll);

        const pelvisY = Math.abs(key.pelvis[1]) > 40 ? key.pelvis[1] : 94.0 + key.pelvis[1];
        const pelvis = v(key.pelvis[0], pelvisY, key.pelvis[2]);
        points.pelvis = pelvis;
        points.spine_01 = pelvis.clone().add(pelvisLocal(0.0, 12.4, 0.7));
        points.spine_02 = points.spine_01.clone().add(pelvisLocal(0.1, 12.9, 0.5));
        points.spine_03 = points.spine_02.clone().add(chestLocal(0.3, 12.8, 0.3));
        points.spine_04 = points.spine_03.clone().add(chestLocal(0.4, 10.0, 0.1));
        points.spine_05 = points.spine_04.clone().add(chestLocal(0.5, 9.7, -0.1));
        points.neck_01 = points.spine_05.clone().add(chestLocal(0.1, 12.2, 0.0)).add(neckLocal(0, 0.5, 0));
        points.head = points.neck_01.clone().add(headLocal(0.1, 16.3, 0.1));
        points.clavicle_l = points.spine_05.clone().add(chestLocal(17.5, 2.8, 1.0));
        points.clavicle_r = points.spine_05.clone().add(chestLocal(-17.5, 2.8, -1.0));

        function placeShuffleArm(side, clavicleName, upperName, lowerName, handName, targetArray, upperOffset, lowerOffset, handOffset) {
          const clavicle = points[clavicleName];
          if (targetArray) {
            const handTarget = Math.abs(targetArray[1]) > 35
              ? v(targetArray[0], targetArray[1], targetArray[2])
              : points.spine_05.clone().add(chestLocal(targetArray[0], targetArray[1], targetArray[2]));
            points[upperName] = clavicle.clone().lerp(handTarget, 0.34).add(chestLocal(side * 4.2, -1.4, -3.0));
            points[lowerName] = clavicle.clone().lerp(handTarget, 0.68).add(chestLocal(side * 2.6, -2.2, 2.2));
            points[handName] = handTarget;
            return;
          }
          points[upperName] = clavicle.clone().add(chestLocal(...upperOffset));
          points[lowerName] = points[upperName].clone().add(chestLocal(...lowerOffset));
          points[handName] = points[lowerName].clone().add(chestLocal(...handOffset));
        }

        placeShuffleArm(1, "clavicle_l", "upperarm_l", "lowerarm_l", "hand_l", key.leftHandTarget, key.lUpper, key.lLower, key.lHand);
        placeShuffleArm(-1, "clavicle_r", "upperarm_r", "lowerarm_r", "hand_r", key.rightHandTarget, key.rUpper, key.rLower, key.rHand);

        points.thigh_l = pelvis.clone().add(pelvisLocal(10.2, -3.8, 1.25));
        points.thigh_r = pelvis.clone().add(pelvisLocal(-10.2, -3.8, -1.25));
        points.foot_l = v(...key.leftFoot.ankle);
        points.ball_l = v(...key.leftFoot.ball);
        points.foot_r = v(...key.rightFoot.ankle);
        points.ball_r = v(...key.rightFoot.ball);
        points.calf_l = solveKnee(points.thigh_l, points.foot_l, 1).add(v(...key.leftKneeBias));
        points.calf_r = solveKnee(points.thigh_r, points.foot_r, -1).add(v(...key.rightKneeBias));

        const faceYaw = chestYaw * 0.38 + neckYaw * 0.45 + headYaw;
        const facePitch = chestPitch * 0.18 + neckPitch * 0.38 + headPitch;
        const faceRoll = chestRoll * 0.22 + neckRoll * 0.40 + headRoll;
        points._faceRight = rotateYawPitchRoll(v(1, 0, 0), faceYaw, facePitch, faceRoll).normalize();
        points._faceUp = rotateYawPitchRoll(v(0, 1, 0), faceYaw, facePitch, faceRoll).normalize();
        points._faceForward = new THREE.Vector3().crossVectors(points._faceRight, points._faceUp).normalize();
        points._leftPlanted = key.leftFoot.planted || key.leftFoot.contactWeight > 0.42;
        points._rightPlanted = key.rightFoot.planted || key.rightFoot.contactWeight > 0.42;
        return points;
      }

      function sampleGenericDanceKey(rawKeys, index) {
        if (Array.isArray(rawKeys) && rawKeys.length === 1) {
          return normalizeShuffleDanceKey(rawKeys[0], readNaturalKeyFrame(rawKeys[0], index));
        }
        const pair = keyposePair(rawKeys, index, frameCount);
        if (!pair) {
          return normalizeShuffleDanceKey(makeShuffleDanceFallbackKey(index), index);
        }
        return interpolateShuffleDanceKey(pair.from.key, pair.to.key, pair.t);
      }

      function buildStaticPointPose(rawKeys) {
        const key = Array.isArray(rawKeys) && rawKeys.length ? rawKeys[0] : null;
        const source = naturalRunPointSource(key);
        if (!source) {
          return null;
        }
        const points = {};
        for (const name of jointNames) {
          const value = naturalRunArray(source[name], 3, null);
          if (!value) {
            return null;
          }
          points[name] = v(value[0], value[1], value[2]);
        }
        const right = new THREE.Vector3().subVectors(points.clavicle_r, points.clavicle_l).normalize();
        const up = new THREE.Vector3().subVectors(points.head, points.neck_01).normalize();
        let forward = new THREE.Vector3().crossVectors(right, up);
        if (forward.lengthSq() < 0.001) {
          forward = v(0, 0, 1);
        }
        points._faceRight = right;
        points._faceUp = up;
        points._faceForward = forward.normalize();
        points._leftPlanted = Boolean(key.leftPlanted ?? key.contacts?.leftFoot ?? key.contacts?.foot_l ?? true);
        points._rightPlanted = Boolean(key.rightPlanted ?? key.contacts?.rightFoot ?? key.contacts?.foot_r ?? true);
        return points;
      }

      function buildGenericPointPose(index, rawKeys) {
        if (!Array.isArray(rawKeys) || rawKeys.length === 0) {
          return null;
        }
        if (rawKeys.length === 1) {
          return buildStaticPointPose(rawKeys);
        }
        const pair = keyposePair(rawKeys, index, frameCount);
        if (!pair) {
          return null;
        }
        const fromPoints = naturalRunPointSource(pair.from.key);
        const toPoints = naturalRunPointSource(pair.to.key);
        if (!fromPoints || !toPoints) {
          return null;
        }
        const points = {};
        for (const name of jointNames) {
          const a = naturalRunArray(fromPoints[name], 3, null);
          const b = naturalRunArray(toPoints[name], 3, null);
          if (!a || !b) {
            return null;
          }
          const value = interpolateNaturalRunArray(a, b, pair.t);
          points[name] = v(value[0], value[1], value[2]);
        }
        const right = new THREE.Vector3().subVectors(points.clavicle_r, points.clavicle_l).normalize();
        const up = new THREE.Vector3().subVectors(points.head, points.neck_01).normalize();
        let forward = new THREE.Vector3().crossVectors(right, up);
        if (forward.lengthSq() < 0.001) {
          forward = v(0, 0, 1);
        }
        points._faceRight = right;
        points._faceUp = up;
        points._faceForward = forward.normalize();
        points._leftPlanted = Boolean(pair.t < 0.5
          ? pair.from.key.leftPlanted ?? pair.from.key.contacts?.leftFoot ?? pair.from.key.contacts?.foot_l ?? true
          : pair.to.key.leftPlanted ?? pair.to.key.contacts?.leftFoot ?? pair.to.key.contacts?.foot_l ?? true);
        points._rightPlanted = Boolean(pair.t < 0.5
          ? pair.from.key.rightPlanted ?? pair.from.key.contacts?.rightFoot ?? pair.from.key.contacts?.foot_r ?? true
          : pair.to.key.rightPlanted ?? pair.to.key.contacts?.rightFoot ?? pair.to.key.contacts?.foot_r ?? true);
        return points;
      }

      function buildGenericDancePose(index, rawKeys) {
        const key = sampleGenericDanceKey(rawKeys, index);
        const deg = THREE.MathUtils.degToRad;
        const points = {};
        const pelvisYaw = deg(key.pelvis[4] || 0);
        const pelvisPitch = deg(key.pelvis[3] || 0);
        const pelvisRoll = deg(key.pelvis[5] || 0);
        const chestYaw = deg(key.chest[1] || 0);
        const chestPitch = deg(key.chest[0] || 0);
        const chestRoll = deg(key.chest[2] || 0);
        const neckYaw = deg(key.neck[1] || 0);
        const neckPitch = deg(key.neck[0] || 0);
        const neckRoll = deg(key.neck[2] || 0);
        const headYaw = deg(key.head[1] || 0);
        const headPitch = deg(key.head[0] || 0);
        const headRoll = deg(key.head[2] || 0);
        const pelvisLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), pelvisYaw, pelvisPitch, pelvisRoll);
        const chestLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), chestYaw, chestPitch, chestRoll);
        const neckLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), neckYaw, neckPitch, neckRoll);
        const headLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), headYaw, headPitch, headRoll);

        const pelvisY = Math.abs(key.pelvis[1]) > 40 ? key.pelvis[1] : 94.0 + key.pelvis[1];
        const pelvis = v(key.pelvis[0], pelvisY, key.pelvis[2]);
        points.pelvis = pelvis;
        points.spine_01 = pelvis.clone().add(pelvisLocal(0.0, 12.4, 0.7));
        points.spine_02 = points.spine_01.clone().add(pelvisLocal(0.1, 12.9, 0.5));
        points.spine_03 = points.spine_02.clone().add(chestLocal(0.3, 12.8, 0.3));
        points.spine_04 = points.spine_03.clone().add(chestLocal(0.4, 10.0, 0.1));
        points.spine_05 = points.spine_04.clone().add(chestLocal(0.5, 9.7, -0.1));
        points.neck_01 = points.spine_05.clone().add(chestLocal(0.1, 12.2, 0.0)).add(neckLocal(0, 0.5, 0));
        points.head = points.neck_01.clone().add(headLocal(0.1, 16.3, 0.1));
        points.clavicle_l = points.spine_05.clone().add(chestLocal(17.5, 2.8, 1.0));
        points.clavicle_r = points.spine_05.clone().add(chestLocal(-17.5, 2.8, -1.0));

        function placeArm(side, clavicleName, upperName, lowerName, handName, targetArray, upperOffset, lowerOffset, handOffset) {
          const clavicle = points[clavicleName];
          if (targetArray) {
            const useWorldTarget = currentMode === "heroLandingPose" || currentMode === "boxOverheadLift" || Math.abs(targetArray[1]) > 35;
            const rawTarget = useWorldTarget
              ? v(targetArray[0], targetArray[1], targetArray[2])
              : points.spine_05.clone().add(chestLocal(targetArray[0], targetArray[1], targetArray[2]));
            const handTarget = clampLimbTarget(clavicle, rawTarget, 58.0);
            points[upperName] = clavicle.clone().lerp(handTarget, 0.34).add(chestLocal(side * 4.2, -1.4, -3.0));
            points[lowerName] = solveElbow(points[upperName], handTarget, side, v(side * 1.2, -0.8, 0.7));
            points[handName] = handTarget;
            return;
          }
          points[upperName] = clavicle.clone().add(chestLocal(...upperOffset));
          points[lowerName] = points[upperName].clone().add(chestLocal(...lowerOffset));
          points[handName] = points[lowerName].clone().add(chestLocal(...handOffset));
        }

        placeArm(1, "clavicle_l", "upperarm_l", "lowerarm_l", "hand_l", key.leftHandTarget, key.lUpper, key.lLower, key.lHand);
        placeArm(-1, "clavicle_r", "upperarm_r", "lowerarm_r", "hand_r", key.rightHandTarget, key.rUpper, key.rLower, key.rHand);

        points.thigh_l = pelvis.clone().add(pelvisLocal(10.2, -3.8, 1.25));
        points.thigh_r = pelvis.clone().add(pelvisLocal(-10.2, -3.8, -1.25));
        points.foot_l = clampLimbTarget(points.thigh_l, v(...key.leftFoot.ankle), 82.0);
        points.ball_l = v(...key.leftFoot.ball);
        points.foot_r = clampLimbTarget(points.thigh_r, v(...key.rightFoot.ankle), 82.0);
        points.ball_r = v(...key.rightFoot.ball);
        points.calf_l = solveKnee(points.thigh_l, points.foot_l, 1).add(v(...key.leftKneeBias));
        points.calf_r = solveKnee(points.thigh_r, points.foot_r, -1).add(v(...key.rightKneeBias));

        const faceYaw = chestYaw * 0.38 + neckYaw * 0.45 + headYaw;
        const facePitch = chestPitch * 0.18 + neckPitch * 0.38 + headPitch;
        const faceRoll = chestRoll * 0.22 + neckRoll * 0.40 + headRoll;
        points._faceRight = rotateYawPitchRoll(v(1, 0, 0), faceYaw, facePitch, faceRoll).normalize();
        points._faceUp = rotateYawPitchRoll(v(0, 1, 0), faceYaw, facePitch, faceRoll).normalize();
        points._faceForward = new THREE.Vector3().crossVectors(points._faceRight, points._faceUp).normalize();
        points._leftPlanted = Boolean(key.leftFoot.planted || key.leftFoot.contactWeight > 0.42);
        points._rightPlanted = Boolean(key.rightFoot.planted || key.rightFoot.contactWeight > 0.42);
        return points;
      }

      function buildIdleShiftPose(index) {
        const phase = index / frameCount;
        const weight = wave(phase);
        const breath = Math.sin(phase * Math.PI * 4 + 0.35);
        const pelvisSway = weight * 3.1;
        const pelvisYaw = Math.sin(phase * Math.PI * 2 + 0.25) * 0.035;
        const pelvisRoll = -weight * 0.075;
        const chestYaw = -pelvisYaw * 0.35;
        const chestRoll = -pelvisRoll * 0.45;
        const leftFree = Math.max(0, -weight);
        const rightFree = Math.max(0, weight);
        const points = {};
        const pelvisLocal = (x, y, z) => rotateYawRoll(v(x, y, z), pelvisYaw, pelvisRoll);
        const chestLocal = (x, y, z) => rotateYawRoll(v(x, y, z), chestYaw, chestRoll);

        const pelvis = v(pelvisSway, 94.6 + breath * 0.22, 0);
        points.pelvis = pelvis;
        points.spine_01 = pelvis.clone().add(pelvisLocal(pelvisSway * 0.08, 13.0, 0.4));
        points.spine_02 = points.spine_01.clone().add(pelvisLocal(pelvisSway * 0.05, 13.2, 0.2));
        points.spine_03 = points.spine_02.clone().add(chestLocal(-pelvisSway * 0.08, 13.0, 0.0));
        points.spine_04 = points.spine_03.clone().add(chestLocal(-pelvisSway * 0.06, 10.2, -0.1));
        points.spine_05 = points.spine_04.clone().add(chestLocal(-pelvisSway * 0.05, 10.0, -0.2));
        points.neck_01 = points.spine_05.clone().add(chestLocal(0, 12.8, -0.1));
        points.head = points.neck_01.clone().add(chestLocal(weight * 0.35, 16.8, 0.0));
        points.clavicle_l = points.spine_05.clone().add(chestLocal(17.2, 2.8, 0.8));
        points.clavicle_r = points.spine_05.clone().add(chestLocal(-17.2, 2.8, -0.8));

        const armDrift = Math.sin(phase * Math.PI * 2 + 0.7);
        points.upperarm_l = points.clavicle_l.clone().add(chestLocal(14.8, -17.8, -1.2 - armDrift * 1.0));
        points.lowerarm_l = points.upperarm_l.clone().add(chestLocal(10.8, -27.0, 1.4 - armDrift * 0.7));
        points.hand_l = points.lowerarm_l.clone().add(chestLocal(4.8, -22.0, 2.4 - armDrift * 0.5));
        points.upperarm_r = points.clavicle_r.clone().add(chestLocal(-14.8, -17.8, 1.2 - armDrift * 1.0));
        points.lowerarm_r = points.upperarm_r.clone().add(chestLocal(-10.8, -27.0, -1.4 - armDrift * 0.7));
        points.hand_r = points.lowerarm_r.clone().add(chestLocal(-4.8, -22.0, -2.4 - armDrift * 0.5));

        points.thigh_l = pelvis.clone().add(pelvisLocal(10.2 + pelvisSway * 0.08, -3.5, 1.0));
        points.thigh_r = pelvis.clone().add(pelvisLocal(-10.2 + pelvisSway * 0.08, -3.5, -1.0));
        points.foot_l = v(12.4, 8.3 + leftFree * 0.75, 7.2);
        points.ball_l = v(13.7, 1.2, 22.0);
        points.foot_r = v(-12.4, 8.3 + rightFree * 0.75, 7.2);
        points.ball_r = v(-13.7, 1.2, 22.0);
        points.calf_l = solveKnee(points.thigh_l, points.foot_l, 1).add(v(0, -leftFree * 0.25, leftFree * 2.8));
        points.calf_r = solveKnee(points.thigh_r, points.foot_r, -1).add(v(0, -rightFree * 0.25, rightFree * 2.8));
        points._leftPlanted = true;
        points._rightPlanted = true;
        return points;
      }

      function buildCrouchTwerkPose(index) {
        const phase = index / frameCount;
        const bounce = Math.sin(phase * Math.PI * 4);
        const rollPulse = Math.sin(phase * Math.PI * 8);
        const rollCounter = Math.cos(phase * Math.PI * 8);
        const sway = Math.sin(phase * Math.PI * 2 + 0.35);
        const points = {};
        const pelvisYaw = rollPulse * 0.095 + sway * 0.035;
        const pelvisPitch = 0.13 + bounce * 0.025;
        const pelvisRoll = rollCounter * 0.13 + sway * 0.035;
        const chestYaw = -pelvisYaw * 0.55 + sway * 0.035;
        const chestPitch = 0.36 - bounce * 0.030;
        const chestRoll = -pelvisRoll * 0.48;
        const pelvisLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), pelvisYaw, pelvisPitch, pelvisRoll);
        const chestLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), chestYaw, chestPitch, chestRoll);

        const pelvis = v(sway * 1.4, 66.5 + bounce * 2.2 + rollCounter * 0.55, -5.2 + rollPulse * 2.6);
        points.pelvis = pelvis;
        points.spine_01 = pelvis.clone().add(pelvisLocal(sway * 0.25, 11.8, 3.4));
        points.spine_02 = points.spine_01.clone().add(pelvisLocal(sway * 0.12, 12.5, 4.2));
        points.spine_03 = points.spine_02.clone().add(chestLocal(-sway * 0.35, 12.5, 3.9));
        points.spine_04 = points.spine_03.clone().add(chestLocal(-sway * 0.22, 9.8, 2.6));
        points.spine_05 = points.spine_04.clone().add(chestLocal(-sway * 0.18, 9.6, 1.9));
        points.neck_01 = points.spine_05.clone().add(chestLocal(-sway * 0.18, 12.4, -0.5));
        points.head = points.neck_01.clone().add(chestLocal(-sway * 0.30, 16.4, -1.3));
        points.clavicle_l = points.spine_05.clone().add(chestLocal(17.2, 2.6, 1.4));
        points.clavicle_r = points.spine_05.clone().add(chestLocal(-17.2, 2.6, -1.4));

        const armCounter = Math.sin(phase * Math.PI * 4 + 1.1);
        points.upperarm_l = points.clavicle_l.clone().add(chestLocal(17.0, -11.8, 8.8 + armCounter * 1.3));
        points.lowerarm_l = points.upperarm_l.clone().add(chestLocal(12.4, -20.2, 8.5 - armCounter * 0.8));
        points.hand_l = points.lowerarm_l.clone().add(chestLocal(4.2, -16.0, 6.4));
        points.upperarm_r = points.clavicle_r.clone().add(chestLocal(-17.0, -11.8, 8.8 - armCounter * 1.3));
        points.lowerarm_r = points.upperarm_r.clone().add(chestLocal(-12.4, -20.2, 8.5 + armCounter * 0.8));
        points.hand_r = points.lowerarm_r.clone().add(chestLocal(-4.2, -16.0, 6.4));

        points.thigh_l = pelvis.clone().add(pelvisLocal(11.8 + sway * 0.25, -3.8, 2.7));
        points.thigh_r = pelvis.clone().add(pelvisLocal(-11.8 + sway * 0.25, -3.8, -2.7));
        points.foot_l = v(18.6, 8.3, 5.8);
        points.ball_l = v(21.4, 1.2, 21.8);
        points.foot_r = v(-18.6, 8.3, 5.8);
        points.ball_r = v(-21.4, 1.2, 21.8);
        points.calf_l = solveKnee(points.thigh_l, points.foot_l, 1).add(v(3.4, -2.8, 8.2 + Math.max(0, bounce) * 1.8));
        points.calf_r = solveKnee(points.thigh_r, points.foot_r, -1).add(v(-3.4, -2.8, 8.2 + Math.max(0, bounce) * 1.8));
        points._leftPlanted = true;
        points._rightPlanted = true;
        return points;
      }

      function buildSwordSlashPose(index) {
        const key = sampleSwordSlashKey(index);
        const deg = THREE.MathUtils.degToRad;
        const points = {};
        const pelvisYaw = deg(key.pelvis[4]);
        const pelvisPitch = deg(key.pelvis[3]);
        const pelvisRoll = deg(key.pelvis[5]);
        const chestYaw = deg(key.chest[1]);
        const chestPitch = deg(key.chest[0]);
        const chestRoll = deg(key.chest[2]);
        const neckYaw = deg(key.neck[1]);
        const neckPitch = deg(key.neck[0]);
        const neckRoll = deg(key.neck[2]);
        const headYaw = deg(key.head[1]);
        const headPitch = deg(key.head[0]);
        const headRoll = deg(key.head[2]);
        const pelvisLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), pelvisYaw, pelvisPitch, pelvisRoll);
        const chestLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), chestYaw, chestPitch, chestRoll);
        const neckLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), neckYaw, neckPitch, neckRoll);
        const headLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), headYaw, headPitch, headRoll);

        const pelvis = v(key.pelvis[0], 94.5 + key.pelvis[1], key.pelvis[2]);
        points.pelvis = pelvis;
        points.spine_01 = pelvis.clone().add(pelvisLocal(0, 12.9, 0.6));
        points.spine_02 = points.spine_01.clone().add(pelvisLocal(0, 13.1, 0.4));
        points.spine_03 = points.spine_02.clone().add(chestLocal(0.4, 12.8, 0.3));
        points.spine_04 = points.spine_03.clone().add(chestLocal(0.6, 10.2, 0.1));
        points.spine_05 = points.spine_04.clone().add(chestLocal(0.8, 10.0, -0.1));
        points.neck_01 = points.spine_05.clone().add(neckLocal(0.4, 12.7, 0.0));
        points.head = points.neck_01.clone().add(headLocal(0.2, 16.7, 0.2));
        points.clavicle_l = points.spine_05.clone().add(chestLocal(17.8, 3.0, 0.8));
        points.clavicle_r = points.spine_05.clone().add(chestLocal(-17.8, 3.0, -0.8));

        points.upperarm_l = points.clavicle_l.clone().add(chestLocal(...key.lUpper));
        points.lowerarm_l = points.upperarm_l.clone().add(chestLocal(...key.lLower));
        points.hand_l = points.lowerarm_l.clone().add(chestLocal(...key.lHand));
        points.upperarm_r = points.clavicle_r.clone().add(chestLocal(...key.rUpper));
        points.lowerarm_r = points.upperarm_r.clone().add(chestLocal(...key.rLower));
        points.hand_r = points.lowerarm_r.clone().add(chestLocal(...key.rHand));

        points.thigh_l = pelvis.clone().add(pelvisLocal(10.4, -3.8, 2.2));
        points.thigh_r = pelvis.clone().add(pelvisLocal(-10.4, -3.8, -2.2));
        points.foot_l = v(13.2, 8.3, 22.0);
        points.ball_l = v(14.8, 1.2, 36.5);
        points.foot_r = v(-13.8, 8.3, -7.0);
        points.ball_r = v(-15.2, 1.2, 7.0);
        points.calf_l = solveKnee(points.thigh_l, points.foot_l, 1).add(v(key.knee[0], key.knee[1], key.knee[2]));
        points.calf_r = solveKnee(points.thigh_r, points.foot_r, -1).add(v(key.knee[3], key.knee[4], key.knee[5]));

        const swordGrip = points.hand_r.clone();
        points._swordGrip = swordGrip;
        points._swordTip = swordGrip.clone().add(chestLocal(...key.swordTip));
        points._swordPommel = swordGrip.clone().add(chestLocal(...key.swordPommel));
        points._swordRight = chestLocal(1, 0, 0).normalize();
        const faceYaw = chestYaw * 0.65 + neckYaw + headYaw;
        const facePitch = chestPitch * 0.25 + neckPitch * 0.45 + headPitch;
        const faceRoll = chestRoll * 0.35 + neckRoll + headRoll;
        points._faceRight = rotateYawPitchRoll(v(1, 0, 0), faceYaw, facePitch, faceRoll).normalize();
        points._faceUp = rotateYawPitchRoll(v(0, 1, 0), faceYaw, facePitch, faceRoll).normalize();
        points._faceForward = new THREE.Vector3().crossVectors(points._faceRight, points._faceUp).normalize();
        points._leftPlanted = true;
        points._rightPlanted = true;
        return points;
      }

      function makeWallPeekFallbackKey(index) {
        return {
          frame: index,
          pelvis: [-2.4, 94.5, 0.8, 0, 0, 0],
          chest: [0, 0, 0],
          neck: [0, 0, 0],
          head: [0, 0, 0],
          lUpper: [14.4, -17.2, -1.0],
          lLower: [10.8, -26.0, 1.3],
          lHand: [4.8, -22.0, 2.5],
          rUpper: [-14.6, -17.5, 1.2],
          rLower: [-10.6, -26.4, -1.3],
          rHand: [-4.8, -22.0, -2.4],
          leftFoot: { ankle: [10.8, 8.3, 8.6], ball: [12.2, 1.2, 23.4], planted: true },
          rightFoot: { ankle: [-11.4, 8.3, 6.6], ball: [-12.7, 1.2, 21.2], planted: true },
          leftKneeBias: [0, 0, 0],
          rightKneeBias: [0, 0, 0]
        };
      }

      function readWallPeekFoot(source, sideName, fallback) {
        const shortName = sideName === "left" ? "l" : "r";
        const foot = source?.[`${sideName}Foot`] || source?.[`${shortName}Foot`] || source?.feet?.[sideName] || {};
        const contacts = source?.contacts || source?.contact || {};
        const contactValue = contacts?.[`${sideName}Foot`] ?? contacts?.[sideName] ?? source?.[`${sideName}Planted`] ?? source?.[`_${sideName}Planted`];
        const planted = contactValue == null ? fallback.planted : Boolean(contactValue);
        return {
          ankle: readNaturalRunArray(foot, ["ankle", "foot", "position", "pos"], 3, fallback.ankle),
          ball: readNaturalRunArray(foot, ["ball", "toe"], 3, fallback.ball),
          planted
        };
      }

      function normalizeWallPeekKey(source, fallbackIndex) {
        const fallback = makeWallPeekFallbackKey(fallbackIndex);
        return {
          frame: readNaturalKeyFrame(source, fallback.frame),
          pelvis: readNaturalRunTransform(source, ["pelvis", "root"], fallback.pelvis),
          chest: readNaturalRunRotation(source, ["chest", "spine", "spine_05", "bones.spine_05"], fallback.chest),
          neck: readNaturalRunRotation(source, ["neck", "neck_01", "bones.neck_01"], fallback.neck),
          head: readNaturalRunRotation(source, ["head", "bones.head"], fallback.head),
          lUpper: readNaturalRunArray(source, ["lUpper", "leftUpper", "leftUpperArm", "upperarm_l"], 3, fallback.lUpper),
          lLower: readNaturalRunArray(source, ["lLower", "leftLower", "leftLowerArm", "lowerarm_l"], 3, fallback.lLower),
          lHand: readNaturalRunArray(source, ["lHand", "leftHand", "hand_l"], 3, fallback.lHand),
          rUpper: readNaturalRunArray(source, ["rUpper", "rightUpper", "rightUpperArm", "upperarm_r"], 3, fallback.rUpper),
          rLower: readNaturalRunArray(source, ["rLower", "rightLower", "rightLowerArm", "lowerarm_r"], 3, fallback.rLower),
          rHand: readNaturalRunArray(source, ["rHand", "rightHand", "hand_r"], 3, fallback.rHand),
          leftFoot: readWallPeekFoot(source, "left", fallback.leftFoot),
          rightFoot: readWallPeekFoot(source, "right", fallback.rightFoot),
          leftKneeBias: readNaturalRunArray(source, ["leftKneeBias", "kneeBias.left", "knee_l", "calfBias_l", "leftLeg.kneeBias"], 3, fallback.leftKneeBias),
          rightKneeBias: readNaturalRunArray(source, ["rightKneeBias", "kneeBias.right", "knee_r", "calfBias_r", "rightLeg.kneeBias"], 3, fallback.rightKneeBias)
        };
      }

      function interpolateWallPeekFoot(a, b, t) {
        const bothPlanted = a.planted && b.planted;
        return {
          ankle: bothPlanted ? a.ankle.slice(0, 3) : interpolateNaturalRunArray(a.ankle, b.ankle, t),
          ball: bothPlanted ? a.ball.slice(0, 3) : interpolateNaturalRunArray(a.ball, b.ball, t),
          planted: t < 0.5 ? a.planted : b.planted
        };
      }

      function sampleWallPeekKey(index) {
        const pair = keyposePair(wallPeekRawKeys, index, frameCount);
        if (!pair) {
          return normalizeWallPeekKey(makeWallPeekFallbackKey(index), index);
        }
        const from = normalizeWallPeekKey(pair.from.key, readNaturalKeyFrame(pair.from.key, 0));
        const to = normalizeWallPeekKey(pair.to.key, readNaturalKeyFrame(pair.to.key, 0));
        return {
          pelvis: interpolateNaturalRunArray(from.pelvis, to.pelvis, pair.t),
          chest: interpolateNaturalRunArray(from.chest, to.chest, pair.t),
          neck: interpolateNaturalRunArray(from.neck, to.neck, pair.t),
          head: interpolateNaturalRunArray(from.head, to.head, pair.t),
          lUpper: interpolateNaturalRunArray(from.lUpper, to.lUpper, pair.t),
          lLower: interpolateNaturalRunArray(from.lLower, to.lLower, pair.t),
          lHand: interpolateNaturalRunArray(from.lHand, to.lHand, pair.t),
          rUpper: interpolateNaturalRunArray(from.rUpper, to.rUpper, pair.t),
          rLower: interpolateNaturalRunArray(from.rLower, to.rLower, pair.t),
          rHand: interpolateNaturalRunArray(from.rHand, to.rHand, pair.t),
          leftFoot: interpolateWallPeekFoot(from.leftFoot, to.leftFoot, pair.t),
          rightFoot: interpolateWallPeekFoot(from.rightFoot, to.rightFoot, pair.t),
          leftKneeBias: interpolateNaturalRunArray(from.leftKneeBias, to.leftKneeBias, pair.t),
          rightKneeBias: interpolateNaturalRunArray(from.rightKneeBias, to.rightKneeBias, pair.t)
        };
      }

      function buildWallPeekPose(index) {
        const key = sampleWallPeekKey(index);
        const deg = THREE.MathUtils.degToRad;
        const points = {};
        const pelvisYaw = deg(key.pelvis[4] || 0);
        const pelvisPitch = deg(key.pelvis[3] || 0);
        const pelvisRoll = deg(key.pelvis[5] || 0);
        const chestYaw = deg(key.chest[1] || 0);
        const chestPitch = deg(key.chest[0] || 0);
        const chestRoll = deg(key.chest[2] || 0);
        const neckYaw = deg(key.neck[1] || 0);
        const neckPitch = deg(key.neck[0] || 0);
        const neckRoll = deg(key.neck[2] || 0);
        const headYaw = deg(key.head[1] || 0);
        const headPitch = deg(key.head[0] || 0);
        const headRoll = deg(key.head[2] || 0);
        const pelvisLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), pelvisYaw, pelvisPitch, pelvisRoll);
        const chestLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), chestYaw, chestPitch, chestRoll);
        const neckLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), neckYaw, neckPitch, neckRoll);
        const headLocal = (x, y, z) => rotateYawPitchRoll(v(x, y, z), headYaw, headPitch, headRoll);

        const pelvis = v(key.pelvis[0], key.pelvis[1], key.pelvis[2]);
        points.pelvis = pelvis;
        points.spine_01 = pelvis.clone().add(pelvisLocal(0, 13.0, -0.8));
        points.spine_02 = points.spine_01.clone().add(pelvisLocal(0, 13.2, -0.6));
        points.spine_03 = points.spine_02.clone().add(chestLocal(0.4, 13.0, -0.35));
        points.spine_04 = points.spine_03.clone().add(chestLocal(0.6, 10.2, -0.12));
        points.spine_05 = points.spine_04.clone().add(chestLocal(0.8, 10.0, 0.15));
        points.neck_01 = points.spine_05.clone().add(neckLocal(0.4, 12.8, 0.35));
        points.head = points.neck_01.clone()
          .add(neckLocal(0.2, 8.4, 0.4))
          .add(headLocal(0.2, 8.4, 1.0));
        points.clavicle_l = points.spine_05.clone().add(chestLocal(17.8, 3.0, 0.8));
        points.clavicle_r = points.spine_05.clone().add(chestLocal(-17.8, 3.0, -0.8));

        points.upperarm_l = points.clavicle_l.clone().add(chestLocal(...key.lUpper));
        points.lowerarm_l = points.upperarm_l.clone().add(chestLocal(...key.lLower));
        points.hand_l = points.lowerarm_l.clone().add(chestLocal(...key.lHand));
        points.upperarm_r = points.clavicle_r.clone().add(chestLocal(...key.rUpper));
        points.lowerarm_r = points.upperarm_r.clone().add(chestLocal(...key.rLower));
        points.hand_r = points.lowerarm_r.clone().add(chestLocal(...key.rHand));

        points.thigh_l = pelvis.clone().add(pelvisLocal(10.1, -3.5, 1.0));
        points.thigh_r = pelvis.clone().add(pelvisLocal(-10.1, -3.5, -1.0));
        points.foot_l = v(...key.leftFoot.ankle);
        points.ball_l = v(...key.leftFoot.ball);
        points.foot_r = v(...key.rightFoot.ankle);
        points.ball_r = v(...key.rightFoot.ball);
        points.calf_l = solveKnee(points.thigh_l, points.foot_l, 1).add(v(...key.leftKneeBias));
        points.calf_r = solveKnee(points.thigh_r, points.foot_r, -1).add(v(...key.rightKneeBias));
        const faceYaw = chestYaw * 0.35 + neckYaw * 0.55 + headYaw;
        const facePitch = chestPitch * 0.18 + neckPitch * 0.45 + headPitch;
        const faceRoll = chestRoll * 0.25 + neckRoll * 0.45 + headRoll;
        points._faceRight = rotateYawPitchRoll(v(1, 0, 0), faceYaw, facePitch, faceRoll).normalize();
        points._faceUp = rotateYawPitchRoll(v(0, 1, 0), faceYaw, facePitch, faceRoll).normalize();
        points._faceForward = new THREE.Vector3().crossVectors(points._faceRight, points._faceUp).normalize();
        points._leftPlanted = key.leftFoot.planted;
        points._rightPlanted = key.rightFoot.planted;
        return points;
      }

      function buildOverheadPose(index) {
        const param = overheadWaveParam(index, frameCount);
        const sway = param.sway;
        const reach = param.reach;
        const lift = param.lift;
        const depth = param.depth;
        const bodySway = sway * 0.75 + reach * 0.25;
        const armDepth = depth * 12;
        const points = {};
        const rotations = {};

        function q(x = 0, y = 0, z = 0) {
          return new THREE.Quaternion().setFromEuler(new THREE.Euler(x, y, z, "XYZ"));
        }

        function place(parent, name, offset, localRotation = q()) {
          points[name] = points[parent].clone().add(offset.clone().applyQuaternion(rotations[parent]));
          rotations[name] = rotations[parent].clone().multiply(localRotation);
        }

        function plantLeg(thighName, calfName, footName, ballName, footPosition, ballPosition, side) {
          points[footName] = footPosition.clone();
          points[ballName] = ballPosition.clone();
          const thigh = points[thighName];
          const kneeBias = v(side * 4 + bodySway * 1.5, -4, -3);
          points[calfName] = thigh.clone().lerp(points[footName], 0.52).add(kneeBias);
          rotations[calfName] = rotations[thighName].clone();
          rotations[footName] = rotations[calfName].clone();
          rotations[ballName] = rotations[footName].clone();
        }

        points.pelvis = v(-bodySway * 3.5, 96 + lift * 0.7, depth * -2.0);
        rotations.pelvis = q(depth * 0.04, depth * 0.05, -bodySway * 0.055);

        place("pelvis", "spine_01", v(0, 13, 0), q(0, depth * 0.01, -bodySway * 0.020));
        place("spine_01", "spine_02", v(0, 13, 0), q(0, depth * 0.012, -bodySway * 0.024));
        place("spine_02", "spine_03", v(0, 13, 0), q(0, depth * 0.014, -bodySway * 0.028));
        place("spine_03", "spine_04", v(0, 11, 0), q(0, depth * 0.016, -bodySway * 0.032));
        place("spine_04", "spine_05", v(0, 9, 0), q(0, depth * 0.018, -bodySway * 0.036));
        place("spine_05", "neck_01", v(0, 13 + lift * 0.4, 0), q(depth * 0.018, depth * 0.012, -bodySway * 0.025));
        place("neck_01", "head", v(0, 16, 0), q(depth * 0.014, -depth * 0.010, -bodySway * 0.020));

        place("spine_05", "clavicle_l", v(15, 3, 1), q(0, depth * 0.020, -0.10 - bodySway * 0.015));
        place("spine_05", "clavicle_r", v(-15, 3, -1), q(0, -depth * 0.020, 0.10 - bodySway * 0.015));
        place("clavicle_l", "upperarm_l", v(18 + reach * 11, 25 + lift * 3, 6 + armDepth), q(depth * 0.05, reach * 0.04, -0.12));
        place("clavicle_r", "upperarm_r", v(-18 + reach * 11, 25 + lift * 3, -6 - armDepth), q(-depth * 0.05, reach * 0.04, 0.12));
        place("upperarm_l", "lowerarm_l", v(10 + reach * 14, 23 + lift * 3, 4 + armDepth * 0.45), q(0, reach * 0.025, -0.06));
        place("upperarm_r", "lowerarm_r", v(-10 + reach * 14, 23 + lift * 3, -4 - armDepth * 0.45), q(0, reach * 0.025, 0.06));
        place("lowerarm_l", "hand_l", v(3 + reach * 8, 12 + lift * 2, 2), q());
        place("lowerarm_r", "hand_r", v(-3 + reach * 8, 12 + lift * 2, -2), q());

        place("pelvis", "thigh_l", v(12 + bodySway * 2, -5, 4), q(0, 0, bodySway * 0.025));
        place("pelvis", "thigh_r", v(-12 + bodySway * 2, -5, -4), q(0, 0, bodySway * 0.025));
        plantLeg("thigh_l", "calf_l", "foot_l", "ball_l", v(17, 10, 10), v(22, 2, 25), 1);
        plantLeg("thigh_r", "calf_r", "foot_r", "ball_r", v(-17, 10, 10), v(-22, 2, 25), -1);

        points._leftPlanted = true;
        points._rightPlanted = true;
        return points;
      }

      const figure = new THREE.Group();
      scene.add(figure);

      const envelopeRenderOrder = 1;
      const boneRenderOrder = 2;
      const faceRenderOrder = 3;
      const envelopeMaterial = new THREE.MeshBasicMaterial({
        color: colors.envelope,
        transparent: true,
        opacity: 0.38,
        depthTest: true,
        depthWrite: false
      });
      const headEnvelopeMaterial = envelopeMaterial.clone();
      headEnvelopeMaterial.color.setHex(0xf2c7a5);
      headEnvelopeMaterial.opacity = 0.7;
      const envelopeSegments = [
        ["pelvis", "spine_01", 8.1],
        ["spine_01", "spine_03", 9.0],
        ["spine_03", "spine_05", 8.6],
        ["spine_05", "neck_01", 5.5],
        ["clavicle_l", "clavicle_r", 4.2],
        ["clavicle_l", "upperarm_l", 4.1],
        ["upperarm_l", "lowerarm_l", 3.3],
        ["lowerarm_l", "hand_l", 2.5],
        ["clavicle_r", "upperarm_r", 4.1],
        ["upperarm_r", "lowerarm_r", 3.3],
        ["lowerarm_r", "hand_r", 2.5],
        ["thigh_l", "calf_l", 4.9],
        ["calf_l", "foot_l", 3.8],
        ["foot_l", "ball_l", 2.8],
        ["thigh_r", "calf_r", 4.9],
        ["calf_r", "foot_r", 3.8],
        ["foot_r", "ball_r", 2.8]
      ];
      const envelopeMeshes = envelopeSegments.map(([from, to, radius]) => {
        const mesh = new THREE.Mesh(new THREE.CylinderGeometry(1, 1, 1, 20), envelopeMaterial);
        mesh.renderOrder = envelopeRenderOrder;
        figure.add(mesh);
        return { from, to, radius, mesh };
      });
      const envelopeJointGeometry = new THREE.SphereGeometry(1, 24, 16);
      const pelvisEnvelopeGeometry = new THREE.CylinderGeometry(1, 1, 1, 24);
      const envelopeJoints = [
        ["head", v(10.95, 14.1, 10.95)],
        ["pelvis", v(5.8, 20.2, 5.8), "pelvisCylinder"],
        ["spine_04", v(10.2, 11.5, 7.1)],
        ["hand_l", v(3.2, 3.4, 3.2)],
        ["hand_r", v(3.2, 3.4, 3.2)],
        ["foot_l", v(4.4, 2.2, 7.8)],
        ["foot_r", v(4.4, 2.2, 7.8)]
      ].map(([name, scale, shape = "sphere"]) => {
        const material = name === "head" ? headEnvelopeMaterial : envelopeMaterial;
        const geometry = shape === "pelvisCylinder" ? pelvisEnvelopeGeometry : envelopeJointGeometry;
        const mesh = new THREE.Mesh(geometry, material);
        if (name !== "head") {
          mesh.renderOrder = envelopeRenderOrder;
        }
        figure.add(mesh);
        return { name, scale, shape, mesh };
      });

      const segmentMeshes = segments.map((segment) => {
        const material = new THREE.MeshStandardMaterial({
          color: colors[segment[2]],
          transparent: true,
          opacity: 1.0,
          depthTest: true,
          depthWrite: true,
          roughness: 0.55,
          metalness: 0.04
        });
        const radius = segment[2] === "center" ? 1.45 : 1.75;
        const mesh = new THREE.Mesh(new THREE.CylinderGeometry(radius, radius, 1, 16), material);
        mesh.renderOrder = boneRenderOrder;
        figure.add(mesh);
        return { segment, mesh };
      });

      const jointGeometry = new THREE.SphereGeometry(2.75, 18, 12);
      const jointMeshes = jointNames.map((name) => {
        const mesh = new THREE.Mesh(
          jointGeometry,
          new THREE.MeshStandardMaterial({
            color: colors.joint,
            transparent: true,
            opacity: 1.0,
            depthTest: true,
            depthWrite: true,
            roughness: 0.48
          })
        );
        mesh.renderOrder = boneRenderOrder;
        figure.add(mesh);
        return { name, mesh };
      });

      const faceMaterial = new THREE.MeshBasicMaterial({
        color: 0x202833,
        depthTest: true,
        depthWrite: true
      });
      const eyeGeometry = new THREE.SphereGeometry(1.0, 12, 8);
      const faceMarkers = {
        eyeL: new THREE.Mesh(eyeGeometry, faceMaterial),
        eyeR: new THREE.Mesh(eyeGeometry, faceMaterial),
        mouth: new THREE.Mesh(new THREE.BoxGeometry(5.2, 0.55, 0.55), faceMaterial)
      };
      Object.values(faceMarkers).forEach((mesh) => {
        mesh.renderOrder = faceRenderOrder;
        figure.add(mesh);
      });

      const plantedMarkers = [
        new THREE.Mesh(
          new THREE.CylinderGeometry(5.5, 5.5, 0.8, 24),
          new THREE.MeshBasicMaterial({ color: colors.footPlant, transparent: true, opacity: 0.18 })
        ),
        new THREE.Mesh(
          new THREE.CylinderGeometry(5.5, 5.5, 0.8, 24),
          new THREE.MeshBasicMaterial({ color: colors.footPlant, transparent: true, opacity: 0.18 })
        )
      ];
      plantedMarkers.forEach((marker) => scene.add(marker));

      function updateCylinder(mesh, a, b) {
        const mid = new THREE.Vector3().addVectors(a, b).multiplyScalar(0.5);
        const diff = new THREE.Vector3().subVectors(b, a);
        const length = Math.max(diff.length(), 0.001);
        mesh.position.copy(mid);
        mesh.scale.set(1, length, 1);
        mesh.quaternion.setFromUnitVectors(new THREE.Vector3(0, 1, 0), diff.normalize());
      }

      function updateLine(line, points) {
        line.geometry.dispose();
        line.geometry = new THREE.BufferGeometry().setFromPoints(points);
        line.geometry.computeBoundingSphere();
      }

      function updateEnvelopeCylinder(mesh, a, b, radius) {
        const mid = new THREE.Vector3().addVectors(a, b).multiplyScalar(0.5);
        const diff = new THREE.Vector3().subVectors(b, a);
        const length = Math.max(diff.length(), 0.001);
        mesh.position.copy(mid);
        mesh.scale.set(radius, length, radius);
        mesh.quaternion.setFromUnitVectors(new THREE.Vector3(0, 1, 0), diff.normalize());
      }

      function updateSwordProp(pose) {
        const visible = currentMode === "swordSlash" && pose._swordGrip && pose._swordTip;
        swordGroup.visible = visible;
        if (!visible) {
          return;
        }
        const grip = pose._swordGrip;
        const tip = pose._swordTip;
        const pommel = pose._swordPommel || grip.clone().add(v(0, -7, 0));
        const right = (pose._swordRight || new THREE.Vector3().subVectors(pose.clavicle_l, pose.clavicle_r)).clone().normalize();
        updateCylinder(swordBlade, grip, tip);
        updateCylinder(swordHandle, pommel, grip);
        updateCylinder(
          swordGuard,
          grip.clone().add(right.clone().multiplyScalar(5.8)),
          grip.clone().add(right.clone().multiplyScalar(-5.8))
        );
      }

      function readBoxVector(source, paths, fallback = null) {
        for (const path of paths) {
          const value = path.includes(".") ? naturalRunPath(source, path) : source?.[path];
          const result = naturalRunArray(value, 3, null);
          if (result) {
            return result;
          }
        }
        return fallback ? fallback.slice(0, 3) : null;
      }

      function boxFrameDataFromKey(key, fallback = null) {
        const frameBox = key?.box || key?.prop?.box || key?.props?.box || {};
        const globalBox = boxOverheadLiftSource.box || boxOverheadLiftSource.prop || {};
        const center = readBoxVector(frameBox, ["center", "position", "pos"], fallback?.center || null);
        const size = readBoxVector(frameBox, ["size", "dimensions"], null)
          || readBoxVector(globalBox, ["size", "dimensions"], fallback?.size || [42, 24, 24]);
        return center || size ? { center, size } : fallback;
      }

      function sampleBoxOverheadLiftBox(index) {
        if (!Array.isArray(boxOverheadLiftRawKeys) || boxOverheadLiftRawKeys.length === 0) {
          return null;
        }
        if (boxOverheadLiftRawKeys.length === 1) {
          return boxFrameDataFromKey(boxOverheadLiftRawKeys[0], null);
        }
        const pair = keyposePair(boxOverheadLiftRawKeys, index, frameCount);
        if (!pair) {
          return boxFrameDataFromKey(boxOverheadLiftRawKeys[0], null);
        }
        const from = boxFrameDataFromKey(pair.from.key, null);
        const to = boxFrameDataFromKey(pair.to.key, from);
        const fallback = from || to;
        if (!fallback) {
          return null;
        }
        return {
          center: from?.center && to?.center ? interpolateNaturalRunArray(from.center, to.center, pair.t) : fallback.center,
          size: from?.size && to?.size ? interpolateNaturalRunArray(from.size, to.size, pair.t) : fallback.size
        };
      }

      function updateLiftBoxProp(pose, index) {
        const visible = currentMode === "boxOverheadLift";
        liftBoxGroup.visible = visible;
        if (!visible) {
          return;
        }
        const boxOptions = boxOverheadLiftSource.box || {};
        const sampledBox = sampleBoxOverheadLiftBox(index);
        const handCenter = new THREE.Vector3().addVectors(pose.hand_l, pose.hand_r).multiplyScalar(0.5);
        const across = new THREE.Vector3().subVectors(pose.hand_l, pose.hand_r);
        const fallbackAcross = new THREE.Vector3().subVectors(pose.clavicle_l, pose.clavicle_r);
        const right = (across.lengthSq() > 0.001 ? across : fallbackAcross).normalize();
        const bodyUp = (pose._faceUp || new THREE.Vector3().subVectors(pose.head, pose.neck_01)).clone().normalize();
        let forward = new THREE.Vector3().crossVectors(right, bodyUp);
        if (forward.lengthSq() < 0.001) {
          forward = (pose._faceForward || v(0, 0, 1)).clone();
        }
        forward.normalize();
        const up = new THREE.Vector3().crossVectors(forward, right).normalize();
        const configuredWidth = readNumberOption(boxOptions, ["width", "size.width"], 42);
        const configuredHeight = readNumberOption(boxOptions, ["height", "size.height"], 24);
        const configuredDepth = readNumberOption(boxOptions, ["depth", "thickness", "size.depth", "size.thickness"], 24);
        const width = sampledBox?.size ? sampledBox.size[0] : Math.max(configuredWidth, Math.min(52, across.length() + 6));
        const height = sampledBox?.size ? sampledBox.size[1] : configuredHeight;
        const depth = sampledBox?.size ? sampledBox.size[2] : configuredDepth;
        const center = sampledBox?.center
          ? v(sampledBox.center[0], sampledBox.center[1], sampledBox.center[2])
          : handCenter.clone().add(up.clone().multiplyScalar(1.5));
        const basis = new THREE.Matrix4().makeBasis(right, up, forward);
        liftBoxGroup.position.copy(center);
        liftBoxGroup.quaternion.setFromRotationMatrix(basis);
        liftBoxGroup.scale.set(width, height, depth);
      }

      function seatedBottleFrameDataFromKey(key, fallback = null) {
        const bottle = key?.bottle || key?.prop?.bottle || key?.props?.bottle || {};
        const bottom = readBoxVector(bottle, ["bottom", "base"], fallback?.bottom || null);
        const top = readBoxVector(bottle, ["top", "cap", "mouth"], fallback?.top || null);
        const center = readBoxVector(bottle, ["center", "position", "pos"], fallback?.center || null);
        return bottom || top || center ? { bottom, top, center } : fallback;
      }

      function sampleSeatedBottle(index) {
        if (!Array.isArray(seatedBottleDrinkRawKeys) || seatedBottleDrinkRawKeys.length === 0) {
          return null;
        }
        if (seatedBottleDrinkRawKeys.length === 1) {
          return seatedBottleFrameDataFromKey(seatedBottleDrinkRawKeys[0], null);
        }
        const pair = keyposePair(seatedBottleDrinkRawKeys, index, frameCount);
        if (!pair) {
          return seatedBottleFrameDataFromKey(seatedBottleDrinkRawKeys[0], null);
        }
        const from = seatedBottleFrameDataFromKey(pair.from.key, null);
        const to = seatedBottleFrameDataFromKey(pair.to.key, from);
        const fallback = from || to;
        if (!fallback) {
          return null;
        }
        return {
          bottom: from?.bottom && to?.bottom ? interpolateNaturalRunArray(from.bottom, to.bottom, pair.t) : fallback.bottom,
          top: from?.top && to?.top ? interpolateNaturalRunArray(from.top, to.top, pair.t) : fallback.top,
          center: from?.center && to?.center ? interpolateNaturalRunArray(from.center, to.center, pair.t) : fallback.center
        };
      }

      function updateSeatedBottleDrinkProps(index) {
        const visible = currentMode === "seatedBottleDrink";
        seatedDrinkGroup.visible = visible;
        if (!visible) {
          return;
        }
        const sampled = sampleSeatedBottle(index);
        const bottom = sampled?.bottom ? v(sampled.bottom[0], sampled.bottom[1], sampled.bottom[2]) : v(48, 62, 30);
        const top = sampled?.top ? v(sampled.top[0], sampled.top[1], sampled.top[2]) : v(48, 94, 30);
        const capBase = bottom.clone().lerp(top, 0.84);
        updateCylinder(seatedDrinkBottleBody, bottom, capBase);
        updateCylinder(seatedDrinkBottleCap, capBase, top);
      }

      function batFrameDataFromKey(key, fallback = null) {
        const bat = key?.bat || key?.prop?.bat || key?.props?.bat || {};
        const knob = readBoxVector(bat, ["knob", "pommel", "handle"], fallback?.knob || null);
        const barrelStart = readBoxVector(bat, ["barrelStart", "barrel_start", "barrelBase"], fallback?.barrelStart || null);
        const tip = readBoxVector(bat, ["tip", "barrelTip", "end"], fallback?.tip || null);
        return knob || barrelStart || tip ? { knob, barrelStart, tip } : fallback;
      }

      function sampleBaseballBat(index) {
        if (!Array.isArray(baseballBatSwingRawKeys) || baseballBatSwingRawKeys.length === 0) {
          return null;
        }
        if (baseballBatSwingRawKeys.length === 1) {
          return batFrameDataFromKey(baseballBatSwingRawKeys[0], null);
        }
        const pair = keyposePair(baseballBatSwingRawKeys, index, frameCount);
        if (!pair) {
          return batFrameDataFromKey(baseballBatSwingRawKeys[0], null);
        }
        const from = batFrameDataFromKey(pair.from.key, null);
        const to = batFrameDataFromKey(pair.to.key, from);
        const fallback = from || to;
        if (!fallback) {
          return null;
        }
        return {
          knob: from?.knob && to?.knob ? interpolateNaturalRunArray(from.knob, to.knob, pair.t) : fallback.knob,
          barrelStart: from?.barrelStart && to?.barrelStart ? interpolateNaturalRunArray(from.barrelStart, to.barrelStart, pair.t) : fallback.barrelStart,
          tip: from?.tip && to?.tip ? interpolateNaturalRunArray(from.tip, to.tip, pair.t) : fallback.tip
        };
      }

      function updateBaseballBatProp(pose, index) {
        const visible = currentMode === "baseballBatSwing";
        baseballBatGroup.visible = visible;
        if (!visible) {
          return;
        }
        const handCenter = new THREE.Vector3().addVectors(pose.hand_l, pose.hand_r).multiplyScalar(0.5);
        const acrossHands = new THREE.Vector3().subVectors(pose.hand_l, pose.hand_r);
        const right = (pose._faceRight || new THREE.Vector3().subVectors(pose.clavicle_r, pose.clavicle_l)).clone().normalize();
        const up = (pose._faceUp || new THREE.Vector3().subVectors(pose.head, pose.neck_01)).clone().normalize();
        const forward = (pose._faceForward || new THREE.Vector3().crossVectors(right, up)).clone().normalize();
        const maxFrame = Math.max(1, frameCount - 1);
        const phase = clamp(index / maxFrame, 0, 1);
        const handSide = acrossHands.lengthSq() > 0.001 ? acrossHands.clone().normalize() : right.clone();
        const knob = handCenter.clone().add(handSide.clone().multiplyScalar(-3.2));
        const early = clamp(phase / 0.34, 0, 1);
        const through = clamp((phase - 0.28) / 0.34, 0, 1);
        const finish = clamp((phase - 0.58) / 0.42, 0, 1);
        const backLift = 0.9 - early * 0.6;
        const contactSweep = Math.sin(through * Math.PI);
        const followWrap = smoothstep(finish);
        const batDir = right.clone().multiplyScalar(-0.86 + followWrap * 0.72)
          .add(forward.clone().multiplyScalar(-0.46 + through * 1.24 - followWrap * 0.72))
          .add(up.clone().multiplyScalar(backLift - contactSweep * 0.34 + followWrap * 0.42))
          .normalize();
        const tip = knob.clone().add(batDir.multiplyScalar(70));
        const barrelStart = knob.clone().lerp(tip, 0.42);
        updateCylinder(baseballBatHandle, knob, barrelStart);
        updateCylinder(baseballBatBarrel, barrelStart, tip);
        baseballBatKnob.position.copy(knob);
      }

      function archeryFrameDataFromKey(key, fallback = null) {
        const archery = key?.archery || key?.bow || key?.prop?.archery || key?.props?.archery || {};
        const grip = readBoxVector(archery, ["grip", "handle", "bowGrip"], fallback?.grip || null);
        const top = readBoxVector(archery, ["top", "bowTop", "upperTip"], fallback?.top || null);
        const bottom = readBoxVector(archery, ["bottom", "bowBottom", "lowerTip"], fallback?.bottom || null);
        const stringHand = readBoxVector(archery, ["stringHand", "nock", "drawHand"], fallback?.stringHand || null);
        const arrowTip = readBoxVector(archery, ["arrowTip", "tip", "arrowEnd"], fallback?.arrowTip || null);
        const released = Boolean(archery.released ?? fallback?.released);
        return grip || top || bottom || stringHand || arrowTip
          ? { grip, top, bottom, stringHand, arrowTip, released }
          : fallback;
      }

      function sampleArcheryFullDraw(index) {
        if (!Array.isArray(archeryFullDrawRawKeys) || archeryFullDrawRawKeys.length === 0) {
          return null;
        }
        if (archeryFullDrawRawKeys.length === 1) {
          return archeryFrameDataFromKey(archeryFullDrawRawKeys[0], null);
        }
        const pair = keyposePair(archeryFullDrawRawKeys, index, frameCount);
        if (!pair) {
          return archeryFrameDataFromKey(archeryFullDrawRawKeys[0], null);
        }
        const from = archeryFrameDataFromKey(pair.from.key, null);
        const to = archeryFrameDataFromKey(pair.to.key, from);
        const fallback = from || to;
        if (!fallback) {
          return null;
        }
        return {
          grip: from?.grip && to?.grip ? interpolateNaturalRunArray(from.grip, to.grip, pair.t) : fallback.grip,
          top: from?.top && to?.top ? interpolateNaturalRunArray(from.top, to.top, pair.t) : fallback.top,
          bottom: from?.bottom && to?.bottom ? interpolateNaturalRunArray(from.bottom, to.bottom, pair.t) : fallback.bottom,
          stringHand: from?.stringHand && to?.stringHand ? interpolateNaturalRunArray(from.stringHand, to.stringHand, pair.t) : fallback.stringHand,
          arrowTip: from?.arrowTip && to?.arrowTip ? interpolateNaturalRunArray(from.arrowTip, to.arrowTip, pair.t) : fallback.arrowTip,
          released: pair.t < 0.5 ? Boolean(from?.released) : Boolean(to?.released)
        };
      }

      function updateArcheryProp(pose, index) {
        const visible = currentMode === "archeryFullDraw";
        archeryGroup.visible = visible;
        if (!visible) {
          return;
        }
        const sampled = sampleArcheryFullDraw(index);
        const right = (pose._faceRight || new THREE.Vector3().subVectors(pose.clavicle_l, pose.clavicle_r)).clone().normalize();
        const up = (pose._faceUp || new THREE.Vector3().subVectors(pose.head, pose.neck_01)).clone().normalize();
        const forward = (pose._faceForward || new THREE.Vector3().crossVectors(right, up)).clone().normalize();
        const grip = sampled?.grip ? v(sampled.grip[0], sampled.grip[1], sampled.grip[2]) : pose.hand_l.clone();
        const top = sampled?.top ? v(sampled.top[0], sampled.top[1], sampled.top[2]) : grip.clone().add(up.clone().multiplyScalar(43));
        const bottom = sampled?.bottom ? v(sampled.bottom[0], sampled.bottom[1], sampled.bottom[2]) : grip.clone().add(up.clone().multiplyScalar(-43));
        const stringHand = sampled?.stringHand
          ? v(sampled.stringHand[0], sampled.stringHand[1], sampled.stringHand[2])
          : (sampled?.released ? grip.clone() : pose.hand_r.clone());
        const arrowTip = sampled?.arrowTip
          ? v(sampled.arrowTip[0], sampled.arrowTip[1], sampled.arrowTip[2])
          : grip.clone().add(forward.clone().multiplyScalar(52)).add(right.clone().multiplyScalar(6));
        const arrowNock = sampled?.released
          ? arrowTip.clone().add(new THREE.Vector3().subVectors(grip, arrowTip).normalize().multiplyScalar(58))
          : stringHand.clone();

        updateCylinder(archeryBowUpper, grip, top);
        updateCylinder(archeryBowLower, grip, bottom);
        archeryGrip.position.copy(grip);
        updateLine(archeryString, [top, stringHand, bottom]);
        updateCylinder(archeryArrow, arrowNock, arrowTip);
        archeryArrowHead.position.copy(arrowTip);
        archeryArrowHead.quaternion.setFromUnitVectors(
          new THREE.Vector3(0, 1, 0),
          new THREE.Vector3().subVectors(arrowTip, arrowNock).normalize()
        );
      }

      function updateTestMotionProps(index) {
        const phase = frameCount > 1 ? index / Math.max(1, frameCount - 1) : 0;
        lowVaultPropGroup.visible = currentMode === "lowVault";
        doorPushPropGroup.visible = currentMode === "doorPush";
        floorPickupPropGroup.visible = currentMode === "floorPickup";
        ladderPropGroup.visible = currentMode === "ladderClimb";
        slideMatPropGroup.visible = currentMode === "forwardRoll" || currentMode === "kneeSlide";

        if (doorPushPropGroup.visible) {
          const open = smoothstep(clamp((phase - 0.28) / 0.46, 0, 1));
          doorLeafPivot.rotation.y = -open * 1.18;
        } else {
          doorLeafPivot.rotation.y = 0;
        }

        if (floorPickupPropGroup.visible) {
          const lift = smoothstep(clamp((phase - 0.42) / 0.32, 0, 1));
          floorPickupPropGroup.position.set(0, lift * 66, -lift * 12);
        } else {
          floorPickupPropGroup.position.set(0, 0, 0);
        }
      }

      function updateHadokenEffect(pose, index) {
        const releaseFrame = readNumberOption(hadokenSource, ["releaseFrame", "projectileFrame", "effect.releaseFrame", "effectTiming.releaseFrame", "effectTiming.projectileFrame"], 18);
        const visible = currentMode === "hadoken" && index >= releaseFrame - 2;
        hadokenGroup.visible = visible;
        if (!visible) {
          return;
        }
        const maxFrame = Math.max(1, frameCount - 1);
        const travel = clamp((index - releaseFrame) / Math.max(1, maxFrame - releaseFrame - 4), 0, 1);
        const charge = clamp((index - (releaseFrame - 2)) / 2, 0, 1);
        const handCenter = new THREE.Vector3().addVectors(pose.hand_l, pose.hand_r).multiplyScalar(0.5);
        const right = new THREE.Vector3().subVectors(pose.clavicle_l, pose.clavicle_r).normalize();
        const up = (pose._faceUp || new THREE.Vector3().subVectors(pose.head, pose.neck_01)).clone().normalize();
        let forward = pose._faceForward ? pose._faceForward.clone() : new THREE.Vector3().crossVectors(right, up);
        if (forward.lengthSq() < 0.001) {
          forward = v(0, 0, 1);
        }
        forward.normalize();
        const center = handCenter
          .clone()
          .add(forward.clone().multiplyScalar(14 + travel * 92))
          .add(up.clone().multiplyScalar(1.5 + Math.sin(travel * Math.PI) * 3));
        const scale = (7 + charge * 6 + Math.sin(travel * Math.PI) * 4) * (1 - travel * 0.18);
        const basis = new THREE.Matrix4().makeBasis(right, up, forward);
        hadokenGroup.position.copy(center);
        hadokenGroup.quaternion.setFromRotationMatrix(basis);
        hadokenCore.scale.setScalar(scale);
        hadokenGlow.scale.setScalar(scale * 1.9);
        hadokenRings.forEach((ring, ringIndex) => {
          const ringScale = scale * (1.18 + ringIndex * 0.26 + travel * 0.35);
          ring.scale.set(ringScale, ringScale, ringScale);
          ring.rotation.z = index * 0.12 + ringIndex * Math.PI / 3;
          ring.material.opacity = Math.max(0.16, 0.58 - travel * 0.28 - ringIndex * 0.08);
        });
      }

      function updateFaceMarkers(pose) {
        const up = pose._faceUp || new THREE.Vector3().subVectors(pose.head, pose.neck_01).normalize();
        const right = pose._faceRight || new THREE.Vector3().subVectors(pose.clavicle_l, pose.clavicle_r).normalize();
        const forward = pose._faceForward || new THREE.Vector3().crossVectors(right, up).normalize();
        const center = pose.head.clone()
          .add(up.clone().multiplyScalar(2.1))
          .add(forward.clone().multiplyScalar(6.8));
        const eyeOffset = right.clone().multiplyScalar(2.8);
        const eyeLift = up.clone().multiplyScalar(1.55);
        faceMarkers.eyeL.position.copy(center.clone().add(eyeOffset).add(eyeLift));
        faceMarkers.eyeR.position.copy(center.clone().sub(eyeOffset).add(eyeLift));
        faceMarkers.mouth.position.copy(center.clone().add(up.clone().multiplyScalar(-2.75)));
        const basis = new THREE.Matrix4().makeBasis(right, up, forward);
        faceMarkers.mouth.quaternion.setFromRotationMatrix(basis);
      }

      function updateEnvelopeJoint(item, pose) {
        item.mesh.position.copy(pose[item.name]);
        item.mesh.scale.copy(item.scale);
        if (item.shape === "pelvisCylinder") {
          const right = new THREE.Vector3().subVectors(pose.thigh_l, pose.thigh_r).normalize();
          item.mesh.quaternion.setFromUnitVectors(new THREE.Vector3(0, 1, 0), right);
        }
      }

      function renderPoseFrame(index) {
        const pose = buildPose(index);
        wallGroup.visible = currentMode === "wallPeek";
        wallClimbGroup.visible = currentMode === "wallClimb";
        travelLine.visible = !travelLineHiddenModes.has(currentMode);
        updateSwordProp(pose);
        updateLiftBoxProp(pose, index);
        updateHadokenEffect(pose, index);
        updateBaseballBatProp(pose, index);
        updateSeatedBottleDrinkProps(index);
        updateArcheryProp(pose, index);
        updateTestMotionProps(index);
        for (const item of envelopeMeshes) {
          updateEnvelopeCylinder(item.mesh, pose[item.from], pose[item.to], item.radius);
        }
        for (const item of envelopeJoints) {
          updateEnvelopeJoint(item, pose);
        }
        for (const item of segmentMeshes) {
          const [from, to] = item.segment;
          updateCylinder(item.mesh, pose[from], pose[to]);
        }
        for (const item of jointMeshes) {
          item.mesh.position.copy(pose[item.name]);
        }
        updateFaceMarkers(pose);

        plantedMarkers[0].position.set(pose.foot_l.x, 0.45, pose.foot_l.z);
        plantedMarkers[0].visible = currentMode !== "wallClimb" && pose._leftPlanted;
        plantedMarkers[1].position.set(pose.foot_r.x, 0.45, pose.foot_r.z);
        plantedMarkers[1].visible = currentMode !== "wallClimb" && pose._rightPlanted;
      }

      function updateCamera() {
        const sinPhi = Math.sin(orbit.phi);
        camera.position.set(
          target.x + orbit.radius * sinPhi * Math.sin(orbit.theta),
          target.y + orbit.radius * Math.cos(orbit.phi),
          target.z + orbit.radius * sinPhi * Math.cos(orbit.theta)
        );
        camera.lookAt(target);
      }

      function clearGizmoActive() {
        gizmoButtons.forEach((button) => button.classList.remove("active"));
      }

      function snapCameraToView(viewKey) {
        const direction = viewDirections[viewKey];
        if (!direction) {
          return;
        }
        const normalized = direction.clone().normalize();
        orbit.theta = Math.atan2(normalized.x, normalized.z);
        orbit.phi = Math.acos(THREE.MathUtils.clamp(normalized.y, -0.995, 0.995));
        updateCamera();
        gizmoButtons.forEach((button) => {
          button.classList.toggle("active", button.dataset.view === viewKey);
        });
      }

      function resize() {
        const rect = canvas.getBoundingClientRect();
        const width = Math.max(1, Math.floor(rect.width));
        const height = Math.max(1, Math.floor(rect.height));
        renderer.setSize(width, height, false);
        camera.aspect = width / height;
        camera.updateProjectionMatrix();
      }

      let dragging = false;
      let lastPointer = { x: 0, y: 0 };

      function setWalkMode(mode) {
        const shouldPlayAfterSwitch = timelineControls.playing || timelineControls.ended;
        currentMode = walkModes[mode] ? mode : "basic";
        const modeConfig = walkModes[currentMode];
        frameCount = modeConfig.frameCount;
        frameIndex = 0;
        modeSelect.value = currentMode;
        updateModeDescription();
        target.y = lowTargetModes.has(currentMode) ? lowPoseTargetY : defaultTargetY;
        updateCamera();
        timelineControls.setConfig(frameCount, modeConfig.fps, frameIndex);
        timelineControls.setPlaying(shouldPlayAfterSwitch);
      }

      modeSelect.addEventListener("change", () => {
        setWalkMode(modeSelect.value);
      });

      canvas.addEventListener("pointerdown", (event) => {
        dragging = true;
        canvas.classList.add("dragging");
        canvas.setPointerCapture(event.pointerId);
        lastPointer = { x: event.clientX, y: event.clientY };
      });

      canvas.addEventListener("pointermove", (event) => {
        if (!dragging) {
          return;
        }
        const dx = event.clientX - lastPointer.x;
        const dy = event.clientY - lastPointer.y;
        orbit.theta -= dx * 0.008;
        orbit.phi = Math.min(Math.PI - 0.18, Math.max(0.22, orbit.phi + dy * 0.006));
        lastPointer = { x: event.clientX, y: event.clientY };
        clearGizmoActive();
        updateCamera();
      });

      canvas.addEventListener("pointerup", (event) => {
        dragging = false;
        canvas.classList.remove("dragging");
        if (canvas.hasPointerCapture(event.pointerId)) {
          canvas.releasePointerCapture(event.pointerId);
        }
      });

      canvas.addEventListener("wheel", (event) => {
        event.preventDefault();
        orbit.radius = Math.min(440, Math.max(170, orbit.radius + event.deltaY * 0.24));
        updateCamera();
      }, { passive: false });

      gizmoButtons.forEach((button) => {
        button.addEventListener("click", () => snapCameraToView(button.dataset.view));
      });

      function animate(now) {
        timelineControls.advance(now);
        renderer.render(scene, camera);
        requestAnimationFrame(animate);
      }

      window.addEventListener("resize", resize);
      resize();
      updateCamera();
      setWalkMode("basic");
      requestAnimationFrame(animate);
    })();
