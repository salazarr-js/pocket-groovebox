# hardware/

Design files for the Pocket Groovebox custom hardware. The *why* and decisions live in
[`docs/`](../docs/); this folder holds the *what* — organized by discipline.

```
hardware/
  3d/        OpenSCAD: shared layout + plate + caps + enclosure (.scad source)  ← production
```

## Toolchain

Decided in [docs/explorations/0004-cad-toolchain.md](../docs/explorations/0004-cad-toolchain.md):
**OpenSCAD** (mechanical + layout). FreeCAD is a fallback. KiCad was reserved for a custom PCB —
**dropped for V1**: the keyboard is hand-wired, no custom PCB (see [`3d/octaves-plan.md`](3d/octaves-plan.md)).
*(Ergogen was evaluated and dropped — the layout lives in `3d/octave-layout.scad`.)*

## 3d/

OpenSCAD parts. `.scad` is the versioned source. Design loop + collaborate live: **`/cad` skill**
(`.claude/skills/cad/`). **Design decisions + print results: [`3d/README.md`](3d/README.md).**

**Status: clean rebuild** — the v0 sources were print-validated (PLA/PETG, 0.2 & 0.4 mm nozzles, 3 print services) and then deleted on purpose; [`3d/octaves-plan.md`](3d/octaves-plan.md) holds the full rebuild plan, calibrated values and source snapshots. Target: **`key_cap.scad` + `plate.scad` + `assembly.scad`**, then incremental integration toward the two-shell enclosure (top shell absorbs the plate + display/speaker/encoders/joystick; bottom shell carries a perfboard main board).

## Key facts (see docs for detail)

- Switch: **Gateron KS-33 Low Profile 2.0 Silent Brown** — cutout **14.0 mm**, **stepped plate ~2.4 mm** (1.2 mm clip shelf), MX-cross stem.
- **Cap 18 mm fixed**, gap sets the pitch → exploring **19–19.5 mm pitch** (not finalised; docs still say 18 mm). Caps **OP-1 × HiChord** flat; per-key **SK6812** RGB; switches direct to **PCF8575** (no diodes).
- Full board = 24 keys. First build = **one-octave module** ([0005](../docs/explorations/0005-proto-keyboard-module.md)).
