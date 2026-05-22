# Synth Lab

🎹 A research and build log for a family of pocket music instruments — portable synths, samplers and grooveboxes inspired by the OP-1, PO-33, Stylophone Beat and nanoKEY2.

## Status

Early planning. Concept defined across two parallel input docs. Hardware on hand: NodeMCU ESP8266 (for the browser-based prototype). Target for v1 standalone: ESP32-S3 + I2S DAC + speaker + battery.

## Direction

A small standalone synthesizer / sampler / loop station with a one-octave mini keyboard, OLED display, speaker, rechargeable battery — strong emphasis on tactile interaction, portability and fun.

## Docs

- [docs/chatgpt.md](docs/chatgpt.md) — full project summary (ChatGPT input)
- [docs/claude.md](docs/claude.md) — full project summary (Claude input)

## Repo layout

- `docs/` — brief, decisions, references
- `prototypes/` — code per prototype (ESP8266 + browser, ESP32 standalone, …)
- `hardware/` — schematics, PCB (KiCad), BOM
