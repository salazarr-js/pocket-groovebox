# Pocket Groovebox — Development Setup

How to set up the toolchain and build/flash firmware. Board-specific flash settings live
in the board's module doc ([modules/esp32-s3-devkit.md](modules/esp32-s3-devkit.md#flashing-this-board)).

> Currently using the **Arduino IDE** for quick module tests. When the firmware grows
> (synth engine, sequencer), the plan is to move to **PlatformIO** for proper project
> structure — see the note at the bottom.

## Arduino IDE

**1. Add the ESP32 boards URL.** File → Preferences → *Additional Boards Manager URLs*:

```
https://espressif.github.io/arduino-esp32/package_esp32_index.json
```

Then Tools → Board → Boards Manager → install **"esp32 by Espressif Systems"** (3.x).

**2. Libraries.** Install via Sketch → Include Library → Manage Libraries:

- **Adafruit NeoPixel** — *not used.* The board's onboard RGB LED was only driven for
  bring-up testing (with the built-in `rgbLedWrite()`); the instrument has no addressable
  LEDs, so no LED library is needed.
- **ESP_I2S** — for PCM5102 audio. No install needed: it ships with the esp32 core. Use
  it (`#include <ESP_I2S.h>`) instead of the generic Arduino `I2S.h` that most PCM5102
  examples show — that one doesn't target the ESP32-S3. It lets us assign the I2S pins.
  See [modules/pcm5102.md](modules/pcm5102.md).
- **GFX Library for Arduino** (`Arduino_GFX`, by moononournation) — for the ST7789 display.
  Install via Library Manager. Chosen over TFT_eSPI because pins are set in code (no
  `User_Setup.h` to edit) and it handles the 170×320 offset. See [modules/st7789.md](modules/st7789.md).

**3. Board settings.** Board-specific Tools-menu values and the upload/download-mode trick
are in [modules/esp32-s3-devkit.md → Flashing this board](modules/esp32-s3-devkit.md#flashing-this-board).

## Sketches

Test sketches live under [`../src/`](../src/):

- `01-hello-esp` — board bring-up: serial chip info + RGB LED cycle
- `02-led-serial` — set the RGB LED color from the Serial Monitor (send `r`/`g`/`b`/`y`/`c`/`m`/`w`/`o`)
- `03-PCM5102A-test` — I2S tone test for the PCM5102 DAC
- `04-audio-notes` — play one octave from the Serial Monitor; keys `a w s e d f t g y h u j k`
- `04-audio-notes-ble` — experiment: same as above, but the letter arrives over BLE UART (play from a phone, no cable)
- `05-audio-pitch-wave` — pitch glides up and down continuously (sine LFO → siren/glissando); plays on its own
- `06-display-hello` — first light for the ST7789: color test + a little UI (title, divider, live counter)
- `07-chord-player` — hold a serial key to sound a note (continuous, non-blocking); arrows pick a single/major/minor layer; ST7789 shows the chord (landscape UI). Display is always landscape from here on. Needs a raw terminal for live keys (see README → Tools).

## Input: desktop serial or BLE UART

The note test reads single letters. The primary path is the **Arduino Serial Monitor**
over USB. As an experiment, `04-audio-notes-ble` accepts the same letters over **Bluetooth
LE** using the Nordic UART Service (NUS), so a phone can drive it wirelessly.

Two constraints shape this:

- The **ESP32-S3 has BLE only — no classic Bluetooth (SPP)**. The common Android "Serial
  Bluetooth Terminal" (SPP) workflow from original-ESP32 tutorials does **not** work here;
  use a BLE UART app instead (nRF Connect, LightBlue, BLE Serial Pro).
- **iOS has no USB-serial support** (no driver class, no DriverKit on iPhone), so an iPhone
  can't be a USB Serial Monitor. BLE UART is the practical way to send characters from iOS.

The BLE sketch uses the core's bundled BLE library (no install); `NimBLE-Arduino` is a
lighter alternative if flash/RAM gets tight.

## Future: PlatformIO

When firmware outgrows single sketches, switch to **PlatformIO** (VS Code). It uses the
same Arduino framework (all libraries still work) but adds project structure, pinned
library versions, and a git-friendly `platformio.ini`. It also auto-configures C/C++
IntelliSense. ESP-IDF stays available as an escape hatch for low-level audio if needed.
