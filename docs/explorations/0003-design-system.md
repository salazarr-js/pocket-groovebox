# 0003 — Design system (measurement grid)

**Status:** decided, with a keyboard exception  ·  (`open → decided → revised`)

> **Revision (2026-06-18) — 280 mm frame.** The frame was shrunk from 320 mm to **280 mm**
> wide for a more pocketable device. A 2-octave keyboard at 20 mm pitch spans 300 mm and no
> longer fits, so the keyboard moved to **18 mm pitch** (15 white keys = 270 mm + 5 mm margins).
> This puts the keyboard **off the 4 mm grid** — which weakens the "decisive argument" below,
> since that argument was the main reason 4 mm beat 5 mm. The 4 mm grid is **retained for the
> shell, walls, control placement, and corner radii**; the keyboard is now an explicit, isolated
> exception in service of fitting two octaves into 280 mm. Tables below are updated to the new
> values; off-grid entries are marked.

## Context

Physical products benefit from a base measurement unit — a grid that all dimensions snap to,
so spacing, proportions, and components feel harmonious rather than arbitrary. This is the
equivalent of the 8px/16px grid in UI design.

The question was: **4mm or 5mm base unit?**

## Decision

**4mm base grid.**

All dimensions should be multiples of 4mm. The enclosure, key proportions, control placement,
wall thicknesses, and gaps should all land on this grid.

## Why 4mm wins

### 1. Black key proportions — the decisive argument

Standard piano black key proportions: ~60% of white key width and ~60% of white key depth.
This is what makes a keyboard look and feel like a musical instrument.

With 20mm white key pitch (shared by both systems):

| Dimension | 4mm grid | 5mm grid |
|---|---|---|
| White key pitch | 20mm = 5×4 ✓ | 20mm = 4×5 ✓ |
| Black key width (60%) | **12mm = 3×4 ✓** | 10mm = 2×5 → 50% ✗, or 12mm off-grid ✗ |
| White key depth | 60mm = 15×4 ✓ | 60mm = 12×5 ✓ |
| Black key depth (60%) | **36mm = 9×4 ✓** | 35mm = 7×5 ~ok, or 40mm = 67% ✗ |

On a 5mm grid, the black key width has no clean solution: 10mm is too narrow (50%) and
12mm breaks the grid. The 4mm grid is the only one where the 60% piano ratio lands on clean
multiples for both width and depth.

> **Note (superseded):** this argument assumed 20mm pitch. The 280 mm frame revision dropped
> the keyboard to 18mm pitch, where the black key (~10mm) is off-grid regardless — so this
> particular point no longer holds for the keyboard. The other reasons (wall thicknesses,
> hardware fit) still favour 4mm for the rest of the device, so the base grid stands.

### 2. 3D printing wall thicknesses

FDM nozzle standard is 0.4mm. Structural walls are always multiples of that:
`0.8 → 1.2 → 1.6 → 2.0 → 2.4mm`. 4mm = 10× the nozzle diameter — walls, bosses, and
rails naturally align. 5mm has no relationship to FDM line width.

### 3. Common hardware dimensions fit cleanly

| Component | Dimension | On 4mm grid |
|---|---|---|
| EC11 encoder body | 24mm | 6×4 ✓ |
| Speaker (current 30mm → target 40mm) | 40mm | 10×4 ✓ |
| White key depth | 72mm | 18×4 ✓ |
| Black key depth | 44mm | 11×4 ✓ |
| Black key width | 10mm | off-grid (18mm pitch) |

## Reference values

All key dimensions for the keyboard, at the revised **18mm pitch** (the pitch and span are off
the 4mm grid; the 16mm keycap and the depth dimensions land on it):

| Dimension | Value | Grid factor |
|---|---|---|
| White key pitch (center-to-center) | 18mm | 4.5×4 ⚠ off-grid |
| White key visible width (keycap) | 16mm (pitch − 2mm gap, 1mm/side) | 4×4 ✓ |
| Black key width | 10mm | off-grid (≈56%) |
| Black key width ratio | ≈56% of pitch | — (60% rule = 10.8mm) |
| White key depth | 72mm | 18×4 |
| Black key depth | 44mm | 11×4 |
| Black key depth ratio | ≈61% | ~✓ |
| 15 white keys total span | 270mm | 67.5×4 ⚠ off-grid |

