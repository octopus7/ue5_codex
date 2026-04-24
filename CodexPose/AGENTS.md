# CodexPose Project Instructions

- Unless the user explicitly says otherwise, treat new motion/animation implementation requests as key-pose-based work.
- Key-pose-based motions should keep their motion data in separate web data files instead of embedding full key-pose arrays in `web/index.html`.
- For each new motion/animation request, create a separate Markdown request note under `Docs/MotionRequests/` so the original intent remains recoverable later. Use a stable motion-oriented filename such as `shuffle_dance.md` or `wall_climb.md`.
- Each motion request note should capture the user's original intent, the display names, frame count/fps, style constraints, key pose/contact requirements, props or environment requirements, and the linked implementation/data files.
- Rebuild `web_deploy.zip` after finishing any web-facing change.
