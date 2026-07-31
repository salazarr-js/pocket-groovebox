# Pocket Groovebox — Plan

The working build plan: concrete, checkable steps grouped by phase. For the vision see [brief.md](brief.md#vision); for component choices see [hardware.md](hardware/README.md#component-selection).

Check items off as you go. Each done step — with notes, wiring, and photos — is raw material for a future build tutorial (see [Toward a tutorial](#toward-a-tutorial)), so write each step as something another person could follow.

> Built on the **ESP32-S3** for both prototyping and V1 (no separate prototype board).

---

## Phase 0 — Define project limits

Nail the scope before building anything.

- [ ] Confirm the V1 feature set (synth, loop station, step sequencer)
- [ ] Confirm what's out of scope (advanced effects, massive sampling, DAW features)
- [ ] Finalize the component list (see [hardware.md](hardware/README.md#component-selection))
- [ ] Confirm the control-layout direction (handheld, Steam Deck–style)
- [ ] Restate the brief's success signals as the bar for "done"

---

## Phase 1 — Hardware 🚧 (in progress)

Gather, identify, and document every module; plan the wiring.

- [x] Inventory most parts on hand against hardware.md
- [ ] Toolbox ready (iron, solder, multimeter — see hardware.md Tools)
- [ ] Identify the exact variant of each module; photograph front + back
- [x] Find and save the pinout for each module
- [x] Draft the wiring plan (which pins connect where) — see [wiring.md](hardware/wiring.md)
- [x] Module docs created: ESP32-S3, PCM5102, PAM8403, ST7789, PCF8575, SK6812 MINI-E, speaker, template
- [x] Input method decision: **mechanical keys** ([0002-key-design.md](explorations/0002-key-design.md))
- [x] Keyboard mechanism (V1): **OP-1-style flat rectangular caps directly on KS-33 Silent Brown switches** (lever bridge deferred to a later iteration — see [0002](explorations/0002-key-design.md))
- [x] Speaker upgrade: **40 mm full-range neodymium** (on order)
- [x] Switch mechanical spec confirmed (KS-33 dims + vernier: cutout **14.0mm**, MX-cross stem) — [module doc](hardware/modules/gateron-ks-33.md)
- [x] Keyboard layout finalized (24 keys) — shared source at [`hardware/3d/octave-layout.scad`](../hardware/3d/octave-layout.scad)
- [x] **CAD/fab toolchain decided** — OpenSCAD (mechanical + layout) + KiCad (PCB); FreeCAD fallback; tools installed — [0004](explorations/0004-cad-toolchain.md)
- [ ] **Build the one-octave proto module** (12 keys + SK6812 + PCF8575, all in one) — [0005](explorations/0005-proto-keyboard-module.md). First learning build.
  - [x] **Plate + cap CAD (OpenSCAD)** — stepped plate, hollow keycap, full-octave assembly — in [`hardware/3d/`](../hardware/3d/README.md)
  - [x] **DEEP MEASUREMENT INVESTIGATION** — web research (Cherry spec, KeyV2, riskable, clp-keycaps) + **sliced the official STEP in FreeCAD**. Confirmed the mount is **standard MX**: cross **4.00 × 1.11**, collar **ø5.68**, mount tower **2.95 mm** (housing/boss shape is irrelevant — the cap only engages the cross + circle). Cap socket updated in `cap.scad`: `socket_depth` 3.6→**2.8** (low-profile), `cross_arm_width` 1.1 + `stem_clearance` (FDM 0.18 / resin 0.05, on arm width only), added `mouth_flare` lead-in + optional `split_post`. See [gateron-ks-33.md](hardware/modules/gateron-ks-33.md).
  - [ ] Finalise open cap/plate questions: **dish** (revisit or keep flat), **sharp offset** illusion, **anti-tilt** for tall naturals, **pitch** (19 vs 19.5)
  - [x] **Test-fit kit printed → PASSED** (PLA + PETG, 0.2 & 0.4 mm nozzles, 3 print services): KS-33 clip + cap socket confirmed with `kerf 0` / `stem_clearance 0.18`. 0.2 mm nozzle = best finish (spec for final parts). One service shipped elephant foot → design bottom-edge chamfers in. Details in [`hardware/3d/README.md`](../hardware/3d/README.md); test files deleted after passing.
  - [ ] Propagate the final pitch to the docs (still say 18mm pitch)
  - [ ] KS-33 custom footprint (KiCad) → one-octave PCB (+ SK6812 + JST)
  - [ ] Assemble + firmware smoke test (read 12 keys, light 12 LEDs)

---

## Phase 2 — Software install and base tests

Get the dev environment running and verify each peripheral on its own.

- [x] Install the toolchain — see [development.md](development.md) (Arduino IDE now; PlatformIO next)
- [x] Flash a blink/serial sketch to the ESP32-S3; confirm upload works

**Three quick tests to get comfortable with the hardware:**

- [x] **Audio · sound** — play a test tone over I2S (PCM5102) — [`sketches/03-PCM5102A-test`](../sketches/03-PCM5102A-test/03-PCM5102A-test.ino) ✅ 440 Hz tone on headphones
  - [x] _Amp + speaker:_ 440 Hz through the PAM8403 to a small speaker — [`sketches/08-amp-speaker-test`](../sketches/08-amp-speaker-test/08-amp-speaker-test.ino) ✅ (fixed-gain amp overdrives a tiny speaker — keep amplitude low; see [modules/pam8403.md](hardware/modules/pam8403.md))
- [ ] **Audio · notes** — send a letter over Serial Monitor and play the matching note — [`sketches/04-audio-notes`](../sketches/04-audio-notes/04-audio-notes.ino)
  - [ ] _Experiment:_ BLE UART variation — [`sketches/04-audio-notes-ble`](../sketches/04-audio-notes-ble/04-audio-notes-ble.ino)
  - [ ] _Note-on/off + polyphony:_ [`sketches/09-audio-notes-gate`](../sketches/09-audio-notes-gate/09-audio-notes-gate.ino) via [`app/keyboard-bridge.html`](../app/keyboard-bridge.html). Open audio issues: [issues/01-audio-issues.md](issues/01-audio-issues.md).
- [x] **Display · play** — drive the ST7789 over SPI — [`sketches/06-display-hello`](../sketches/06-display-hello/06-display-hello.ino) ✅
  - [x] _Experiment:_ chord player — serial keys + chord display on ST7789 — [`sketches/07-chord-player`](../sketches/07-chord-player/07-chord-player.ino) ✅

**Remaining peripheral checks:**

- [ ] Rotary encoder (EC11): read rotation + button
- [x] Joystick (KY-023): read both axes + button — [`sketches/12-ky023-joystick-test`](../sketches/12-ky023-joystick-test/12-ky023-joystick-test.ino) ✅ centered deltas + single `JOY_MOUNT` orientation knob; see [modules/ky023.md](hardware/modules/ky023.md)
- [ ] Keyboard: read all 25 keys
- [ ] Power: run from the 18650 cell through the regulator
- [ ] Update each module doc with wiring + gotchas as it passes

---

## Checkpoint — Toolchain + Architecture + Research (⬅ current)

Pause here. The exploration sketches have taught us enough to plan seriously. Two things to
do before writing more firmware.

### 1. Migrate to PlatformIO

Arduino IDE served the exploration phase but doesn't scale:
- No dependency management — libraries installed globally, version conflicts are silent
- No project-level build config — every sketch reimports everything manually
- Poor C++ support — auto-prototype injection causes real bugs (hit this with the drum sketch)
- No unit testing, no CI, no multi-target builds

**Tasks:**
- [ ] Install PlatformIO CLI + VS Code extension
- [ ] Create `platformio.ini` at the repo root with the ESP32-S3 target and shared lib paths
- [ ] Migrate the two "keeper" sketches (09-audio-notes-gate, 10-audio-looper) as PlatformIO projects under `firmware/`
- [ ] Confirm upload + serial monitor work from VS Code
- [ ] Update [development.md](development.md) with the new workflow

### 2. Design the firmware architecture

Before writing Phase 3 code, define the system. The exploration taught us what we need;
now build it the right way: modular, configurable, maintainable.

See the architecture design document: [architecture.md](architecture.md)

> **⛔ Blocked on research before any Phase 3 work begins:**
> - **Synthesis system** — deep research into which engines (subtractive, FM, wavetable…), voice budget, modulation model, and patch design. May reshape the audio engine layer. Starting points: `docs/research/digital-synths.md`, `docs/research/synthesis.md`.
> - **Input method** — mechanical keys vs. capacitive touch. See `docs/explorations/0001-input-method.md`. Drives pin assignments and the entire UI layer.

**Key decisions to finalize:**
- [ ] Firmware structure: how state flows (state machine? task-based? event queue?)
- [ ] Audio engine abstraction: voice pool, mixing, effects chain — how voices are allocated and freed
- [ ] Envelope model: exponential ADSR + per-voice params, not hardcoded rates
- [ ] Waveform model: oscillator abstraction that supports sine / saw / square / triangle + wavetable
- [ ] Filter: state-variable filter (SVF) — gives LP, HP, BP from one struct; the right choice for both synth and drums
- [ ] Sequencer: data model for patterns (pitched notes + drum hits on the same timeline or separate?)
- [ ] UI layer: display + encoder + key matrix decoupled from audio — how do they communicate?
- [ ] Persistence: NVS (Non-Volatile Storage) for settings, patterns, presets
- [x] Drum approach: synthesized (SVF + exp env) for V1; polymorphic `IVoice` interface keeps sampler path open for V2

---

## Milestone v0.2 — Consolidated hardware + test UI

All hardware in one tidy place; first navigable UI without a laptop.

### Hardware

- [ ] **Wiring diagram** — draw the full circuit in a dedicated tool before committing anything to solder
  - Investigate which tool to use: **Fritzing** (maker-friendly, has breadboard view), **KiCad Schematic** (open source, same tool we'll use for the PCB in Phase 4), **EasyEDA** (online, integrated with JLCPCB), **Wokwi** (online ESP32 simulator with virtual breadboard view)
  - Tentative recommendation: start with Fritzing for the breadboard view; migrate to KiCad when Phase 4 (PCB) begins
- [ ] **Consolidated protoboard** — all modules wired on a single perfboard with the ESP32-S3 seated on the protoboard; connectors / jumpers so each module can be plugged in and out cleanly
  - Includes: PCF8575 (keyboard), both EC11 encoders + buttons (GPIO 39/40/16, 41/42/17), KY-023 joystick, ST7789 display, PCM5102 DAC, PAM8403 amp, SK6812 LEDs
  - Encoders are the only peripheral not yet wired and verified

### Software (sketch 16 or similar)

- [ ] **Test menu sketch** — first navigable UI without a laptop; defines the interaction patterns the real firmware will use
  - Encoders: navigate options (turn = move cursor, click = select / back)
  - Joystick: move between sections / submenus
  - Display: two-level menu hierarchy; lock in font size, colors (TEAL on black scheme already established)
  - Test options to explore: select note, select chord, adjust volume, select mode (Play / Sequencer)
  - Audio: selected note/chord plays on confirm → immediate feedback

---

## Phase 3 — Core prototype (breadboard / protoboard)

Build from the architecture plan. Each item here depends on the architecture decisions above.

- [ ] **PlatformIO project structure in place** (from Checkpoint)
- [ ] **Audio engine v1** — voice pool, exponential ADSR, sine oscillator, SVF filter (LP mode)
- [ ] **Synth voice** — full waveform + envelope + filter from the architecture; playable via keyboard matrix
- [ ] **Encoders** — volume and one live parameter (e.g. filter cutoff)
- [ ] **Display** — live state (note, BPM, mode, parameter values)
- [ ] **Step sequencer** — pitched notes + drums on the same pattern timeline
- [ ] **Drum voices** — synthesized (TR-606 style; see [research/drums.md](research/drums.md)); SVF + exp env recipes per drum type
- [ ] **Loop station** — live record, layer playback (evolve from `firmware/10-audio-looper`)
- [ ] **Speaker + headphone** — both outputs working ✅ (already verified in Phase 2)
- [ ] **Control layout** — lock the final physical layout

---

## Phase 4 — Standalone V1

Turn the breadboard prototype into a self-contained battery-powered device.

- [ ] Design the PCB in KiCad (MCU + charger + amp + display + encoders + key matrix)
- [ ] Order the PCB from a fab service (e.g. PCBway or a local provider)
- [ ] Assemble and test the populated board
- [ ] Design the enclosure (3D-printed / laser-cut or CNC acrylic)
- [ ] Assemble the device
- [ ] Battery + USB-C charging working in the enclosure
- [ ] Full power-on-to-play test (boots and plays with no setup)
- [ ] Check against the brief's success signals

Out of scope for V1: advanced effects, massive sampling, complex DAW features.

---

## Exploration log

Sketches written during Phase 2 exploration. Not production code — each was a learning exercise.

| Sketch | What it taught | Status |
| --- | --- | --- |
| 01-hello-esp | Upload works, serial works | Done |
| 02-led-serial | GPIO, serial input | Done |
| 03-PCM5102A-test | I2S DAC wiring + basic tone | Done ✅ |
| 04-audio-notes | Pitch table, simple tone mapping | Done |
| 04-audio-notes-ble | BLE UART — S3 BLE-only, iOS has no USB serial | Done |
| 05-audio-pitch-wave | Waveform switching over serial | Done |
| 06-display-hello | ST7789 + Arduino_GFX | Done ✅ |
| 07-chord-player | Chord logic + display + I2S | Done ✅ |
| 08-amp-speaker-test | PAM8403 wiring + safe amplitude | Done ✅ |
| 09-audio-notes-gate | Note-on/off protocol, polyphony, envelope | Done |
| 10-audio-looper | Step sequencer architecture, live recording | Done |
| 11-drum-machine | Synthesized drums — removed; linear decay is wrong, need exponential env + SVF; see [research/drums.md](research/drums.md) | Removed |
| 12-ky023-joystick-test | KY-023 read + NVS calibration + single-knob mount orientation; see [hardware/modules/ky023.md](hardware/modules/ky023.md) | Done ✅ |
| 13-pcf8575-keyboard-test | PCF8575 I2C read, 12-key physical keyboard (inverted wiring), I2C scan, debounce | Done ✅ |
| 14-pcf8575-display | PCF8575 keyboard → ST7789 display; large centered note name, idle dot, landscape 90° | Done ✅ |
| 15-hardware-chord-player | Full integration: PCF8575 + KY-023 + ST7789 + I2S; major/minor chords via joystick, 8-level log volume scale, click-free crossfade, layer debounce | Done ✅ |

---

## Toward a tutorial

A "build your own" guide is a later goal, not a current task. The plan is structured to feed it: keep notes, wiring tables, and front/back + pinout photos in each module doc as you work through the phases. When V1 works, those docs assemble into a step-by-step build guide — bill of materials, tools, wiring, firmware flashing, assembly.
