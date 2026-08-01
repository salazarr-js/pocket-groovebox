# FDM fits & tolerances — project reference

Working reference for every printed part in this project. The **calibrated, print-validated values live in [`hardware/3d/octaves-plan.md`](../../hardware/3d/octaves-plan.md) §2 — never re-derive those**; this doc covers the general rules for the fits we haven't calibrated yet (enclosure, inserts, snap-fits). Deep theory lives in the external cad-lab repo (not part of this repo).

## Validated on this project (source: octaves-plan §2)

| Fit | Value | Context |
|---|---|---|
| KS-33 plate cutout kerf | **0.0** | PLA + PETG, 0.2 & 0.4 mm nozzles, 3 print services — fit passed as-modeled |
| Cap stem clearance | **0.18** FDM / 0.05 resin | on cross-arm width only, with 0.5 mm `mouth_flare` 45° lead-in |
| Elephant foot | present on 1 of 3 services | countermeasure: **0.3 mm chamfer** (≈ first layer height + margin; 0.3–0.5 range, dial up only if a printer's foot is worse). On flat plates, chamfer **both faces** so print orientation can't leave a raw edge |

Lesson: kerf on *this* printer chain is near zero — but every new mating geometry still gets a **parameter + test coupon** before the full part (that's how the values above were earned).

## General rules for the next parts

**Clearance fits (hole/slot larger than the inserted part):**

| Fit type | Clearance per side | Use |
|---|---|---|
| Press / snug | 0.0–0.1 mm | parts that must not rattle (switch cutouts — validated at 0 here) |
| Sliding | 0.2–0.3 mm | display/module drop-in pockets, perfboard seats |
| Free / loose | 0.4–0.5 mm | anything hand-assembled blind, cable pass-throughs |

**Holes print undersized** (0.2–0.4 mm on FDM, worse on small diameters): model holes with a diameter parameter and calibrate — never trust nominal. Vertical-axis holes are rounder than horizontal ones; horizontal holes sag at the top.

**M2 fasteners (the project standard):**

- Through-hole clearance: **ø2.2** (validated — octaves-plan screw spec).
- Self-tapping into a printed boss: **ø1.8** pilot, boss wall ≥ 2 mm around the hole.
- **Heat-set insert (enclosure side of the sandwich):** hole per the insert's datasheet — typical M2 short insert wants **ø3.1–3.2**, depth = insert length + ~1 mm; boss OD ≥ insert OD + 2× 1.6 mm wall. Buy the inserts first, then model from the measured spec.

**Snap-fits (the plate wire channels):** slot for the 1.4 mm bus wire starts at **1.4 + kerf** (1.6 initial); for retention, neck the opening to ~**1.2** (≈85% of the captured diameter) and widen inside. Flexing walls ≥ 1.2 mm thick; never put a snap-fit's flex direction across layer lines.

**Printability constraints that shape fits:**

- Overhangs ≤ 45°, bridges ≤ ~20 mm — design pockets stepped (like `ks33_cutout`) so every face rests on solid material in the chosen print orientation.
- State the intended **print orientation** in the part file; fits are orientation-dependent (a slot that's clean printed vertically sags printed flat).
- Walls ≥ 2 perimeters of the nozzle (≥ 0.8 mm at 0.4, ≥ 1.2 mm structural).

**Process:** for each new mating feature → expose a clearance parameter → print a coupon (only the feature, minutes not hours) → lock the value → record it in octaves-plan §2 so it joins the "don't re-derive" table.
