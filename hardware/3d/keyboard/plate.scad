// plate.scad — N-octave keyboard plate for Gateron KS-33 low-profile switches.
// Spec: hardware/3d/keyboard/octaves-plan.md — §2 calibrated values (do NOT re-derive),
// §7 layout/cutout snapshots, §8 v1.0 scope (octaves = 1, 12 keys C–B).
// Batch-2 scope (2026-08-02): the plate prints as a COMPLETE keyboard module —
// PCF8575 VERTICAL in the D#↔F# bay (the pillar-free spot), M2×5 bosses:
// south (mid) pair in the naturals↔sharps web, north (header) pair on a
// derived back tab (module dims: docs/hardware/modules/pcf8575.md §Mechanical).
// Module mounts component side + right-angle I2C header toward the plate,
// solder face out (tails point outward, joints accessible).
// ASSEMBLY RULE: D# switch turns -90° (pins WEST) and F# +90° (pins EAST) so
// their pins point away from the board (pins sit in the switch's south half).
// Wire channels: routed 1:1 from hardware/3d/keyboard/wiring-plan.svg (FROZEN
// 2026-08-03) — signal lanes in the naturals↔sharps web at the EXACT PCF pin
// heights (Δ2.54, dead-straight entry into every pin, both rows), pin map
// C→P04 D→P05 E→P06 F→P07 C#→P02 D#→P01 · A→P10 B→P11 A#→P12 G#→P13 F#→P15
// G→P17 (free P00·P03·P14·P16; A#/G# switch pins inverted: signal = east
// pin). GND = two chains per octave (west: row GND → D# → high C#–D# passage
// → Z-descent in the C#–C web → C; east: row GND → F# → S-drop in the F#–G#
// web → G#/A# pin-height passages → side-drop column → B) + ONE gutter
// across the front apron whose ends curl 90° north into the C/B GND pins
// and which reaches every other natural's GND pin through a spur at the pin
// x — double width at D–A (the wire enters AND leaves, two cables side by
// side, a T with both 90° sweeps). Two under-board runs: G→P17 squeezed
// between the south bosses, F→P07 climbing straight into its pin after
// crossing the E pocket at the pillar limit. Every direction change is a
// swept chan_bend arc (r 1.2 — the 1.4 wire takes it). D#→P01 and F#→P15
// are pure air; under pockets and over the board the wire flies free.
// Feet (dev aid, `feet` flag): the plate stands flat with switches + PCF +
// header mounted — turn off at shell-merge time.
// Ghosts: the visualization modules (parametric KS-33 / PCF board / I2C
// header + opt-in vendor-mesh imports) are DEFINED here (they read this
// file's params) but COMPOSED in assembly.scad — that's where the show_*
// flags live. This file renders the printable plate only.
// No M2 plate↔shell screws yet — batch 3.
// Print orientation: modeled keyboard-face UP; flip 180° about Y in the slicer
// (REQUIRED: each cutout step then rests on solid material — no bridging).
// Edge chamfers are on BOTH faces, so either face down avoids elephant foot.

/* [Layout] */
octaves = 1; // [1:4]

/* [Print fit] */
// cutout kerf — print-validated (PLA/PETG, 0.2 & 0.4 nozzles): 0; + looser / − tighter
kerf = 0.0;

/* [Feet] */
// print the feet (bench aid + integration-deck bosses; turn off at shell-merge time)
feet = true;

// ---- everything below is FROZEN (print-validated or decided — dates on each
// group). Not Customizer knobs: change only with a documented re-decision. ----
/* [Hidden] */
$fn = $preview ? 32 : 96;
eps = 0.01;

// ---- cap grid (frozen 2026-08-01) ----
// cap unit (mm) — fixed by the cap design
cap_1u = 18;
// pitch = cap_1u + cap_gap (18.8: 0.8 = 2×0.4 grid, backed by v0's
// print-validated 1.0 gap; 0.5 was never printed side-by-side)
cap_gap = 0.8;
pitch = cap_1u + cap_gap;

// ---- plate (frozen 2026-08-01/02) ----
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
// concave fillet where the back tab meets the plate edge (2D closing offset)
tab_fillet = 2.4;

// ---- KS-33 cutout (calibrated §2, do NOT re-derive) ----
// clip cutout (top, KS-33 standard)
hole_t = 14.0;
// body pocket (bottom) — INDEPENDENT value, never derive as hole_t + shelf
hole_b = 15.2;
// clip shelf thickness (plate-thickness spec)
shelf = 1.2;
hole_r = 0.5;
// elephant-foot relief on the TOP-face hole rim (the face the print starts
// on in the flipped orientation): a 45° chamfer, hole_chamfer wide AND tall.
// fdm-tolerances prescribes 0.3, but the flange seat caps the lateral relief:
// the Silent flange is 14.5 on a 14.0 hole → 0.25/side of seat, so 0.15
// (keeping 0.10/side) is the max. This 45° × 0.15 relief is what batch 2
// printed. 0 = the exact print-validated geometry (slicer compensation)
hole_chamfer = 0.15;
// Silent flange width (vernier, gateron-ks-33.md) — caps hole_chamfer
ks33_flange_silent = 14.5;
// 45° edge break on the pocket rims of the BOTTOM (channel) face — same
// language as chan_rim; chamfer over fillet (2026-08-03): at 0.3 with 0.2
// layers both print identically, and the whole file speaks 45°. No seat
// constraint on this face (the flange seats on the top face only)
hole_rim = 0.3;

// ---- PCF8575 mount (measured, pcf8575.md §Mechanical; settled 2026-08-02) ----
// board outline (vernier 2026-08-01); dummy/ghost only, bosses anchor on holes
pcf_board_l = 32.1;
pcf_board_w = 21.3;
pcf_board_t = 1.6;
// mounting-hole pitch along the board's long axis (pitch-true PCB drill)
pcf_hole_pitch = 28.3;
// hole-pair spans across the board: corner pair (5-pin-header end) / mid pair
pcf_span_a = 17.0;
pcf_span_b = 6.5;
// module centre Y — vertical module, 24.4 (0.4 grid, moved +0.4 north
// 2026-08-02): both boss-pair root coves land fully on solid web — south
// pair clears the naturals pockets AND the wire channels; north pair clears
// the sharps pockets (all four sides asserted)
pcf_cy = 24.4;
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
// hole-pattern centre X of the vendor STL frame (measured in FreeCAD) — only
// used by the opt-in mesh ghost (assembly.scad → show_pcf_mesh)
pcf_stl_pattern_cx = 15.25;

