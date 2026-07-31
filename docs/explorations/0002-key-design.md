# 0002 — Key design

**Status:** decided (OP-1-style flat caps, direct on switch)  ·  (`open → exploring → decided → revised`)

> **Revision (2026-07) — OP-1 pivot (V1).** For the **first iteration**, the lever-bridge +
> piano-key mechanism is **deferred** in favour of the OP-1 approach: **flat rectangular keycaps
> mounted directly on the switch**, no lever, no sculpted piano-key structure. Reasons: far
> simpler mechanically, no springs or pivot rod, and it's cheap to 3D-print and **easy to
> re-print a single bad part** — the priority for the first physical instance (fabrication is
> outsourced to a print service). The note layout stays a **flattened 2-octave piano** (naturals
> row + sharps offset in a back row) so it still plays like an instrument. The lever design is
> **kept open below as a candidate for a later iteration** — not rejected, just parked to keep V1
> simple. Switch choice, 18 mm pitch, and the 2-octave range are unchanged.

Decisions taken:
- ✅ Switch: **Gateron KS-33 Low Profile 2.0 Silent Brown** — full specs + mechanical drawing in [`../hardware/modules/gateron-ks-33.md`](../hardware/modules/gateron-ks-33.md)
- ✅ Mechanism (**V1**): **flat rectangular cap directly on the switch** (OP-1 style; no lever, no spring, no pivot rod). Lever bridge deferred to a later iteration — see bottom.
- ✅ Cap mounting: **thin low-profile cap with a guide rib that inserts into a plate slot** (G915-style — the guide lives in the cap, not the plate walls). The rib reuses the plate's **stabilizer cutout** as its guide slot, so **no MX stabilizer hardware** is needed. Stem socket = MX cross (4.00 / 1.10 / 1.28). *(The "skirt wrapping the switch body" variant and the plate-well "floating cap" were considered and dropped.)*
- ✅ Layout: **flattened 2-octave piano** = **24 keys** — 14 naturals (C3–B4, 1u × 2u tall) + 10 sharps (back row: C#/D#/F#/A# = 1.5u wide, G# = 1u). From the user's KLE; encoded in [`hardware/3d/octave-layout.scad`](../../hardware/3d/octave-layout.scad). *(No top C5.)*
- ✅ Pitch: **18 mm** (fits 2 octaves in a 280 mm frame — see [0003-design-system.md](0003-design-system.md))
- ✅ Plate: **3D-printed**, 1.2 mm low-profile cutout (14 mm), outsourced to a print service
- ✅ Look & feel: **OP-1 × HiChord** (primary aesthetic references) — refined minimalism, flat rounded-rectangle caps, muted base palette + per-key RGB color accents; premium tactile, not toy-like
- ✅ Cutout confirmed by **vernier**: switch housing 13.8 mm / relaxed clips 14.5 mm → plate cutout **14.0 mm** (housing passes, clips grip). Two tilt axes to guide: naturals (2u tall) tilt fore-aft; wide sharps (1.5u) tilt sideways.
- ✅ Toolchain: layout + plate + caps in **OpenSCAD**, PCB in **KiCad** — see [0004-cad-toolchain.md](0004-cad-toolchain.md).
- 🚧 First build: a **one-octave proto module** (12 keys) before the full 24 — see [0005-proto-keyboard-module.md](0005-proto-keyboard-module.md).

## Context

The keyboard uses **Gateron KS-33 Low Profile 2.0 Silent Brown** switches (15×15 mm body,
MX-style cross stem). The goal is a keyboard that looks and plays like an instrument — not a
computer keyboard — while staying simple enough to 3D-print and iterate cheaply.

## Decision: switch — KS-33 Silent Brown

**Decided: Gateron KS-33 Low Profile 2.0 Silent Brown.** Full confirmed specs and the
dimensional drawing live in the [module doc](../hardware/modules/gateron-ks-33.md). Summary:

| Parameter | Value |
|---|---|
| Type | Silent tactile |
| Operating force | 55 ± 15 gf |
| Total travel | 3.0 ± 0.2 mm |
| Pre-travel | 1.7 mm |
| Body footprint | 15 × 15 mm |
| Stem | MX-style cross (4.00 env., 1.10 / 1.28 arms) |
| Plate cutout / thickness | 14.0 mm / 1.2 mm |

Chosen for: silent + pre-lubed out of the box, low height (~12 mm body saves ~6 mm vs MX),
cheaper than silent tactile MX, and an MX-cross stem that keeps keycap options open.

## Decision: mechanism — flat cap directly on switch (OP-1 style)

No lever, no spring, no pivot. **Each key = one switch + one 3D-printed rectangular cap.**
The finger presses the cap, the cap presses the switch stem, 3 mm of travel. This trades the
piano-like deep travel of the lever design for radical simplicity and easy iteration.

### Cap mounting — two routes

- **Floating cap, guided by the plate (primary).** The rectangular cap sits in a well in the
  plate and pushes the stem via a **nub** underneath; it does *not* clip the stem. Most forgiving
  for outsourced FDM, and a bad cap is re-printed on its own without depending on the tiny stem
  fit. Recommended for the first instance.
- **Stem-mounted cap (plan B).** The cap has a cross socket that clips onto the stem. Cleaner
  look, feasible now that we have exact stem cotas (model the socket to 4.00 / 1.10 / 1.28, not
  generic MX 1.17/1.27). Better in resin.

### Terminology

See the [module-doc glossary](../hardware/modules/gateron-ks-33.md#anatomy--glossary) for
cap / nub / stem / pins.

## Decision: layout — flattened 2-octave piano

**Range: C3–C5 = 25 keys** (15 naturals + 10 sharps). Two rows of flat rectangular caps:
naturals in the front row, sharps offset in a back row (with the usual gaps at E–F and B–C),
like the OP-1.

```
  ▢ ▢     ▢ ▢ ▢     ▢ ▢     ▢ ▢ ▢        ← sharps (back row, 10)
▢ ▢ ▢ ▢ ▢ ▢ ▢ ▢ ▢ ▢ ▢ ▢ ▢ ▢ ▢            ← naturals (front row, 15)
C D E F G A B C D E F G A B C
```

### Switch grid (mm, from the left-most natural center = x 0)

**Naturals (front row, y = 0):** 15 switches at `x = n × 18`, n = 0…14
→ 0, 18, 36, 54, 72, 90, 108, 126, 144, 162, 180, 198, 216, 234, 252
(notes C3 D3 E3 F3 G3 A3 B3 C4 D4 E4 F4 G4 A4 B4 C5).

**Sharps (back row, y = +row-spacing):** 10 switches, centered on the boundary between the two
naturals they sit between → `x = (i + 0.5) × 18`:

| Note | x (mm) | Note | x (mm) |
|---|---|---|---|
| C#3 | 9 | C#4 | 135 |
| D#3 | 27 | D#4 | 153 |
| F#3 | 63 | F#4 | 189 |
| G#3 | 81 | G#4 | 207 |
| A#3 | 99 | A#4 | 225 |

(No sharp at x 45 / 117 / 234 — the E–F and B–C gaps.)

### Cap & row dimensions (starting points — tunable)

| Dimension | Value | Notes |
|---|---|---|
| Natural cap | 16 × 20 mm (W×D) | 16 = pitch − 2 mm gap; depth is free now (no lever) |
| Sharp cap | 10 × 16 mm | width per design system; free-standing rectangle, can widen |
| Row spacing (natural↔sharp switch centers) | ~17 mm | **hard floor ≈16 mm**: 15 mm bodies interleave in X, so the rows must clear in Y |
| Keyboard span (width) | 270 mm | 15 × 18; unchanged |
| Keyboard depth | **~35 mm** | down from ~72 mm+ with levers — see consequences |

> **Body-collision constraint:** a sharp at x 9 sits between naturals at x 0 and x 18; their
> 15 mm bodies overlap in X, so the sharp row must be offset in Y by at least one body depth
> (~16 mm). This sets the row-spacing floor, not aesthetics.

## Look & feel — OP-1 × HiChord (design language)

The keys target the aesthetic of two Primary references in [brief.md](../brief.md):

- **OP-1** — flat rectangular caps with softly rounded corners, deliberate color accents, personality without clutter.
- **HiChord** — premium CNC-aluminium / anodised minimalism, tight tolerances, colored buttons used for *visual hierarchy* (function vs play keys); a "serious tool, pocket-sized" — not a toy.

Shared target: **refined minimalism with intentional color**, a quiet premium tactile press, and uniform tight gaps that read as "considered."

| Aspect | Direction |
|---|---|
| Cap form | Flat rectangle, softly rounded corners (r ~1–2 mm), uniform tight gaps |
| Base palette | Muted / anodised-like (black, cream, warm gray) — pick one shell colorway |
| Color accents | Per-key **SK6812 RGB** carries the color — HiChord-style hierarchy (active note, chord tones, mode) + OP-1 pops. It's *lit*, not painted, so it reconfigures per mode |
| Feel | KS-33 **silent tactile** already matches the quiet, quality press both aim for |
| Finish (our reality) | We 3D-print, not CNC aluminium → prefer **resin caps** for a smoother surface, and hold tight, consistent tolerances so the printed grid still reads premium |

Note: HiChord is a *chord* instrument — we borrow its **look and feel**, not its 7-button chord layout. Our layout stays the flattened 2-octave piano; chord/scale behaviour can be layered via modes + the RGB without changing the physical keys.

## Consequences

- **Keyboard depth drops from ~72 mm to ~35 mm.** Removing the lever frees ~35–40 mm of device
  depth. This should be reflected in [0003-design-system.md](0003-design-system.md) and the
  enclosure layout — the 160 mm frame height now has significant slack, or the device can shrink.
- **PCB**: 25 switches wired directly to 2× PCF8575 (no diodes, common GND — see
  [module doc](../hardware/modules/gateron-ks-33.md#wiring-to-the-keyboard-i2c-expanders)).
  Footprint is the KS-33 bottom pattern; two switch rows at the grid above.
- **3D-print files needed**: (1) natural cap, (2) sharp cap, (3) plate with 14 mm cutouts +
  cap wells at the grid above. Print a **single-key test** first to validate cutout clip, cap
  fit, and feel before committing to all 25.
- **No springs, no pivot rod** — the compression springs and steel rod from the lever design are
  dropped from the BOM.
- Update `docs/hardware/enclosure-layout.svg`: replace the sculpted piano keys with the two-row
  flat-cap grid and reclaim the freed depth.

---

## CAD — plate & keycap (first pass, 2026-07)

First OpenSCAD pass — files + full parametric detail in [`hardware/3d/README.md`](../../hardware/3d/README.md):

- **Plate:** **stepped cutout** — 1.2 mm clip shelf at 14 mm, widened to ~15.2 mm below, so the ~2.4 mm plate sits **flush with the switch base** and both rest on the PCB. **M2 countersunk (allen flat-head)** screws, 4 corners + 1 dead-centre; **heat-set inserts in the enclosure**. Margins 1 mm (the plate is expected to merge into the enclosure as the key container).
- **Keycap:** **hollow shell** (THT low-profile) — straight base rim → bevel → **rounded top edge**; central cylindrical **post with a "+" cross bore**. **Flat top** (a dish was tried three ways and removed — to revisit).
- **Pitch:** the cap is fixed at **18 mm**; the gap sets the pitch. Exploring **19–19.5 mm** — **not finalised** (supersedes the earlier "18 mm pitch" once settled).
- **Assembly:** `assembly-octave.scad` renders the octave with real caps to judge the whole together.

⚠️ **Before any print:** deep-check every measurement (cutout, stepped shelf, plate thickness, cap dims, **stem socket**) against other keycap/plate models — tracked in [plan.md](../plan.md).

## Deferred to a future iteration — lever bridge + spring

> **Open, not rejected.** This mechanism is parked for a later iteration; V1 keeps it simple with
> flat caps (see top). It's a strong candidate for a V2 that wants real piano feel: it amplifies
> the 3 mm switch travel to ~7–8 mm at the finger via a lever, with a compression spring tuning
> force independently — at the cost of springs, a pivot rod, deep 72 mm keys, and per-key parts
> that are fiddly to 3D-print. Revisit once the flat-cap V1 is validated.

- **Lever bridge**: keycap on a lever pivoting at the back; `finger_travel = switch_travel × (b/a)`.
  Target was 7–8 mm travel at 40–50 gf, with switch at ~20 mm from a ~55 mm finger point (ratio ~2.75×).
- **Spring compensation**: a compression spring at distance `c` restored force after the lever
  divided it down: `finger_force = switch_force × (a/b) + spring_force × (c/b)`.
- **Pivot**: continuous 3 mm steel/brass rod spanning all keys, U-slot ears per key.
- **Key proportions**: 72 mm white / 44 mm black depth (for lever leverage + Casio SA / Yamaha
  PSS mini feel).
- Alternatives considered and rejected: direct-on-switch (too flat — *now chosen anyway for
  simplicity*), leaf spring (needs heat-treated steel), living hinge (≤1–2 mm travel, fatigue),
  rubber dome (inconsistent), capacitive (no tactile feedback, see 0001).
