# Speaker (40 mm full-range, 4 Ω)

Small full-range speaker — the acoustic output of the groovebox. Sits at the end of the speaker chain: ESP32-S3 → I2S → PCM5102 DAC → PAM8403 amp → **speaker**. Driven in BTL across one PAM8403 channel (the speaker floats between `L+` and `L−`).

| | |
| --- | --- |
| **Role** | Speaker (acoustic output) |
| **Variant** | 40 mm neodymium full-range, rubber edge (no brand) |
| **Impedance** | 4 Ω |
| **Power** | 20 W |
| **Sensitivity (SPL)** | 90 dB |
| **Source** | AliExpress |
| **Price** | — |
| **Quantity** | 2 — 🛒 on order |

The enclosure body will act as a sealed acoustic chamber (~100 cc internal volume) around the driver. The PAM8403 amp and wiring are the same as with the 30 mm proto driver — BTL across one channel.

## Wiring

Wiring → [../wiring.md](../wiring.md#pam8403--speaker-amp) — connections live only there (single source).

⚠️ **Never connect either speaker lead to GND** — the PAM8403 output is bridge-tied; grounding a `−` output shorts the bridge and can destroy the amp. See [pam8403.md](pam8403.md).

## Proto / legacy — 30 mm mini speaker (bench-verified)

The driver used for all Phase 2 bench tests, superseded by the 40 mm above. **All measured levels in the notes below (e.g. `AMPLITUDE` ≈ 320 comfortable) were taken on this 30 mm driver** — re-measure on the 40 mm.

| | |
| --- | --- |
| **Variant on hand** | Generic 30 mm round mini speaker (no brand) |
| **Impedance** | 4 Ω ±15 % |
| **Power** | 3 W RMS |
| **Sensitivity (SPL)** | 89 ±3 dB |
| **Frequency range** | quoted 0–20 kHz (marketing — a 30 mm driver is weak below a few hundred Hz) |
| **Lead length** | 11.5 mm |
| **Size** | 30 × 27 × 15 mm |
| **Quantity** | 1 |

## Notes & gotchas (measured on the 30 mm proto driver)

- **It's robust — rated 3 W / 4 Ω.** It comfortably handles the PAM8403's full output (≈ 1.7 W at our peak test amplitude), so there's no fragility concern; the goal is a *comfortable* level, not protecting the driver.
- **It's efficient and loud (89 dB SPL).** An efficient driver + a 3 W amp is genuinely loud up close — high volume is real SPL, not a fault. Judge level at ~1 m, not against your ear.
- **Small driver distorts on loud low/mid tones.** A 30 mm cone needs large excursion to reproduce a tone like 440 Hz at high SPL, so it adds harmonics (880, 1320 Hz…). Driven hard, those harmonics can dominate and the *perceived pitch shifts* — this is physics, not damage, and clears up as the level drops. The 40 mm driver should push this threshold higher.
- **Volume is controlled in software.** The PAM8403 (QA03) has fixed gain and no pot, so level is set by the digital amplitude before the DAC. For reference, into 4 Ω: `AMPLITUDE` 8000 ≈ 1.7 W (very loud / driver strained), 640 ≈ 11 mW (moderate), 160 ≈ 0.7 mW (quiet). **Comfortable on the 30 mm proto speaker ≈ `AMPLITUDE` 320** (measured by ear with `sketches/08-amp-speaker-test`). Confirmed by the build at [hackaday.io accordion log](https://hackaday.io/project/197399-arduino-esp32-standalone-accordion/log/232201-i-can-listen-to-music), which runs `audio.setVolume(5)` on a 0–21 scale (same idea: keep it low).
- **Constant background hiss at low levels is normal.** The PAM8403 is a cheap BTL class-D amp with no input filter and an audible noise floor that's present regardless of `AMPLITUDE`. Since the noise stays put while the tone drops, it's most noticeable at quiet levels (poorer SNR). Reduce it — but don't expect to fully remove it — with: a **100 µF + 0.1 µF decoupling cap across the amp `5V`/`GND`** (USB 5 V is noisy and class-D barely rejects it — biggest win), **short input wires routed away from the I2S/clock/USB lines** (long breadboard jumpers act as antennas), and a solid common ground. A tonal buzz/whine (vs. plain hiss) points to digital/ground coupling rather than the amp's own floor.
- **For a permanent comfortable level** without pushing the digital amplitude very low (which loses resolution), attenuate the amp input with a divider (e.g. 10 kΩ series + 1 kΩ to GND on L/R, ≈ −21 dB) and keep `AMPLITUDE` high — see [pam8403.md](pam8403.md).
- **Test sketch:** [`sketches/08-amp-speaker-test`](../../../sketches/08-amp-speaker-test/08-amp-speaker-test.ino) sweeps amplitude to find a clean level on a given speaker.
