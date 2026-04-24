# Seated Bottle Drink

- Request: add a generated-image-based motion where Manny sits on a chair, picks up a bottled drink from a table, drinks, and puts it back.
- Display names: `Seated Bottle Drink` / `앉아서 병음료 마시기`
- Type: key-pose based non-looping action with scene props.
- Data file: `web/manny_seated_bottle_drink_keyposes.js`
- Reference image: `imagegen/generated/seated_bottle_drink_reference.png`

## Intent

The motion should read as a small seated interaction scene: Manny is seated on a chair beside a table, reaches for a bottle drink, grips it, lifts it to the mouth, drinks with a slight head tilt, lowers it, returns it to the tabletop, and settles back to seated idle.

## Props

- Chair: fixed seat/back/legs.
- Table: fixed tabletop and legs.
- Bottle: animated prop with amber contents and dark cap.

## Requirements

- Feet stay on the floor.
- Pelvis remains seated instead of standing up.
- Right hand drives the bottle while the left hand rests near the lap.
- Bottle starts and ends on the table.
- Generated and extracted image assets must live under `imagegen/generated/`; preview captures belong under ignored `imagegen/previews/`.