// ---- KS-33 dims (doc-exact, gateron-ks-33.md) — routing/asserts + ghost ----
// flange footprint (rests on the plate top) / below-flange body (through-plate)
ks33_flange = 15.0;
ks33_body = 13.9;
// body height below the flange — base ends 0.1 proud of the plate bottom
ks33_below = 2.5;
// housing above the plate top (5.2 total − 2.5 below the flange)
ks33_housing_h = 2.7;
// keycap-mount tower: footprint + official height (its top = cap socket ref)
ks33_stem_w = 6.4;
ks33_stem_d = 6.2;
ks33_tower_h = 2.95;
// under-plate drops below the housing base: pillar tip → 3.4 / pin tips → 2.7
// below the plate bottom (doc-exact, the numbers the mount design leans on)
ks33_pillar_h = 3.3;
ks33_pin_drop = 2.6;
// contact-pin positions from the switch centre (STEP slice — both SOUTH half)
ks33_pin_pos = [[-4.4, -4.7], [2.6, -5.75]];
// vendor STL frame (ks33-switch.stl): x centred at 60, z0 = pillar tip —
// the flange underside is derived from the doc dims so a re-measure keeps
// the mesh ghost seated right (frame notes: vendor/README.md). Only used
// by the opt-in mesh ghost (assembly.scad → show_switch_mesh)
ks33_stl_cx = 60;
ks33_stl_flange_z = ks33_below + ks33_pillar_h;

// ---- I2C header (2.54 right-angle male on the 5-pin column) — the pin
// ladder (hdr_pitch) also derives the routing lanes; rest is ghost/stack ----
hdr_pitch = 2.54;
hdr_pins = 5; // [1:10]
// pin cross-section (0.64 sq standard)
hdr_pin_sq = 0.64;
// plastic body height/depth (2.5 sq profile standard) = the standoff floor
hdr_body = 2.5;
// horizontal pin length beyond the plastic body
hdr_horiz = 6.0;
// tail protrusion past the solder (bare) face — sets the under-plate
// maximum. 1.0 = TRIMMED after soldering (2026-08-03, was 2.0 uncut: clip
// the tails flush with cutters once the header is soldered)
hdr_tail = 1.0;
// column position (pcf8575.md): holes 2.3 from the 5-pin edge, column 1.2 in
pcf_hole_edge = 2.3;
pcf_hdr_inset = 1.2;

// ---- feet — dev aid AND the module's deck-mount interface (2026-08-03) ----
// height below the plate bottom — must clear the deepest under-plate stack:
// standoff 2.8 + board 1.6 + trimmed header tails 1.0 = 5.4 → 6.0 (0.4
// grid, 0.6 air; lowered from 7.2 with the tail-trim decision 2026-08-03)
foot_h = 6.0;
foot_od = 6.0;
// edge inset for the foot centres — minimum that keeps the root cove on the
// bottom face (≥ od/2 + foot_fillet + edge_chamfer, asserted)
foot_inset = 4.4;
// M2 self-tap pilot up each foot tip (2026-08-03): the feet double as
// mounting bosses — modules screw down onto a common dev-deck floor before
// the final shell exists. Same M2 family as the PCF (loads are compressive;
// ø6 can't take an M3 pilot anyway — the front foot sits 4.2 from the GND
// gutter). Depth 5.2 = the max inside the foot: M2×6 for a 2.4 deck (3.6
// thread) or M2×8 for a 3.2–4.0 deck (4.8, no bottoming). A future
// deck.scad derives its through-holes from plate_feet()
foot_pilot_d = 1.8;
foot_pilot_depth = 5.2;

// ---- posts — shared finish for the PCF bosses + feet ----
// concave root cove where each post meets the plate bottom (root strength:
// self-tap hoop stress on the bosses, lever loads on the feet). Boss coves
// are smaller — their pairs run close to the pockets and the trunk channel,
// and 0.4 is what fits with every cove FULLY on solid web (asserted). A foot
// cove that reaches a pocket cut gets clipped by it — intentional, the cuts
// always win (corner feet near the outermost sharps lose a small bite)
foot_fillet = 0.8;
boss_fillet = 0.4;
// 45° tip chamfer (lead-in/cosmetic; posts print as up-towers — no elephant
// foot on them). On the ø4.4 bosses the remaining ø3.6 flat still seats the
// board around its ø2.0 hole and keeps 0.9 of wall around the ø1.8 pilot
tip_chamfer = 0.4;

// ---- wire channels — routed 1:1 from wiring-plan.svg (FROZEN 2026-08-03) ----
// snug slot for the 24 AWG hookup wire (OD ~1.4): body = wire OD (press
// tight), shallow — the wire sits ~0.2 proud of the plate bottom, it only
// needs a path and grip, not full cover (decided 2026-08-03)
chan_w = 1.4;
chan_neck = 1.2;
// neck band: chan_rim chamfer at the face + a straight grip lip above it
chan_neck_h = 0.4;
chan_depth = 1.2;
// 45° edge break on the slot mouth (the face-side outer vertices) — eases
// the wire in and kills the sharp printed edge; every channel END is a
// rounded (half-revolved) cap, no square terminations (2026-08-03)
chan_rim = 0.3;
// min solid web between channels of DIFFERENT wires (same-wire pieces touch)
chan_wall = 0.8;
// corner radius: every direction change is a swept 90° bend. 1.2 ≈ the wire
// OD — soft hookup wire takes it (2026-08-03: tightened from 2.4 on
// request); auto-capped per corner by the flanking leg lengths
chan_bend = 1.2;
// PCF pin ladder (pcf8575.md §Mechanical): rows 1.86 in from each long edge,
// first hole (GND) 7.2 from the 5-pin edge, then Δ2.54 south. Signal lanes
// sit at the EXACT pin heights (P04–P07 west / P13–P10 east), so every
// channel enters its pin dead straight on both sides
pcf_pin_edge = 7.2;
pcf_pin_row_inset = 1.86;
// gutter offset: this much solid wall between the straight gutter run and
// the naturals pocket edges (the spurs cross it at each GND pin)
gnd_gap = 2.0;
// GND side-drop column east of the A# pocket (x = key B centre + this): the
// east chain curves out of A# and drops to the B pocket, east of every lane
gnd_drop_dx = 4.6;
// the S-drop's top leg runs this much BELOW F#'s GND-pin height (2026-08-03:
// the back foot moved into the F#|G# web — the leg ducks under its keep-out;
// the wire angles gently down from the pin to the mouth in pocket air)
gnd_sdrop_drop = 1.2;
// channels overshoot the PCF board edge by this much (open mouth over the
// 2.8 air gap: the wire leaves the channel and flies to its row hole)
chan_reach = 1.0;
// F→P07 crossing height in the E–F web: the wire then crosses the E pocket
// in air right at the pillar limit (ø4.9/2 + wire) and climbs under the board
f_cross_y = 3.0;
// G→P17 crossing height (F–G web, then air across the F pocket): lines up
// the entry into the under-board climb between the south bosses. Lowered
// 6.0 → 5.2 (2026-08-03: was only 0.9 of web from the pocket top edge, now
// 1.7); floor = the 0.8 wall to F's E–F web notch (f_cross_y) — the climb
// crosses that same web
g_cross_y = 5.2;
// solid wall left between a channel and a PCF boss: 0.25 accepted 2026-08-03
// for the G→P17 squeeze between the south bosses (2.1 gap vs the channel);
// a boss root cove may get locally clipped — cuts win, and the wall to the
// ø1.8 pilot stays ≥ 1.55
chan_boss_wall = 0.25;
// GND spur slot at the D–A naturals: the bus wire enters AND leaves through
// it — two cables side by side (derived, not a knob)
gnd_spur_w = 2 * chan_w;

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
// ASSEMBLY RULE (2026-08-02): pins sit in the switch's south half — D# turns
// -90° (pins WEST) and F# +90° (pins EAST) so both point away from the PCF
// board between them (works at any octave: the %7 strips the octave shift)
function key_rot(k) = k[0] % 7 == 2.25 ? -90 : k[0] % 7 == 3.75 ? 90 : 0;
// touch direction per key (key_cap.scad directional profile, closed
// 2026-08-01): naturals front; sharps N/S symmetric, deep side W (C#/F#)
// or E (D#/A#); G# all-light. Lives HERE beside key_rot so both per-key
// identity switches stay next to the oct_sharps table they encode;
// consumed by assembly.scad (preview only)
function key_dir(k) =
  k[3] == 2 ? "S" :
  let (xu = k[0] % 7)
  xu == 0.75 ? "W" :
  xu == 2.25 ? "E" :
  xu == 3.75 ? "W" :
  xu == 5    ? "ALL" : "E";

