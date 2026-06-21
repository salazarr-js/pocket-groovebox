# 0001 — Input method

**Status:** decided  ·  (`exploring → leaning toward X → decided`)

## Context
We don't have physical input wired yet. The original plan called for a mechanical keyboard + 2× EC11 encoders,
but nothing is built — notes are currently played from the laptop keyboard over the Web Serial bridge
([app/keyboard-bridge.html](../../app/keyboard-bridge.html)
→ [sketches/09-audio-notes-gate](../../sketches/09-audio-notes-gate/09-audio-notes-gate.ino)). Before
committing to hardware, weigh the input options. Trigger for this: saw the **Elta Music Solar 42**,
a touch-sensitive instrument.

## Options

### A) Mechanical keys
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

**Option A — mechanical keys via PCF8575 I2C I/O expander.**

Native ESP32-S3 touch (Option B) was ruled out: the display and I2S peripherals consume GPIO5–7 and GPIO9–14, leaving only 4–5 native touch channels — not enough for a full keyboard.

A PCF8575 breakout was sourced. It gives 16 GPIO pins over I2C (SDA + SCL), freeing all native GPIOs for encoders and other use. Keys wire directly to P-pins with no matrix or diodes needed. The INT pin enables interrupt-driven key reads. (The initial count was 13 keys; the final design expanded to 25 keys across two PCF8575s — see [Consequences](#consequences) below.)

## Consequences

- **Expanded to 25 keys (2 octaves, C3–C5).** The original 13-key plan grew — one PCF8575 (16 pins) covers C3–D#4; a second PCF8575 at address 0x21 covers E4–C5.
- Key wiring: P-pin → key → GND; key pressed = pin reads 0.
- I2C bus: GPIO4 (SDA) + GPIO8 (SCL). Single 4.7 kΩ pull-up per line, shared by both chips.
- INT: both chips' INT outputs wire directly to GPIO15 (open-drain wired-OR — any keypress on either chip asserts the line low).
- Module doc: [hardware/modules/pcf8575.md](../hardware/modules/pcf8575.md).
- Full pin assignments: [hardware/wiring.md](../hardware/wiring.md#pcf8575--16-bit-i2c-io-expander-keyboard-2-chips).
