# Octaves plan — handoff from the cad-lab FreeCAD exercise (2026-07-31)

The FreeCAD parametric replica of `plate-octave.scad` (cad-lab, `projects/groovebox-octave-plate/`) got paused — the tool fought back more than it taught. This doc carries over everything the exercise produced that this repo doesn't have yet, so the plate work finishes here in OpenSCAD.

## 1. The `octaves` parameter (the main deliverable)

Generalize the plate to N side-by-side octaves. The insight: with the plate, screws and caps all **derived from the key list bounding box** (which `plate-octave.scad` already does), the only real change is generating a multi-octave key list — everything else scales for free.

**`octave-layout.scad`** — add octave-aware helpers, keep the old names working (defaults preserve the existing caller: `assembly-octave.scad`):

```scad
function shift_key(k, o) = [k[0] + o*7, k[1], k[2], k[3]];
function layout_keys(octaves = 1)     = [ for (o = [0:octaves-1], k = oct_keys)     shift_key(k, o) ];
function layout_naturals(octaves = 1) = [ for (o = [0:octaves-1], k = oct_naturals) shift_key(k, o) ];
function layout_sharps(octaves = 1)   = [ for (o = [0:octaves-1], k = oct_sharps)   shift_key(k, o) ];

// centre the WHOLE keyboard on x=0: 3.5 key-units per octave
function key_x(k, pitch, octaves = 1) = (k[0] - 3.5 * octaves) * pitch;
```

**`plate-octave.scad`** — add `octaves = 1;` to the Customizer params, then swap `oct_keys` → `layout_keys(octaves)` (same for naturals/sharps in the cap preview) and pass `octaves` to every `key_x` call. The bbox loops (`cxmin`…`cymax`), plate size and corner screws need **no changes** — they derive from the key list.

