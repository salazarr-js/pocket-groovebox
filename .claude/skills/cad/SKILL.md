---
name: cad
description: Collaborative OpenSCAD design loop — Claude edits + renders a PNG to self-check while the USER watches live in the OpenSCAD GUI (auto-reload) and approves or requests changes before STL export. Generic OpenSCAD code-generation guidance only; part dimensions ALWAYS come from the project's spec docs, never from this skill. Use when creating or editing any .scad file.
---

# CAD — collaborative OpenSCAD loop

Design 3D-printable parts in OpenSCAD **with the user in the loop** — Claude edits and renders, the user watches live in the OpenSCAD GUI and steers. This skill is generic: it says *how* to write good OpenSCAD, never *what* the dimensions are.

## Where dimensions come from

**Never from this skill and never from memory.** Read the project's spec doc for the part before modeling — in this repo that is `hardware/3d/keyboard/octaves-plan.md` (calibrated, print-validated values; do **not** re-derive them). If a value is missing there, ask the user or measure the physical part — don't invent it.

## Tools & locations

- OpenSCAD CLI: `/Applications/OpenSCAD.app/Contents/MacOS/OpenSCAD`
- Parts live in `hardware/3d/` — `.scad` is the source of truth; `.stl` is generated on approval.
- FreeCAD (for STEP geometry, headless): `QT_QPA_PLATFORM=offscreen /Applications/FreeCAD.app/Contents/MacOS/FreeCAD -c script.py`

## The loop (each iteration)

1. **Edit** the target `.scad` (parameters at the top).
2. **Render** a PNG so Claude can see the result and self-check:
   ```sh
   OSC="/Applications/OpenSCAD.app/Contents/MacOS/OpenSCAD"
   "$OSC" -o /tmp/preview.png --render --viewall --autocenter \
     --imgsize=1000,750 --colorscheme=Tomorrow --camera=0,0,0,58,0,22,0 hardware/3d/<part>.scad
   ```
   Then Read the PNG. Cutaway: add `-D section=true` and use camera `0,0,0,62,0,30,0`. For non-obvious geometry, render a second angle before concluding it's right.
3. **Claude reviews** the PNG; **the user reviews live** in the OpenSCAD GUI (below).
4. **User approves or requests changes** → repeat from step 1. Do NOT silently run a whole part end-to-end — pause for the user on meaningful changes.
5. On approval, **export STL**:
   ```sh
   "$OSC" -o hardware/3d/<part>.stl hardware/3d/<part>.scad
   ```

## The user's role — live 3D (this is the point)

1. Open the `.scad` in the **OpenSCAD app** (`hardware/3d/<part>.scad`).
2. Enable **Design → Automatic Reload and Preview**. Now every Claude edit auto-reloads and re-previews — the user sees it in interactive 3D (rotate, zoom, measure).
3. The user **approves or requests changes** ("rounder corners", "socket 0.1 looser", "taller"). The GUI **Customizer** (Window → Customizer) exposes the top-of-file parameters as sliders so the user can also tweak values directly.

## Writing OpenSCAD well

- **Every dimension is a named parameter at the top** of the file, grouped with comments — the Customizer surfaces them. No magic numbers inside module bodies.
- **Derive, don't hand-formula.** Sizes that depend on other features (plate outline from a key bounding box, screw positions from corners) must be computed from those features, not hand-calculated constants — hand math ships silent off-by-a-bit bugs.
- **`assert()` the invariants** (e.g. wall thickness stays positive, a pocket never breaks through) and **`echo()` the key derived values** (part count, bounding box) so every render is a sanity check.
- **Small composable modules** for features; **functions** for computed geometry. Prefer building from 2D profiles + `linear_extrude`/`rotate_extrude` when the shape allows — cleaner CSG, faster renders.
- **Boolean hygiene:** extend cutting solids by a small epsilon (~0.01) past the faces they cut so previews don't z-fight and exports stay manifold.
- **`$fn` gated by preview:** `$fn = $preview ? 32 : 96;` — fast iteration, smooth exports.
- **A `kerf`/`clearance` parameter on anything that mates with a real part** (cutouts, sockets, snap-fits) — dialed in on a test print, never assumed.
- **A `section = false;` flag** + `if (section) intersection() { ... }` so a cutaway renders on demand.
- Use **BOSL2** for fillets/chamfers/rounded boxes on non-trivial shapes.

## Printability (FDM)

- **Chamfer bottom edges** (counters elephant-foot and needs no supports); save fillets for top/vertical edges.
- Keep **overhangs ≤ 45°** and bridges short; if a feature can't meet that, redesign or plan the print orientation around it.
- **Walls ≥ 2 perimeters** of the intended nozzle; state the intended **print orientation** in a comment at the top of the file — design decisions (steps, countersinks, cosmetic faces) depend on it.
- Printed holes come out undersized: give mating holes their clearance via the `kerf` parameter and **calibrate with a small test coupon before printing the full part**.
