# Pocket Groovebox — Firmware Architecture

Design decisions for the Phase 3 firmware. Written after the Phase 2 exploration sketches
to capture what we learned and define the right foundation before writing production code.

> **Status:** planning. Nothing here is implemented yet. Decisions marked ✅ are settled;
> those marked ❓ are open questions to resolve before building that layer.

---

## Why a new architecture

The exploration sketches (01–10) were written with Arduino IDE and no shared code. They
taught us the hardware and the problem space well. But they don't scale:

- Audio engine, sequencer, and protocol are tangled together in each sketch
- No shared types — `Key`, `DrumVoice`, `Layer` redefined per file
- Arduino auto-prototype injection breaks on user-defined structs (hit this with sketch 11)
- No way to unit-test audio math without the hardware
- Adding a feature (e.g. a filter to the oscillator) requires touching the inner loop in every sketch

Phase 3 firmware is written from scratch with PlatformIO and a clear layer separation.

---

## Layer model

```
┌─────────────────────────────────────────────────────┐
│  UI layer — display, encoders, key matrix           │  reads input, draws state
├─────────────────────────────────────────────────────┤
│  App layer — sequencer, modes, patch management     │  orchestrates voices + UI
├─────────────────────────────────────────────────────┤
│  Audio engine — voice pool, mixer, effects          │  runs in the I2S callback
├─────────────────────────────────────────────────────┤
│  HAL — I2S, SPI, GPIO, NVS, timers                 │  hardware abstraction
└─────────────────────────────────────────────────────┘
```

Each layer talks only to the layer below it. The UI never touches the I2S buffer; the
audio engine never reads a GPIO pin.

---

## Toolchain ✅

**PlatformIO** replaces Arduino IDE.

- `platformio.ini` at repo root defines the ESP32-S3 target, framework (Arduino or IDF),
  and shared library paths
- `firmware/` holds PlatformIO projects (one per major feature or the combined V1 firmware)
- `lib/` holds shared C++ modules (audio engine, etc.) — imported by all projects
- VS Code + PlatformIO extension is the primary IDE

Reasons: proper dependency management, project-scoped builds, no auto-prototype injection,
unit testing possible via `test/` directory, multi-environment builds (debug / release).

---

## Audio engine

### Voice model ✅

A **voice** is the unit of sound. Each voice has:

```
Oscillator → Filter (SVF) → VCA → out
                ↑               ↑
          [filter env]     [amp env]        ← both are full ADSR
```

This is the same subtractive model described in [theory/synthesis.md](theory/synthesis.md),
but now the envelope and oscillator are concrete, reusable C++ types.

### Oscillator ✅

```cpp
struct Oscillator {
  float phase = 0;
  float inc   = 0;        // 2π × freq / SAMPLE_RATE
  WaveShape shape = SINE; // SINE, SAW, SQUARE, TRIANGLE

  float next();           // returns next sample in -1..1
  void  setFreq(float hz);
  void  setShape(WaveShape s);
};
```

`next()` switches on `shape`; wavetable lookup replaces the `sinf()` call once we measure
CPU headroom. Anti-aliasing (PolyBLEP) added if aliasing is audible on high notes.

### Envelope — ADSR ✅

Exponential decay is the critical improvement over the exploration sketches.
Linear `env -= rate` sounds artificial. Exponential `env *= coeff` matches how all
physical systems decay (strings, drums, room reverb).

```cpp
struct Envelope {
  float attackSecs  = 0.005f;
  float decaySecs   = 0.1f;
  float sustainLevel= 0.7f;
  float releaseSecs = 0.05f;

  void noteOn();
  void noteOff();
  float next();   // call once per sample; returns 0..1
};
```

`next()` runs the state machine: IDLE → ATTACK → DECAY → SUSTAIN → RELEASE → IDLE.
Attack is linear (sounds natural for the rising edge). Decay and release are exponential:
`level *= decayCoeff` where `decayCoeff = expf(-1.0f / (SAMPLE_RATE × decaySecs))`.

