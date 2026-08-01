# Explorations

Investigations and options being weighed **before** a decision is locked — research,
alternatives, tradeoffs. Each file is lightweight (ADR-style) with a `Status:` line
(`exploring → leaning toward X → decided`). One numbered file per topic.

This is **not** for bugs (see [../issues/](../issues/)) nor settled reference material
(see [../synthesis.md](../research/synthesis.md), [../music-theory.md](../research/music-theory.md)).

| # | Topic | Status | File |
| --- | --- | --- | --- |
| 0001 | Input method (mechanical keys vs capacitive touch vs ribbon; Solar 42) | decided — mechanical keys + PCF8575 | [0001-input-method.md](0001-input-method.md) |
| 0002 | Key design (switch, mechanism, layout, look & feel) | decided — geometry superseded by [`octaves-plan.md`](../../hardware/3d/octaves-plan.md) | [0002-key-design.md](0002-key-design.md) |
| 0003 | Design system (4 mm measurement grid) | decided — 4 mm grid; keyboard is an off-grid exception | [0003-design-system.md](0003-design-system.md) |
| 0004 | CAD & fabrication toolchain | decided, revised — KiCad dropped (no custom PCB) | [0004-cad-toolchain.md](0004-cad-toolchain.md) |
| 0005 | Proto keyboard module (one octave) | superseded in part → build 1.0 | [0005-proto-keyboard-module.md](0005-proto-keyboard-module.md) |
