// plate.scad — N-octave keyboard plate for Gateron KS-33 low-profile switches.
// Spec: hardware/3d/octaves-plan.md — §2 calibrated values (do NOT re-derive),
// §7 layout/cutout snapshots, §8 v1.0 scope (octaves = 1, 12 keys C–B).
// Batch-2 scope (2026-08-02): the plate prints as a COMPLETE keyboard module —
// PCF8575 VERTICAL in the D#↔F# bay (the pillar-free spot), M2×5 bosses:
// south (mid) pair in the naturals↔sharps web, north (header) pair on a
// derived back tab (module dims: docs/hardware/modules/pcf8575.md §Mechanical).
// Module mounts component side + right-angle I2C header toward the plate,
// solder face out (tails point outward, joints accessible).
// ASSEMBLY RULE: D# switch turns -90° (pins WEST) and F# +90° (pins EAST) so
// their pins point away from the board (pins sit in the switch's south half).
// Wire channels (octaves-plan §4) temporarily disabled pending a re-route to
// the vertical module. No M2 plate↔shell screws yet — batch 3.
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

/* [PCF8575 mount — measured, pcf8575.md §Mechanical] */
// board outline (vernier 2026-08-01); dummy/ghost only, bosses anchor on holes
pcf_board_l = 32.1;
pcf_board_w = 21.3;
pcf_board_t = 1.6;
// mounting-hole pitch along the board's long axis (pitch-true PCB drill)
pcf_hole_pitch = 28.3;
// hole-pair spans across the board: corner pair (5-pin-header end) / mid pair
pcf_span_a = 17.0;
pcf_span_b = 6.5;
// module centre Y — vertical module, 24.0 (0.4 grid) = minimum clean spot:
// south boss pair just clears the naturals pockets (≥ hole_b/2 + od/2)
pcf_cy = 24.0;
// boss height below the plate — 2.8 (0.4 grid, 2026-08-02): the board sits in
// the pillar-free D#↔F# spot (D#/F# pins rotated away), so the floor is the
// right-angle header body 2.5 (tallest on the component side, > SSOP 2.1)
// + 0.3 air under the tab
pcf_standoff = 2.8;
// M2 self-tap boss OD — 4.4 (0.4 grid) = pilot 1.8 + 1.3 wall (>3 perimeters
// of 0.4). Below the fdm-tolerances ≥2 wall rule, accepted 2026-08-02: tiny
// 5 g board, non-structural; tap gently — self-tap hoop stress splits
// thinner walls, ø4.0 is the absolute floor
pcf_boss_od = 4.4;
pcf_pilot_d = 1.8;
// M2×5 (decided 2026-08-02): 5 − 1.6 board = 3.4 thread + 0.4 tip room —
// pilot pierces 1.0 into the plate web (top face intact; shelf assert guards)
pcf_pilot_depth = 3.8;
// KS-33 under-plate centre pillar ø (gateron-ks-33.md §under-plate) — used
// for the board↔pillar lateral-clearance invariant
ks33_pillar_d = 4.9;

/* [Wire channels — octaves-plan §4] */
// snap-in slot for the 1.4 mm copper bus wire: neck grips, body holds
chan_w = 1.6;
chan_neck = 1.2;
chan_neck_h = 0.4;
chan_depth = 1.6;
// trunk channel Y — runs in the solid web between naturals and sharps pockets
trunk_y = 14;

/* [Render] */
// ghost cap footprints for visual fit check
show_caps = false;
// ghost PCF8575 board at standoff height
show_pcf = false;
// cutaway through the front (naturals) row
section = false;
// cutaway through the bay-1 boss axes (shows pilot holes + standoff)
section_bay = false;
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

// ---- PCF8575 placement: VERTICAL in the D#↔F# bay (derive, per octave) ----
// module centre x = bay centre (midpoint of the flanking sharps D# and F#);
// long axis along Y, the I2C-header end (corner hole pair) NORTH on the tab
function oct_sharp_x(o, i) = key_x(shift_key(oct_sharps[i], o));
function pcf_cx(o) = (oct_sharp_x(o, 1) + oct_sharp_x(o, 2)) / 2; // D#|F#
// hole pattern in the module frame (corner/header pair at -X, true drill
// pitch 28.3), rotated -90° so the header end maps to +Y = north
pcf_rot = -90;
function rot2(p, a) = [p[0]*cos(a) - p[1]*sin(a), p[0]*sin(a) + p[1]*cos(a)];
pcf_holes_local = [
  [-pcf_hole_pitch/2, -pcf_span_a/2],
  [-pcf_hole_pitch/2,  pcf_span_a/2],
  [ pcf_hole_pitch/2, -pcf_span_b/2],
  [ pcf_hole_pitch/2,  pcf_span_b/2],
];
function pcf_bosses(o) = [ for (h = pcf_holes_local) [pcf_cx(o), pcf_cy] + rot2(h, pcf_rot) ];
all_bosses = [ for (o = [0:octaves-1], b = pcf_bosses(o)) b ];

