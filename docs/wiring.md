# Pocket Groovebox — Wiring

> 🚧 **Draft — provisional, pending bench tests.** Pin choices may change.

How the modules connect to the ESP32-S3. This doc is **only the connection structure** — each module's own specs and pinout live in [modules/](modules/). Pin choices follow the verified-safe 3.3 V GPIO pool documented in [modules/esp32-s3-devkit.md](modules/esp32-s3-devkit.md#gpio-usage-n32r16v).

## Connection overview

```mermaid
flowchart LR
    MCU["ESP32-S3"]
    MCU -->|"I2S — BCK 5 / LRCK 6 / DIN 7"| DAC["PCM5102 DAC"]
    MCU -->|"SPI — SCLK 12 / MOSI 11 / CS 10 / DC 14 / RST 15 / BLK 16"| TFT["ST7789 1.9in 170x320"]
    DAC -->|"line out"| HP["Headphone jack"]
    DAC -->|"line out"| AMP["PAM8403"]
    AMP --> SPK["Speaker"]
```

## Connections

### PCM5102 — I2S DAC

| ESP32-S3 | PCM5102 |
| --- | --- |
| GPIO5 | BCK (bit clock) |
| GPIO6 | LRCK / WS |
| GPIO7 | DIN (data) |
| 3V3 | VIN / VCC |
| GND | GND |

Module config: tie **SCK → GND** (internal PLL); set the FLT / DEMP / FMT / XSMT jumpers per the breakout.

### ST7789 — 1.9" 170×320 SPI display

| ESP32-S3 | ST7789 |
| --- | --- |
| GPIO12 | SCL / SCLK |
| GPIO11 | SDA / MOSI |
| GPIO10 | CS |
| GPIO14 | DC |
| GPIO15 | RST |
| GPIO16 | BLK (backlight) |
| 3V3 | VCC |
| GND | GND |

Write-only — no MISO. The 170×320 panel needs a driver offset (Arduino_GFX / TFT_eSPI).

### Still to assign

Keyboard (13 keys) and 2× EC11 encoders — pins TBD (I2C default 8/9 left free for an expander if needed).
