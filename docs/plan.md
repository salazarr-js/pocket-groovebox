# Pocket Groovebox — Plan

The working build plan: concrete, checkable steps grouped by phase. For the vision see
[brief.md](brief.md); for component choices see [hardware.md](hardware.md).

Check items off as you go. Each done step — with notes, wiring, and photos — is raw
material for a future build tutorial (see [Toward a tutorial](#toward-a-tutorial)), so
write each step as something another person could follow.

> Built on the **ESP32-S3** for both prototyping and V1 (no separate prototype board).

## Phase 0 — Define project limits

Nail the scope before building anything.

- [ ] Confirm the V1 feature set (synth, loop station, step sequencer)
- [ ] Confirm what's out of scope (advanced effects, massive sampling, DAW features)
- [ ] Finalize the component list (see [hardware.md](hardware.md))
- [ ] Confirm the control-layout direction (handheld, Steam Deck–style)
- [ ] Restate the brief's success signals as the bar for "done"

## Phase 1 — Hardware 🚧 (paused — incomplete, needs work)

Gather, identify, and document every module; plan the wiring. **Provisional** — hardware
selection isn't final; revisit after the board tests in Phase 2.

- [ ] Inventory all parts on hand against hardware.md
- [ ] Toolbox ready (iron, solder, multimeter — see hardware.md Tools)
- [ ] Identify the exact variant of each module; photograph front + back
- [ ] Find and save the pinout for each module
- [ ] Draft the wiring plan (which pins connect where)
- [ ] Start a module doc per part (copy [`modules/module-template.md`](modules/module-template.md))

## Phase 2 — Software install and base tests

Get the dev environment running and verify each peripheral on its own.

- [ ] Install the toolchain (Arduino IDE / PlatformIO / ESP-IDF — pick one)
- [ ] Flash a blink/serial sketch to the ESP32-S3; confirm upload works
- [ ] OLED (SSD1306): show text over I2C
- [ ] Rotary encoder (EC11): read rotation + button
- [ ] Audio amp (MAX98357A): play a test tone over I2S
- [ ] Keyboard: read all 13 keys
- [ ] Power: run from the 18650 cell through the regulator
- [ ] Update each module doc with wiring + gotchas as it passes

## Phase 3 — Core prototype (breadboard / protoboard)

Combine modules into a playable instrument on breadboard, then perfboard/protoboard;
validate interaction and workflow before committing to an enclosure or PCB.

- [ ] Keyboard → audible synth note (key press makes sound)
- [ ] Basic synth voice (waveform + envelope)
- [ ] Encoders control volume and one parameter
- [ ] OLED shows live state (note, BPM, preset)
- [ ] Step sequencer: program and play back a pattern
- [ ] Simple loop station: record and replay a phrase
- [ ] Play through the speaker and the headphone output
- [ ] Lock the final control layout

## Phase 4 — Standalone V1

Turn the breadboard into a self-contained, battery-powered device that runs without a
computer.

- [ ] Design the PCB in KiCad (MCU + charger + amp + OLED + encoders + key matrix)
- [ ] Order the PCB from a fab service (e.g. PCBway or a local provider)
- [ ] Assemble and test the populated board
- [ ] Design the enclosure (3D-printed / laser-cut or CNC acrylic)
- [ ] Assemble the device
- [ ] Battery + USB-C charging working in the enclosure
- [ ] Full power-on-to-play test (boots and plays with no setup)
- [ ] Check against the brief's success signals

Out of scope for V1: advanced effects, massive sampling, complex DAW features.

## Toward a tutorial

A "build your own" guide is a later goal, not a current task. The plan is structured to
feed it: keep notes, wiring tables, and front/back + pinout photos in each module doc as
you work through Phases 1–3. When V1 works, those docs assemble into a step-by-step build
guide — bill of materials, tools, wiring, firmware flashing, assembly.
