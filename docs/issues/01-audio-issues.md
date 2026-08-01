# 01 — Audio output issues & backlog

Captured while bringing up the playable keyboard sketch ([`sketches/09-audio-notes-gate`](../../sketches/09-audio-notes-gate/09-audio-notes-gate.ino)). The sketch is kept **simple on purpose** — note on/off, polyphony, momentary maj/min chords, volume. No tone-shaping (filters/limiter) lives in it. The items below are real but each belongs to a *separate* fix (a future DSP sketch, or hardware), not this one.

## Known issues

### 1. Low chords sound "bloated" on the speaker
- **Status:** ✅ absorbed into `architecture.md` — the mixer spec (1/MAX_VOICES scaling + tanh soft limiter) covers polyphony level, and the ~150–200 Hz speaker high-pass was migrated there too.
- **Cause:** the 30 mm 4 Ω speaker can't reproduce deep bass; low notes (esp. several at once) over-excurt the cone → flabby/overflowing sound. Confirmed it's **physical**: the same chords are clean on headphones off the PCM5102.
- **Fix later (software, separate DSP sketch):** high-pass the mix (~150–200 Hz, the speaker barely outputs below that anyway), and/or scale amplitude with polyphony so stacking notes doesn't inflate level. A 2-pole high-pass was prototyped and removed to keep this sketch simple.
- **Fix later (hardware):** a bigger speaker / one with an enclosure (see hardware eval below).

### 2. Speaker hisses when nothing is playing
- **Status:** 🔧 pending (hardware, unowned).
- **Cause:** the PAM8403 (QA03) is a cheap BTL class-D amp with an always-present noise floor; USB 5 V is noisy and class-D barely rejects it. See [modules/pam8403.md](../hardware/modules/pam8403.md).
- **Fix later (hardware):** **100 µF + 0.1 µF decoupling cap** across the amp `5V`/`GND` (most effective; already on the shopping list), short input wires away from I2S/USB. Eventually an amp with a **shutdown pin** to mute it when idle.

### 3. Headphones are much quieter than the speaker
- **Status:** 🔧 pending (hardware, unowned).
- **Cause:** the headphone jack gets the **raw line level** from the PCM5102, while the speaker gets that **+ the amp's ~24 dB gain**. `masterAmp` is tuned low (~320) for the loud amplified speaker, so on headphones it's faint. Two different gain stages, one digital level.
- **Consolidating both (options, for a later iteration):**
  - **Software:** detect headphone insertion (needs a jack with a detect switch wired to a GPIO) and bump the digital level when plugged; keep separate "speaker level" vs "headphone level" presets.
  - **Hardware:** a small headphone amp on the line-out, or accept that headphone listening just needs the volume turned up (the slider already allows it).

### 4. Speaker keeps playing when headphones are plugged in
- **Status:** 🔧 pending (hardware, unowned).
- **Cause:** the PCM5102 module's 3.5 mm jack does **not** cut the line-out feeding the amp, and we don't detect insertion, so both sound at once.
- **Fix later (hardware):** use a headphone jack with a **switched (detect) contact** → either break the amp input mechanically, or wire the switch to a GPIO and **mute the amp** (needs an amp with a shutdown pin, or mute via the DAC's XSMT / a software gate on the speaker path). Pure-software muting isn't possible without that insertion signal.

### 5. Physical master volume
- **Want:** a real knob instead of only the on-screen/`V0..9` digital volume.
- **Fix later:** the plan already includes **2× EC11 encoders** ([hardware.md](../hardware/README.md)) — assign one to master volume in firmware (read encoder → set `masterAmp`). Alternatively a real potentiometer, but a pot on the line-out attenuates before the amp and adds noise; an encoder driving the digital level is cleaner. (A physical knob also lets headphone vs speaker levels be matched live, helping issue #3.)

## Hardware eval — better amp / speaker?

> **Status:** 🔧 pending (hardware, unowned).

- **Speaker is the bigger limitation.** The 30 mm driver is the main reason low chords bloat and there's no real bass. A larger driver (≈40–50 mm) or a small full-range **with an enclosure/baffle** would improve low-mid handling and reduce the bloat far more than any software filter. Highest-impact upgrade for sound quality.
- **Amp is acceptable.** The PAM8403 delivers enough power (2×3 W); its weak point is the **idle hiss** (issue #2) — mostly fixed by the decoupling cap. If we respin: consider an amp with a **shutdown/mute pin** (e.g. PAM8302A mono) so issues #2 and #4 get a clean hardware mute, or the **MAX98357A** (I2S amp, lower noise — but it's speaker-only, so headphones would still come off the PCM5102).
- **Verdict:** keep the PAM8403 for now (+ decoupling cap + later a shutdown-capable amp); **prioritize a better/larger speaker** for the next hardware order.

> Historical note: a proposal to split sketch 09 into focused sketches was dropped when Phase 2 closed.
