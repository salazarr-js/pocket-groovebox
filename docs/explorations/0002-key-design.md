# 0002 — Key design

**Status:** decided (lever + switch)  ·  (`open → exploring → decided`)

Decisions taken:
- ✅ Switch: **Gateron KS-33 Low Profile 2.0 Silent Brown** (tactile, silent, 3.0mm travel)
- ✅ Mechanism: **Lever bridge** with continuous rod pivot
- ✅ Key proportions: **18mm pitch** (revised from 20mm to fit a 280mm frame — see [0003-design-system.md](0003-design-system.md))
- Mecánica complementaria: **resorte de compresión extra** entre lever y PCB

## Context

The keyboard uses **Gateron KS-33 Low Profile 2.0 Silent Brown** switches (15×15mm footprint).
The goal is a keyboard that looks and feels like a real instrument — not a computer keyboard.

## Decision: switch — KS-33 Silent Brown

**Decided: Gateron KS-33 Low Profile 2.0 Silent Brown.**

| Parameter | Value |
|---|---|
| Type | Tactile, silent |
| Total travel | 3.0 ± 0.2 mm |
| Pre-travel (actuation) | 1.7 ± 0.4 mm |
| Operating force | 55 ± 15 gf |
| Switch height | ~12.2 mm |
| Footprint | 15 × 15 mm (MX-compatible) |
| Pre-lubed | Yes (factory) |

Chosen over MX standard ($\varnothing$ 4.0 mm travel, ~18.5 mm height):
- **Silencioso**: viene con amortiguación de serie y pre-lubricado de fábrica. Los MX silenciosos táctiles son raros y caros (Boba U4, etc.)
- **Altura**: ahorra ~6 mm en el ensamble total vs MX — crítico en un gabinete de 150 mm
- **Costo**: más barato que alternativas MX silenciosas táctiles (en ARG ~31,636 ARS)
- El viaje (3.0 mm) se amplifica con el lever, dando ~7-8 mm en el dedo

Si en el futuro se necesita más viaje sin cambiar el diseño mecánico, se puede migrar a MX
con el mismo lever y PCB (mismo footprint de 15×15 mm), solo ajustando la altura del lever
y la posición del switch.

## Decision: mechanism — lever bridge + spring

### Why a lever

Standard approach (switch + keycap directly on top) produces a flat, computer-keyboard look
with only 3 mm of key travel. A **lever bridge** decouples keycap shape from switch:
- Amplifies switch travel at the finger via lever ratio
- Allows piano-style proportions (72 mm deep keys, elevated black keys)
- Gives space for proper pivot action, like commercial mini synths (Arturia, Novation, Yamaha)

### Lever geometry

```
Side view — one white key

  FRONT ◄──────────────────────► BACK
  ┌────────────────────────────────┐
  │     KEYCAP / lever (3D print)  │  ← player presses here (b = ~55mm from pivot)
  └─────────────────┬──────────────┘
                    │  pivot (at back)
                    │
     [KS-33 switch] ← lever activates here (a = distance from pivot)
                    │
  ══════════════[PCB]═══════════════
```

Key relationship — lever ratio:

```
finger_travel = switch_travel × (b / a)
finger_force  = switch_force  × (a / b)    (sin resorte extra)
```

With the white key depth of 72 mm and pivot at the back (~70 mm from front edge),
the finger contact point is at b ≈ 55 mm from the pivot, leaving ~15 mm of key in front of it.

| Switch position (a) | Ratio (b/a) | Finger travel | Finger force (no spring) |
|---|---|---|---|
| **30 mm** from pivot | 1.83× | **5.5 mm** | 30 gf |
| **25 mm** from pivot | 2.2× | **6.6 mm** | 25 gf |
| **22 mm** from pivot | 2.5× | **7.5 mm** | 22 gf |
| **20 mm** from pivot | 2.75× | **8.25 mm** | 20 gf |

**Target:** 7-8 mm travel at the finger with 40-50 gf force.

### Spring compensation

The lever ratio reduces force at the finger proportionally. To restore a piano-like feel
(40-50 gf), a **compression spring** is added between the lever and the PCB/enclosure
at distance `c` from the pivot:

```
finger_force = switch_force × (a/b) + spring_force × (c/b)
```

This **decouples travel from force** — the switch defines travel (via lever ratio), the
spring tunes the force independently.

**Example for ~8 mm + ~45 gf:**
- Lever ratio b/a = 2.75× → switch at 20 mm from pivot → 8.25 mm at finger
- Switch contributes: 55 × (20/55) = 20 gf at finger
- Spring needed: 25 gf at finger → placed at c = 40 mm → spring_force = 25 × (55/40) ≈ 34 gf

The spring can be:
- A small coil compression spring between lever and PCB (easy to source, replaceable)
- A torsion spring at the pivot rod (one spring per key or one spanning multiple keys)

### Alternative mechanisms considered

