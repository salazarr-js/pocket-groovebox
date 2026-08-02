# CLAUDE.md

## Git workflow

Solo project — **work directly on `main`, no feature branches**. Committing and pushing straight to `main` is pre-approved and considered safe; don't create branches or PRs for changes here.

## Build, flash, and monitor

```sh
pio run                  # build
pio run -t upload        # build + flash
pio device monitor       # serial monitor (115200 baud)
pio run -t erase         # wipe flash (required when partition table changes)
pio run -t uploadfs      # upload LittleFS image (samples, wavetables)
pio test                 # run unit tests on device
pio test -e native       # run unit tests on host (no hardware needed — requires [env:native])
```

If upload fails: hold **BOOT**, tap **RESET**, release **BOOT** to enter download mode.

**Serial monitor port:** `monitor_port` in `platformio.ini` must be the CH343 UART port, not the JTAG port. Run `platformio device list` if the port changes.

**Live serial input** (for sketches that read keystrokes): Arduino Serial Monitor is line-buffered and can't send arrow keys. Use a raw terminal instead:

```sh
screen /dev/cu.usbmodemXXXX 115200   # quit: Ctrl-A then K
```

Close the terminal before flashing — only one process can hold the port.

## Architecture

Docs to read first: `docs/architecture.md` (firmware design), `docs/brief.md` (vision and principles), `docs/hardware/wiring.md` (pin assignments).

**Phase status:** Phase 2 exploration sketches are complete and live in `sketches/`. Phase 3 (production firmware) has not started yet. `firmware/src/main.cpp` is currently only a PlatformIO board validation sketch. The architecture is designed and documented in `docs/architecture.md` but not yet implemented.

**Layer model** (each layer talks only to the one below):

```
UI layer        — display, encoders, key matrix
App layer       — sequencer, modes, patch management
Audio engine    — voice pool, mixer, effects (runs in I2S callback)
HAL             — I2S, SPI, GPIO, NVS wrappers
```

**Audio engine design** (from `docs/architecture.md`):
- Voice: `Oscillator → SVF filter → VCA`, each with independent ADSR envelopes
- Exponential envelopes (`level *= coeff`), not linear — this is a hard requirement; linear was tried and rejected in sketch 11
- State-Variable Filter (SVF) — gives LP/HP/BP from one struct, safe to modulate per-sample
- Voice pool: max 8 voices, steal the quietest when all busy
- Mixer: sum + `1/MAX_VOICES` scale + `tanh` soft limiter before int16 conversion
- UI↔Audio communication: lock-free ring buffer or atomic flags only — **no mutexes in the audio path**

**⛔ Blocked — research required before Phase 3 starts:**

**Synthesis system design** is the one open research area that must be completed before Phase 3 implementation decisions are locked — it could still reshape the audio engine layer:

- What synthesis engines will this device actually have? The current architecture assumes basic subtractive (oscillator → SVF → VCA), but the right answer requires a proper research pass: which paradigms fit the ESP32-S3 CPU budget (subtractive, FM, wavetable, phase distortion?), how many voices, what modulation system (LFOs, envelopes, macro routing), what the patch model looks like. See `docs/research/digital-synths.md` and `docs/research/synthesis.md` as starting points.

**Resolved (no longer blocking):**

