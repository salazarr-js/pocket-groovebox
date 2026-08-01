# Pocket Groovebox

🎹 A modern DIY pocket groovebox. Just for fun.

<img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/8feba0f5-99bf-40f7-8a49-3b96758bdf67" />


## Status

Hardware checkpoint: peripherals verified on the ESP32-S3 (Phase 2 sketches done); keyboard CAD rebuild in progress ([hardware/3d/octaves-plan.md](hardware/3d/octaves-plan.md)). Next milestone: **build 1.0** — a one-octave (12-key) hand-wired keyboard integration build with printed caps + plate, no LEDs.

## Docs

- [docs/brief.md](docs/brief.md) — project brief (the why and what: vision, inspirations, principles)
- [docs/design.md](docs/design.md) — design & concept exploration (AI-generated prototype renders)
- [docs/hardware/README.md](docs/hardware/README.md) — hardware decisions (platform, components, power)
- [docs/hardware/wiring.md](docs/hardware/wiring.md) — how the modules connect to the ESP32-S3 (diagram + pin tables)
- [docs/plan.md](docs/plan.md) — build plan (checklist of steps by phase)
- [docs/architecture.md](docs/architecture.md) — firmware architecture (layer model, audio engine, open questions)
- [docs/development.md](docs/development.md) — dev environment setup (toolchain, libraries, build/flash)
- [docs/research/](docs/research/) — background research (synthesis, digital synths, drums, music theory)
- [docs/explorations/](docs/explorations/) — ADR-style decision records, mostly resolved (input method, key design, design system decided; keyboard CAD rebuild in progress)

## References

Inspirations behind the design are in the [brief](docs/brief.md#inspirations).

**Prior-art projects worth learning from:**
- Oskitone
  - https://github.com/oskitone/scout
  - https://youtu.be/TJ4WLL2fQbY
- https://github.com/wprudencio/esp32-synth

## Tools

- https://fritzing.org/
- https://wokwi.com/

**Live serial input** (char-by-char, raw terminal via `screen`): see [docs/development.md](docs/development.md).

### Skills
- https://github.com/ezrover/ESP32-AI-Agent-Skill
- https://github.com/adamlipecz/esp32-firmware-engineer-skill

## License

MIT — see [LICENSE](LICENSE).
