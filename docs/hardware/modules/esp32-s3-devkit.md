# Waveshare ESP32-S3-DEV-KIT-N32R16V

The main board / MCU — runs the firmware and drives every peripheral.

| | |
| --- | --- |
| **Role** | Microcontroller (main board) |
| **Variant on hand** | Waveshare ESP32-S3-DEV-KIT-**N32R16V** (ESP32-S3-WROOM-2, 32 MB OPI flash, 16 MB OPI PSRAM) |
| **Interface** | USB-C (CH343 UART + native USB), GPIO; pin-compatible with ESP32-S3-DevKitC-1 |
| **Operating voltage** | 3.3 V logic (USB-C 5 V in) |
| **Size** | 63.3 × 25.4 mm (DevKitC-1 form factor) |
| **Current draw** | TBD (measure on the bench) |
| **Source** | Waveshare |
| **Price** | — |
| **Quantity** | 1 |

## Datasheets & references

- Product page: https://www.waveshare.com/esp32-s3-dev-kit-n8r8.htm
- Waveshare docs (schematic in Resources): https://docs.waveshare.com/ESP32-S3-DEV-KIT-N8R8
- Pinout (CircuitState): https://www.circuitstate.com/pinouts/waveshare-esp32-s3-dev-kit-nxr8-wi-fi-development-board-pinout-diagram-arduino-reference/
- ESP32-S3-WROOM-1 module: https://www.espressif.com/sites/default/files/documentation/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf
- ESP32-S3 chip: https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf

## Photos

| Front | Back |
| --- | --- |
| ![ESP32-S3-DEV-KIT front](https://www.waveshare.com/media/catalog/product/cache/1/image/800x800/9df78eab33525d08d6e5fb8d27136e95/e/s/esp32-s3-dev-kit-n8r8-3_6.jpg) | ![ESP32-S3-DEV-KIT back](https://www.waveshare.com/media/catalog/product/cache/1/image/800x800/9df78eab33525d08d6e5fb8d27136e95/e/s/esp32-s3-dev-kit-n8r8-4_6.jpg) |

> Source / credit: [Waveshare](https://www.waveshare.com/esp32-s3-dev-kit-n8r8.htm)

## Pinout

![Waveshare ESP32-S3-DEV-KIT-NxR8 pinout — CircuitState Electronics](https://www.circuitstate.com/wp-content/uploads/2025/06/ESP32-S3-DEV-KIT-NxR8-Pinout-R0.1-CIRCUITSTATE-Electronics-1_1.png)

> Source / credit: [CircuitState Electronics — Waveshare ESP32-S3-DEV-KIT-NxR8 pinout & Arduino reference](https://www.circuitstate.com/pinouts/waveshare-esp32-s3-dev-kit-nxr8-wi-fi-development-board-pinout-diagram-arduino-reference/)

## GPIO usage (N32R16V)

The ESP32-S3 chip has 45 GPIOs; ~9 go to the module's embedded flash, so **36 reach the headers**. Of those, **~18 are freely usable at 3.3 V**, with only **5 permanently off-limits** (PSRAM + 1.8 V) — several of the rest are *reclaimable* if you give up the default function (USB, serial console, the onboard LED).

| | Count | GPIO | Why |
| --- | --- | --- | --- |
| ✅ **Safe (3.3 V)** | 18 | `1 2 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 21` | Free for general use (17/18 default to UART1 but are remappable) |
| ⚠️ **Input-only** | 4 | `39 40 41 42` | JTAG; fine for buttons/switches, can't drive outputs |
| 🔓 **Reclaimable** | 5 | `19 20` (USB) · `43 44` (UART0) · `38` (LED) | Reserved by default; free if you forgo that function |
| ⚠️ **Strapping** | 4 | `0 3 45 46` | Boot / JTAG / VDD_SPI — usable with care |
| ⛔ **PSRAM** | 3 | `35 36 37` | Octal PSRAM — unusable on R16V |
| ⛔ **1.8 V** | 2 | `47 48` | VDD_SPI domain — not 3.3 V, needs level shifting |

> Legend: ✅ use freely · ⚠️ with care · 🔓 reclaimable · ⛔ off-limits · Total: 36 GPIO

## Flashing this board

### PlatformIO (current)

Configured in `platformio.ini` at the repo root. Key settings and why they matter:

| Setting | Value | Reason |
| --- | --- | --- |
| `board_build.flash_mode` | `opi` | WROOM-2 has **Octal SPI flash** — `qio` causes boot failure |
| `board_build.arduino.memory_type` | `opi_opi` | Both flash AND PSRAM are OPI — `qio_opi` selects the wrong SDK |
| `board_build.psram_type` | `opi` | Explicit OPI PSRAM declaration |
| `board_build.partitions` | `default_32MB.csv` | Without this, only ~3 MB of the 32 MB flash is usable |
| `upload_speed` | `921600` | CH343 supports this; default 460800 is unnecessarily slow |

If upload fails, enter download mode: hold **BOOT**, tap **RESET**, release **BOOT**.

### Arduino IDE (legacy sketches only)

- Board: **ESP32S3 Dev Module**
- USB CDC On Boot: **Enabled**
- Flash Size: **32MB (256Mb)**
- PSRAM: **OPI PSRAM**

## Onboard RGB LED

- **WS2812**-type addressable LED on **GPIO38**. Used only for board bring-up testing (`sketches/01-hello-esp`, `sketches/02-led-serial`) — the instrument itself has no LED feature.
- **Color order is RGB on this board.** The built-in `rgbLedWrite()` (no library) transmits GRB, so red/green come out swapped — pass them swapped to compensate. (WS2812B is normally GRB; this unit's swap confirms it's RGB.)

## Notes & gotchas

- **Both flash and PSRAM are Octal SPI (OPI)** on the WROOM-2 module. This is different from cheaper S3 boards (WROOM-1) that use Quad SPI flash. PlatformIO must be configured for `opi_opi` or it will use the wrong SDK and fail to boot or corrupt PSRAM.
- **16 MB PSRAM at 80 MHz** — do not enable 120 MHz DDR mode. Espressif labels it experimental and it is temperature-sensitive: if ambient temperature shifts >20 °C after boot, random crashes occur. 80 MHz is stable.
- **16 MB PSRAM headroom** is generous for audio buffers, wavetables, and the loop station.
- See the **GPIO usage** table above for safe vs. off-limits pins. Two R16V-specific traps, both confirmed in the datasheet (footnotes b & c) and Waveshare docs: **GPIO 35/36/37** are tied to the octal PSRAM (unusable), and **GPIO 47/48** run at **1.8 V** (VDD_SPI domain) — don't drive 3.3 V parts from them without level shifting.
- **`-mfix-esp32-psram-cache-issue` does NOT apply here** — that flag is for the original ESP32 chip only. Never add it for S3.
- **USB-C does flashing + serial** via onboard CH343 (USB-UART) + CH334 (hub). CH343 may need a driver on some machines.
- Peripheral pin assignments (I2S, SPI, I2C) live in [../wiring.md](../wiring.md#connections).
