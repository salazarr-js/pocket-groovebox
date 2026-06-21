# SK6812 MINI-E — Addressable RGB LED

Addressable RGB LED with integrated controller IC — one data wire drives the full chain. Used for per-key lighting on the keyboard (active note, chord tones, sequencer steps).

| | |
| --- | --- |
| **Role** | Per-key RGB LED (keyboard backlight) |
| **Chip / variant** | SK6812MINI-E Rev.02 (Dongguan Opsco Optoelectronics) |
| **Interface** | Single-wire serial (800 kHz, unipolar RZ) |
| **Supply voltage** | 5 V (3.7–5.5 V range) |
| **Current per LED** | 12 mA per channel, 36 mA max (white), 60 mA absolute max |
| **Package** | 3.2 × 2.8 × 1.78 mm SMD, 4 pads, south-facing (MX switch compatible) |
| **Source** | AliExpress (LOAMLIN LED Store) |
| **Price** | ARS 12.257,98 (50 pcs) |
| **Quantity** | 50 (25 per key + 25 spares — on order) |

## Datasheets & references

- Datasheet Rev.02 (Adafruit mirror): https://cdn-shop.adafruit.com/product-files/4960/4960_SK6812MINI-E_REV02_EN.pdf
- Adafruit product page (4960): https://www.adafruit.com/product/4960

## Package & pin configuration

**Body:** 3.2 × 2.8 × 1.78 mm. The "-E" suffix means the pads extend horizontally out from the sides — designed to mount south-facing under MX-compatible switches so the LED shines up through the switch housing.

```
        ┌─────────────────┐
1 VDD ──┤                 ├── 4 DIN
2 DOUT──┤   SK6812MINI-E  ├── 3 GND
        └─────────────────┘
```

| Pin | Symbol | Function |
| --- | --- | --- |
| 1 | VDD | Power supply (5 V) |
| 2 | DOUT | Data output → DIN of next LED |
| 3 | GND | Ground |
| 4 | DIN | Data input from controller or previous LED |

**PCB footprint (recommended by datasheet):** two pad pairs on left/right sides; body sits flat on the board. 4-pad SMD, not through-hole.

## Electrical parameters

### Absolute maximum ratings (Ta = 25 °C)

| Parameter | Symbol | Range | Unit |
| --- | --- | --- | --- |
| Supply voltage | VDD | 3.7 – 5.5 | V |
| Logic input voltage | V_IN | −0.5 – VDD+0.5 | V |
| Working temperature | Topt | −40 – +85 | °C |
| Storage temperature | Tstg | −50 – +150 | °C |
| ESD (HBM) | V_ESD | 4000 | V |
| ESD (DM) | V_ESD | 200 | V |

### IC electrical parameters (TA = −20 – +70 °C, VDD = 4.5 – 5.5 V)

| Parameter | Symbol | Typical | Unit |
| --- | --- | --- | --- |
| Supply voltage | VDD | 5.2 | V |
| Logic HIGH threshold | VIH | 0.7 × VDD | V |
| Logic LOW threshold | VIL | 0.3 × VDD | V |
| PWM frequency | FPWM | 1.2 | kHz |
| Static current | IDD | 1 | mA |

