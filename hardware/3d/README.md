# hardware/3d/ — OpenSCAD parts

3D-printable parts for the keyboard, **FDM print-validated** (PLA + PETG — see *Print-validated*
below), designed in OpenSCAD via the **`/cad`** loop
(`.claude/skills/cad/`). Reference dimensions and decisions:
[0002-key-design.md](../../docs/explorations/0002-key-design.md) and
[gateron-ks-33.md](../../docs/hardware/modules/gateron-ks-33.md) (the **Silent vernier table**
is the source of truth).

## Files

**The v0 sources were deleted on purpose (2026-07-31)** after the print validation — clean rebuild ahead. Everything needed to rebuild is in [`octaves-plan.md`](octaves-plan.md) (plan, calibrated values, verbatim source snapshots) + the design notes below. Target structure (plan §5): **`key_cap.scad` · `plate.scad` · `assembly.scad`** — three files, layout + cutout module live inside `plate.scad`.

Open a `.scad` in OpenSCAD (**Design → Automatic Reload and Preview**) and tune the Customizer
live; Claude edits + renders in parallel (`/cad` skill).

## Design decisions (this session, 2026-07)

### Plate (v0 `plate-octave.scad` / `switch-cutout.scad`, deleted — snapshot in [`octaves-plan.md`](octaves-plan.md) §7)
- **Stepped cutout**, not a plain 14 mm hole: a **1.2 mm clip shelf at 14 mm** (the switch clips latch here), then the hole **widens to ~15.2 mm** below → the plate bottom sits **flush with the switch base**, so plate + switch both rest on the PCB. Plate total **~2.4 mm**. Rounded cutout corners (`hole_r`).
- **Screws:** **M2 countersunk hex-socket** (allen flat-head, DIN 7991) so they sit **flush** (a socket-cap counterbore is too deep for a 2.4 mm plate). Layout: **4 corners + 1 dead-centre** (in the gap between the natural and sharp rows). M2 **heat-set threaded inserts** go in the **enclosure**, not the plate (the plate is just a clearance hole).
- **Margins → 1.6 mm** (rev. 2026-08-01: = 2× cap_gap, 0.4 grid; plate corner `prad 3.2` = cap corner + margin, concentric): the plate will likely **merge into the enclosure** as the key container, so a big border isn't needed.
- **Pitch:** the **cap is fixed at 18 mm**; the gap sets the pitch (`pitch = cap + gap`). Current value: **gap 0.8 mm → pitch 18.8 mm** (decided 2026-08-01: 2×0.4 grid, backed by v0's print-validated 1.0 gap; the interim 0.5 was never printed side-by-side).

### Keycap (v0 `cap.scad`, deleted — snapshot in [`octaves-plan.md`](octaves-plan.md) §7)
- **Design:** hollow shell (THT low-profile style, open bottom), rim → bevel → rounded top edge profile, central MX post with "+" cross bore. **Directional profile (2026-08-01):** per-side top insets, front (south) deepest — the slope reads as touch direction, OEM-style; bottom chamfers on skirt + post foot. **All parameter values** (walls, profile radii, insets, post/cross dims, `stem_clearance`, `mouth_flare`, sizes) live in [`octaves-plan.md`](octaves-plan.md) §7 — the **single source of values**; don't duplicate them here.
- **Flat top / no dish.** A concave dish was tried three ways (spherical, rounded-rect clipped, ellipsoid "pill") and **removed** — it kept eating the rounded top edge and never matched the reference. **To revisit** with a different method.

### Toolchain proven
OpenSCAD driven headless by Claude via CLI (edit → `openscad --render` PNG → inspect → iterate),
plus FreeCAD headless to measure the official STEP. See [0004](../../docs/explorations/0004-cad-toolchain.md).

## ✅ Print-validated (2026-07)

The fit-test kit (2-space stepped plate + 1×2 and 1×1 caps) was printed and **passed** — switch clips firmly at the 1.2 mm shelf, cap cross-socket clips snug without cracking. The test parts were deleted after passing; the calibrated values live in the sources and are the spec:

- **Materials/processes:** PLA and PETG, FDM, **0.2 mm and 0.4 mm nozzles**, across **3 print services** — all fit.
- **Tolerances confirmed as-designed:** `kerf = 0` (plate cutout) and `stem_clearance = 0.18` (cap socket). Don't re-derive.
- **Finish:** **0.2 mm nozzle is the clear winner** — looks and feels better; 0.4 works but visibly rougher. Spec 0.2 for cosmetic/final parts; 0.4 acceptable for hidden/structural.
- **⚠️ Elephant foot is a real service-side risk:** one of the three services shipped parts with a squished first layer (their printer/slicer config — outside our control). **Design it out**: final parts get a small chamfer/lead-in on bottom edges (the cap's `mouth_flare` already does this; the plate cutouts and enclosure bottom edges should too).

## ⚠️ Open — design questions (see [plan.md](../../docs/plan.md))
- **Bottom-edge chamfers** against service-side elephant foot (see Print-validated) — add to plate cutouts + enclosure.
- ✅ **Cross bore — resolved (2026-08-01, `key_cap.scad`):** the stem cross is asymmetric, but the wide arm is **1.23** (official drawing + vernier — the earlier 1.28 was a misread). The bore is now **per-arm** (1.23/1.10) with clearances **0.05/0.18** → **both slots 1.28**, which is bit-for-bit the print-validated v0 geometry (that's *why* it worked: same principle as injection MX caps — snug wide arm, loose thin arm). Equal slots ⇒ **no LED-north constraint needed**; it only appears if `stem_clearance_wide` is raised (the echo warns). Also adopted from the official cap spec: bore length **4.1** + **R0.3** inner-corner fillets.
- Revisit the **dish** (or keep flat).
- **Sharp offset** ("illusion" toward the true note) — not done.
- **Anti-tilt** for the tall 2u naturals — not addressed.
- ✅ **Pitch** — resolved for now: **18.5 mm** (cap 18 + gap 0.5, parametric — [`octaves-plan.md`](octaves-plan.md) §7); revisit after build 1.0.
- ✅ **Sharps** — positions follow the layout snapshot in [`octaves-plan.md`](octaves-plan.md) §7 (C#–D# at 1.5·pitch, F#–G#–A# at 1.25·pitch).
