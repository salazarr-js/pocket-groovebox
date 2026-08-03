# hardware/3d/vendor/ — third-party meshes

Reference meshes from external sources, committed for reuse as visual ghosts in the OpenSCAD assemblies (fit checks, placement iteration). **Not our geometry**: never derive dimensions from these — dimensional sources of truth are the module docs in [`docs/hardware/modules/`](../../../docs/hardware/modules/) (vernier tables + official drawings).

| File | What | Source | Notes | Mesh frame (⚠️ re-measure if the file is ever replaced) |
|---|---|---|---|---|
| `ks33-switch.stl` | Gateron KS-33 Low Profile 2.0 switch | Converted (FreeCAD) from Gateron's official STEP — [KS-33 LP 2.0 3D (.rar)](https://www.gateron.com/u_file/2309/25/file/GateronKS-33LowProfile20KS33-20230109KS-33-Y313D.rar), from [gateron.com/pages/3d](https://www.gateron.com/pages/3d) | ⚠️ **STANDARD KS-33, not the Silent we build with** (Silent body ~0.5 mm smaller; stem top differs) — visual only. Spec: [`gateron-ks-33.md`](../../../docs/hardware/modules/gateron-ks-33.md) | x centred at **60**; **z0 = bottom-pillar tip** (flange underside = below-flange body + pillar above z0). Consumed by `plate.scad` `ks33_stl_cx` / `ks33_stl_flange_z` |
| `pcf8575-module.stl` | PCF8575 16-bit I/O expander breakout (blue 21×32 module) | Converted (FreeCAD) from the community [GrabCAD STEP](https://grabcad.com/library/module-pcf8575-16bit-io-expander-1) | Cross-checked against vernier + listing photo. Spec: [`pcf8575.md` § Mechanical](../../../docs/hardware/modules/pcf8575.md#mechanical--v10-module-measured-2026-08-01) | hole-pattern centre at **x 15.25** (measured in FreeCAD); board top = z0. Consumed by `plate.scad` `pcf_stl_pattern_cx` |

Both originals are published for free download by their sources (manufacturer / GrabCAD community); the converted STLs are redistributed here for build convenience with attribution. If a rights holder objects, drop the file and fall back to the download links above.
