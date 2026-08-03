# 0005 — Proto keyboard module (one octave)

**Status:** superseded in part → **build 1.0** (revised 2026-07-31)

> **Revision (2026-07-31) — this module is now build 1.0.** The print-fit and CAD-flow objectives below are **done** — the fit-test kit validated printing and the OpenSCAD loop (see [`hardware/3d/README.md`](../../hardware/3d/README.md)). The remaining electrical one-octave module is now **build 1.0**, the first integration build: **12 keys (C–B), 1× PCF8575, hand-wired, no LEDs, no PCB** — see [`hardware/3d/keyboard/octaves-plan.md`](../../hardware/3d/keyboard/octaves-plan.md) §8. The per-key SK6812 plan below assumed a custom PCB, which was **revoked** (no custom PCB for V1); LEDs are deferred to a possible future LED/PCB version, and the KiCad footprint/PCB steps are obsolete. Original text kept below for the reasoning, with inline notes.

## Why

The project can't advance without the keyboard, and we're learning **3D printing and CAD at the same time**. So the first physical build is a **small one-octave module** — a self-contained learning vehicle — before committing to the full 2-octave (24-key) keyboard ([0002](0002-key-design.md)). De-risk cheap and small first.

## Scope

- **One octave = 12 keys, C to B**: 7 naturals + 5 sharps.
- ~~**Per-key RGB**: 12× SK6812 MINI-E, single data chain (with 3.3→5 V level shifter).~~ **Dropped for build 1.0** — the LED plan assumed a custom PCB (revoked); deferred to a possible LED/PCB version.
- **Input**: 1× **PCF8575** I2C expander (16 IO, 12 used, 4 spare) — direct-wire, common GND, **no diodes**. *(The user says "multiplexor"; it's really an I2C GPIO expander — see [module doc](../hardware/modules/pcf8575.md).)*
- **All in one module**: switches + expander **hand-wired** on the printed plate (the PCF8575 module mounts into the plate — see [`octaves-plan.md`](../../hardware/3d/keyboard/octaves-plan.md) §4), one cable bundle to the ESP32 (3.3 V, GND, SDA, SCL, INT). *(The original JST pinout included 5 V + LED_DATA for the SK6812 chain — dropped with the LEDs.)*
- Same **parametric pitch** (cap 18 mm + gap 0.8 mm = **18.8 mm** (rev. 2026-08-01)), **KS-33 Silent Brown** switches, and **OP-1 × HiChord** flat caps as the full design — just fewer keys.

## Learning objectives

1. ✅ **3D print fit** — plate cutout at **14.0 mm**, cap stem-socket tolerance — **validated** by the fit-test kit (`kerf 0`, `stem_clearance 0.18` — see [`hardware/3d/README.md`](../../hardware/3d/README.md)).
2. ✅ **CAD flow** — OpenSCAD caps + plate loop **proven** (`/cad` skill). *(The KiCad KS-33 footprint + PCB objective is obsolete — no custom PCB.)*
3. **Electrical stack** — PCF8575 read over I2C, hand-wired switch-to-expander loom end-to-end. **This is build 1.0's remaining objective.** *(SK6812 chain dropped with the PCB.)*

## Relationship to the full keyboard

Everything here scales 1:1 to the 24-key keyboard, C3–B4 (just more keys + a second PCF8575: 16 + 8 pins, 8 spares). This module proves the whole vertical slice — switch → plate → cap → expander → firmware — at 1/2 scale. The C–B octave is a strict subset of the final keybed (see [`octaves-plan.md`](../../hardware/3d/keyboard/octaves-plan.md) §8).

## Build order (build 1.0)

1. ✅ **Socket-test cap** (OpenSCAD) → printed → clips on a real KS-33 stem; socket tolerance dialed (fit-test kit).
2. **One-octave plate** — rebuild `plate.scad` at `octaves = 1` per [`octaves-plan.md`](../../hardware/3d/keyboard/octaves-plan.md) *(v0's `plate-octave.scad` was deleted on purpose)* → plate STL + 12 caps.
3. **Hand-wire** the 12 switches to the PCF8575 module (common GND, no diodes; PCF mounts into the plate). *(Replaces the original KiCad footprint + PCB step — no custom PCB.)*
4. Assemble + firmware smoke test (read 12 keys over I2C).
