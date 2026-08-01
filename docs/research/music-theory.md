# Music theory — reference for the groovebox

The musical side companion to [synthesis.md](synthesis.md): that doc is *how sound is generated*,
this one is *which pitches and relationships we play*. Grounded in what the project already does —
the note→frequency math, the two-row keyboard, and the major/minor chord layers in
[`sketches/09-audio-notes-gate`](../../sketches/09-audio-notes-gate/09-audio-notes-gate.ino).

## 1. Notes, semitones, octaves

Western music uses **12 equally-spaced pitches per octave** (the chromatic scale):

```
C  C#  D  D#  E  F  F#  G  G#  A  A#  B   (then C again, one octave up)
0   1  2   3  4  5   6  7   8  9  10 11
```

- A step of **1 = a semitone** (smallest step, adjacent keys); **2 = a tone** (whole step).
- An **octave** = 12 semitones; the note repeats with the same name and **double the frequency**.
- **Octave numbers** tell you which octave: C3, C4 (middle C), C5… Our keyboard's base is **C3**.
- White keys = the 7 "natural" notes (C D E F G A B); black keys = the 5 sharps/flats (C#…).
  The black-key gaps fall between **E–F** and **B–C** (no black key there) — that's the pattern
  the on-screen piano draws.

## 2. Frequency & equal temperament

Each semitone multiplies frequency by the **twelfth root of 2** (≈ 1.0595). So:

```
freq(n) = ref · 2^(n/12)
```

where `n` is semitones from the reference. Two common references:
- **A4 = 440 Hz** (concert standard).
- **C4 ≈ 261.63 Hz** (middle C), **C3 ≈ 130.81 Hz** — our base.

This is exactly the firmware's `semiToFreq(semi) = 130.81 · 2^(semi/12)` (semi 0 = C3). Equal
temperament means every key is equally (slightly) "out of tune" vs pure ratios, in exchange for
being able to play in any key — the standard tradeoff.

## 3. Intervals

An **interval** is the distance between two notes, in semitones. The ones that matter for chords:

| Semitones | Interval | Sound |
| --- | --- | --- |
| 0 | unison | same note |
| 3 | minor third | darker |
| 4 | major third | brighter |
| 7 | perfect fifth | stable, "open" |
| 12 | octave | same note, higher |

The **third** (3 vs 4 semitones) is what flips a chord between minor and major — the single most
important interval for mood.

## 4. Scales

A **scale** is a subset of the 12 notes that sound good together — a pattern of tones (T) and
semitones (S) from a root:

- **Major** (happy/bright): `T T S T T T S` → e.g. C: C D E F G A B.
- **Natural minor** (sad/dark): `T S T T S T T` → e.g. A: A B C D E F G.
- **Pentatonic** (5 notes, hard to play "wrong"): major pentatonic drops the 4th & 7th.

Useful later for a **"scale lock"** feature (snap played notes to a chosen scale) and for the
sequencer.

## 5. Chords & triads

A **triad** stacks the root, a third, and a fifth:

| Chord | Recipe (semitones) | Feel |
| --- | --- | --- |
| **Major** | root, **+4**, +7 | bright |
| **Minor** | root, **+3**, +7 | dark |
| Diminished | root, +3, +6 | tense |
| Augmented | root, +4, +8 | uneasy |

> This is **exactly** what the firmware's chord layers do: major = `{root, +4, +7}`, minor =
> `{root, +3, +7}` (see `gateOn()` in `09-audio-notes-gate`). The momentary ↑/↓ layers just pick
> the third interval. Later additions: **7th chords** (add +10 or +11), **inversions** (reorder
> the notes), and strumming.

## 6. The keyboard layout

The black/white key pattern is a visual map of the 12 semitones: 7 white + 5 black per octave,
with black-key gaps at E–F and B–C. Our two-row computer-keyboard mapping (tracker style) lays one
octave on the bottom row + home row and the next on the top + number rows — see the layout in
[`sketches/09-audio-notes-gate`](../../sketches/09-audio-notes-gate/09-audio-notes-gate.ino) and the piano in
[`app/keyboard-bridge.html`](../../app/keyboard-bridge.html).

## 7. Rhythm & time (for the sequencer, later)

- **BPM (beats per minute)** — tempo; one "beat" is usually a quarter note.
- **Bar / measure** — a group of beats (commonly 4 = 4/4 time).
- **Note values** — whole, half, quarter, eighth, sixteenth… each half the previous.
- **Steps** — a step sequencer divides a bar into equal slots (e.g. 16 sixteenth-note steps);
  each step can trigger a note. This is the basis of the planned pattern sequencer
  ([plan.md](../plan.md)).

## How it maps to the project

- **Pitch math** → `semiToFreq()` (base C3), [synthesis.md](synthesis.md) for how it's voiced.
- **Chords** → the maj/min layers (§5) in `09-audio-notes-gate`.
- **Keyboard** → the two-row mapping + web piano (§6).
- **Scales / 7ths / sequencer timing** → not built yet; this doc is the reference when we do.
