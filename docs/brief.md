# Pocket Groovebox — Project Brief

> 🎹 A modern DIY pocket groovebox. Just for fun.

## Origin

The project started after receiving a **Stylophone Beat** as a gift from a friend. It sparked interest in portable music devices, standalone instruments, tactile interfaces, embedded systems, and realtime interaction.

> Small playful devices can inspire surprisingly deep creative experiences.

## Vision

Build a single portable, playful music instrument — a modern, reimagined Stylophone in the form of a standalone pocket groovebox — refined over a few iterations.

The goal is instant fun, tactile interaction, playful experimentation, minimal interfaces, and portable creativity.

## Inspirations

No single device is the blueprint. Each one below contributes a piece — the spirit, the play style, the form factor, or a lesson about restraint — and together they point at what Pocket Groovebox should feel like.

| | Source | Role | Takeaway |
| --- | --- | --- | --- |
| <a href="https://stylophone.com/product/stylophone-beat"><img src="https://stylophone.com/wp-content/uploads/2026/04/beat-pdi-02-2-1024x1024.png" width="110"></a> | [Stylophone Beat](https://stylophone.com/product/stylophone-beat) | Foundational | The instrument that started it all — the thing to reimagine |
| <a href="https://hichord.shop/"><img src="https://hichord.shop/cdn/shop/files/HiChord_-_Aluminum3.png?v=1738820365&width=990" width="110"></a> | [Hichord](https://hichord.shop/) | Primary | Core inspiration for the instrument concept and play style |
| <a href="https://aftersound.tech/"><img src="https://aftersound.tech/main.png" width="110"></a> | [Aftersound LOOPA](https://aftersound.tech/) | Primary | Battery-powered "musical multi-tool" — poly synth + tape recorder + step sequencer + live looper in one pocket device; closest match to this project's scope |
| <a href="https://teenage.engineering/products/op-1"><img src="https://assets.teenage.engineering/_img/627cab620e44b900044f3c11_1024.png" width="110"></a> | [Teenage Engineering OP-1](https://teenage.engineering/products/op-1) | Primary | Personality and interaction design beat feature count |
| <a href="https://github.com/BenjaminPoilve/minichord"><img src="https://raw.githubusercontent.com/BenjaminPoilve/minichord/main/documentation/site/ressources/thumbnail.png" width="110"></a> | [minichord](https://minichord.com/) | Primary | Open-source pocket chord instrument (open PCB, BOM, 3D enclosure + firmware) — closest DIY reference for a chord-based build; see also `sketches/16-omnichord` |
| <a href="https://www.chompiclub.com/"><img src="https://www.chompiclub.com/favicon.ico" width="110"></a> | [CHOMPI](https://www.chompiclub.com/) | Primary | Playful all-in-one sampler/looper groovebox — character-driven design, tactile keys + hands-on sampling, joyful "instant fun" feel in a pocketable box |
| <a href="https://www.steamdeck.com/"><img src="https://clan.fastly.steamstatic.com/images//39049601/e54b85b6e75bc7ec589372474ef1705b3471bb66.png" width="110"></a> | [Steam Deck](https://www.steamdeck.com/) | Form factor | Handheld console look and feel — held in two hands, screen + controls |
| <a href="https://teenage.engineering/products/po"><img src="https://assets.teenage.engineering/_img/66211af6b7c4b559bf79be46_1024.webp" width="110"></a> | [Teenage Engineering Pocket Operator family](https://teenage.engineering/products/po) | Secondary | Pocket form factor, creative constraints, minimal UI |
| <a href="https://www.korg.com/us/products/computergear/nanokey2/"><img src="https://cdn.korg.com/us/products/upload/8db603344139affd32e255b4ad01546e_pc.jpg" width="110"></a> | [Korg nanoKEY2](https://www.korg.com/us/products/computergear/nanokey2/) | Secondary | Compact one-octave mini-key keyboard |
| <a href="https://www.m-vave.com/product?id=smk25-mini"><img src="https://www.m-vave.com/images/productInfo/smk25-mini-4.webp" width="110"></a> | [M-Vave SMK25 Mini](https://www.m-vave.com/product?id=smk25-mini) | Secondary | Compact mini-key MIDI keyboard in a portable layout |
| <a href="https://www.korg.com/us/products/dj/monotron_duo/"><img src="https://cdn.korg.com/us/products/upload/6c8dfb0cef26b60ff372a0c609d5afc4_pc.png" width="110"></a> | [Korg Monotron family](https://www.korg.com/us/products/dj/monotron_duo/) | Secondary | Tiny analog synths — physical controls as part of the experience |
| <a href="https://www.m-vave.com/product?id=fm-1"><img src="https://synthanatomy.com/wp-content/uploads/2026/06/M-VAVE-FM-1-synthesizer.jpg" width="110"></a> | [M-Vave FM-1](https://www.m-vave.com/product?id=fm-1) | Secondary | Battery-powered DX-7-style FM polysynth with built-in speaker, 27 keys, TFT screen, 16-step sequencer + arpeggiator — an all-in-one pocket synth in the same class |
| | [Roland TB-303](https://en.wikipedia.org/wiki/Roland_TB-303) | Secondary (sound) | The acid-bassline machine — squelchy resonant lowpass + slide/accent step sequencer. Origin of the whole "303-style" sound; reference for an acid-bass engine, and a case study in how a synth's *character* can define a genre |
| | [Donner Essential B1](https://us.donnermusic.com/products/donner-synthesizer-and-sequencer-analog-bass-b1) | Secondary (sound) | Modern, affordable **303-style** acid-bass synth + sequencer (Red Dot 2022) — silicone keys with colour-changing backlight; a current pocket-sized take on the TB-303 formula |
| | [Roland TR-606](https://en.wikipedia.org/wiki/Roland_TR-606) | Secondary (drums) | *Drumatix* — the analog drum machine companion to the TB-303 in the classic acid setup. Reference for the synthesized-drum voices + step sequencer (see [research/drums.md](research/drums.md), already "TR-606-style") |
| | Panasonic R1088 | Secondary | |
| | Roland P-6 | Secondary | |
| | [Casio SK-1](https://en.wikipedia.org/wiki/Casio_SK-1) | Secondary | |

> 🔬 **To research (later):** every inspiration source above deserves a proper deep-research report — pull the full product story, design decisions, feature set, community reception, and the specific lesson for Pocket Groovebox — so this table becomes a set of studied references rather than links. Not doing it now; just flagging it.

> 🔬 **To research (later):** why is *"303-style"* a genre of its own? What makes the TB-303's sound + sequencer so endlessly cloned (Donner B1, Behringer TD-3, Roland TB-03…), and what of it belongs in this synth's engine. Feeds the synthesis-system research (`docs/research/synthesis.md`).

### Videos

- [NES-SY2.0 Nintendo homage synth](https://www.youtube.com/watch?v=KWfsQgcx9cc) — Nintendo-inspired synthesizer build
- [Custom Isomorphic Keyboard Demo](https://youtu.be/9wTGm-IKtV0) — demo of a custom isomorphic keyboard
- [Custom Isomorphic Keyboard — Technical Breakdown](https://youtu.be/jn62QLrfST8) — technical breakdown of the build

## Product Concept

A **modern**, lightweight, battery-powered standalone synthesizer / sampler / loop station with a step sequencer, in a **handheld console form factor** (OP-1 × HiChord layout — screen and controls above, keyboard below), with a built-in speaker, display, and a 2-octave 24-key keyboard (C3–B4). Works without a computer or internet.

> Concept renders exploring form factor and layout: see [design.md](design.md).

## Design Principles

1. **Instant fun** — playable within seconds of power-on; every control does something audible immediately.
2. **Playful minimalism** — few controls and shallow menus (no deeper than two levels); add interaction depth, not button count.
3. **Standalone first, optional companion** — the device works on its own, with no computer or internet. Optional companion software (web or native) could improve the experience later — e.g. a visuals editor, sound packs, or a pattern editor — but it must never be required for play.
4. **Open, DIY hardware** — an open-source hardware project, not a product. Built from accessible, maker-friendly dev modules and community tools, and from techniques anyone can try at home: repurposed toy instruments, 3D-printed parts, laser-cut or CNC acrylic, robotics kits. Favor off-the-shelf and reusable over custom and specialized.

## Non-goals

This is **not** a DAW, a workstation, or a feature-heavy synth — and not a product line. No deep menus, no massive sample libraries, no chasing feature count. The constraints are the point.

## Success signals

- I reach for it instead of my phone when I have a spare minute.
- It makes sound within seconds of power-on, with no setup.
- A complete idea — a loop with a melody — comes together in one sitting, no manual needed.
