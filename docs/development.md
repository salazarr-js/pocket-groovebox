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

**3. Board settings.** Board-specific Tools-menu values and the upload/download-mode trick
are in [modules/esp32-s3-devkit.md → Flashing this board](modules/esp32-s3-devkit.md#flashing-this-board).

## Sketches

Test sketches live under [`../src/`](../src/):

- `01-hello-esp` — board bring-up: serial chip info + RGB LED cycle
- `02-led-serial` — set the RGB LED color from the Serial Monitor (send `r`/`g`/`b`/`y`/`c`/`m`/`w`/`o`)
- `03-PCM5102A-test` — I2S tone test for the PCM5102 DAC

## Future: PlatformIO

When firmware outgrows single sketches, switch to **PlatformIO** (VS Code). It uses the
same Arduino framework (all libraries still work) but adds project structure, pinned
library versions, and a git-friendly `platformio.ini`. It also auto-configures C/C++
IntelliSense. ESP-IDF stays available as an escape hatch for low-level audio if needed.