keys = layout_keys(octaves);

// accessors for assembly.scad / a future deck.scad (`use <>` imports
// functions, not variables) — the deck derives its screw pattern from these.
// DECK NOTE (2026-08-03): besides the feet, the deck should carry a SUPPORT
// under the PCF module — a block or two bosses aligned with the module's
// south mounting-hole pair (positions = the south entries of
// plate_pcf_bosses()), either as a rest the board sits on or through-screwed
// (longer M2 up through deck boss + board hole into the plate boss pilot).
// Keep it clear of the solder-face wire joints (~2 protrusion) and the
// trimmed header tails (hdr_tail); the solder face sits at plate_pcf_z(),
// i.e. foot_h + pcf_z() = 1.6 above the deck floor at the current values
function plate_octaves() = octaves;
function plate_thick() = thick;
function plate_feet() = feet_pos;
function plate_foot_od() = foot_od;
function plate_foot_h() = foot_h;
function plate_foot_pilot_d() = foot_pilot_d;
function plate_bbox() = [[cxmin, cymin], [cxmax, cymax]];
function plate_pcf_bosses() = all_bosses;
function plate_pcf_pilot_d() = pcf_pilot_d;
function plate_pcf_z() = pcf_z();

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

// internal — the public accessor is plate_pcf_z()
function pcf_z() = -pcf_standoff - pcf_board_t; // bare (solder) face sits here

// ---- feet: 4 corners + per octave (front web centre + F#|G# back web) —
// every centre lands on solid plate bottom (asserted below). The C#|D# back
// foot was dropped 2026-08-03 (the GND high passage owns that web) and the
// back foot moved G#|A# → F#|G# the same day (user request; the S-drop's
// top leg ducks it via gnd_sdrop_drop) — feet are a bench aid and lose to
// routing ----
function oct_centre_x(o) = (o*7 + 3.5 - 3.5*octaves) * pitch;
feet_pos = !feet ? [] : concat(
  [ for (sx = [0, 1], sy = [0, 1])
      [ sx == 0 ? cxmin + foot_inset : cxmax - foot_inset,
        sy == 0 ? cymin + foot_inset : cymax - foot_inset ] ],
  [ for (o = [0:octaves-1]) [oct_centre_x(o), cymin + foot_inset] ],
  [ for (o = [0:octaves-1]) [(oct_sharp_x(o, 2) + oct_sharp_x(o, 3))/2, cymax - foot_inset] ]
);

// ---- wire routes (wiring-plan.svg, FROZEN 2026-08-03): one route = one
// physical wire = a list of channel polylines (disjoint solid-plate pieces);
// a polyline = axis-aligned waypoints, every interior corner swept with a
// chan_bend arc. Rule: naturals signal = WEST pin / GND = EAST pin, except C
// (swapped — the GND Z-descent owns its west side); A#/G# switch pins are
// INVERTED (signal = east pin, GND = west pin) so the east GND chain runs
// dead flat at pin height. Channels exist only in solid plate; pocket and
// over-the-board stretches are open-air wire and carry no geometry here.
pin_wx = ks33_pin_pos[0][0]; // west contact-pin x offset (-4.4)
pin_ex = ks33_pin_pos[1][0]; // east contact-pin x offset (+2.6)
function kxu(o, u) = (u + 7*o - 3.5*octaves) * pitch;
// pin ladder: hole i of either row, north→south (GND=0, VDD=1, P00/P17=2 …
// P07/P10=9) — derived from the measured module, matches the frozen SVG
function pin_y(i) = pcf_cy + pcf_board_l/2 - pcf_pin_edge - i*hdr_pitch;
nat_n = hole_b/2;                        // naturals pocket north edge
nat_s = -hole_b/2;                       // naturals pocket south edge
gnd_line_y = nat_s - gnd_gap - chan_w/2; // gutter centre (derived: wall = gnd_gap)
shp_y = key_y(oct_sharps[0]);            // sharps row centre y
shp_s = shp_y - hole_b/2;                // sharps pocket south edge
shp_n = shp_y + hole_b/2;                // sharps pocket north edge
function wbe(o) = pcf_cx(o) - pcf_board_w/2; // board west/east edges
function ebe(o) = pcf_cx(o) + pcf_board_w/2;
function row_wx(o) = wbe(o) + pcf_pin_row_inset; // P0x pin-row x (west row)
// pin heights the web passages align to (dead-straight entry): D#/F# GND
// pins after their key_rot turns; G#/A# GND = west pin (inverted); C# signal
// = east pin (its channel runs flat at that height through the C#–D# web)
gnd_dsharp_y = shp_y + rot2(ks33_pin_pos[0], key_rot(oct_sharps[1]))[1];
gnd_fsharp_y = shp_y + rot2(ks33_pin_pos[1], key_rot(oct_sharps[2]))[1];
gnd_e_y  = shp_y + ks33_pin_pos[0][1];   // G#↔A# passages (west-pin height)
csharp_y = shp_y + ks33_pin_pos[1][1];   // C# channel (east-pin height)

