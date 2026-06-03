# Pocket Groovebox — Plan

The working build plan: concrete, checkable steps grouped by phase. For the vision see [brief.md](brief.md#vision); for component choices see [hardware.md](hardware/README.md#component-selection).

Check items off as you go. Each done step — with notes, wiring, and photos — is raw material for a future build tutorial (see [Toward a tutorial](#toward-a-tutorial)), so write each step as something another person could follow.

> Built on the **ESP32-S3** for both prototyping and V1 (no separate prototype board).

## Phase 0 — Define project limits

Nail the scope before building anything.

- [ ] Confirm the V1 feature set (synth, loop station, step sequencer)
- [ ] Confirm what's out of scope (advanced effects, massive sampling, DAW features)
- [ ] Finalize the component list (see [hardware.md](hardware/README.md#component-selection))
- [ ] Confirm the control-layout direction (handheld, Steam Deck–style)
- [ ] Restate the brief's success signals as the bar for "done"

## Phase 1 — Hardware 🚧 (paused — incomplete, needs work)

Gather, identify, and document every module; plan the wiring. **Provisional** — hardware selection isn't final; revisit after the board tests in Phase 2.

- [ ] Inventory all parts on hand against hardware.md
- [ ] Toolbox ready (iron, solder, multimeter — see hardware.md Tools)
- [ ] Identify the exact variant of each module; photograph front + back
- [ ] Find and save the pinout for each module
- [ ] Draft the wiring plan (which pins connect where)
- [ ] Start a module doc per part (copy [`modules/module-template.md`](hardware/modules/module-template.md))

## Phase 2 — Software install and base tests

Get the dev environment running and verify each peripheral on its own.

- [x] Install the toolchain — see [development.md](development.md) (Arduino IDE now; PlatformIO later)
- [x] Flash a blink/serial sketch to the ESP32-S3; confirm upload works

**Next — three quick tests to get comfortable with the hardware:**

- [x] **Audio · sound** — play a test tone over I2S (PCM5102) — [`firmware/03-PCM5102A-test`](../firmware/03-PCM5102A-test/03-PCM5102A-test.ino) ✅ verified: 440 Hz tone on headphones
  - [x] _Amp + speaker:_ 440 Hz through the PAM8403 to a small speaker; sweeps amplitude to find a safe (non-distorting) level — [`firmware/08-amp-speaker-test`](../firmware/08-amp-speaker-test/08-amp-speaker-test.ino) ✅ verified (fixed-gain amp overdrives a tiny speaker — keep amplitude low; see [modules/pam8403.md](hardware/modules/pam8403.md))
- [ ] **Audio · notes** — send a letter over the Serial Monitor / Plotter and play the matching note — [`firmware/04-audio-notes`](../firmware/04-audio-notes/04-audio-notes.ino) (one octave mapped to `a w s e d f t g y h u j k`)
  - [ ] _Experiment:_ BLE UART variation — same notes, but the letter arrives from a phone over Bluetooth LE — [`firmware/04-audio-notes-ble`](../firmware/04-audio-notes-ble/04-audio-notes-ble.ino) (S3 is BLE-only; iOS has no USB serial — see [development.md](development.md#input-desktop-serial-or-ble-uart))
  - [ ] _Note-on/off + polyphony:_ real key-down/up gate — a serial terminal can't send key-up, so [`app/keyboard-bridge.html`](../app/keyboard-bridge.html) (Web Serial, piano layout) turns the laptop keyboard's keydown/keyup into a `+a`/`-a` protocol that [`firmware/09-audio-notes-gate`](../firmware/09-audio-notes-gate/09-audio-notes-gate.ino) plays polyphonically (hold a chord, each note releases on its own key-up). ~2.5 octaves across two QWERTY rows (tracker layout, base C3). Holding an arrow gives a momentary, monophonic major/minor triad (extra keys ignored; chord tones highlighted; release collapses to the base note). Volume slider (`V0`..`V9`). Bare keys still work as taps in a plain serial monitor. Open audio/output issues + feature-split plan: [issues/01-audio-issues.md](issues/01-audio-issues.md).
- [x] **Display · play** — drive the ST7789 over SPI; draw text, shapes, and live values to get a feel for it — [`firmware/06-display-hello`](../firmware/06-display-hello/06-display-hello.ino) (Arduino_GFX) ✅ verified
  - [x] _Experiment:_ chord player — serial keys play single/maj/min on the DAC + show the chord on the ST7789 (landscape) — [`firmware/07-chord-player`](../firmware/07-chord-player/07-chord-player.ino) ✅ verified (with the serial hold/release limitation)

**Remaining peripheral checks:**

- [ ] Rotary encoder (EC11): read rotation + button
- [ ] Keyboard: read all 13 keys
- [ ] Power: run from the 18650 cell through the regulator
- [ ] Update each module doc with wiring + gotchas as it passes

## Checkpoint — Re-evaluate before serious development

After the tests above, pause and reassess before committing to deeper work.

- [ ] Re-evaluate the hardware selection against what the tests revealed — confirm or swap modules (see [hardware.md](hardware/README.md#component-selection))
- [ ] Revisit this plan and the scope; reshape the phases for "serious" development
- [ ] Decide the toolchain move (Arduino IDE → PlatformIO) now that firmware is growing — see [development.md](development.md#future-platformio)

## Phase 3 — Core prototype (breadboard / protoboard)

Combine modules into a playable instrument on breadboard, then perfboard/protoboard; validate interaction and workflow before committing to an enclosure or PCB.

- [ ] Keyboard → audible synth note (key press makes sound)
- [ ] Basic synth voice (waveform + envelope) — theory & build path in [synthesis.md](theory/synthesis.md)
- [ ] Encoders control volume and one parameter
- [ ] Display (ST7789) shows live state (note, BPM, preset)
- [ ] Step sequencer: program and play back a pattern — `firmware/10-audio-looper` ⬅ next test (4 layers × 16 steps, play/stop, layer cycling)
- [ ] Simple loop station: record and replay a phrase — same sketch (record notes per step, playback loops, layers stack)
- [x] Play through the speaker and the headphone output ✅ 440 Hz via PAM8403 → small speaker works (keep `AMPLITUDE` low — fixed-gain amp overdrives a tiny speaker)
- [ ] Lock the final control layout

## Phase 4 — Standalone V1

Turn the breadboard into a self-contained, battery-powered device that runs without a computer.

- [ ] Design the PCB in KiCad (MCU + charger + amp + display + encoders + key matrix)
- [ ] Order the PCB from a fab service (e.g. PCBway or a local provider)
- [ ] Assemble and test the populated board
- [ ] Design the enclosure (3D-printed / laser-cut or CNC acrylic)
- [ ] Assemble the device
- [ ] Battery + USB-C charging working in the enclosure
- [ ] Full power-on-to-play test (boots and plays with no setup)
- [ ] Check against the brief's success signals

Out of scope for V1: advanced effects, massive sampling, complex DAW features.

## Toward a tutorial

A "build your own" guide is a later goal, not a current task. The plan is structured to feed it: keep notes, wiring tables, and front/back + pinout photos in each module doc as you work through Phases 1–3. When V1 works, those docs assemble into a step-by-step build guide — bill of materials, tools, wiring, firmware flashing, assembly.
