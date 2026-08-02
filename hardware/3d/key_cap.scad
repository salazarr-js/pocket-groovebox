// key_cap.scad — OP-1-style directional keycap family for the Gateron KS-33 (MX stem).
// Spec: hardware/3d/octaves-plan.md §7 (cap params — the single source of values);
// stem measurements: docs/hardware/modules/gateron-ks-33.md (official drawing + vernier).
// v1.0 family (12 caps): 7× natural 1u×2u · 4× sharp 1.5u×1u · 1× G# 1u×1u.
//
// Profile — DIRECTIONAL, OEM-style (decided 2026-08-01): the deep bevel marks
// the touch side, per key: naturals SOUTH (front) · C#/F# WEST · D#/A# EAST ·
// G# ALL sides (under test — fallback: rest value). Other sides get the light
// bevel. Design values sit on a 0.4 mm grid (2026-08-01 polish); FIT values
// (mount/bore) are calibrated or official — NEVER snapped to the grid.
//
// Cross bore — per-arm. Official Gateron LP drawing (vernier-confirmed): stem
// cross 4.00±0.05, arms 1.10±0.04 vertical / 1.23±0.04 horizontal, collar
// ø5.70±0.05, tower 2.95. Slots default to 1.28 × 1.28 — the PRINT-VALIDATED
// geometry (PLA/PETG, 0.2 & 0.4 nozzles): thin 1.10 + 0.18, wide 1.23 + 0.05.
// Equal slots → cap orientation stays free (no LED-north constraint). Bore
// length 4.1 (= span + 0.1) and R0.3 inner-corner fillets follow the official
// Cherry keycap spec; the Cherry 1.17 symmetric slot is for injection POM/ABS
// (interference grip) — too tight for stiff FDM PLA, do not adopt it. Cherry
// socket depths (3.6/3.8 min) are full-height MX — the KS-33 tower is only
// 2.95, so socket_depth stays 2.8.
//
// Print orientation: TOP FACE DOWN — cosmetic face on the (textured) build
// plate; walls, post and cavity ceiling all build support-free. bottom_chamfer
// (skirt + post foot) covers the skirt-down orientation too, and doubles as a
// lead-in over the switch collar.

/* [Part] */
// what to render/export: family = one of each PRINTED part. sharp15 covers both
// C#/F# and (rotated 180° at install) D#/A# — the N/S-symmetric profile makes
// W/E the same piece; assembly.scad still previews the "E" orientation via dir.
part = "family"; // [family, natural, sharp15, sharp1]

/* [Size] */
// cap unit (mm) — fixed by the design; pitch = unit + gap lives in plate.scad
unit_mm = 18;
cap_height = 5.6;

/* [Directional profile — 0.4 grid] */
// deep bevel = the touch side (naturals S, C#/F# W, D#/A# E) — sharps stay
// N/S symmetric ("centred"), only left/right varies per key (closed 2026-08-01)
bevel_main = 2.4;
// light bevel on the remaining sides; G# (1u×1u) uses this on ALL sides
bevel_rest = 1.2;

/* [Profile — tuned look, §7] */
// vertical rim before the bevel starts
base_rim_height = 1.2;
// circular roundover into the flat top (all four edges); MUST stay < top_thickness
top_edge_round = 1.2;
base_corner_r = 1.6;
// = 2× base_corner_r
top_corner_r = 3.2;

/* [Shell — hollow, open bottom] */
wall_thickness = 1.2;
// cavity ceiling; must exceed top_edge_round (or the roundover eats the ceiling edge)
top_thickness = 1.6;
// cavity corner radius (= base_corner_r, matching the outer look; corners get
// slightly thicker than the flats — fine, adds strength)
cavity_corner_r = 1.6;
// 45° chamfer on the bottom edges of skirt (outer + inner) AND post foot (outer)
// — counters elephant foot if printed skirt-down; min ≈ first layer (0.2),
// recommended 0.3–0.5 → 0.4 on the grid
bottom_chamfer = 0.4;