// naturals/sharps spur into a pin-height lane, lane to the board edge
function lane_w(x, i, o)  = [[[x, nat_n - eps], [x, pin_y(i)], [wbe(o) + chan_reach, pin_y(i)]]];
function lane_e(x, i, o)  = [[[x, nat_n - eps], [x, pin_y(i)], [ebe(o) - chan_reach, pin_y(i)]]];
function lane_es(x, i, o) = [[[x, shp_s + eps], [x, pin_y(i)], [ebe(o) - chan_reach, pin_y(i)]]];

function sig_routes(o) = [
  lane_w(kxu(o, 0.5) + pin_ex, 6, o),   // C → P04 (east pin: the swapped natural)
  lane_w(kxu(o, 1.5) + pin_wx, 7, o),   // D → P05
  lane_w(kxu(o, 2.5) + pin_wx, 8, o),   // E → P06
  // C# → P02: flat at its own pin height through the C#–D# web, then air
  // across the D# pocket to its row hole
  [[[kxu(o, 2.25) - hole_b/2 + eps, csharp_y], [kxu(o, 0.75) + hole_b/2 - eps, csharp_y]]],
  // F → P07: E–F web notch, air across the E pocket at the pillar limit,
  // then a short climb under the board straight into its pin
  [ [[kxu(o, 2.5) + hole_b/2 - eps, f_cross_y], [kxu(o, 3.5) - hole_b/2 + eps, f_cross_y]],
    [[row_wx(o), nat_n - eps], [row_wx(o), pin_y(9)]] ],
  // G → P17: F–G web notch, air across the F pocket, then the under-board
  // climb squeezed between the south bosses up to P17's pin height and east
  // to the F# pocket edge (the last stretch to the pin is over the board)
  [ [[kxu(o, 4.5) - hole_b/2 + eps, g_cross_y], [kxu(o, 3.5) + hole_b/2 - eps, g_cross_y]],
    [[kxu(o, 3.5) - hole_b/2 + eps, g_cross_y], [pcf_cx(o), g_cross_y],
     [pcf_cx(o), pin_y(2)], [kxu(o, 3.75) - hole_b/2 + eps, pin_y(2)]] ],
  lane_e(kxu(o, 5.5) + pin_wx, 9, o),   // A → P10
  lane_e(kxu(o, 6.5) + pin_wx, 8, o),   // B → P11
  lane_es(kxu(o, 6.25) + pin_ex, 7, o), // A# → P12 (inverted: signal = east pin)
  lane_es(kxu(o, 5.0) + pin_ex, 6, o)   // G# → P13 (inverted)
];                                       // D# → P01, F# → P15: pure air

// GND = one wire per octave: two chains + the gutter (common net, no closed
// ring needed — the gutter's E–F stretch is the optional ring closure)
function gnd_route(o) =
  let (mid_y   = (shp_s + nat_n) / 2,               // Z-descent flat, centred between the rows
       sdrop_x = (kxu(o, 3.75) + kxu(o, 5.0)) / 2,  // S-drop, centred in the F#–G# web
       drop_x  = kxu(o, 6.5) + gnd_drop_dx)
  [
    // west chain: row GND → D# (air), high C#–D# passage at D#'s pin height
    [[kxu(o, 2.25) - hole_b/2 + eps, gnd_dsharp_y], [kxu(o, 0.75) + hole_b/2 - eps, gnd_dsharp_y]],
    // west chain: Z-descent in the C#–C web — straight off C#'s GND pin,
    // flat jog, straight down onto C's GND-pin line (both pins visited
    // in-line by the wire inside the pockets)
    [[kxu(o, 0.75) + pin_wx, shp_s + eps], [kxu(o, 0.75) + pin_wx, mid_y],
     [kxu(o, 0.5) + pin_wx, mid_y], [kxu(o, 0.5) + pin_wx, nat_n - eps]],
    // east chain: row GND → F# (air), S-drop from just below F#'s pin height
    // (gnd_sdrop_drop ducks the back foot) down to the west-pin-height passage
    [[kxu(o, 3.75) + hole_b/2 - eps, gnd_fsharp_y - gnd_sdrop_drop],
     [sdrop_x, gnd_fsharp_y - gnd_sdrop_drop],
     [sdrop_x, gnd_e_y], [kxu(o, 5.0) - hole_b/2 + eps, gnd_e_y]],
    // east chain: G#–A# passage at the same height (pins visited in-line)
    [[kxu(o, 5.0) + hole_b/2 - eps, gnd_e_y], [kxu(o, 6.25) - hole_b/2 + eps, gnd_e_y]],
    // east chain: curve out of A# into the side-drop column, down to B
    [[kxu(o, 6.25) + hole_b/2 - eps, gnd_e_y], [drop_x, gnd_e_y], [drop_x, nat_n - eps]],
    // the naturals gutter: one slot across the apron whose ENDS curl 90°
    // north into the C (west pin) / B (east pin) pockets, pointing straight
    // at their GND pins; D–A pins are reached through the gnd_spur tees
    [[kxu(o, 0.5) + pin_wx, nat_s + eps], [kxu(o, 0.5) + pin_wx, gnd_line_y],
     [kxu(o, 6.5) + pin_ex, gnd_line_y], [kxu(o, 6.5) + pin_ex, nat_s + eps]]
  ];

// GND spur tees (D–A): a double-width slot from the gutter to the pocket
// edge at the GND (east) pin x — the bus wire rises to the pin and comes
// back, two cables side by side, each turning through its own 90° sweep.
// ONE datum (leg + [P, u, v, r] arcs) feeds BOTH the clearance asserts
// (via arc_pts) and the printed geometry (module gnd_spur) — they can't
// silently drift apart
function gnd_spur_xs(o) = [ for (u = [1.5, 2.5, 3.5, 4.5, 5.5]) kxu(o, u) + pin_ex ];
function gnd_spur_leg(x) = [[x, gnd_line_y], [x, nat_s + eps], gnd_spur_w/2];
function gnd_spur_arcs(x) = [
  [[x - gnd_spur_w/2 + chan_w/2, gnd_line_y], [1, 0], [0, 1], chan_bend],
  [[x + gnd_spur_w/2 - chan_w/2, gnd_line_y], [0, -1], [1, 0], chan_bend] ];
