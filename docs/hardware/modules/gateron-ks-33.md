# Gateron KS-33 Low Profile 2.0 — Silent Brown

Low-profile mechanical switch used for the keyboard keys. Silent tactile, pre-lubed, hot-swappable, with an **MX-style cross stem** and a low-profile body designed for a thin plate. One switch per key; actuated directly by a printed keycap (no lever bridge — see the OP-1-style key direction).

<a href="https://www.gateron.com/products/gateron-ks-33-low-profile-red-silent-20-mechanical-switches-set?VariantsId=11457"><img src="https://ueeshop.ly200-cdn.com/u_file/UPAW/UPAW819/2512/30/products/gateron-ks33-low-profile-20-silent-switch-white-3pi.webp?x-oss-process=image/resize,m_lfit,h_1000,w_1000/quality,q_100" width="300" alt="Gateron KS-33 Low Profile 2.0 Silent switch"></a>

*Gateron KS-33 Low Profile 2.0 Silent (product image; Silent White shown — Silent Brown has the same body with a tactile brown stem).*

| | |
| --- | --- |
| **Role** | Per-key mechanical switch (keyboard) |
| **Model** | Gateron KS-33 Low Profile 2.0, **Silent Brown** |
| **Feel** | Silent **tactile**, **55 gf** operating force, factory pre-lubed |
| **Travel** | Pre-travel **1.7 mm**, total **3.0 ±0.2 mm** |
| **Stem** | **MX-style cross ("+")**, low-profile — brown, POM |
| **Mount** | 3-pin: 2 metal contact pins + 1 central plastic locating post; plate-mount clips on the sides |
| **Hot-swap** | Yes (designed for low-profile hot-swap sockets) |
| **Plate** | Designed for a **1.2 mm** low-profile plate |
| **LED** | SMD LED slot, south-facing (pairs with [SK6812 MINI-E](sk6812mini-e.md)) |
| **Housing** | PC transparent top, nylon black bottom, POM stem |
| **Compatibility** | NOT interchangeable with standard MX or Kailh low-profile switches (different body) |

