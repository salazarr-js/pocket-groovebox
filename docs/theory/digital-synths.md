# Digital Synthesizers — research & learning resources

A reference list of synths, techniques, and learning material to study before designing
the audio engine. Grounded in what matters for this project: an embedded synth running on
an ESP32-S3 with limited CPU and no OS.

---

## Synthesis paradigms overview

Most digital synthesizers are built on one of a few core ideas. Understanding the
differences shapes what the audio engine needs to support.

| Paradigm | Core idea | CPU cost | Character | Example |
| --- | --- | --- | --- | --- |
| **Subtractive** | Harmonically rich wave → filter → VCA | Low | Warm, classic analog feel | Moog, ARP, Roland synths |
| **Wavetable** | Cycle through a table of pre-computed waveform frames | Low | Clean, modern, morphable | Serum, Waldorf Wave |
| **FM** | One oscillator modulates another's frequency | Low (just math) | Bright, metallic, bell-like | Yamaha DX7, FM8 |
| **Phase Distortion** | Distort playback position through a waveform table | Low | Similar to FM, different control | Casio CZ series, DigitsVst |
| **Granular** | Slice audio into tiny grains, reassemble | High | Textural, evolving, "glitchy" | Granulator, Borderlands |
| **Spectral** | Work in frequency domain (FFT) | Very high | Resynthesis, extreme morphing | Harmor, Serum 2 Spectral |
| **Physical modeling** | Simulate the physics of an instrument | Medium–High | Realistic strings, wind, drum heads | Pianoteq, Moog's membrane |

**For the ESP32-S3:** subtractive is the foundation (already planned). Wavetable and FM
are the two best additions — both are cheap enough to run at 44.1 kHz with multiple voices.
Granular, spectral, and physical modeling are out of scope for V1.

---

## Serum 2 — what to learn from it

**What it is:** Xfer Records' flagship wavetable synthesizer. Industry standard for
modern digital synthesis, known for extremely high sound quality and clear UI.
https://xferrecords.com/products/serum-2

**Why it's relevant:** Serum is the clearest modern reference for how digital synthesis
should be organized — each oscillator type is a separate, well-defined concept. Even if
we only implement a subset, the taxonomy is worth understanding.

### Oscillator types in Serum 2

| Type | How it works | ESP32-S3 feasibility |
| --- | --- | --- |
| **Wavetable** | Loops through a table of waveform frames, interpolates between them | ✅ Very feasible — table lookup replaces `sinf()`, much cheaper |
| **Multisample** | Plays back recorded instrument samples at pitched intervals | ✅ Feasible — sample playback from flash (planned for drums) |
| **Sample** | Loops a single audio sample with tape-stop and slicing effects | ✅ Feasible for simple playback; tape-stop needs pitch shifting |
| **Granular** | Breaks audio into overlapping "grains" with adjustable size, density | ⚠️ Possible but CPU-heavy for many voices |
| **Spectral** | FFT-based resynthesis, real-time harmonic manipulation | ❌ Too heavy for embedded |

### Modulation system

Serum's routing model is worth studying regardless of synthesis type:

- Any parameter can be a modulation **destination**
- Modulation **sources**: LFOs, envelopes, MIDI velocity, key tracking, macros
- Sources connect to destinations via a "drag-to-assign" matrix
- Macro knobs can control multiple parameters at once with one control

**Takeaway for this project:** a modulation matrix — even a simple one (2 LFOs, 2 envelopes,
each assignable to a few destinations) — dramatically increases expressiveness without
adding more oscillators. This is how you get "one knob does something musical" rather than
"one knob changes one number."

### Wavetable — the key technique to add

A wavetable is an array of one period of a waveform at a fixed number of samples (typically
2048). You advance a phase pointer through it at `phase_increment = freq × table_size / SR`
per sample. Benefits over a `sinf()` oscillator:

- Table lookup is ~10× cheaper than `sinf()` per sample
- Store multiple waveforms in one file — interpolate between them for morphing
- Classic shapes (sine, saw, square, triangle) plus custom shapes from one table
- The ESP32-S3 has 8 MB flash — room for hundreds of wavetable files if needed

Serum uses 2048-sample tables. For embedded use, 256 or 512 is enough (less memory,
adequate aliasing at normal sample rates).

---

## DigitsVst — open-source phase distortion synthesizer

**What it is:** a free, open-source VST plugin by Louis Gorenfeld implementing
phase distortion synthesis in C++. GPL-3.0 licensed.
https://github.com/LouisGorenfeld/DigitsVst

**Why it's relevant:** it's open-source C++ synthesis code — readable, studyable, portable.

### Phase distortion synthesis

