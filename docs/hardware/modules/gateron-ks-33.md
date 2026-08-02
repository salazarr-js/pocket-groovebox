# Gateron KS-33 Low Profile 2.0 — Silent Brown

Low-profile mechanical switch used for the keyboard keys. Silent tactile, pre-lubed, hot-swappable, with an **MX-style cross stem** and a low-profile body designed for a thin plate. One switch per key; actuated directly by a printed keycap (no lever bridge — see the OP-1-style key direction).

<a href="https://www.gateron.com/products/gateron-ks-33-low-profile-red-silent-20-mechanical-switches-set?VariantsId=11457"><img src="https://ueeshop.ly200-cdn.com/u_file/UPAW/UPAW819/2512/30/products/gateron-ks33-low-profile-20-silent-switch-white-3pi.webp?x-oss-process=image/resize,m_lfit,h_1000,w_1000/quality,q_100" width="300" alt="Gateron KS-33 Low Profile 2.0 Silent switch"></a>

*Gateron KS-33 Low Profile 2.0 Silent (product image; Silent White shown — Silent Brown has the same body with a tactile brown stem).*

| | |
| --- | --- |
| **Role** | Per-key mechanical switch (keyboard) |
| **Model** | Gateron KS-33 Low Profile 2.0, **Silent Brown** |
| **Feel** | Silent **tactile**, **55 gf** (±15 gf) operating force, factory pre-lubed |
| **Travel** | Pre-travel **1.7 mm**, total **3.0 ±0.2 mm** |
| **Stem** | **MX-style cross ("+")**, low-profile — brown, POM |
| **Mount** | 3-pin: 2 metal contact pins + 1 central plastic locating post; plate-mount clips on the sides. 3-pin confirmed on our units by photo + purchase listing (some Gateron marketing text for the range says "5-pin") |
| **Hot-swap** | Yes (designed for low-profile hot-swap sockets) |
| **Plate** | Designed for a **1.2 mm** low-profile plate |
| **LED** | SMD LED slot, south-facing (pairs with [SK6812 MINI-E](sk6812mini-e.md)) |
| **Housing** | PC transparent top, nylon black bottom, POM stem |
| **Compatibility** | NOT interchangeable with standard MX or Kailh low-profile switches (different body) |

*Specs confirmed against Gateron's official product page. Reference (same family): Silent Red = linear 45 gf; Silent Brown = tactile 55 gf. Same 1.7 mm / 3.0 mm travel across the line.*