// ---- back tab (per octave): seat for bosses overhanging the back edge ----
function pcf_tab_bosses(o) = [ for (b = pcf_bosses(o)) if (b[1] + pcf_boss_od/2 > cymax - eps) b ];
function tab_x0(o) = min([ for (b = pcf_tab_bosses(o)) b[0] ]) - pcf_boss_od/2 - margin;
function tab_x1(o) = max([ for (b = pcf_tab_bosses(o)) b[0] ]) + pcf_boss_od/2 + margin;
function tab_y1(o) = max([ for (b = pcf_tab_bosses(o)) b[1] ]) + pcf_boss_od/2 + margin;

// accessors for assembly.scad
function pcf_centre(o) = [pcf_cx(o), pcf_cy];
function pcf_z() = -pcf_standoff - pcf_board_t; // bare (solder) face sits here

echo(str(len(keys), " keys · plate ", plate_w, " × ", plate_d, " × ", thick, " mm · pitch ", pitch));
echo(str(octaves, " PCF8575 module(s) · bosses ", all_bosses, " · under-plate stack ", pcf_standoff + pcf_board_t, " mm"));
for (o = [0:octaves-1])
  if (len(pcf_tab_bosses(o)) > 0)
    echo(str("octave ", o, " back tab: x [", tab_x0(o), ", ", tab_x1(o), "] w ", tab_x1(o) - tab_x0(o), " · reaches y ", tab_y1(o), " (+", tab_y1(o) - cymax, " past the back edge)"));
assert(hole_b > hole_t, "body pocket must be wider than the clip cutout");
assert(thick > shelf, "plate must be thicker than the clip shelf");
assert(2 * edge_chamfer < thick - shelf, "chamfers would eat through the walls");
// PCF mount invariants (vertical placement)
assert(pcf_pilot_depth - pcf_standoff <= thick - shelf, "pilot must not reach the clip shelf");
assert(pcf_cy - pcf_hole_pitch/2 - pcf_boss_od/2 >= hole_b/2, "south boss pair hits the naturals pockets");
assert(pcf_cy + pcf_hole_pitch/2 - pcf_boss_od/2 >= key_y(oct_sharps[1]) + hole_b/2, "north boss pair clips the sharps pockets");
assert(oct_sharp_x(0, 2) - pcf_cx(0) - ks33_pillar_d/2 >= pcf_board_w/2, "board hits the D#/F# pillar");
assert(pcf_cy - pcf_board_l/2 >= hole_b/2 - eps, "board south edge reaches under the naturals row");
// channel invariants — DISABLED while repositioning the PCF (2026-08-02);
// re-enable together with wire_channels() in plate()
// assert(thick - chan_depth >= 0.8 - eps, "channel roof thinner than 2 layers");
// assert(chan_neck < chan_w, "neck must be narrower than the channel body");
// assert(trunk_y - chan_w/2 > hole_b/2 && trunk_y + chan_w/2 < 1.5*pitch - hole_b/2, "trunk leaves the naturals/sharps web");
// assert(trunk_y + chan_w/2 < pcf_cy - pcf_span_a/2 - pcf_boss_od/2, "trunk collides with the corner-pair bosses");

// ---- geometry ----
module plate_outline() {
  translate([(cxmin + cxmax)/2, (cymin + cymax)/2])
    offset(r = prad) square([plate_w - 2*prad, plate_d - 2*prad], center = true);
}

// rounded tab off the back edge seating that octave's overhanging bosses;
// its south side is buried ≥ 2 mm inside the main outline → seamless union.
// corner radius caps at half the rect — offset(r) + square(size − 2r) dies
// silently (negative size) when 2r exceeds a side
module tab_outline(o) {
  y0 = cymax - 2;
  w = tab_x1(o) - tab_x0(o);
  d = tab_y1(o) - y0;
  r = min(prad, w/2 - eps, d/2 - eps);
  assert(w > 0 && d > 0, "degenerate back tab");
  translate([(tab_x0(o) + tab_x1(o))/2, (y0 + tab_y1(o))/2])
    offset(r = r) square([w - 2*r, d - 2*r], center = true);
}

