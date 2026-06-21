# Mini speaker (30 mm, 4 Ω 3 W)

Small full-range speaker — the acoustic output of the groovebox. Sits at the end of the speaker chain: ESP32-S3 → I2S → PCM5102 DAC → PAM8403 amp → **speaker**. Driven in BTL across one PAM8403 channel (the speaker floats between `L+` and `L−`).

| | |
| --- | --- |
| **Role** | Speaker (acoustic output) |
| **Variant on hand** | Generic 30 mm round mini speaker (no brand) |
| **Impedance** | 4 Ω ±15 % |
| **Power** | 3 W RMS |
| **Sensitivity (SPL)** | 89 ±3 dB |
| **Frequency range** | quoted 0–20 kHz (marketing — a 30 mm driver is weak below a few hundred Hz) |
| **Lead length** | 11.5 mm |
| **Size** | 30 × 27 × 15 mm |
| **Source** | — |
| **Price** | — |
| **Quantity** | 1 |

## Wiring in this project

Driven by the PAM8403, **BTL** — the two leads go to one channel's `+` and `−`; there is no ground connection to the speaker.

| Speaker lead | PAM8403 (QA03) |
| --- | --- |
| Lead 1 | L+ |
| Lead 2 | L− |

⚠️ **Never connect either speaker lead to GND** — the PAM8403 output is bridge-tied; grounding a `−` output shorts the bridge and can destroy the amp. See [pam8403.md](pam8403.md). Full connection structure: [../wiring.md](../wiring.md#connections).

## Upgrade — 40 mm full-range speaker

The 30 mm driver is being replaced with a **40 mm neodymium full-range speaker** (4Ω, 20W, 90 dB, rubber edge). Ordered from AliExpress. The enclosure body will act as a sealed acoustic chamber (~100 cc internal volume) around the driver. The PAM8403 amp and wiring remain unchanged — the 40 mm speaker connects across L+/L− in BTL just like the 30 mm driver.

## Notes & gotchas

- **It's robust — rated 3 W / 4 Ω.** It comfortably handles the PAM8403's full output (≈ 1.7 W at our peak test amplitude), so there's no fragility concern; the goal is a *comfortable* level, not protecting the driver.
- **It's efficient and loud (89 dB SPL).** An efficient driver + a 3 W amp is genuinely loud up close — high volume is real SPL, not a fault. Judge level at ~1 m, not against your ear.
- **Small driver distorts on loud low/mid tones.** A 30 mm cone needs large excursion to reproduce a tone like 440 Hz at high SPL, so it adds harmonics (880, 1320 Hz…). Driven hard, those harmonics can dominate and the *perceived pitch shifts* — this is physics, not damage, and clears up as the level drops.
- **Volume is controlled in software.** The PAM8403 (QA03) has fixed gain and no pot, so level is set by the digital amplitude before the DAC. For reference, into 4 Ω: `AMPLITUDE` 8000 ≈ 1.7 W (very loud / driver strained), 640 ≈ 11 mW (moderate), 160 ≈ 0.7 mW (quiet). **Comfortable on this speaker ≈ `AMPLITUDE` 320** (measured by ear with `sketches/08-amp-speaker-test`). Confirmed by the build at [hackaday.io accordion log](https://hackaday.io/project/197399-arduino-esp32-standalone-accordion/log/232201-i-can-listen-to-music), which runs `audio.setVolume(5)` on a 0–21 scale (same idea: keep it low).
- **Constant background hiss at low levels is normal.** The PAM8403 is a cheap BTL class-D amp with no input filter and an audible noise floor that's present regardless of `AMPLITUDE`. Since the noise stays put while the tone drops, it's most noticeable at quiet levels (poorer SNR). Reduce it — but don't expect to fully remove it — with: a **100 µF + 0.1 µF decoupling cap across the amp `5V`/`GND`** (USB 5 V is noisy and class-D barely rejects it — biggest win), **short input wires routed away from the I2S/clock/USB lines** (long breadboard jumpers act as antennas), and a solid common ground. A tonal buzz/whine (vs. plain hiss) points to digital/ground coupling rather than the amp's own floor.
- **For a permanent comfortable level** without pushing the digital amplitude very low (which loses resolution), attenuate the amp input with a divider (e.g. 10 kΩ series + 1 kΩ to GND on L/R, ≈ −21 dB) and keep `AMPLITUDE` high — see [pam8403.md](pam8403.md).
- **Test sketch:** [`sketches/08-amp-speaker-test`](../../../sketches/08-amp-speaker-test/08-amp-speaker-test.ino) sweeps amplitude to find a clean level on this speaker.