> ✅ **Dimensional drawing obtained** ("Gateron Low Profile" dimensional drawing, via Ranked) — full mechanical dimensions in the [Mechanical dimensions](#mechanical-dimensions-from-drawing) section below. The drawing is labeled generically "Gateron Low Profile", so a quick **caliper sanity-check** was done. Gateron's own product page and the bundled PDF still publish no drawing.
>
> ⚠️ **The drawing + STEP are the STANDARD KS-33 LP 2.0 — NOT the Silent** we actually have. They differ (stem top detail; Silent body is ~0.5 mm smaller). See the vernier-measured Silent values below — **those are the source of truth for the build.**

## Anatomy / glossary

Terms used throughout the keyboard design:

- **Switch** — the whole component ("shaft"/"eje" in some listings). Houses spring + contacts.
- **Stem** — the moving part that travels up/down when pressed. On the KS-33 it's the **brown "+" cross** (MX-compatible). The keycap mounts here.
- **Keycap / cap** — the plastic top you touch with your finger; clips onto the stem cross. In this project these are **3D-printed, rectangular, OP-1 style**.
- **Nub** — a small protrusion. Not a stock switch part — a feature the *floating-cap* route would have added under the cap to push the stem down (that route was considered and dropped — see "Keycap strategy").
- **Pins** — the 2 metal contacts (electrical) + 1 central plastic post (mechanical alignment) = the "3-pin" spec.

```
   keycap  ──▶ ┌────────┐   rectangular, printed
               └──┐  ┌──┘
   stem "+" ──▶    ╋       MX cross, moves down
               ┌────────┐
   housing ──▶ │  ▢▢▢   │  spring + contacts inside
               └─┬─┬─┬──┘
   2 metal pins ─┘ │ └─── + central plastic post ("3-pin")
```

## Mechanical dimensions (from drawing)

<a href="https://www.amazon.com/dp/B0BW63X3R2"><img src="https://m.media-amazon.com/images/I/61eAfLWQA3L._SL1080_.jpg" width="440" alt="Gateron KS-33 Low Profile dimensional drawing"></a>

*Dimensional drawing (source: Amazon listing). Confirms the side-view stack: 1.20 mm plate notch · 2.50 mm flange-to-base · 14.00 mm cutout · 14.70 mm outer.*

From the Gateron Low Profile dimensional drawing (all mm) — annotated drawing via Ranked, cross-checked against the vernier measurements above. Gateron publishes CAD/3D models and spec sheets at [gateron.com/pages/3d](https://www.gateron.com/pages/3d) and [product-specification](https://www.gateron.com/pages/product-specification).

**Official 3D model (authoritative geometry):** Gateron provides a **STEP (`.stp`)** file — [KS-33 Low Profile 2.0 3D (.rar)](https://www.gateron.com/u_file/2309/25/file/GateronKS-33LowProfile20KS33-20230109KS-33-Y313D.rar) (from the [3D models page](https://www.gateron.com/pages/3d)). Import into **FreeCAD / KiCad** for the exact stem + body geometry — the source of truth for the cap socket, plate cutout, and the KiCad footprint's 3D view. *(Binary — not committed to the repo; download via the link.)*

**Body & mounting**

| Dimension | Value |
| --- | --- |
| Top housing (body) footprint | 15.00 × 15.00 |
| **Plate cutout** (body passes through; 15 mm flange rests on top) | **14.00 ±0.03** |
| **Plate thickness** (clip engages this) | **1.20 ±0.05** |
| Overall height | ~12.15 (+0.20/−0) |

**Stem (keycap mount)** — MX-style cross, but model printed caps to these *exact* numbers, not generic MX:

| Dimension | Value |
| --- | --- |
| Cross envelope | 4.00 × 4.00 (±0.05) |
| Cross arm widths | 1.10 ±0.04 (vertical) and 1.23 ±0.04 (horizontal, LED side) — *earlier docs misread the wide arm as 1.28; corrected 2026-08-01 against the drawing + vernier (1.1 × 1.2)* |
| Stem collar diameter | ø5.70 ±0.05 |
| Keycap-mount tower height | **2.95 mm** — sliced from the STEP; cross + collar + boss all reach the same top plane (z=12.15). **This caps the cap socket depth.** |

> **The keycap mount is standard MX.** Gateron: *"MX-styled stem with a circular dust wall"*, compatible with standard MX keycaps ([gateron.co](https://www.gateron.co/products/gateron-low-profile-mechanical-switch-set)). So the **housing/boss shape (square vs rectangular) is irrelevant to the cap** — the cap only engages the **cross ("+") + the circular collar**, and both are standard (**cross 4.0 × 1.1, collar ø5.7**). Verified by slicing the official STEP: cross **4.00 × 1.11** arm, collar **ø5.68**. The *one* non-standard trait is **low profile** — the mount tower is only **~2.95 mm** tall, so the cap socket must be shorter than the full-MX ~4 mm.

**Bottom / PCB footprint (hot-swap)** — only relevant if a PCB version happens (deferred; build 1.0 is hand-wired, no PCB). Transcribe directly into the PCB footprint:

- Central pole at center, ø ~4.80–5.05 (minor rev variation between drawing sheets).
- Metal contact pins + RGB LED slot positioned by offsets: 5.00, 3.20, 2.25, 5.75, 4.70, 2.60, 4.40 (from center).
- RGB (SMD LED) slot on the **south** side — sets where the [SK6812 MINI-E](sk6812mini-e.md) sits relative to switch center.

**Under-plate protrusion (sliced from the official STEP, 2026-08-02)** — what hangs below the 2.4 mm stepped plate when the switch is seated (flange on plate top, below-flange body 2.5 tall → housing base ≈ 0.1 proud of the plate bottom):

| Feature | Below housing base | Below plate bottom |
| --- | --- | --- |
| Bottom centre pillar (ø4.9) | 3.3 | **3.4** — the deepest hard feature |
| Metal contact pins (tips) | 2.6 | **2.7** (+ solder when hand-wired) |

**Pin positions (sliced from the STEP, 2026-08-02):** both pins sit in the switch's **south half** — (−4.4, −4.7) and (2.6, −5.75) from the switch centre (x = east, y = north). A switch can be rotated in the square plate cutout to point its pins away from whatever runs underneath.

**PCF8575 mount consequence (settled 2026-08-02):** the module mounts **vertically in the D#↔F# bay**, clear of every pillar — so the standoff in `plate.scad` dropped from 4.0 (old pillar floor) to **2.8** (new floor = the module's right-angle I2C header body 2.5 + 0.3 air). **Assembly rule: D# turns −90° (pins WEST) and F# +90° (pins EAST)** so their pins + solder point away from the board.

**Layout implication:** body is 15.00 mm and the natural (1u) pitch is 18.5 mm (18 mm cap + 0.5 mm gap) → **3.5 mm between switch bodies** (1.75 mm/side) and **4.5 mm of plate web between 14 mm cutouts**. Tight but workable. The 1.5u sharps (27 mm caps) sit on wider spacings, so their plate webs differ — see [`hardware/3d/octaves-plan.md`](../../../hardware/3d/octaves-plan.md).

### Silent variant — vernier measurements (SOURCE OF TRUTH)

The STEP/drawing above are the **standard** KS-33 LP 2.0. Our actual switch is the **Silent Brown**, which is slightly different. Measured on the physical Silent switch — **use these for CAD**:

| Feature | Silent (vernier) | Standard (model) |
|---|---|---|
| Outer flange — widest, rests on plate | **14.5 mm** | 15.0 |
| Below-flange body — passes through plate | **13.9 × 13.9 mm** | ~14 |
| Housing height (excl. bottom pillar + stem) | **5.2 mm** | — |
| Bottom centre round pillar (ø) | **4.9 mm** | ~4.8–5.05 |
| Stem top (the moving part, rounded-rectangle) | **6.4 × 6.2 mm** | round-ish |
| Cross arm width | **~1.0 mm** (re-measured: 1.1 × 1.2) | 1.10 / 1.23 |
| Stem top difference | clean cross on rounded base | cross + centre detail |

**For CAD:**
- **Plate cutout:** target **~14.0 mm** (below-flange 13.9 passes; flange 14.5 rests on top). Dial exactly on the single-key test print.
- **Plate thickness — DECIDED: 2.4 mm stepped plate, no PCB underneath.** The keyboard is **plate-mount, hand-wired** to the PCF8575 — there is no PCB, so the plate alone carries the switches and the clips do all the retention (nothing rests on a board below). The **1.2 mm clip shelf is the critical reference**; the rest is structural:
  - **Top 1.2 mm:** 14.0 mm hole = the **clip shelf** (body passes, clips hook under this ledge — matches the switch's 1.2 mm plate spec).
  - **Lower 1.2 mm:** widen to **15.2 mm** to clear the 15 mm body and the sprung-out clips (~14.7 mm hooked). Total 2.4 mm prints far sturdier than a bare 1.2 mm plate.
  - These values are **print-validated** (kerf 0.0, PLA/PETG, multiple print services) — see [`hardware/3d/octaves-plan.md`](../../../hardware/3d/octaves-plan.md) §2 (calibrated values) and §5–6 (rebuild file structure + enclosure context). Switch pins are hand-wired point-to-point below the plate; wire channels in the plate underside route the runs to the PCF8575, which mounts into the plate's sharps-row bays.
  - Per-key SK6812 LEDs are **deferred** — they require a PCB version of the keyboard; build 1.0 has no LEDs, so the plate needs no LED provisions.
- **Cap cross-socket (finalised 2026-08-01 — source of truth: `hardware/3d/key_cap.scad`):** standard MX cross-in-cylinder mount — **post Ø5.5** (matches collar ø5.7), **socket depth 2.8 mm** (tower is 2.95 — do NOT adopt Cherry's 3.6/3.8 full-MX depths). Bore is **per-arm**: stem arms 1.10 (vertical) / 1.23 (horizontal) + clearances **0.18 / 0.05** → **both slots 1.28 mm** — exactly the print-validated fit (PLA/PETG, 0.2 & 0.4 nozzles), and equal slots keep cap orientation free (no LED-north constraint). Bore length **4.1** (stem 4.0 + 0.1, official Cherry cap spec) with **R0.3 inner-corner fillets** (official cap spec; avoids FDM corner cracks). The common symmetric **1.17** slot is for injection POM/ABS (interference grip) — too tight for stiff FDM PLA. **0.2 mm 45° flared lead-in** at the mouth (official cap spec); optional **split-post** for FDM so the cross flexes instead of cracking.
- The STEP model is a **rough visual reference only** for the Silent — geometry authority is this table.

## Keycap strategy (this project)

**Decided: stem-mounted cap** — printed cap with a cross socket on the MX stem, plus a plate guide rib for anti-rotation; the floating-cap route was considered and dropped ([0002-key-design.md](../../explorations/0002-key-design.md)).

Socket geometry and print-calibrated clearances: [`hardware/3d/octaves-plan.md`](../../../hardware/3d/octaves-plan.md) §7.

## Wiring (to the keyboard I2C expanders)

Direct wiring, no matrix, no diodes:

- One leg of each switch → one input pin of a [PCF8575](pcf8575.md) I2C expander. The PCF8575 ports are quasi-bidirectional: each pin has a **weak current source to VCC** (not a true internal pull-up) that the switch pulls low — no external pull-ups needed on the P-pins (see [pcf8575.md](pcf8575.md#how-the-quasi-bidirectional-io-works)).
- Other leg → common GND.
- Press = pin reads LOW. Each key is independent (32 inputs across 2× PCF8575), so no ghosting and no diodes needed.

## Sources

- **Official Gateron product page** (spec table — force, travel, materials, pins): https://www.gateron.com/products/gateron-ks-33-low-profile-red-silent-20-mechanical-switches-set?VariantsId=11457
- Gateron KS-33 datasheet index (PDFs per color, no mechanical drawing): https://www.gateron.co/pages/gateron-ks-33-low-profile-2-0-mechanical-switch-datasheet
- Bundled "Product Manual" PDF: generic CE/RoHS safety manual only — **no mechanical data**.
- Keebio product page (secondary): https://keeb.io/products/gateron-ks-33-low-profile-2-0-switches
