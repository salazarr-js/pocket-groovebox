# CLAUDE.md

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

Two areas of deep research must be completed before any Phase 3 implementation decisions are locked. Both could change the architecture:

1. **Synthesis system design** — what synthesis engines will this device actually have? The current architecture assumes basic subtractive (oscillator → SVF → VCA), but the right answer requires a proper research pass: which paradigms fit the ESP32-S3 CPU budget (subtractive, FM, wavetable, phase distortion?), how many voices, what modulation system (LFOs, envelopes, macro routing), what the patch model looks like. See `docs/research/digital-synths.md` and `docs/research/synthesis.md` as starting points. This research may reshape the audio engine layer.

2. **Input method** — mechanical keys vs. capacitive touch vs. hybrid. See `docs/explorations/0001-input-method.md`. This decision drives pin assignments, physical layout, and the entire keyboard/UI layer design.

**Do not finalize the architecture or begin Phase 3 code until both are resolved.**

**Remaining architecture questions** (after the above are answered):
1. ~~Drum approach~~ ✅ Synthesized for V1; `IVoice` interface keeps sampler path open for V2
2. Sequencer data model: unified timeline vs. separate pitched + drum grids
3. Device modes: Play / Sequencer / Looper / Drums — mode machine and screen layouts not yet designed
4. FreeRTOS dual-core vs. single-core loop (measure audio block latency first)

## Unit testing

Tests live in `firmware/test/`. Framework is Unity (bundled with PlatformIO). The goal is to test `lib/audio` math (Envelope, SVF, Oscillator) on the host without hardware. Add a `[env:native]` block to `platformio.ini` to enable `pio test -e native`.

## Hardware

Target: **Waveshare ESP32-S3-R32N16** (32 MB Octal flash, 16 MB PSRAM). The `platformio.ini` board entry is `esp32-s3-devkitc-1` with overrides for flash/PSRAM — do not change those settings without reading `docs/hardware/modules/esp32-s3-devkit.md`.

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
