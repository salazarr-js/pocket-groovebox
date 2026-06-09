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

Binary files (PCM samples, wavetables) can be baked directly into the firmware binary — no filesystem needed:

```ini
; platformio.ini
board_build.embed_files =
    firmware/src/samples/kick.raw
    firmware/src/samples/snare.raw
```

Accessed in code as:

```cpp
extern const uint8_t kick_raw_start[] asm("_binary_kick_raw_start");
extern const uint8_t kick_raw_end[]   asm("_binary_kick_raw_end");
```

Alternative: store samples in LittleFS and stream them at runtime (more flexible, uses the filesystem partition). For V1, drums are synthesized so neither path is needed yet — this section is relevant when V2 sampler support is added.

---

## Arduino IDE (legacy sketches only)

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
  See [modules/pcm5102.md](hardware/modules/pcm5102.md).
- **GFX Library for Arduino** (`Arduino_GFX`, by moononournation) — for the ST7789 display.
  Install via Library Manager. Chosen over TFT_eSPI because pins are set in code (no
  `User_Setup.h` to edit) and it handles the 170×320 offset. See [modules/st7789.md](hardware/modules/st7789.md).

**3. Board settings.** Board-specific Tools-menu values and the upload/download-mode trick
are in [modules/esp32-s3-devkit.md → Flashing this board](hardware/modules/esp32-s3-devkit.md#flashing-this-board).

## Sketches

Exploration sketches from Phase 2 live under [`../sketches/`](../sketches/):

- `01-hello-esp` — board bring-up: serial chip info + RGB LED cycle
- `02-led-serial` — set the RGB LED color from the Serial Monitor (send `r`/`g`/`b`/`y`/`c`/`m`/`w`/`o`)
- `03-PCM5102A-test` — I2S tone test for the PCM5102 DAC
- `04-audio-notes` — play one octave from the Serial Monitor; keys `a w s e d f t g y h u j k`
- `04-audio-notes-ble` — experiment: same as above, but the letter arrives over BLE UART (play from a phone, no cable)
- `05-audio-pitch-wave` — pitch glides up and down continuously (sine LFO → siren/glissando); plays on its own
- `06-display-hello` — first light for the ST7789: color test + a little UI (title, divider, live counter)
- `07-chord-player` — hold a serial key to sound a note (continuous, non-blocking); arrows pick a single/major/minor layer; ST7789 shows the chord (landscape UI). Display is always landscape from here on. Needs a raw terminal for live keys (see README → Tools).
- `08-amp-speaker-test` — sweep amplitude through the PAM8403 → small speaker to find a clean (non-distorting) level
- `09-audio-notes-gate` — polyphonic note-on/off with real key-down/up gate (+a/-a protocol); momentary maj/min chord layers; Web Serial keyboard bridge
- `10-audio-looper` — step sequencer / phrase looper (⬅ next to flash and test): 4 layers × up to 32 steps (4/8/12/16/32 selectable), live recording and step-entry editing, per-layer volume, free improvisation over a running loop. Use [`looper-bridge.html`](../app/looper-bridge.html) for full transport control. Keyboard shortcuts: `Space` play/stop, `Enter` record, `←→` step, `↑↓` layer, `Del` clear step, `A{l}{0-9}` layer volume. Clear buttons are disabled while playing as a guard against stuck notes.

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
