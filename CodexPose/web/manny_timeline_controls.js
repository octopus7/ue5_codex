(function (global) {
  "use strict";

  function clamp(value, min, max) {
    return Math.max(min, Math.min(max, value));
  }

  function create(options) {
    const playButton = options.playButton;
    const stopButton = options.stopButton;
    const timelineInput = options.timelineInput;
    const frameReadout = options.frameReadout;
    const strip = options.strip;
    const getText = options.getText;
    const onFrameChange = options.onFrameChange;
    const ticks = [];
    const state = {
      playing: true,
      frameIndex: 0,
      frameCount: 24,
      frameDurationMs: 1000 / 24,
      loop: true,
      ended: false,
      lastFrameTime: performance.now()
    };

    function label(key) {
      return typeof getText === "function" ? getText(key) : key;
    }

    function updatePlayButtonText() {
      playButton.textContent = state.playing ? label("pause") : label("play");
      playButton.setAttribute("aria-pressed", String(state.playing));
    }

    function updateFrameReadout(index) {
      frameReadout.textContent = `${label("frame")} ${index + 1} / ${state.frameCount}`;
    }

    function updateTimeline(index) {
      const maxFrame = Math.max(0, state.frameCount - 1);
      const boundedIndex = clamp(Math.round(index), 0, maxFrame);
      const progress = maxFrame > 0 ? (boundedIndex / maxFrame) * 100 : 0;
      timelineInput.max = String(maxFrame);
      timelineInput.value = String(boundedIndex);
      timelineInput.style.setProperty("--progress", `${progress}%`);
      timelineInput.setAttribute("aria-valuetext", `${label("frame")} ${boundedIndex + 1} / ${state.frameCount}`);
    }

    function updateTicks(index) {
      ticks.forEach((tick, tickIndex) => tick.classList.toggle("active", tickIndex === index));
    }

    function updateUi(index) {
      updatePlayButtonText();
      updateFrameReadout(index);
      updateTimeline(index);
      updateTicks(index);
    }

    function resetClock() {
      state.lastFrameTime = performance.now();
    }

    function setFrame(index, render = true) {
      const maxFrame = Math.max(0, state.frameCount - 1);
      state.ended = false;
      state.frameIndex = clamp(Math.round(Number(index) || 0), 0, maxFrame);
      updateUi(state.frameIndex);
      if (render && typeof onFrameChange === "function") {
        onFrameChange(state.frameIndex);
      }
      resetClock();
      return state.frameIndex;
    }

    function rebuildTicks() {
      strip.textContent = "";
      strip.style.gridTemplateColumns = `repeat(${state.frameCount}, minmax(0, 1fr))`;
      ticks.length = 0;
      for (let index = 0; index < state.frameCount; index += 1) {
        const tick = document.createElement("button");
        tick.type = "button";
        tick.className = "walk-tick";
        tick.title = `${index + 1}`;
        tick.setAttribute("aria-label", `${label("frame")} ${index + 1}`);
        tick.addEventListener("click", () => setFrame(index));
        strip.appendChild(tick);
        ticks.push(tick);
      }
    }

    function setConfig(frameCount, fps, preferredFrameIndex, loop = true) {
      state.frameCount = Math.max(1, Math.round(Number(frameCount) || 1));
      state.frameDurationMs = 1000 / Math.max(1, Number(fps) || 24);
      state.loop = loop !== false;
      rebuildTicks();
      setFrame(preferredFrameIndex ?? state.frameIndex);
    }

    function setPlaying(playing) {
      state.playing = Boolean(playing);
      if (state.playing && !state.loop && state.frameIndex >= state.frameCount - 1) {
        state.frameIndex = 0;
        state.ended = false;
        updateUi(state.frameIndex);
        if (typeof onFrameChange === "function") {
          onFrameChange(state.frameIndex);
        }
      }
      if (state.playing) {
        state.ended = false;
      }
      updatePlayButtonText();
      resetClock();
    }

    function stop() {
      state.ended = false;
      state.playing = false;
      setFrame(0);
    }

    function advance(now) {
      if (!state.playing || now - state.lastFrameTime < state.frameDurationMs) {
        return false;
      }
      const elapsedFrames = Math.floor((now - state.lastFrameTime) / state.frameDurationMs);
      const nextFrame = state.frameIndex + elapsedFrames;
      if (state.loop) {
        state.frameIndex = nextFrame % state.frameCount;
      } else {
        state.frameIndex = Math.min(nextFrame, state.frameCount - 1);
        if (state.frameIndex >= state.frameCount - 1) {
          state.playing = false;
          state.ended = true;
        }
      }
      state.lastFrameTime += elapsedFrames * state.frameDurationMs;
      updateUi(state.frameIndex);
      if (typeof onFrameChange === "function") {
        onFrameChange(state.frameIndex);
      }
      return true;
    }

    function refreshLabels() {
      stopButton.textContent = label("stop");
      updateUi(state.frameIndex);
      ticks.forEach((tick, index) => {
        tick.setAttribute("aria-label", `${label("frame")} ${index + 1}`);
      });
    }

    playButton.addEventListener("click", () => {
      setPlaying(!state.playing);
    });

    stopButton.addEventListener("click", stop);

    timelineInput.addEventListener("input", () => {
      setFrame(timelineInput.value);
    });

    timelineInput.addEventListener("change", () => {
      setFrame(timelineInput.value);
    });

    refreshLabels();

    return {
      advance,
      refreshLabels,
      setConfig,
      setFrame,
      setPlaying,
      stop,
      get playing() {
        return state.playing;
      },
      get frameIndex() {
        return state.frameIndex;
      },
      get frameCount() {
        return state.frameCount;
      },
      get frameDurationMs() {
        return state.frameDurationMs;
      },
      get ended() {
        return state.ended;
      }
    };
  }

  global.MannyTimelineControls = { create };
})(window);