Depth (length) was set to 72/44mm — up from 60/36 — to reach Casio SA / Yamaha PSS mini feel
without deepening the frame; the front shelf absorbs the difference (see margins). Note the
keycap (16 = 4×4) and *depths* land cleanly on the 4mm grid (72 = 18×4, 44 = 11×4); only the
18mm pitch and 270mm span remain off-grid.

*(Previous values were 20mm pitch / 12mm black / 19mm visible / 300mm span at 60/36mm depth —
see the revision note at the top of this file.)*

### Enclosure outline

| Dimension | Value | Grid factor |
|---|---|---|
| Enclosure width | 280mm | 70×4 |
| Enclosure height | 160mm | 40×4 |

Width was reduced from 320mm to **280mm = 70×4** for a more pocketable device; both are clean
4mm multiples. The keyboard (now 270mm at 18mm pitch) sets the floor on width — see the margin
table below. The height was 150mm in early sketches, but 150 = 37.5×4 is **off-grid**. 160mm is
the nearest value that is both a clean 4mm multiple (40×4) and a whole number of 1cm grid cells,
and its 160mm depth splits as **86mm control band + 72mm keys + 2mm front shelf** (the keys
are pushed to the bottom edge, leaving only the shell's corner-radius clearance below them).
140mm (the other on-grid option) is too short — the 72mm keys plus the control band already
need ~158mm before any shelf.

### Margins

| Margin | Value | Grid factor | Notes |
|---|---|---|---|
| Side wall → keyboard edge | 5mm | 1.25×4 ⚠ | absorbs the leftover from 280 − 270mm keyboard; the margin takes the half-unit (same kind of compromise as the old 10mm margin) |
| Top edge → keyboard top | 86mm | 21.5×4 ⚠ | the control zone (display, encoders, joystick, speaker) sits in this band; grew from 80mm when the keys were pushed to the bottom |
| Keyboard front → bottom edge | 2mm | 0.5×4 ⚠ | minimal front shelf — keys pushed as far down as the 8mm shell corner radius allows |

The off-grid values are now the keyboard itself (18mm pitch) and the 5mm side margin it leaves
(280 − 15×18 = 10mm total → 5mm per side). A true 4mm-clean margin isn't reachable while the
keyboard sits on an 18mm sub-pitch; the shell, control zone, and shelf remain grid-aligned.

### Corner radii

Multiples of 4mm for structural parts; 2mm (half-unit) allowed for small inset features only.

| Element | Radius | Grid factor |
|---|---|---|
| Enclosure shell | 8mm | 2×4 |
| Component cutouts (display bezel, joystick plate, speaker box) | 4mm | 1×4 |
| Inner insets (screen active area, speaker grille) | 2mm | half-unit |
| Piano keys (bottom corners only) | 2mm | half-unit |
| Black–white key separation (notch fillet) | 3mm concave / 1.5mm convex | r3 = key r2 + 1mm gap |

There is a **1mm gap between black and white keys**; the white-key notch fillet (r3) is
concentric with the black key's r2 bottom corner, so the separation stays a uniform 1mm all
the way around the corner.

## Consequences

- `docs/hardware/enclosure-layout.svg` — ✅ updated: 280×160mm enclosure, 18mm-pitch keyboard
  pushed to the shell bottom with rounded keys (r2) + a 1mm rounded black↔white gap; keybed bg
  rounds r8 to follow the shell; dimension annotations moved outside the part and a yellow NOTES
  box added (see CLAUDE.md docs table for structure)
- `docs/explorations/0002-key-design.md` — ✅ key proportions table updated to 18mm pitch
  (16mm white keycap, 10mm black, 72/44mm depths, 1mm black↔white gap)
- Enclosure width must accommodate the 270mm keyboard + margins (5mm each side → 280mm = 70×4);
  height is 160mm = 40×4 (see table above)
- All 3D print files: walls in multiples of 0.8mm, bosses/rails in multiples of 4mm