### Filter — State-Variable Filter (SVF) ✅

The SVF gives low-pass, high-pass, and band-pass simultaneously from one struct, is
numerically stable, and is easy to modulate (cutoff and resonance change per sample with
no stability risk). It's the right choice for both synth voices and drum noise shaping.

```cpp
struct SVF {
  float cutoff    = 1000.0f;  // Hz
  float resonance = 0.5f;     // 0..1 (0 = none, approach 1 for self-oscillation)

  void  setParams(float cutHz, float q);
  float nextLP(float in);   // low-pass output
  float nextHP(float in);   // high-pass output
  float nextBP(float in);   // band-pass output
  // or: struct Out { float lp, hp, bp; }; Out next(float in);
};
```

The Chamberlin SVF topology is the standard reference. One multiply per sample per mode.

### Voice pool ✅

```cpp
constexpr int MAX_VOICES = 8;

struct Voice {
  Oscillator osc;
  SVF        filter;
  Envelope   ampEnv;
  Envelope   filterEnv;  // optional: sweeps filter cutoff
  bool       active = false;
  int        noteKey = -1;
};

Voice voicePool[MAX_VOICES];

Voice* allocate(int key);   // steal oldest if all busy
void   release(int key);
```

Voice stealing policy: steal the voice with the lowest amplitude (quietest = least audible
interruption). Fall back to oldest-active if all are at similar levels.

### Mixer ✅

Sum all active voices, scale by `1.0f / MAX_VOICES` to prevent clipping when all fire
at once. A soft limiter (tanh or cubic saturation) on the final output before the int16
conversion prevents hard clipping while adding a small amount of pleasant warmth.

```cpp
float mix = 0;
for (auto &v : voicePool) mix += v.next();
mix *= 1.0f / MAX_VOICES;
mix = tanhf(mix * drive) / tanhf(drive);  // soft limiter; drive ≈ 1.5–2.0
int16_t out = (int16_t)(mix * 32767.0f);
```

### Audio callback ✅

The I2S write happens in a tight loop in `loop()` (same as exploration sketches) or in a
FreeRTOS task pinned to core 0. The audio engine is the only thing that runs there.
All state changes from the UI/sequencer come through a lock-free ring buffer or atomic
flags — no mutexes in the audio path.

---

## Sequencer

### Data model ❓ (open question)

Two options:

**Option A — Unified timeline (piano roll model)**
Every event is a `{step, lane, noteOrDrum, velocity}` record. Drum hits and pitched notes
live on the same 32-step grid. Cleaner data model; harder to display.

**Option B — Separate pitched + drum grids (TR-style)**
Pitched voices: N-step pattern per layer (like sketch 10-audio-looper).
Drum voices: 1-bit per drum per step (like sketch 11, the removed drum machine).
Simpler to display and edit on a small screen; two separate data structures.

Decision: resolve when the display layout is designed — the UI constraint drives the
data model choice more than the audio engine does.

### Step resolution ✅

16th notes at the default BPM. Optionally 32nd notes (double resolution). BPM range:
20–300. `stepMs = 60000 / BPM / 4` (same formula as exploration sketches, confirmed stable).

### Live recording ✅

While playing + recording, incoming note-on events are OR'd into the current step.
This matches the looper sketch behavior and feels natural for performance recording.

---

## Drum voices ❓ (open question — decide before Phase 3)

Two approaches, not yet decided:

### Option A — Synthesized (TR-606 style)

Use the audio engine voice primitives (SVF + exponential ADSR + oscillator):

```
Kick  = Oscillator(SINE) + exp_pitch_env + SVF(BP) + exp_amp_env
Snare = 2× Oscillator(SINE, detuned) + SVF(BP, noise) + exp_amp_env
Hat   = 6× Oscillator(SQUARE, inharmonic) + SVF(HP) + short exp_amp_env
```