| Mechanism | Travel | Force tuning | DIY-friendly | Verdict |
|---|---|---|---|---|
| **Lever bridge + spring** (elegido) | 5-8 mm | Sí (resorte extra) | ★★★★☆ | Mejor balance: viaje, fuerza, forma de piano |
| Directo sobre switch (sin lever) | 3 mm | No | ★★★★★ | Muy corto, tecla plana |
| Leaf spring (Yamaha FS/HQ mini) | 4-5 mm | Parcial | ★★☆☆☆ | Excelente tacto pero requiere acero resortado y tratamiento térmico — no práctico para DIY |
| Living hinge impreso 3D | 1-2 mm | Parcial | ★★★★★ | Viaje muy limitado, fatiga del material |
| Rubber dome + membrana | 2-4 mm | No | ★★☆☆☆ | Spring + contacto integrados, pero inconsistentes y de vida limitada |
| Capacitivo (sin mecanismo) | 0 mm | N/A | ★★★★★ | Descartado en 0001-input-method.md — no da suficiente feedback táctil |

## Decision: key proportions

Full-size piano: white key ~23mm wide. nanoKEY2-style mini: ~12mm.

**Decided: 18mm pitch** (revised from 20mm). The original choice was 20mm because it is the
only pitch where the 60% black-key ratio lands on clean 4mm-grid multiples. That decision was
**superseded when the frame was shrunk to 280mm wide**: a 15-key (2-octave) keyboard at 20mm
pitch spans 300mm and no longer fits. 18mm keeps the full C3–C5 range in 270mm (+5mm margins)
at the cost of leaving the 4mm grid. See `docs/explorations/0003-design-system.md` for the
full trade-off. The 18mm *pitch* and the 270mm *span* are the only off-grid dimensions; the
keycap itself was sized to **16mm = 4×4** (with a 2mm gap, 1mm/side) and the depths land on the
grid too — so everything except the pitch and span stays grid-clean (see below).

**Key length (depth):** set to **72mm white / 44mm black** (up from 60/36). 60mm was the bare
minimum for the lever; 72mm brings the feel up toward the Casio SA / Yamaha PSS mini standard
(~78mm white / ~48mm black) — a 28mm front fingertip zone and more lever leverage. It costs no
extra device depth: the front shelf was trimmed (keys pushed to the bottom edge, ~2mm shelf,
control band grew to 86mm), so the frame stays 280 × 160mm.

| Dimension | Value | Grid factor |
|---|---|---|
| White key pitch (center-to-center) | 18 mm | 4.5×4 (off-grid) |
| White key visible width (keycap) | 16 mm (pitch − 2 mm gap, 1 mm/side) | 4×4 ✓ |
| Black key width | 10 mm | off-grid |
| Black key width ratio | ≈56 % of pitch | — (60% rule = 10.8 mm) |
| White key depth | 72 mm | 18×4 |
| Black key depth | 44 mm | 11×4 |
| Black key depth ratio | ≈61 % | ~✓ |
| Black–white key gap | 1 mm | rounded separation |
| 15 white keys total span | 270 mm | 67.5×4 (off-grid) |

**Black–white separation:** a 1 mm gap separates each black key from its neighbouring white
keys, with a rounded transition — the white-key notch fillet (r3) is concentric with the black
key's r2 bottom corner (r3 = r2 + 1 mm), so the gap stays uniform around the corner.

Black-key width is **10 mm (≈56% of pitch)** rather than the literal 60% rule (10.8 mm):
cleaner geometry, and real pianos run even slimmer (~40-45% of pitch), so 10 mm reads more
like an instrument than the heuristic 60%.

**Reference instruments.** 18mm pitch / 10mm black / 72mm white matches the mini-key class —
Casio SA-2/SA-46 (32 mini keys, ~381mm body) and Yamaha PSS-F30 (37 "HQ Mini" keys, 506mm
body) both use ~18mm white-key pitch and ~78/48mm key lengths. Full-size for comparison is
23.5mm pitch. So this keyboard sits squarely in proven mini-key territory, just in a narrower
2-octave frame than either reference (both are ≥38cm wide).

## Pivot mechanism

**Decided: continuous rod.** A single steel or brass rod (3 mm diameter) spans all keys.
Each key has a U-slot "ear" at the back that wraps around the rod. This is the simplest
and most proven approach — closest to real piano action, no per-key alignment needed.

| Option | Difficulty | Notes |
|---|---|---|
| **Continuous rod** ✅ | Low | Steel/brass wire 3 mm; keys have U-slot ears |
| Per-key pivot (snap-in shaft) | Medium | All-plastic, no metal parts, but per-key alignment |
| Living hinge (printed flexure) | Low | Zero assembly, but ~1 mm max travel, fatigue risk |

## Consequences

- **PCB**: switch footprint must align with lever geometry. Switch position defines the
  lever ratio and thus travel/force. Default: switch center at ~20 mm from pivot
  (~52 mm from the front edge of the 72 mm key) for ~8 mm finger travel; the deeper key
  leaves more room to place the finger press point for the target ratio.
- **Lever 3D print files needed**: white key lever, black key lever, pivot rod brackets.
  Each lever includes: U-slot ear for pivot rod, switch pad (contacts switch stem),
  spring seat (retains compression spring), keycap profile.
- **Springs**: 20 compression springs 0.4×5×15mm (AliExpress, on order). ~30-42 gf at
  6 mm compression — matches the ~35 gf target. One spring per key + spares.
- **Enclosure**: keyboard zone depth and internal clearance must accommodate lever
  swing arc (~5-8 mm at front, less at pivot). Switch-to-keytop height ~17 mm.
- Update `docs/hardware/wiring.md` with switch matrix PCB layout once lever
  geometry is finalized.
- Update `docs/hardware/enclosure-layout.svg` with keyboard zone geometry.