function gnd_spur_prims(xs) = [
  [ for (x = xs) gnd_spur_leg(x) ],
  [ for (x = xs, a = gnd_spur_arcs(x), s = arc_pts(a[0], a[1], a[2], a[3])) s ] ];

// ---- polyline machinery: corner radii, trimmed legs, arc samples ----
function pl_dirs(p) = [ for (i = [0:len(p) - 2]) (p[i+1] - p[i]) / norm(p[i+1] - p[i]) ];
// per-corner radius: chan_bend, capped by the flanking legs (half a leg
// whose other end also turns — two corners share it)
function pl_rads(p) = len(p) < 3 ? [] :
  [ for (i = [0:1:len(p) - 3])
      min(chan_bend,
          norm(p[i+1] - p[i])   / (i == 0          ? 1 : 2),
          norm(p[i+2] - p[i+1]) / (i == len(p) - 3 ? 1 : 2)) ];
// straight legs, shortened where a corner arc takes over (empty legs dropped)
function pl_legs(p) = let (d = pl_dirs(p), rs = pl_rads(p))
  [ for (i = [0:len(p) - 2])
      let (a = p[i]   + (i > 0          ? d[i] * rs[i-1] : [0, 0]),
           b = p[i+1] - (i < len(p) - 2 ? d[i] * rs[i]   : [0, 0]))
      if (norm(b - a) > eps) [a, b] ];
// arc centreline samples (9 per 90° corner) for the clearance asserts
function arc_pts(P, u, v, r) = let (C = P - u * r + v * r)
  [ for (t = [0:8]) C + r * (-v * cos(t * 11.25) + u * sin(t * 11.25)) ];
function pl_arcs(p) = let (d = pl_dirs(p), rs = pl_rads(p))
  [ for (i = [0:1:len(p) - 3]) arc_pts(p[i+1], d[i], d[i+1], rs[i]) ];

// per-route primitives for the asserts: [trimmed legs, arc sample points];
// the GND route additionally carries its spur-tee slots and sweeps
function route_prims(r) = [
  [ for (pl = r, l = pl_legs(pl)) l ],
  [ for (pl = r, arc = pl_arcs(pl), s = arc) s ] ];
function route_prims_gnd(o) = let (rp = route_prims(oct_gnd_routes[o]), sp = gnd_spur_prims(oct_spur_xs[o]))
  [ concat(rp[0], sp[0]), concat(rp[1], sp[1]) ];
// route data evaluated ONCE per octave (top-level assignments memoize) —
// the asserts, all_polys and wire_channels() all consume these lists
oct_sig_routes = [ for (o = [0:octaves-1]) sig_routes(o) ];
oct_gnd_routes = [ for (o = [0:octaves-1]) gnd_route(o) ];
oct_spur_xs    = [ for (o = [0:octaves-1]) gnd_spur_xs(o) ];
all_route_prims = [ for (o = [0:octaves-1])
  each concat([ for (r = oct_sig_routes[o]) route_prims(r) ], [route_prims_gnd(o)]) ];
all_polys = [ for (o = [0:octaves-1], r = concat(oct_sig_routes[o], [oct_gnd_routes[o]]), pl = r) pl ];

echo(str(len(keys), " keys · plate ", plate_w, " × ", plate_d, " × ", thick, " mm · pitch ", pitch));
echo(str(octaves, " PCF8575 module(s) · bosses ", all_bosses, " · under-plate stack ", pcf_standoff + pcf_board_t, " mm"));
echo(str(len(feet_pos), " feet · h ", foot_h, " · air over the header tails ",
         foot_h - (pcf_standoff + pcf_board_t + hdr_tail), " mm"));
for (o = [0:octaves-1])
  if (len(pcf_tab_bosses(o)) > 0)
    echo(str("octave ", o, " back tab: x [", tab_x0(o), ", ", tab_x1(o), "] w ", tab_x1(o) - tab_x0(o), " · reaches y ", tab_y1(o), " (+", tab_y1(o) - cymax, " past the back edge)"));
assert(hole_b > hole_t, "body pocket must be wider than the clip cutout");
assert(thick > shelf, "plate must be thicker than the clip shelf");
assert(2 * edge_chamfer < thick - shelf, "chamfers would eat through the walls");
assert(hole_t + kerf + 2*hole_chamfer <= ks33_flange_silent - 0.2,
       "hole relief eats the Silent flange seat (14.5 flange on a 14.0 hole)");
assert(hole_chamfer < shelf, "hole relief taller than the clip shelf");
assert((pcf_boss_od - 2*tip_chamfer - pcf_pilot_d)/2 >= 0.8, "boss tip wall under 2 perimeters");
// PCF mount invariants (vertical placement)
assert(pcf_pilot_depth - pcf_standoff <= thick - shelf, "pilot must not reach the clip shelf");
assert(pcf_cy - pcf_hole_pitch/2 - pcf_boss_od/2 - boss_fillet >= hole_b/2,
       "south boss pair (incl. root cove) hits the naturals pockets");
assert(pcf_cy + pcf_hole_pitch/2 - pcf_boss_od/2 - boss_fillet >= key_y(oct_sharps[1]) + hole_b/2,
       "north boss pair (incl. root cove) clips the sharps pockets");
assert(oct_sharp_x(0, 2) - pcf_cx(0) - ks33_pillar_d/2 >= pcf_board_w/2, "board hits the D#/F# pillar");
assert(pcf_cy - pcf_board_l/2 >= hole_b/2 - eps, "board south edge reaches under the naturals row");
// feet invariants — a foot centre must sit on SOLID plate bottom and its
// column must reach the table past everything hanging under the plate
function foot_clear_key(f, k) =
  abs(f[0] - key_x(k)) >= (hole_b + kerf)/2 + foot_od/2 ||
  abs(f[1] - key_y(k)) >= (hole_b + kerf)/2 + foot_od/2;
assert([ for (f = feet_pos, k = keys) if (!foot_clear_key(f, k)) f ] == [],
       "a foot overlaps a switch pocket");
assert([ for (f = feet_pos, b = all_bosses) if (norm(f - b) < (foot_od + pcf_boss_od)/2) f ] == [],
       "a foot overlaps a PCF boss");
assert([ for (f = feet_pos, o = [0:octaves-1])
         if (abs(f[0] - pcf_cx(o)) < foot_od/2 + pcf_board_w/2 &&
             abs(f[1] - pcf_cy)    < foot_od/2 + pcf_board_l/2) f ] == [],
       "a foot lands on the PCF board footprint");
