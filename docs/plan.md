# Pocket Groovebox — Plan

The working build plan: concrete, checkable steps grouped by phase. For the vision see [brief.md](brief.md#vision); for component choices see [hardware.md](hardware/README.md#component-selection).

Check items off as you go. Each done step — with notes, wiring, and photos — is raw material for a future build tutorial (see [Toward a tutorial](#toward-a-tutorial)), so write each step as something another person could follow.

> Built on the **ESP32-S3** for both prototyping and V1 (no separate prototype board).

---

## Phase 0 — Define project limits ✅

Scope settled — all of this now lives in the brief and the hardware README.

- [x] Confirm the V1 feature set (synth, loop station, step sequencer) — [brief.md](brief.md#product-concept)
- [x] Confirm what's out of scope (advanced effects, massive sampling, DAW features) — [brief.md](brief.md#non-goals)
- [x] Finalize the component list — [hardware.md](hardware/README.md#component-selection)
- [x] Confirm the control-layout direction — OP-1 × HiChord: screen + controls above, keyboard below ([0003](explorations/0003-design-system.md))
- [x] Restate the brief's success signals as the bar for "done" — [brief.md](brief.md#success-signals)

---

## Phase 1 — Hardware 🚧 (in progress)

Gather, identify, and document every module; plan the wiring.

- [x] Inventory most parts on hand against hardware.md
- [ ] Toolbox ready (iron, solder, multimeter — see hardware.md Tools)
- [ ] Identify the exact variant of each module; photograph front + back
- [x] Find and save the pinout for each module
- [x] Draft the wiring plan (which pins connect where) — see [wiring.md](hardware/wiring.md)
- [x] Module docs created: ESP32-S3, PCM5102, PAM8403, ST7789, PCF8575, SK6812 MINI-E, speaker, template
- [x] Input method decision: **mechanical keys** ([0001-input-method.md](explorations/0001-input-method.md))
- [x] Keyboard mechanism (V1): **OP-1-style flat rectangular caps directly on KS-33 Silent Brown switches** (lever bridge deferred to a later iteration — see [0002](explorations/0002-key-design.md))
- [x] Speaker upgrade: **40 mm full-range neodymium** (on order)
- [x] Switch mechanical spec confirmed (KS-33 dims + vernier: cutout **14.0mm**, MX-cross stem) — [module doc](hardware/modules/gateron-ks-33.md)
- [x] Keyboard layout finalized (24 keys, C3–B4) — geometry source of truth: [`hardware/3d/octaves-plan.md`](../hardware/3d/octaves-plan.md)
- [x] **CAD/fab toolchain decided** — OpenSCAD (mechanical + layout); FreeCAD fallback; tools installed. KiCad was part of the original decision but dropped with the no-PCB call — [0004](explorations/0004-cad-toolchain.md)
- [ ] **Milestone: v1.0 — one-octave device.** First complete integration after the first 3D prints: a working one-octave groovebox — one octave C–B (**12 keys**), **1× PCF8575, hand-wired**, printed caps + plate, **enclosure with ST7789 display + 40mm speaker + 2× EC11 encoders + joystick**, perfboard main board — **no LEDs, no custom PCB** (the SK6812 LEDs require a PCB that won't be made in this version; deferred). The 2-octave 24-key keybed is the target for a later version. Layout: [`docs/hardware/enclosure-layout.svg`](hardware/enclosure-layout.svg). Background: [0005](explorations/0005-proto-keyboard-module.md). ("v1.0" ≠ the V1/V2 *design* iterations used elsewhere in the docs.)
  - [x] **Plate + cap CAD (OpenSCAD)** — stepped plate, hollow keycap, full-octave assembly — in [`hardware/3d/`](../hardware/3d/README.md)
  - [x] **DEEP MEASUREMENT INVESTIGATION** — web research (Cherry spec, KeyV2, riskable, clp-keycaps) + **sliced the official STEP in FreeCAD**. Confirmed the mount is **standard MX**: cross **4.00 × 1.11**, collar **ø5.68**, mount tower **2.95 mm** (housing/boss shape is irrelevant — the cap only engages the cross + circle). Cap socket values: `socket_depth` 3.6→**2.8** (low-profile), `cross_arm_width` 1.1 + `stem_clearance` (FDM 0.18 / resin 0.05, on arm width only), plus `mouth_flare` lead-in + optional `split_post`. (`cap.scad` was deleted with the v0 rebuild — these values live as a snapshot in [`octaves-plan.md`](../hardware/3d/octaves-plan.md).) See [gateron-ks-33.md](hardware/modules/gateron-ks-33.md).
  - [ ] Finalise open cap/plate questions: **dish** (revisit or keep flat), **sharp offset** illusion, **anti-tilt** for tall naturals. Pitch is settled as **parametric** (cap width + gap; currently 18mm cap + 0.8mm gap = **18.8mm pitch**, decided 2026-08-01 on the 0.4 grid) — see [`octaves-plan.md`](../hardware/3d/octaves-plan.md)
  - [x] **Test-fit kit printed → PASSED** (PLA + PETG, 0.2 & 0.4 mm nozzles, 3 print services): KS-33 clip + cap socket confirmed with `kerf 0` / `stem_clearance 0.18`. 0.2 mm nozzle = best finish (spec for final parts). One service shipped elephant foot → design bottom-edge chamfers in. Details in [`hardware/3d/README.md`](../hardware/3d/README.md); test files deleted after passing.
  - [x] Propagate the parametric pitch (cap 18mm + gap 0.5mm = 18.5mm) to the docs — done in the 2026-07-31 docs audit *(superseded 2026-08-01: gap → 0.8, pitch 18.8, re-propagated; `enclosure-layout.svg` still pending)*
  - [x] **`plate.scad` rebuilt — first pass (2026-08-01)**: 12 stepped KS-33 cutouts at `octaves = 1`, plate derived from the cap bbox (134 × 58.4 × 2.4 at pitch 18.8, margin 1.6, prad 3.2), symmetric 0.3mm edge chamfers. Test-batch scope: **no screws / PCF bosses / wire channels yet** — those come when the plate merges into the enclosure top shell
  - [x] **`key_cap.scad` — FINAL for the test batch (2026-08-01)**: cap family `cap(w_u, d_u)` (1u×2u, 1.5u×1u, 1u×1u) with the **directional OEM-style profile** (deep bevel 2.4 on the touch side — naturals front, C#/F# left, D#/A# right; 1.2 on the rest and all around G#), bottom chamfers (skirt + post foot, anti elephant-foot), per-arm cross bore from the **official stem cotas 1.23/1.10** (the old 1.28 was a misread) + clearances 0.05/0.18 → slots 1.28 = the print-validated fit; bore 4.1 + R0.3 inner fillets (official cap spec)
  - [x] **`assembly.scad` — first pass (2026-08-01)**: plate + 12 caps at stem height, natural/sharp colors, `pressed_key` travel preview (+ PCF dummy later)
  - [x] Caps final + STLs (2026-08-01): `key_cap-{natural,sharp15,sharp1}.stl` — 7×natural, 4×sharp15 (the N/S-symmetric profile makes W/E the same part, **rotate 180° at install for D#/A#**), 1×sharp1. `plate.stl` v1 exported too but **superseded by the scope change below**
  - [x] **Calipers: PCF8575 module** ✅ (2026-08-01) — outline vernier-measured **32.1 × 21.3 × 1.6**; holes/headers/heights extracted from the GrabCAD STEP with FreeCAD, cross-checked against vernier + listing photo: 4× ø2.0 holes, long-axis pitch **28.30** (≈ the 28.2 bay-centre spacing at pitch 18.8), tallest component 2.1. Full dims: [`pcf8575.md` § Mechanical](hardware/modules/pcf8575.md#mechanical--v10-module-measured-2026-08-01). Residual: confirm hole ø accepts M2 at dry-fit — mount modeling **unblocked**
  - [x] **PCF8575 bosses + wire channels into `plate.scad` — first pass (2026-08-02)**: 4× M2 bosses at the true hole pitch 28.3 in the C#↔D# / D#↔F# bays (pilot ø1.8×5, standoff 4.0 — clears the KS-33 under-plate pillar 3.4, measured from the official Gateron STEP → `gateron-ks-33.md`), snap-in wire channels (neck 1.2 → body 1.6 × 1.6 deep, trunk at y14 + one spur per switch). Screws: **M2×6 autorroscante** (×8 bottoms out)
  - [x] **PCF placement SETTLED + folded into `plate.scad` (2026-08-02)** — iterated in `plate-pcf-fit.scad` with the real module + KS-33 STEP meshes: module **VERTICAL in the D#↔F# bay** (cx = bay centre −9.4, cy 24.0) — the **pillar-free spot** (board clears every KS-33 pillar; 1.0 mm lateral air to the D#/F# pillars). North (corner/header) boss pair sits on a **derived back tab** (24.6 wide, +3.15 past the back edge); south (mid) pair in the naturals↔sharps web. **Bosses ø4.4** (0.4 grid; below the ≥2-wall rule — accepted, tap gently), pilot ø1.8×3.8, **standoff 2.8** (floor = right-angle header body 2.5 + 0.3), **screws M2×5** (3.4 thread). Component side + 90° male I2C header face the plate (the ø4.4 ring clears the SSOP corner that blocked this at ø6); solder face + header tails outward; I2C exits north over the tab. **Assembly rule: rotate D# −90° (pins WEST) / F# +90° (pins EAST)** — KS-33 pins live in the switch's south half (see `gateron-ks-33.md`). Under-plate stack: **4.4 mm**. `plate_body` refactored to per-convex-piece `chamfered_prism()` (hull-chamfer breaks on concave outlines)
  - [ ] **Polish `plate.scad`** before re-exporting `plate.stl`: **integrate the fit-test extras** from `plate-pcf-fit.scad` (KS-33 switch ghosts + `key_rot()` D#/F# rotation, right-angle header ghost, module-mesh option), add **feet bosses** so the plate rests flat on a table with switches + PCF mounted (dev/testing aid), polish the **tab + chamfers/fillets** (tab corner r currently capped at ~2.56 by the offset trick vs prad 3.2), **re-route the wire channels** to the vertical module (disabled 2026-08-02 in both files), then delete `plate-pcf-fit.scad` + the third-party meshes (`pcf8575-module.stl`, `ks33-switch.stl` — GrabCAD/Gateron content, NOT for commit)
  - [ ] Print **batch 2**: plate-module + 12 caps → solder the octave → a ready-to-use/integrate keyboard module
  - [ ] Calipers pass: KY-023, perfboard (+ re-measure anything the layout needs) — replace the "verify" placeholders in `enclosure-layout.svg` and **update the SVG to pitch 18.8 / plate 134×58.4**
  - [ ] **Batch 3 = the full v1.0 device, SIMPLIFIED** (decided 2026-08-01): the two-shell enclosure — top shell absorbs the plate region + openings/mounts for ST7789, 40mm speaker, 2× EC11, joystick; bottom shell carries the **hand-wired perfboard**; M2 sandwich with heat-set inserts. **No battery for now** — powered over the **ESP32-S3's own USB port, exposed through a shell opening** (18650 + charging deferred). Gates before modeling: all-module calipers done, SVG layout re-verified at 18.8
  - [ ] Hand-wire the octave: switch pins → PCF8575 module via plate wire channels — **no custom PCB** (decided 2026-07-31, see [`octaves-plan.md`](../hardware/3d/octaves-plan.md)); PCF mounts into the plate between the sharps. SK6812 LEDs: **deferred** — they need a PCB, which is left to a future PCB version
  - [ ] Assemble + firmware smoke test (read 12 keys)

---

## Phase 2 — Software install and base tests

Get the dev environment running and verify each peripheral on its own.

- [x] Install the toolchain — see [development.md](development.md) (PlatformIO is now the primary toolchain; Arduino IDE kept only for legacy sketches)
- [x] Flash a blink/serial sketch to the ESP32-S3; confirm upload works

**Three quick tests to get comfortable with the hardware:**

- [x] **Audio · sound** — play a test tone over I2S (PCM5102) — [`sketches/03-PCM5102A-test`](../sketches/03-PCM5102A-test/03-PCM5102A-test.ino) ✅ 440 Hz tone on headphones
  - [x] _Amp + speaker:_ 440 Hz through the PAM8403 to a small speaker — [`sketches/08-amp-speaker-test`](../sketches/08-amp-speaker-test/08-amp-speaker-test.ino) ✅ (fixed-gain amp overdrives a tiny speaker — keep amplitude low; see [modules/pam8403.md](hardware/modules/pam8403.md))
- [ ] **Audio · notes** — send a letter over Serial Monitor and play the matching note — [`sketches/04-audio-notes`](../sketches/04-audio-notes/04-audio-notes.ino)
  - [ ] _Experiment:_ BLE UART variation — [`sketches/04-audio-notes-ble`](../sketches/04-audio-notes-ble/04-audio-notes-ble.ino)
  - [ ] _Note-on/off + polyphony:_ [`sketches/09-audio-notes-gate`](../sketches/09-audio-notes-gate/09-audio-notes-gate.ino) via [`app/keyboard-bridge.html`](../app/keyboard-bridge.html). Open audio issues: [issues/01-audio-issues.md](issues/01-audio-issues.md).
- [x] **Display · play** — drive the ST7789 over SPI — [`sketches/06-display-hello`](../sketches/06-display-hello/06-display-hello.ino) ✅
  - [x] _Experiment:_ chord player — serial keys + chord display on ST7789 — [`sketches/07-chord-player`](../sketches/07-chord-player/07-chord-player.ino) ✅

**Remaining peripheral checks:**

- [ ] Rotary encoder (EC11): read rotation + button
- [x] Joystick (KY-023): read both axes + button — [`sketches/12-ky023-joystick-test`](../sketches/12-ky023-joystick-test/12-ky023-joystick-test.ino) ✅ centered deltas + single `JOY_MOUNT` orientation knob; see [modules/ky023.md](hardware/modules/ky023.md)
- [ ] Keyboard: read all 24 keys
- [ ] Power: run from the 18650 cell through the regulator
- [ ] Update each module doc with wiring + gotchas as it passes

---

## Checkpoint — Toolchain + Architecture + Research (⬅ current)

Pause here. The exploration sketches have taught us enough to plan seriously. Two things to
do before writing more firmware.

### 1. Migrate to PlatformIO ✅ (mostly done)

Arduino IDE served the exploration phase but doesn't scale:
- No dependency management — libraries installed globally, version conflicts are silent
- No project-level build config — every sketch reimports everything manually
- Poor C++ support — auto-prototype injection causes real bugs (hit this with the drum sketch)
- No unit testing, no CI, no multi-target builds

**Tasks:**
- [x] Install PlatformIO CLI + VS Code extension
- [x] Create `platformio.ini` at the repo root — single env `waveshare-esp32-s3-r32n16`, src/lib/test/include under `firmware/`
- [ ] Migrate the two "keeper" sketches (09-audio-notes-gate, 10-audio-looper) into `firmware/`
- [x] Confirm upload + serial monitor work (board validation sketch in `firmware/src/main.cpp`)
- [x] Update [development.md](development.md) with the new workflow (now PlatformIO-first)

### 2. Design the firmware architecture

Before writing Phase 3 code, define the system. The exploration taught us what we need;
now build it the right way: modular, configurable, maintainable.

See the architecture design document: [architecture.md](architecture.md)

> **⛔ Blocked on research before any Phase 3 work begins:**
> - **Synthesis system** — deep research into which engines (subtractive, FM, wavetable…), voice budget, modulation model, and patch design. May reshape the audio engine layer. Starting points: `docs/research/digital-synths.md`, `docs/research/synthesis.md`. This is the **only** remaining blocker — input method and key design are resolved ([0001](explorations/0001-input-method.md), [0002](explorations/0002-key-design.md)).

**Key decisions to finalize:**
- [ ] Firmware structure: how state flows (state machine? task-based? event queue?)
- [ ] Audio engine abstraction: voice pool, mixing, effects chain — how voices are allocated and freed
- [ ] Envelope model: exponential ADSR + per-voice params, not hardcoded rates
- [ ] Waveform model: oscillator abstraction that supports sine / saw / square / triangle + wavetable
- [ ] Filter: state-variable filter (SVF) — gives LP, HP, BP from one struct; the right choice for both synth and drums
- [ ] Sequencer: data model for patterns (pitched notes + drum hits on the same timeline or separate?)
- [ ] UI layer: display + encoder + key matrix decoupled from audio — how do they communicate?
- [ ] Persistence: NVS (Non-Volatile Storage) for settings, patterns, presets
- [x] Drum approach: synthesized (SVF + exp env) for V1; polymorphic `IVoice` interface keeps sampler path open for V2

---

## Milestone v0.2 — Consolidated hardware + test UI

All hardware in one tidy place; first navigable UI without a laptop.

### Hardware

- [ ] **Wiring diagram** — draw the full circuit in a dedicated tool before committing anything to solder
  - Investigate which tool to use: **Fritzing** (maker-friendly, has breadboard view), **KiCad Schematic** (open source), **EasyEDA** (online, integrated with JLCPCB), **Wokwi** (online ESP32 simulator with virtual breadboard view)
  - Tentative recommendation: Fritzing for the breadboard view — there is **no custom PCB** in the current plan (keyboard hand-wired to the PCF8575, main board on perfboard); KiCad becomes relevant only if a future PCB version (for the SK6812 LEDs) happens
- [ ] **Consolidated protoboard** — all modules wired on a single perfboard with the ESP32-S3 seated on the protoboard; connectors / jumpers so each module can be plugged in and out cleanly
  - Includes: PCF8575 (keyboard), both EC11 encoders + buttons (GPIO 39/40/16, 41/42/17), KY-023 joystick, ST7789 display, PCM5102 DAC, PAM8403 amp, SK6812 LEDs
  - Encoders are the only peripheral not yet wired and verified

### Software (sketch 16 or similar)

- [ ] **Test menu sketch** — first navigable UI without a laptop; defines the interaction patterns the real firmware will use
  - Encoders: navigate options (turn = move cursor, click = select / back)
  - Joystick: move between sections / submenus
  - Display: two-level menu hierarchy; lock in font size, colors (TEAL on black scheme already established)
  - Test options to explore: select note, select chord, adjust volume, select mode (Play / Sequencer)
  - Audio: selected note/chord plays on confirm → immediate feedback

---

## Phase 3 — Core prototype (breadboard / protoboard)

Build from the architecture plan. Each item here depends on the architecture decisions above.

- [ ] **PlatformIO project structure in place** (from Checkpoint)
- [ ] **Audio engine v1** — voice pool, exponential ADSR, sine oscillator, SVF filter (LP mode)
- [ ] **Synth voice** — full waveform + envelope + filter from the architecture; playable via keyboard matrix
- [ ] **Encoders** — volume and one live parameter (e.g. filter cutoff)
- [ ] **Display** — live state (note, BPM, mode, parameter values)
- [ ] **Step sequencer** — pitched notes + drums on the same pattern timeline
- [ ] **Drum voices** — synthesized (TR-606 style; see [research/drums.md](research/drums.md)); SVF + exp env recipes per drum type
- [ ] **Loop station** — live record, layer playback (evolve from `sketches/10-audio-looper`)
- [ ] **Speaker + headphone** — both outputs working ✅ (already verified in Phase 2)
- [ ] **Control layout** — lock the final physical layout

---

## Phase 4 — Standalone V1

Turn the breadboard prototype into a self-contained battery-powered device.

- [ ] Consolidate the main board on **perfboard** (MCU + charger + amp + display + encoders) — **no custom PCB** (decided 2026-07-31)
- [ ] Hand-wire the full 24-key keyboard to the 2× PCF8575 expanders
- [ ] _Future option (not V1):_ a custom PCB in KiCad, only for the version that adds the SK6812 per-key LEDs — the LEDs require a PCB
- [ ] Design the enclosure (3D-printed / laser-cut or CNC acrylic)
- [ ] Assemble the device
- [ ] Battery + USB-C charging working in the enclosure
- [ ] Full power-on-to-play test (boots and plays with no setup)
- [ ] Check against the brief's success signals

Out of scope for V1: advanced effects, massive sampling, complex DAW features.

---

## Exploration log

Sketches written during Phase 2 exploration. Not production code — each was a learning exercise.

| Sketch | What it taught | Status |
| --- | --- | --- |
| 01-hello-esp | Upload works, serial works | Done |
| 02-led-serial | GPIO, serial input | Done |
| 03-PCM5102A-test | I2S DAC wiring + basic tone | Done ✅ |
| 04-audio-notes | Pitch table, simple tone mapping | Done |
| 04-audio-notes-ble | BLE UART — S3 BLE-only, iOS has no USB serial | Done |
| 05-audio-pitch-wave | Waveform switching over serial | Done |
| 06-display-hello | ST7789 + Arduino_GFX | Done ✅ |
| 07-chord-player | Chord logic + display + I2S | Done ✅ |
| 08-amp-speaker-test | PAM8403 wiring + safe amplitude | Done ✅ |
| 09-audio-notes-gate | Note-on/off protocol, polyphony, envelope | Done |
| 10-audio-looper | Step sequencer architecture, live recording | Done |
| 11-drum-machine | Synthesized drums — removed; linear decay is wrong, need exponential env + SVF; see [research/drums.md](research/drums.md) | Removed |
| 12-ky023-joystick-test | KY-023 read + NVS calibration + single-knob mount orientation; see [hardware/modules/ky023.md](hardware/modules/ky023.md) | Done ✅ |
| 13-pcf8575-keyboard-test | PCF8575 I2C read, 12-key physical keyboard (inverted wiring), I2C scan, debounce | Done ✅ |
| 14-pcf8575-display | PCF8575 keyboard → ST7789 display; large centered note name, idle dot, landscape 90° | Done ✅ |
| 15-hardware-chord-player | Full integration: PCF8575 + KY-023 + ST7789 + I2S; major/minor chords via joystick, 8-level log volume scale, click-free crossfade, layer debounce | Done ✅ |
| 16-omnichord | Omnichord-style play: latching chord select on PCF8575 + joystick strum plate (harp-like pluck decay, OM-27 12-position layout), drone + strum layers, joystick up/down = chord quality | Done ✅ |

---

## Toward a tutorial

A "build your own" guide is a later goal, not a current task. The plan is structured to feed it: keep notes, wiring tables, and front/back + pinout photos in each module doc as you work through the phases. When V1 works, those docs assemble into a step-by-step build guide — bill of materials, tools, wiring, firmware flashing, assembly.