- ✅ **Input method** — mechanical keys via 2× PCF8575 I2C expanders. See `docs/explorations/0001-input-method.md`.
- ✅ **Key design (V1)** — **OP-1-style flat rectangular caps directly on KS-33 Silent Brown switches** (no lever bridge — that's deferred to a later iteration). Flattened 2-octave piano, C3–B4 = **24 keys** (14 naturals front row + 10 sharps offset behind — **no top C5**). Geometry source of truth = `hardware/3d/octaves-plan.md` §7: caps in units of **1u = 18mm** (naturals 1u×2u, sharps 1.5u×1u, G# 1u×1u), **pitch parametric = cap + gap, currently 18 + 0.8 = 18.8mm** (2026-08-01, 2×0.4 grid; revisit after build 1.0); front row ≈ 262mm, plate ≈ 58mm deep. Look & feel: **OP-1 × HiChord** (refined minimalism, flat rounded-rectangle caps, muted base + per-key RGB accents, premium tactile — not toy-like). Decision rationale in `docs/explorations/0002-key-design.md` + `docs/explorations/0003-design-system.md`; switch data in `docs/hardware/modules/gateron-ks-33.md`. `enclosure-layout.svg` now draws the **v1.0 one-octave device** (updated 2026-08-01).
- ✅ **v1.0 (next milestone)** — a **complete one-octave device**: 12 keys C–B (`octaves = 1`, strict subset of the final keybed), 1× PCF8575 hand-wired, printed caps + plate, **enclosure** with ST7789 display + 40mm speaker + 2× EC11 encoders + joystick, perfboard main board — **no LEDs** (SK6812 needs a PCB — deferred), no custom PCB, and **no battery** (2026-08-01: powered over the ESP32-S3's own USB exposed through the shell; 18650 + charging deferred). The 2-octave 24-key keybed is the target for a later version. See `hardware/3d/octaves-plan.md` §8. ("v1.0" is a build milestone — distinct from the V1/V2 design-iteration labels.)

**Do not finalize the audio engine or begin Phase 3 code until the synthesis research is resolved.**

**Remaining architecture questions** (after the above are answered):
1. ~~Drum approach~~ ✅ Synthesized for V1; `IVoice` interface keeps sampler path open for V2
2. Sequencer data model: unified timeline vs. separate pitched + drum grids
3. Device modes: Play / Sequencer / Looper / Drums — mode machine and screen layouts not yet designed
4. FreeRTOS dual-core vs. single-core loop (measure audio block latency first)

## Unit testing

Tests live in `firmware/test/`. Framework is Unity (bundled with PlatformIO). The goal is to test `lib/audio` math (Envelope, SVF, Oscillator) on the host without hardware. Add a `[env:native]` block to `platformio.ini` to enable `pio test -e native`.

## Hardware

Target: **Waveshare ESP32-S3 N32R16V** (32 MB Octal flash, 16 MB PSRAM; the `platformio.ini` env is historically named `waveshare-esp32-s3-r32n16`). The `platformio.ini` board entry is `esp32-s3-devkitc-1` with overrides for flash/PSRAM — do not change those settings without reading `docs/hardware/modules/esp32-s3-devkit.md`.

Key pin assignments (already wired and tested):
- I2S DAC (PCM5102): BCK=5, DIN=6, LRCK=7
- SPI display (ST7789 170×320): SCL=9, SDA=10, RES=11, DC=12, CS=13, BLK=14

Audio output chain: ESP32-S3 → I2S → PCM5102 → headphone jack + PAM8403 amp → speaker. The amp's input is the **analog line out** of the PCM5102, not the I2S pins.

## Exploration sketches

`sketches/` contains Arduino IDE `.ino` files from Phase 2. Not production code — read them for reference on how peripherals were driven. The most useful: `09-audio-notes-gate` (note-on/off protocol, polyphony) and `10-audio-looper` (step sequencer with live recording). `app/` contains companion web bridges for those sketches.

## Docs

| File | What it covers |
|---|---|
| `docs/architecture.md` | Firmware spec: layer model, audio engine, open questions |
| `docs/brief.md` | Vision, design principles, inspirations |
| `docs/plan.md` | Phase checklist (what's done vs. next) |
| `docs/hardware/wiring.md` | Pin assignments and wiring rationale |
| `docs/hardware/README.md` | Component selection, system diagrams |
| `docs/development.md` | Toolchain setup, embedding samples, debug flags (tutorial source) |
| `docs/design.md` | Concept renders of the physical device |
| `docs/research/` | Background: synthesis theory, drum machines, music theory, digital synths |
| `docs/explorations/` | Open decisions being weighed (ADR-style, one file per topic) |
| `docs/issues/` | Bug and investigation log |
| `docs/hardware/fdm-tolerances.md` | FDM fits & tolerances reference — validated values (kerf, stem clearance) + rules for inserts, snap-fits, module pockets |
| `docs/hardware/modules/sk6812mini-e.md` | SK6812 MINI-E addressable RGB LED — specs, timing, level shifter requirement, BOM (deferred to a future PCB version) |
| `docs/hardware/modules/gateron-ks-33.md` | Gateron KS-33 Low Profile 2.0 Silent Brown switch — full mechanical dims (15mm body, 14mm plate cutout, 1.2mm plate, MX-cross stem 4.00/1.10/1.23), 3-pin/plate mount, confirmed Gateron specs (55gf tactile), keycap strategy, PCF8575 wiring |
| `docs/hardware/enclosure-layout.svg` | Interior layout diagram — **v1.0 one-octave device, 184×120mm shell** (walls 2.4, 4mm grid) on a 252×204mm page, 2D top view. Bottom band: joystick KY-023 left + 12-key plate 131×56.75 right (caps as `<use>` defs, pitch 18.5 — **⚠️ drawn before the 2026-08-01 pitch move to 18.8 / plate 134×58.4; SVG pending update**); top band: ST7789 62×29 landscape + 2× EC11 + speaker ø40. Dashed = hidden layers (PCF8575 under plate, perfboard bottom shell, USB-C TBD). Authored in **mm units** (1 user unit = 1mm); styling via a `<style>` class block; each module is a `<g id="…">`; dimension annotations **outside** the part; yellow **NOTES** box with spec + legend. Edit module positions by group coords (or drag in Boxy SVG). Geometry source: `hardware/3d/octaves-plan.md` §7–§8 |
| `docs/explorations/0003-design-system.md` | 4mm base grid decision — key proportions, wall thicknesses, reference values. Keyboard pitch 18.8 (cap 18 + gap 0.8, per octaves-plan); cap design values on a finer 0.4mm grid (2026-08-01) |
| `docs/explorations/0004-cad-toolchain.md` | CAD/fab toolchain decision — OpenSCAD (mechanical + layout) + FreeCAD (fallback); KiCad dropped with the no-PCB decision; Ergogen evaluated + dropped; installed tool versions |
| `docs/explorations/0005-proto-keyboard-module.md` | One-octave proto keyboard module ADR — superseded in part by **build 1.0** (`hardware/3d/octaves-plan.md` §8): 12 keys C–B, 1× PCF8575 hand-wired, no LEDs/PCB |
| `hardware/` | Custom hardware — `3d/` (OpenSCAD, **clean rebuild in progress**: v0 was print-validated then deleted on purpose; `3d/octaves-plan.md` = THE rebuild plan (calibrated values, source snapshots, target `key_cap`/`plate`/`assembly` + two-shell enclosure vision); **no custom PCB** — keyboard hand-wired to the PCF8575, main board = perfboard). **Design loop = `/cad` skill** (`.claude/skills/cad/`) — Claude edits + renders, user watches live in OpenSCAD (auto-reload) and approves/requests |