assert(feet_pos == [] || foot_h >= pcf_standoff + pcf_board_t + hdr_tail + 0.4,
       "feet shorter than the under-plate stack (board + header tails)");
assert(foot_inset >= foot_od/2 + foot_fillet + edge_chamfer,
       "foot root cove pokes past the bottom-face edge");
assert(foot_pilot_depth <= foot_h - 0.8, "foot pilot reaches the plate web");
assert((foot_od - 2*tip_chamfer - foot_pilot_d)/2 >= 0.8,
       "foot tip wall around the pilot under 2 perimeters");
// channel invariants — routed per wiring-plan.svg (FROZEN 2026-08-03).
// Geometry helpers: a leg's channel body as a TIGHT rect (transverse ±w/2,
// exact axial ends — the corner arcs curve away from them), rect/rect
// separation, point-to-rect distance. Arcs check via centreline samples
// a leg is [a, b] (half-width chan_w/2) or [a, b, hw] (wide GND spur slot)
function leg_rect(l) = let (v = abs(l[0][0] - l[1][0]) < eps,
                            hw = len(l) > 2 ? l[2] : chan_w/2)
  [min(l[0][0], l[1][0]) - (v ? hw : 0), min(l[0][1], l[1][1]) - (v ? 0 : hw),
   max(l[0][0], l[1][0]) + (v ? hw : 0), max(l[0][1], l[1][1]) + (v ? 0 : hw)];
function rects_apart(a, b, gap) =
  b[0] - a[2] >= gap || a[0] - b[2] >= gap || b[1] - a[3] >= gap || a[1] - b[3] >= gap;
function rect_pt_d(r, p) = norm([max(max(r[0] - p[0], p[0] - r[2]), 0),
                                 max(max(r[1] - p[1], p[1] - r[3]), 0)]);
function routes_apart(A, B) =
  [ for (a = A[0], b = B[0]) if (!rects_apart(leg_rect(a), leg_rect(b), chan_wall - eps)) 1 ] == [] &&
  [ for (a = A[0], s = B[1]) if (rect_pt_d(leg_rect(a), s) < chan_w/2 + chan_wall - eps) 1 ] == [] &&
  [ for (b = B[0], s = A[1]) if (rect_pt_d(leg_rect(b), s) < chan_w/2 + chan_wall - eps) 1 ] == [] &&
  [ for (sa = A[1], sb = B[1]) if (norm(sa - sb) < chan_w + chan_wall - eps) 1 ] == [];
echo(str(len(all_route_prims), " wire routes · ",
         len([ for (rp = all_route_prims, l = rp[0]) l ]), " channel legs · ",
         len([ for (rp = all_route_prims, s = rp[1]) s ]) / 9, " bends"));
echo(str("pin-height lanes y ", [ for (i = [9:-1:6]) pin_y(i) ],
         " · C# ", csharp_y, " · GND passages ", [gnd_dsharp_y, gnd_fsharp_y, gnd_e_y],
         " · gutter y ", gnd_line_y));
assert(thick - chan_depth >= 0.8 - eps, "channel roof thinner than 2 layers");
assert(chan_neck < chan_w, "neck must be narrower than the channel body");
assert(pin_y(9) - chan_w/2 >= nat_n + chan_wall - eps &&
       pin_y(6) + chan_w/2 <= shp_s - chan_wall + eps,
       "pin-height lanes leave the naturals/sharps web");
assert(gnd_line_y - chan_w/2 >= cymin + 1 - eps,
       "GND gutter runs off the front apron");
// web passages must open INTO their flanking pockets: band ⊆ pocket band
assert([ for (y = [csharp_y, gnd_dsharp_y, gnd_fsharp_y - gnd_sdrop_drop, gnd_e_y])
         if (y - chan_w/2 < shp_s - eps || y + chan_w/2 > shp_n + eps) y ] == [],
       "a sharps-web passage leaves the pocket band");
assert([ for (y = [f_cross_y, g_cross_y]) if (abs(y) + chan_w/2 > nat_n + eps) y ] == [],
       "a naturals-web crossing leaves the pocket band");
assert(kxu(0, 6.5) + gnd_drop_dx - chan_w/2 - chan_wall >= kxu(0, 6.25) + hole_b/2 - eps,
       "GND side drop crowds the A# pocket");
assert([ for (i = [0:1:len(all_route_prims) - 2], j = [i+1:1:len(all_route_prims) - 1])
         if (!routes_apart(all_route_prims[i], all_route_prims[j])) [i, j] ] == [],
       "two wire routes run closer than chan_wall");
// chan_boss_wall (0.25) is the accepted G→P17 squeeze: the channel may clip
// a boss root cove, never the boss wall itself
assert([ for (rp = all_route_prims, l = rp[0], b = all_bosses)
         if (rect_pt_d(leg_rect(l), b) < pcf_boss_od/2 + chan_boss_wall - eps) l ] == [] &&
       [ for (rp = all_route_prims, s = rp[1], b = all_bosses)
         if (norm(s - b) < pcf_boss_od/2 + chan_boss_wall + chan_w/2 - eps) s ] == [],
       "a channel cuts into a PCF boss wall");
assert([ for (rp = all_route_prims, l = rp[0], f = feet_pos)
         if (rect_pt_d(leg_rect(l), f) < foot_od/2 + foot_fillet - eps) l ] == [] &&
       [ for (rp = all_route_prims, s = rp[1], f = feet_pos)
         if (norm(s - f) < foot_od/2 + foot_fillet + chan_w/2 - eps) s ] == [],
       "a channel clips a foot (or its root cove)");

// ---- geometry ----
// centred rounded rectangle — THE corner idiom of this file (offset + inset
// square). Degenerate when r exceeds half a side — call sites guard for it
module rrect(w, d, r) offset(r = r) square([w - 2*r, d - 2*r], center = true);

module plate_outline() {
  translate([(cxmin + cxmax)/2, (cymin + cymax)/2]) rrect(plate_w, plate_d, prad);
}

// rounded tab off the back edge seating that octave's overhanging bosses;
// its south side is buried deep enough inside the main outline that the
// exposed corners always reach the full prad (the r cap only guards the
// degenerate rrect case)
module tab_outline(o) {
  ty1 = tab_y1(o);
  y0 = min(cymax - 2, ty1 - 2*prad - 0.4);
  w = tab_x1(o) - tab_x0(o);
  d = ty1 - y0;
  r = min(prad, w/2 - eps, d/2 - eps);
  assert(w > 0 && d > 0, "degenerate back tab");
  translate([(tab_x0(o) + tab_x1(o))/2, (y0 + ty1)/2]) rrect(w, d, r);
}

