# Synthesis — theory & design reference

The conceptual base for the sound sketches. Explains the model we're building toward and how
the code maps to it. Grounded in this project: ESP32-S3 generating samples in software, out
over I2S to the PCM5102 DAC → PAM8403 → speaker (see [hardware.md](../hardware/README.md)).

> **Scope:** theory of the subtractive voice chain we're building. Paradigm survey → [digital-synths.md](digital-synths.md); build spec → `docs/architecture.md`.

> **Phase 3 starting point:** Phase 2 exploration is complete. The architecture in
> `docs/architecture.md` defines the production implementation of everything described here.
> This doc is the theory foundation; `architecture.md` is the build spec.

## 1. The subtractive-synth voice

The classic model: start with a harmonically-rich waveform, then *subtract* with a filter,
and shape the loudness over time. **One voice** is:

```
 Oscillator  ──►  Filter (VCF)  ──►  VCA  ──►  out
 (waveform)         ▲                 ▲
                    │                 │
              [filter ADSR]      [amp ADSR]     ← envelopes are MODULATORS, not audio stages
```

Key idea: the **ADSR envelopes are control signals**, not links in the audio path. The audio
flows `Osc → Filter → VCA`; the **amp envelope** drives the VCA (loudness over time) and an
optional **second envelope** sweeps the **filter cutoff** (the classic synth "wah").

## 2. Oscillator / waveforms

The oscillator outputs a periodic shape. More edges/corners = more harmonics = brighter.

| Wave | Harmonics | Character | Naive gen |
| --- | --- | --- | --- |
| **Sine** | fundamental only | pure, soft (what we have now) | `sinf(phase)` |
| **Triangle** | odd, fall off fast (1/n²) | soft, hollow | ramp up/down |
| **Square** | odd only (1/n) | hollow, "buzzy", retro | `phase < π ? +1 : -1` |
| **Saw** | all (1/n) | bright, brassy | `phase/π − 1` |

We track a **phase** accumulator per oscillator and advance it by `2π·f/SR` each sample
(exactly what the sketches already do for the sine).

> **Aliasing:** naive square/saw have infinite harmonics; above SR/2 they fold back as
> inharmonic "grit", worst on high notes. Fine to start naive (simplest, and a lo-fi groovebox
> can live with some); later use band-limited methods (PolyBLEP, or a wavetable) if it bothers.

## 3. ADSR envelope

Shapes a value (usually amplitude) over the life of a note:

```
 1 ┤      /\
   │     /  \____________        ← Sustain level (held while key down)
   │    /                \
 0 ┤___/                  \___
       A  D       S        R
       │  │                │
  key down            key released
```

- **Attack** — time to rise 0→peak after note-on.
- **Decay** — time to fall from peak to the sustain level.
- **Sustain** — the level held *while the key stays down* (a level, not a time).
- **Release** — time to fall to 0 after note-off.

> **What we have today is just A + R** (rise on note-on, fall on note-off, no decay/sustain
> stage) — enough to avoid clicks, but not expressive. Full ADSR adds the decay→sustain shape
> that gives plucks, pads, stabs, etc.

## 4. Filter

Removes part of the spectrum to shape tone. Most useful here: a **resonant low-pass**.

- **Cutoff** — frequency above which content is attenuated (darker as it lowers).
- **Resonance (Q)** — a boost right at the cutoff; high Q = whistly/squelchy.
- Sweeping cutoff with an envelope or LFO is the signature subtractive-synth motion.

Cheap implementations for an MCU: a **state-variable filter** (gives LP/HP/BP at once, stable,
easy to modulate) or a **biquad**. **Decided:** the SVF is what we're building (see `architecture.md`).
Run per sample, per voice — this is the heaviest part (see §6).

## 5. Polyphony & voice allocation

Polyphony is **not a stage** — it's running N copies of the whole voice chain and summing them:

```
 Voice 1:  Osc→Filter→VCA ┐
 Voice 2:  Osc→Filter→VCA ┤──► Mixer ──► master volume ──► DAC
 Voice N:  Osc→Filter→VCA ┘
                ▲
         voice allocation
```

It adds two new concerns:
- **Mixer** — sum the voices; watch headroom/clipping (more notes = louder; may need scaling
  or a limiter — see [issues/01-audio-issues.md](../issues/01-audio-issues.md) #1).
- **Voice allocation** — map each key-press to a free voice; when all are busy, **steal** one
  (usually the oldest or quietest). Decide the max voice count up front. **Decided:** 8 (`architecture.md`) — with an open tension vs. the HiChord voice-budget analysis in [digital-synths.md](digital-synths.md).

> Today `sketches/09-audio-notes-gate` is a simple form of this: one "voice" slot per key, summed,
> with a hard clamp instead of a real mixer/limiter.

## 6. Cost on the ESP32-S3

Everything runs per sample at 44.1 kHz, so CPU is the budget. Rough order of cost, cheapest
first: phase/osc < envelope < **filter** < many voices. A resonant filter **per voice × N
voices** every sample is the expensive combo. Plan to **measure** how many voices stay
glitch-free, and consider: fewer voices, a lighter filter, one shared filter, or a wavetable
oscillator (table lookup instead of `sinf`/branches). `sinf()` itself isn't free — a wavetable
is worth it once there are several voices.

Build spec: see `docs/architecture.md`.

Companion: [music-theory.md](music-theory.md) (which notes/pitches/chords we play). Hardware path: [hardware/README.md](../hardware/README.md).