**Centre screw** becomes one per octave (at each octave's centre, in the inter-row gap):

```scad
centre = screw_center ? [ for (o = [0:octaves-1]) [ (o*7 + 3.5 - 3.5*octaves) * pitch, screw_cy ] ] : [];
```

At `octaves = 1` this evaluates to the current single `[0, screw_cy]` — no behavior change.

**Sanity check** after wiring it: `octaves = 2` → echo should report 24 keys, plate ≈ 268.5 × 57.5 mm, 6 screws; `octaves = 1` must be byte-identical to today's STL.

## 2. Known-good values — calibrated, don't re-derive

All already in `plate-octave.scad` / `switch-cutout.scad`; listed so nobody "improves" them:

| Param | Value | Why |
|---|---|---|
| `hole_t` | 14.0 | KS-33 standard plate cutout |
| `hole_b` | 15.2 | clears the 15 mm switch body — **independent value**, never derive it as `hole_t + shelf` (numeric coincidence today; deriving would silently move the pocket if `shelf` changes) |
| `shelf` | 1.2 | KS-33 clip geometry (plate-thickness spec) |
| `kerf` | 0.0 | print-validated (PLA/PETG, 0.2 & 0.4 mm nozzles, 3 print services); + looser / − tighter |
| `thick` | 2.4 | = shelf 1.2 + pocket 1.2 |
| `screw_d` / `cs_d` | 2.2 / 4.0 | M2 clearance + flat-head countersink, 90° cone |

## 3. Lessons worth keeping (from the FreeCAD detour)

- **Derive, don't hand-formula.** The FreeCAD model hand-computed plate depth and shipped a 0.5 mm bug (57.0 vs 57.5). The SCAD's bbox-from-caps approach never had it. Keep deriving.
- **Print orientation**: model keyboard-up (as this repo does), flip 180° about Y in the slicer — each cutout step rests on solid material (no bridging), the cosmetic keyboard face gets the textured build plate, and the 90° countersink prints as a clean 45° overhang.
- **Sharps structure** (info, list-driven here anyway): the "irregular" row is two regular groups — C#–D# spaced 1.5·pitch, F#–G#–A# spaced 1.25·pitch.

## 4. Architecture decision (2026-07-31): NO custom PCB

V1 keyboard = **plate + switches hand-wired to the PCF8575 module**. No KiCad keyboard PCB. Consequences for the plate design:

- **PCF8575 mounts INTO the plate** (no separate carrier backplate): M2 bosses in the **switch-free bays of the sharps row**. The two wide bays flank D# — **C#↔D# and D#↔F#** (28.5 mm switch spacing − 15 mm body = **13.5 mm free** each; the F#↔G# / G#↔A# bays are only 8.75 mm). Bay centres sit at the D|E and E|F boundaries, 28.5 mm apart — the module hangs under the plate spanning them. Reference model: [GrabCAD — Module PCF8575](https://grabcad.com/library/module-pcf8575-16bit-io-expander-1), but **verify board outline + hole pitch with calipers on the physical board** (community model); the tallest component on the plate-facing side sets the standoff height. Measured values become a `pcf_*` param block + dummy board model for fit checks.
- **Wire channels in the plate underside** for the 1.4 mm copper bus wire: slot width `1.4 + kerf` (start 1.6), depth ~1.6–1.8. For snap-in retention: neck the opening to ~1.2 and widen inside — mini snap-fit, same kerf-calibration logic as the switch cutouts. Routes: switch pins → PCF bays.
- **Plate ↔ enclosure**: the M2 screws are the sandwich (plate + enclosure, heat-set inserts in the enclosure) — plate outline/screws must stay compatible with the future enclosure interface.
- ⚠ **Open**: the per-key SK6812 LED plan assumed a PCB. Hand-wiring MINI-E LEDs is painful — resolve (defer LEDs? LED strip? mini flex board?) before locking the V1 plate.

## 5. Target file structure — clean rebuild (decided 2026-07-31)

The v0 sources were **deleted on purpose** (print-validated; everything needed to rebuild is in this doc + [README](README.md)). The rebuild is exactly **three files** — layout data and the cutout module live *inside* `plate.scad` (no separate shared files: with no PCB, only the plate and the assembly consume positions):

```
hardware/3d/
  key_cap.scad   — PRINTABLE: the keycap family cap(w_u, d_u) — WITH the asymmetric cross-bore fix
  plate.scad     — PRINTABLE: N-octave plate (octaves param) = key layout + stepped cutouts + M2
                   + PCF8575 bosses + wire channels + bottom chamfers
  assembly.scad  — VISUAL fit-check: includes both + PCF dummy (collisions, clearances)
```

**Pre-flight fixes baked into the rebuild** (details in [README](README.md) Open): asymmetric cross bore (1.28/1.10 + clearance, wide slot horizontal, LED-north), bottom-edge chamfers vs elephant foot, pitch finalised (19 vs 19.5).

## 6. Enclosure vision (context the plate must fit into)

Two shells, M2 sandwich:

- **Top shell** — eventually *absorbs the plate* (the plate is the keyboard region of the top shell) + openings/mounts for: **display (screwed — ST7789 170×320)**, speaker, 2× EC11 encoders, joystick.
- **Bottom shell** — carries the main board: **perfboard (placa perforada) screwed to the enclosure**. No custom PCB this iteration; keyboard = switches hand-wired to the PCF8575, the plate routes the wires (channels).

**Iteration path: start with `plate.scad` + `key_cap.scad` alone, integrate the rest incrementally** (PCF mount → wire channels → display/speaker/encoders/joystick → shell merge).

## 7. Source snapshots (drained from the deleted v0 .scad — verbatim, print-validated)

**Key layout** (from `octave-layout.scad` — each entry `[x_u, y_u, w_u, h_u]`, centres; naturals 1u×2u front row, sharps behind):

```scad
oct_naturals = [ [0.5,2,1,2],[1.5,2,1,2],[2.5,2,1,2],[3.5,2,1,2],[4.5,2,1,2],[5.5,2,1,2],[6.5,2,1,2] ];
oct_sharps   = [ [0.75,0.5,1.5,1],[2.25,0.5,1.5,1],[3.75,0.5,1.5,1],[5.0,0.5,1.0,1],[6.25,0.5,1.5,1] ];
oct_keys     = concat(oct_naturals, oct_sharps);
function key_x(k, pitch) = (k[0] - 3.5) * pitch;  // → octaves form: (k[0] - 3.5*octaves) * pitch
function key_y(k, pitch) = (2 - k[1]) * pitch;    // naturals at y=0, sharps behind (+y)
function key_w(k, unit)  = k[2] * unit;
function key_d(k, unit)  = k[3] * unit;
```

**Stepped cutout** (from `switch-cutout.scad` — clip shelf on top, widened pocket below so the plate bottom sits flush with the switch base):

```scad
module ks33_cutout(thick, shelf = 1.2, hole_t = 14.0, hole_b = 15.2, hole_r = 0.5, kerf = 0) {
  module rsq(sz) offset(r = hole_r) square(sz - 2 * hole_r, center = true);
  translate([0, 0, -0.1]) linear_extrude(thick - shelf + 0.1) rsq(hole_b + kerf);          // pocket (bottom)
  translate([0, 0, thick - shelf - 0.2]) linear_extrude(shelf + 0.4) rsq(hole_t + kerf);   // clip shelf (top)
}
```

**Plate params** (from `plate-octave.scad`): `cap_1u 18` (fixed) · `cap_gap 1.0` → `pitch 19` (⚠ 19 vs 19.5 open) · `thick 2.4` · margins `1` · corner `prad 3` · plate = cap bounding box + margins (derive, don't hand-formula). Screws: M2 flat-head countersunk (DIN 7991) — shaft `2.2`, countersink `Ø4.0` 90° (depth = `(4.0−2.2)/2`), 4 corners inset `4` + 1 per octave centre at `y 14`; positions derive from the bbox.

**Cap params** (from `cap.scad` — the tuned look-and-feel values):

| Group | Values |
|---|---|
| Size | `unit_mm 18` · `cap_height 5.5` |
| Bevel/look | `base_rim_height 1` · `top_inset 1` (per side) · `top_edge_round 1` · `base_corner_r 1.5` · `top_corner_r 2.5` · flat top, no dish |
| Shell (hollow) | `wall_thickness 1.2` · `top_thickness 1.4` · open bottom |
| Mount | `post_diameter 5.5` · cross `4.0` span · arms `1.1` (v0 both — **rebuild: 1.28 horizontal / 1.10 vertical**) · `socket_depth 2.8` (KS-33 tower 2.95) |
| Print | `stem_clearance 0.18` FDM / `0.05` resin (arm width only) · `mouth_flare 0.5` (45° lead-in) · optional `split_post` |
| Assembly | caps float `2.0` above the plate (switch stem height, from `assembly-octave.scad`) |

Background theory lives in cad-lab: `docs/researchs/fits-and-tolerances-fdm-2026-07-26.md` and `elements/01-tolerances-and-fits/`.
