# Pink Survivor

Pink Survivor is a UE 5.5 project that reimagines the Vampire Survivors loop in a fully 3D environment. Combat pacing and growth structure stay familiar while systems are extended to support third-person play.

## Implemented Features

- **Experience Component**
  - `UPSVExperienceComponent` encapsulates run-based experience, level, and threshold logic.
  - Thresholds are fully data-driven via `ExperienceThresholds`, allowing Blueprint tuning per level.
  - Level progression events are exposed through `OnExperienceChanged` and `OnLevelUp`.

- **Experience Gem Pickup**
  - `APSVExperienceGem` drops from defeated enemies and awards the configured amount of XP when collected.
  - Gems are designed for Blueprint tier variants so visuals and values are easy to author.

- **Persistent Gold Economy**
  - `UPSVGameInstance` + `UPSVSaveGame` load and store long-term gold across sessions.
  - `APSVGoldCoin` is a low-probability drop that adds gold through the game instance before saving.
  - Player HUD receives `OnPersistentGoldChanged` callbacks to reflect the saved balance.

- **Player & Enemy Integration**
  - The player character owns both health and experience components and fans out UI updates through the HUD.
  - Enemies spawn experience gems every time and gold coins based on configurable drop chances.

## Next Steps

1. Author Blueprint derivatives of `APSVExperienceGem` and `APSVGoldCoin` with final art, FX, and tuned values.
2. Implement HUD reactions for `OnExperienceChanged`, `OnLevelUp`, and `OnPersistentGoldChanged` to display meters and totals.
3. Balance gold drop chances per enemy archetype and playtest persistent progression loops in PIE or standalone builds.

