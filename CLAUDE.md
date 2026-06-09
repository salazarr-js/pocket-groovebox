# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

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

**Open architecture questions** (resolve before Phase 3, in priority order):
1. Drum approach: synthesized (SVF + exponential env) vs. PCM samples baked into flash
2. Sequencer data model: unified timeline vs. separate pitched + drum grids
3. FreeRTOS dual-core vs. single-core loop (measure audio block latency first)

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
| `docs/architecture.md` | Firmware layer model, audio engine design, open questions |
| `docs/brief.md` | Vision, design principles, inspirations |
| `docs/plan.md` | Phase checklist (what's done vs. next) |
| `docs/hardware/wiring.md` | Pin assignments and wiring rationale |
| `docs/development.md` | Toolchain setup, embedding samples, debug flags |
| `docs/theory/` | Synthesis, drums, digital DSP — DSP theory behind the audio engine |
| `docs/brief.md#inspirations` | Design inspirations (devices + videos) that inform the direction |
