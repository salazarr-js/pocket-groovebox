# 0004 — CAD & fabrication toolchain

**Status:** decided · (`open → decided → revised`)

> **Revision (2026-07) — Ergogen dropped.** Ergogen was meant to hold the layout, but the plate is built in OpenSCAD (Ergogen can't do the **stepped cutout**), and the layout is small — a separate Ergogen YAML just duplicated it, risking drift. The layout now lives **once** in `hardware/3d/octave-layout.scad`, shared by the plate and the caps (and later a KiCad placement export). Toolchain simplified to **OpenSCAD (mechanical + layout) + KiCad (PCB)**.

Which tool holds each part of the hardware design. Chosen primarily for **how agentically Claude can drive it** (the stated priority), plus open-source and git-friendly.

## Decision

| Domain | Tool | Format | Why |
|---|---|---|---|
| Knowledge / decisions / measurements | **docs/ (markdown)** | text | tool-independent "brain" — the *why* lives here, not inside CAD files |
| Keyboard layout (key positions) | **OpenSCAD** (`octave-layout.scad`) | text | one shared data file → feeds the plate + caps (and later a PCB placement export). *(Ergogen evaluated + dropped — see revision note above.)* |
| PCB (switches, PCF8575, SK6812, JST) | **KiCad 10** | project | only real EDA option; agentic via KiCad-MCP |
| Mechanical (caps, plate, enclosure) | **OpenSCAD** | `.scad` text | most agentic: pure code + CLI render + PNG feedback loop; boxy design fits CSG (BOSL2 for fillets) |
| Mechanical upgrade path | **build123d** (CadQuery family) | Python | B-rep kernel → STEP + real fillets, *if* OpenSCAD hits a wall |
| Enclosure fit-check / fallback | **FreeCAD 1.1** | — | KiCad StepUp import; only if needed |

## Why OpenSCAD for mechanical (agentic rationale)

OpenSCAD is **stateless code**: Claude writes/edits the `.scad`, renders via `openscad -o out.png --render`, sees the PNG, and self-corrects — no running GUI, no MCP required. **This loop was validated end-to-end this session** (rendered a test part, inspected the PNG). `iancanderson/openscad-agent` packages exactly this as Claude Code skills — its example is literally "make a mini piano".

FreeCAD is more powerful, but its agentic path (`neka-nat/freecad-mcp`) needs a **live FreeCAD app + addon + RPC + MCP** — stateful, heavier, Claude-Desktop-oriented. Kept as fallback only.

## Agentic options researched (MCP / skills)

| Domain | Best option | ★ | Notes |
|---|---|---|---|
| PCB | mixelpixx/KiCAD-MCP-Server | 1523 | full flow + **custom footprint generation** + JLCPCB + freerouting (targets KiCad 9; successor "Konnect" is KiCad 10 native) |
| PCB alt | Seeed-Studio/kicad-mcp-server · oaslananka/kicad-mcp-pro | 62 · 25 | pin-level analysis · ERC/DRC/DFM/BOM |
| OpenSCAD | iancanderson/openscad-agent (skills) · quellant/openscad-mcp | 106 · 115 | Claude Code skills · MCP render |
| build123d | Svetlana-DAO-LLC/cad-agent | 27 | code-CAD B-rep + render→PNG loop in a container |
| FreeCAD | neka-nat/freecad-mcp | 1292 | mature but stateful / Claude Desktop |
| Slicer (later) | dmikushin/orca-slicer-mcp · VisualBoy/ai-slicer-mcp | new | STL→G-code (printing is outsourced for now) |

## Installed on this machine (brew casks)

- **OpenSCAD 2026.06.12** (snapshot — the stable 2021.01 cask is deprecated / fails Gatekeeper). CLI: `/Applications/OpenSCAD.app/Contents/MacOS/OpenSCAD`
- **KiCad 10.0.4** — `kicad-cli` works. ⚠ heads-up: the mixelpixx MCP targets KiCad 9; confirm it works on 10 or use Konnect when we set up the PCB MCP.
- **FreeCAD 1.1.1**

## Target repo layout

```
docs/                       ← knowledge (decisions, specs, measurements)
hardware/
  3d/                       ← OpenSCAD: octave-layout.scad (shared layout), switch-cutout, plate, cap, assembly
  pcb/                      ← KiCad project (TBD)
  README.md                 ← index, links back to docs/
hardware/enclosure/         ← later (OpenSCAD, or FreeCAD if needed)
```

## Consequences / next

- No MCP is strictly required for OpenSCAD — the CLI loop is enough. A KiCad-MCP *is* worth setting up for the PCB (custom KS-33 footprint + JLCPCB).
- See [0005-proto-keyboard-module.md](0005-proto-keyboard-module.md) for the first physical build that exercises this toolchain.