> ✅ **Dimensional drawing obtained** ("Gateron Low Profile" dimensional drawing, via Ranked) — full mechanical dimensions in the [Mechanical dimensions](#mechanical-dimensions-from-drawing) section below. The drawing is labeled generically "Gateron Low Profile", so a quick **caliper sanity-check** was done. Gateron's own product page and the bundled PDF still publish no drawing.
>
> ⚠️ **The drawing + STEP are the STANDARD KS-33 LP 2.0 — NOT the Silent** we actually have. They differ (stem top detail; Silent body is ~0.5 mm smaller). See the vernier-measured Silent values below — **those are the source of truth for the build.**

## Anatomy / glossary

Terms used throughout the keyboard design:

- **Switch** — the whole component ("shaft"/"eje" in some listings). Houses spring + contacts.
- **Stem** — the moving part that travels up/down when pressed. On the KS-33 it's the **brown "+" cross** (MX-compatible). The keycap mounts here.
- **Keycap / cap** — the plastic top you touch with your finger; clips onto the stem cross. In this project these are **3D-printed, rectangular, OP-1 style**.
- **Nub** — a small protrusion. Not a stock switch part — a feature we'd add under a *floating* cap to push the stem down. See "Keycap strategy".
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

## Specs (confirmed — Gateron official)

| Spec | Value |
| --- | --- |
| Feel | Silent tactile |
| Operating force | 55 gf (±15 gf) |
| Pre-travel (actuation) | 1.7 mm |
| Total travel | 3.0 ±0.2 mm |
| Pre-lubed | Yes |
| Pins | 3-Pin (our units — confirmed by photo + purchase listing; note some Gateron marketing text for the range says "5-pin") |
| Stem / housing | POM stem, PC transparent top, Nylon black bottom |
| LED | SMD support (south-facing) |
| Plate thickness | 1.2 mm |

*Reference (same family): Silent Red = linear 45 gf; Silent Brown = tactile 55 gf. Same 1.7 mm / 3.0 mm travel across the line.*

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
| Cross arm widths | 1.10 ±0.04 and 1.28 ±0.04 |
| Stem collar diameter | ø5.70 ±0.05 |
| Keycap-mount tower height | **2.95 mm** — sliced from the STEP; cross + collar + boss all reach the same top plane (z=12.15). **This caps the cap socket depth.** |

> **The keycap mount is standard MX.** Gateron: *"MX-styled stem with a circular dust wall"*, compatible with standard MX keycaps ([gateron.co](https://www.gateron.co/products/gateron-low-profile-mechanical-switch-set)). So the **housing/boss shape (square vs rectangular) is irrelevant to the cap** — the cap only engages the **cross ("+") + the circular collar**, and both are standard (**cross 4.0 × 1.1, collar ø5.7**). Verified by slicing the official STEP: cross **4.00 × 1.11** arm, collar **ø5.68**. The *one* non-standard trait is **low profile** — the mount tower is only **~2.95 mm** tall, so the cap socket must be shorter than the full-MX ~4 mm.

**Bottom / PCB footprint (hot-swap)** — transcribe directly into the PCB footprint:

- Central pole at center, ø ~4.80–5.05 (minor rev variation between drawing sheets).
- Metal contact pins + RGB LED slot positioned by offsets: 5.00, 3.20, 2.25, 5.75, 4.70, 2.60, 4.40 (from center).
- RGB (SMD LED) slot on the **south** side — sets where the [SK6812 MINI-E](sk6812mini-e.md) sits relative to switch center.

**Layout implication:** body is 15.00 mm and the key pitch is 18 mm → only **3 mm between switch bodies** (1.5 mm/side) and **4 mm of plate web between 14 mm cutouts**. Tight but workable.

### Silent variant — vernier measurements (SOURCE OF TRUTH)

The STEP/drawing above are the **standard** KS-33 LP 2.0. Our actual switch is the **Silent Brown**, which is slightly different. Measured on the physical Silent switch — **use these for CAD**:

| Feature | Silent (vernier) | Standard (model) |
|---|---|---|
| Outer flange — widest, rests on plate | **14.5 mm** | 15.0 |
| Below-flange body — passes through plate | **13.9 × 13.9 mm** | ~14 |
| Housing height (excl. bottom pillar + stem) | **5.2 mm** | — |
| Bottom centre round pillar (ø) | **4.9 mm** | ~4.8–5.05 |
| Stem top (the moving part, rounded-rectangle) | **6.4 × 6.2 mm** | round-ish |
| Cross arm width | **~1.0 mm** | 1.10 / 1.28 |
| Stem top difference | clean cross on rounded base | cross + centre detail |

**For CAD:**
- **Plate cutout:** target **~14.0 mm** (below-flange 13.9 passes; flange 14.5 rests on top). Dial exactly on the single-key test print.
- **Plate thickness — DECIDED: ~2.5 mm stepped plate** (variant-independent). The switch geometry: clip notch at **1.2 mm** below the flange, housing **base at 2.5 mm** (the "bed" that rests on the PCB). A plain 1.2 mm plate clamps the clips fine but leaves the base floating **1.3 mm above the PCB**. Instead, use a **2.5 mm-thick plate with a stepped cutout**:
  - **Top 1.2 mm:** 14.0 mm hole = the **clip shelf** (body passes, clips hook under this ledge).
  - **Lower 1.3 mm:** widen to **~15 mm** to clear the sprung-out clips (they extend to ~14.7 mm hooked).
  - Result: switch clips firmly at 1.2 mm **and** the plate bottom sits flush with the switch base → **both plate and switch rest on the PCB** (rigid, no float). Bonus: 2.5 mm prints far sturdier than 1.2 mm.
  - Make total **~2.4 mm** (a hair under 2.5) so the switch seats fully before the plate bottoms on the PCB. The **1.2 mm clip shelf is the critical reference**; the rest is structural.
  - LEDs (SK6812) sit on the PCB inside each cutout (under the switch); the plate rests on the PCB **in the ~4 mm webs between keys**, clear of the LEDs.
  - ⚠️ **Verify on the test print:** confirm the switch **clicks firmly** at the 1.2 mm shelf and the base meets the PCB. Adjust the shelf depth if the measured flange→clip differs.
- **Cap cross-socket (finalised in [`hardware/3d/cap.scad`](../../../hardware/3d/cap.scad)):** standard MX cross-in-cylinder mount — **post Ø5.5** (matches collar ø5.7), **cross 4.0 span × 1.1 male arm**, **socket depth 2.8 mm** (tower is 2.95). Add print clearance to the **arm width only** — asymmetric, the length self-locates: **+0.18 mm FDM (0.2 mm nozzle) / +0.05 mm resin** (`stem_clearance`). Add a **0.5 mm 45° flared lead-in** at the mouth (eases insertion + absorbs elephant-foot); optional **split-post** for FDM so the cross flexes instead of cracking. Validate with a **stem-only tolerance ladder** (0.10 / 0.15 / 0.20 / 0.25) before committing to a full cap.
- The STEP model is a **rough visual reference only** for the Silent — geometry authority is this table.

## Keycap strategy (this project)

Because the stem is an **MX cross**, both routes are viable:

- **Floating cap (guided by plate)** — printed rectangular cap sits in a plate well and pushes the stem via a nub; does *not* clip the stem. Most forgiving for outsourced FDM, easiest to re-print a single bad cap. Recommended for the first instance.
- **Stem-mounted cap** — printed cap with a cross socket clips onto the stem. Cleaner look; feasible now that we have exact stem cotas (4.00 envelope, 1.10 / 1.28 arms). Model the socket to *these* numbers (not generic MX 1.17/1.27) for a snug fit. Better in resin. Plan B.

18 mm key pitch (per the design system). Layout: flattened piano (OP-1 style) — naturals row + sharps offset above.

## Wiring (to the keyboard I2C expanders)

Direct wiring, no matrix, no diodes:

- One leg of each switch → one input pin of a [PCF8575](../wiring.md) I2C expander (internal pull-up).
- Other leg → common GND.
- Press = pin reads LOW. Each key is independent (32 inputs across 2× PCF8575), so no ghosting and no diodes needed.

## Sources

- **Official Gateron product page** (spec table — force, travel, materials, pins): https://www.gateron.com/products/gateron-ks-33-low-profile-red-silent-20-mechanical-switches-set?VariantsId=11457
- Gateron KS-33 datasheet index (PDFs per color, no mechanical drawing): https://www.gateron.co/pages/gateron-ks-33-low-profile-2-0-mechanical-switch-datasheet
- Bundled "Product Manual" PDF: generic CE/RoHS safety manual only — **no mechanical data**.
- Keebio product page (secondary): https://keeb.io/products/gateron-ks-33-low-profile-2-0-switches