// plate + tabs as ONE outline: the 2D closing offset (+f then −f) fills the
// concave corners where each tab meets the back edge with a tab_fillet round;
// convex corners keep their own radii (prad survives the round trip)
module full_outline() {
  offset(r = -tab_fillet) offset(r = tab_fillet) {
    plate_outline();
    for (o = [0:octaves-1])
      if (len(pcf_tab_bosses(o)) > 0) tab_outline(o);
  }
}

// straight walls, 45° chamfer collapsing to an inset footprint at BOTH faces.
// minkowski (inset prism × bipyramid) chamfers ANY outline — the old hull()
// trick only worked on convex pieces and left the tab junction unfilleted
module chamfer_tool() {
  $fn = 24; // 0.3 mm tool — facets invisible, keeps minkowski cheap
  translate([0, 0, -edge_chamfer]) cylinder(h = edge_chamfer, d1 = 0, d2 = 2*edge_chamfer);
  cylinder(h = edge_chamfer, d1 = 2*edge_chamfer, d2 = 0);
}

module chamfered_prism() {
  minkowski() {
    translate([0, 0, edge_chamfer])
      linear_extrude(thick - 2*edge_chamfer)
        offset(delta = -edge_chamfer) children();
    chamfer_tool();
  }
}

module plate_body() {
  chamfered_prism() full_outline();
}

// stepped cutout (§7): clip shelf on top, widened pocket below so the plate
// bottom sits flush with the switch base
module ks33_cutout() {
  module rsq(sz) rrect(sz, sz, hole_r);
  translate([0, 0, -0.1]) linear_extrude(thick - shelf + 0.1) rsq(hole_b + kerf);
  translate([0, 0, thick - shelf - 0.2]) linear_extrude(shelf + 0.4) rsq(hole_t + kerf);
  // top-rim elephant-foot chamfer (45°, then straight past the face) — trims
  // only the shelf's top edge; the clip ledge at its bottom stays full
  if (hole_chamfer > 0)
    hull() {
      translate([0, 0, thick - hole_chamfer]) linear_extrude(eps) rsq(hole_t + kerf);
      translate([0, 0, thick]) linear_extrude(0.1) rsq(hole_t + kerf + 2*hole_chamfer);
    }
  // bottom-rim edge break on the channel face (45°, hole_rim)
  if (hole_rim > 0)
    hull() {
      translate([0, 0, -0.1]) linear_extrude(0.1 + eps) rsq(hole_b + kerf + 2*hole_rim);
      translate([0, 0, hole_rim]) linear_extrude(eps) rsq(hole_b + kerf);
    }
}

// ---- posts (additive): shared shape for PCF bosses + feet ----
// concave cove ring where a ø d post meets the plate bottom (z = 0).
// Runs at ambient $fn (96 on render) — heavy for a sub-mm fillet, but any
// $fn cap changes the in-print plate.stl mesh; candidate for the next
// re-export batch, not now
module base_cove(d, r) {
  rotate_extrude()
    translate([d/2, -r])
      difference() {
        square([r, r]);
        translate([r, 0]) circle(r);
      }
}

// post hanging from the plate bottom: chamfered tip + root cove
module post(d, h, cove) {
  translate([0, 0, -h]) {
    cylinder(h = tip_chamfer, d1 = d - 2*tip_chamfer, d2 = d);
    translate([0, 0, tip_chamfer - eps]) cylinder(h = h - tip_chamfer + 2*eps, d = d);
  }
  base_cove(d, cove);
}

module pcf_boss_posts() {
  for (b = all_bosses) translate([b[0], b[1]]) post(pcf_boss_od, pcf_standoff, boss_fillet);
}

module pcf_pilot_holes() {
  for (b = all_bosses)
    translate([b[0], b[1], -pcf_standoff - eps]) cylinder(h = pcf_pilot_depth + eps, d = pcf_pilot_d);
}

module feet_posts() {
  for (f = feet_pos) translate([f[0], f[1]]) post(foot_od, foot_h, foot_fillet);
}

module foot_pilot_holes() {
  for (f = feet_pos)
    translate([f[0], f[1], -foot_h - eps])
      cylinder(h = foot_pilot_depth + eps, d = foot_pilot_d);
}

// ---- wire channels (underside, octaves-plan §4) ----
// snap-in cross-section, z up from the plate bottom: neck (grips the 1.4 wire),
// 45° widening, straight body — printable in the flipped orientation. Wider
// slots (the GND spur tees) pass w with the same neck REDUCTION (w - (chan_w
// - chan_neck)), keeping the grip-lip geometry identical at any width
module chan_profile(w = chan_w, neck = chan_neck) {
  polygon([
    [-neck/2 - chan_rim, -eps], [neck/2 + chan_rim, -eps],
    [neck/2, chan_rim],                       // 45° mouth break
    [neck/2, chan_neck_h],
    [w/2, chan_neck_h + (w - neck)/2],
    [w/2, chan_depth], [-w/2, chan_depth],
    [-neck/2, chan_neck_h + (w - neck)/2],
    [-neck/2, chan_neck_h],
    [-neck/2, chan_rim],
  ]);
}

// rounded end cap: the half profile revolved 180° about the endpoint, on
// the far side of the end plane (d = the leg's outward direction there)
module chan_cap(p, d, hw) {
  w = 2 * hw;
  translate([p[0], p[1], 0])
    rotate([0, 0, atan2(d[1], d[0]) - 90])
      rotate_extrude(angle = 180)
        intersection() {
          chan_profile(w, w - (chan_w - chan_neck));
          translate([0, -1]) square([w + 1, chan_depth + 2]);
        }
}

// straight leg between two points (profile upright, extruded along the leg);
// an optional third element is the half-width (wide GND spur slots)
module chan_leg(l) {
  w = len(l) > 2 ? 2 * l[2] : chan_w;
  d = l[1] - l[0];
  translate([l[0][0], l[0][1], 0])
    rotate([0, 0, atan2(d[1], d[0])])
      rotate([90, 0, 90])
        linear_extrude(norm(d)) chan_profile(w, w - (chan_w - chan_neck));
}