> ⚠️ **At VDD = 5 V: VIH = 3.5 V minimum.** The ESP32-S3 GPIO outputs 3.3 V — this is **below the guaranteed HIGH threshold**. Direct connection may work in some units but is out-of-spec. A **74AHCT125 level shifter is required** for a reliable PCB design. See [Notes & gotchas](#notes--gotchas).

### Switching characteristics (VCC = 5 V, Ta = 25 °C)

| Parameter | Symbol | Typical | Max | Unit |
| --- | --- | --- | --- | --- |
| Data transmission speed | fDIN | 800 | — | kHz |
| DOUT propagation delay (rising) | TPLH | — | 500 | ns |
| DOUT propagation delay (falling) | TPHL | — | 500 | ns |
| Output rise time | Tr | 100 | — | ns |
| Output fall time | Tf | 100 | — | ns |

## Optical characteristics (12 mA version)

| Color | Wavelength | Luminance | Luminous flux |
| --- | --- | --- | --- |
| RED | 620 – 625 nm | 400 – 700 mcd | 1.0 – 2.0 lm |
| GREEN | 520 – 530 nm | 1000 – 1500 mcd | 3.0 – 4.0 lm |
| BLUE | 460 – 470 nm | 200 – 400 mcd | 0.5 – 1.0 lm |

**Viewing angle:** 120° (half-angle 60°)

## Data protocol

### Timing

| Symbol | Name | Min | Typical | Max | Unit |
| --- | --- | --- | --- | --- | --- |
| T | Code period | 1.20 | — | — | µs |
| T0H | `0` bit — high time | 0.2 | 0.32 | 0.4 | µs |
| T0L | `0` bit — low time | 0.8 | — | — | µs |
| T1H | `1` bit — high time | 0.58 | 0.64 | 1.0 | µs |
| T1L | `1` bit — low time | 0.2 | — | — | µs |
| Trst | Reset — low time | >80 | — | — | µs |

Protocol: **unipolar RZ (return-to-zero)**. Each bit starts HIGH; the width of the high pulse determines `0` or `1`. After all pixels are sent, hold DIN LOW for ≥ 80 µs to latch.

### Data structure — 24 bits per pixel, GRB order

```
G7 G6 G5 G4 G3 G2 G1 G0 | R7 R6 R5 R4 R3 R2 R1 R0 | B7 B6 B5 B4 B3 B2 B1 B0
```

> ⚠️ **Order is GRB, not RGB.** MSB first. This matches WS2812B — FastLED and the NeoPixel library handle it automatically if you specify `NEO_GRB` / `GRB`.

### Cascade wiring

```
ESP32-S3 GPIO18 ──[330–500Ω]──► LED1 DIN   LED1 DOUT ──► LED2 DIN   LED2 DOUT ──► ...
```

Each LED consumes its first 24 bits and forwards the rest downstream. The chain self-terminates.

## Power budget

| Scenario | Current per LED | 25 LEDs total |
| --- | --- | --- |
| All off | 1 mA (IC idle) | 25 mA |
| One channel full | 12 mA | 300 mA (worst case 1 color) |
| Full white (all channels) | 36 mA | 900 mA |
| Typical use (1/3 brightness) | ~12 mA | ~300 mA |

All current is drawn from **5 V** (VBUS). The 3.3 V rail does not supply the LEDs — only the data signal passes through the level shifter at 3.3 V → 5 V.

## Required support components

| Component | Value | Qty | Purpose |
| --- | --- | --- | --- |
| **74AHCT125** level shifter | SO14/DIP14 | 1 (✅ on hand) | 3.3 V → 5 V on DIN data line |
| Series resistor on DIN | 300–500 Ω | 1 | Damps signal reflections (datasheet recommends ~500 Ω) |
| Decoupling cap per LED | 100 nF ceramic | 25 | VDD–GND, placed as close to each LED as possible |
| Bulk cap at 5 V entry | 10 µF electrolytic | 1 | Absorbs current spikes when many LEDs switch together |

## Notes & gotchas

- **Level shifter is mandatory.** VIH = 3.5 V at VDD = 5 V; the ESP32-S3 outputs 3.3 V. The **74AHCT125** (SO14, one chip, one gate per channel) is the standard fix — fast propagation, 3.3 V input, 5 V output, cheap. Without it the signal may work in some conditions but will fail intermittently, especially at higher temperatures or with longer chains.
- **GRB, not RGB.** The data order is Green–Red–Blue, MSB first. FastLED: `CRGB` + `GRB`. NeoPixel: `NEO_GRB + NEO_KHZ800`. Getting this wrong shifts all colors one channel.
- **Reset ≥ 80 µs.** The ESP32-S3 RMT peripheral handles this automatically — just configure the idle level LOW and the reset duration. If driving manually, ensure the line stays LOW for at least 80 µs between frames or colors will latch mid-chain.
- **5 V power, not 3.3 V.** Connect VDD to the devkit's VBUS (5 V) pin, not 3.3 V. The LED won't illuminate correctly below ~3.7 V and spec guarantees require 5 V.
- **South-facing mount.** The "-E" pads extend sideways — the LED sits flat on the PCB, shining upward through the switch. Orient pad 1 (VDD) consistently across all footprints or the whole chain will be miswired.
- **ESP32-S3 RMT peripheral** is the right driver. GPIO18 → 74AHCT125 input → LED chain DIN. FastLED supports RMT natively on the S3 from v3.6+. Avoid bit-banging; the S3's Xtensa core can be interrupted mid-bit by Wi-Fi or FreeRTOS ticks and corrupt the signal.
- **ESD sensitivity.** The datasheet marks these as electrostatic sensitive — handle with care during assembly, especially before soldering.
- **MSL 5a.** Moisture sensitive level 5a — bake or use immediately after opening the reel packaging if stored more than 48 hours in ambient humidity.
- Connections to the ESP32-S3 → see [../wiring.md](../wiring.md).
