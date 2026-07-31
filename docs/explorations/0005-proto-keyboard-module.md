# 0005 — Proto keyboard module (one octave)

**Status:** decided — immediate build target (V0 learning module)

## Why

The project can't advance without the keyboard, and we're learning **3D printing and CAD at the same time**. So the first physical build is a **small one-octave module** — a self-contained learning vehicle — before committing to the full 2-octave (24-key) keyboard ([0002](0002-key-design.md)). De-risk cheap and small first.

## Scope

- **One octave = 12 keys**: 7 naturals + 5 sharps.
- **Per-key RGB**: 12× SK6812 MINI-E, single data chain (with 3.3→5 V level shifter).
- **Input**: 1× **PCF8575** I2C expander (16 IO, 12 used) — direct-wire, common GND, **no diodes**. *(The user says "multiplexor"; it's really an I2C GPIO expander — see [module doc](../hardware/modules/pcf8575.md).)*
- **All in one module**: switches + LEDs + expander + level shifter on one small assembly, **one JST cable** to the ESP32 (5 V, 3.3 V, GND, SDA, SCL, LED_DATA).
- Same **18 mm pitch**, **KS-33 Silent Brown** switches, and **OP-1 × HiChord** flat caps as the full design — just fewer keys.

## Learning objectives

1. **3D print fit** — plate cutout at **14.0 mm** (housing measured 13.8, clips 14.5), cap stem-socket tolerance, guide-rib fit. Start with a single-key/socket test print.
2. **CAD flow** — OpenSCAD layout + caps + plate, KiCad KS-33 footprint + PCB.
3. **Electrical stack** — PCF8575 read over I2C, SK6812 chain lit, one JST cable end-to-end.

## Relationship to the full keyboard

Everything here scales 1:1 to the 24-key keyboard (just more keys + 2× PCF8575). This module proves the whole vertical slice — switch → plate → cap → LED → expander → firmware — at 1/2 scale.

## Build order (proposed)

1. **Socket-test cap** (OpenSCAD) → print → confirm it clips on a real KS-33 stem; dial the socket tolerance.
2. **One-octave plate** ([`hardware/3d/plate-octave.scad`](../../hardware/3d/plate-octave.scad)) → plate STL.
3. **KS-33 footprint** in KiCad → one-octave PCB (12 switches + 12 SK6812 + PCF8575 + JST).
4. Assemble + firmware smoke test (read 12 keys, light 12 LEDs).
