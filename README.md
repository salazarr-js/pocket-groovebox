# Pocket Groovebox

🎹 A modern DIY pocket groovebox. Just for fun.

<img width="1536" height="1024" alt="image" src="https://github.com/user-attachments/assets/8feba0f5-99bf-40f7-8a49-3b96758bdf67" />


## Status

Early prototyping. Most hardware is on hand; currently testing individual modules on the ESP32-S3.

## Docs

- [docs/brief.md](docs/brief.md) — project brief (the why and what: vision, inspirations, principles)
- [docs/design.md](docs/design.md) — design & concept exploration (AI-generated prototype renders)
- [docs/hardware/README.md](docs/hardware/README.md) — hardware decisions (platform, components, power)
- [docs/hardware/wiring.md](docs/hardware/wiring.md) — how the modules connect to the ESP32-S3 (diagram + pin tables)
- [docs/plan.md](docs/plan.md) — build plan (checklist of steps by phase)
- [docs/development.md](docs/development.md) — dev environment setup (toolchain, libraries, build/flash)
- [docs/glossary.md](docs/glossary.md) — definitions of acronyms and terms (MIDI, DAW, I2S, …)

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

**Live serial input (char-by-char, no Enter).** The Arduino Serial Monitor is line-buffered
and can't send arrow keys. To send each keystroke instantly (e.g. to play notes), use a raw
terminal — find the port with `ls /dev/cu.*`, then:

```
screen /dev/cu.usbmodemXXXX 115200      # quit: Ctrl-A then K
```

Arrow keys work here too. Close it before flashing — only one program can hold the port.

### Skills
- https://github.com/ezrover/ESP32-AI-Agent-Skill
- https://github.com/adamlipecz/esp32-firmware-engineer-skill

## License

MIT — see [LICENSE](LICENSE).
