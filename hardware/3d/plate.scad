// plate.scad — N-octave keyboard plate for Gateron KS-33 low-profile switches.
// Spec: hardware/3d/octaves-plan.md — §2 calibrated values (do NOT re-derive),
// §7 layout/cutout snapshots, §8 v1.0 scope (octaves = 1, 12 keys C–B).
// Batch-2 scope (2026-08-01): the plate prints as a COMPLETE keyboard module —
// PCF8575 bosses + wire channels PENDING (blocked on calipers of the real
// module: outline, hole positions/ø, tallest top-side component). No M2
// plate↔shell screws yet — that sandwich comes with the enclosure (batch 3).
// Print orientation: modeled keyboard-face UP; flip 180° about Y in the slicer
// (REQUIRED: each cutout step then rests on solid material — no bridging).
// Edge chamfers are on BOTH faces, so either face down avoids elephant foot.

/* [Layout] */
octaves = 1; // [1:4]
// cap unit (mm) — fixed by the cap design
cap_1u = 18;
// pitch = cap_1u + cap_gap (18.8 current, decided 2026-08-01: 0.8 = 2×0.4 grid,
// backed by v0's print-validated 1.0 gap; 0.5 was never printed side-by-side)
cap_gap = 0.8;

/* [Plate] */
// = shelf 1.2 + pocket 1.2
thick = 2.4;
// plate margin beyond the cap bounding box (2026-08-01: 1.6 = 2× cap_gap, 0.4 grid)
margin = 1.6;
// plate corner radius — cap base_corner_r 1.6 + margin 1.6 = 3.2: the plate edge
// runs concentric with the corner cap's rounded corner
prad = 3.2;
// edge chamfer on BOTH faces — counters elephant foot whichever face prints down;
// ~0.3 ≈ first layer height + margin (see docs/hardware/fdm-tolerances.md)
edge_chamfer = 0.3;

/* [KS-33 cutout — calibrated §2, do not re-derive] */
// clip cutout (top, KS-33 standard)
hole_t = 14.0;
// body pocket (bottom) — INDEPENDENT value, never derive as hole_t + shelf
hole_b = 15.2;
// clip shelf thickness (plate-thickness spec)
shelf = 1.2;
hole_r = 0.5;
// print-validated (PLA/PETG, 0.2 & 0.4 nozzles): 0; + looser / − tighter
kerf = 0.0;

/* [Render] */
// ghost cap footprints for visual fit check
show_caps = false;
// cutaway through the front (naturals) row
section = false;
$fn = $preview ? 32 : 96;

eps = 0.01;
pitch = cap_1u + cap_gap;

// ---- key layout (octaves-plan §7) — [x_u, y_u, w_u, h_u], centres ----
oct_naturals = [ [0.5,2,1,2],[1.5,2,1,2],[2.5,2,1,2],[3.5,2,1,2],[4.5,2,1,2],[5.5,2,1,2],[6.5,2,1,2] ];
oct_sharps   = [ [0.75,0.5,1.5,1],[2.25,0.5,1.5,1],[3.75,0.5,1.5,1],[5.0,0.5,1.0,1],[6.25,0.5,1.5,1] ];

function shift_key(k, o) = [k[0] + o*7, k[1], k[2], k[3]];
function layout_keys(n) = [ for (o = [0:n-1], k = concat(oct_naturals, oct_sharps)) shift_key(k, o) ];
// centre the WHOLE keyboard on x=0: 3.5 key-units per octave
function key_x(k) = (k[0] - 3.5*octaves) * pitch;
// naturals at y=0, sharps behind (+y)
function key_y(k) = (2 - k[1]) * pitch;
function key_w(k) = k[2] * cap_1u;
function key_d(k) = k[3] * cap_1u;

keys = layout_keys(octaves);

// accessors for assembly.scad (`use <>` imports functions, not variables)
function plate_thick() = thick;

// ---- plate outline: cap bbox + margin (derive, don't hand-formula) ----
cxmin = min([ for (k = keys) key_x(k) - key_w(k)/2 ]) - margin;
cxmax = max([ for (k = keys) key_x(k) + key_w(k)/2 ]) + margin;
cymin = min([ for (k = keys) key_y(k) - key_d(k)/2 ]) - margin;
cymax = max([ for (k = keys) key_y(k) + key_d(k)/2 ]) + margin;
plate_w = cxmax - cxmin;
plate_d = cymax - cymin;

echo(str(len(keys), " keys · plate ", plate_w, " × ", plate_d, " × ", thick, " mm · pitch ", pitch));
assert(hole_b > hole_t, "body pocket must be wider than the clip cutout");
assert(thick > shelf, "plate must be thicker than the clip shelf");
assert(2 * edge_chamfer < thick - shelf, "chamfers would eat through the walls");

// ---- geometry ----
module plate_outline() {
  translate([(cxmin + cxmax)/2, (cymin + cymax)/2])
    offset(r = prad) square([plate_w - 2*prad, plate_d - 2*prad], center = true);
}

// straight walls, 45° chamfer collapsing to an inset footprint at BOTH faces
module plate_body() {
  hull() {
    translate([0, 0, edge_chamfer]) linear_extrude(thick - 2*edge_chamfer) plate_outline();
    linear_extrude(eps) offset(delta = -edge_chamfer) plate_outline();
    translate([0, 0, thick - eps]) linear_extrude(eps) offset(delta = -edge_chamfer) plate_outline();
  }
}

// stepped cutout (§7): clip shelf on top, widened pocket below so the plate
// bottom sits flush with the switch base
module ks33_cutout() {
  module rsq(sz) offset(r = hole_r) square(sz - 2*hole_r, center = true);
  translate([0, 0, -0.1]) linear_extrude(thick - shelf + 0.1) rsq(hole_b + kerf);
  translate([0, 0, thick - shelf - 0.2]) linear_extrude(shelf + 0.4) rsq(hole_t + kerf);
}

module plate() {
  difference() {
    plate_body();
    for (k = keys) translate([key_x(k), key_y(k)]) ks33_cutout();
  }
}

// ghost caps float 2.0 above the plate (switch stem height, §7)
module cap_ghosts() {
  for (k = keys)
    %translate([key_x(k), key_y(k), thick + 2])
      linear_extrude(5.5) offset(r = 1.5) square([key_w(k) - 3, key_d(k) - 3], center = true);
}

if (section)
  intersection() {
    plate();
    translate([0, -25, thick/2]) cube([plate_w + 20, 50, thick + 10], center = true);
  }
else {
  plate();
  if (show_caps) cap_ghosts();
}
