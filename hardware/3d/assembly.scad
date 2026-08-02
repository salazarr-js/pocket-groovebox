// assembly.scad — VISUAL fit check: plate + 12 caps (v1.0, octaves = 1).
// Spec: hardware/3d/octaves-plan.md §5 (file structure) / §8 (v1.0 scope).
// Not printable — export STLs from plate.scad and key_cap.scad. This file only
// composes them: caps float cap_float above the plate (switch stem height, §7).
// Later: + PCF8575 dummy board for collision/clearance checks.

use <plate.scad>
use <key_cap.scad>

/* [View] */
show_plate = true;
show_caps = true;
// switch stem height above the plate top (§7 assembly value)
cap_float = 2.0;
// keys pressed for a quick travel sanity view (0 = none), e.g. 1 presses C
pressed_key = 0; // [0:12]
// KS-33 total travel
travel = 3.0;

/* [Colors — OP-1 × HiChord mood, preview only] */
plate_color = "#37474f";
natural_color = "#eceff1";
sharp_color = "#455a64";

$fn = $preview ? 32 : 64;

keys = layout_keys(1);
echo(str("assembly: ", len(keys), " keys · plate top z=", plate_thick(), " · caps at z=", plate_thick() + cap_float));

// touch direction per key (key_cap.scad profile, closed 2026-08-01): naturals
// front; sharps N/S symmetric, deep side W (C#/F#) or E (D#/A#); G# all-light
function key_dir(k) =
  k[3] == 2 ? "S" :
  let (xu = k[0] % 7)
  xu == 0.75 ? "W" :
  xu == 2.25 ? "E" :
  xu == 3.75 ? "W" :
  xu == 5    ? "ALL" : "E";

if (show_plate) color(plate_color) plate();

if (show_caps)
  for (i = [0 : len(keys) - 1]) {
    k = keys[i];
    z = plate_thick() + cap_float - (i + 1 == pressed_key ? travel : 0);
    color(k[3] == 2 ? natural_color : sharp_color)
      translate([key_x(k), key_y(k), z])
        cap(k[2], k[3], key_dir(k));
  }
