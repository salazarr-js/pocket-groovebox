---
name: cad
description: Collaborative OpenSCAD 3D design loop for Pocket Groovebox hardware (plate, caps, enclosure). Use when creating or editing any .scad part in hardware/3d/. Claude edits + renders a PNG to self-check while the USER watches live in the OpenSCAD GUI (auto-reload) and approves or requests changes before STL export. Keeps the user in the loop instead of running end-to-end.
---

# CAD — collaborative OpenSCAD loop

Design 3D-printable parts for this project in OpenSCAD, **with the user in the loop** — Claude edits and renders, the user watches live in the OpenSCAD GUI and steers.

## Tools & locations

- OpenSCAD CLI: `/Applications/OpenSCAD.app/Contents/MacOS/OpenSCAD`
- Parts live in `hardware/3d/` — `.scad` (source, versioned) + `.stl` (generated, committed).
- FreeCAD (for STEP geometry, headless): `QT_QPA_PLATFORM=offscreen /Applications/FreeCAD.app/Contents/MacOS/FreeCAD -c script.py`

## The loop (each iteration)

1. **Edit** the target `.scad` (parameters at the top).
2. **Render** a PNG so Claude can see the result and self-check:
   ```sh
   OSC="/Applications/OpenSCAD.app/Contents/MacOS/OpenSCAD"
   "$OSC" -o /tmp/preview.png --render --viewall --autocenter \
     --imgsize=1000,750 --colorscheme=Tomorrow --camera=0,0,0,58,0,22,0 hardware/3d/<part>.scad
   ```
   Then Read the PNG. Cutaway: add `-D section=true` and use camera `0,0,0,62,0,30,0`.
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

## Conventions

- **Parameters at the top** of every `.scad` (dimensions, tolerances, `$fn`) — the Customizer surfaces them.
- Add a **`kerf`/tolerance** parameter to anything that mates with a real part (cutouts, sockets) — dialed on the first test print.
- Add a **`section = false;`** flag + `if(section) intersection(){ part(); translate([0,-25,thick/2]) cube([W+20,50,thick+10],center=true); }` so a cutaway renders on demand.
- Use **BOSL2** for fillets/chamfers/rounded boxes on non-trivial shapes.
- `.stl` is regenerated from `.scad`; the `.scad` is the source of truth.

## Reference dimensions

Authoritative switch/plate/cap numbers: [gateron-ks-33.md](../../docs/hardware/modules/gateron-ks-33.md) (the **Silent vernier table** is the source of truth). Key values: cutout **14.0 mm**, **stepped plate ~2.4 mm** with a **1.2 mm clip shelf** (widened to ~15 mm below), stem cross **~1 mm** arms, **18 mm** pitch.