// straight walls, 45° chamfer collapsing to an inset footprint at BOTH faces.
// hull() only chamfers CONVEX outlines — so each convex piece (main plate,
// tabs) is chamfered separately and unioned; the seam hides inside the solid
module chamfered_prism() {
  hull() {
    translate([0, 0, edge_chamfer]) linear_extrude(thick - 2*edge_chamfer) children();
    linear_extrude(eps) offset(delta = -edge_chamfer) children();
    translate([0, 0, thick - eps]) linear_extrude(eps) offset(delta = -edge_chamfer) children();
  }
}

module plate_body() {
  chamfered_prism() plate_outline();
  for (o = [0:octaves-1])
    if (len(pcf_tab_bosses(o)) > 0) chamfered_prism() tab_outline(o);
}

// stepped cutout (§7): clip shelf on top, widened pocket below so the plate
// bottom sits flush with the switch base
module ks33_cutout() {
  module rsq(sz) offset(r = hole_r) square(sz - 2*hole_r, center = true);
  translate([0, 0, -0.1]) linear_extrude(thick - shelf + 0.1) rsq(hole_b + kerf);
  translate([0, 0, thick - shelf - 0.2]) linear_extrude(shelf + 0.4) rsq(hole_t + kerf);
}

// ---- PCF8575 bosses (additive) + M2 pilot holes (subtractive) ----
module pcf_boss_posts() {
  for (b = all_bosses)
    translate([b[0], b[1], -pcf_standoff]) cylinder(h = pcf_standoff + eps, d = pcf_boss_od);
}

module pcf_pilot_holes() {
  for (b = all_bosses)
    translate([b[0], b[1], -pcf_standoff - eps]) cylinder(h = pcf_pilot_depth + eps, d = pcf_pilot_d);
}

// ---- wire channels (underside, octaves-plan §4) ----
// snap-in cross-section, z up from the plate bottom: neck (grips the 1.4 wire),
// 45° widening, straight body — printable in the flipped orientation
module chan_profile() {
  polygon([
    [-chan_neck/2, -eps], [chan_neck/2, -eps],
    [chan_neck/2, chan_neck_h],
    [chan_w/2, chan_neck_h + (chan_w - chan_neck)/2],
    [chan_w/2, chan_depth], [-chan_w/2, chan_depth],
    [-chan_neck/2, chan_neck_h + (chan_w - chan_neck)/2],
    [-chan_neck/2, chan_neck_h],
  ]);
}

module chan_x(x0, x1, y) { // along X
  translate([x0, y, 0]) rotate([90, 0, 90]) linear_extrude(x1 - x0) chan_profile();
}

module chan_y(y0, y1, x) { // along Y
  translate([x, y1, 0]) rotate([90, 0, 0]) linear_extrude(y1 - y0) chan_profile();
}

// trunk in the naturals↔sharps web + one spur per switch, web-only by
// construction (spurs stop at the pocket edge; the pocket itself is open)
module wire_channels() {
  xs = [ for (k = keys) key_x(k) ];
  chan_x(min(xs) - chan_w/2, max(xs) + chan_w/2, trunk_y);
  for (k = keys)
    if (k[1] == 2) chan_y(key_y(k) + hole_b/2 - eps, trunk_y, key_x(k)); // naturals: pocket → trunk
    else chan_y(trunk_y, key_y(k) - hole_b/2 + eps, key_x(k));           // sharps: trunk → pocket
}

module plate() {
  difference() {
    union() {
      plate_body();
      pcf_boss_posts();
    }
    for (k = keys) translate([key_x(k), key_y(k)]) ks33_cutout();
    pcf_pilot_holes();
    // wire_channels(); // DISABLED while repositioning the PCF (2026-08-02)
  }
}

// ghost caps float 2.0 above the plate (switch stem height, §7)
module cap_ghosts() {
  for (k = keys)
    %translate([key_x(k), key_y(k), thick + 2])
      linear_extrude(5.5) offset(r = 1.5) square([key_w(k) - 3, key_d(k) - 3], center = true);
}

// ghost PCF8575 board hanging on the bosses, component side toward the plate
module pcf_ghosts() {
  for (o = [0:octaves-1])
    %translate([pcf_cx(o), pcf_cy, pcf_z()])
      rotate([0, 0, pcf_rot])
        linear_extrude(pcf_board_t)
          square([pcf_board_l, pcf_board_w], center = true);
}

module rendered_plate() {
  plate();
  if (show_caps) cap_ghosts();
  if (show_pcf) pcf_ghosts();
}

if (section)
  intersection() {
    rendered_plate();
    translate([0, -25, thick/2]) cube([plate_w + 20, 50, thick + 30], center = true);
  }
else if (section_bay)
  intersection() {
    rendered_plate();
    translate([pcf_cx(0), pcf_cy, 0]) cube([pcf_board_l + 20, pcf_board_l + 20, 30], center = true);
  }
else
  rendered_plate();