// 90° corner sweep: quarter rotate_extrude of the profile at the arc radius
// (arc centre C; the sweep runs from the incoming leg's end P - u·r to the
// outgoing leg's start P + v·r)
module chan_arc(P, u, v, r) {
  C = P - u * r + v * r;
  aA = atan2(-v[1], -v[0]);
  aB = atan2(u[1], u[0]);
  a0 = abs(((aB - aA + 720) % 360) - 90) < 1 ? aA : aB;
  translate([C[0], C[1], 0]) rotate([0, 0, a0])
    rotate_extrude(angle = 90) translate([r, 0]) chan_profile();
}

module chan_polyline(p) {
  d = pl_dirs(p);
  rs = pl_rads(p);
  for (l = pl_legs(p)) chan_leg(l);
  if (len(p) > 2)
    for (i = [0:len(p) - 3]) chan_arc(p[i+1], d[i], d[i+1], rs[i]);
  chan_cap(p[0], -d[0], chan_w/2);
  chan_cap(p[len(p) - 1], d[len(d) - 1], chan_w/2);
}

// GND spur tee: the wide slot up to the pocket edge + one 90° sweep per
// wire (in from the west gutter run, out to the east one), each on its own
// wire line inside the slot. Rounded cap only at the pocket end — a south
// cap would bite through the gutter's far wall
module gnd_spur(x) {
  chan_leg(gnd_spur_leg(x));
  chan_cap([x, nat_s + eps], [0, 1], gnd_spur_w/2);
  for (a = gnd_spur_arcs(x)) chan_arc(a[0], a[1], a[2], a[3]);
}

module wire_channels() {
  for (pl = all_polys) chan_polyline(pl);
  for (o = [0:octaves-1], x = oct_spur_xs[o]) gnd_spur(x);
}

module plate() {
  difference() {
    union() {
      plate_body();
      pcf_boss_posts();
      feet_posts();
    }
    for (k = keys) translate([key_x(k), key_y(k)]) ks33_cutout();
    pcf_pilot_holes();
    foot_pilot_holes();
    wire_channels();
  }
}

// ---- ghosts (visual only) — DEFINED here (they read this file's params via
// `use <>` closure), COMPOSED in assembly.scad. Nothing below prints. ----

// ghost PCF8575 board hanging on the bosses, component side toward the plate
module pcf_ghosts() {
  for (o = [0:octaves-1])
    %translate([pcf_cx(o), pcf_cy, pcf_z()])
      rotate([0, 0, pcf_rot])
        linear_extrude(pcf_board_t)
          square([pcf_board_l, pcf_board_w], center = true);
}

// parametric KS-33 ghost, switch centre at origin, plate TOP = z0. Under-plate
// features are doc-exact (pillar tip 3.4 / pin tips 2.7 below the plate
// bottom); the above-plate housing is an envelope, not the true shape
module ks33_ghost() {
  module rbox(w, d, h) linear_extrude(h) rrect(w, d, 1);
  rbox(ks33_flange, ks33_flange, 0.8);                                    // flange on the plate top
  translate([0, 0, -ks33_below]) rbox(ks33_body, ks33_body, ks33_below);  // through-plate body
  rbox(ks33_body, ks33_body, ks33_housing_h);                             // upper housing
  translate([0, 0, ks33_housing_h])
    rbox(ks33_stem_w, ks33_stem_d, ks33_tower_h);                        // keycap-mount tower
  translate([0, 0, -ks33_below - ks33_pillar_h])
    cylinder(h = ks33_pillar_h + eps, d = ks33_pillar_d);                 // centre pillar
  for (p = ks33_pin_pos)
    translate([p[0], p[1], -ks33_below - ks33_pin_drop])
      cylinder(h = ks33_pin_drop + eps, d = 1.0);                         // contact pins
}

module switch_ghosts() {
  for (k = keys)
    %translate([key_x(k), key_y(k), thick])
      rotate([0, 0, key_rot(k)])
        ks33_ghost();
}

// right-angle male header at the 5-pin I2C column (visual): soldered on the
// component side (toward the plate), plastic body = the 2.5 standoff floor,
// horizontal legs point north off the header end, tails protrude past the
// solder face — the deepest thing under the plate (standoff + board + tail)
module zbox(x, y, sq, z1, z2) { // pin segment: sq × sq, z from z1 to z2
  translate([x - sq/2, y - sq/2, min(z1, z2)]) cube([sq, sq, abs(z2 - z1)]);
}

module pcf_header_ghosts() {
  hdr_x = -(pcf_hole_pitch/2 + pcf_hole_edge - pcf_hdr_inset); // I2C column (module frame)
  zf = -pcf_standoff;        // component face (board top, on the boss tips)
  zbare = zf - pcf_board_t;  // solder face (outward)
  for (o = [0:octaves-1])
    translate([pcf_cx(o), pcf_cy, 0]) rotate([0, 0, pcf_rot])
      color("gold") {
        translate([hdr_x - hdr_body/2, -hdr_pins*hdr_pitch/2, zf])
          cube([hdr_body, hdr_pins*hdr_pitch, hdr_body]);
        for (i = [0:hdr_pins-1]) {
          y = (i - (hdr_pins - 1)/2) * hdr_pitch;
          // tail: through the board, protruding hdr_tail past the solder face
          zbox(hdr_x, y, hdr_pin_sq, zf + hdr_body/2, zbare - hdr_tail);
          // horizontal leg: out of the body toward the header end (north)
          translate([hdr_x - hdr_body/2 - hdr_horiz, y - hdr_pin_sq/2,
                     zf + hdr_body/2 - hdr_pin_sq/2])
            cube([hdr_horiz + hdr_body/2, hdr_pin_sq, hdr_pin_sq]);
        }
      }
}

// real module mesh (opt-in): hole-pattern centre anchored on the bosses,
// component side toward the plate. Third-party mesh, committed in
// ../vendor/ (provenance: vendor/README.md) — the true component detail
// (this is what caught the SSOP corner vs the ø6 boss ring)
module pcf_mesh_ghosts() {
  for (o = [0:octaves-1])
    translate([pcf_cx(o), pcf_cy, -pcf_standoff])
      rotate([0, 0, pcf_rot])
        translate([-pcf_stl_pattern_cx, 0, 0])
          %import("../vendor/pcf8575-module.stl");
}

// real switch mesh (opt-in): flange underside seated on the plate top.
// ⚠️ the vendor STL is the STANDARD KS-33, not the Silent we build with
// (body ~0.5 wider, stem top differs) — visual only, never a fit reference
module switch_mesh_ghosts() {
  for (k = keys)
    %translate([key_x(k), key_y(k), thick - ks33_stl_flange_z])
      rotate([0, 0, key_rot(k)])
        translate([-ks33_stl_cx, 0, 0])
          import("../vendor/ks33-switch.stl");
}

plate();