/* [Mount — official Gateron LP drawing, vernier-confirmed — NOT on the 0.4 grid] */
// cap post ø (official cap spec 5.5±0.05; switch collar is ø5.70)
post_diameter = 5.5;
// stem cross envelope 4.00±0.05
cross_span = 4.0;
// stem arm widths ±0.04: wide arm horizontal (LED-window side)
cross_arm_wide = 1.23;
cross_arm_thin = 1.10;
// KS-33 mount tower is 2.95 — socket must stay shorter (NOT Cherry's 3.6/3.8)
socket_depth = 2.8;

/* [Print fit — calibrated, NOT on the 0.4 grid] */
// thin arm clearance — print-validated 0.18 FDM (0.2 & 0.4 nozzles, PLA/PETG); resin 0.05
stem_clearance = 0.18;
// wide arm clearance — 0.05 → slot 1.28 = the print-validated grip fit;
// unequal slots would force LED-north orientation (echo warns)
stem_clearance_wide = 0.05;
// bore length clearance → 4.1 total (official Cherry cap spec 4.1+0.05)
span_clearance = 0.1;
// inner-corner fillet of the cross (official cap spec; avoids FDM corner cracks)
cross_corner_r = 0.3;
// 45° flared lead-in at the bore mouth (0.2 = the official cap spec value)
mouth_flare = 0.2;
// split the post through the wide slot so the cross flexes instead of cracking
split_post = false;

/* [Render] */
// cutaway through the cross bore
section = false;
$fn = $preview ? 32 : 96;

eps = 0.01;
cavity_top = cap_height - top_thickness;
bevel_start = base_rim_height;
bevel_end = cap_height - top_edge_round;
slot_wide = cross_arm_wide + stem_clearance_wide; // x-slot width (in y)
slot_thin = cross_arm_thin + stem_clearance;      // y-slot width (in x)
bore_span = cross_span + span_clearance;          // slot length, both axes

// per-key insets [N, E, S, W] from the touch direction;
// "ALL" (G#) = the light bevel all around — no deep side (decided 2026-08-01)
function cap_insets(dir) =
  dir == "S"   ? [bevel_rest, bevel_rest, bevel_main, bevel_rest] :
  dir == "W"   ? [bevel_rest, bevel_rest, bevel_rest, bevel_main] :
  dir == "E"   ? [bevel_rest, bevel_main, bevel_rest, bevel_rest] :
                 [bevel_rest, bevel_rest, bevel_rest, bevel_rest];

bore_wall = post_diameter/2 - sqrt(pow(bore_span/2, 2) + pow(max(slot_wide, slot_thin)/2, 2));
echo(str("slots: wide(horiz) ", slot_wide, " · thin(vert) ", slot_thin,
         slot_wide == slot_thin ? " — EQUAL (orientation-free, = validated v0)" : " — ASYMMETRIC (mount LED-north!)"));
echo(str("bore ", bore_span, " long · post wall at slot ends ", bore_wall,
         " mm · bevels main/rest ", bevel_main, "/", bevel_rest));
assert(bore_wall > 0.4, "cross bore leaves too little post wall");
assert(cavity_top < bevel_end, "cavity must end below the top roundover: top_thickness must exceed top_edge_round");
assert(socket_depth < cavity_top, "cross bore would pierce the cavity ceiling");
assert(mouth_flare < socket_depth, "flare deeper than the socket");
assert(bottom_chamfer < base_rim_height, "chamfer taller than the rim");
assert(2 * bottom_chamfer < wall_thickness, "chamfers meet inside the wall");

module rsq(w, d, r) offset(r = r) square([w - 2*r, d - 2*r], center = true);

// rectangle with independent per-side insets (from the w×d centred footprint)
module arect(w, d, ins, r) {
  xw = w - ins[1] - ins[3];
  yd = d - ins[0] - ins[2];
  translate([(ins[3] - ins[1])/2, (ins[2] - ins[0])/2]) rsq(xw, yd, r);
}