Phase distortion (PD) was invented by Casio for the CZ series (1984). Instead of
modulating a sine's frequency (FM), you distort the *playback position* (phase) through
the table using a nonlinear function. The result sounds similar to FM but is controlled
differently and is arguably more intuitive.

```
Normal:    phase increments linearly 0 → 2π → 0 → ...
           output = sin(phase)

With PD:   phase goes through a distortion function before the sine lookup
           phase_distorted = distortFn(phase)
           output = sin(phase_distorted)
```

The distortion function shapes the harmonics. A sharp bend in the function creates
high harmonics (bright, FM-like). A smooth function preserves the fundamental (warm).
This is how the Casio CZ achieved its wide range of timbres without FM complexity.

**For the ESP32-S3:** phase distortion is very cheap — one extra function call per sample.
The CZ synthesizer was running on a Z80 processor in 1984; our ESP32-S3 has thousands of
times more compute.

---

## YouTube learning resources

### Synthesis fundamentals

| Video | What to get from it |
| --- | --- |
| [How to Use Any Synthesizer (in 60 Seconds)](https://www.youtube.com/shorts/8u4xviPiz6Y) | Quick mental model for the subtractive signal path |
| [Synthesizer Basics: Amplitude, Oscillators, Timbre — Berklee Online](https://www.youtube.com/watch?v=c3udLCvoCC0) | Academic foundation; good definitions of terms we use in synthesis.md |
| [Synthesis for beginners](https://www.youtube.com/watch?v=eu0zpa7OiYA) | Broad overview; watch before diving into FM or wavetable |
| [An Introduction to Analog Synthesizers (Moog Mavis)](https://www.youtube.com/watch?v=kBT4ycltfBE) | Explains subtractive synthesis on real hardware; good complement to theory |
| [what is an LFO? simple explanation](https://www.youtube.com/shorts/m3w7RsckJoI) | LFO explained simply — critical for the modulation system |
| [Sound Design and Synth Fundamentals](https://www.youtube.com/watch?v=NJLIS2MkFe4) | Sound design and synth fundamentals |

### FM synthesis

| Video | What to get from it |
| --- | --- |
| [Short Introduction to FM Synthesis in 3 minutes](https://www.youtube.com/watch?v=_4g1Y3HNPF8) | Fastest entry point to FM concepts |
| [FM SYNTHS in under 4 minutes](https://www.youtube.com/watch?v=vvBl3YUBUyY) | Second pass — slightly more detail |
| [FM Algorithms: How to read them, how to choose one](https://www.youtube.com/watch?v=FilDXbQyhpg) | Explains FM *operator graphs* (which oscillator modulates which) — needed before implementing FM |

### Tools

| Video | What to get from it |
| --- | --- |
| [9 Best Free Synths](https://www.youtube.com/watch?v=_rIwdIrz66I) | Survey of free VSTs; useful for hands-on experimentation with synthesis paradigms before building them |

---

## Suggested learning order

Before implementing the audio engine, work through these in sequence:

1. **Watch:** "How to Use Any Synthesizer (60s)" → "Synthesis for beginners" → Berklee basics video
   *Goal: solid mental model of the subtractive chain we're already building.*

2. **Read:** `docs/theory/synthesis.md` in this repo — already covers oscillator, ADSR, filter, polyphony.

3. **Watch:** "What is an LFO" → understand modulation before implementing the modulation matrix.

4. **Watch:** FM intro (3 min) → FM under 4 min → FM Algorithms
   *Goal: decide whether to include FM in V1 or V2.*

5. **Experiment:** download one of the free synths from the "9 Best Free Synths" video. Spend an hour with it. The hands-on feel of patch design informs how we should design the UI and parameter layout.

6. **Read:** DigitsVst source code — focus on how the phase distortion function is implemented in C++.

7. **Study:** Serum 2 features list — use it as a checklist of "what a full synth has" and decide what subset V1 needs.

---

## Hichord — primary inspiration device (study the manual)

The Hichord is listed in [brief.md](../brief.md) as the primary inspiration for this project's
concept and play style. Reading the manual reveals concrete architecture decisions worth
studying closely before finalising our own design.

**Manual:** https://hichord.shop/pages/manual

### What the Hichord actually is (from the manual)

- **12 oscillators** arranged as **6 stereo pairs**, running four synthesis engines simultaneously:
  analog waveforms, FM, samples, and noise.
- **7 buttons** mapped to the 7 diatonic chords of the selected key — I, ii, iii, IV, V, vi, vii°.
  Button 1 is always the "home" chord. Zero wrong notes possible.
- **Joystick** extends each chord button into variations: 7ths, suspensions, augmented tones —
  8 directions = 8 chord colours per button.
- **Volume wheel** doubles as a parameter fine-tuner.
- **30+ built-in instruments** (piano, strings, synth pads, etc.) + reverb, delay, chorus effects.
- **6-track looper** — record chord progressions on track 1, layer 5 more tracks on top,
  all auto-synced. ~20 seconds per track.
- **USB-C** for charging, audio recording, and MIDI output to software instruments.

### Seven play modes

| Mode | What it does |
| --- | --- |
| **Play** | Standard chord trigger — press button, hold chord |
| **Strum** | Notes roll out sequentially (arpeggio-like feel) |
| **Lead** | Single melodic notes from the current key |
| **Arpeggio** | Cycles chord tones automatically, tempo-synced |
| **Looper** | 6-track live recording and overdub |
| **Drums** | Trigger percussion sounds |
| **Sequencer** | Step-by-step pattern builder |

### What to learn from it

**The diatonic 7-button layout is the most important UX insight.** Mapping buttons to chord
functions (I ii iii IV V vi vii°) rather than individual notes means every button sounds
musically correct in context. This is how the Hichord achieves "zero wrong notes" — the
constraint is the feature.

**Questions to answer while studying the manual:**

- How does the joystick map to chord extensions? Which 8 directions = which 8 qualities?
- How does the looper handle timing sync across 6 tracks without a DAW?
- How does it handle key changes — does the current pattern retune, or start fresh?
- How does Lead mode select which note from the scale each button triggers?
- What does the sequencer grid look like — steps per bar, max length?

**Architecture implications for this project:**

- 12 oscillators at once is the real voice budget we need to target. 6 stereo pairs = 6
  polyphonic chord voices, each potentially a 3-note triad = 18 note-slots minimum.
- The diatonic button map is a strong candidate for our own control layout —
  much more playable than a piano keyboard for chord-based music.
- The joystick-as-chord-modifier is a brilliant interaction model for a small device with
  few buttons. Worth considering for our encoder + button layout.
- Their looper runs on embedded hardware with 20 s capacity — understanding their memory
  and timing architecture would directly inform our loop station design.

---

## Recommended free tools for experimentation

These run on macOS/Windows and let you hear synthesis techniques before implementing them:

| Tool | Paradigm | Notes |
| --- | --- | --- |
| **VCV Rack** (free) | Modular / subtractive | Best way to understand signal flow visually |
| **Surge XT** (free, open source) | Wavetable + FM + subtractive | Open-source C++ — excellent reference code |
| **Dexed** (free, open source) | FM (DX7 clone) | Open-source FM implementation in C++ |
| **ZynAddSubFX** (free, open source) | Additive + subtractive | Deep but educational open-source synth |
| **3x Osc** (FL Studio built-in) | Subtractive | 3-oscillator subtractive synth; simple, good starting reference for the classic signal chain |

> **Surge XT and Dexed are particularly worth studying as source code** — both are
> open-source C++ synths that implement real production-quality DSP. Surge XT's
> oscillator code is a direct reference for wavetable + FM on modern hardware.

---

## Links summary

| Resource | URL | Type |
| --- | --- | --- |
| Serum 2 product page | https://xferrecords.com/products/serum-2 | Product / feature reference |
| DigitsVst (phase distortion, open source C++) | https://github.com/LouisGorenfeld/DigitsVst | Source code |
| Surge XT (wavetable + FM, open source C++) | https://github.com/surge-synthesizer/surge | Source code |
| Dexed (FM/DX7 clone, open source C++) | https://github.com/asb2m10/dexed | Source code |
| Mutable Instruments source code (Peaks, Braids…) | https://github.com/pichenettes/eurorack | Source code — embedded C++ for Eurorack modules |
| How to Use Any Synthesizer (60s) | https://www.youtube.com/shorts/8u4xviPiz6Y | Video |
| What is an LFO? | https://www.youtube.com/shorts/m3w7RsckJoI | Video |
| Synthesis for beginners | https://www.youtube.com/watch?v=eu0zpa7OiYA | Video |
| 9 Best Free Synths | https://www.youtube.com/watch?v=_rIwdIrz66I | Video |
| FM SYNTHS in under 4 minutes | https://www.youtube.com/watch?v=vvBl3YUBUyY | Video |
| Berklee: Amplitude, Oscillators, Timbre | https://www.youtube.com/watch?v=c3udLCvoCC0 | Video |
| An Introduction to Analog Synthesizers (Moog Mavis) | https://www.youtube.com/watch?v=kBT4ycltfBE | Video |
| FM Algorithms: How to read them | https://www.youtube.com/watch?v=FilDXbQyhpg | Video |
| Short Introduction to FM Synthesis (3 min) | https://www.youtube.com/watch?v=_4g1Y3HNPF8 | Video |
| Hichord manual (primary inspiration device) | https://hichord.shop/pages/manual | Manual / UX reference |
