# Pocket Groovebox — Development Setup

How to set up the toolchain and build/flash firmware. Board-specific flash settings live
in the board's module doc ([modules/esp32-s3-devkit.md](hardware/modules/esp32-s3-devkit.md#flashing-this-board)).

> Phase 3 firmware uses **PlatformIO** (VS Code). Arduino IDE is kept only for opening legacy exploration sketches under `sketches/`.

## PlatformIO (primary)

**1. Install PlatformIO.** Install the [PlatformIO IDE extension](https://platformio.org/install/ide?install=vscode) for VS Code.

**2. Open the repo root** in VS Code. PlatformIO will detect `platformio.ini` automatically and generate `.vscode/c_cpp_properties.json` and `launch.json`.

**3. Build and upload.**

```sh
pio run                  # build
pio run -t upload        # build + flash
pio device monitor       # open serial monitor (115200 baud)
```

Board-specific settings (`flash_mode`, `memory_type`, partition table) are already configured in `platformio.ini`. See [modules/esp32-s3-devkit.md → Flashing this board](hardware/modules/esp32-s3-devkit.md#flashing-this-board) for why each setting matters.

If the upload fails, enter download mode: hold **BOOT**, tap **RESET**, release **BOOT**.

**Other useful commands:**

```sh
pio run -t erase       # wipe entire flash (use when partition table changes)
pio run -t uploadfs    # upload LittleFS filesystem image (samples, wavetables)
pio test               # run unit tests in test/
```

**Live serial input (char-by-char, no Enter).** The Arduino Serial Monitor and `pio device monitor` are line-buffered and can't send arrow keys. For sketches that read live keystrokes, use a raw terminal — find the port with `ls /dev/cu.*`, then:

```sh
screen /dev/cu.usbmodemXXXX 115200   # quit: Ctrl-A then K
```

Arrow keys work here too. Close it before flashing — only one process can hold the port.

## Debugging

The ESP32-S3 has **built-in USB JTAG** — no external probe needed. The `USB JTAG/serial debug unit` port that appears when you plug in the board is it. To enable in VS Code:

```ini
; platformio.ini
debug_tool  = esp-builtin
debug_speed = 5000
```

Press **F5** in VS Code to start a debug session. Set breakpoints normally. Useful for catching crashes in the audio callback or PSRAM issues.

**Serial debug levels** — add to `build_flags` to get verbose output from the ESP32 Arduino core:

```ini
build_flags =
    -DCORE_DEBUG_LEVEL=0   ; 0=none  1=error  2=warn  3=info  4=debug  5=verbose
```

Keep at `0` in normal builds — level 3+ adds significant serial noise.

## Unit testing

Tests live in `firmware/test/`. PlatformIO uses the **Unity** framework (included, no install):

```cpp
// firmware/test/test_envelope/test_envelope.cpp
#include <unity.h>
#include "Envelope.h"

void test_decay_is_exponential() {
    // ...
    TEST_ASSERT_FLOAT_WITHIN(0.001f, expected, actual);
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_decay_is_exponential);
    UNITY_END();
}
void loop() {}
```

Run on host (no hardware needed — tests the audio math directly):

```sh
pio test -e native   # requires a [env:native] entry in platformio.ini
```

Or on the device:

```sh
pio test
```

Host testing is the goal for `lib/audio` — lets us verify Envelope, SVF, and Oscillator math without flashing hardware every time.

## Embedding audio samples

**Not needed yet — V1 drums are synthesized** (see [architecture.md → Drum voices](architecture.md#drum-voices-)). When V2 sampler support arrives, binary files (PCM samples, wavetables) can be baked into the firmware via `board_build.embed_files` in `platformio.ini` (accessed in code through the `_binary_<name>_start`/`_end` symbols), or stored in LittleFS and streamed at runtime.

---

## Arduino IDE (legacy sketches only)

**1. Add the ESP32 boards URL.** File → Preferences → *Additional Boards Manager URLs*:

```
https://espressif.github.io/arduino-esp32/package_esp32_index.json
```

Then Tools → Board → Boards Manager → install **"esp32 by Espressif Systems"** (3.x).

**2. Libraries.** Install via Sketch → Include Library → Manage Libraries:

- **Adafruit NeoPixel** — *not needed yet.* The final design does include addressable LEDs — 24× SK6812 MINI-E, one per key — but they require a custom PCB and are **deferred to a future PCB version** (build 1.0 has no LEDs). When they're integrated, an RMT-based driver or FastLED will be needed; see [modules/sk6812mini-e.md](hardware/modules/sk6812mini-e.md). The board's onboard RGB LED was only driven for bring-up testing with the built-in `rgbLedWrite()`.
- **ESP_I2S** — for PCM5102 audio. No install needed: it ships with the esp32 core. Use
  it (`#include <ESP_I2S.h>`) instead of the generic Arduino `I2S.h` that most PCM5102
  examples show — that one doesn't target the ESP32-S3. It lets us assign the I2S pins.
  See [modules/pcm5102.md](hardware/modules/pcm5102.md).
- **GFX Library for Arduino** (`Arduino_GFX`, by moononournation) — for the ST7789 display.
  Install via Library Manager. Chosen over TFT_eSPI because pins are set in code (no
  `User_Setup.h` to edit) and it handles the 170×320 offset. See [modules/st7789.md](hardware/modules/st7789.md).

**3. Board settings.** Board-specific Tools-menu values and the upload/download-mode trick
are in [modules/esp32-s3-devkit.md → Flashing this board](hardware/modules/esp32-s3-devkit.md#flashing-this-board).

## Sketches

Exploration sketches from Phase 2 live under [`../sketches/`](../sketches/) (01 through 16). The single source of truth for what each sketch covers is the [exploration log table in plan.md](plan.md#exploration-log) — this doc no longer duplicates the list.

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

## Maintenance scripts

When files are moved or renamed, doc references break. Fix them with a shell one-liner instead of asking Claude (saves tokens, faster, repeatable):

```sh
# Rename a path prefix across all markdown docs
find docs -name "*.md" | xargs sed -i '' 's|old/path|new/path|g'

# Verify no stale references remain
grep -r "old/path" docs/
```

Write a dedicated script (`scripts/fix-refs.sh`) when the pattern recurs often enough to be worth it.

## Libraries

All libraries from the Arduino IDE phase still work under PlatformIO — declare them in `platformio.ini` under `lib_deps`. The ones used in the exploration sketches:

- **ESP_I2S** — ships with the esp32 core, no install needed
- **GFX Library for Arduino** (`Arduino_GFX`, by moononournation) — add to `lib_deps` when the display layer is built