// bottom chamfer → vertical rim → per-side bevel → roundover on all edges.
module cap_outer(w, d, ins) {
  hull() {
    linear_extrude(eps) offset(delta = -bottom_chamfer) rsq(w, d, base_corner_r);
    translate([0, 0, bottom_chamfer])
      linear_extrude(base_rim_height - bottom_chamfer) rsq(w, d, base_corner_r);
    for (a = [0:15:90]) {
      ro = top_edge_round * (1 - cos(a));
      translate([0, 0, bevel_end + top_edge_round * sin(a) - eps])
        linear_extrude(eps)
          arect(w, d, [for (i = ins) i + ro], top_corner_r);
    }
  }
}

// cavity follows the same per-side geometry, scaled to where it tops out;
// the mouth flares out by bottom_chamfer (inner anti-elephant-foot chamfer)
module cap_cavity(w, d, ins) {
  k = (cavity_top - bevel_start) / (bevel_end - bevel_start);
  union() {
    // mouth chamfer — separate hull, or the convex hull would smear it into a taper
    hull() {
      translate([0, 0, -eps]) linear_extrude(eps)
        offset(delta = bottom_chamfer) rsq(w - 2*wall_thickness, d - 2*wall_thickness, cavity_corner_r);
      translate([0, 0, bottom_chamfer]) linear_extrude(eps)
        rsq(w - 2*wall_thickness, d - 2*wall_thickness, cavity_corner_r);
    }
    hull() {
      translate([0, 0, -eps]) linear_extrude(base_rim_height + eps)
        rsq(w - 2*wall_thickness, d - 2*wall_thickness, cavity_corner_r);
      translate([0, 0, cavity_top - eps]) linear_extrude(eps)
        arect(w - 2*wall_thickness, d - 2*wall_thickness, [for (i = ins) i*k], cavity_corner_r);
    }
  }
}

// "+" profile with R0.3 fillets in the four inner corners (offset-closing)
module cross_profile() {
  offset(r = -cross_corner_r) offset(r = cross_corner_r) union() {
    square([bore_span, slot_wide], center = true); // wide arm horizontal
    square([slot_thin, bore_span], center = true); // thin arm vertical
  }
}

module cross_bore() {
  translate([0, 0, -eps]) linear_extrude(socket_depth + eps) cross_profile();
  translate([0, 0, -eps])                      // 45° flared mouth
    minkowski() {
      linear_extrude(eps) cross_profile();
      cylinder(h = mouth_flare, r1 = mouth_flare, r2 = 0);
    }
  if (split_post)              // full-height wide slot → post halves flex in y
    translate([0, 0, -eps]) linear_extrude(cavity_top + 2*eps)
      square([bore_span, slot_wide], center = true);
}

// central post, foot chamfered on the outside (same 45° anti-elephant-foot)
module post() {
  cylinder(d1 = post_diameter - 2*bottom_chamfer, d2 = post_diameter, h = bottom_chamfer + eps);
  translate([0, 0, bottom_chamfer])
    cylinder(d = post_diameter, h = cavity_top - bottom_chamfer + eps); // fuses into the top
}

module cap(w_u, d_u, dir = "S") {
  w = w_u * unit_mm;
  d = d_u * unit_mm;
  ins = cap_insets(dir);
  top_w = w - ins[1] - ins[3] - 2*top_edge_round;
  top_d = d - ins[0] - ins[2] - 2*top_edge_round;
  assert(top_w > 2*top_corner_r && top_d > 2*top_corner_r,
         str("top face too small for insets/radii (", dir, " on ", w_u, "u×", d_u, "u)"));
  difference() {
    union() {
      difference() { cap_outer(w, d, ins); cap_cavity(w, d, ins); }
      post();
    }
    cross_bore();
  }
}

module picked() {
  if (part == "natural")      cap(1, 2, "S");
  else if (part == "sharp15") cap(1.5, 1, "W");
  else if (part == "sharp1")  cap(1, 1, "ALL");
  else {                      // family: one of each printed part
    translate([-2 * unit_mm, 0]) cap(1, 2, "S");
    translate([0, 0]) cap(1.5, 1, "W");
    translate([1.7 * unit_mm, 0]) cap(1, 1, "ALL");
  }
}

if (section)
  intersection() {
    picked();
    translate([0, 50, 0]) cube([200, 100, 20], center = true); // keep y>0: bore face
  }
else
  picked();