**Pro:** no flash storage, fully configurable, same code as synth voices.
**Con:** harder to get sounding good; took an entire experiment sketch to discover this.
Reference: Mutable Instruments Peaks source code (open source, used in PO-32 Tonic).

### Option B — Sample playback from internal flash

Bake 8–12 short PCM samples (kick, snare, hat, clap, etc.) as C arrays in flash:

```cpp
// generated from wav files: python wav2c.py kick.wav > kick_sample.h
extern const int16_t KICK_SAMPLE[];
extern const int     KICK_SAMPLE_LEN;
```

A sample voice simply reads through the array at the right playback rate.

**Pro:** sounds like real drums immediately; no DSP complexity.
**Con:** samples are fixed assets; changing a sound requires recompiling.

**Likely decision:** Option B for V1 (get it working and fun fast), Option A for V2
(full synthesis once the audio engine primitives are solid).

---

## UI layer

### Inputs ❓

- Key matrix (13 keys): debounced via timer or state comparison each frame
- Rotary encoders (EC11 × N): interrupt-driven edge count + button debounce
- How many encoders? TBD — depends on final control layout

### Display ✅

ST7789 over SPI via Arduino_GFX. Double-buffered where possible. UI redraws only changed
regions (dirty-rect tracking) to avoid blocking the audio loop.

Display update runs in `loop()` after the audio write, or in a FreeRTOS task on core 1
(audio on core 0, UI on core 1). The I2S callback is the timing constraint — display
must not block it.

### State communication between UI and audio ✅

The audio engine never blocks on UI. Communication is one-directional:
- UI → Audio: atomic flags or a small lock-free ring buffer (`NoteEvent` structs)
- Audio → UI: shared atomic state (current step, active notes bitmask, CPU load counter)

No mutexes in the audio path.

---

## Persistence

NVS (Non-Volatile Storage) via the Arduino `Preferences` library or ESP-IDF NVS API.

- **Settings** (BPM, master volume, active mode) — stored on change
- **Patterns** — stored on user request (hold a button) or on power-off if practical
- **Presets** — named patches for the synth voice parameters

NVS survives power cycles and firmware updates (unless the partition table changes).

---

## Project structure (PlatformIO)

```
pocket-groovebox/
├── platformio.ini          ← build targets: esp32-s3 (debug + release)
├── lib/
│   ├── audio/              ← Oscillator, SVF, Envelope, Voice, Mixer
│   ├── sequencer/          ← Pattern, Step, Transport
│   ├── ui/                 ← Display, Encoder, KeyMatrix
│   └── hal/                ← I2S, NVS, SPI wrappers
├── firmware/
│   ├── groovebox/          ← main V1 firmware (src/main.cpp)
│   └── [exploration]/      ← old .ino sketches kept for reference
├── test/
│   ├── test_envelope/      ← unit tests for Envelope math
│   ├── test_oscillator/    ← unit tests for Oscillator output
│   └── test_svf/           ← unit tests for SVF stability + response
├── app/                    ← web bridges (keyboard-bridge, looper-bridge)
└── docs/
```

`lib/audio` is the most critical — it runs at 44.1 kHz and must be tested independently
of the hardware. PlatformIO's native unit test environment (runs on the host machine, not
the ESP32) makes this possible.

---

## Open questions (resolve in order)

1. **Drum approach** — samples vs synthesis? Resolve first — it affects the voice pool design.
2. **Sequencer data model** — unified timeline vs separate grids? Resolve when display layout is designed.
3. **Number of encoders** — physical control layout TBD.
4. **FreeRTOS or single-core loop?** — single-core loop works if the audio block stays < ~3 ms. Measure first.
5. **Wavetable vs sinf()** — profile sinf() with 8 voices. If it's a bottleneck, switch to a 1024-entry wavetable lookup.
