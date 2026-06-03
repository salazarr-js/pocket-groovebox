# 0001 — Input method

**Status:** exploring  ·  (`exploring → leaning toward X → decided`)

## Context
We don't have physical input wired yet. The plan ([hardware.md](../hardware/README.md)) calls for a
**13-key mechanical keyboard + 2× EC11 encoders**, but nothing is built — notes are currently
played from the laptop keyboard over the Web Serial bridge ([tools/keyboard-bridge.html](../../tools/keyboard-bridge.html)
→ [src/09-audio-notes-gate](../../src/09-audio-notes-gate/09-audio-notes-gate.ino)). Before
committing to hardware, weigh the input options. Trigger for this: saw the **Elta Music Solar 42**,
a touch-sensitive instrument.

## Options

### A) Mechanical keys (current plan — 13-key matrix)
- **Pros:** real tactile feedback, familiar, robust; velocity *possible* (with extra sensing).
- **Cons:** parts + wiring + space (key matrix, diodes), more BOM, taller enclosure.

### B) Capacitive touch (ESP32-S3 native touch pads) — the Solar 42 approach
- **What:** copper/PCB pads read as touch channels; the ESP32-S3 has capacitive touch **built in**.
- **Pros:** no moving parts, slim/flat, very cheap (just pads), fewer components. The capacitance
  value rises with **finger contact area**, so you can map it to volume/filter for a **cheap
  expression** (pseudo aftertouch) — exactly the Solar 42 trick.
- **Cons:** no tactile feedback, needs calibration, sensitive to humidity/grounding, **no true
  velocity** (only contact area / quasi-pressure), limited number of touch GPIO channels.

### C) Ribbon controller (continuous strip)
- **What:** a resistive/capacitive strip sensing finger **position**.
- **Pros:** expressive glides/slides for pitch or filter.
- **Cons:** awkward for discrete notes — better as a **complement** (pitch/mod) than the main input.

## Reference — Elta Music Solar 42 / 42f
Experimental analogue microtonal ambient/drone instrument (Elta Music). Its keyboard is a
**performance touch controller with 12 capacitive plates**; **not** force-sensitive — it responds
to the **surface area of skin** on the plate (more finger = more output). A clean example of cheap,
playable capacitive expression.
- https://www.eltamusic.com/solar-42
- https://moogaudio.com/products/elta-solar-42f-electro-music-instrument-black
- https://www.perfectcircuit.com/elta-solar-42.html

## Open questions
- Do we want **velocity/expression**, or is on/off enough for V1?
- How many keys/pads, and do the ESP32-S3 touch channels cover that count?
- Tactile feel vs slim form factor — which matters more for this device?
- Could we **mix**: capacitive keys + a ribbon for pitch/filter?

## Decision
Pending. (Leaning: prototype **B (capacitive touch)** cheaply on the ESP32-S3 to feel it out,
since it's low-cost and native — then compare against mechanical keys.)

## Consequences
- If B: a touch-pads test sketch + pad layout; revisit the keyboard line in
  [hardware.md](../hardware/README.md).
- If A: proceed with the key-matrix wiring already implied by the plan.
