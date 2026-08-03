// assembly.scad — VISUAL fit check: plate + caps + under-plate ghosts.
// Spec: hardware/3d/keyboard/octaves-plan.md §5 (file structure) / §8 (v1.0 scope).
// Not printable — export STLs from plate.scad and key_cap.scad. This file
// composes them: caps float cap_float above the plate (switch stem height, §7)
// plus the ghost visualizations DEFINED in plate.scad (parametric KS-33 /
// PCF board / right-angle I2C header — fast, and the KS-33 one is the Silent
// we build with). Opt-in vendor meshes (../vendor/, see vendor/README.md)
// overlay the real geometry for high-fidelity checks: the PCF mesh carries
// the true component detail (it caught the SSOP corner), the switch mesh is
// the STANDARD KS-33 — not the Silent — so visual reference only.

use <plate.scad>
use <key_cap.scad>

/* [View] */
show_plate = true;
show_caps = true;
// parametric KS-33 ghosts seated in every cutout (D#/F# rotated per key_rot)
show_switches = false;
// ghost PCF8575 board hanging at standoff height
show_pcf = false;
// ghost right-angle I2C header (5-pin column, tails north over the tab)
show_header = false;
// keys pressed for a quick travel sanity view (0 = none), e.g. 1 presses C;
// range covers the full 24-key final keybed at octaves = 2
pressed_key = 0; // [0:24]

/* [View — vendor meshes (opt-in, slower)] */
// real PCF8575 module mesh (true component detail)
show_pcf_mesh = false;
// real switch mesh — ⚠️ STANDARD KS-33, not the Silent; visual only
show_switch_mesh = false;

/* [Colors — OP-1 × HiChord mood, preview only] */
plate_color = "#37474f";
natural_color = "#eceff1";
sharp_color = "#455a64";

/* [Hidden] */
$fn = $preview ? 32 : 64;
// switch stem height above the plate top (§7 assembly value)
cap_float = 2.0;
// KS-33 total travel
travel = 3.0;

keys = layout_keys(plate_octaves());
echo(str("assembly: ", len(keys), " keys · plate top z=", plate_thick(), " · caps at z=", plate_thick() + cap_float));

// key_dir() (touch direction per key) comes from plate.scad, defined beside
// key_rot() so the per-key identity switches stay with the layout table

if (show_plate) color(plate_color) plate();

if (show_caps)
  for (i = [0 : len(keys) - 1]) {
    k = keys[i];
    z = plate_thick() + cap_float - (i + 1 == pressed_key ? travel : 0);
    color(k[3] == 2 ? natural_color : sharp_color)
      translate([key_x(k), key_y(k), z])
        cap(k[2], k[3], key_dir(k));
  }

if (show_switches) switch_ghosts();
if (show_pcf) pcf_ghosts();
if (show_header) pcf_header_ghosts();
if (show_pcf_mesh) pcf_mesh_ghosts();
if (show_switch_mesh) switch_mesh_ghosts();
